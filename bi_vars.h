
/********************************************
bi_vars.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/


/* $Log:	bi_vars.h,v $
 * Revision 3.4.1.1  91/09/14  17:22:47  brennan
 * VERSION 1.0
 * 
 * Revision 3.4  91/08/13  06:50:54  brennan
 * VERSION .9994
 * 
 * Revision 3.3  91/06/29  09:46:55  brennan
 * Only track NR if needed
 * 
 * Revision 3.2  91/06/28  04:16:10  brennan
 * VERSION 0.999
 * 
 * Revision 3.1  91/06/07  10:26:58  brennan
 * VERSION 0.995
 * 
 * Revision 2.9  91/05/29  14:24:55  brennan
 * -V option for version
 * 
 * Revision 2.8  91/05/28  09:04:25  brennan
 * removed main_buff
 * 
 * Revision 2.7  91/05/22  07:42:19  brennan
 * dependency cleanup for DOS
 * 
 * Revision 2.6  91/05/16  12:19:29  brennan
 * cleanup of machine dependencies
 * 
 * Revision 2.5  91/05/15  12:07:30  brennan
 * dval hash table for arrays
 * 
 * Revision 2.4  91/05/08  13:40:23  brennan
 * version string 0.981
 * 
 * Revision 2.3  91/04/29  07:50:30  brennan
 * version string 0.980
 * 
 * Revision 2.2  91/04/24  13:51:36  brennan
 * update version string
 * 
 * Revision 2.1  91/04/08  08:26:30  brennan
 * VERSION 0.97
 * 
*/


/* bi_vars.h */

#ifndef  BI_VARS_H
#define  BI_VARS_H  1


/* builtin variables NF, RS, FS, OFMT are stored
   internally in field[], so side effects of assignment can
   be handled 
*/

#define  NR	   0  /* NR and FNR must be next to each other */
#define  FNR	   1
#define  ARGC      2
#define  FILENAME  3
#define  OFS       4
#define  ORS       5
#define  RLENGTH   6
#define  RSTART    7
#define  SUBSEP    8
#define  NUM_BI_VAR  9

extern CELL bi_vars[NUM_BI_VAR] ;


#endif
