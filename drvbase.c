/* 
   drvbase.c : This file is part of pstoedit
   Basic, driver independent output routines

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

#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include "drvbase.h"


drvbase::drvbase(ostream & theoutStream, float theMagnification) : // constructor
    page_empty          (1),
    outf                (theoutStream),
    useFillPattern      (0), 
    useColor            (1),
    FillValue           (0),
    noFillValue         (0),
    segmentCount        (0),
    currentOutputSegment(0),
    scale               (1.0),
    magnification       (theMagnification),
    // set some common defaults
    currentLineWidth    (1.0),
    currentDeviceHeight (792.0 * scale),
    x_offset            (0),
    y_offset            (0),
    currentFontSize     (1.0),
    currentFontAngle    (0.0),
    currentPageNumber   (1),
    currentGrayLevel    (0.0), // black 
    currentR    	(0.0),
    currentG    	(0.0),
    currentB    	(0.0),
    objectId            (1)
{ 
    // init segment info for first segment
    // all others will be updated with each newsegment
    segments[0].currenttype= drvbase::polyline;
    segments[0].start      = 0;
    segments[0].end        = 0;

    setCurrentFontName("Courier");
    setCurrentFontFamilyName("Courier");
    setCurrentFontWeight("Regular");
    setCurrentFontFullName("Courier");
    setCurrentFontSize(10);
}

drvbase::~drvbase() { }	// destructor

void drvbase::addNumber(float co)
{

// printf("Adding %f\n",co);
 if (currentSegment().end < maxpoints) { 
     numbers[currentSegment().end++] = co; 
     // currentSegment().end always points to the next free number
 } else { 
   cerr << "Too many points in segment. Please increase maxpoints in drvbase.h \n"; 
   exit(1); 
 }
}

float drvbase::pNumber(int index) const 
{
	return numbers[index + segments[currentOutputSegment].start];
}

void drvbase::newSegment()
{
	int currentend = currentSegment().end;
	segmentCount++;
	currentSegment().currenttype = polyline;
	currentSegment().start = currentend ; // remember end points to the next free index
	currentSegment().end   = currentend ; // remember end points to the next free index
}

void drvbase::showText(const char * const thetext)
{
  add_to_page();
  currentOutputSegment = segmentCount; // use current segment for text output;
  show_textstring(thetext);
  pop();
  pop();
}

float drvbase::pop() 
{ 
  currentSegment().end-- ; 
//  printf("popping %f\n",numbers[currentSegment().end]);
  return numbers[currentSegment().end]; // the value we just popped
}

void drvbase::setCurrentFontName(const char * const Name) 
{ strncpy(currentFontName,Name,maxFontNamesLength); }

void drvbase::setCurrentFontFamilyName(const char * const Name)
{ strncpy(currentFontFamilyName,Name,maxFontNamesLength); }

void drvbase::setCurrentFontFullName(const char * const Name)
{ strncpy(currentFontFullName,Name,maxFontNamesLength); }

void drvbase::setCurrentFontWeight(const char * const Name)
{ strncpy(currentFontWeight,Name,maxFontNamesLength); }

void drvbase::setCurrentFontSize(const float Size)
{ /* cerr << "setting Size to " << Size << endl; */  currentFontSize = Size ; }

void drvbase::setCurrentFontAngle(float value)
{ currentFontAngle =+ value; }

int drvbase::is_a_rectangle()
{
    int     dir;		/* 1 - up/down (same x) 0 - left/right (same y) */
    if (numbersInCurrentSegment() != 10)
	return 0;
/* it might be a  rectangle */

    if (pNumber(0) == pNumber(2)) {
	/* same x */
	dir = 0;
    } else if (pNumber(1) == pNumber(3)) {
	/* same y */
	dir = 1;
    } else {
	/* no rectangle */
	return 0;
    }

    for (int i = 0; i < 8; i++, i++) {
	if (dir == 1) {
	    if (!(pNumber(i + 1) == pNumber(i + 1 + 2)))
		return 0;
	    dir = 0;
	} else {
	    if (!(pNumber(i) == pNumber(i + 2)))
		return 0;
	    dir = 1;
	}
    }
    return 1;
}

static float max(float a,float b)
{
	return (a>b ? a : b);
}

static float min(float a,float b)
{
	return (a<b ? a : b);
}

void drvbase::add_to_page()
{
    if (page_empty) {
	page_empty = 0;
	open_page();
    }
}

void drvbase::showSegments(const int fillpat)
{
	for (int segment = 0; segment <= segmentCount; segment++) {
		currentOutputSegment = segment;
		showpath(fillpat);
	}
    	segmentCount      = 0;			/* clear segments */
	segments[0].start = segments[0].end = 0;
	segments[0].currenttype = polyline;
}

void drvbase::showpath(const int fillpat)
{
    int numbersInSegment = numbersInCurrentSegment();
    if (numbersInSegment <= 2) {
	/* cannot draw single points */
	return;
    }
    if (numbersInSegment % 2) {
	cerr << "% odd number of data in next record" << numbersInSegment << endl;
    }
    add_to_page();
    if (segments[currentOutputSegment].currenttype == polyline) { /* PolyLine */
	show_polyline(fillpat);
    } else { /* Polygon */
	if (is_a_rectangle()) {
		{
		float llx,lly,urx,ury;

		llx = min( min(pNumber(0),pNumber(2)), min(pNumber(4),pNumber(6)));
		urx = max( max(pNumber(0),pNumber(2)), max(pNumber(4),pNumber(6)));
		lly = min( min(pNumber(1),pNumber(3)), min(pNumber(5),pNumber(7)));
		ury = max( max(pNumber(1),pNumber(3)), max(pNumber(5),pNumber(7)));

		show_rectangle(fillpat,llx,lly,urx,ury);
	        }
	} else {
		show_polygon(fillpat);
	}
    };
}
