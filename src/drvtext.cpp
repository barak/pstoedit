/* 
   drvTEXT.cpp : This file is part of pstoedit
   Skeleton for the implementation of text based backends
   for example this could be extended towards an HTML backend.

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
#include "drvtext.h"
#include I_fstream
#include I_stdio
#include I_stdlib


drvTEXT::derivedConstructor(drvTEXT):
	constructBase
{
// driver specific initializations
// and writing of header to output file
	outf << "Sample header \n";
//	float           scale;
//	float           x_offset;
//	float           y_offset;
}

drvTEXT::~drvTEXT() {
// driver specific deallocations
// and writing of trailer to output file
	outf << "Sample trailer \n";
}

void drvTEXT::print_coords()
{
}


void drvTEXT::open_page()
{
	outf << "Opening page: " << currentPageNumber << endl;
}

void drvTEXT::close_page()
{
	outf << "Closing page: " << (currentPageNumber) << endl;
	unsigned int nroflines = page.size();
	for (unsigned int i = 0; i < nroflines; i++) {
		Line * lineptr = page[i];
		unsigned int nrofpieces = lineptr->textpieces.size();
		outf << "***********************************************" << endl;
		for (unsigned int j = 0 ; j < nrofpieces ; j++ ) {
			const TextInfo & textinfo  = lineptr->textpieces[j];
	outf << "Text String : " << textinfo.thetext.value() << endl;
	outf << '\t' << "X " << textinfo.x << " Y " << textinfo.y << endl;
	outf << '\t' << "X_END " << textinfo.x_end << " Y_END " << textinfo.y_end << endl;
	outf << '\t' << "currentFontName: " <<  textinfo.currentFontName.value() << endl;
	outf << '\t' << "is_non_standard_font: " <<  textinfo.is_non_standard_font << endl;
	outf << '\t' << "currentFontFamilyName: " << textinfo.currentFontFamilyName.value() << endl;
	outf << '\t' << "currentFontFullName: " << textinfo.currentFontFullName.value() << endl;
	outf << '\t' << "currentFontWeight: " << textinfo.currentFontWeight.value() << endl;
	outf << '\t' << "currentFontSize: " << textinfo.currentFontSize << endl;
	outf << '\t' << "currentFontAngle: " << textinfo.currentFontAngle << endl;
	outf << '\t' << "currentR: " << textinfo.currentR << endl;
	outf << '\t' << "currentG: " << textinfo.currentG << endl;
	outf << '\t' << "currentB: " << textinfo.currentB << endl;
			
		}
	}
	for (unsigned int ii = 0; ii < nroflines; ii++) {
		Line * lineptr = page[ii];
		delete lineptr;
	}
	page.clear();
}

void drvTEXT::show_text(const TextInfo & textinfo)
{
	// check which line this piece of text fits in.
	//
	unsigned int nroflines = page.size();
	bool inserted = false;
	for (unsigned int i = 0; i < nroflines; i++) {
		if ((textinfo.y <= page[i]->y_max) && (textinfo.y >= page[i]->y_min))
		{
			page[i]->textpieces.insert(textinfo);
			inserted = true;
			break;
		}
	}
	if (!inserted) {
		Line * newline = new Line;
		page.insert(newline);
		newline->y_max = textinfo.y + 0.1f * textinfo.currentFontSize;
		newline->y_min = textinfo.y - 0.1f * textinfo.currentFontSize;
		newline->textpieces.insert(textinfo);
	}
#if 0
	outf << "Text String : " << textinfo.thetext.value() << endl;
	outf << '\t' << "X " << textinfo.x << " Y " << textinfo.y << endl;
	outf << '\t' << "currentFontName: " <<  textinfo.currentFontName.value() << endl;
	outf << '\t' << "is_non_standard_font: " <<  textinfo.is_non_standard_font << endl;
	outf << '\t' << "currentFontFamilyName: " << textinfo.currentFontFamilyName.value() << endl;
	outf << '\t' << "currentFontFullName: " << textinfo.currentFontFullName.value() << endl;
	outf << '\t' << "currentFontWeight: " << textinfo.currentFontWeight.value() << endl;
	outf << '\t' << "currentFontSize: " << textinfo.currentFontSize << endl;
	outf << '\t' << "currentFontAngle: " << textinfo.currentFontAngle << endl;
	outf << '\t' << "currentR: " << textinfo.currentR << endl;
	outf << '\t' << "currentG: " << textinfo.currentG << endl;
	outf << '\t' << "currentB: " << textinfo.currentB << endl;
#endif
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

#if 0
outf << "Image:\n";
outf << "\ttype: ";
	switch (imageinfo.type) {
	case colorimage : outf << " colorimage\n" ; break;
	case normalimage : outf << " normalimage\n" ; break;
	case imagemask : outf << " imagemask\n" ; break;
	default: break;
	}
outf << "\theight: " << imageinfo.height << endl;
outf << "\twidth: " << imageinfo.width << endl;
outf << "\tbits/component: " << imageinfo.bits << endl;
outf << "\tnumber of color components: " << imageinfo.ncomp << endl;
outf << "\timageMatrix: ";
{ for (unsigned int i = 0; i < 6 ; i++ ) {
	outf << imageinfo.imageMatrix[i] << ' ';
	}
}
outf << endl;
outf << "\tnormalizedImageCurrentMatrix: ";
{ for (unsigned int i = 0; i < 6 ; i++ ) {
	outf << imageinfo.normalizedImageCurrentMatrix[i] << ' ';
	}
}
outf << endl;
outf << "\tpolarity: " << imageinfo.polarity << endl;
{ 
	for  (unsigned int i = 0 ; i < imageinfo.nextfreedataitem ; i++) {
		if (!(i%8)) outf << "\n\t " << i << ":\t";
		outf << imageinfo.data[i] << ' '; 
	}
}
outf << endl;
#endif

}

static DriverDescriptionT<drvTEXT> D_text(
		"text","text in different forms ","txt",
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

		false, // if backend supports curves, else 0
		false, // if backend supports elements with fill and edges
		true, // if backend supports text, else 0
		false, // if backend supports Images
		DriverDescription::normalopen,
		true); // if format supports multiple pages in one file

 
