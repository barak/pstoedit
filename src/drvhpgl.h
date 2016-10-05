#ifndef __drvHPGL_h
#define __drvHPGL_h

/* 
   drvhpgl.h : This file is part of pstoedit
   Class declaration for hpgl output driver with no additional attributes
   and methods (minimal interface)

   Copyright (C) 1993,1994,1995,1996,1997,1998 Peter Katzmann p.katzmann@thiesen.com 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "drvbase.h"


class drvHPGL : public drvbase {
   
   public:
   
	derivedConstructor(drvHPGL);
 
   ~drvHPGL(); // Destructor
   
   #include "drvfuncs.h"

   private:
		void print_coords();

	   const char * fillinstruction;
	   bool penplotter;
	   //  Start DA hpgl color addition
       unsigned int prevColor;
       unsigned int maxPen;
       unsigned int * penColors;	
	   unsigned int maxPenColors; // = 16;
       //  End DA hpgl color addition
   
};

#endif 
 
 
 
