#ifndef __drvRPL_h
#define __drvRPL_h

/* 
   drvrpl.h - header for Real3D RPL polygon driver
            - written by Glenn M. Lewis (glewis@c2.net) - 6/18/96
	      http://www.c2.net/~glewis/
	      Based on...

   drvsampl.h : This file is part of pstoedit
   Class declaration for a sample output driver with no additional attributes
   and methods (minimal interface)

   Copyright (C) 1993,1994,1995,1996,1997 Wolfgang Glunz, Wolfgang.Glunz@mchp.siemens.de

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

class drvRPL : public drvbase {

public:

	drvRPL(const char * driveroptions_P,ostream & theoutStream,ostream & theerrStream ); // Constructor

	~drvRPL(); // Destructor

#include "drvfuncs.h"

};

#endif