/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  bbis_kernel.c
 *
 *      \author  ulrich.bogensperger@men.de
 *        $Date: 2009/07/09 10:47:44 $
 *    $Revision: 1.3 $
 *
 *	   \project  MDIS4QNX
 *  	 \brief  BBIS kernel library
 *
 *    \switches
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: bbis_kernel.c,v $
 * Revision 1.3  2009/07/09 10:47:44  channoyer
 * R: QNX 6.4 warning: pointer targets differ in signedness
 * M: change variable type
 *
 * Revision 1.2  2008/09/12 16:09:25  gvarlet
 * R: When built in DBG mode, try to find libraies without "_dbg" suffix
 * M: Now reach libraries with "_dbg" suffix when build in DBG mode
 *
 * Revision 1.1  2004/03/23 17:58:42  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2004 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

/*-----------------------------------------+
|  INCLUDES                                |
+------------------------------------------*/
#include "bk_intern.h"


/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define BK_DRV_PREFIX     "libmen_bb_"
#ifdef DBG
	#define BK_DRV_SUFFIX     "_dbg.so"
#else
	#define BK_DRV_SUFFIX     ".so"
#endif

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
u_int32 		    bk_dbglevel=OSS_DBG_DEFAULT;  /* debug level */
OSS_HANDLE          *G_bkOsh;           /* OSS handle */
DBG_HANDLE          *G_bkDbh;           /* DEBUG handle */
OSS_SEM_HANDLE      *G_bkLockSem;       /* global semaphore for BBIS kernel */
OSS_DL_LIST         G_bkDrvList;        /* list of reg. BBIS LL drivers */
OSS_DL_LIST         G_bkDevList;        /* list of devices */


/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
static int32 OpenDevice( DESC_SPEC *bbDesc, BK_DEV *node );
static BK_DRV *FindDrvByName( char *name );
static void strtolower( char *s );
static BK_DRV* addDriver( char* drvName,
                          void* drvHandle,
                          void (*getEntry)(BBIS_ENTRY *) );
static int drvInUse( char *name );



/**********************************************************************/
/** Initialize library's  global variables
 *
 *  \return 0 on success or c ERR_OSS_xxx error code on error
 *
 *  \sa bbis_open
 */

int bbis_init( void )
{
	int error = ERR_SUCCESS;

    DBGWRT_1((DBH, "bbis_init\n"));

    /* init OSS */
    if( OSS_Init( "BBIS_KERNEL", &OSH, NULL ) ) {
        error = ERR_OSS_MEM_ALLOC;
        goto CLEANUP;
    }

    /* create global semaphore */
    if( OSS_SemCreate( OSH, OSS_SEM_BIN, 1, &G_bkLockSem ) ) {
        error = ERR_OSS_SEM_CREATE;
        goto CLEANUP;
    }

    OSS_DL_NewList( &G_bkDrvList );
    OSS_DL_NewList( &G_bkDevList );

    return( error );

 CLEANUP:
	if( OSH )
		OSS_Exit( &OSH );

	return error;
}


/**********************************************************************/
/** Open a BBIS low-level device
 *
 * Initializes global variables on first call.
 *
 *  \param  devName    \IN   name of device to be opened
 *  \param  bbDesc     \IN   board descriptor data
 *  \param  bbHdlP     \OUT  base board handle
 *  \param  bbEntries  \OUT  table of board driver functions
 *
 *  \return 0 on success or c ERR_OSS_xxx error code on error
 *
 * \sa bbis_close
 */

int bbis_open(
	char *devName,
	DESC_SPEC *bbDesc,
	BBIS_HANDLE **bbHdlP,
    BBIS_ENTRY *bbEntries)
{
	BK_DEV *node=NULL;
	int error = ERR_SUCCESS;

    DBGWRT_1((DBH,"bbis_open\n"));

	/* init BBIS handle using information from descriptor */
	*bbHdlP = NULL;

	/*---- lock ----*/
	BK_LOCK( error );
	if( error ) return error;

	/* check given name */
	if( strlen( devName ) > BK_MAX_DEVNAME ) {
		error = ERR_BBIS_ILL_PARAM;
		goto ERREXIT;
	}


	/*---- search device in list ----*/
	for( node=(BK_DEV *)G_bkDevList.head;
		 node->node.next;
		 node = (BK_DEV *)node->node.next ){

		if( strcmp(node->devName, devName ) == 0 ){
			/* found node */
			DBGWRT_2((DBH,"%s found\n", devName));
			break;
		}
	}

	if( node->node.next == NULL ){
		/*
		 * New device
		 */

		/* alloc memory for node */
		node = malloc( sizeof(*node) );
		if( node == NULL ){
			error = ERR_OSS_MEM_ALLOC;
			goto ERREXIT;
		}

		memset( node, 0, sizeof(*node));
		strcpy( node->devName, devName );
		node->useCount = 0;

		/*--- Try to initialize the device ---*/
		if( (error = OpenDevice( bbDesc, node )) )
			goto ERREXIT;

		/* add to list */
		OSS_DL_AddTail( &G_bkDevList, &(node->node) );
	}


    DBGWRT_3((DBH, "bbis_open: device sucessfully added\n"));

	node->useCount++;
	*bbHdlP = node->bb;					/* return board handle */
	node->drv->getEntry( bbEntries ); 	/* get board function entry points */
	goto EXIT;

 ERREXIT:
	if( node != NULL ) free(node);
 EXIT:
	DBGWRT_1((DBH,"bbis_open exit: %s error=%d (0x%x)\n", devName, error, error ));

	/*---- unlock ----*/
	BK_UNLOCK();
	return error;

}


/**********************************************************************/
/** Do the first open on the BBIS device
 *
 *  \param bbDesc      \IN   board descriptor data
 *	\param node        \IN   device node
 *	\param node->bb    \OUT  device handle
 *	\param node->drv   \OUT  ptr to driver structure
 *
 *  \return 0 on success or c ERR_OSS_xxx error code on error
 */
static int32 OpenDevice( DESC_SPEC *bbDesc, BK_DEV *node )
{
	int32       error;
	u_int32     strLen, value;
	char        drvName[BK_MAX_DRVNAME+1];
	DESC_HANDLE *descHdl=NULL;
    void        *drvHdl = NULL;
    BK_DRV      *drvNode;
	BBIS_ENTRY  ent;
	BBIS_HANDLE *bb = NULL;

    DBGWRT_1((DBH, "BK - OpenDevice: name=%s\n", drvName )); 

    error = ERR_SUCCESS;

    /*---- init OSS and DESC ----*/
	if( (error = DESC_Init( bbDesc, OSH, &descHdl ))){
		DBGWRT_ERR((DBH,"*** BBIS:OpenDevice: can't init dev desc "
					"err=0x%lx\n", error ));
		goto ERREXIT;
	}

	if( (error = OSS_Init( node->devName, &node->osh, NULL ))) {
		DBGWRT_ERR((DBH,"*** BBIS:OpenDevice: can't init OSS "
					"err=0x%lx\n", error ));
		goto ERREXIT;
    }

	/*---- get debug levels ----*/
	if ((error = DESC_GetUInt32(descHdl, OSS_DBG_DEFAULT, &value,
								"DEBUG_LEVEL_OSS")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		goto ERREXIT;

	OSS_DbgLevelSet(node->osh, value);

	if ((error = DESC_GetUInt32(descHdl, OSS_DBG_DEFAULT, &value,
								"DEBUG_LEVEL_DESC")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		goto ERREXIT;

	DESC_DbgLevelSet(descHdl, value);

	if ((error = DESC_GetUInt32(descHdl, OSS_DBG_DEFAULT, &DBG_MYLEVEL,
								"DEBUG_LEVEL_BK")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		goto ERREXIT;


	/*--- get name of BBIS driver ---*/
	strcpy( drvName, BK_DRV_PREFIX );  /* preset module name prefix */
	strLen = BK_MAX_DRVNAME - strlen(drvName) - strlen(BK_DRV_SUFFIX);

	if( (error = DESC_GetString( descHdl, "", &drvName[strlen(drvName)],
								 &strLen, "HW_TYPE"))){
		DBGWRT_ERR((DBH,"*** BBIS:OpenDevice: can't find HW_TYPE "
					"err=0x%lx\n", error ));
		goto ERREXIT;
	}
    strcat( drvName, BK_DRV_SUFFIX );
	strtolower( drvName );
	DBGWRT_2((DBH," driver filename=%s\n", drvName ));


	/*--- get debug level ---*/
	if( (error = DESC_GetUInt32( descHdl, OSS_DBG_DEFAULT,
								 &DBG_MYLEVEL,"DEBUG_LEVEL_BK" )) &&
		error != ERR_DESC_KEY_NOTFOUND ){
		DBGWRT_ERR((DBH,"*** BBIS:OpenDevice: can't find DEBUG_LEVEL_BK "
					"err=0x%lx\n", error ));
		goto ERREXIT;
	}

	DESC_Exit( &descHdl );		/* descriptor handle no longer required */


    /*--- check if driver already loaded ---*/
	if( (drvNode = FindDrvByName( drvName )) == NULL ) {
        void (*getEntry)(BBIS_ENTRY *);

        DBGWRT_2((DBH, "loading driver %s\n", drvName ));


        /*--- load shared library ---*/
        if( (drvHdl = dlopen( drvName, RTLD_NOW | RTLD_LOCAL | RTLD_WORLD ))
            == NULL ) {
            error = ERR_BK_NO_LLDRV;
            DBGWRT_ERR((DBH,"*** BBIS:OpenDevice: can't find driver %s\n",
                        drvName ));
            goto ERREXIT;
        }

        /*--- get address of getEntry routine ---*/
        if( (getEntry = dlsym( drvHdl, "BBIS_GetEntry" )) == NULL ) {
            error = ERR_BK_NO_LLDRV;
            DBGWRT_ERR((DBH,"*** BBIS:OpenDevice: can't find getEntry routine "
                        "in %s\n", drvName ));
            goto ERREXIT;
        }


        /*--- add driver to driver list ---*/
        if(( (drvNode = addDriver( drvName, drvHdl, getEntry )) == NULL )) {
            error = ERR_OSS_MEM_ALLOC;
            goto ERREXIT;
        }
    }

    node->drv = drvNode;


    /*--- get driver's function entry points ---*/
    drvNode->getEntry( &ent );

    /*--- init driver ---*/
    if( (error = ent.init( node->osh, bbDesc, &bb )) ) {
        DBGWRT_ERR((DBH,"*** BBIS:OpenDevice: %s: init failed\n",
                    drvName ));
        goto ERREXIT;
    }

    node->bb = bb;
    node->drv = drvNode;

    /*--- init board ---*/
    if( (error = ent.brdInit( bb )) ) {
        DBGWRT_ERR((DBH,"*** BBIS:OpenDevice: %s: brdInit failed\n",
                    drvName ));
        goto ERREXIT;
    }

    goto EXIT;

 ERREXIT:
    DBGWRT_ERR((DBH, "OpenDevice: error 0x%x\n", error ));
	if( node->osh ) OSS_Exit( &node->osh );
	if( descHdl ) DESC_Exit( &descHdl );
    /* 
     * if the board init fails the board driver frees its allocated resources,
     * so we don't have to call ent.exit() here
     */
 EXIT:
	return error;
}

/****************************** bbis_close **********************************/
/** Close a BBIS device
 *
 *	The last close deinitializes the base board
 *
 *  \param devName     \IN   device name of board (e.g. D201_1)
 *
 *  \return 0 on success or c ERR_OSS_xxx error code on error
 */
int bbis_close(char *devName)
{
	int error = ERR_SUCCESS;
	BK_DEV *node=(BK_DEV *)G_bkDevList.head;

	DBGWRT_1((DBH,"bbis_close: %s\n", devName ));

	BK_LOCK(error);

	if( error ) return error;

	for( node=(BK_DEV *)G_bkDevList.head;
		 node->node.next;
		 node = (BK_DEV *)node->node.next ){

		if( strcmp(node->devName, devName ) == 0 ){
			/* found node */
			DBGWRT_2((DBH," found node\n"));
			break;
		}
	}

	if( node->node.next == NULL ){
		error = ERR_BK_ILL_PARAM;		/* device not open */
		goto ERREXIT;
	}

	if( --node->useCount == 0 ) {
		/*
		 * Final termination call. Deinit device
		 */
		BBIS_ENTRY ent;
        BK_DRV *drv;
        char drvName[BK_MAX_DRVNAME+1];

        /* save driver name & pointer */
        strncpy( drvName, node->drv->drvName, BK_MAX_DRVNAME+1 );
        drv = node->drv;

		node->drv->getEntry( &ent );     /* get board function entry points */
		error = ent.brdExit( node->bb ); /* deinit board */

		if( error == 0 ){
			error = ent.exit( &node->bb ); /* destroy handle */
		}

        /* (ignore errors from board exit routine) */

		OSS_DL_Remove( &node->node ); 	/* detach device from list */
		OSS_Exit( &node->osh );			/* exit OSS */
		free( node );

        /* remove driver if no longer used */
        if( drvInUse( drvName ) == 0 ) {
            DBGWRT_3((DBH, "removing driver %s\n", drvName ));
            dlclose( drv->drvHandle );
            OSS_DL_Remove( &drv->node );
            free( drv );
        }
	}

 ERREXIT:
	DBGWRT_1((DBH,"bbis_close ex: %s error=%d (0x%x)\n",
              devName, error, error));
	BK_UNLOCK();

	return( error );
}


/**********************************************************************/
/** Search for a driver in BBIS driver list
 *
 *  \param  name    \IN   name to look for
 *
 *  \return  ptr to driver struct or NULL if not found
 */

static BK_DRV *FindDrvByName( char *name )
{
	BK_DRV *node;

	for( node=(BK_DRV *)G_bkDrvList.head;
		 node->node.next;
		 node = (BK_DRV *)node->node.next ){

		if( strcmp(node->drvName, name ) == 0 )
			break;
	}

	if( node->node.next == NULL )
		node=NULL;

	return( node );
}

static void strtolower( char *s )
{
	while( *s ){
		if( *s >= 'A' && *s <= 'Z' )
			*s += 'a' - 'A';
		s++;
	}
}

static BK_DRV*
addDriver( char* drvName, void* drvHandle, void (*getEntry)(BBIS_ENTRY *) )
{
    BK_DRV *node;

    /* create entry */
    if(( node = (BK_DRV*)malloc( sizeof( BK_DRV ) ) ) == NULL )
       return( NULL );

    strncpy( node->drvName, drvName, BK_MAX_DRVNAME );
    node->drvHandle = drvHandle;
    node->getEntry  = getEntry;

    OSS_DL_AddTail( &G_bkDrvList, &(node->node) );

    return( node );
}

/**********************************************************************/
/** Search for driver in device list
 *
 * Determine if driver \c name is used by any device.
 *
 *  \param  name  \IN   Name of driver to search for
 *
 *  \return Pointer to BK_DEV struct of driver; NULL if none found
 */
static int
drvInUse( char *name )
{
    BK_DEV *node = (BK_DEV*)G_bkDevList.head;

    for( node = (BK_DEV*)G_bkDevList.head;
         node->node.next;
         node = (BK_DEV*)node->node.next ) {

        if( node->drv )
            if( strcmp( node->drv->drvName, name ) == 0 )
                break;
    }

    if( node->node.next == NULL )
        return( 0 );

    return( 1 );
}
