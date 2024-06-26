/* 
   drvgcode.cpp : This file is part of pstoedit
   simple backend for CNC g code format.
   Contributed / Copyright 2008 by: Lawrence Glaister VE7IT 

   Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net
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

#include "drvgcode.h"

#include I_stdio
#include I_string_h
#include I_iostream
#include <time.h>
#include "pstoedit_config.h"

drvGCODE::derivedConstructor(drvGCODE):
constructBase
{
// driver specific initializations
	
}

drvGCODE::~drvGCODE()
{
	options=nullptr;
}

void drvGCODE::open_page()
{
#if 0
// Comments by Rohan
// This is a hack
// Since Windows CE does not support, I am just putting a dummy date(i.e "01/01/18 09:00:00")
#ifndef OS_WIN32_WCE
	//date and time of convertion
	const time_t kz = time(nullptr);
	static char sdate[30];
	sdate[0] = 0;
	const struct tm * const ozt = localtime(&kz);
	if (ozt) strftime(sdate, (size_t) 30, "%c", ozt);
#else
	static char sdate[30] = "01/01/18 09:00:00";
#endif
#endif
  	outf << "( Generated by pstoedit " << drvbase::VersionString() << " from " << inFileName.c_str() << " at " << drvbase::DateString() << " )\n";
	outf << "( gcode output module by Lawrence Glaister VE7IT )\n";
	outf << "G20            ( set inches mode )\n";
	outf << "G90            ( absolute distance mode )\n";
	outf << "G17            ( XY plane )\n";
	outf << "G64 P0.003     ( continuous mode with path tolerance )\n";
	outf << "#1000 = 0.100  ( safe Z )\n";
	outf << "#1001 = 10.0   ( feedrate )\n";
	outf << "#1002 = -0.010 ( cutting depth )\n";
	outf << "( mirror about X by setting scale factor negative )\n";
	outf << "#1003 = 0.0139 ( X points to inches scale factor =1/72 )\n";
	outf << "#1004 = 0.0139 ( Y points to inches scale factor =1/72 )\n";
	outf << "M3 S15000      ( spindle cw at 15000rpm )\n";
	outf << "G4 P2          ( let spindle get to speed )\n";
	outf << "M7             ( mist coolant on )\n";
	outf << "G01 Z#1000 F#1001\n";
}

void drvGCODE::close_page()
{
	outf << "G00 Z#1000 ( safe Z )\n";
	outf << "M5         ( spindle off )\n";
	outf << "M9         ( coolant off )\n";
	outf << "M2         ( end of program )\n";
}

void drvGCODE::show_path()
{
	Point currentPoint(0.0f, 0.0f);	
	const Point firstPoint = pathElement(0).getPoint(0);

	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);

		switch (elem.getType()) {
		case moveto:{
				const Point & p = elem.getPoint(0);
				outf << "\nG00 Z#1000\n";
				outf << "G00 X[#1003*" << p.x() << "] Y[#1004*" << p.y() << "]\n";
				outf << "G01 Z#1002\n";
				currentPoint = p;
			}
			break;
		case lineto:{
				const Point & p = elem.getPoint(0);
				outf << "G01 X[#1003*" << p.x() << "] Y[#1004*" << p.y() << "]\n";
				currentPoint = p;
			}
			break;
		case closepath:
				outf << "G01 X[#1003*" << firstPoint.x() << "] Y[#1004*" << firstPoint.y() << "]\n";
			break;

		case curveto:{
			const Point & cp1 = elem.getPoint(0);
			const Point & cp2 = elem.getPoint(1);
			const Point & ep  = elem.getPoint(2);
			// curve is approximated with a variable number or linear segments.
			// fitpoints should be somewhere between 5 and 50 for reasonable page size plots
			// we compute distance between current point and endpoint and use that to help
			// pick the number of segments to use.
			const float dist = (float) pythagoras((float)(ep.x() - currentPoint.x()),(float)(ep.y() - currentPoint.y())); 
			unsigned int fitpoints = (unsigned int)(dist / 10.0);
			if ( fitpoints < 5 ) fitpoints = 5;
			if ( fitpoints > 50 ) fitpoints = 50;

			for (unsigned int s = 1; s < fitpoints; s++) {
				const float t = 1.0f * s / (fitpoints - 1);
				const Point pt = PointOnBezier(t, currentPoint, cp1, cp2, ep);
				outf << " G01 X[#1003*" << pt.x() << "] Y[#1004*" << pt.y() << "]\n";
			}
			currentPoint = ep;

			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvgcode " << endl;
			abort();
			break;
		}
	}
}


static DriverDescriptionT < drvGCODE > D_gcode("gcode", "emc2 gcode format", 
												 "See also:  \\URL{http://linuxcnc.org/}","gcode", false,	// if backend supports subpaths
												   // if subpaths are supported, the backend must deal with
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
												   true,	// if backend supports curves
												   false,	// if backend supports elements with fill and edges
												   false,	// if backend supports text
												   DriverDescription::imageformat::noimage,	// no support for PNG file images
												   DriverDescription::opentype::normalopen, false,	// if format supports multiple pages in one file
												   false /*clipping */ );
 
