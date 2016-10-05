/* 
   drvFIG.c : This file is part of pstoedit
   Based on the skeleton for the implementation of new backends

   Copyright (C) 1993,1994 Wolfgang Glunz, Wolfgang.Glunz@zfe.siemens.de

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
*/

#include "drvFIG.h"
#include <iostream.h>
#include <string.h>
// for sprintf
#include <stdio.h>
// for free
#include <malloc.h>

static const char * colorstring(float r, float g, float b)
{
static char buffer[10];
	sprintf(buffer,"%s%.2x%.2x%.2x","#", (int) (r * 255), (int) ( g * 255) ,  (int) (b * 255));
	return buffer;
}

static const char * fig_default_colors[] =  {
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
                        "#ffd700" };

// I know we could make a class here
const int maxcolors = 1000;
int   currentcolor  = 0;
static char * newcolors[maxcolors];
const int defaults = sizeof(fig_default_colors)/sizeof(char*);

static void initnewcolors()
{
	for (int i = 0 ; i< maxcolors; i++) newcolors[i] = 0;
}

static void freenewcolors()
{
int current= 0;
while (newcolors[current] != 0) {
	free(newcolors[current]); // need free instead of delete, because strdup calls malloc
	current++;
}
}

static int registercolor(float r, float g, float b)
{
// returns the index under which the color was registered
const char * cmp = colorstring(r,g,b);
for (int i = 0; i < defaults ; i++ )
{
	if (strcmp(cmp,fig_default_colors[i]) == 0) {
		return i;
	}
}
// not found so far
// add a copy to newcolors
	if (currentcolor < maxcolors) {
		newcolors[currentcolor] = strdup(cmp);
		currentcolor++;
		return currentcolor+defaults-1;
	} else {
		cerr << "running out of colors in drvFIG.c" << endl;
		return 0;
	}

}
static void dumpnewcolors(ostream & theoutStream)
{
int current= 0;
while (newcolors[current] != 0) {
	theoutStream << "#color 0 " << defaults+current << " " << newcolors[current] << endl;
	current++;
}
}


drvFIG::drvFIG(ostream & theoutStream, float theMagnification):
	drvbase(theoutStream,theMagnification)
{
// driver specific initializations

    // set FIG specific values
    scale    = 1;		
    currentDeviceHeight = 13200.0 * scale;
    currentFontAngle = 0.0;

    x_offset = 0.0;
    y_offset = currentDeviceHeight;
    noFillValue   = -1;
    initnewcolors();
}

drvFIG::~drvFIG() {
freenewcolors();
}

void drvFIG::setGrayLevel(const float grayLevel)
{
    currentGrayLevel = grayLevel;
//
// from the TGIF driver docs, this value is 0=black, 1=white
// FIG 3.1 supports 21 levels of grey  20=black to 0=white
//         (plus -1 = not filled) for BLACK colour
    FillValue = 20 - (int)((grayLevel * 20.0) + 0.5);
}


void drvFIG::print_coords()
{
// Okay, this seems to be where one would print out the coordinates
// for a polyline
    int j;
    float PntFig = 1200.0 / 72.0;

    j = 0;
    for (int n = 0; n < numbersInCurrentSegment(); n++) {
	if (j == 0) { outf << "\t"; }
	outf << (int)(PntFig * pNumber(n)) << " " 
	     << (int)(y_offset - (PntFig * pNumber(n+1))) <<  " ";
	n++; j++;
	if (j == 5) { j=0; outf << "\n"; }
    }
    if (j != 0) { outf << "\n"; }
}

void drvFIG::close_page()
{
// Well, since FIG doesn't support multipage output, we'll move down
// 11 inches and start again.
    y_offset += currentDeviceHeight;
}

void drvFIG::open_page()
{
}

// 
// FIG 3.1 uses an index for the popular fonts:
//
// (we cannot make this array local to drvFIG::show_textstring
// because some CCs don't support that yet.

const char *FigFonts[] = {
	"Times-Roman","Times-Italic", "Times-Bold", "Times-BoldItalic",
	"AvantGarde-Book", "AvantGarde-BookOblique", "AvantGarde-Demi",
	"AvantGarde-DemiOblique", "Bookman-Light", "Bookman-LightItalic",
	"Bookman-Demi", "Bookman-DemiItalic", "Courier", "Courier-Oblique",
	"Courier-Bold","Courier-BoldOblique","Helvetica","Helvetica-Oblique",
	"Helvetica-Bold", "Helvetica-BoldOblique", "Helvetica-Narrow",
	"Helvetica-Narrow-Oblique", "Helvetica-Narrow-Bold",
	"Helvetica-Narrow-BoldOblique", "NewCenturySchlbk-Roman",
	"NewCenturySchlbk-Italic", "NewCenturySchlbk-Bold",
	"NewCenturySchlbk-BoldItalic", "Palatino-Roman",
	"Palatino-Italic", "Palatino-Bold", "Palatino-BoldItalic",
	"Symbol", "ZapfChancery-MediumItalic", "ZapfDingbats"};

void drvFIG::show_textstring(const char *const thetext)
{
    int MAXFNTNUM,FigFontNum,fntlength;
    float FigHeight,FigLength;
    float PntFig = 1200.0 / 72.0;
    float toRadians = 3.14159265359 / 180.0;

    MAXFNTNUM = sizeof(FigFonts)/(sizeof(char *)) - 1;
    FigFontNum = -1;
    fntlength = strlen(currentFontName);
    for (int i=0; i<=MAXFNTNUM; i++) {
	if (fntlength == strlen(FigFonts[i])) {
	    if (strncmp(currentFontName,FigFonts[i],fntlength) == 0)
		FigFontNum = i;
	}
    }

    int color = registercolor(currentR,currentG,currentB);
    if (currentFontSize <=  0.1) { currentFontSize = 9; }
    currentFontSize++; // There appears to be a reduction for some reason
    FigHeight = 1200.0 * currentFontSize / 72.0;
    FigLength = FigHeight * strlen(thetext);
    outf << "4 0 ";
    if(useFillPattern) {
    	outf << "0";
    } else {
	outf << color;
    }
    outf << " 0 -1 " 
	 << FigFontNum << " " 
	 << (int) currentFontSize << " " 
	 << currentFontAngle*toRadians << " 4 " 
	 << FigHeight << " " 
	 << FigLength << " " 
	 << (int)(PntFig * pNumber(0)) << " " 
	 << (int)(y_offset - (PntFig * pNumber(1))) << " " 
	 << thetext << "\\001\n";
}

void drvFIG::print_header()
{
// I assume that this is what would normally be the initialization
// routine for the output.
    outf << "#FIG 3.1\nPortrait\nFlush Left\nInches\n1200 2\n";
}

void drvFIG::print_trailer()
{
// I guess this closes the device nothing to do here for FIG
	dumpnewcolors(outf);
}

void drvFIG::show_polyline(const int fillpat)
{

    int FIGLColour = 0;
  
    if ((currentLineWidth < 0.0) || 
       ((currentLineWidth > 0.0) && (currentLineWidth <= 1.0)))
               { currentLineWidth = 1.0; }
    if ((fillpat == noFillValue) && (currentLineWidth == 0.0))
               { currentLineWidth = 1.0; }
    if ((fillpat == noFillValue) && (FillValue == 0)) {
               FIGLColour = 7;
    }
    int color = registercolor(currentR,currentG,currentB);
    outf << "2 1 0 " 
	 << (int)currentLineWidth << " " ;
    if (useFillPattern) {
       outf << FIGLColour << " 0 0 0 " 
   	    << (int)fillpat << " 0.00 0 0 0 0 0 " ;
    } else {
       outf << color << " 0 0 0 " 
	    << 20            << " 0.00 0 0 0 0 0 ";
    }
    outf << (int)(numbersInCurrentSegment()/2) << "\n";
 
    print_coords();
};

void drvFIG::show_rectangle(const int fillpat, const float llx, const float lly, const float urx, const float ury)
{
 // just do show_polyline for a first guess
 show_polyline(fillpat);
}

void drvFIG::show_polygon(const int fillpat)
{
 // just do show_polyline for a first guess
 show_polyline(fillpat);
}
