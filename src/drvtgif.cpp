/* 
   drvTGIF.cpp : This file is part of pstoedit
   Backend for TGIF

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

#include "drvtgif.h"
#include <iostream.h>
#include <stdio.h>
// rcw2: work round case insensitivity in RiscOS
#ifdef riscos
  #include "unix:string.h"
#else
  #include <string.h>
#endif

// for sin and cos
#include <math.h>


static const char * colorstring(float r, float g, float b)
{
static char buffer[10];
	sprintf(buffer,"%s%.2x%.2x%.2x","#", (int) (r * 255), (int) ( g * 255) ,  (int) (b * 255));
	return buffer;
}

drvTGIF::drvTGIF(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream, float theMagnification):
	drvbase(driveroptions_p,theoutStream,theerrStream,0,0,0),
    	buffer(tempFile.asOutput()),
	objectId            (1),
	magnification(theMagnification)
{
    // set tgif specific values
    scale = 128.0 / (72.0 *magnification );		/* tgif internal scale factor */
    currentDeviceHeight = 792.0 * scale;

    x_offset = 0.0;		/* set to fit to tgif page    */
    y_offset = 89.61;		/*          "                 */

    // cannot write any header part, since we need the total number of pages
    // in the header
}

drvTGIF::~drvTGIF() 
{
    	outf << "state(0,33," << (int) (magnification * 100) 
	 << ",0,0,1,16,1,9,1,1,0,0,1,0,1,0,'Courier',0,17,0,0,1,5,0,0,1,1,0,16,1,0,1," << currentPageNumber << ",1,0,1056,1497,0,0,2880)." << endl;
	outf << "unit(\"1 pixel/pixel\")." << endl;
	// now we can copy the buffer the output
	ifstream & inbuffer = tempFile.asInput();
	copy_file(inbuffer,outf);
}

void drvTGIF::print_coords()
{
    for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
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
}
void drvTGIF::close_page()
{
}

void drvTGIF::open_page()
{
    buffer << "page(" << currentPageNumber << ",\"\",1).\n";
}

void drvTGIF::show_text(const TextInfo & textinfo)
{
    buffer << "text('" <<  colorstring(textinfo.currentR,textinfo.currentG,textinfo.currentB) << "'";
    buffer << "," << textinfo.x + x_offset;
    buffer << "," << currentDeviceHeight - textinfo.y + y_offset - textinfo.currentFontSize;
    // TGIF's origin of text is at the top line, pstoedit's at the bottom
    buffer << ",'" << textinfo.currentFontName.value() << "'";
    int boldfont   =  (strstr(textinfo.currentFontName.value(),"Bold") != NULL);
    int italicfont = ((strstr(textinfo.currentFontName.value(),"Italic") != NULL) ||
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
	const float toRadians = 3.14159265359 / 180.0;
	const float angleInRadians = textinfo.currentFontAngle * toRadians;
	const float cosphi = cos(angleInRadians);
	const float sinphi = sin(angleInRadians);
//	const float Sx = textinfo.currentFontSize;
//	const float Sy = textinfo.currentFontSize;
//	const int len = strlen(textinfo.thetext);
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
    const char * cp = textinfo.thetext;
    while (*cp) {
	if ( (*cp  == '"') || (*cp == '\\') ) {
		buffer << '\\';
	}
	buffer << *cp ;
	cp++;
    }
    buffer << "\"]\n";
}

static const int Fill   = 1;
static const int noFill = 0;
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
	 << ",0,0,0,0,0,'1";
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
