#ifndef __drvbase_h
#define __drvbase_h
/*
   drvbase.h : This file is part of pstoedit Base class for all specific
   driver classes/backends. All virtual functions have to be implemented by
   the specific driver class. See drvSAMPL.cpp
  
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

#include <fstream.h>
#include <stdio.h>

// for free
#include <stdlib.h>
// for strcpy
// rcw2: work round case insensitivity in RiscOS
#ifdef riscos
#include "unix:string.h"
// rcw2: tempnam doesn't seem to be defined in UnixLib 3.7b for RiscOS 
  char *tempnam(const char *, const char *);
#else
#include <string.h>
#endif


const unsigned int	maxFontNamesLength = 1000;
const unsigned int	maxpoints    = 20000;	// twice the maximal number of points in a path
const unsigned int	maxElements  = maxpoints/2;
const unsigned int	maxsegments  = maxpoints/2;// at least half of maxpoints (if we only have segments with one point)

#if defined (__GNUG__) || defined (__BCPLUSPLUS__) || defined (INTERNALBOOL)
// no need to define bool
#else
typedef int bool;
const bool false = 0;
const bool true  = 1;
#endif


static const char emptyDashPattern[] =  "[ ] 0.0";

class basedrawingelement ; // forward

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
	// = PUBLIC TYPES 

	enum showtype { stroke, fill, eofill };
	enum linetype { solid, dashed, dotted, dashdot, dashdotdot }; // corresponding to the CGM patterns

// need to make RSString public because of problem with SparcCompiler
	class  RSString {
	public:
		// a very very simple resizing string
		RSString(const char * arg = 0) : 
			content(0),
			allocatedLength(0) { if (arg) this->copy(arg); }
		~RSString() { delete [] content; content = 0; allocatedLength = 0; }
		const char * value() const { return content; }
		void copy(const char *src) {
			if ((strlen(src)+1) <= allocatedLength) {
				// we have enough space
				::strcpy(content,src);
			} else {
				// resize
				delete [] content;
				allocatedLength = strlen(src) + 1;
				content = new char[allocatedLength];
				::strcpy(content,src);
			}
		}
	private:
		char * content;
		unsigned int allocatedLength;
		// declared but not defined
		RSString(const RSString &);
		const RSString & operator == (const RSString &);
	};

protected:
	// = PROTECTED TYPES 

	struct TextInfo {
		float 		x;
		float		y;
		const char *	thetext;
		bool		is_non_standard_font;
		RSString        currentFontName;
		RSString        currentFontFamilyName;
		RSString        currentFontFullName;
		RSString        currentFontWeight;
		float           currentFontSize;
		float		currentFontAngle;
		float           currentR; // Colors
		float           currentG;
		float           currentB;
		float		cx; // next five items correspond to the 
		float		cy; // params for the awidthshow operator
		int		Char; // of PostScript
		float		ax; 
		float		ay;
		TextInfo() :
			x(0.0f),
			y(0.0f),
			thetext(0),
			is_non_standard_font(false),
			currentFontSize(10.0f),
			currentFontAngle(0.0f),
			currentR(0.0f),
			currentG(0.0f),
			currentB(0.0f),
			cx(0.0f),
			cy(0.0f),
			Char(32), // 32 means space
			ax(0.0f),
			ay(0.0f) { 
			}
	};

private:
	// = PRIVATE TYPES 

	struct PathInfo {
		showtype	currentShowType;
		linetype	currentLineType;
		unsigned int    currentLineCap;
		unsigned int    currentLineJoin;
		unsigned int    nr;
		basedrawingelement * * path; // a path is an array of pointers to basedrawingelements
		bool	 	isPolygon; // whether current path was closed via closepath or not
		unsigned int	numberOfElementsInPath;
		float           currentLineWidth;
		float           edgeR; // edge colors
		float           edgeG;
		float           edgeB;
		float           fillR; // fill colors
		float           fillG;
		float           fillB;
		RSString	dashPattern; // just the dump of currentdash as string
		PathInfo() :
			currentShowType(drvbase::stroke),
			currentLineType(drvbase::solid),
			currentLineCap(0),
			currentLineJoin(0),
			nr(0),
			path(0),
			isPolygon(0),
			numberOfElementsInPath(0),
			currentLineWidth(0.0f),
			edgeR(0.0f),
			edgeG(0.0f),
			edgeB(0.0f),
			fillR(0.0f),
			fillG(0.0f),
			fillB(0.0f), 
			dashPattern(emptyDashPattern)
			{
			    path = new basedrawingelement *[maxElements];
			}
		~PathInfo() {
			// the path content is deleted by clear
			clear();
			delete [] path;
		}
		void clear();
		void copyInfo(const PathInfo & p);
			// copies the whole path state except the path array
	private:
		// Inhibitors (declared, but not defined)
		const PathInfo& operator=(const PathInfo&);
		PathInfo(const PathInfo &);
	};

public:
	// = PUBLIC DATA

	const bool 	backendSupportsSubPathes;
	const bool 	backendSupportsCurveto;
	const bool 	backendSupportsMerging; // merge a separate outline and filling of a polygon -> 1. element

protected:
	// = PROTECTED DATA

	ostream &	outf;           // the output stream
	ostream &	errf;           // the error stream
	unsigned int	d_argc;
	const char * *	d_argv;
	float           scale;
	float           currentDeviceHeight; // normally 792 pt; used for flipping y values.
	float           currentDeviceWidth;  
	float           x_offset;
	float           y_offset;
	unsigned int    currentPageNumber;
	bool		verbose;
	bool		domerge;


private:
	// = PRIVATE DATA

	bool    	page_empty;	// indicates whether the current page is empty or not
	char * 		driveroptions; // string containing options for backend
	PathInfo 	p1,p2;
	PathInfo * 	currentPath; // for filling from lexer
	PathInfo * 	lastPath;    // for merging
	PathInfo * 	outputPath;  // for output driver
	TextInfo 	textInfo_;


public:
	// = PUBLIC METHODS

	// = CONSTRUCTION, DESTRUCTION AND COPYING

	drvbase(
		const char * driverOptions_p,
		ostream & theoutStream,
		ostream & theerrStream,
		bool backendSupportsSubPathes_p,
		bool backendSupportsCurveto_p,
		bool backendSupportsMerging_p); // constructor
	virtual ~drvbase();  		// destructor

	// = BACKEND GENERIC FUNCTIONS 
        // These functions are not backend specific and shouldn't have to be
        // changed for new backends

	void		startup(bool merge);

	void 		finalize(); // needed because base destructor will be called after
				    // derived destructor

	void            setCurrentDeviceHeight(const float deviceHeight) 
			{ currentDeviceHeight = deviceHeight; }

	void            setCurrentDeviceWidth(const float deviceWidth) 
			{ currentDeviceWidth = deviceWidth; }

	float           get_scale() const { return scale; }

	void		setRGB(const float R,const float G, const float B)
			{ textInfo_.currentR = R ; textInfo_.currentG = G; textInfo_.currentB = B; 
			 currentPath->edgeR = R ; currentPath->edgeG = G; currentPath->edgeB = B; 
			 currentPath->fillR = R ; currentPath->fillG = G; currentPath->fillB = B; }

	void    	showpage();


	// = DRAWING RELATED METHODS

	void		addtopath(basedrawingelement * newelement);

	unsigned int 	&numberOfElementsInPath() { return outputPath->numberOfElementsInPath; }

	const basedrawingelement & pathElement(unsigned int index) const;

	void            setCurrentLineType(const linetype how) 
			{ currentPath->currentLineType = how; }

	void            setCurrentLineWidth(const float linewidth) 
			{ currentPath->currentLineWidth = linewidth; }
			
	void 		setDash(const char * const dash)
			{ currentPath->dashPattern.copy(dash); }

	void	 	setIsPolygon(bool what) { currentPath->isPolygon=what; } // whether current path was closed via closepath or not

	void	 	setPathNumber(unsigned int nr) { currentPath->nr=nr; } 

	void            setCurrentLineJoin(const unsigned int joinType) 
			{ currentPath->currentLineJoin = joinType; }

	void            setCurrentLineCap(const unsigned int capType) 
			{ currentPath->currentLineCap = capType; }

	void            setCurrentShowType(const showtype how) 
			{ currentPath->currentShowType = how; }

	void 		dumpPath();  // shows current path

	// = TEXT RELATED METHODS

 	void		setCurrentWidthParams(float ax,float ay,int Char,float cx,float cy);

	void            setCurrentFontName(const char *const Name,bool is_non_standard_font);

	void            setCurrentFontFamilyName(const char *const Name);

	void            setCurrentFontFullName(const char *const Name);

	void            setCurrentFontWeight(const char *const Name);

	void            setCurrentFontSize(const float Size);

	void		setCurrentFontAngle(float value);

	void    	dumpText(const char *const thetext,const float x, const float y);



	// = BACKEND SPECIFIC FUNCTIONS

	// If a backend only deals with a special set of font names
	// the following function must return a 0 terminated list
	// of font names.
	virtual const char * const * 	knownFontNames() const { return 0; }


protected:
	// = PROTECTED METHODS

	showtype	currentShowType() const { return outputPath->currentShowType; }
	linetype	currentLineType() const { return outputPath->currentLineType; }
	unsigned int	currentLineCap() const { return outputPath->currentLineCap; }
	unsigned int	currentLineJoin() const { return outputPath->currentLineJoin; }
	bool	 	isPolygon() const { return outputPath->isPolygon;} // whether current path was closed via closepath or not
protected:
	float           currentLineWidth() const { return outputPath->currentLineWidth; }
	unsigned int    currentNr() const { return outputPath->nr; } 
	float           edgeR() const { return outputPath->edgeR; } // edge colors
	float           edgeG() const { return outputPath->edgeG; }
	float           edgeB() const { return outputPath->edgeB; }
	float           fillR() const { return outputPath->fillR; } // fill colors
	float           fillG() const { return outputPath->fillG; }
	float           fillB() const { return outputPath->fillB; }

	const char *    dashPattern() const { return outputPath->dashPattern.value(); }

	float           currentR() const { return outputPath->fillR; } // backends that don't support merging 
	float           currentG() const { return outputPath->fillG; } // don't need to differentiate and
	float           currentB() const { return outputPath->fillB; } // can use these functions.



private:
	// = PRIVATE METHODS

	void 		guess_linetype();

	bool		is_a_rectangle() const;

	void            add_to_page();

	bool		pathsCanBeMerged(const PathInfo & p1, const PathInfo & p2);

	// = BACKEND SPECIFIC FUNCTIONS

        //  These next functions are pure virtual and thus need to be implemented for every new backend.

	virtual void    close_page() = 0;
	// writes a trailer whenever a page is finished (triggered by showpage)

	virtual void    open_page() = 0;
	// writes a page header whenever a new page is needed

	virtual void    show_text(const TextInfo & textinfo) = 0;

	virtual void    show_path() = 0;

	virtual void    show_rectangle(
				       const float llx,
				       const float lly,
				       const float urx,
				       const float ury) = 0;
	// writes a rectangle at points (llx,lly) (urx,ury)

	// = INHIBITORS (declared, but not defined)

	drvbase(const drvbase &);
	drvbase & operator=(const drvbase&);

};


typedef const char * (*makeColorNameType)(float r, float g, float b);
const unsigned int maxcolors = 1000 ; // 1000 colors maximum
class ColorTable 
{
public:
	ColorTable(const char * const * defaultColors,
		   const unsigned int numberOfDefaultColors,
		   makeColorNameType makeColorName);
	~ColorTable();
	unsigned int getColorIndex(float r, float g, float b);
	const char * const getColorString(float r, float g, float b);
	bool 	isKnownColor(float r, float g, float b) const;
	const char * const getColorString(unsigned int index) const;
		
private:
	const char * const * const defaultColors_;
	const unsigned int  numberOfDefaultColors_;
	char * newColors[maxcolors];
        makeColorNameType makeColorName_ ;
};


struct Point
{
public:
	Point(float x, float y) : x_(x),y_(y) {}
	Point() : x_(0.0f), y_(0.0f) {}; // for arrays
	float x_;
	float y_;
	friend bool operator==(const Point & p1, const Point & p2) { return (p1.x_ == p2.x_) && (p1.y_ == p2.y_); }
private:
};

#ifdef __TCPLUSPLUS__
// turbo C++ has problems with enum for template parameters
typedef unsigned int Dtype;
const Dtype moveto = 1;
const Dtype lineto = 2;
const Dtype closepath = 3;
const Dtype curveto = 4;
#else
enum  Dtype {moveto, lineto, closepath, curveto};
#endif
// closepath is only generated if backend supportes subpathes
// curveto   is only generated if backend supportes it


class basedrawingelement 
{
public:
	basedrawingelement(unsigned int size_p) : size(size_p) {}
	virtual const Point &getPoint(unsigned int i) const = 0;
	virtual Dtype getType() const = 0;
	friend ostream & operator<<(ostream & out,const basedrawingelement &elem);
	friend bool operator==(const basedrawingelement & bd1, const basedrawingelement & bd2);
protected:
	const unsigned int size;
};


static void copyPoints(unsigned int nr, const Point src[], Point target[])
{
// needed because CenterLine cannot inline for loops
	for (unsigned int i = 0 ; i < nr ; i++ ) target[i] = src[i]; 
}

template <unsigned int nr, Dtype curtype>
class drawingelement : public basedrawingelement
{
public:
// CenterLine !!!!
// "drvbase.h", line 455: sorry, not implemented: cannot expand inline function  drawingelement 
//   <1 , 0 >::drawingelement__pt__19_XCUiL11XC5DtypeL10(Point*) with  for statement in inline

	drawingelement(float x1 = 0.0 ,float y1 = 0.0 , float x2 = 0.0, float y2 = 0.0, float x3 = 0.0, float y3 = 0.0)
	: basedrawingelement(nr)
{
	Point  p[] = {Point(x1,y1),Point(x2,y2),Point(x3,y3)};
#if 0
	Point   p[3];
	p[0].x_ = x1;
	p[0].y_ = y1;
	p[1].x_ = x2;
	p[1].y_ = y2;
	p[2].x_ = x3;
	p[2].y_ = y3;
#endif
	copyPoints(nr,p,points);
}

	drawingelement(const Point p[])
	: basedrawingelement(nr)
{
//	for (unsigned int i = 0 ; i < nr ; i++ ) points[i] = p[i]; 
	copyPoints(nr,p,points);
}
	const Point &getPoint(unsigned int i) const  { return points[i]; }
	Dtype getType() const 		     { return (Dtype) curtype; }
						// This cast (Dtype) is necessary
						// to eliminate a compiler warning
						// from the SparcCompiler 4.1.
						// although curtype is of type Dtype
private:
	Point points[nr > 0 ? nr : 1];
};


// CenterLine !!!!
// "drvbase.h", line 477: sorry, not implemented: cannot expand inline function  
// drawingelement <3 , 3 >::drawingelement__pt__19_XCUiL13XC5DtypeL13(Point*) with  for statement in inline

#if 0
template <unsigned int nr, Dtype curtype>
inline drawingelement<nr,curtype>::drawingelement(Point p[]) 
	: basedrawingelement(nr)
{
	for (unsigned int i = 0 ; i < nr ; i++ ) points[i] = p[i]; 
}
#endif

typedef drawingelement<(unsigned int) 1,moveto>  	Moveto;
typedef drawingelement<(unsigned int) 1,lineto> 	Lineto;
typedef drawingelement<(unsigned int) 0,closepath>  	Closepath;
typedef drawingelement<(unsigned int) 3,curveto> 	Curveto;

// A temporary file, that is automatically removed after usage
class TempFile {
public:
	TempFile()  ;
	~TempFile() ;
	ofstream & asOutput();
	ifstream & asInput();
private:
	void close() ;
	char * tempFileName;
	ofstream outFileStream;
	ifstream inFileStream;
};

// used to eliminate compiler warnings about unused parameters
inline void unused(const void * const) { }

class istream;
class ostream;
void copy_file(istream& infile,ostream& outfile) ;
void freeconst(const void *ptr);
#endif
