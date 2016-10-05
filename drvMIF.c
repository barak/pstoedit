/* 
   drvMIF.c : This file is part of pstoedit
   Backend for Framemaker(TM) MIF format

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

#include "drvMIF.h"
#include <iostream.h>
#include <string.h>
#include <stdio.h>

static const char * colorstring(float r, float g, float b)
{
static char buffer[15];
	sprintf(buffer,"R%.3dG%.3dB%.3d", (int) (r * 255), (int) ( g * 255) ,  (int) (b * 255));
	return buffer;
}

static void writeColorCatEntry(ostream & out, float r, float g, float b)
{
int cyan    = (int) (100 * (1.0 - r)) ;
int magenta = (int) (100 * (1.0 - g)) ;
int yellow  = (int) (100 * (1.0 - b)) ;
//int black   = ((cyan + magenta + yellow) == 300 ) ? 100 : 0; // black 0 if not black
int black = 0;

out << "#pstoeditcolorentry <Color "
    << "<ColorTag " << colorstring(r,g,b) << " > " 
    << "<ColorCyan " << cyan  << " >"
    << "<ColorMagenta " << magenta  << " >"
    << "<ColorYellow " << yellow  << " >"
    << "<ColorBlack " << black  << " >"
    << ">";
}

drvMIF::drvMIF(ostream & theoutStream, float theMagnification):
	drvbase(theoutStream,theMagnification)
{
	noFillValue = 15;
}

drvMIF::~drvMIF() {} 


void drvMIF::setGrayLevel(const float grayLevel)
{
    currentGrayLevel = grayLevel;

    if (useFillPattern) {
        // GrayLevel 1 should result in FillValue 7 
    	if (currentGrayLevel >= 1.0 ) {
		FillValue = 7;
	    } else {
	       FillValue = (int) ((currentGrayLevel) * 8); 
	    }
    } else {
        // use colors painted solid
    	FillValue = 0; // solid
    }
}


void drvMIF::print_coords()
{
    const int numberOfPoints = numbersInCurrentSegment();
    for (int n = 0; n < numberOfPoints; n++, n++) {
        outf <<" <Point "  
	     << pNumber(n) + x_offset << " pt " 
	     << currentDeviceHeight - pNumber(n + 1) + y_offset << " pt>" 
	     << endl;
    }
}

void drvMIF::close_page()
{
    outf << "> # end of Frame\n";
    outf << "> # end of Page\n";
}

void drvMIF::open_page()
{
    outf << "<Page\n";
    outf << " <PageType BodyPage >\n";
    outf << " <PageNum `" << currentPageNumber++ << "'>\n";
    outf << " <PageSize  612.0 pt 792.0 pt>\n";
    outf << " <PageOrientation Portrait >\n";
    outf << " <PageBackground `Default'>\n";
    outf << "<Frame\n";
    outf << "  <Pen 0>\n";
    outf << "  <Fill " << noFillValue << ">\n"; 
    outf << "  <PenWidth  1.0 pt>\n";
    outf << "  <Separation 0>\n";
    outf << "  <BRect  0.0 pt 0.0 pt 612.0 pt 792.0 pt>\n";
    outf << "  <FrameType NotAnchored >\n";
}

void drvMIF::show_textstring(const char *const thetext)
{
    const char * start_of_text = thetext;
    outf << "  <TextLine \n";
    outf << "   <TLOrigin  " 
	 << pNumber(0) + x_offset << " pt " 
	 << currentDeviceHeight - pNumber(1) + y_offset << " pt>\n";
    outf << "   <TLAlignment Left >\n";
    outf << "   <Angle " << currentFontAngle << " >" << endl;
    outf << "   <Font \n";
    outf << "    <FTag `'>\n";
    outf << "    <FFamily `";
    char * cp = currentFontName;
    // print up to first '-' or end of string 
    while (cp && *cp && (*cp != '-') ) {
	outf << *cp;
	cp++;
    }
    outf << "'>\n";

    outf << "# currentFontFamilyName: " <<  currentFontFamilyName << endl;
    outf << "# currentFontFullName: " << currentFontFullName << endl;
    outf << "# currentFontName: " << currentFontName << endl;
    outf << "    <FVar `Regular'>\n";
    outf << "    <FWeight `" << currentFontWeight << "'>"<< endl;
    if (  (strstr(currentFontName,"Italic") != NULL)  ||
          (strstr(currentFontName,"Oblique") != NULL))  {
    	outf << "    <FAngle `Italic'>\n";
    } else {
    	outf << "    <FAngle `Regular'>\n";
    }
    outf << "    <FSize  " << currentFontSize << " pt>\n" ;
    outf << "    <FUnderline No >\n";
    outf << "    <FOverline No >\n";
    outf << "    <FStrike No >\n";
    outf << "    <FSupScript No >\n";
    outf << "    <FSubScript No >\n";
    outf << "    <FChangeBar No >\n";
    outf << "    <FOutline No >\n";
    outf << "    <FShadow No >\n";
    outf << "    <FPairKern Yes >\n";
    outf << "    <FDoubleUnderline No >\n";
    outf << "    <FNumericUnderline No >\n";
    outf << "    <FDX  0.0 pt>\n";
    outf << "    <FDY  0.0 pt>\n";
    outf << "    <FDW  0.0 pt>\n";
    outf << "    <FSeparation 0>\n";
    outf << "     <FColor `" << colorstring(currentR,currentG,currentB) << "'> ";
    writeColorCatEntry(outf,currentR,currentG,currentB);
    outf << "\n" << endl;
    outf << "   > # end of Font\n";
    outf << "   <String `";
    while (*start_of_text) {
	    if (*start_of_text == '>') {
		outf << "\\";
	    }
	    outf << *start_of_text;
	    start_of_text++;
    }
    outf << "'>\n";
    outf << "  > # end of TextLine\n";
}

void drvMIF::print_header()
{
//    outf << "<MIFFile 4.00>\n";
//    outf << "<Units Upt >\n";
// This is done outside now (in pstoedit)
}

void drvMIF::print_trailer()
{
}

void drvMIF::show_polyline(const int fillpat)
{

    outf << "<PolyLine\n <NumPoints " << numbersInCurrentSegment() / 2 << ">\n";
    outf << " <Fill " << fillpat << ">\n";
    outf << " <Pen " << FillValue << ">\n";
    outf << " <PenWidth " << currentLineWidth << " pt>\n";
    outf << " <ObColor `" << colorstring(currentR,currentG,currentB) << "'>";
    writeColorCatEntry(outf,currentR,currentG,currentB) ;
    outf << "\n" << endl;
    print_coords();
    outf << "> # end of PolyLine\n";
}

void drvMIF::show_rectangle(const int fillpat, 
			    const float llx, const float lly, const float urx, const float ury)
{
    outf << "<Rectangle\n";
    outf << " <Fill " << fillpat   << ">\n"; 
    outf << " <Pen  " << FillValue << ">\n";
    outf << " <PenWidth " << currentLineWidth << " pt>\n";
    outf << " <ObColor `" << colorstring(currentR,currentG,currentB) << "'>";
    writeColorCatEntry(outf,currentR,currentG,currentB) ;
    outf << "\n" << endl;
    outf << " <BRect ";
    outf <<  llx + x_offset << " pt ";
    outf <<  currentDeviceHeight - ury + y_offset << " pt ";
    outf <<  urx - llx << " pt "; /* dx */
    outf <<  ury - lly << " pt "; /* dy */
    outf << "> \n> # end of Rectangle\n";
}

void drvMIF::show_polygon(const int fillpat)
{
    pop(); pop(); /* MIF does not need end=start */
    outf << "<Polygon\n <NumPoints " << numbersInCurrentSegment() / 2 << ">\n";
    outf << " <Fill " << fillpat   << ">\n"; 
    outf << " <Pen  " << FillValue << ">\n";
    outf << " <PenWidth "  << currentLineWidth << " pt>\n";
    outf << " <ObColor `" << colorstring(currentR,currentG,currentB) << "'>";
    writeColorCatEntry(outf,currentR,currentG,currentB) ;
    outf << "\n" << endl;
    print_coords();
    outf << "> # end of Polygon\n";
}
