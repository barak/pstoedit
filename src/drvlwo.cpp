/* 
   drvlwo.cpp - Driver to output LightWave 3D Objects (LWO)
             - written by Glenn M. Lewis <glenn@gmlewis.com> - 6/18/96
	       http://www.gmlewis.com/>
	       Based on... 

   drvSAMPL.cpp : This file is part of pstoedit
   Skeleton for the implementation of new backends

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
#include "drvlwo.h"
#include I_fstream
#include I_stdio
#include I_stdlib

drvLWO::derivedConstructor(drvLWO):
  constructBase
{
  // driver specific initializations
  // and writing of header to output file
  total_vertices = 0;
  polys = 0;
  total_polys = 0;

  //	float           scale;
  //	float           x_offset;
  //	float           y_offset;
}

drvLWO::~drvLWO()
{
  unsigned long total_bytes = 0;
  unsigned long count = 0;
  LWO_POLY *p;

  // driver specific deallocations
  // and writing of trailer to output file
  outf << "FORM";

  total_bytes = 12L;  // LWOBPNTS+size
  total_bytes += (12L*total_vertices);  // PNTS section...
  total_bytes += 8L;  // POLS+size
  total_bytes += (4L*total_polys + 2L*total_vertices);  // POLS section...
  out_ulong(outf, total_bytes);  // Total file size (-8)
  outf << "LWOBPNTS";
  out_ulong(outf, 12L*total_vertices);
  // Output vertices...
  if (total_vertices > 65536) {
    errf << "ERROR!  Generated more than 65536 vertices!!!  Abort.";
    return;
  }
  for (p = polys; p; p=p->next) {
    for (unsigned long n = 0; n<p->num; n++) {
      out_float(outf, p->x[n]);
      out_float(outf, p->y[n]);
      out_float(outf, 0.0);
    }
  }

  // Now, output polygons...
  outf << "POLS";
  out_ulong(outf, 4L*total_polys + 2L*total_vertices);
  count = 0;
  for (p = polys; p; p=p->next) {
    out_ushort(outf, p->num);
    for (unsigned long n = 0; n<p->num; n++)
      out_ushort(outf, count + n);
    count += p->num;
    out_ushort(outf, 0);  // which surface
  }
}

void drvLWO::print_coords()
{
  LWO_POLY *p = new LWO_POLY;
  p->r = (unsigned char)(255.0 * currentR());
  p->g = (unsigned char)(255.0 * currentG());
  p->b = (unsigned char)(255.0 * currentB());
  p->num = numberOfElementsInPath();
  p->x = new float[p->num];
  p->y = new float[p->num];
  p->next = polys;
  polys = p;
  total_polys++;
  total_vertices += p->num;

  for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
    const basedrawingelement & elem = pathElement(n);
    switch (elem.getType()) {
    case moveto: {
      const Point & pe = elem.getPoint(0);
      // outf << "\t\tmoveto ";
      p->x[n] = pe.x_ + x_offset;
      p->y[n] = pe.y_ + y_offset;
    }
    break;
    case lineto: {
      const Point & pe = elem.getPoint(0);
      // outf << "\t\tlineto ";
      p->x[n] = pe.x_ + x_offset;
      p->y[n] = pe.y_ + y_offset;
    }
    break;
    case closepath: // Not supported
      // outf << "\t\tclosepath ";
      break;
    case curveto:{  // Not supported
    }
    break;
    default:
      errf << "\t\tFatal: unexpected case in drvpdf " << endl;
      abort();
      break;
    }
    //    outf << endl;
  }
  // outf << "]" << endl;
}


void drvLWO::open_page()
{
  //  outf << "Opening page: " << currentPageNumber << endl;
}

void drvLWO::close_page()
{
  //  outf << "Closing page: " << (currentPageNumber) << endl;
}

void drvLWO::show_text(const TextInfo & textinfo )
{
  unused(&textinfo);
  // Must use the -dt flag for this, since RenderMan doesn't support text
}

void drvLWO::show_path()
{
  print_coords();
};

void drvLWO::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
  // outf << "Rectangle ( " << llx << "," << lly << ") (" << urx << "," << ury << ")" << endl;
  // just do show_path for a first guess
  unused(&llx);
  unused(&lly);
  unused(&urx);
  unused(&ury);
  show_path();
}

static DriverDescriptionT<drvLWO> D_lwo("lwo","LightWave 3D Object Format","lwo",
   	  false, // if backend supports subpathes, else 0
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

	  false,  // if backend supports curves, else 0
	  false,  // if backend supports elements with fill and edges
	  false,  // backend supports text
	  false,  // backend supports images
	  DriverDescription::binaryopen,
	  false); // backend supports multiple pages
 
