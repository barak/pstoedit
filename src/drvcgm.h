#ifndef __drvCGM_h
#define __drvCGM_h

/* 
   drvcgm.h : This file is part of pstoedit
	Class declaration for CGM backend

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

struct cdImageStruct; // forward
struct cdPointStruct; // forward

class drvCGM : public drvbase {

public:

	drvCGM(const char * driveroptions_P,ostream & theoutStream,ostream & theerrStream, bool binary); // Constructor

	~drvCGM(); // Destructor

	virtual const char * const *    knownFontNames() const ;

#include "drvfuncs.h"

	unsigned int getcdcolor(float r, float g, float b);

	const bool binary_; // indicates whether to write binary or ascii cgm
	cdPointStruct 	*cdpoints;
	cdImageStruct 	*cgmimage;

};

#endif
