/* 
   drvTGIF.cpp : This file is part of pstoedit
   Backend for TGIF

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

#include "drvtgif.h"
#include I_fstream
#include I_stdio
#include I_stdlib

// for sin and cos
#include <math.h>


static const char * colorstring(float r, float g, float b)
{
static char buffer[10];
	sprintf(buffer,"%s%.2x%.2x%.2x","#", (unsigned int) (r * 255), (unsigned int) ( g * 255) ,  (unsigned int) (b * 255));
	return buffer;
}

const float tgifscale = 128.0f / 72.0f;
drvTGIF::derivedConstructor(drvTGIF):
	constructBase,
    buffer(tempFile.asOutput()),
	objectId            (1),
	textAsAttribute(false)
//	magnification(1.0f) // scalefactor_p /* 1.0f */)
{
//	magnification(theMagnification)
    // set tgif specific values
    scale = tgifscale * scale ;		/* tgif internal scale factor */
    currentDeviceHeight = 792.0f * scale;

    x_offset = 0.0f;		/* set to fit to tgif page    */
    y_offset = 89.61f;		/*          "                 */

    // cannot write any header part, since we need the total number of pages
    // in the header
    if (verbose) errf << "% Driver options:" << endl;
	for (unsigned int i = 0; i < d_argc ; i++ ) {
		if (verbose) errf << "% " << d_argv[i] << endl; 
		if (strcmp(d_argv[i],"-ta") == 0) {
			textAsAttribute = true;
		}
	}
}

drvTGIF::~drvTGIF() 
{
    	outf << "state(0,33," <<  100 // (int) ((scale / tgifscale )* 100) 
	 << ",0,0,1,16,1,9,1,1,0,0,1,0,1,0,'Courier',0,17,0,0,1,5,0,0,1,1,0,16,1,0,1," << currentPageNumber << ",1,0,1056,1497,0,0,2880)." << endl;
	outf << "unit(\"1 pixel/pixel\")." << endl;
	// now we can copy the buffer the output
	ifstream & inbuffer = tempFile.asInput();
	copy_file(inbuffer,outf);
}

void drvTGIF::print_coords()
{
    for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
    const basedrawingelement & elem = pathElement(n);
    switch(elem.getType()) {
        case lineto:
        case moveto: {
            const Point & p = pathElement(n).getPoint(0);
    	buffer << p.x_ + x_offset ;
	buffer << ',' << currentDeviceHeight - p.y_ + y_offset;
	if (n != numberOfElementsInPath() - 1 ) {
		// it is not the last point, so add a ,
		buffer << ','; 
	}
	if ((!((n + 1) % 8)) && ((n + 1) != numberOfElementsInPath())) {
	    buffer << "\n\t";
	}
            }
            break;
        case closepath: {
            const Point & p = pathElement(0).getPoint(0);
    	buffer << p.x_ + x_offset ;
	buffer << ',' << currentDeviceHeight - p.y_ + y_offset;
	if (n != numberOfElementsInPath() - 1 ) {
		// it is not the last point, so add a ,
		buffer << ','; 
	}
	if ((!((n + 1) % 8)) && ((n + 1) != numberOfElementsInPath())) {
	    buffer << "\n\t";
	}
            }
            break;
        default:
	    errf << "\t\tFatal: unexpected case in drvtgif " << endl;
 	    abort();
	    break;

    }
    }
}
void drvTGIF::close_page()
{
}

void drvTGIF::open_page()
{
    buffer << "page(" << currentPageNumber << ",\"\",1).\n";
}

static const int Fill   = 1;
static const int noFill = 0;
void drvTGIF::show_text(const TextInfo & textinfo)
{
    if (textAsAttribute) {
//	buffer << "box('magenta',150,50,250,100,0,1,1,22,0,0,0,0,0,'1',[\n"
    	buffer << "box('" <<  colorstring(textinfo.currentR,textinfo.currentG,textinfo.currentB) << "'";
	buffer << "," << (textinfo.x) + x_offset; // llx
	buffer << "," <<  currentDeviceHeight - (textinfo.y_end) + y_offset  - textinfo.currentFontSize; // lly
    	buffer << "," <<  textinfo.x_end + x_offset; //urx
	buffer << "," <<  currentDeviceHeight - textinfo.y + y_offset; //ury
	const int fillpat =  noFill;
	buffer << ","
	 << fillpat
	 << ","
	 << 1 // currentLineWidth() 
	 << ","
	 << Fill
	 << ","
	 << objectId++
	 << ",0,0,0,0,0,'1',[\n"

	 << "attr(\"href=\", \"";
    	const char * cp = textinfo.thetext.value();
    	while (*cp) {
		if ( (*cp  == '"') || (*cp == '\\') ) {
			buffer << '\\';
		}
		buffer << *cp ;
		cp++;
    	}
	buffer << "\", 1, 1, 0,\n";
    }
    buffer << "text('" <<  colorstring(textinfo.currentR,textinfo.currentG,textinfo.currentB) << "'";
    buffer << "," << textinfo.x + x_offset;
    buffer << "," << currentDeviceHeight - textinfo.y + y_offset - textinfo.currentFontSize;
    // TGIF's origin of text is at the top line, pstoedit's at the bottom
    buffer << ",'" << textinfo.currentFontName.value() << "'";
    const bool boldfont   =  (strstr(textinfo.currentFontName.value(),"Bold") != NULL);
    const bool italicfont = ((strstr(textinfo.currentFontName.value(),"Italic") != NULL) ||
		      (strstr(textinfo.currentFontName.value(),"Oblique") != NULL));
    int fonttype = 0;
    if (boldfont) {
	if (italicfont) fonttype = 3;
	else            fonttype = 1;
    } else {
	if (italicfont) fonttype = 2;
	else            fonttype = 0;
    }
    buffer << "," << fonttype <<  "," << (int) (textinfo.currentFontSize + 0.5) << ",1,0,0,1,70," << textinfo.currentFontSize << "," <<  objectId++ << ",0,14,3,0,0,0,0,0,0,0,0,0,0,\"\",0,";

    const bool rotated = textinfo.currentFontAngle != 0.0;
    if (rotated) {
    	buffer << "1,0,[" << endl;
    } else {
    	buffer << "0,0,[" << endl;
    }
    if (rotated) {
	const float toRadians = 3.14159265359f / 180.0f;
	const float angleInRadians = textinfo.currentFontAngle * toRadians;
	const float cosphi = (float) cos(angleInRadians);
	const float sinphi = (float) sin(angleInRadians);
//	const float Sx = textinfo.currentFontSize;
//	const float Sy = textinfo.currentFontSize;
//	const int len = strlen(textinfo.thetext.value());
	buffer << '\t'<< textinfo.x + x_offset;
	buffer << "," << currentDeviceHeight - textinfo.y + y_offset;

	// the obbox stuff
#ifdef OLDTGIF
	buffer << "," << textinfo.x + x_offset  ;
	buffer << "," << currentDeviceHeight - textinfo.y + y_offset;
	buffer << "," << textinfo.x + x_offset + len * textinfo.currentFontSize ;
	buffer << "," << currentDeviceHeight - textinfo.y + textinfo.currentFontSize + y_offset;
#else
	// starting with tgif 3.0 pl 7. an all 0 bounding box causes tgif to recalculate it
	buffer << "," << 0;
	buffer << "," << 0;
	buffer << "," << 0;
	buffer << "," << 0;
#endif

	// the CTM stuff
	buffer << "," << cosphi * 1000 ;
	buffer << "," << -sinphi * 1000 ;
	buffer << "," << sinphi * 1000 ;
	buffer << "," << cosphi * 1000 ;
	buffer << "," << 0 ; // no translate
	buffer << "," << 0 ; // no translate

	// the bbox stuff
#ifdef OLDTGIF
	buffer << "," << textinfo.x + x_offset  ;
	buffer << "," << currentDeviceHeight - textinfo.y + y_offset;
	buffer << "," << textinfo.x + x_offset + len * textinfo.currentFontSize ;
	buffer << "," << currentDeviceHeight - textinfo.y + y_offset  + textinfo.currentFontSize + y_offset ;
#else
	// starting with tgif 3.0 pl 7. an all 0 bounding box causes tgif to recalculate it
	buffer << "," << 0;
	buffer << "," << 0;
	buffer << "," << 0;
	buffer << "," << 0;
#endif

	buffer << "],[" << endl;
    } 
    buffer <<  "\t\"" ;
    const char * cp = textinfo.thetext.value();
    while (*cp) {
	if ( (*cp  == '"') || (*cp == '\\') ) {
		buffer << '\\';
	}
	buffer << *cp ;
	cp++;
    }
    if (textAsAttribute) {
	    buffer << "\"]))\n]).\n";
    } else {
	    buffer << "\"]).\n";
    }
}

void drvTGIF::show_path()
{
//     buffer << "# Path " << currentNr() << " " << numberOfElementsInPath() << endl;
    const int fillpat = (currentShowType() == drvbase::stroke) ? noFill : Fill;
    if (isPolygon()) {
    buffer << "polygon('"
	 << colorstring(currentR(),currentG(),currentB())
	 << "',"
	 << numberOfElementsInPath()
	 << ",["
	 << endl
	 << "\t";
    print_coords();
    buffer << "],"
	 << fillpat
	 << ","
	 << currentLineWidth()
	 << ","
	 << Fill
	 << ",0,"
	 << objectId++
	 << ",0,0,0,0,0,'";
//	 << ",0,0,0,0,0,'1"; // bug fix 
/*	I think the 1 at the end has to be deleted.
         << ",0,0,0,0,0,'";

This error resulted for polygons in lines that where too wide.
This was however not visible in tgif, because (i think) tgif
doesn't allow lines to be wider than 7.
It became visible after printing in tgif to postscript or eps.
*/
    buffer << (int) (currentLineWidth() +0.5);
    buffer << "',\n    \"";
	 for (unsigned int i = 0 ; i < numberOfElementsInPath() ; i = i + 4) {
		if ( (i > 0) && ((i) % 256) == 0) {
			buffer << "\n     " ;
		}
		buffer << '0';
	 }
    buffer << "\",["
	 << endl
	 << "])."
	 << endl;
    } else {
    buffer << "poly('"
	 << colorstring(currentR(),currentG(),currentB())
	 << "'," 
	 << numberOfElementsInPath()
	 << ",[" 
	 << endl
	 << "\t";
    print_coords();
    buffer << "],0," 
	 << currentLineWidth()
	 << ","
         << Fill 
	 << ","
         << objectId++
         << ",0,"
         << fillpat
         << ",0,0,0,3,0,0,0,'";
    buffer << (int) (currentLineWidth() +0.5);
    buffer <<	"','8','3',\n    \"";
	 for (unsigned int i = 0 ; i < numberOfElementsInPath() ; i = i + 4) {
		if ( (i > 0) && ((i) % 256) == 0) {
			buffer << "\n     " ;
		}
		buffer << '0';
	 }
    buffer << "\",[" 
	 << endl
         << "])." 
	 << endl;
    }
};

void drvTGIF::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
    buffer << "box('" <<  colorstring(currentR(),currentG(),currentB()) << "'";
    buffer << "," << llx + x_offset;
    buffer << "," <<  currentDeviceHeight - lly + y_offset;
    buffer << "," <<  urx + x_offset;
    buffer << "," <<  currentDeviceHeight - ury + y_offset;
    const int fillpat = (currentShowType() == drvbase::stroke) ? noFill : Fill;
    buffer << ","
	 << fillpat
	 << ","
	 << currentLineWidth() 
	 << ","
	 << Fill
	 << ","
	 << objectId++
	 << ",0,0,0,0,0,'1',["
	 << endl
	 << "])."
	 << endl;
}

static DriverDescriptionT<drvTGIF> D_tgif(
		"tgif","Tgif .obj format (for tgif version >= 3)","obj",
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
		true, // if backend supports elements with fill and edges
		true, // if backend supports text, else 0
		false, // if backend supports Images
		DriverDescription::normalopen,
		true); // if format supports multiple pages in one file
 
 
