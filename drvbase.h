#ifndef __drvbase_h
#define __drvbase_h
/*
   drvbase.h : This file is part of pstoedit Base class for all specific
   driver classes/backends. All virtual functions have to be implemented by
   the specific driver class. See drvexample.c
  
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
/*
// ============================================================================
//
// = LIBRARY
//     pstoedit
//
// = FILENAME
//     drvbase.h
//
// = RCSID
//     $Id$
*/

class ostream; // Just a simple forward


const int       maxFontNamesLength = 1000;
const int       maxpoints = 20000;	// twice the maximal number of points in a path
const int       maxsegments  = maxpoints/2;// at least half of maxpoints (if we only have segments with one point)

class           drvbase 
    // = TITLE
    // Base class for backends to pstoedit
    //
    // = CLASS TYPE
    // Abstract
    //
    // = AUDIENCE
    // Developers of new backends
    //
    // = DESCRIPTION
    //  Abstract base class for backends to pstoedit. 
    // This class defines the virtual functions that every backend has
    // to implement and some functions that are common to all backends
    //
{

public:

	// = CONSTRUCTION, DESTRUCTION AND COPYING

	drvbase(ostream & theoutStream, float theMagnification);// constructor
	virtual ~drvbase();  		// destructor

	// = PUBLIC ATTRIBUTES 
	int             page_empty;	// indicates whether the current page is empty or not

	enum polytype { polygon, polyline };
	struct segmentInfo {
		polytype        currenttype;
		int             start;  // start index in numbers array
		int             end;    // next free index in numbers array
	};

	ostream &	outf;           // the output stream
	int		useFillPattern; // whether to use color/grey or fillpatterns
	int 		useColor;	// true color or grey values 
	int             FillValue;	// The current pattern number for filling

	int             noFillValue;	// A driver specific constant for the 'non fill pattern'

	float           numbers[maxpoints]; // The number stack
	segmentInfo	segments[maxsegments]; // The segment list 

	int		segmentCount;	// used as current index for the 'segments' array
	int 	        currentOutputSegment;  // used to iterate through segments during output;
	int		numbersInCurrentSegment() { return nrOfPointsInSegment(segments[currentOutputSegment]); }

protected:
	float           scale;
	const float     magnification;	// for tgif only
	float           currentLineWidth;
	float           currentDeviceHeight; // normally 792 pt; used for flipping y values.
	float           x_offset;
	float           y_offset;
	char            currentFontName[maxFontNamesLength];
	char            currentFontFamilyName[maxFontNamesLength];
	char            currentFontFullName[maxFontNamesLength];
	char            currentFontWeight[maxFontNamesLength];
	float           currentFontSize;
	float		currentFontAngle;
	int             currentPageNumber;
	float           currentGrayLevel;
	float           currentR; // Colors
	float           currentG;
	float           currentB;

	int             objectId;

public:

	// = COMMON FUNCTIONS
        // These functions are not backend specific and shouldn't have to be
        // changed for new backends

	void            add_to_page();

	int             is_a_rectangle();

	void            addNumber(float value); // add a number to the current path

	void		newSegment(); // starts a new segment

        segmentInfo &   currentSegment() { return segments[segmentCount]; }

	void            showSegments(const int fillpat);

	void    	showText(const char *const thetext);

	float           pop(); // pops and returns last value on stack

static  int             nrOfPointsInSegment(const segmentInfo & aSegment) { return aSegment.end - aSegment.start;}

	float		pNumber(int index) const ; //  returns number with index relative to begin of currentOutputSegment

	float           get_scale() const { return scale; }

	void            setCurrentFontName(const char *const);

	void            setCurrentFontFamilyName(const char *const);

	void            setCurrentFontFullName(const char *const);

	void            setCurrentFontWeight(const char *const);

	void            setCurrentFontSize(const float Size);

	void		setCurrentFontAngle(float value);

	void            setcurrentLineWidth(const float linewidth) 
			{ currentLineWidth = linewidth; }

	void            setCurrentDeviceHeight(const float deviceheight) 
			{ currentDeviceHeight = deviceheight; }

	void		setRGB(const float R,const float G, const float B)
			{ currentR = R ; currentG = G; currentB = B; }

	// = BACKEND SPECIFIC FUNCTIONS
        //  These are backend specific and thus have to be implemented for every new backend.

	virtual void    setGrayLevel(const float grayLevel) = 0;
	// sets 'FillValue' to a int number corresponding to a fill pattern.
	// It is called whenever setgray is executed in PostScript.
	// the grayLevel is a number between 0.0 (white) and 1.0 (black)

	virtual void    close_page() = 0;
	// writes a trailer whenever a page is finished (triggered by showpage)

	virtual void    open_page() = 0;
	// writes a page header whenever a new page is needed

	virtual void    show_textstring(const char *const thetext) = 0;
	// writes thetext at the point (numbers[0],numbers[1])
	// font information  is available in the currentFont... data members.

	virtual void    print_header() = 0;
	// writes the header for the output file
	// called after program start

	virtual void    print_trailer() = 0;
	// writes a trailer for the output file
	// called at end of program

	virtual void    show_polyline(const int fillpat) = 0;
	// writes a polyline for numberCount/2 points 
	// (numbers[0],numbers[1])
	// ...
	// (numbers[numberCount-1],numbers[numberCount])
	// fillpat is the FillValue if the polyline is filled or noFillValue
	// if not.

	virtual void    show_rectangle(const int fillpat,
				       const float llx,
				       const float lly,
				       const float urx,
				       const float ury) = 0;
	// writes a rectangle at points (llx,lly) (urx,ury)
	// fillpat is the FillValue if the rectangle is filled or noFillValue
	// if not.

	virtual void    show_polygon(const int fillpat) = 0;
	// writes a polygon for numberCount/2 points 
	// (numbers[0],numbers[1])
	//   ...
	// (numbers[numberCount-1],numbers[numberCount])
	// fillpat is the FillValue if the polyline is filled or noFillValue
	// if not.
	// The first and the last point in the numbers array are the same.

private:

	void showpath(const int fillpat); // shows just one path using currentOutputSegment

};
#endif
