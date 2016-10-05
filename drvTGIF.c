/* 
   drvTGIF.c : This file is part of pstoedit
   Backend for TGIF

   Copyright (C) 1993,1994,1995 Wolfgang Glunz, Wolfgang.Glunz@zfe.siemens.de

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

#include "drvTGIF.h"
#include <iostream.h>
#include <stdio.h>
#include <string.h>

static const char * colorstring(float r, float g, float b)
{
static char buffer[10];
	sprintf(buffer,"%s%.2x%.2x%.2x","#", (int) (r * 255), (int) ( g * 255) ,  (int) (b * 255));
	return buffer;
}

drvTGIF::drvTGIF(ostream & theoutStream, float theMagnification):
	drvbase(theoutStream,theMagnification)
{
    // set tgif specific values
    scale = 128.0 / 72.0;		/* tgif internal scale factor */
    currentDeviceHeight = 792.0 * scale;

    x_offset = 0.0;		/* set to fit to tgif page    */
    y_offset = 89.61;		/*          "                 */
    noFillValue = 0;
}

drvTGIF::~drvTGIF() {}

void drvTGIF::setGrayLevel(const float grayLevel)
{
    currentGrayLevel = grayLevel;
//   sets the fill pattern to be used (FillValue) depending on 
//   the current graylevel (PS:setgray) 
//   graylevel 0 means black 1 white
//   for TGIF
//   no fill 0;
//   white 2
//   black 1
//   light gray 3
//   ...
//   dark gray 11
//   so we have 11 different gray patterns 
     
    if (useFillPattern) {
    	if (grayLevel >= (10.0/11.0) ) {
		FillValue = 2; // white
	} else if (grayLevel <= (1.0/11.0) ) {
		FillValue = 1; // black
	} else {
		FillValue = 11 - ((int) (currentGrayLevel * 11)) + 1 ;
	}
    } else {
	// use color or grey
    	FillValue = 1 ;  // means solid in this case
    }
       
// fprintf(stderr,"set FIll to %d from %f\n",FillValue,currentGrayLevel); 
}


void drvTGIF::print_coords()
{
    const int numberOfPoints = numbersInCurrentSegment();

    outf << pNumber(0) + x_offset ;
    for (int n = 1; n < numberOfPoints; n++) {
	if (n % 2) {		/* y coord */
	    outf << ',' << currentDeviceHeight - pNumber(n) + y_offset;
	} else {
	    outf << ',' << pNumber(n) + x_offset;
	}
	if ((!((n + 1) % 16)) && ((n + 1) != numberOfPoints)) {
	    outf << "\n         ";
	}
    }
}
void drvTGIF::close_page()
{
}

void drvTGIF::open_page()
{
    outf << "page(" << currentPageNumber++ << ",\"\").\n";
}

void drvTGIF::show_textstring(const char *const thetext)
{
    outf << "text('" <<  colorstring(currentR,currentG,currentB) << "'";
    outf << "," << pNumber(0) + x_offset;
    outf << "," << currentDeviceHeight - pNumber(1) + y_offset;
    outf << ",'" << currentFontName << "'";
    int boldfont   =  (strstr(currentFontName,"Bold") != NULL);
    int italicfont = ((strstr(currentFontName,"Italic") != NULL) ||
		      (strstr(currentFontName,"Oblique") != NULL));
    int fonttype = 0;
    if (boldfont) {
	if (italicfont) fonttype = 3;
	else            fonttype = 1;
    } else {
	if (italicfont) fonttype = 2;
	else            fonttype = 0;
    }
    outf << "," << fonttype <<  "," << currentFontSize << ",1,0,0,1,70," << currentFontSize << "," <<  objectId++ << ",0,14,3,0,0,0,0,[" << endl;
    outf << "\"" << thetext << "\"]\n";
}

void drvTGIF::print_header()
{
    outf << "state(0,30," << (int) (magnification * 100) 
	 << ",0,0,1,16,1,9,1,1,0,0,1,0,1,0,'Courier',0,17,0,0,1,5,0,0,1,1,0,16,1,0,1,PSTOTGIFPAGENR,1)." << endl;
}

void drvTGIF::print_trailer()
{
    outf << "%pstoeditpagenumber: PAGENR " << currentPageNumber - 1 << endl;;
}

void drvTGIF::show_polyline(const int fillpat)
{
    outf << "poly('"
	 << colorstring(currentR,currentG,currentB)
	 << "'," 
	 << numbersInCurrentSegment() / 2
	 << ",[" 
	 << endl
	 << "         ";
    print_coords();
    outf << "],0," 
	 << currentLineWidth 
	 << ","
         << FillValue 
	 << ","
         << objectId++
         << ",0,"
         << fillpat
         << ",0,0,0,0,0,[" 
	 << endl
         << "])." 
	 << endl;
};

void drvTGIF::show_rectangle(const int fillpat, const float llx, const float lly, const float urx, const float ury)
{
    outf << "box('" <<  colorstring(currentR,currentG,currentB) << "'";
    outf << "," << llx + x_offset;
    outf << "," <<  currentDeviceHeight - lly + y_offset;
    outf << "," <<  urx + x_offset;
    outf << "," <<  currentDeviceHeight - ury + y_offset;
    outf << ","
	 << fillpat
	 << ","
	 << currentLineWidth 
	 << ","
	 << FillValue
	 << ","
	 << objectId++
	 << ",0,0,0,["
	 << endl
	 << "])."
	 << endl;
}

void drvTGIF::show_polygon(const int fillpat)
{
    outf << "polygon('"
	 << colorstring(currentR,currentG,currentB)
	 << "',"
	 << numbersInCurrentSegment() / 2
	 << ",["
	 << endl
	 << "         ";
    print_coords();
    outf << "],"
	 << fillpat
	 << ","
	 << currentLineWidth 
	 << ","
	 << FillValue
	 << ",0,"
	 << objectId++
	 << ",0,0,0,["
	 << endl
	 << "])."
	 << endl;
}
