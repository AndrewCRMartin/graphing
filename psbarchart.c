/*************************************************************************

   Program:    psbarchart
   File:       psbarchart.c
   
   Version:    V1.1
   Date:       14.11.97
   Function:   Draw a simple barchart
   
   Copyright:  (c) Dr. Andrew C. R. Martin, UCL 1997
   Author:     Dr. Andrew C. R. Martin
   Address:    Biomolecular Structure & Modelling Unit,
               Department of Biochemistry & Molecular Biology,
               University College,
               Gower Street,
               London.
               WC1E 6BT.
   Phone:      (Home) +44 (0)1372 275775
               (Work) +44 (0)171 419 3890
   EMail:      INTERNET: martin@biochem.ucl.ac.uk
               
**************************************************************************

   This program is not in the public domain, but it may be copied
   according to the conditions laid out in the accompanying file
   COPYING.DOC

   The code may be modified as required, but any modifications must be
   documented so that the person responsible can be identified. If someone
   else breaks this code, I don't want to be blamed for code that does not
   work! 

   The code may not be sold commercially or included as part of a 
   commercial product except as described in the file COPYING.DOC.

   This program uses routines from AMPlot2 by permission of the author.
   These may not be used outside psbarchart without seeking
   permission from amartin@stagleys.demon.co.uk

**************************************************************************

   Description:
   ============
   A very simple program to generate a shaded barchart from a single
   set of numbers.

**************************************************************************

   Usage:
   ======
   Input is a file of number pairs where the first number is the
   x-value and the second is the bar height; the x-values should be
   evenly spaced as all except the values in the first and second
   row are ignored (the first row gives a starting point for the
   left hand end of the x-axis and the seond is used to calculate the
   gap for subsequent values)

   The file may contain records beginning with T, X and Y which are
   used to specify title, x and y axis labels respectively. The letter
   is followed by a space and the text of the title/label.

**************************************************************************

   Revision History:
   =================
   V1.0  09.10.97 Original
   V1.1  14.11.97 Fixed value used for calculation of Y-label offset

*************************************************************************/
/* Includes
*/
#include <stdio.h>
#include <string.h>
#include "bioplib/ps.h"
#include "bioplib/MathType.h"
#include "bioplib/SysDefs.h"
#include "bioplib/macros.h"
#include "bioplib/general.h"

/************************************************************************/
/* Defines and macros
*/
#define MAXBARS 1000
#define MAXBUFF 160
#define TICKSIZE 6
#define TFONTSIZE 20
#define AXLABFONTSIZE 14
#define LABELSIZE 11

/************************************************************************/
/* Globals
*/
char gTitle[MAXBUFF],
     gXLabel[MAXBUFF],
     gYLabel[MAXBUFF];

/************************************************************************/
/* Prototypes
*/
extern int RoundUp(REAL *val, REAL range);
int main(int argc, char **argv);
void Usage(void);
int ReadBars(int *bars, char *filename, REAL *zero, REAL *barwidth);
BOOL DrawBarchart(int *bars, char *filename, int nbars, REAL zero,
                  REAL barwidth);
void DrawAxes(int nbars, REAL zero, REAL barwidth, int ymax);
   

/************************************************************************/
/*>int main(int argc, char **argv)
   -------------------------------
   Main program for drawing a simple barchart

   08.10.97 Original   By: ACRM
*/
int main(int argc, char **argv)
{
   int  bars[MAXBARS], nbars;
   REAL barwidth,zero;
   
   if(argc != 3)
   {
      Usage();
      return(0);
   }

   if((nbars=ReadBars(bars, argv[1],&zero,&barwidth)))
      DrawBarchart(bars,argv[2],nbars,zero,barwidth);
   
   return(0);
}

/************************************************************************/
/*>void Usage(void)
   ----------------
   Prints a usage message

   09.10.97 Original   By: ACRM
*/
void Usage(void)
{
   fprintf(stderr,"\npsbarchart V1.1 (c) 1997, Dr.Andrew C.R. Martin, \
UCL\n");
   fprintf(stderr,"\nUsage: psbarchart input.dat output.ps\n");

   fprintf(stderr,"\nA very simple program to generate a shaded barchart \
from a single\n");
   fprintf(stderr,"set of number pairs.\n");

   fprintf(stderr,"\nInput is a file of number pairs where the first \
number is the\n");
   fprintf(stderr,"x-value and the second is the bar height; the \
x-values should be\n");
   fprintf(stderr,"evenly spaced as all except the values in the first \
and second\n");
   fprintf(stderr,"row are ignored (the first row gives a starting \
point for the\n");
   fprintf(stderr,"left hand end of the x-axis and the seond is used \
to calculate the\n");
   fprintf(stderr,"gap for subsequent values)\n");

   fprintf(stderr,"\nThe file may contain records beginning with T, X \
and Y which are\n");
   fprintf(stderr,"used to specify title, x and y axis labels \
respectively. The letter\n");
   fprintf(stderr,"is followed by a space and the text of the \
title/label.\n");

   fprintf(stderr,"\nThis program uses routines from AMPlot2 by \
permission of the author.\n");
   fprintf(stderr,"These may not be used outside psbarchart without \
seeking\n");
   fprintf(stderr,"permission from amartin@stagleys.demon.co.uk\n");
}

/************************************************************************/
/*>int ReadBars(int *bars, char *filename, REAL *zero, REAL *barwidth)
   -------------------------------------------------------------------
   Read the bars data file

   08.10.97 Original   By: ACRM
*/
int ReadBars(int *bars, char *filename, REAL *zero, REAL *barwidth)
{
   FILE *fp;
   char buffer[MAXBUFF];
   int  i = 0;
   REAL x;
   
   if((fp=fopen(filename,"r"))==NULL)
      return(0);
   gTitle[0]  = '\0';
   gXLabel[0] = '\0';
   gYLabel[0] = '\0';

   while(fgets(buffer,MAXBUFF,fp))
   {
      TERMINATE(buffer);
      switch(buffer[0])
      {
      case 'T':
         strcpy(gTitle, buffer+2);
         break;
      case 'X':
         strcpy(gXLabel, buffer+2);
         break;
      case 'Y':
         strcpy(gYLabel, buffer+2);
         break;
      default:
         sscanf(buffer,"%lf %d",&x,&(bars[i]));
         switch(i)
         {
         case 0:
            *zero = x;
            break;
         case 1:
            *barwidth = (x - *zero);
            break;
         default:
            break;
         }
         
         if(++i >= MAXBARS)
         {
            return(0);
         }
      }
   }

   *zero -= *barwidth;
   
   return(i);
}

   
/************************************************************************/
/*>BOOL DrawBarchart(int *bars, char *filename, int nbars, REAL zero,
                     REAL barwidth)
   ------------------------------------------------------------------
   Draws the actual barchart

   08.10.97 Original   By: ACRM
*/
BOOL DrawBarchart(int *bars, char *filename, int nbars, REAL zero,
                  REAL barwidth)
{
   int i, maxy,
       highbar = 0;
   REAL fmaxy;

   /* Find the highest bar                                              */
   for(i=0; i<nbars; i++)
   {
      if(bars[i] > highbar)
         highbar = bars[i];
   }
   fmaxy = highbar;
   RoundUp(&fmaxy, fmaxy);
   maxy = (int)fmaxy;
   if(maxy < 10)
      maxy++;
   
   PSxpicsize = (REAL)5.0;
   PSypicsize = (REAL)3.0;
   PSxoffset  = (REAL)1.0;
   PSyoffset  = (REAL)1.0;
   
   if(PSInit(filename, "PrimePlot", "Symbol"))
   {
      DrawAxes(nbars,zero,barwidth,maxy);
      for(i=0; i<nbars; i++)
      {
         REAL x1, x2, shrink;
         x1 = (REAL)i/(REAL)nbars;
         x2 = (REAL)(i+1)/(REAL)nbars;
         shrink = 0.1 * (x2 - x1);
         x1 += shrink;
         x2 -= shrink;

         fprintf(gPSFile,"0.5 setgray newpath\n");
         PSMove(x1, 0);
         PSDraw(x2, 0);
         PSDraw(x2, (REAL)bars[i]/(REAL)maxy);
         PSDraw(x1, (REAL)bars[i]/(REAL)maxy);
         fprintf(gPSFile,"closepath fill stroke\n");

         fprintf(gPSFile,"0.0 setgray newpath\n");
         PSMove(x1, 0);
         PSDraw(x2, 0);
         PSDraw(x2, (REAL)bars[i]/(REAL)maxy);
         PSDraw(x1, (REAL)bars[i]/(REAL)maxy);
         fprintf(gPSFile,"closepath stroke\n");
      }
      PSEnd();
   }
   else
   {
      return(FALSE);
   }

   return(TRUE);
}

/************************************************************************/
/*>void DrawAxes(int nbars, REAL zero, REAL barwidth, int ymax)
   ------------------------------------------------------------
   Draw the axes and labels for the barchart

   09.10.97 Original   By: ACRM
   14.11.97 Fixed values used for Y-label offset calculation
*/
void DrawAxes(int nbars, REAL zero, REAL barwidth, int ymax)
{
   char titlefont[80],
        valstr[16];
   int  i;
   REAL val, 
        ystep,
        reali;
   
   strcpy(titlefont,"Helvetica");
   
   /* TODO: Add tick marks and labels */
   /* draw axes                                                         */
   fprintf(gPSFile,"newpath\n");
   PSMove(0.0,0.0);
   PSDraw(1.0,0.0);
   PSDraw(1.0,1.0);
   PSDraw(0.0,1.0);
   fprintf(gPSFile,"closepath stroke\n");

   /* Display the labels                                                */
   if(gTitle[0])
   {
      PSFont(titlefont,(REAL)TFONTSIZE);
      PSCBText(0.5, 1.02, 0.0, gTitle);
   }
   if(gXLabel[0])
   {
      PSFont(titlefont,(REAL)AXLABFONTSIZE);
      PSCTText(0.5,0.0,-(LABELSIZE+TICKSIZE),gXLabel);
   }
   if(gYLabel[0])
   {
      ftostr(valstr, 4, (REAL)ymax, (ymax<2)?1:0);
      PSFont(titlefont,(REAL)AXLABFONTSIZE);
      PSVText(0.0, 0.5, -(TICKSIZE+5),
              gYLabel, titlefont, (REAL)AXLABFONTSIZE,
              valstr,  titlefont, (REAL)LABELSIZE);
   }
   
   /* X ticks                                                           */
   PSFont(titlefont,(REAL)LABELSIZE);
   for(i=-1; i<nbars; i++)
   {
      val = zero + ((1+i)*barwidth);
      ftostr(valstr, 4, val, (nbars*barwidth<10)?1:0);
      
      PSMove((REAL)(i+1)/(REAL)nbars, 0);
      fprintf(gPSFile,"0 %d rlineto\n",-TICKSIZE);
      PSStroke();
      PSCTText((REAL)(i+1)/(REAL)nbars, 0, -TICKSIZE, valstr);
   }

   /* Y ticks                                                           */
   PSFont(titlefont,(REAL)LABELSIZE);

   if(ymax <= (REAL)1.0)
   {
      ystep = (REAL)0.1;
   }
   else if(ymax <= (REAL)10.0)
   {
      ystep = (REAL)1.0;
   }
   else
   {
      ystep = (REAL)((int)(ymax/10));
   }
   
   for(reali=0.0; reali<=(REAL)ymax; reali+=ystep)
   {
      PSMove((REAL)0.0, reali/(REAL)ymax);
      fprintf(gPSFile,"%d 0 rlineto\n",-TICKSIZE);
      PSStroke();
      ftostr(valstr, 4, reali, (ymax<2)?1:0);
      PSROffText((REAL)0.0, reali/(REAL)ymax, -TICKSIZE, valstr);
   }
}

