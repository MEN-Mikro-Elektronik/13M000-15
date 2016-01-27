/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  oss_dl_list.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2003/08/04 14:01:35 $
 *    $Revision: 1.1 $
 * 
 *	   \project  MDIS4Linux
 *  	 \brief  double linked list routines
 *      
 *    \switches  -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_dl_list.c,v $
 * Revision 1.1  2003/08/04 14:01:35  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/

#include "oss_intern.h"

/**********************************************************************/
/** Initialize list header
 * \copydoc oss_specification.c::OSS_DL_NewList()
 */
OSS_DL_LIST *OSS_DL_NewList( OSS_DL_LIST *l )
{
	l->head = (OSS_DL_NODE *)&l->tail;
	l->tailpred = (OSS_DL_NODE *)&l->head;
	l->tail = (OSS_DL_NODE *)NULL;
	return l;
}

/**********************************************************************/
/** Remove a node from a list
 * \copydoc oss_specification.c::OSS_DL_Remove()
 */
OSS_DL_NODE *OSS_DL_Remove( OSS_DL_NODE *n )
{
	n->next->prev = n->prev;
	n->prev->next = n->next;
	return n;
}

/**********************************************************************/
/** Remove a node from the head of the list
 * \copydoc oss_specification.c::OSS_DL_RemHead()
 */
OSS_DL_NODE *OSS_DL_RemHead( OSS_DL_LIST *l )
{
	OSS_DL_NODE *n;
	if( l->head->next == NULL ) return NULL; /* list empty */

	n = l->head;
	l->head = n->next;
	n->next->prev = (OSS_DL_NODE *)&(l->head);

	return n;
}

/**********************************************************************/
/** Add a node at tail of list
 * \copydoc oss_specification.c::OSS_DL_AddTail()
 */
OSS_DL_NODE *OSS_DL_AddTail( OSS_DL_LIST *l, OSS_DL_NODE *n )
{
	n->prev = l->tailpred;
	l->tailpred = n;
	n->next = n->prev->next;
	n->prev->next = n;
	return n;
}
	
