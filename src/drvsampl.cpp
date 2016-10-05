/* 
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
#include "drvsampl.h"
#include I_fstream
#include I_stdio
#include I_stdlib


drvSAMPL::derivedConstructor(drvSAMPL):
//(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream): // Constructor
	constructBase
{
// driver specific initializations
// and writing of header to output file
	outf << "Sample header \n";
//	float           scale;
//	float           x_offset;
//	float           y_offset;
}

drvSAMPL::~drvSAMPL() {
// driver specific deallocations
// and writing of trailer to output file
	outf << "Sample trailer \n";
}

void drvSAMPL::print_coords()
{
    for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
	const basedrawingelement & elem = pathElement(n);
	switch (elem.getType()) {
		case moveto: {
			const Point & p = elem.getPoint(0);
			outf << "\t\tmoveto ";
        		outf  << p.x_ + x_offset << " " 
	     			<<  /*   currentDeviceHeight -  */   p.y_ + y_offset << " " ;
			}
			break;
		case lineto: {
			const Point & p = elem.getPoint(0);
			outf << "\t\tlineto ";
        		outf  << p.x_ + x_offset << " " 
	     			<<  /*   currentDeviceHeight -  */   p.y_ + y_offset << " " ;
			}
			break;
		case closepath: 
			outf << "\t\tclosepath ";
			break;
		case curveto:{
			outf << "\t\tcurveto " ;
			for (unsigned int cp = 0 ; cp < 3; cp++ ) {
				const Point & p = elem.getPoint(cp);
        			outf  << (p.x_ + x_offset) << " " 
	     				<<  /*   currentDeviceHeight -  */   (p.y_ + y_offset) << " " ;
			}
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvpdf " << endl;
			abort();
			break;
	}
	outf << endl;
    }
}


void drvSAMPL::open_page()
{
	outf << "Opening page: " << currentPageNumber << endl;
}

void drvSAMPL::close_page()
{
	outf << "Closing page: " << (currentPageNumber) << endl;
}

void drvSAMPL::show_text(const TextInfo & textinfo)
{
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

void drvSAMPL::show_path()
{
	outf << "Path # " << currentNr() ;
    	if (isPolygon()) outf << " (polygon): "  << endl;
		else   outf << " (polyline): " << endl;
	outf << "\tcurrentShowType: ";
    	switch (currentShowType() ) {
    	case drvbase::stroke : 
		outf << "stroked";
      		break;
    	case drvbase::fill :
		outf << "filled";
      		break;
    	case drvbase::eofill :
		outf << "eofilled";
      		break;
    	default: 
	// cannot happen
      		outf << "unexpected ShowType " << (int) currentShowType() ;
      		break;
    	}
	outf << endl;
	if (currentShowType() == drvbase::fill)   outf << "\tcurrentShowType: filled " << endl;
	if (currentShowType() == drvbase::eofill) outf << "\tcurrentShowType: eofilled " << endl;
	outf << "\tcurrentLineWidth: " <<  currentLineWidth() << endl;
	outf << "\tcurrentR: " << currentR() << endl;
	outf << "\tcurrentG: " << currentG() << endl;
	outf << "\tcurrentB: " << currentB() << endl;
	outf << "\tcurrentLineCap: " << currentLineCap() << endl;
	outf << "\tdashPattern: " << dashPattern() << endl;
	outf << "\tPath Elements 0 to "  <<  numberOfElementsInPath()-1  << endl;
	print_coords();

};

void drvSAMPL::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
	outf << "Rectangle ( " << llx << "," << lly << ") (" << urx << "," << ury << ")" << endl;
// just do show_path for a first guess
	show_path();
}
void drvSAMPL::show_image(const Image & imageinfo)
{

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
		outf << (int) imageinfo.data[i] << ' '; 
	}
}
outf << endl;
}

static DriverDescriptionT<drvSAMPL> D_sampl("sample","sample driver: if you don't want to see this, uncomment the corresponding line in makefile and make again","sam",
								 
		true, // backend supports subpathes
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

		true, // backend supports curves
		false, // backend supports elements which are filled and have edges 
		true, // backend supports text
		true, // backend supports Images
		DriverDescription::normalopen,
		true); // if format supports multiple pages in one file

 
