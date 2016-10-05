/*
   drvFIG.cpp : This file is part of pstoedit
   Based on the skeleton for the implementation of new backends

   Copyright (C) 1993 - 2001 Wolfgang Glunz, wglunz@pstoedit.net

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

/*
    FIG 3.1 driver by Ian MacPhedran (Ian_MacPhedran@engr.usask.ca)
    April 1995

    Color support and conversion to use C++ streams done by Wolfgang Glunz

    Object depth support by Gerhard Kircher <kircher@edvz.tuwien.ac.at>
    March 1996

    Curves and image support by Leszek Piotrowicz <zorro@delta.ds2.pg.gda.pl>
    July 1997

    Support for xfig format 3.2 (X-Splines) by Wolfgang Glunz

    Support for subpaths by Burkhard Plaum
    
 Change to level calculation Ian MacPhedran
 November 2000
 Simplify - remove superfluous 3.1 code Ian MacPhedran
 December 2000
 Use quintic interpolated X-Splines Ian MacPhedran
 December 2000
*/

#include "drvfig.h"

// for sprintf
#include I_stdio
#include I_string_h
#include I_iostream
#include I_iomanip


static const float PntFig = 1200.0f / 72.0f;


static const char *colorstring(float r, float g, float b)
{
	static char buffer[15];
	sprintf(buffer, "%s%.2x%.2x%.2x", "#", (unsigned int) (r * 255),
			(unsigned int) (g * 255), (unsigned int) (b * 255));

	return buffer;
}

static const char *fig_default_colors[] = {
	"#000000",
	"#0000ff",
	"#00ff00",
	"#00ffff",
	"#ff0000",
	"#ff00ff",
	"#ffff00",
	"#ffffff",
	"#000090",
	"#0000b0",
	"#0000d0",
	"#87ceff",
	"#009000",
	"#00b000",
	"#00d000",
	"#009090",
	"#00b0b0",
	"#00d0d0",
	"#900000",
	"#b00000",
	"#d00000",
	"#900090",
	"#b000b0",
	"#d000d0",
	"#803000",
	"#a04000",
	"#c06000",
	"#ff8080",
	"#ffa0a0",
	"#ffc0c0",
	"#ffe0e0",
	"#ffd700"
};

const int defaults = sizeof(fig_default_colors) / sizeof(char *);
static ColorTable colorTable(fig_default_colors, defaults, colorstring);

static unsigned int registercolor(float r, float g, float b)
{
	return colorTable.getColorIndex(r, g, b);
}

static void dumpnewcolors(ostream & theoutStream)
{
	unsigned int current = defaults;
	const char *colstring = 0;
	while ((colstring = colorTable.getColorString(current)) != 0) {
		theoutStream << "0 " << current << " " << colstring << endl;
		current++;
	}
}

static const OptionDescription driveroptions[] = {
	OptionDescription("-startdepth","number","Set the initial depth (default 999)"),
	OptionDescription("-depth","number","Set the page depth in inches"),
	endofoptions};

drvFIG::derivedConstructor(drvFIG):
constructBase,
buffer(tempFile.asOutput()),
imgcount(1),
format(32),
startdepth(999),
glob_min_x(0), glob_max_x(0), glob_min_y(0), glob_max_y(0),
loc_min_x(0), loc_max_x(0), loc_min_y(0), loc_max_y(0), glo_bbox_flag(0), loc_bbox_flag(0)
{
	// driver specific initializations
	float depth_in_inches = 11;
	bool show_usage_line = false;
	const char *paper_size = NULL;

	for (unsigned int i = 0; i < d_argc; i++) {
		assert(d_argv && d_argv[i]);	//lint !e796 !e1776
		if (verbose)
			outf << "% " << d_argv[i] << endl;
		if (strcmp(d_argv[i], "-startdepth") == 0) {
			i++;
			if (i >= d_argc) {
				errf << "-startdepth requires a depth in the range of 0-999" << endl;
				show_usage_line = true;
			} else {
				assert(d_argv && d_argv[i]);	//lint !e796 !e1776
				startdepth = (int) atoi(d_argv[i]);
			}
			//            } else if (strcmp(d_argv[i],"-f31") == 0) {
			//                        format = 31;
		} else if (strcmp(d_argv[i], "-depth") == 0) {
			i++;
			if (i >= d_argc) {
				errf << "-depth requires a depth in inches" << endl;
				show_usage_line = true;
			} else {
				assert(d_argv && d_argv[i]);	//lint !e796 !e1776
				depth_in_inches = (float) atof(d_argv[i]);
			}
		} else if (strcmp(d_argv[i], "-help") == 0) {
			errf << "-help    Show this message" << endl;
			errf << "-depth # Set the page depth in inches" << endl;
			errf << "-startdepth # Set the initial depth (default 999)" << endl;
			show_usage_line = true;
		} else {
			errf << "Unknown fig driver option: " << d_argv[i] << endl;
			show_usage_line = true;
		}
	}

	if (show_usage_line) {
		errf << "Usage -f 'fig: [-help] [-depth #] [-startdepth #]'" << endl;
	}
	// Set the papersize
	if (!paper_size)
		paper_size = (depth_in_inches <= 11.0 ? "Letter" : "A4");

	// set FIG specific values
	scale = 1;
	currentDeviceHeight = depth_in_inches * 1200.0f * scale;
	// We use objectId as depth value.
	// We need this because editing will reorder objects of equal depth,
	// which has an undesireable effect if objects overlap.
	// Depth must be in the range 0..999 (FIG 3.1).
	// If we have more than 1000 objects this will get negative and
	// xfig will set negative depth values to zero.
	// This feature will thus become useless if we have more
	// than 1000 objects. This is an xfig limitation.
	objectId = startdepth + 1;	// +1 because a predecrement is done when used

	x_offset = 0.0;
	y_offset = currentDeviceHeight;
	// output buffer, needed because
	// color entries must be written at
	// top of output file, but are known
	// only after processing the full input

	// print the header part
	outf << "#FIG 3.2\nPortrait\nFlush left\nInches\n" << paper_size <<
		"\n100.00\nSingle\n0\n1200 2\n";
}

drvFIG::~drvFIG()
{
	dumpnewcolors(outf);
	// now we can copy the buffer the output
	ifstream & inbuffer = tempFile.asInput();
	copy_file(inbuffer, outf);
}


unsigned int
 drvFIG::nrOfCurvetos() const
{
	unsigned int nr = 0;
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		if (elem.getType() == curveto)
			nr++;
	}
	return nr;
}

void drvFIG::prpoint(ostream & os, const Point & p, bool withspaceatend) const
{
	os << (int) (PntFig * p.x_) << " " << (int) (y_offset - (PntFig * p.y_));
	if (withspaceatend)
		os << " ";
}


 // static int nd_calls=0; Debug

// bounding box
void drvFIG::new_depth()
{
// errf <<  "Called #" << nd_calls++ << "\n"; Debug
	if (glo_bbox_flag == 0) {
		glob_max_y = loc_max_y;
		glob_min_y = loc_min_y;
		glob_max_x = loc_max_x;
		glob_min_x = loc_min_x;
		glo_bbox_flag = 1;
	} else {
		if ((loc_max_y <= glob_min_y) ||
			(loc_min_y >= glob_max_y) || (loc_max_x <= glob_min_x) || (loc_min_x >= glob_max_x)) {
// outside global bounds, increase global box
			if (loc_max_y > glob_max_y)
				glob_max_y = loc_max_y;
			if (loc_min_y < glob_min_y)
				glob_min_y = loc_min_y;
			if (loc_max_x > glob_max_x)
				glob_max_x = loc_max_x;
			if (loc_min_x < glob_min_x)
				glob_min_x = loc_min_x;
		} else {
// inside global bounds, decrease depth and create new bounds
//      errf << " Overlap detected\n" 
//          << " Local " << loc_min_x << ", " << loc_min_y
//          << ", " << loc_max_x << ", " << loc_max_y << "\n"
//          << "Global " << glob_min_x << ", " << glob_min_y 
//          << ", " << glob_max_x << ", " << glob_max_y << "\n"; Debug
			glob_max_y = loc_max_y;
			glob_min_y = loc_min_y;
			glob_max_x = loc_max_x;
			glob_min_x = loc_min_x;
			if (objectId)
				objectId--;		// don't let it get < 0
//      errf << " Depth increased " << objectId << "\n"; Debug
		}
	}
	loc_bbox_flag = 0;
}

void drvFIG::addtobbox(const Point & p)
{
	if (loc_bbox_flag == 0) {
		loc_max_y = p.y_;
		loc_min_y = p.y_;
		loc_max_x = p.x_;
		loc_min_x = p.x_;
		loc_bbox_flag = 1;
	} else {
		if (loc_max_y < p.y_)
			loc_max_y = p.y_;
		if (loc_min_y > p.y_)
			loc_min_y = p.y_;
		if (loc_max_x < p.x_)
			loc_max_x = p.x_;
		if (loc_min_x > p.x_)
			loc_min_x = p.x_;
	}
}


void drvFIG::print_polyline_coords()
{
	int j = 0;
	//  const Point & p;
	unsigned int last = numberOfElementsInPath() - 1;
	for (unsigned int n = 0; n <= last; n++) {
		const basedrawingelement & elem = pathElement(n);
		if (j == 0) {
			buffer << "\t";
		}
		switch (elem.getType()) {
		case lineto:
		case moveto:
			{
				const Point & p = pathElement(n).getPoint(0);
				prpoint(buffer, p, (n != last));
			}
			break;
		case closepath:
			{
				const Point & p = pathElement(0).getPoint(0);
				prpoint(buffer, p, (n != last));
			}
			break;
		case curveto:
		default:
			errf << "\t\tFatal: unexpected case in drvfig " << endl;
			abort();
			break;

		}

		j++;
		if (j == 5) {
			j = 0;
			buffer << "\n";
		}
	}
	if (j != 0) {
		buffer << "\n";
	}
}

// print edge points
void drvFIG::print_spline_coords1()
{
// IJMP - need curr_point
	Point P1;
	int j = 0;
	unsigned int last = numberOfElementsInPath() - 1;
	for (unsigned int n = 0; n <= last; n++) {
		if (j == 0) {
			buffer << "\t";
		}
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:
		case lineto:
			{
				const Point & p = elem.getPoint(0);
				prpoint(buffer, p, (n != last));
				P1 = p;
			}
			j++;
			if (j == 5) {
				j = 0;
				buffer << "\n";
			}
			break;
		case curveto:
			{
// IJMP - change to quintic spline
				// put all points, middle points will have control value 1
// IJMP = do bezier fit
				
				const Point & P2 = elem.getPoint(0);
				const Point & P3 = elem.getPoint(1);
				const Point & P4 = elem.getPoint(2);

				for (int cp = 1; cp <= 5; cp++) {
					const Point p = PointOnBezier((cp * 0.2f), P1, P2, P3, P4);
					// p.x_ = bezpnt((cp * 0.2f), P1.x_, P2.x_, P3.x_, P4.x_);
					// p.y_ = bezpnt((cp * 0.2f), P1.y_, P2.y_, P3.y_, P4.y_);
					prpoint(buffer, p, !((n == last) && (cp == 5)));
					j++;
					if (j == 5) {
						j = 0;
						buffer << "\n";
					}
//                      if ((j == 0) && (cp != 5) && ( n+1 != (numberOfElementsInPath()))) { buffer << "\t"; }
					if ((j == 0) && (n != (numberOfElementsInPath()))) {
						buffer << "\t";
					}
				}
				P1 = P4;
			}
			break;
		case closepath:
			{
				const Point & p = pathElement(0).getPoint(0);
				P1 = p;
				prpoint(buffer, p, (n != last));
			}
			j++;
			if (j == 5) {
				j = 0;
				buffer << "\n";
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvfig " << endl;
			abort();
			break;
		}
	}
	if (j != 0) {
		buffer << "\n";
	}
	buffer << "\t";
}

// print control points
void drvFIG::print_spline_coords2()
{
	int j = 0;
	Point lastp;
	int maxj = 8;

	unsigned int last = numberOfElementsInPath() - 1;
	for (unsigned int n = 0; n <= last; n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:
			{
				buffer << " 0";
				if (n != last)
					buffer << " ";
				j++;
				if (j == maxj) {
					j = 0;
					buffer << "\n";
					if ((n + 1) != numberOfElementsInPath()) {
						buffer << "\t";
					}
				}
				lastp = elem.getPoint(0);
			}
			break;
		case lineto:
			{
				buffer << " 0";
				if (n != last)
					buffer << " ";
				j++;
				if (j == maxj) {
					j = 0;
					buffer << "\n";
					if ((n + 1) != numberOfElementsInPath()) {
						buffer << "\t";
					}
				}
			}
			break;
		case closepath:
			{
				buffer << " 0";
				if (n != last)
					buffer << " ";
				j++;
				if (j == maxj) {
					j = 0;
					buffer << "\n";
					if ((n + 1) != numberOfElementsInPath()) {
						buffer << "\t";
					}
				}
			}
			break;
		case curveto:
			{
// IJMP - change to quintic spline
				// put all points, middle points will have control value -1
				float kp = 0.0;
				for (unsigned int i = 0; i < 5; i++) {
					if (i == 1) {
						kp = -1.0; //lint !e736
					}
					if (i == 4) {
						kp = 0.0;
					}
					buffer << " " << kp;
					if (!((n == last) && (i == 4)))
						buffer << " ";
					j++;
					if (j == maxj) {
						j = 0;
						buffer << "\n";
						if (!((i == 4)
							  && ((n + 1) == numberOfElementsInPath()))) {
							buffer << "\t";
						}
					}
				}
				lastp = elem.getPoint(2);
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvfig " << endl;
			abort();
			break;
		}
	}
	if (j != 0) {
		buffer << endl;
	}
}

void drvFIG::close_page()
{
// Well, since FIG doesn't support multipage output, we'll move down
// 11 inches and start again.
	y_offset += currentDeviceHeight;
	// reset depth counter
	objectId = startdepth + 1;	// also changed to 499
}

void drvFIG::open_page()
{
}

	//
	// FIG 3.1 uses an index for the popular fonts:
	//
	// (we cannot make this array local to drvFIG::show_textstring
	// because some CCs don't support that yet.

static const char * const FigFonts[] = {
	"Times-Roman", "Times-Italic", "Times-Bold", "Times-BoldItalic",
	"AvantGarde-Book", "AvantGarde-BookOblique", "AvantGarde-Demi",
	"AvantGarde-DemiOblique", "Bookman-Light", "Bookman-LightItalic",
	"Bookman-Demi", "Bookman-DemiItalic", "Courier", "Courier-Oblique",
	"Courier-Bold", "Courier-BoldOblique", "Helvetica",
	"Helvetica-Oblique",
	"Helvetica-Bold", "Helvetica-BoldOblique", "Helvetica-Narrow",
	"Helvetica-Narrow-Oblique", "Helvetica-Narrow-Bold",
	"Helvetica-Narrow-BoldOblique", "NewCenturySchlbk-Roman",
	"NewCenturySchlbk-Italic", "NewCenturySchlbk-Bold",
	"NewCenturySchlbk-BoldItalic", "Palatino-Roman",
	"Palatino-Italic", "Palatino-Bold", "Palatino-BoldItalic",
	"Symbol", "ZapfChancery-MediumItalic", "ZapfDingbats"
};

static int getfigfontnumber(const char *fname)
{
	size_t fntlength = strlen(fname);
	size_t MAXFNTNUM = sizeof(FigFonts) / (sizeof(char *)) - 1;
	for ( unsigned int i = 0; i <= MAXFNTNUM; i++) {
		if (fntlength == strlen(FigFonts[i])) {
			if (strncmp(fname, FigFonts[i], fntlength) == 0)
				return (int) i;
		}
	}
	return -1;
}


void drvFIG::show_text(const TextInfo & textinfo)
{
	const float toRadians = 3.14159265359f / 180.0f;

	int FigFontNum = getfigfontnumber(textinfo.currentFontName.value());
	if (FigFontNum == -1) {
		errf << "Warning, unsupported font " << textinfo.currentFontName.value() << ", using ";
		FigFontNum = getfigfontnumber(defaultFontName);
		if (FigFontNum != -1) {
			errf << defaultFontName;
		} else {
			if (strstr(textinfo.currentFontName.value(), "Bold") == 0) {
				if (strstr(textinfo.currentFontName.value(), "Italic") == 0) {
					errf << "Times-Roman";
					FigFontNum = 0;	// Times-Roman
				} else {
					FigFontNum = 1;
					errf << "Times-Italic";
				}
			} else {
				if (strstr(textinfo.currentFontName.value(), "Italic") == 0) {
					errf << "Times-Bold";
					FigFontNum = 2;	// Times-Bold
				} else {
					FigFontNum = 3;
					errf << "Times-BoldItalic";
				}
			}
		}
		errf << " instead." << endl;
	}

	const unsigned int color = registercolor(textinfo.currentR, textinfo.currentG,
									textinfo.currentB);
	float localFontSize = textinfo.currentFontSize;
	if (localFontSize <= 0.1) {
		localFontSize = 9;
	}
	localFontSize++;			// There appears to be a reduction for some reason
	const float FigHeight = PntFig * localFontSize;
	const float FigLength = FigHeight * strlen(textinfo.thetext.value());
	const float PSHeight = localFontSize;
	const float PSLength = PSHeight * strlen(textinfo.thetext.value());
// Calculate BBox
	if (textinfo.currentFontAngle == 0) {
		addtobbox(Point(textinfo.x, textinfo.y));
		addtobbox(Point((textinfo.x + PSLength), (textinfo.y + PSHeight)));
	} else if (textinfo.currentFontAngle == 90) {
		addtobbox(Point(textinfo.x, textinfo.y));
		addtobbox(Point((textinfo.x - PSHeight), (textinfo.y + PSLength)));
	} else if (textinfo.currentFontAngle == 180) {
		addtobbox(Point(textinfo.x, textinfo.y));
		addtobbox(Point((textinfo.x - PSLength), (textinfo.y - PSHeight)));
	} else if (textinfo.currentFontAngle == 270) {
		addtobbox(Point(textinfo.x, textinfo.y));
		addtobbox(Point((textinfo.x + PSHeight), (textinfo.y - PSLength)));
	} else {
// To simplify this, a box of width 2*PSLength centered on textinfo.(x,y) used
		addtobbox(Point((textinfo.x - PSLength), (textinfo.y + PSLength)));
		addtobbox(Point((textinfo.x + PSLength), (textinfo.y + PSLength)));
		addtobbox(Point((textinfo.x - PSLength), (textinfo.y - PSLength)));
		addtobbox(Point((textinfo.x + PSLength), (textinfo.y - PSLength)));
	}
	buffer << "# text\n";
	new_depth();
	buffer << "4 0 ";
	buffer << color;
	if (objectId)
		objectId--;				// don't let it get < 0
	buffer << " " << objectId << " -1 "
		<< FigFontNum << " "
		<< (int) localFontSize << " "
		<< textinfo.currentFontAngle * toRadians << " 4 "
		<< FigHeight << " "
		<< FigLength << " "
		<< (int) (PntFig * textinfo.x) << " "
		<< (int) (y_offset - (PntFig * textinfo.y)) << " " << textinfo.thetext.value() << "\\001\n";
}

void drvFIG::bbox_path()
{
	for (unsigned int i = 0; i < numberOfElementsInPath(); i++) {
		const basedrawingelement & elem = pathElement(i);
		switch (elem.getType()) {
		case curveto:
			{
				addtobbox(elem.getPoint(0));
				addtobbox(elem.getPoint(1));
				addtobbox(elem.getPoint(2));
				break;
			}
		case moveto:
		case lineto:
			{
				addtobbox(elem.getPoint(0));
				break;
			}
		case closepath:
		default:
			{					// will get caught later
				break;
			}
		}
	}
	new_depth();
}

void drvFIG::show_path()
{
	float localLineWidth = currentLineWidth();
	if ((localLineWidth < 0.0) || ((localLineWidth > 0.0) && (localLineWidth <= 1.0))) {
		localLineWidth = 1.0;
	}
	int linestyle = 0;
	switch (currentLineType()) {
	case solid:
		linestyle = 0;
		break;
	case dashed:
		linestyle = 1;
		break;
	case dashdot:
		linestyle = 3;
		break;
	case dotted:
		linestyle = 4;
		break;
	case dashdotdot:
		linestyle = 2;
		break;
	}
	// Calculate BBox
	bbox_path();

	unsigned int curvetos = nrOfCurvetos();
	if (curvetos == 0)			// polyline
	{
		buffer << "# polyline\n";
		buffer << "2 1 " << linestyle << " " << (int) localLineWidth << " ";
		const unsigned int color = registercolor(currentR(), currentG(), currentB());
		const int fill_or_nofill = (currentShowType() == drvbase::stroke) ? -1 : 20;
		if (objectId)
			objectId--;			// don't let it get < 0
		buffer << color << " " << color << " " << objectId << " 0 " <<
			fill_or_nofill << " " << "4.0" << " 0 0 0 0 0 ";
		// 4.0 is the gap spec. we could also derive this from the input
		buffer << (int) (numberOfElementsInPath()) << "\n";
		print_polyline_coords();
	} else						// contains at least one curveto 
	{
		buffer << "# spline\n";
		// 3 2 means "open interpolated spline"
		buffer << "3 4 " << linestyle << " " << (int) localLineWidth << " ";
		const unsigned int color = registercolor(currentR(), currentG(), currentB());
		const int fill_or_nofill = (currentShowType() == drvbase::stroke) ? -1 : 20;
		if (objectId)
			objectId--;			// don't let it get < 0
		buffer << color << " " << color << " " << objectId << " 0 " <<
			fill_or_nofill << " " << "4.0" << " 0 0 0 ";
		// 4.0 is the gap spec. we could also derive this from the input

		// IJMP - change to quintic spline - 5 pnts per spline, not 3
		buffer << numberOfElementsInPath() + 5 * curvetos - curvetos << "\n";
		print_spline_coords1();
		print_spline_coords2();

	}
}

void drvFIG::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
	// just do show_polyline for a first guess
	unused(&llx);
	unused(&lly);
	unused(&urx);
	unused(&ury);
	show_path();
}

void drvFIG::show_image(const Image & imageinfo)
{
	if (outDirName == NULL || outBaseName == NULL) {
		errf << "images cannot be handled via standard output. Use an output file " << endl;
		return;
	}

	char *EPSoutFileName = new char[strlen(outBaseName) + 21];
	char *EPSoutFullFileName = new char[strlen(outDirName) + strlen(outBaseName) + 21];

	sprintf(EPSoutFileName, "%s%02d.eps", outBaseName, imgcount++);
	sprintf(EPSoutFullFileName, "%s%s", outDirName, EPSoutFileName);
	ofstream outi(EPSoutFullFileName);
	if (!outi) {
		errf << "Could not open file " << EPSoutFullFileName << " for output";
		exit(1);
	}
	// remember, we have to flip the image from PostScript coord to fig coords
	Point ll, ur;
	imageinfo.getBoundingBox(ll, ur);
// Calculate BBox
	addtobbox(ll);
	addtobbox(ur);
	Point fig_ur(PntFig * ur.x_, y_offset - PntFig * ll.y_);
	Point fig_ll(PntFig * ll.x_, y_offset - PntFig * ur.y_);

	// first output link to an external *.eps file into *.fig file
	buffer << "# image\n";
	new_depth();
	buffer << "2 5 0 1 -1 -1 ";
	if (objectId)
		objectId--;				// don't let it get < 0
	buffer << objectId << " 0 -1 0.000 0 0 -1 0 0 5\n";
	buffer << "\t0 " << EPSoutFileName << "\n";

	buffer << "\t" << (int) fig_ll.x_ << " " << (int) fig_ll.y_ << " "
		<< (int) fig_ur.x_ << " " << (int) fig_ll.y_ << " "
		<< (int) fig_ur.x_ << " " << (int) fig_ur.y_ << " "
		<< (int) fig_ll.x_ << " " << (int) fig_ur.y_ << " "
		<< (int) fig_ll.x_ << " " << (int) fig_ll.y_;
	buffer << "\n";



	imageinfo.writeEPSImage(outi);
	outi.close();

	delete[]EPSoutFullFileName;
	delete[]EPSoutFileName;
}


static DriverDescriptionT < drvFIG > D_fig("fig", ".fig format for xfig", "fig", false, true, true, true, true, false,	// no support for PNG file images
										   DriverDescription::normalopen,
										   false, false /*clipping */ ,driveroptions);

static DriverDescriptionT < drvFIG > D_xfig("xfig", ".fig format for xfig", "fig", false, true, true, true, true, false,	// no support for PNG file images
											DriverDescription::normalopen,
											false, false /*clipping */ ,driveroptions);
 
