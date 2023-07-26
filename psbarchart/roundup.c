/****************************************************************************

   Program:    AMPlot2
   File:       axes.c
   
   Version:    V1.0
   Date:       31.03.92
   Function:   Handle menu and gadget functions.
   
   Copyright:  SciTech Software 1992
   Author:     Dr. Andrew C. R. Martin
   Address:    SciTech Software
               23, Stag Leys,
               Ashtead,
               Surrey,
               KT21 2TD.
   Phone:      +44 (0372) 275775
   EMail:      UUCP: cbmuk!cbmuka!scitec!amartin
               JANET: andrew@uk.ac.ox.biop
               
*****************************************************************************

   This program is copyright. Any copying without the permission of
   SciTech Software is illegal.

*****************************************************************************

   Description:
   ============

*****************************************************************************

   Usage:
   ======

*****************************************************************************

   Revision History:
   =================

****************************************************************************/
#include <math.h>
#include "bioplib/MathType.h"

/***************************************************************************/
/*>RoundUp(REAL *value, REAL Size)
   -------------------------------
   Takes a value and a size (to gain order of magnitude of variable range)
   and rounds the value down to a clean value.
   06.04.92 Original
   09.10.97 Changed double to REAL
*/
int RoundUp(REAL *value,
            REAL Size)
{
   REAL     val,
            LastValue,
            Work,
            TenPower,
            Factor;
   int      Negative,
            LogValue,
            Mult;
   
   if(*value==0.0) return(0);
   
   val = *value;
   if(val < 0.0)
   {
      val  = -val;
      Negative = -1;
   }
   else
   {
      Negative = 1;
   }
   LogValue = log10(val);
   TenPower = (int)LogValue;
   if (LogValue < 0) TenPower--;
   Factor   = pow((REAL)10,(REAL)TenPower);
   Mult     = (int)(val/Factor);
   if((Negative == 1) && ((val/Factor) - Mult)) Mult++;
   val     = Mult * Factor;
   val     = val * Negative;
   LastValue = *value;

   while (((val - *value) > Size/3.0) && val != LastValue)
   {
      LastValue = val;
      Work = val - *value;
#ifdef DEBUG
      printf("val %g, Work %g\n",val,Work);
#endif
      if(Work < 0.0)
      {
         Work  = -Work;
         Negative = -1;
      }
      else
      {
         Negative = 1;
      }
      LogValue = log10(Work);
      TenPower = (int)LogValue;
      if (LogValue < 0) TenPower--;
      Factor   = pow((REAL)10,(REAL)TenPower);
      Mult     = (int)(Work/Factor);
      Work     = Mult * Factor;
      Work     = Work * Negative;

      val = val - Work;

      if(val < *value)
      {
         val = LastValue;
         break;
      }
   }
   *value = val;
   return(0);
}

