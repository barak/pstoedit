#ifndef __drvTK_h
#define __drvTK_h

/* 
   drvtk.h   - Header file for driver to output Tcl/Tk canvas
             - written by Christopher Jay Cox (ccox@airmail.net) - 9/22/97
               http://www.tcltk.com/tclets/impress/
               Based on...
  
   drvsample.h
   Interface for new driver backends

   Copyright (C) 1993,1994,1995,1996,1997,1998 Wolfgang Glunz, wglunz@geocities.com

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

class drvTK : public drvbase {

public:

	derivedConstructor(drvTK);
//	drvTK(const char * driveroptions_P,ostream & theoutStream, ostream & theerrStream); // Constructor

	~drvTK(); // Destructor

private:
	TempFile		tempFile;
	ofstream		&buffer;
	int			objectId;

	void			canvasCreate();
#include "drvfuncs.h"

};
#endif

