/* 
   drvGNUPLOT.cpp : This file is part of pstoedit
   simple backend for GNUplot format.
   Contributed by: Carsten Hammer (chammer@hermes.hrz.uni-bielefeld.de)

   Copyright (C) 1993 - 2001 Wolfgang Glunz, wglunz@pstoedit.net
   (for the skeleton and the rest of pstoedit)

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

#include "drvgnplt.h"

#include I_stdio
#include I_string_h
#include I_iostream



drvGNUPLOT::derivedConstructor(drvGNUPLOT):
//(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream):
constructBase
{
// driver specific initializations
}

drvGNUPLOT::~drvGNUPLOT()
{
}


void drvGNUPLOT::close_page()
{
	outf << "#Seite beendet.\n";
}

void drvGNUPLOT::open_page()
{
	outf << "#Neue Seite\n";
}

void drvGNUPLOT::show_text(const TextInfo & textinfo)
{
	unused(&textinfo);
}

void drvGNUPLOT::show_path()
{
	outf << "\n#Polyline:\n";
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const Point & p = pathElement(n).getPoint(0);
		outf << p.x_ << "	" << p.y_ << "\n";
	}
};

void drvGNUPLOT::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
	// just do show_polyline for a first guess
	unused(&llx);
	unused(&lly);
	unused(&urx);
	unused(&ury);
	show_path();
}

static DriverDescriptionT < drvGNUPLOT > D_gnuplot("gnuplot", "gnuplot format", "gnuplot", false,	// if backend supports subpathes
												   // if subpathes are supported, the backend must deal with
												   // sequences of the following form
												   // moveto (start of subpath)
												   // lineto (a line segment)
												   // lineto 
												   // moveto (start of a new subpath)
												   // lineto (a line segment)
												   // lineto 
												   //
												   // If this argument is set to false each subpath is drawn 
												   // individually which might not necessarily represent
												   // the original drawing.
												   false,	// if backend supports curves
												   false,	// if backend supports elements with fill and edges
												   false,	// if backend supports text
												   false,	// if backend supports Images
												   false,	// no support for PNG file images
												   DriverDescription::normalopen, false,	// if format supports multiple pages in one file
												   false /*clipping */ , nodriverspecificoptions);
 
