/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  oss_ident.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2014/10/30 11:51:20 $
 *    $Revision: 1.21 $
 * 
 *	   \project  MDIS4QNX
 *  	 \brief  OSS library version info
 *      
 *    \switches  
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_ident.c,v $
 * Revision 1.21  2014/10/30 11:51:20  ch
 * (AUTOCI) Checkin due to new revision Release-1.6 of fileset QNX/LIBSRC/oss_src
 *
 * Revision 1.20  2014/10/29 16:18:16  ch
 * (AUTOCI) Checkin due to new revision Release-1.5 of fileset QNX/LIBSRC/oss_src
 *
 * Revision 1.19  2011/09/02 19:18:41  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.4 of fileset QNX/LIBSRC/oss_src
 *
 * Revision 1.18  2011/09/02 12:02:23  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.3 of fileset QNX/LIBSRC/oss_src
 *
 * Revision 1.17  2011/09/01 18:38:52  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.3 of fileset QNX/LIBSRC/oss_src
 *
 * Revision 1.16  2010/11/05 15:22:38  gvarlet
 * (AUTOCI) Checkin due to new revision Release-1.1 of fileset QNX/LIBSRC/oss_src
 *
 * Revision 1.15  2010/06/21 19:35:03  ch
 * (AUTOCI) Checkin due to new revision Release-1.2c of fileset QNX/LIBSRC/oss
 *
 * Revision 1.14  2009/06/07 16:51:34  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.2b of fileset QNX/LIBSRC/oss
 *
 * Revision 1.13  2008/11/12 17:36:32  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.2a of fileset QNX/LIBSRC/oss
 *
 * Revision 1.12  2008/11/10 15:49:42  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.2 of fileset QNX/LIBSRC/oss
 *
 * Revision 1.11  2008/11/06 20:03:35  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.1 of fileset QNX/LIBSRC/oss
 *
 * Revision 1.10  2008/09/26 16:30:06  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.0e of fileset QNX/LIBSRC/oss
 *
 * Revision 1.9  2008/06/26 18:39:30  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.0d of fileset QNX/LIBSRC/oss
 *
 * Revision 1.8  2008/03/27 11:00:52  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.0c of fileset QNX/LIBSRC/oss
 *
 * Revision 1.7  2008/03/26 10:44:02  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.0c of fileset QNX/LIBSRC/oss
 *
 * Revision 1.6  2004/09/17 08:56:50  kp
 * (AUTOCI) Checkin due to new revision Release-1.0b of fileset QNX/LIBSRC/oss
 *
 * Revision 1.5  2004/05/19 10:49:31  kp
 * (AUTOCI) Checkin due to new revision Release-1.0a of fileset QNX/LIBSRC/oss
 *
 * Revision 1.4  2004/03/25 08:32:47  ub
 * (AUTOCI) Checkin due to new revision Release-1.0 of fileset QNX/LIBSRC/oss
 *
 * Revision 1.3  2004/03/24 09:43:48  ub
 * (AUTOCI) Checkin due to new revision Release-1.0-Beta1 of fileset QNX/LIBSRC/oss
 *
 * Revision 1.2  2004/03/19 14:59:25  ub
 * adapted for MDIS
 *
 * Revision 1.1  2003/08/04 14:01:47  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char *IdentString="OSS - MEN QNX Operating System Services: %FSREV QNX/LIBSRC/oss_src Release-1.6 2014-10-30%";

char* OSS_Ident( void )
{
    return( (char*)IdentString );
}

