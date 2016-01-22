/* 
   drvPIC.cpp : This file is part of pstoedit

   Driver for PIC troff/groff drawing format
   NOTE: primitve version, only supports lines and text

   Copyright (C) 1999 Egil Kvaleberg, egil_AT_kvaleberg.no

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
#include "drvpic.h"
#include I_fstream
#include I_stdio
#include I_stdlib

const float PIC_SCALE = 72.0f;	/* convert from points to inches */
#define USE_SPLINE 0			/* BUG: must implement */

/*
 *  fonts supported by groff
 *  NOTE: could have been made dynamic
 */
static const struct fx {
	const char *f_name;
	const char *f_groff;
} fontxlate[] = {
	{
	"Times-Roman", "TR"}, {
	"Times-Italic", "TI"}, {
	"Times-Bold", "TB"}, {
	"Times-BoldItalic", "TBI"}, {
	"AvantGarde-Book", "AR"}, {
	"AvantGarde-BookOblique", "AI"}, {
	"AvantGarde-Demi", "AB"}, {
	"AvantGarde-DemiOblique", "ABI"}, {
	"Bookman-Light", "BMR"}, {
	"Bookman-LightItalic", "BMI"}, {
	"Bookman-Demi", "BMB"}, {
	"Bookman-DemiItalic", "BMBI"}, {
	"Courier", "CR"}, {
	"Courier-Oblique", "CI"}, {
	"Courier-Bold", "CB"}, {
	"Courier-BoldOblique", "CBI"}, {
	"Helvetica", "HR"}, {
	"Helvetica-Oblique", "HI"}, {
	"Helvetica-Bold", "HB"}, {
	"Helvetica-BoldOblique", "HBI"}, {
	"Helvetica-Narrow", "HNR"}, {
	"Helvetica-Narrow-Oblique", "HNI"}, {
	"Helvetica-Narrow-Bold", "HNB"}, {
	"Helvetica-Narrow-BoldOblique", "HNBI"}, {
	"NewCenturySchlbk-Roman", "NR"}, {
	"NewCenturySchlbk-Italic", "NI"}, {
	"NewCenturySchlbk-Bold", "NB"}, {
	"NewCenturySchlbk-BoldItalic", "NBI"}, {
	"Palatino-Roman", "PR"}, {
	"Palatino-Italic", "PI"}, {
	"Palatino-Bold", "PB"}, {
	"Palatino-BoldItalic", "PBI"}, {
	"Symbol", /* (Greek) */ "S"},
	{
	"Symbol-Slanted", "SS"}, {
	"ZapfChancery-MediumItalic", "ZCMI"}, {
	"ZapfDingbats", "ZD"}, {
	"ZapfDingbats-Reverse", "ZDR"}, {
	0, 0}
};

/*
 *
 */




drvPIC::derivedConstructor(drvPIC):constructBase
// troff_mode(0), landscape(0), keep_font(0), text_as_text(0), debug(0)
{
// driver specific initializations
// and writing of header to output file
	outf << ".\\\" PIC generated by pstoedit\n";

	withinPS = 0;
	pageheight = 10.5;			/* BUG: silly default */
	largest_y = 0.0;
#if 0
	if (d_argc > 0) {
		if (Verbose())
			errf << ".\\\" Driver options:" << endl;
		for (unsigned int i = 0; i < d_argc; i++) {
			assert(d_argv && d_argv[i]);
			if (Verbose())
				errf << ".\\\" " << d_argv[i] << endl;
			if (strcmp(d_argv[i], "-groff") == 0) {
				troff_mode = 0;
			} else if (strcmp(d_argv[i], "-troff") == 0) {
				troff_mode = 1;
			} else if (strcmp(d_argv[i], "-landscape") == 0) {
				landscape = 1;
			} else if (strcmp(d_argv[i], "-portrait") == 0) {
				landscape = 0;
			} else if (strcmp(d_argv[i], "-keep") == 0) {
				keep_font = 1;
			} else if (strcmp(d_argv[i], "-text") == 0) {
				text_as_text = 1;
			} else if (strcmp(d_argv[i], "-debug") == 0) {
				debug = 1;
			} else {
				errf << "Unrecognized pic option: " << d_argv[i]
					<< endl;
			}
		}
	}

#endif

//      float           scale;
}

drvPIC::~drvPIC()
{
// driver specific deallocations
// and writing of trailer to output file
	outf << ".\\\" PIC end\n";
	options=0;
}

float drvPIC::x_coord(float x, float y) const
{
	if (options->landscape)
		return (y + y_offset) / PIC_SCALE;
	else
		return (x + x_offset) / PIC_SCALE;
}

float drvPIC::y_coord(float x, float y) const 
{
	if (options->landscape)
		return pageheight - (x + x_offset) / PIC_SCALE;
	else
		return (y + y_offset) / PIC_SCALE;
}


void drvPIC::print_coords()
{
	float move_x = 0.0;
	float move_y = 0.0;
	float y;
	int within_line = 0;

	ps_begin();

	if (options->debug) {
		outf << ".\\\" xoffs,yoffs,height: "
			<< x_offset << "," << y_offset << "," << currentDeviceHeight << endl;
	}

	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:{
				const Point & p = elem.getPoint(0);
				if (within_line)
					outf << endl;
				y = y_coord(p.x_, p.y_);
				outf << "line from " << x_coord(p.x_, p.y_) << "," << y;
				move_x = p.x_;
				move_y = p.y_;
				if (y > largest_y)
					largest_y = y;
				within_line = 1;
			}
			break;
		case lineto:{
				const Point & p = elem.getPoint(0);
				if (!within_line) {
					errf << "line from no starting point" << endl;
				}
				y = y_coord(p.x_, p.y_);
				outf << " to " << x_coord(p.x_, p.y_) << "," << y;
				if (y > largest_y)
					largest_y = y;
				within_line = 1;
			}
			break;
#if USE_SPLINE
		case curveto:{
				/* BUG: outf << "spline from "; */
				for (unsigned int cp = 0; cp < 3; cp++) {
					const Point & p = elem.getPoint(cp);
					y = y_coord(p.x_, p.y_);
					outf << " to " << x_xoord(p.x_, p.y_) << "," << y;
					if (y > largest_y)
						largest_y = y;
				}
				within_line = 1;
			}
			break;
#endif
		case closepath:
			// do this "manually"...
			outf << " to " << x_coord(move_x, move_y) << "," << y_coord(move_x, move_y);
			break;
		default:
			errf << "\tFatal: unexpected case in drvpic " << endl;
			abort();
			break;
		}
	}
	if (within_line)
		outf << endl;
}

void drvPIC::ps_begin()
{
	if (!withinPS) {
		outf << ".PS\n";
		outf << ".ps 11\n";
		withinPS = 1;
		largest_y = 0.0;
	}
}


void drvPIC::ps_end()
{
	if (withinPS) {
		outf << ".PE\n";
		withinPS = 0;
	}
}


void drvPIC::open_page()
{
	outf << ".\\\" Page: " << currentPageNumber << endl;
}

void drvPIC::close_page()
{
	ps_end();
}

void drvPIC::show_text(const TextInfo & textinfo)
{
	const char *fontname = textinfo.currentFontName.c_str();
	const char *fontweight = textinfo.currentFontWeight.c_str();
	int pointsize = (textinfo.currentFontSize < 2.0) ? 0 : (int) (textinfo.currentFontSize + 1.8);
	// BUG: point size is strange...
	const char *tfont = 0;

	const float x = x_coord(textinfo.x, textinfo.y);
	const float y = y_coord(textinfo.x, textinfo.y);

	// const unsigned int fontstringlen=80;
	static RSString selected_font;	// Could / should these be class members (wogl? ? ?) 
	static bool font_selected = false;

	static int is_text = 0;		// ...

	/* translate font to groff/troff style */
	if (!options->troff_mode) {
		for (const struct fx * fp = fontxlate; fp->f_name; ++fp) {
			if (strcmp(fontname, fp->f_name) == 0) {
				tfont = fp->f_groff;
				break;
			}
		}
	}
	if (options->keepFont && !tfont) {
		tfont = fontname;
	}

	if (!tfont) {
		if (strcmp(fontweight, "Bold") == 0) {
			tfont = "B";
		} else {				/* "Regular" */
			tfont = "R";
		}
	}

	/* this is veeery ad-hoc: make assumption about text mode */
	if (options->textAsText) {
		if (!withinPS) {
			/* we assume drawing does not start with text */
			is_text = 1;
		} else if (y > largest_y) {
			/* forwards beyond drawing: suspect text mode */
			is_text = 1;
		} else {
			/* continue in drawing */
			is_text = 0;
		}

	}

	if (is_text) {
		static int selected_size = 0;	// ...
		ps_end();

		if (!font_selected) {
			outf << ".ft " << tfont << endl;
			selected_font = tfont;
			font_selected = true;
		} else if ( tfont != selected_font) {
			outf << ".ft " << tfont << endl;
			selected_font = tfont;
		}
		if (pointsize && selected_size != pointsize) {
			outf << ".ps " << pointsize << endl;
			selected_size = pointsize;
		}

		for (const char *p = textinfo.thetext.c_str(); *p; ++p) {
			switch (*p) {
			case '\\':
				outf << "\\\\";
				break;
			case '`':
			case '.':
				if (p == textinfo.thetext.c_str()) {
					outf << "\\&";
				}
				/* drop through */
				//lint -fallthrough
			default:
				outf << *p;
				break;
			}
		}
		outf << endl;
	} else {
		ps_begin();

		if (options->debug) {
			outf << endl;
			outf << ".\\\" currentFontName: " << textinfo.currentFontName.c_str() << endl;
			outf << ".\\\" currentFontFamilyName: " << textinfo.
				currentFontFamilyName.c_str() << endl;
			outf << ".\\\" currentFontFullName: " << textinfo.currentFontFullName.c_str() << endl;
			outf << ".\\\" currentFontSize: " << textinfo.currentFontSize << endl;
			outf << ".\\\" currentFontWeight: " << textinfo.currentFontWeight.c_str() << endl;
			outf << ".\\\" currentFontAngle: " << textinfo.currentFontAngle << endl;
			outf << ".\\\" currentRGB: " << textinfo.
				currentR << "," << textinfo.currentG << "," << textinfo.currentB << endl;
		}

		outf << '"';
		if (pointsize)
			outf << "\\s" << pointsize;
		outf << "\\f";
		// groff/troff style font name
		if (strlen(tfont) > 1)
			outf << '[' << tfont << ']';
		else
			outf << tfont;

		// string itself
		for (const char *p = textinfo.thetext.c_str(); *p; ++p) {
			switch (*p) {
			case '"':
				outf << "\\\"";
				break;
			case '\\':
				outf << "\\\\";
				break;
			default:
				outf << *p;
				break;
			}
		}

		outf << "\\fP"			// restore font
			<< '"' << " at " << x << "," << y << " ljust" << endl;
	}
}

void drvPIC::show_path()
{
	// BUG:
	if (options->debug) {
		outf << endl << ".\\\" Path # " << currentNr();
		if (isPolygon())
			outf << " (polygon): " << endl;
		else
			outf << " (polyline): " << endl;
		outf << ".\\\" currentShowType: ";
		switch (currentShowType()) {
		case drvbase::stroke:
			outf << "stroked";
			break;
		case drvbase::fill:
			outf << "filled";
			// BUG: implement... */
			break;
		case drvbase::eofill:
			outf << "eofilled";
			// BUG: implement... */
			break;
		default:
			// cannot happen
			errf << "unexpected ShowType " << (int) currentShowType();
			break;
		}
		outf << endl;
		outf << ".\\\" currentLineWidth: " << currentLineWidth() << endl;
		outf << ".\\\" currentRGB: " << currentR() << ","
			<< currentG() << "," << currentB() << endl;
		outf << ".\\\" currentLineCap: " << currentLineCap() << endl;
		outf << ".\\\" dashPattern: " << dashPattern() << endl;
	}
	print_coords();
}

static DriverDescriptionT < drvPIC > D_PIC("pic", "PIC format for troff et.al.","", "pic", true,	// backend supports subpathes
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
										   USE_SPLINE,	// backend supports curves
										   true,	// backend supports elements which are filled and have edges 
										   true,	// backend supports text
										   DriverDescription::noimage,	// no support for PNG file images
										   DriverDescription::normalopen, true,	// if format supports multiple pages in one file (wogl? ? ?)
										   false  /*clipping */
										   );
