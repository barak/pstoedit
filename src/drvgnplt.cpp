/* 
   drvGNUPLOT.cpp : This file is part of pstoedit
   simple backend for GNUplot format.
   Contributed by: Carsten Hammer (chammer@hermes.hrz.uni-bielefeld.de)

   Copyright (C) 1993,1994,1995,1996,1997 Wolfgang Glunz, Wolfgang.Glunz@mchp.siemens.de
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

#include <stdio.h>
// rcw2: work round case insensitivity in RiscOS
#ifdef riscos
  #include "unix:string.h"
#else
  #include <string.h>
#endif
#include <iostream.h>

#include "drvgnplt.h"

drvGNUPLOT::drvGNUPLOT(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream):
	drvbase(driveroptions_p,theoutStream,theerrStream,0,0,0)
{
// driver specific initializations
}

drvGNUPLOT::~drvGNUPLOT() {}


void drvGNUPLOT::print_coords()
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
		outf << p.x_ <<"	" << p.y_ << "\n";
	}
};

void drvGNUPLOT::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
 // just do show_polyline for a first guess
  	unused(&llx); unused(&lly); unused(&urx); unused(&ury);
	show_path();
}
