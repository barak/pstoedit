#ifndef __drvFIG_h
#define __drvFIG_h

/*
   drvFIG.h : This file is part of pstoedit
   Interface for new driver backends

   Copyright (C) 1993 - 1999 Wolfgang Glunz, wglunz@geocities.com

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

class drvFIG : public drvbase {

public:

	derivedConstructor(drvFIG);
	//(const char * driveroptions_P, ostream & theoutStream, ostream & theerrStream, const char* outPath_P, const char* outName_P); // Constructor

	~drvFIG(); // Destructor

private:
	int	      objectId;
	TempFile      tempFile;
	ofstream      &buffer;
	int	      imgcount;
	int	      format; // either 31 or something else

#include "drvfuncs.h"

	unsigned int nrOfCurvetos() const;
	void print_polyline_coords();
	void print_spline_coords_noxsplines(int linestyle, float localLineWidth);
	void prpoint(ostream & os, const Point& p) const;
	void print_spline_coords1();
	void print_spline_coords2();

	void show_image(const Image & imageinfo);
};
#endif

