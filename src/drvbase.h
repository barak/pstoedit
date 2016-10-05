#ifndef __drvbase_h
#define __drvbase_h
/*
   drvbase.h : This file is part of pstoedit Base class for all specific
   driver classes/backends. All virtual functions have to be implemented by
   the specific driver class. See drvSAMPL.cpp
  
   Copyright (C) 1993 - 1999 Wolfgang Glunz, wglunz@geocities.com

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


#include "cppcomp.h"


#include I_fstream
#include I_stdio
#include I_stdlib
#include I_string_h
USESTD

#include <assert.h>
#include "miscutil.h"


// for compatibility checking
static const unsigned int drvbaseVersion = 105;
// 101 introduced the driverOK function
// 102 introduced the font optimization (lasttextinfo_)
// 103 introduced the -ssp support and the virtual pathscanbemerged
// 104 introduced the fontmatrix handling
// 105 introduced the miterlimit Info and outputPageSize

const unsigned int	maxFontNamesLength = 1000;
const unsigned int	maxpoints    = 20000;	// twice the maximal number of points in a path
const unsigned int	maxElements  = maxpoints/2;
const unsigned int	maxsegments  = maxpoints/2;// at least half of maxpoints (if we only have segments with one point)


struct Point
{
public:
	Point(float x, float y) : x_(x),y_(y) {}
	Point() : x_(0.0f), y_(0.0f) {}; // for arrays
	float x_;
	float y_;
	friend bool operator==(const Point & p1, const Point & p2) { return (p1.x_ == p2.x_) && (p1.y_ == p2.y_); }
	Point transform(const float matrix[6]) const;
private:
};

// image needs Point !
#include "image.h"

static const char emptyDashPattern[] =  "[ ] 0.0";

class basedrawingelement ; // forward
class DriverDescription ;  // forward
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
	friend class sub_path; // needs PathInfo
	friend class sub_path_list;
public:
	// = PUBLIC TYPES 

	enum showtype { stroke, fill, eofill };
	enum linetype { solid=0, dashed, dotted, dashdot, dashdotdot }; // corresponding to the CGM patterns

	struct TextInfo {
		float 		x;
		float		y;
		float		FontMatrix[6];
		float 		x_end; // pen coordinates after show in PostScript
		float		y_end; // 
		RSString 	thetext;
		bool		is_non_standard_font;
		RSString    currentFontName;
		RSString    currentFontFamilyName;
		RSString    currentFontFullName;
		RSString    currentFontWeight;
		float       currentFontSize;
		float		currentFontAngle;
		float       currentR; // Colors
		float       currentG;
		float       currentB;
		float		cx; // next five items correspond to the 
		float		cy; // params for the awidthshow operator
		int			Char; // of PostScript
		float		ax; 
		float		ay;
		bool		mappedtoIsoLatin1;
		bool		samefont(const TextInfo& cmp) const {
			return ( currentFontName == cmp.currentFontName ) &&
				//	( currentFontFamilyName == cmp.currentFontFamilyName ) &&
				//	( currentFontFullName == cmp.currentFontFullName ) &&
					( currentFontWeight == cmp.currentFontWeight ) &&
					( currentFontSize == cmp.currentFontSize ) &&
					( currentFontAngle == cmp.currentFontAngle ) ;
		}

		bool		samecolor(const TextInfo& cmp) const {
			return ( currentR == cmp.currentR ) &&
					( currentG == cmp.currentG ) &&
					( currentB == cmp.currentB );
		}
		TextInfo() :
			x(0.0f),
			y(0.0f),
			x_end(0.0f),
			y_end(0.0f),
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
			ay(0.0f), 
			mappedtoIsoLatin1(true) {
				for (int i = 0; i < 6 ; i++ ) FontMatrix[i] = 0.0f;
			}
		~TextInfo() { }
	private:
		// declared but not defined
		// const TextInfo & operator = (const TextInfo &); // default is ok
		// TextInfo(const TextInfo &); // default is ok
	};

private:
	// = PRIVATE TYPES 

protected:
	// = PROTECTED TYPES 

	struct PathInfo {
		showtype	currentShowType;
		linetype	currentLineType;
		unsigned int    currentLineCap;
		unsigned int    currentLineJoin;
		float			currentMiterLimit;
		unsigned int    nr;
		basedrawingelement * * path; // a path is an array of pointers to basedrawingelements
		bool	 	isPolygon; // whether current path was closed via closepath or not
		unsigned int	numberOfElementsInPath;
		unsigned int	subpathoffset; // normally 0, but if subpathes are simulated
									   // then this is set to the begin of the current subpath 
									   // before show_path is executed
		float           currentLineWidth;
		float           edgeR; // edge colors
		float           edgeG;
		float           edgeB;
		float           fillR; // fill colors
		float           fillG;
		float           fillB;
		bool			pathWasMerged; // true, if this path is a result of a merge operation
		RSString	dashPattern; // just the dump of currentdash as string
		PathInfo() :
			currentShowType(drvbase::stroke),
			currentLineType(drvbase::solid),
			currentLineCap(0),
			currentLineJoin(0),
			currentMiterLimit(10.0f),
			nr(0),
			path(0),
			isPolygon(0),
			numberOfElementsInPath(0),
			subpathoffset(0),
			currentLineWidth(0.0f),
			edgeR(0.0f),
			edgeG(0.0f),
			edgeB(0.0f),
			fillR(0.0f),
			fillG(0.0f),
			fillB(0.0f),
			pathWasMerged(false),
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
		void rearrange();
            // rearrange subpaths for backends which don't support them 
	private:
		// Inhibitors (declared, but not defined)
		const PathInfo& operator=(const PathInfo&);
		PathInfo(const PathInfo &);
	};

public:
	// = PUBLIC DATA

	const DriverDescription * Pdriverdesc; // pointer to the derived class' driverdescription
	bool simulateSubPaths; // simulate sub paths for backend that don't support it directly
	Image 		imageInfo;

	static FontMapper theFontMapper;
	static bool	verbose;

protected:
	// = PROTECTED DATA

	ostream &	outf;           // the output stream
	ostream &	errf;           // the error stream
	const char*		inFileName; // full name of input file
	const char*		outFileName; // full name of output file
	char*         	outDirName; 	// output path with trailing slash or backslash
	char*         	outBaseName; 	// just the basename (no path, no suffix)
	unsigned int	d_argc;
	char * *	d_argv;
	float           scale;
	RSString		outputPageSize; // set via -pagesize option
	float           currentDeviceHeight; // normally 792 pt; used for flipping y values.
	float           currentDeviceWidth;  
	float           x_offset;
	float           y_offset;
	unsigned int    currentPageNumber;
	bool			domerge;
	const char *	defaultFontName; // name of default font 
	bool			ctorOK;			// indicated Constructor failure
									// returned via driverOK() function


private:
	// = PRIVATE DATA

	bool    	page_empty;	// indicates whether the current page is empty or not
	char * 		driveroptions; // string containing options for backend
	PathInfo 	p1,p2;
	PathInfo * 	currentPath; // for filling from lexer
protected: // for drvrtf
	PathInfo * 	outputPath;  // for output driver
private:
	PathInfo * 	lastPath;    // for merging
	TextInfo 	textInfo_;
	TextInfo 	lasttextInfo_; // for saving font settings


public:
	// = PUBLIC METHODS

	// = CONSTRUCTION, DESTRUCTION AND COPYING

	drvbase(
		const char * driverOptions_p,
		ostream & theoutStream,
		ostream & theerrStream,		
		const char* nameOfInputFile_p,
		const char* nameOfOutputFile_p,
		const float scalefactor,
		const RSString & pagesize,
		const DriverDescription * Pdriverdesc_p
	); // constructor
	virtual ~drvbase();  		// destructor

	// = BACKEND GENERIC FUNCTIONS 
        // These functions are not backend specific and shouldn't have to be
        // changed for new backends

	void		startup(bool merge);

	void 		finalize(); // needed because base destructor will be called after
				    // derived destructor

	void		setdefaultFontName(const char * n) {defaultFontName = n;}

	void            setCurrentDeviceHeight(const float deviceHeight) 
			{ currentDeviceHeight = deviceHeight; }

	void            setCurrentDeviceWidth(const float deviceWidth) 
			{ currentDeviceWidth = deviceWidth; }

	float           getScale() const { return scale; }

	const RSString & getPageSize() const { return outputPageSize; }

	bool		fontchanged() const { return ! textInfo_.samefont(lasttextInfo_); }
	bool		textcolorchanged() const { return ! textInfo_.samecolor(lasttextInfo_); }

	void		setRGB(const float R,const float G, const float B)
			{ 
			 if ( ( R > 1.0 ) || ( G > 1.0 ) || ( B > 1.0 ) ||
			      ( R < 0.0 ) || ( G < 0.0 ) || ( B < 0.0 ) ) {
				errf << "Warning: color value out of range (0..1). Color change ignored." << R << ' ' << G << ' ' << B << endl;
			 } else {
			 	textInfo_.currentR = R ; textInfo_.currentG = G; textInfo_.currentB = B; 
			 	currentPath->edgeR = R ; currentPath->edgeG = G; currentPath->edgeB = B; 
			 	currentPath->fillR = R ; currentPath->fillG = G; currentPath->fillB = B; 
			 }
			}

	void    	showpage();


	// = DRAWING RELATED METHODS

	void		addtopath(basedrawingelement * newelement);

	unsigned int 	&numberOfElementsInPath() { return outputPath->numberOfElementsInPath; }
	unsigned int 	numberOfElementsInPath() const { return outputPath->numberOfElementsInPath; }

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

	void            setCurrentMiterLimit(const float miterLimit) 
			{ currentPath->currentMiterLimit = miterLimit; }

	void            setCurrentLineCap(const unsigned int capType) 
			{ currentPath->currentLineCap = capType; }

	void            setCurrentShowType(const showtype how) 
			{ currentPath->currentShowType = how; }

	void 		dumpPath();  // shows current path

	void		dumpRearrangedPathes(); // show the current subpathes after calling rearrange

	unsigned int nrOfSubpaths() const;

	void 		dumpImage();  // shows current image

	// = TEXT RELATED METHODS

 	void		setCurrentWidthParams(  const float ax,
						const float ay,
						const int Char,
						const float cx,
						const float cy,
				 		const float x_end = 0.0f, 
				 		const float y_end = 0.0f);

	void		setMappedtoisolatin1 (  const bool mapped )
			{ textInfo_.mappedtoIsoLatin1 = mapped; }

	void            setCurrentFontName(const char *const Name,bool is_non_standard_font);

	void            setCurrentFontFamilyName(const char *const Name);

	void            setCurrentFontFullName(const char *const Name);

	void            setCurrentFontWeight(const char *const Name);

	void            setCurrentFontSize(const float Size);

	void		setCurrentFontAngle(float value);

	float *		getCurrentFontMatrix() { return textInfo_.FontMatrix; }

	void    	dumpText(const char *const thetext,
				 const float x, 
				 const float y);



	// = BACKEND SPECIFIC FUNCTIONS

	// If a backend only deals with a special set of font names
	// the following function must return a 0 terminated list
	// of font names.
	virtual const char * const * 	knownFontNames() const { return 0; }

	// The next functions are virtual with a default empty implementation

	virtual void    show_image(const Image & imageinfo) {
		cerr << "show_image called, although backend does not support images" << endl;
		unused(&imageinfo);
	}

	// if during construction something may go wrong, a backend can
	// overwrite this function and return false in case of an error.
	// or it can just set the ctorOK to false.
	virtual bool driverOK() const { return ctorOK; } // some  

protected:
	// = PROTECTED METHODS

	showtype		currentShowType() const { return outputPath->currentShowType; }
	linetype		currentLineType() const { return outputPath->currentLineType; }
	unsigned int	currentLineCap() const { return outputPath->currentLineCap; }
	unsigned int	currentLineJoin() const { return outputPath->currentLineJoin; }
	float			currentMiterLimit() const { return outputPath->currentMiterLimit; }
	bool	 		isPolygon() const { return outputPath->isPolygon;} // whether current path was closed via closepath or not
	virtual bool	pathsCanBeMerged  (const PathInfo & p1, const PathInfo & p2) const;
	bool			pathWasMerged() const  { return outputPath->pathWasMerged; }
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

	void		add_to_page();


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
	drvbase(); // avoid default ctor
	drvbase(const drvbase &);
	drvbase & operator=(const drvbase&);

};

class DashPattern {
public:
	DashPattern(const char * patternAsSetDashString);
	~DashPattern();
	const RSString dashString;
	int nrOfEntries;
	float * numbers;
	float offset;
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
        const makeColorNameType makeColorName_ ;
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
//	basedrawingelement(unsigned int size_p) /*: size(size_p) */ {}
	virtual const Point &getPoint(unsigned int i) const = 0;
	virtual Dtype getType() const = 0;
	friend ostream & operator<<(ostream & out,const basedrawingelement &elem);
	friend bool operator==(const basedrawingelement & bd1, const basedrawingelement & bd2);
	virtual unsigned int getNrOfPoints() const = 0;
	virtual basedrawingelement* clone() const = 0; // make a copy
private:
//	const unsigned int size;
};


inline void copyPoints(unsigned int nr, const Point src[], Point target[])
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
	// : basedrawingelement(nr)
	{
#if defined (__GNUG__) || defined (_MSC_VER) && _MSC_VER >= 1100
	const Point  p[] = {Point(x1,y1),Point(x2,y2),Point(x3,y3)};
	copyPoints(nr,p,points);
#else
	// Turbo C++ hangs if the other solution is used.
	// and the HP CC compiler doesn't like it either
	// so use this for all compilers besides GNU and MS VC++
	// This, however, is somewhat slower than the solution above
	Point  * p = new Point[3];
	p[0] = Point(x1,y1);
	p[1] = Point(x2,y2);
	p[2] = Point(x3,y3);
	copyPoints(nr,p,points);
	delete [] p;
#endif

	}

	drawingelement(const Point p[])
	//: basedrawingelement(nr)
	{
//	for (unsigned int i = 0 ; i < nr ; i++ ) points[i] = p[i]; 
		copyPoints(nr,p,points);
	}
	drawingelement(const drawingelement<nr,curtype> & orig)
		//: basedrawingelement(nr)
	{ // copy ctor
		if (orig.getType() != curtype ) {
			cerr << "illegal usage of copy ctor of drawingelement" << endl;
			exit(1);
		} else {
			copyPoints(nr,orig.points,points);
		}
	}
	virtual basedrawingelement* clone() const {
		return new drawingelement<nr,curtype>(*this);
	}
	const Point &getPoint(unsigned int i) const  { 
		return points[i]; 
	}
	virtual Dtype getType() const 		     { return (Dtype) curtype; }
						// This cast (Dtype) is necessary
						// to eliminate a compiler warning
						// from the SparcCompiler 4.1.
						// although curtype is of type Dtype
	virtual unsigned int getNrOfPoints() const { return nr; }
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
typedef drawingelement<(unsigned int) 0,closepath>  Closepath;
typedef drawingelement<(unsigned int) 3,curveto> 	Curveto;


#define derivedConstructor(Class)			\
	Class(const char * driveroptions_p, 	\
	       ostream & theoutStream, 			\
	       ostream & theerrStream, 			\
		   const char* nameOfInputFile_p,	\
	       const char* nameOfOutputFile_p,	\
		   const float scalefactor_p,	    \
		   const RSString & pagesize,		\
		   const DriverDescription * descptr)	

#define constructBase drvbase(driveroptions_p,theoutStream,theerrStream,nameOfInputFile_p,nameOfOutputFile_p,scalefactor_p,pagesize,descptr)


class DescriptionRegister
{
	enum {maxelems = 100 };
public:
	DescriptionRegister() { 
		ind = 0;
		for (int i = 0; i < maxelems; i++) rp[i] = 0; 
	//	cout << " R constructed " << (void *) this << endl;
	}
	~DescriptionRegister() {
	//	cout << " R destructed " << (void *) this << endl;
	}

	static DescriptionRegister& getInstance();

	void registerDriver(DriverDescription* xp);
	void mergeRegister(ostream & out,const DescriptionRegister & src,const char * filename);
	void explainformats(ostream & out) const;
	const DriverDescription * getdriverdesc(const char * drivername) const;

	DriverDescription* rp[maxelems];

	int nrOfDescriptions() const { return ind; }
private:
	
	int ind;
};

extern DescriptionRegister* globalRp;

//extern __declspec ( dllexport) "C" {
//not needed // DescriptionRegister* getglobalRp();
typedef DescriptionRegister* (*getglobalRpFuncPtr)();
//}

//class Rinit 
//{
//public:
//	Rinit() { if (!globalRp) {globalRp = new DescriptionRegister; ref = 1 ; } else { ref++;} }
//	~Rinit() { ref--; if (ref == 0) delete globalRp; }
//
//private:
//	static	int ref;
//};

//static Rinit Rinit_var;

//now in drvdesc.h typedef bool (*checkfuncptr)();
// static bool nocheck() { return true; }

typedef bool (*checkfuncptr)(void);
class drvbase;

class DriverDescription {
public:
	enum opentype {noopen, normalopen, binaryopen};
	DriverDescription(const char * s_name, 
			const char * expl, 
			const char * suffix_p, 
			const bool 	backendSupportsSubPathes_p,
			const bool 	backendSupportsCurveto_p,
			const bool 	backendSupportsMerging_p, // merge a separate outline and filling of a polygon -> 1. element
			const bool 	backendSupportsText_p,
			const bool 	backendSupportsImages_p,  // supports bitmap images
			const opentype  backendFileOpenType_p,
			const bool	backendSupportsMultiplePages_p,
			checkfuncptr checkfunc_p = 0);
	virtual ~DriverDescription() {}

	virtual drvbase * CreateBackend (const char * driveroptions_P,
			 ostream & theoutStream, 
			 ostream & theerrStream,   
			 const char* nameOfInputFile,
		     const char* nameOfOutputFile,
			 const float scalefactor,
			 const RSString & pagesize
			 ) const;
	virtual unsigned int getdrvbaseVersion() const { return 0; } // this is only needed for the driverless backends (ps/dump/gs)

 // Data members
	const char * const symbolicname;
	const char * const explanation;
	const char * const suffix;
	const char * const additionalInfo;
	const bool 	backendSupportsSubPathes;
	const bool 	backendSupportsCurveto;
	const bool 	backendSupportsMerging; // merge a separate outline and filling of a polygon -> 1. element
	const bool 	backendSupportsText;
	const bool 	backendSupportsImages;  // supports bitmap images
	const opentype  backendFileOpenType;
	const bool	backendSupportsMultiplePages;
	RSString filename; // where this driver is loaded from
	const checkfuncptr checkfunc;
};

class DescriptionRegister;

template <class T>
class DriverDescriptionT : public DriverDescription {
public:
	DriverDescriptionT(const char * s_name, 
			const char * expl, 
			const char * suffix_p, 
			const bool 	backendSupportsSubPathes_p,
			const bool 	backendSupportsCurveto_p,
			const bool 	backendSupportsMerging_p, // merge a separate outline and filling of a polygon -> 1. element
			const bool 	backendSupportsText_p,
			const bool 	backendSupportsImages_p,  // supports bitmap images
			const DriverDescription::opentype  backendFileOpenType_p,
			const bool	backendSupportsMultiplePages_p,
			checkfuncptr checkfunc_p = 0 ):
	DriverDescription( 
			s_name, 
			expl, 
			suffix_p, 
			backendSupportsSubPathes_p,
			backendSupportsCurveto_p,
			backendSupportsMerging_p, 
			backendSupportsText_p,
			backendSupportsImages_p, 
			backendFileOpenType_p,
			backendSupportsMultiplePages_p,
			checkfunc_p
			)
		{}
	drvbase * CreateBackend (
			const char * driveroptions_P,
		    ostream & theoutStream, 
		    ostream & theerrStream,   
			const char* nameOfInputFile,
	       	const char* nameOfOutputFile,
			const float scalefactor,
			const RSString & pagesize
			 ) const
	{ 
	  return new T(driveroptions_P, theoutStream, theerrStream,nameOfInputFile,nameOfOutputFile, scalefactor,pagesize,this); 
	} 
//	virtual void DeleteBackend(drvbase * & ptr) const { delete (T*) ptr; ptr = 0; }
	virtual unsigned int getdrvbaseVersion() const { return drvbaseVersion; }
};



inline float min(float x,float y)
{
	return (x<y) ? x:y;
}

inline float max(float x,float y)
{
	return (x>y) ? x:y;
}


#endif
 
 
 
