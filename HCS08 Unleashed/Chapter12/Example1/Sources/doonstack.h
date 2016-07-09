/**********************************************************************/
/*                                                                    */
/* Project Name:  doonstack.h                                         */
/* Last modified: 04/11/2004                                          */
/* By:            r60817                                              */
/*                                                                    */
/*                                                                    */
/**********************************************************************/
/*                                                                    */
/* Description: MC9S08GB60_FLASH_DOONSTACK - demo                     */
/*                                                                    */
/*                                                                    */
/* Documentation: MC9S08GB60/D Rev. 2.2                               */
/*                HCS08RMv1/D  Rev. 1(4.8FLASH Application Examples)  */
/*                                                                    */
/* This software is classified as Engineering Sample Software.        */
/*                                                                    */
/**********************************************************************/
/*                                                                    */
/* Services performed by FREESCALE in this matter are performed AS IS */
/* and without any warranty. CUSTOMER retains the final decision      */
/* relative to the total design and functionality of the end product. */ 
/* FREESCALE neither guarantees nor will be held liable by CUSTOMER   */
/* for the success of this project. FREESCALE DISCLAIMS ALL           */
/* WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY INCLUDING, BUT NOT     */
/* LIMITED TO, IMPLIED WARRANTY OF MERCHANTABILITY OR  FITNESS FOR A  */
/* PARTICULAR PURPOSE ON ANY HARDWARE, SOFTWARE ORE ADVISE SUPPLIED   */
/* TO THE PROJECT BY FREESCALE, AND OR NAY PRODUCT RESULTING FROM     */
/* FREESCALE SERVICES . IN NO EVENT SHALL FREESCALE BE LIABLE FOR     */
/* INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT. */
/*                                                                    */
/* CUSTOMER agrees to hold FREESCALE harmless against any and all     */
/* claims demands or actions by anyone on account of any damage, or   */
/* injury, whether commercial, contractual, or tortuous, rising       */
/* directly or indirectly as a result of the advise or assistance     */
/* supplied CUSTOMER in connection with product, services or goods    */
/* supplied under this Agreement.                                     */
/*                                                                    */
/**********************************************************************/

/*
- this file API between main.c and doonstack.asm
*/

#ifndef _doonstack
#define _doonstack

#ifdef __cplusplus
	extern "C" { /* our assembly functions have C calling convention */
#endif

void DoOnStack(void); /* prototype for DoOnStack routine */
void FlashErase(unsigned char *); /* prototype for FlashErase routine */
                               /* Page Erase command */
void FlashProg(unsigned char *, unsigned char); /* prototype for FlashProg routine */
                                             /* Byte Program command */

#ifdef __cplusplus
	}
#endif
  
#endif /* _doonstack */