#ifndef __drvTGIF_h
#define __drvTGIF_h

/* 
   drvTGIF.h : This file is part of pstoedit
   Interface for new driver backends

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
#include <fstream.h>

// for tmpnam
#include <stdio.h>
#include "drvbase.h"

class drvTGIF : public drvbase {

public:

	drvTGIF(const char * driveroptions_P,ostream & theoutStream,ostream & theerrStream ,float theMagnification); // Constructor

	~drvTGIF(); // Destructor

private:
	TempFile      tempFile;
	ofstream     &buffer;
	int             objectId;
	const float     magnification;	

#include "drvfuncs.h"

};
#endif

