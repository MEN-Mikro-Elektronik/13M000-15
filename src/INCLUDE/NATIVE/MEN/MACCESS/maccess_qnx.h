/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: maccess_qnx.h
 *
 *       Author: ub
 *        $Date: 2010/11/08 18:12:45 $
 *    $Revision: 1.2 $
 *
 *  Description: MACCESS macros for QNX
 *
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: maccess_qnx.h,v $
 * Revision 1.2  2010/11/08 18:12:45  gvarlet
 * R: Wrong swapping in little endian mode IO mapped accesses.
 * M: define a case for little endian and big endian, byteswapped or not
 *
 * Revision 1.1  2004/03/24 11:35:06  ub
 * Initial Revision
 *

 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _MACCESS_QNX_H
#define _MACCESS_QNX_H

#include <hw/inout.h>

typedef unsigned long MACCESS;         /* access pointer */

#define MACCESS_CLONE(ma_src,ma_dst,offs)	ma_dst=(ma_src)+((offs))

#define _MAC_INB_	in8
#define _MAC_OUTB_	out8

#if ((defined (_BIG_ENDIAN_) && !(defined (MAC_BYTESWAP))) || \
      (defined (_LITTLE_ENDIAN_) && defined (MAC_BYTESWAP)) )
# define _MAC_INW_	inbe16
# define _MAC_INL_	inbe32
# define _MAC_OUTW_	outbe16
# define _MAC_OUTL_ outbe32
#else
# define _MAC_INW_	inle16
# define _MAC_INL_	inle32
# define _MAC_OUTW_	outle16
# define _MAC_OUTL_ outle32
#endif

# define _MAC_OFF_	(0) /* offset for I/O space (ok???) */


#define MREAD_D8(ma,offs)		\
            _MAC_INB_(((MACCESS)(ma)+(offs)+_MAC_OFF_))
#define MREAD_D16(ma,offs)		\
            _MAC_INW_(((MACCESS)(ma)+(offs)+_MAC_OFF_))
#define MREAD_D32(ma,offs)		\
             _MAC_INL_(((MACCESS)(ma)+(offs)+_MAC_OFF_))

#define MWRITE_D8(ma,offs,val)	\
            _MAC_OUTB_(((MACCESS)(ma)+(offs)+_MAC_OFF_),val)
#define MWRITE_D16(ma,offs,val) \
            _MAC_OUTW_(((MACCESS)(ma)+(offs)+_MAC_OFF_),val)
#define MWRITE_D32(ma,offs,val) \
            _MAC_OUTL_(((MACCESS)(ma)+(offs)+_MAC_OFF_),val)

#define MSETMASK_D8(ma,offs,mask) \
            MWRITE_D8(ma,offs,MREAD_D8(ma,offs)|(mask))
#define MSETMASK_D16(ma,offs,mask) \
            MWRITE_D16(ma,offs,MREAD_D16(ma,offs)|(mask))
#define MSETMASK_D32(ma,offs,mask) \
            MWRITE_D32(ma,offs,MREAD_D32(ma,offs)|(mask))

#define MCLRMASK_D8(ma,offs,mask) \
            MWRITE_D8(ma,offs,MREAD_D8(ma,offs)&~(mask))
#define MCLRMASK_D16(ma,offs,mask) \
            MWRITE_D16(ma,offs,MREAD_D16(ma,offs)&~(mask))
#define MCLRMASK_D32(ma,offs,mask) \
            MWRITE_D32(ma,offs,MREAD_D32(ma,offs)&~(mask))


#define MBLOCK_READ_D8(ma,offs,size,dst) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)dst; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
			  *mem++ = _MAC_INB_( hw );\
              hw++;	\
          }             \
        }

#define MBLOCK_READ_D16(ma,offs,size,dst) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)dst; \
          unsigned long hw =(MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
			  *mem++ = _MAC_INW_( hw );\
              hw += 2;	\
          }             \
        }

#define MBLOCK_READ_D32(ma,offs,size,dst) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)dst; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
			  *mem++ = _MAC_INL_( hw );\
              hw += 4;	\
          }             \
        }

#define MBLOCK_WRITE_D8(ma,offs,size,src) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)src; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
              _MAC_OUTB_(hw,*mem);\
              mem++;\
              hw++;	\
          }             \
        }

#define MBLOCK_WRITE_D16(ma,offs,size,src) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)src; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
              _MAC_OUTW_(hw,*mem);\
              mem++; \
              hw += 2;	\
          }             \
        }

#define MBLOCK_WRITE_D32(ma,offs,size,src) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)src; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
              _MAC_OUTL_(hw,*mem);\
              mem++; \
              hw += 4;	\
          }             \
        }

#define MFIFO_READ_D8(ma,offs,size,dst) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)dst; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
			  *mem++ = _MAC_INB_(hw);\
          }             \
        }

#define MFIFO_READ_D16(ma,offs,size,dst) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)dst; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
			  *mem++ = _MAC_INW_(hw);\
          }             \
        }

#define MFIFO_READ_D32(ma,offs,size,dst) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)dst; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
			  *mem++ = _MAC_INL_(hw);\
          }             \
        }

#define MFIFO_WRITE_D8(ma,offs,size,src) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)src; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
              _MAC_OUTB_(hw,*mem);\
              mem++;\
          }             \
        }

#define MFIFO_WRITE_D16(ma,offs,size,src) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)src; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
              _MAC_OUTW_(hw,*mem);\
              mem++; \
          }             \
        }

#define MFIFO_WRITE_D32(ma,offs,size,src) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)src; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
              _MAC_OUTL_(hw,*mem);\
              mem++; \
          }             \
        }


#define	MBLOCK_SET_D8(ma,offs,size,val)		\
		{ 	u_int32 i;					\
		for( i=0; i<(size); i++ )		\
			MWRITE_D8(ma,(offs)+i,val);		\
		}

#define MBLOCK_SET_D16(ma,offs,size,val)	\
		{ 	u_int32 i;	\
		for( i=0; i<(size); i+=2 )	\
			MWRITE_D16(ma,(offs)+i,val);			\
		}

#define MBLOCK_SET_D32(ma,offs,size,val)	\
		{ 	u_int32 i;		\
		for( i=0; i<(size); i+=4 )			\
			MWRITE_D32(ma,(offs)+i,val); 		\
		}

#endif /* _MACCESS_QNX_H */
