/* 
   drvFIG.cpp : This file is part of pstoedit
   Based on the skeleton for the implementation of new backends

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

/*
    FIG 3.1 driver by Ian MacPhedran (Ian_MacPhedran@engr.usask.ca)
    April 1995

    Color support and conversion to use C++ streams done by Wolfgang Glunz

    Object depth support by Gerhard Kircher <kircher@edvz.tuwien.ac.at>
    March 1996
*/

#include "drvfig.h"

#include <iostream.h>
// rcw2: work round case insensitivity in RiscOS
#ifdef riscos
  #include "unix:string.h"
#else
  #include <string.h>
#endif
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

const int defaults = sizeof(fig_default_colors)/sizeof(char*);
static ColorTable colorTable(fig_default_colors,defaults,colorstring);

static int registercolor(float r, float g, float b)
{
	return colorTable.getColorIndex(r,g,b);
}

static void dumpnewcolors(ostream & theoutStream)
{
	int current=defaults;
	const char * colstring;
	while ((colstring = colorTable.getColorString(current)) != 0) {
		theoutStream << "0 " << current << " " << colstring << endl;
		current++;
	}
}


drvFIG::drvFIG(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream):
	drvbase(driveroptions_p,theoutStream,theerrStream,0,0,0),
    	buffer(tempFile.asOutput())
{
// driver specific initializations

    // set FIG specific values
    scale    = 1;		
    currentDeviceHeight = 13200.0 * scale;
    // We use objectId as depth value.
    // We need this because editing will reorder objects of equal depth,
    // which has an undesireable effect if objects overlap.
    // Depth must be in the range 0..999 (FIG 3.1).
    // If we have more than 1000 objects this will get negative and
    // xfig will set negative depth values to zero.
    // This feature will thus become useless if we have more
    // than 1000 objects. This is an xfig limitation.
    objectId = 999;

    x_offset = 0.0;
    y_offset = currentDeviceHeight;
				 // output buffer, needed because
				 // color entries must be written at
				 // top of output file, but are known
				 // only after processing the full input

    // print the header part
    outf << "#FIG 3.1\nPortrait\nFlush Left\nInches\n1200 2\n";
}

drvFIG::~drvFIG() {
	dumpnewcolors(outf);
	// now we can copy the buffer the output
	ifstream & inbuffer = tempFile.asInput();
	copy_file(inbuffer,outf);
}

void drvFIG::print_coords()
{
    int j;
    float PntFig = 1200.0 / 72.0;

    j = 0;
    for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
	const Point & p = pathElement(n).getPoint(0);
	if (j == 0) { buffer << "\t"; }
	buffer << (int)(PntFig * p.x_) << " " 
	     << (int)(y_offset - (PntFig * p.y_)) <<  " ";
	j++;
	if (j == 5) { j=0; buffer << "\n"; }
    }
    if (j != 0) { buffer << "\n"; }
}

void drvFIG::close_page()
{
// Well, since FIG doesn't support multipage output, we'll move down
// 11 inches and start again.
    y_offset += currentDeviceHeight;
    // reset depth counter
    objectId = 999;
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

void drvFIG::show_text(const TextInfo & textinfo)
{
    unsigned int MAXFNTNUM, fntlength;
    int FigFontNum;
    float FigHeight,FigLength;
    float PntFig = 1200.0 / 72.0;
    float toRadians = 3.14159265359 / 180.0;

    MAXFNTNUM = sizeof(FigFonts)/(sizeof(char *)) - 1;
    FigFontNum = -1;
    fntlength = strlen(textinfo.currentFontName.value());
    for (unsigned int i=0; i<=MAXFNTNUM; i++) {
	if (fntlength == strlen(FigFonts[i])) {
	    if (strncmp(textinfo.currentFontName.value(),FigFonts[i],fntlength) == 0)
		FigFontNum = i;
	}
    }
    if (FigFontNum == -1) {
      errf << "Warning, unsupported font " << textinfo.currentFontName.value() <<
              ", using ";
      if (strstr (textinfo.currentFontName.value(), "Bold") == NULL) {
        if (strstr (textinfo.currentFontName.value(), "Italic") == NULL) {
          errf << "Times-Roman";
          FigFontNum = 0; // Times-Roman
        } else {
          FigFontNum = 1;
          errf << "Times-Italic";
        }
      } else {
        if (strstr (textinfo.currentFontName.value(), "Italic") == NULL) {
          errf << "Times-Bold";
          FigFontNum = 2; // Times-Bold
        } else {
          FigFontNum = 3;
          errf << "Times-BoldItalic";
        }
      }
      errf << " instead." << endl;
    }                                     
#if 0
// old stuff
    if (FigFontNum == -1) {
	errf << "Warning, unsupported font " << textinfo.currentFontName.value() << ", using Courier instead" << endl;
	FigFontNum = 12; // Courier
    } 
#endif

    int color = registercolor(textinfo.currentR,textinfo.currentG,textinfo.currentB);
    float localFontSize = textinfo.currentFontSize;
    if (localFontSize <=  0.1) { localFontSize = 9; }
    localFontSize++; // There appears to be a reduction for some reason
    FigHeight = 1200.0 * localFontSize / 72.0;
    FigLength = FigHeight * strlen(textinfo.thetext);
    buffer << "4 0 ";
    buffer << color;
    if (objectId) objectId--; // don't let it get < 0
    buffer << " " << objectId << " -1 " 
	 << FigFontNum << " " 
	 << (int) localFontSize << " " 
	 << textinfo.currentFontAngle*toRadians << " 4 " 
	 << FigHeight << " " 
	 << FigLength << " " 
	 << (int)(PntFig * textinfo.x) << " " 
	 << (int)(y_offset - (PntFig * textinfo.y)) << " " 
	 << textinfo.thetext << "\\001\n";
}

void drvFIG::show_path()
{
  
    float localLineWidth = currentLineWidth();
    if ((localLineWidth < 0.0) || 
       ((localLineWidth > 0.0) && (localLineWidth <= 1.0)))
               { localLineWidth = 1.0; }
    int linestyle = 0;
    switch (currentLineType()) {
    	case solid : linestyle = 0; break;
    	case dashed :
    	case dashdot : linestyle = 1; break;
    	case dotted :
    	case dashdotdot : linestyle = 2; break;
    }
    buffer << "2 1 " << linestyle << " " 
	 << (int)localLineWidth << " " ;
    const int color = registercolor(currentR(),currentG(),currentB());
    const int fill_or_nofill = (currentShowType() == drvbase::stroke) ? -1 : 20;
    if (objectId) objectId--; // don't let it get < 0
    buffer << color <<  " " << color << " " << objectId << " 0 "
	    << fill_or_nofill            <<  " " << 4.0  << " 0 0 0 0 0 ";
	    // 4.0 is the gap spec. we could also derive this from the input
    buffer << (int)(numberOfElementsInPath()) << "\n";
 
    print_coords();
};

void drvFIG::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
 // just do show_polyline for a first guess
  unused(&llx);
  unused(&lly);
  unused(&urx);
  unused(&ury);
 show_path();
}
