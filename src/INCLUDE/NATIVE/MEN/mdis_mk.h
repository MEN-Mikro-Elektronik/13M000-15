/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: mdis_mk.h
 *
 *       Author: ub
 *        $Date: 2014/09/19 16:53:57 $
 *    $Revision: 1.3 $
 *
 *  Description: MDIS driver definitions 
 *
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mdis_mk.h,v $
 * Revision 1.3  2014/09/19 16:53:57  channoyer
 * R: Not possible to use MDIS over Qnet
 * M: Add node ID to struct MDIS_OPEN_DEVICE_DATA and MDIS_CLOSE_DEVICE_DATA
 *
 * Revision 1.2  2009/06/05 15:58:12  channoyer
 * R: No possibility to use MDIS kernel server for native driver
 * M: Add messages for native driver
 *
 * Revision 1.1  2004/03/24 11:35:07  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _MDIS_MK_H
#define _MDIS_MK_H

#ifdef __cplusplus
      extern "C" {
#endif

/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/

/*! structure passed in M_open() via devctl() _to_ MDIS kernel */
typedef struct {
	char  devName[40];           /*!< IN: MDIS device name */
    pid_t pid;                   /*!< IN: process id of calling thread */
	int   tid;                   /*!< IN: thread id of caller */
	int   nid;                   /*!< IN: node id of caller */
} MDIS_OPEN_DEVICE_DATA;

/*! structure passed in M_close() via devctl() _to_ MDIS kernel */
typedef struct {                 /* M_close */
    void* path;                  /*!< IN: ptr to device path */
    pid_t pid;                   /*!< IN: process id of calling thread */
	int   tid;                   /*!< IN: thread id of caller */
	int   nid;                   /*!< IN: node id of caller */
} MDIS_CLOSE_DEVICE_DATA;


/*! structure returned _from_ MDIS kernel on device open */
typedef struct {
    void     *path;              /*!< path (=handle) of newly opened device */
    int      mkPid;              /*!< process id of MDIS kernel */
    int      mkCid;              /*!< channel id of MDIS kernel thread */
} MDIS_OPEN_DEVICE_RETURN;


/*! structure passed via MsgSend() to MDIS kernel for M_read()/M_write() etc.*/
typedef struct {
    int32    path;         /*!< path of MDIS device as used by kernel */
    int32    id;           /*!< message ID */
    int32    code;         /*!< code (used for M_getstat/M_setstat) */
    int32    val;          /*!< value (used for M_write) */
    int32    length;       /*!< length of data block (used for M_getblock) */
} MDIS_MSGHEADER;

/*! structure returned _from_ MDIS kernel on native device open */
typedef struct {
    void    *physAddr;
	u_int32	 irqVector;
} MDIS_NATDEV_RETURN;

/*! info for the IRQ handling thread inside the MDIS kernel */
typedef struct {
    pthread_t   tid;                /*!< thread id */
    int         cid;                /*!< channel id */
    int         coid;               /*!< connection id */
} IRQ_THREAD;

typedef struct {
    void *path;
    int  coid;
    int  fd;
    int  nid;
} MDIS_PATH_QNX;

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/

/* command codes for devctl() */
#define MDIS_CMD_CODE       1
#define MDIS_DEVCTL_M_CLOSE __DIOT(_DCMD_MISC,  MDIS_CMD_CODE + 1, int)
#define MDIS_DEVCTL_M_OPEN  __DIOTF(_DCMD_MISC, MDIS_CMD_CODE + 2, \
                                    MDIS_OPEN_DEVICE_DATA)


/* ids for messages to MDIS kernel */
#define MDIS_MSGID_READ     0           /* M_read call */
#define MDIS_MSGID_WRITE    1           /* M_write */
#define MDIS_MSGID_GETBLOCK 2           /* M_getblock */
#define MDIS_MSGID_SETBLOCK 3           /* M_setblock */
#define MDIS_MSGID_GETSTAT  4           /* M_getstat */
#define MDIS_MSGID_SETSTAT  5           /* M_setstat */
#define MDIS_MSGID_NATGET   6           /* Get native info */
#define MDIS_MSGID_NATIRQ   7           /* Attach native IRQ */


/* general */
#define MDIS_DEV_NAME		"/dev/mdis"	/* global MDIS device name */
#define MDIS_DESCRIPTOR_DIR "/etc/mdis" /* where to find MDIS descriptors */

#define MDIS_SETMSG( h, _path, _id, _code, _val, _length )      { \
    (h)->path=(int32)_path; (h)->id=_id; (h)->code=_code; \
    (h)->val=_val; (h)->length=_length; }

 
/*-----------------------------------------+
|  GLOBALS                                 |
+-----------------------------------------*/
/* none */

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
/* none */

#ifdef __cplusplus
   }
#endif

#endif  /* _MDIS_MK_H  */

