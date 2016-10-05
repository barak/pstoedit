/* 
   drvHPGL.cpp : This file is part of pstoedit
   HPGL / HPGL2 Device driver

   Copyright (C) 1993,1994,1995,1996,1997,1998 Peter Katzmann p.katzmann@thiesen.com

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
#include "drvhpgl.h"
#include I_stdio
#include I_string_h
#include I_iostream

//   drvHPGL::drvHPGL(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream): // Constructor
//  drvbase(driveroptions_p,theoutStream,theerrStream,
// 0, // if backend supports subpathes, else 0
//0,  // if backend supports curves, else 0
//   0  // if backend supports elements with fill and edges
//   )

drvHPGL::derivedConstructor(drvHPGL):
	constructBase
   {
   // driver specific initializations
   // and writing of header to output file
      outf << "IN;SC;PU;PU;SP1;LT;VS10\n";
      scale = 10;
   //	float           x_offset;
   //	float           y_offset;
   }

   drvHPGL::~drvHPGL() {
   // driver specific deallocations
   // and writing of trailer to output file
      outf << "PU;PA0,0;SP;EC;PG1;EC1;OE\n";
   }

   void drvHPGL::print_coords()
   {
      for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
         const basedrawingelement & elem = pathElement(n);
         switch (elem.getType()) {
         case moveto: 
            {
               const Point & p = elem.getPoint(0);
               outf << "PU";
               outf  << p.x_ + x_offset << "," 
                  <<  p.y_ + y_offset << ";" ;
            }
            break;
         case lineto: 
            {
               const Point & p = elem.getPoint(0);
               outf << "PD";
               outf  << p.x_ + x_offset << "," 
                  <<   p.y_ + y_offset << ";" ;
	       if (isPolygon() && (n==numberOfElementsInPath())) {
                 outf << "PD";
		 const basedrawingelement & elemnull = pathElement(0);
                 const Point & pnull = elemnull.getPoint(0);
                 outf  << pnull.x_ + x_offset << ","
                  <<  pnull.y_ + y_offset << ";" ;
	       }
            }
            break;
         case closepath: 
         // outf << "\t\tclosepath ";
            break;
         case curveto:
            {
            }
            break;
         default:
            errf << "\t\tFatal: unexpected case in drvpdf " << endl;
            abort();
            break;
         }
      }
   }


   void drvHPGL::open_page()
   {
      outf << "IN;SC;PU;PU;SP1;LT;VS10\n";
   }

   void drvHPGL::close_page()
   {
      outf << "PU;SP;EC;PG1;EC1;OE\n";
   }

   void drvHPGL::show_text(const TextInfo & textinfo)
   {
	unused(&textinfo);
   }

   void drvHPGL::show_path()
   {
      switch (currentShowType() ) {
      case drvbase::stroke : 
         break;
      case drvbase::eofill :
      // Probably Wrong, but treat eoffil like fill
      case drvbase::fill :
         break;
      default:       // cannot happen
         outf << "unexpected ShowType " << (int) currentShowType() ;
         break;
      }
      outf << "PW" << currentLineWidth() / 10 << endl;
      print_coords();
   
      switch (currentShowType() ) {  // To check which endsequenz we need
      case drvbase::stroke : 
         break;
      case drvbase::eofill :
      // Probably Wrong, but treat eoffil like fill
      case drvbase::fill :
         break;
      default:       // cannot happen
         outf << "unexpected ShowType " << (int) currentShowType() ;
         break;
      }
   };

   void drvHPGL::show_rectangle(const float llx, const float lly, const float urx, const float ury)
   {
	unused(&llx);
	unused(&lly);
	unused(&urx);
	unused(&ury);
   // just do show_path for a first guess
      show_path();
   }

static DriverDescriptionT<drvHPGL> D_hpgl(
			"hpgl","HPGL code","hpgl",
		false, // backend supports subpathes
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

		false, // backend supports curves
		false, // backend supports elements which are filled and have edges
		false, // backend supports text
		false, // backend supports Images
		DriverDescription::normalopen,	
		false); // backend support multiple pages

 
