/* 
   drvHPGL.cpp : This file is part of pstoedit
   HPGL / HPGL2 Device driver

   Copyright (C) 1993 - 2001 Peter Katzmann p.katzmann@thiesen.com
   Copyright (C) 2000  Katzmann & Glunz (fill stuff)
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

  
	Aug 21, 2001: Donald Arseneau (asnd@triumf.ca) adds pen changes when color changes.
    Changed sections are delimited by:
    //  Start DA hpgl color addition
    //  End DA hpgl color addition

*/

#include "drvhpgl.h"
#include I_stdio
#include I_string_h
#include I_iostream

static const OptionDescription driveroptions[] = {
	OptionDescription("-pen",0,"plotter is pen plotter"),
	OptionDescription("-pencolors","number","number of pen colors available"),
	OptionDescription("-fill","string","select fill type e.g. FT 1"),
	endofoptions};

drvHPGL::derivedConstructor(drvHPGL):
constructBase, fillinstruction("FT1"), penplotter(false),
	//  Start DA hpgl color addition
	prevColor(5555), 
	maxPen(0), 
	penColors(0), 
	maxPenColors(0)
	//  End DA hpgl color addition
{
	// driver specific initializations
	// and writing of header to output file

	bool show_usage_line = false;

	for (unsigned int i = 0; i < d_argc; i++) {
		assert(d_argv && d_argv[i]);	//lint !e796
		if (verbose)
			outf << "% " << d_argv[i] << endl;
		if (strcmp(d_argv[i], "-pen") == 0) {
			penplotter = true;
		} else if (strcmp(d_argv[i], "-pencolors") == 0) {
			i++;
			if (i >= d_argc) {
				errf << "-pencolors requires a numeric argument " << endl;
				show_usage_line = true;
			} else {
				maxPenColors = atoi(d_argv[i]);
			}
		} else if (strcmp(d_argv[i], "-fill") == 0) {
			i++;
			if (i >= d_argc) {
				errf << "-fill requires a string argument " << endl;
				show_usage_line = true;
			} else {
				fillinstruction = d_argv[i];
			}
			/*
			   Fill Type (FT) Command 
			   ========================================= 
			   This command selects the shading pattern used to fill polygons ( FP ), rectangles 
			   ( RA or RR ), wedges ( WG ), or characters ( CF ). The Fill Type command ( FT 
			   ), can use solid, shading, parallel lines (hatching), cross hatching, patterned 
			   (raster) fill, or PCL user-defined patterns. For more information see the PCL 5 
			   Printer Language Technical Reference Manual. The syntax for this command is 
			   as follows: 

			   FT fill type,[option1,[option2]]; or FT; 
			 */
// known fill types:
			// FT 1 - solid black
			// FT 3 - parallel lines FT 3[,delta,angle]
			// FT 4 - cross hatching FT 4[,delta,angle]
			// FT 10 - shading FT 10,[percentage]
		} else if (strcmp(d_argv[i], "-help") == 0) {
			errf << "-help    Show this message" << endl;
			errf << "-fill fillstring (default FT1)" << endl;
			errf << "-pencolors number // define a number of colors pens to use" << endl;
			errf << "-pen (for pen plotters)" << endl;
			//                   errf << "-depth # Set the page depth in inches" << endl;
			show_usage_line = true;
		} else {
			errf << "Unknown fig driver option: " << d_argv[i] << endl;
			show_usage_line = true;
		}
	}
	if (show_usage_line) {
		errf << "Usage -f 'hpgl: [-help] [-fill fillstring] [-pen] [-pencolors number]'" << endl;
	}
//  if (0) {
//      const char esc = (char) 27;
//      outf << esc << ".(;";
//  }
	outf << "IN;SC;PU;PU;SP1;LT;VS10\n";
	scale = 10;

	penColors = new unsigned int[maxPenColors + 1 + 1 ];	// 1 offset - 0 is not used // one more for flint ;-)
	for (unsigned p = 0; p <= maxPenColors + 1; p++) {
		penColors[p] = 0;
	}
	//   float           x_offset;
	//   float           y_offset;
}

drvHPGL::~drvHPGL()
{
	// driver specific deallocations
	// and writing of trailer to output file
	outf << "PU;PA0,0;SP;EC;PG1;EC1;OE\n";
	fillinstruction = NIL;
	delete[]penColors;
	penColors = NIL;
}

void drvHPGL::print_coords()
{
	const unsigned int elems = numberOfElementsInPath();
	if (elems) {
		for (unsigned int n = 0; n < elems; n++) {
			const basedrawingelement & elem = pathElement(n);
			switch (elem.getType()) {
			case moveto:
				{
					const Point & p = elem.getPoint(0);
					outf << "PU";
					outf << p.x_ + x_offset << "," << p.y_ + y_offset << ";";
				}
				break;
			case lineto:
				{
					const Point & p = elem.getPoint(0);
					outf << "PD";
					outf << p.x_ + x_offset << "," << p.y_ + y_offset << ";";
					if (isPolygon() && (n == elems)) {
						outf << "PD";
						const basedrawingelement & elemnull = pathElement(0);
						const Point & pnull = elemnull.getPoint(0);
						outf << pnull.x_ + x_offset << "," << pnull.y_ + y_offset << ";";
					}
				}
				break;
			case closepath:
				{
					// outf << "\t\tclosepath ";
					const Point & p = pathElement(0).getPoint(0);
					outf << "PD";
					outf << p.x_ + x_offset << "," << p.y_ + y_offset << ";";
				}
				break;
			case curveto:
				errf << "\t\tFatal: unexpected case curveto in drvhpgl " << endl;
				abort();
				break;
			default:
				errf << "\t\tFatal: unexpected case default in drvhpgl " << endl;
				abort();
				break;
			}
		}
	}
}


void drvHPGL::open_page()
{
	//  Start DA hpgl color addition
	prevColor = 5555;
	maxPen = 0;
	//  End DA hpgl color addition
	outf << "IN;SC;PU;PU;SP1;LT;VS10\n";
}

void drvHPGL::close_page()
{
	outf << "PU;SP;EC;PG1;EC1;OE\n";
}

void drvHPGL::show_text(const TextInfo & textinfo)
{
	unused(&textinfo);
	// outf << "textttt " << textinfo.thetext.value() << endl;
}

void drvHPGL::show_path()
{
	//  Start DA hpgl color addition

	/* 
	 *  This block should be a separate subroutine, called by each of
	 *  the hpgl subroutines.  
	 *
	 *  The object is to generate pen switching commands when the color
	 *  changes.  We keep a list of pen colors, which approximate the 
	 *  desired rgb colors.  Choose an existing pen number when the 
	 *  rgb color approximates that color, and add a new color to the
	 *  list when the rgb color is distinctly new.
	 */


	if (numberOfElementsInPath()) {

		if (maxPenColors > 0) {
			const unsigned int reducedColor = 256 * (unsigned int) (currentR() * 16) +
				16 * (unsigned int) (currentG() * 16) + (unsigned int) (currentB() * 16);

			if (prevColor != reducedColor) {
				// If color changed, see if color has been used before
				unsigned int npen = 0;
				if (maxPen > 0) {
					for (unsigned int j = 1; j <= maxPen; j++) {	// 0th element is never used - 0 indicates "new" color
						if (penColors[j] == reducedColor) {
							npen = j;
						}
					}
				}
				// If color is new, add it to list, if room
				if (npen == 0) {
					if (maxPen < maxPenColors) {
						maxPen++;
					}
					npen = maxPen;
					//cout << "npen : " << npen << " maxPenColors" << maxPenColors << endl;
					penColors[npen] = reducedColor;
				}
				// Select new pen
				prevColor = reducedColor;
				outf << "PU; \nSP" << npen << ";\n";
			}
			//  End DA hpgl color addition
		}

		switch (currentShowType()) {
		case drvbase::stroke:
			break;
		case drvbase::eofill:
			// Probably Wrong, but treat eoffil like fill
		case drvbase::fill:
			{
				const Point & p = pathElement(0).getPoint(0);
				outf << "PU";
				outf << p.x_ + x_offset << "," << p.y_ + y_offset << ";";
				outf << fillinstruction << ";PM0;";
			}
			break;
		default:				// cannot happen
			outf << "unexpected ShowType " << (int) currentShowType();
			break;
		}
		if (!penplotter) {
			outf << "PW" << currentLineWidth() / 10 << ";";
		}
		print_coords();

		switch (currentShowType()) {	// To check which endsequenz we need
		case drvbase::stroke:
			break;
		case drvbase::eofill:
			// Probably Wrong, but treat eoffil like fill
		case drvbase::fill:
			outf << "PM2;FP;EP;";	// EP also draws path
			break;
		default:				// cannot happen
			outf << "unexpected ShowType " << (int) currentShowType();
			break;
		}
		outf << endl;
	}
}

void drvHPGL::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
	unused(&llx);
	unused(&lly);
	unused(&urx);
	unused(&ury);
	// just do show_path for a first guess
	show_path();
}

static DriverDescriptionT < drvHPGL > D_hpgl("hpgl", "HPGL code", "hpgl", false,	// backend supports subpathes
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
											 false,	// backend supports curves
											 false,	// backend supports elements which are filled and have edges
											 false,	// backend supports text
											 false,	// backend supports Images
											 false,	// no support for PNG file images
											 DriverDescription::normalopen, false,	// backend support multiple pages
											 false /*clipping */, driveroptions );
 
