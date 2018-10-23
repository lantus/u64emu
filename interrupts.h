/**
 * TrueReality - N64/interrupts.h
 * Copyright (C) 1998, 1999 Niki W. Waibel
 *
 * This program is free software; you can redistribute it and/
 * or modify it under the terms of the GNU General Public Li-
 * cence as published by the Free Software Foundation; either
 * version 2 of the Licence, or any later version.
 *
 * This program is distributed in the hope that it will be use-
 * ful, but WITHOUT ANY WARRANTY; without even the implied war-
 * ranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public Licence for more details.
 *
 * You should have received a copy of the GNU General Public
 * Licence along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139,
 * USA.
 *
 * Information about me (the author):
 *   Niki W. Waibel, Reichenau 20, 6890 Lustenau, Austria - EUROPE
 *   niki.waibel@gmx.net
**/





#define __INTERRUPTS_H

#ifndef __EXCEPTIONS_H
        #include "exceptions.h"
#endif

#ifndef __TYPE_SIZES_H
        #include <type_sizes.h>
#endif





//#define VERTICAL_RETRACE_TIME 0x98968   /* I hope that this value ok - pur uses it. */
//#define VERTICAL_RETRACE_TIME 312500*2   /* I hope that this value ok - pur uses it. */
                               /* Maybe it should be 0x2faf08 (instr in 1/30sec by 93.75MHz) */
/* #define VERTICAL_RETRACE_TIME 0x2faf08 */
/* #define VERTICAL_RETRACE_TIME 0x17d784 */





#define MI_INTR_NO  0x00
#define MI_INTR_ALL 0x3f
#define MI_INTR_SP  0x01
#define MI_INTR_SI  0x02
#define MI_INTR_AI  0x04
#define MI_INTR_VI  0x08
#define MI_INTR_PI  0x10
#define MI_INTR_DP  0x20





extern DWORD next_vi_interrupt;





void rs4300i_InitInterrupts();

void rs4300i_MI_Intr_SP();
void rs4300i_MI_Intr_SI();
void rs4300i_MI_Intr_AI();
void rs4300i_MI_Intr_VI();
void rs4300i_MI_Intr_PI();
void rs4300i_MI_Intr_DP();
void rs4300i_CompareInterrupt();

void rs4300i_Check_MI_Interrupts();
void rs4300i_Check_CPU_Interrupts();
