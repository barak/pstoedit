#ifndef __drvFIG_h
#define __drvFIG_h

/* 
   drvFIG.h : This file is part of pstoedit
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

class drvFIG : public drvbase {

public:

	drvFIG(const char * driveroptions_P,ostream & theoutStream,ostream & theerrStream ); // Constructor

	~drvFIG(); // Destructor

private:
	int           objectId;
	TempFile      tempFile;
	ofstream     &buffer;

#include "drvfuncs.h"

};
#endif

