/* 
   drvTEXT.cpp : This file is part of pstoedit
   Skeleton for the implementation of text based backends
   for example this could be extended towards an HTML backend.

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
#include "drvtext.h"
#include I_fstream
#include I_stdio
#include I_stdlib

// sizes in terms of lines and characters


static const OptionDescription driveroptions[] = {
	OptionDescription("-height", "number", "page height in terms of characters"),
	OptionDescription("-width", "number", "page width in terms of characters"),
	OptionDescription("-dump", 0, "dump text pieces"),
	endofoptions
};



drvTEXT::derivedConstructor(drvTEXT):
constructBase, charpage(0), dumptextpieces(false), pageheight(200), pagewidth(120)
{
// driver specific initializations

	for (unsigned int i = 0; i < d_argc; i++) {
		assert(d_argv && d_argv[i]);
		if (verbose)
			errf << "% " << d_argv[i] << endl;
		if (strcmp(d_argv[i], "-dump") == 0) {
			dumptextpieces = true;
		} else if (strcmp(d_argv[i], "-height") == 0) {
			pageheight = atoi(d_argv[i + 1]);
			i++;
		} else if (strcmp(d_argv[i], "-width") == 0) {
			pagewidth = atoi(d_argv[i + 1]);
			i++;
		}
	}


	if (!dumptextpieces) {
#if 0
		charpage = new char[pageheight * pagewidth];
		for (unsigned int ii = 0; ii < pageheight; ii++) {
			//charpage[i] = new char[pagewidth];
			for (unsigned int j = 0; j < pageheight; j++)
				(charpage[ii])[j] = ' ';
		}
#else
		charpage = new char *[pageheight];
		for (unsigned int ii = 0; ii < pageheight; ii++) {
			charpage[ii] = new char[pagewidth];
			for (unsigned int j = 0; j < pagewidth; j++)
				(charpage[ii])[j] = ' ';
		}
#endif
	}
// and writing of header to output file
//  outf << "Sample header \n";
//  float           scale;
//  float           x_offset;
//  float           y_offset;
}

drvTEXT::~drvTEXT()
{
// driver specific deallocations
// and writing of trailer to output file
	if (dumptextpieces) {
		outf << "Sample trailer \n";
	} else {
		if (charpage) {
			for (unsigned int i = 0; i < pageheight; i++) {
				delete[]charpage[i];
				charpage[i] = 0;
			}
			delete[]charpage;
			charpage = 0;
		}
	}
}

void drvTEXT::open_page()
{
	if (dumptextpieces)
		outf << "Opening page: " << currentPageNumber << endl;
}

void drvTEXT::close_page()
{
	if (dumptextpieces) {
		outf << "Closing page: " << (currentPageNumber) << endl;
		unsigned int nroflines = page.size();
		for (unsigned int i = 0; i < nroflines; i++) {
			Line *lineptr = page[i];
			unsigned int nrofpieces = lineptr->textpieces.size();
			outf << "***********************************************" << endl;
			for (unsigned int j = 0; j < nrofpieces; j++) {
				const TextInfo & textinfo = lineptr->textpieces[j];
				outf << "Text String : " << textinfo.thetext.value() << endl;
				outf << '\t' << "X " << textinfo.x << " Y " << textinfo.y << endl;
				outf << '\t' << "X_END " << textinfo.x_end << " Y_END " << textinfo.y_end << endl;
				outf << '\t' << "currentFontName: " << textinfo.currentFontName.value() << endl;
				outf << '\t' << "is_non_standard_font: " << textinfo.is_non_standard_font << endl;
				outf << '\t' << "currentFontFamilyName: " << textinfo.
					currentFontFamilyName.value() << endl;
				outf << '\t' << "currentFontFullName: " << textinfo.currentFontFullName.
					value() << endl;
				outf << '\t' << "currentFontWeight: " << textinfo.currentFontWeight.value() << endl;
				outf << '\t' << "currentFontSize: " << textinfo.currentFontSize << endl;
				outf << '\t' << "currentFontAngle: " << textinfo.currentFontAngle << endl;
				outf << '\t' << "currentR: " << textinfo.currentR << endl;
				outf << '\t' << "currentG: " << textinfo.currentG << endl;
				outf << '\t' << "currentB: " << textinfo.currentB << endl;

			}
		}
		for (unsigned int ii = 0; ii < nroflines; ii++) {
			Line *lineptr = page[ii];
			delete lineptr;
		}
		page.clear();
	} else {
		for (unsigned int i = 0; i < pageheight; i++) {
			for (unsigned int j = 0; j < pagewidth; j++) {
				outf << (charpage[i])[j];
				(charpage[i])[j] = ' ';
			}
			outf << endl;
		}

	}
}

void drvTEXT::show_text(const TextInfo & textinfo)
{
	if (dumptextpieces) {
		// check which line this piece of text fits in.
		//
		unsigned int nroflines = page.size();
		bool inserted = false;
		for (unsigned int i = 0; i < nroflines; i++) {
			if ((textinfo.y <= page[i]->y_max)
				&& (textinfo.y >= page[i]->y_min)) {
				page[i]->textpieces.insert(textinfo);
				inserted = true;
				break;
			}
		}
		if (!inserted) {
			Line *newline = new Line;
			page.insert(newline);
			newline->y_max = textinfo.y + 0.1f * textinfo.currentFontSize;
			newline->y_min = textinfo.y - 0.1f * textinfo.currentFontSize;
			newline->textpieces.insert(textinfo);
		}
	} else {
		int x = (int) (pagewidth * (textinfo.x / 700.0f));
		int y = (int) (pageheight * ((currentDeviceHeight + y_offset - textinfo.y) / 800.0f));
		if ((x >= 0) && (y >= 0) && (x < pagewidth) && (y < pageheight)) {
			if (((charpage[y])[x] != ' ')) {
				cerr << "character " << (charpage[y])[x] << " overwritten with " << textinfo.
					thetext.
					value()[0] << " at " << x << " " << y <<
					" - Hint increase -width and/or -height" << endl;
			}
			(charpage[y])[x] = textinfo.thetext.value()[0];
		} else {
			cerr << "seems to be off-page: " << textinfo.thetext.value()[0] << endl;
			cerr << x << " " << y << " " << textinfo.x << " " << textinfo.y << endl;
		}
	}
}

void drvTEXT::show_path()
{
}

void drvTEXT::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
	unused(&llx);
	unused(&lly);
	unused(&urx);
	unused(&ury);
}

void drvTEXT::show_image(const Image & imageinfo)
{
	unused(&imageinfo);
}

static DriverDescriptionT < drvTEXT > D_text("text", "text in different forms ", "txt", false,	// if backend supports subpathes, else 0
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
											 false,	// if backend supports curves, else 0
											 false,	// if backend supports elements with fill and edges
											 true,	// if backend supports text, else 0
											 false,	// if backend supports Images
											 false,	// no support for PNG file images
											 DriverDescription::normalopen, true,	// if format supports multiple pages in one file
											 false,	/*clipping */
											 driveroptions);
 
