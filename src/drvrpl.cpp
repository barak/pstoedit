/* 
   drvrpl.cpp - Driver to output Real3D RPL polygons
             - written by Glenn M. Lewis (glewis@c2.net) - 6/18/96
	       http://www.c2.net/~glewis/
	       Based on...

   drvSAMPL.cpp : This file is part of pstoedit
   Skeleton for the implementation of new backends

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

#include <stdio.h>
// rcw2: work round case insensitivity in RiscOS
#ifdef riscos
  #include "unix:string.h"
#else
  #include <string.h>
#endif
#include <stdlib.h>
#include "drvrpl.h"

drvRPL::drvRPL(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream): // Constructor
  drvbase(driveroptions_p,theoutStream,theerrStream,
	  0, // if backend supports subpathes, else 0
	  // if subpathes are supported, the backend must deal with
	  // sequences of the following form
	  // moveto (start of subpath)
	  // lineto (a line segment)
	  // lineto 
	  // moveto (start of a new subpath)
	  // lineto (a line segment)
	  // lineto 
	  //
	  // If this argument is set to 0 each subpath is drawn 
	  // individually which might not necessarily represent
	  // the original drawing.

	  0,  // if backend supports curves, else 0
	  0  // if backend supports elements with fill and edges
	  )
{
  // driver specific initializations
  // and writing of header to output file
}

drvRPL::~drvRPL()
{
  // driver specific deallocations
  // and writing of trailer to output file
}

void drvRPL::print_coords()
{
  for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
    const basedrawingelement & elem = pathElement(n);
    switch (elem.getType()) {
    case moveto: {
      const Point & p = elem.getPoint(0);
      outf  << p.x_ + x_offset << " 0 " << p.y_ + y_offset << endl;
    }							      
    break;						      
    case lineto: {					      
      const Point & p = elem.getPoint(0);		      
      outf  << p.x_ + x_offset << " 0 " << p.y_ + y_offset << endl;
    }
    break;
    case closepath: // Not supported
      break;
    case curveto:{  // Not supported
    }
    break;
    default:
      errf << "\t\tFatal: unexpected case in drvpdf " << endl;
      abort();
      break;
    }
  }
  outf << "0 1 0 ( dvect )" << endl;
  outf << numberOfElementsInPath() << " ( count )" << endl;
  outf << currentR() << " " << currentG() << " " << currentB() << " ( RGBA )" << endl;
  outf << "\"polygon\" ( name )" << endl;
  outf << "0 ( flags )" << endl;
  outf << "\"CEND\"" << endl;
  outf << "C_POLYGON DROP" << endl << endl;
}


void drvRPL::open_page()
{
  //  outf << "Opening page: " << currentPageNumber << endl;
}

void drvRPL::close_page()
{
  //  outf << "Closing page: " << (currentPageNumber) << endl;
}

void drvRPL::show_text(const TextInfo & textinfo)
{
  unused(&textinfo);
  // Must use the -dt flag for this, since RenderMan doesn't support text
}

void drvRPL::show_path()
{
  print_coords();
};

void drvRPL::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
  // outf << "Rectangle ( " << llx << "," << lly << ") (" << urx << "," << ury << ")" << endl;
  // just do show_path for a first guess
  unused(&llx);
  unused(&lly);
  unused(&urx);
  unused(&ury);
  show_path();
}