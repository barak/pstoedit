/* 
   drvbase.cpp : This file is part of pstoedit
   Basic, driver independent output routines

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

#include "drvbase.h"

#include I_stdlib
#include I_iostream
#include I_iomanip

#include I_string_h

#include I_strstream

#include "version.h"

#if defined(_WIN32)
// for registry access
#include <windows.h> 
#endif

#include "miscutil.h"

static void splitFullFileName(const char* fullName, char* pathName, char* baseName, char* fileExt)
{
    if(fullName == NULL) return;

    char* fullName_T = cppstrdup(fullName);
    char* baseName_T;
    char* c;
#if defined(unix) || defined(__unix__) || defined(_unix) || defined(__unix) || defined(__EMX__)
    c = strrchr(fullName_T, '/');
#else
    c = strrchr(fullName_T, '\\');
#endif
    if(c != NULL)
    {
       baseName_T = cppstrdup(c+1);
       *(c+1) = 0;
       if(pathName != NULL) strcpy(pathName, fullName_T);
    } else {
       baseName_T = cppstrdup(fullName_T);
       if(pathName != NULL) strcpy(pathName, "");
    }

    c = strrchr(baseName_T, '.');
    if(c != NULL)
    {
       if(fileExt != NULL) strcpy(fileExt, c+1);
       *c = 0;
       if(baseName != NULL) strcpy(baseName, baseName_T);
    } else {
       if(fileExt != NULL) strcpy(fileExt, "");
       if(baseName != NULL) strcpy(baseName, baseName_T);
    }
    delete baseName_T;
    delete fullName_T;
}




drvbase::drvbase(const char * driveroptions_p,ostream & theoutStream, 
		 ostream & theerrStream,
		 const char* nameOfInputFile_p,
		 const char* nameOfOutputFile_p,
		 const float scalefactor,
		 const RSString & pagesize,
		 const DriverDescription * Pdriverdesc_p
		 ) 
		: // constructor
	Pdriverdesc		(Pdriverdesc_p),
	simulateSubPaths (false),
    outf                (theoutStream),
    errf                (theerrStream),
	inFileName(nameOfInputFile_p),
	outFileName(nameOfOutputFile_p),
    outDirName		(0),
    outBaseName		(0),

    d_argc		(0),
    d_argv		(0),
    scale               (scalefactor),
	outputPageSize(pagesize),
    // set some common defaults
    currentDeviceHeight (792.0f * scale),
    currentDeviceWidth  (640 * scale),
    x_offset            (0.0f),
    y_offset            (0.0f),
    currentPageNumber   (0),
    domerge    		(false),
	defaultFontName (0),
	ctorOK			(true),
    page_empty          (1),
    driveroptions	(0),
    // default for p1 and p2
    currentPath		(0),
    outputPath		(0),
    lastPath		(0)
    // default for textInfo_ and lasttextInfo_
{ 
    // verbose = (getenv("PSTOEDITVERBOSE") != 0);
    if (verbose) { 
		errf << "verbose mode turned on\n" << endl;
    }

	if (nameOfOutputFile_p) {
		outDirName  = new char [strlen(nameOfOutputFile_p)+1];
		outBaseName = new char [strlen(nameOfOutputFile_p)+1];
		splitFullFileName(nameOfOutputFile_p, outDirName, outBaseName, NULL);
		if (verbose) {
			errf << " nameofOutputFile " << nameOfOutputFile_p;
			errf << " outDirName " << outDirName;
			errf << " outBaseName " << outBaseName;
			errf << endl;
		}
	}


    // preparse driveroptions and build d_argc and d_argv
    if (driveroptions_p) {
    	driveroptions = cppstrdup(driveroptions_p);

		istrstream optstream(driveroptions,strlen(driveroptions));
		const long startOfStream = optstream.tellg();
		char currentarg[100];
    	// first count number of arguments
    	while(!optstream.eof()) {
		optstream.width(sizeof(currentarg));
		optstream >> currentarg;
		d_argc++;
	}
	d_argv = new char *[d_argc+1];
	// now fill d_args array;
	optstream.seekg(startOfStream); // reposition to start
	optstream.clear();
	d_argc=0;
    	while(!optstream.eof()) {
		optstream >> currentarg;
		d_argv[d_argc] = cppstrdup(currentarg);
		d_argc++;
	}
	d_argv[d_argc] = 0;
    	if (verbose) { 
			errf << "got " << d_argc << " driver arguments" << endl;
			for (unsigned int i = 0; i < d_argc ; i++ ) {
				errf << "Driver option " << i << ":" << d_argv[i] << endl;
			}
		}
    }

    // init segment info for first segment
    // all others will be updated with each newsegment


    currentPath = &p1;
    lastPath    = &p2;
    outputPath  = currentPath;
    
    if ((p1.path == 0) || (p2.path == 0) ) {
		errf << "new failed in drvbase::drvbase " << endl;
		exit(1);
    }

    textInfo_.thetext.copy("");
	setCurrentFontName("Courier",1);
    setCurrentFontFamilyName("Courier");
    setCurrentFontWeight("Regular");
    setCurrentFontFullName("Courier");
    setCurrentFontSize(10.0f);
	lasttextInfo_ = textInfo_;
	lasttextInfo_.currentFontSize = -textInfo_.currentFontSize; // to force a new font the first time.
	lasttextInfo_.currentR = textInfo_.currentR +1; // to force new color
}

drvbase::~drvbase() { 
	currentPath = 0;
	lastPath    = 0;
	outputPath  = 0;
	for (unsigned int i = 0; i < d_argc ; i++ ) {
		delete [] (d_argv[i]);  
		d_argv[i] = 0;
	}
	delete [] d_argv;
	if (driveroptions) {
		delete driveroptions;
	}
	delete[] outDirName;
	delete[] outBaseName;
}

void drvbase::startup(bool merge)
{
	domerge = false; // default
	if (merge) {
		if ( Pdriverdesc->backendSupportsMerging ) {
			domerge = true;
		} else {
			errf << "the selected backend does not support merging, -merge ignored" << endl;
		}
	}
}

void drvbase::finalize()
{
// needed because base destructor is called after derived destructor
	outputPath->clear(); // define past the end path as empty
	// close page (if no explicit showpage was done)
	showpage();
}
void	drvbase::showpage()
{
	dumpPath(); // dump last path 
	if (!page_empty) {
		close_page();
	}
	page_empty = 1;
}

bool drvbase::pathsCanBeMerged(const PathInfo & path1, const PathInfo & path2) const
{
	//
	// two paths can be merged if one of them is a stroke and the
	// other a fill or eofill AND
	// all pathelements are the same
	//
	// This is a default implementation which allows only solid edges since
	// most backends support only such edges.
	// If a backend allows more, it can overwrite this function
	// 
	if (((path1.currentShowType == stroke && path1.currentLineType == solid && 
			( (path2.currentShowType == fill) || (path2.currentShowType == eofill) ))  ||
	     (path2.currentShowType == stroke && path2.currentLineType == solid && 
			( (path1.currentShowType == fill) || (path1.currentShowType == eofill) ))  )
	   && (path1.numberOfElementsInPath == path2.numberOfElementsInPath)  ) {
 		//errf << "Pathes seem to be mergeable" << endl;
		for (unsigned int i = 0; i < path1.numberOfElementsInPath ; i++) {
			const basedrawingelement *bd1 = path1.path[i];
			const basedrawingelement *bd2 = path2.path[i];
//			if (! *(path1.path[i]) == *(path2.path[i]) ) return 0;
			//errf << "comparing " << *bd1 << " with " << *bd2 << endl;
			if (! (*bd1 == *bd2) ) return 0;
		}
 		//errf << "Pathes are mergeable" << endl;
		return 1;
	} else {
		return 0;
	}
}

const basedrawingelement & drvbase::pathElement(unsigned int index) const 
{
	return *(outputPath->path[index+outputPath->subpathoffset]);
}

bool operator==(const basedrawingelement & bd1, const basedrawingelement & bd2) 
{ 
	if (bd1.getType() != bd2.getType() ) {
		return 0;
	} else {
		for (unsigned int i = 0; i < bd1.getNrOfPoints(); i++ ) {
			if (! (bd1.getPoint(i) == bd2.getPoint(i)) ) return 0;
		}
	}
	return 1;
}

static bool textIsWorthToPrint(const char * text) 
{
	// check whether it contains just blanks. This makes
	// problems, e.g. with the xfig backend.
	if (strlen(text) > 0) {
		const char * cp = text;
		while (*cp) {
			if (*cp != ' ') return true;
			cp++;
		}
	}
	return false;
}

void drvbase::dumpText(const char * const thetext,
			const float x, 
			const float y)
{
	if (textIsWorthToPrint(thetext)) {
  		dumpPath(); // dump last path to avoid wrong sequence of text and graphics
  		add_to_page();
		textInfo_.x = x;
  		textInfo_.y = y;
  		textInfo_.thetext.copy(thetext);
		const char * remappedFontName = drvbase::theFontMapper.mapFont(textInfo_.currentFontName);
		// errf << " Mapping of " << textInfo_.currentFontName << " returned " << (remappedFontName ? remappedFontName:" ") << endl;
		if (remappedFontName) {
			if (verbose) {
				errf << "Font remapped from '" << textInfo_.currentFontName << "' to '" << remappedFontName << endl;
			}
			textInfo_.currentFontName.copy(remappedFontName);
		}
  		show_text(textInfo_);
#if 0
		if ( (lasttextInfo_.y == textInfo_.y) && (lasttextInfo_.x_end >= textInfo_.x) && (lasttextInfo_.x < textInfo_.x) && lasttextInfo_.samefont(textInfo_) ) {
			if (verbose) {
				errf << "Text overlap ! '" << lasttextInfo_.thetext.value() << "' and '" << textInfo_.thetext.value() << endl;
			}
		}
#endif
		lasttextInfo_ = textInfo_; // save for font and color comparison
	}
}

void drvbase::setCurrentWidthParams(const float ax,
				    const float ay,
				    const int Char,
				    const float cx,
				    const float cy,
				    const float x_end,
				    const float y_end)
{
	textInfo_.ax = ax;
	textInfo_.ay = ay;
	textInfo_.Char = Char;
	textInfo_.cx = cx;
	textInfo_.x_end = x_end;
	textInfo_.y_end = y_end;
	textInfo_.cy = cy;
}

void drvbase::setCurrentFontName(const char * const Name,bool is_non_standard_font) 
{ 
	textInfo_.currentFontName.copy(Name); 
	textInfo_.is_non_standard_font = is_non_standard_font;
}

void drvbase::setCurrentFontFamilyName(const char * const Name)
	{ textInfo_.currentFontFamilyName.copy(Name); }

void drvbase::setCurrentFontFullName(const char * const Name)
	{ textInfo_.currentFontFullName.copy(Name); }

void drvbase::setCurrentFontWeight(const char * const Name)
	{ textInfo_.currentFontWeight.copy(Name); }

void drvbase::setCurrentFontSize(const float Size)
	{ /* errf << "setting Size to " << Size << endl; */  textInfo_.currentFontSize = Size ; }

void drvbase::setCurrentFontAngle(float value)
	{ textInfo_.currentFontAngle = value; }

bool drvbase::is_a_rectangle() const
{
//in most cases of rectangles there are 5 Elements
	if (numberOfElementsInPath()!=5) 
		return 0;
		
//first and last points are identical
	if (pathElement(0).getPoint(0).x_ != pathElement(4).getPoint(0).x_ ||
		pathElement(0).getPoint(0).y_ != pathElement(4).getPoint(0).y_)
		return 0;
	
	int start_horic_test;
	int start_vert_test;

	if (pathElement(0).getPoint(0).x_ == pathElement(1).getPoint(0).x_) {
		start_horic_test=0;
		start_vert_test=1;
	}
	else {
		start_horic_test=1;
		start_vert_test=0;
	}

	{for (int i = start_horic_test; i < 4; i++,i++) 
		if (pathElement(i).getPoint(0).x_ != pathElement((i+1)%4).getPoint(0).x_)
			return 0;
	}
			
	{for (int i = start_vert_test; i < 4; i++,i++) 
		if (pathElement(i).getPoint(0).y_ != pathElement((i+1)%4).getPoint(0).y_)
			return 0;
	}
	return 1;
} 	

void drvbase::add_to_page()
{
    if (page_empty) {
		page_empty = 0;
		currentPageNumber++;
		open_page();
    }
}


DashPattern::	DashPattern(const char * patternAsSetDashString) :
	 dashString(patternAsSetDashString),
	 nrOfEntries(-1),
	 numbers(0),
	 offset(0)
{
	const char * pattern = patternAsSetDashString;
	// first count number of ' ' in pattern to determine number of entries
	// we normally have one less than number of blanks
	// line looks like: " [ 2.25 6.75 ] 0.0 setdash"

	while ((*pattern) && (*pattern != ']' ) ) {
		if (*pattern == ' ') nrOfEntries++;
		pattern++;
	}

	// errf << nr_of_entries << " entries found in " << pattern << endl;
	if (nrOfEntries > 0) {	
		pattern = patternAsSetDashString;
		// now get the numbers
		// repeat the numbers, if number of entries is odd
		int rep = nrOfEntries % 2; // rep is 1 for odd numbers 0 for even
		numbers = new float[nrOfEntries *(rep + 1)];
		int cur = 0;
#if 1
		for (int i = 0; i <= rep ; i++ ) {
			pattern = patternAsSetDashString;
			while ((*pattern) && (*pattern != ']' ) ) {
				if (*pattern == ' ' && ( *(pattern+1) != ']' ) ) {
					float f = (float) atof(pattern);
					numbers[cur] = f;
				 	// errf << d_numbers[cur] << endl;
					cur++;
				}
				pattern++;
			}
		}
		if ( *(pattern+1) == ']' ) {
			offset = (float) atof(pattern +2);
		}
#else
		// this is the "C++" version. But this doesn't work with the GNU library under Linux
		for (int i = 0; i <= rep ; i++ ) {
			// on some systems istrstreams expects a non const char *
			// so we need to make a copy
			char * localpattern = new char[strlen(pattern+1) + 1];
			strcpy(localpattern,pattern+1); // skip leading [
			istrstream instream(localpattern); 
			while (!instream.fail()) {
				float f;
				instream >> f;
				if (!instream.fail() ) {
					d_numbers[cur] = f;
				 	// errf << d_numbers[cur] << endl;
					cur++;
				}
			}
			delete [] localpattern;
		}
#endif
	}
}

DashPattern::~DashPattern()
{
	delete [] numbers; 
	numbers=0;
	nrOfEntries = 0;
}


void drvbase::guess_linetype()
{
	DashPattern dp(dashPattern());
	const float * const d_numbers = dp.numbers;
	const int nr_of_entries = dp.nrOfEntries;

	drvbase::linetype curtype = solid;
	if (nr_of_entries > 0) {
		int rep = nr_of_entries % 2; // rep is 1 for odd numbers 0 for even
		// now guess a pattern from
		// solid, dashed, dotted, dashdot, dashdotdot ; // corresponding to the CGM patterns
		switch ( nr_of_entries *(rep + 1) ) {
		case 2: 
			if (d_numbers[1] == 0.0f) {
				curtype = drvbase::solid; // if off is 0 -> solid
			} else if ( (d_numbers[0] / d_numbers[1]) > 100 ) {
				curtype = drvbase::solid; // if on/off > 100 -> use solid
			} else if (d_numbers[0] < 2.0f) {
				// if on is < 2 then always dotted
				// ok we miss '.             .             .'
				curtype = drvbase::dotted;
			} else {
				curtype = drvbase::dashed;
			}
			break;
		case 4:
			if (( d_numbers[1] == 0.0f) && (d_numbers[3] == 0.0f) ) {
				curtype = drvbase::solid; // if off is 0 -> solid
			} else if ( (d_numbers[0] < 2.0f) || (d_numbers[2] < 2.0f) ) {
				curtype = drvbase::dashdot;
			} else {
				curtype = drvbase::dashed;
			}
			break;
		case 6:
			if (( d_numbers[1] == 0.0f) && (d_numbers[3] == 0.0f) && (d_numbers[5] == 0.0f) ) {
				curtype = drvbase::solid; // if off is 0 -> solid
			} else if ( (d_numbers[0] < 2.0f) || (d_numbers[2] < 2.0f)  || (d_numbers[2] < 2.0f) ) {
				curtype = drvbase::dashdotdot;
			} else {
				curtype = drvbase::dashed;
			}
			break;
		default:
			curtype = drvbase::dashed;
			break;
		}
	} else {
		// no entry
		curtype = drvbase::solid;
	}
//	 errf << "linetype from " << dashPattern() << " is " << curtype << endl;
	setCurrentLineType(curtype);
}

void drvbase::dumpImage()
{
	dumpPath(); // dump last path to avoid wrong sequence of text and graphics
	add_to_page();
	imageInfo.calculateBoundingBox();
	show_image(imageInfo);
	delete [] imageInfo.data ;
	imageInfo.nextfreedataitem = 0;
	imageInfo.data = 0;
}

unsigned int drvbase::nrOfSubpaths() const
{
  unsigned int nr = 0;
  for (unsigned int n = 0; n+1 < numberOfElementsInPath(); n++) {
    const basedrawingelement & elem = pathElement(n);
    if (elem.getType() == moveto)
      nr++ ;
  }
  return nr;
}


void drvbase::dumpRearrangedPathes()
{

		// Count the subpaths
	unsigned int numpaths = nrOfSubpaths();
	if (verbose) errf << "numpaths: " << numpaths << endl;
	// Rearrange the path if necessary
	if ((numpaths > 1) && (currentLineWidth() == 0.0) &&
		(currentShowType() != drvbase::stroke) ) {
		if (verbose) errf << "Starting rearrangment of subpaths" << endl;
		outputPath->rearrange();     
		numpaths = nrOfSubpaths();
	}
	if (!numpaths) numpaths = 1;
	
	const unsigned int origCount = numberOfElementsInPath();
	unsigned int start = 0; 
	for(unsigned int i = 0; i < numpaths; i++) {
		unsigned int end = start;
		outputPath->subpathoffset = 0;
		while(1)       // Find the next end index
		{
	            end++;
 			if (end >= origCount )
                   break;
 			else if(pathElement(end).getType() == moveto)
                   break;
		}
		if (end <= origCount) {
			if (verbose) errf  << "dumping subpath from " << start << " to " << end << endl;
			outputPath->subpathoffset = start;
			outputPath->numberOfElementsInPath = end - start;
			show_path(); // from start to end
		}
		start = end;
	}
	outputPath->numberOfElementsInPath = origCount;
	outputPath->subpathoffset = 0;
}

void drvbase::dumpPath()
{
    guess_linetype(); // needs to be done here, because we must write to currentpath
    if (currentPath->numberOfElementsInPath == 2) {
		// a polygon with two points is drawn as a line
		currentPath->isPolygon=false;
		currentPath->currentShowType=drvbase::stroke;
    }
    if (currentPath->currentShowType!=drvbase::stroke){
    	/* don't show border with fill */
    	setCurrentLineWidth(0.0f);  
    }

    if (domerge && pathsCanBeMerged(p1,p2)) {
	// make p1 the outputPath and clear p2
		if (verbose) {
			errf << "Path " << p1.nr << " type " << (int) p1.currentShowType << endl;
			errf << p1.fillR << " " << p1.fillG << " " << p1.fillB << endl;
			errf << p1.edgeR << " " << p1.edgeG << " " << p1.edgeB << endl;
			errf << p1.currentLineWidth << endl;

			errf << "Path " << p2.nr << " type " << (int) p2.currentShowType << endl;
			errf << p2.fillR << " " << p2.fillG << " " << p2.fillB << endl;
			errf << p2.edgeR << " " << p2.edgeG << " " << p2.edgeB << endl;
			errf << p2.currentLineWidth << endl;
			errf << " have been merged\n";
		}
		// merge p2 into p1
		if (p1.currentShowType == stroke) {
		// p2 is the fill
			p1.currentShowType = p2.currentShowType;
			p1.fillR = p2.fillR;
			p1.fillG = p2.fillG;
			p1.fillB = p2.fillB;
		} else {
		// p1 is the fill, so copy the line parameters from p2
			p1.currentLineWidth = p2.currentLineWidth;
			p1.edgeR = p2.edgeR;
			p1.edgeG = p2.edgeG;
			p1.edgeB = p2.edgeB;
		}
		if (verbose) {
			errf << " result is \n";
			errf << "Path " << p1.nr << " type " << (int) p1.currentShowType << endl;
			errf << p1.fillR << " " << p1.fillG << " " << p1.fillB << endl;
			errf << p1.edgeR << " " << p1.edgeG << " " << p1.edgeB << endl;
			errf << p1.currentLineWidth << endl;
		}
		outputPath = &p1;
		p1.pathWasMerged = true;
		p2.clear();
    } else {
		outputPath = lastPath;
    }
    if (numberOfElementsInPath() > 0 ) {
	// nothing to do for empty pathes
	// pathes may be empty due to a merge operation

    	if (verbose) {
    		errf << "working on";
			switch ( currentShowType() ) {
			case  drvbase::stroke:
				errf << " stroked ";
				break;
			case  drvbase::fill:
				errf << " filled ";
				break;
			case  drvbase::eofill:
				errf << " eofilled ";
				break;
			default:
				break;
			}
			errf << "path " << currentNr()  << " with " << 
			numberOfElementsInPath() << " elements" << endl;
		}

		if (numberOfElementsInPath() > 1) {
	// cannot draw single points 
    		add_to_page();
    		if (isPolygon()) { /* PolyGon */
				if (is_a_rectangle()) {
					float llx,lly,urx,ury;
					llx =	min( 	min(pathElement(0).getPoint(0).x_,pathElement(1).getPoint(0).x_),
							min(pathElement(2).getPoint(0).x_,pathElement(3).getPoint(0).x_));
					urx =	max( 	max(pathElement(0).getPoint(0).x_,pathElement(1).getPoint(0).x_),
							max(pathElement(2).getPoint(0).x_,pathElement(3).getPoint(0).x_));
					lly =	min( 	min(pathElement(0).getPoint(0).y_,pathElement(1).getPoint(0).y_),
							min(pathElement(2).getPoint(0).y_,pathElement(3).getPoint(0).y_));
					ury =	max( 	max(pathElement(0).getPoint(0).y_,pathElement(1).getPoint(0).y_),
							max(pathElement(2).getPoint(0).y_,pathElement(3).getPoint(0).y_));

					show_rectangle(llx,lly,urx,ury);
				} else {
					if (simulateSubPaths) dumpRearrangedPathes();
					else show_path();
				}
			} else { /* PolyLine */
				if (simulateSubPaths) dumpRearrangedPathes(); 
				else show_path();
    		}
    	}
		// cleanup
		outputPath->clear();
    }

    // swap current and last pointers
    PathInfo *help 	  = currentPath; 
          currentPath = lastPath;
    	  lastPath    = help;

    currentPath->copyInfo(*help); // initialize next path with state of last path
				  // currentPath is the path filled next by lexer

    outputPath = currentPath;
}

void	drvbase::addtopath(basedrawingelement * newelement)
{
	if (newelement) {
		if (currentPath->numberOfElementsInPath < maxElements) {
			currentPath->path[currentPath->numberOfElementsInPath] = newelement;
#ifdef DEBUG
			cout << "pathelement " << currentPath->numberOfElementsInPath << " added "
			     << *newelement << endl;
#endif
			currentPath->numberOfElementsInPath++;
		} else {
			errf << "Fatal: number of path elements exceeded. Increase maxElements in drvbase.h" << endl;
			exit(1);
		}
	} else {
		errf << "Fatal: newelement is NULL in addtopath " << endl;
		exit(1);
	}
}

void drvbase::PathInfo::clear() 
{
	for (unsigned int i = 0 ; i < numberOfElementsInPath; i++ ) {
		delete path[i];
		path[i] = 0;
	}
	numberOfElementsInPath = 0;
	pathWasMerged = false;
}

void drvbase::PathInfo::copyInfo(const PathInfo & p) 
{
	// copies the whole path state except the path array
	currentShowType = p.currentShowType;
// wogl: I added the following three ones since
// these were obviously missing
	currentLineType = p.currentLineType;
	currentLineCap	= p.currentLineCap;
	currentLineJoin = p.currentLineJoin;
	currentMiterLimit = p.currentMiterLimit;
	nr = p.nr;
	// Path is not copied path(0),
	isPolygon = p.isPolygon;
	// numberOfElementsInPath = p.numberOfElementsInPath;
	currentLineWidth = p.currentLineWidth;
	edgeR = p.edgeR;
	edgeG = p.edgeG;
	edgeB = p.edgeB;
	fillR = p.fillR;
	fillG = p.fillG;
	fillB = p.fillB;
	dashPattern = p.dashPattern;
}

ostream & operator<<(ostream & out,const basedrawingelement &elem)
{
	out << "type: " << (int) elem.getType() << " params: " ;
	for (unsigned int i = 0 ; i < elem.getNrOfPoints() ; i++ ) {
		out << elem.getPoint(i).x_ << " "
		    << elem.getPoint(i).y_ << " "; 
	}
	out << endl;
	return out;
}

ColorTable::ColorTable(	const char * const * defaultColors,
		   	const unsigned int numberOfDefaultColors,
		   	makeColorNameType makeColorName)  :
			defaultColors_(defaultColors),
			numberOfDefaultColors_(numberOfDefaultColors),
			makeColorName_(makeColorName) 
{ 
//dbg	cerr << " Constructing a color table with " << numberOfDefaultColors << " default colors" << endl;
	for (unsigned int i = 0 ; i< maxcolors; i++) newColors[i] = 0;
//dbg	cerr << 1/(1/numberOfDefaultColors) << endl;
}

ColorTable::~ColorTable() 
{
	unsigned int current= 0;
	while (newColors[current] != 0) {
		delete [] newColors[current];
		current++;
	}
}

unsigned int ColorTable::getColorIndex(float r, float g, float b)
{
// registers a possibly new color and returns the index 
// under which the color was registered
	const char * cmp = makeColorName_(r,g,b);
	for (unsigned int i = 0; i < numberOfDefaultColors_ ; i++ )
	{
		if (strcmp(cmp,defaultColors_[i]) == 0) {
			return i;
		}
	}
// look in new colors
	unsigned int j = 0;
	for (j = 0; ((j < maxcolors) && (newColors[j] != 0)) ; j++ )
	{
		if (strcmp(cmp,newColors[j]) == 0) {
			return j+numberOfDefaultColors_;
		}
	}
// not found so far
// j is either maxcolors or the index of the next free entry
// add a copy to newColors
	if (j < maxcolors) {
		newColors[j] = new char[strlen(cmp) +1];
		strcpy(newColors[j],cmp);
		return j+numberOfDefaultColors_;
	} else {
//		cerr << "running out of colors" << endl;
		return 0;
	}

}

const char * const ColorTable::getColorString(float r, float g, float b)
{
	return getColorString(getColorIndex(r,g,b));
}

bool ColorTable::isKnownColor(float r, float g, float b) const
{
// Possible improvements:
// could return the next free entry as negative number in case
// the color is not found. This would make it possible to
// use this function in getColorEntry as well, or (better)
// make a pure registercolor(index,.....) instead of
// getColorEntry.
	const char * cmp = makeColorName_(r,g,b);
	for (unsigned int i = 0; i < numberOfDefaultColors_ ; i++ )
	{
		if (strcmp(cmp,defaultColors_[i]) == 0) {
			return true;
		}
	}
	// look in new colors
	unsigned int j = 0;
	for (j = 0; ((j < maxcolors) && (newColors[j] != 0)) ; j++ )
	{
		if (strcmp(cmp,newColors[j]) == 0) {
			return true; // j+numberOfDefaultColors_;
		}
	}
	// not found so far
	return false; 
}

const char * const ColorTable::getColorString(unsigned int index) const
{
	return (index < numberOfDefaultColors_) ? defaultColors_[index] :
					  	  newColors[index - numberOfDefaultColors_];
}


// the global static FontMapper 
FontMapper drvbase::theFontMapper;
bool drvbase::verbose = false;

const DriverDescription * DescriptionRegister::getdriverdesc(const char * drivername) const
{
	unsigned int i = 0;
	while(rp[i] != 0) {
		if ( ( strcmp(drivername,rp[i]->symbolicname) == 0 )  ) {
			return	rp[i];
		}
		i++;
	}
	return 0;
}

void DescriptionRegister::explainformats(ostream & out) const
{

	out << "Available formats :\n";
	unsigned int i = 0;
	while(rp[i] != 0) {
		out << '\t' << rp[i]->symbolicname << ":\t" ;
		if (strlen(rp[i]->symbolicname) <7) {
			out << '\t';
		}
		out << rp[i]->explanation << " " << rp[i]->additionalInfo;
#if 0
		if (rp[i]->checkfunc) {
			if (!(rp[i]->checkfunc())) {
				out << " (no valid key found)";
			}
		}
#endif
		out << "\t(" << rp[i]->filename << ")"; 
		out << endl;
		i++;
	}
}
void DescriptionRegister::mergeRegister(ostream & out,const DescriptionRegister & src,const char * filename)
{
		int i = 0;
		while(src.rp[i]) {
			const unsigned int srcversion = src.rp[i]->getdrvbaseVersion();
			if (srcversion != 0) {
				if(srcversion == drvbaseVersion) {
					src.rp[i]->filename = filename;
					registerDriver(src.rp[i]);
				} else {
					out << src.rp[i]->explanation << " - backend has other version than expected by pstoedit core " << srcversion << " <> " <<  drvbaseVersion << endl;
					out << "The pstoedit.dll (core) and the additional DLLs (plugins.dll or importps.dll) must have the same version number." << endl;
				}
			}
			i++;
		}
}
void DescriptionRegister::registerDriver(DriverDescription* xp) { 
	//	cout << " registering " << (void *) xp << endl;
		// check for duplicate:
		for (int i = 0; i < ind; i++) {
			if (strcmp(rp[i]->symbolicname,xp->symbolicname) == 0) {
				// duplicate found
				if (xp->checkfunc() && !(rp[i]->checkfunc()) ) {
					// the new one has a license, so use it
					rp[i] = xp;
					return;
				}
			}
		}
		rp[ind] = xp; ind++; 
	}

// int Rinit::ref = 0;
DescriptionRegister* globalRp = 0;
#ifdef _MSC_VER
#define DLLEXPORT __declspec( dllexport ) 
#else
#define DLLEXPORT 
#endif
extern "C" DLLEXPORT DescriptionRegister* getglobalRp() { return &DescriptionRegister::getInstance(); }

Point Point::transform(const float matrix[6]) const
{
    const float tx = matrix[0]*x_ + matrix[2]*y_ + matrix[4];
    const float ty = matrix[1]*x_ + matrix[3]*y_ + matrix[5];
    return Point(tx,ty);
}

void Image::calculateBoundingBox()
{
    // calculate image bounding box

    Point P1(0.0f,0.0f);
    Point P1T = P1.transform(normalizedImageCurrentMatrix);
    Point P2((float) (width-1), (float) (height-1));
    Point P2T = P2.transform(normalizedImageCurrentMatrix);
    Point P3(0.0f , (float) (height-1));
    Point P3T = P3.transform(normalizedImageCurrentMatrix);
    Point P4((float) (width-1), 0.0f);
    Point P4T = P4.transform(normalizedImageCurrentMatrix);

    ur.x_ = max(max(P1T.x_,P2T.x_),max(P3T.x_,P4T.x_));
    ur.y_ = max(max(P1T.y_,P2T.y_),max(P3T.y_,P4T.y_));
    ll.x_ = min(min(P1T.x_,P2T.x_),min(P3T.x_,P4T.x_));
    ll.y_ = min(min(P1T.y_,P2T.y_),min(P3T.y_,P4T.y_));
}


void Image::writeEPSImage(ostream & outi) const
{
    // output the image data along with decoding procedure
    // into a separate *.eps file
    outi << "%!PS-Adobe-2.0 EPSF-2.0" << endl;
    outi << "%%Title: image created by pstoedit" <<  endl;
    outi << "%%Creator: pstoedit version " << version << endl;
    outi << "%%BoundingBox: " << (int)ll.x_ << " " << (int)ll.y_ << " "
	 << (int)ur.x_ << " " << (int)ur.y_ << endl;
    outi << "%%Pages: 1" << endl;
    outi << "%%EndComments" << endl << endl;
    outi << "%%Page: 1 1" << endl << endl;
    outi << "% save current state" << endl;
    outi << "gsave" << endl << endl;
    outi << "1 dict begin % temp dict for storing str1" << endl;
    outi << "% transformation matrix" << endl;
    outi << "[ " ;
    {for (unsigned int i = 0 ; i < 6 ; i++ )
		outi << normalizedImageCurrentMatrix[i] << " ";
    }
    outi << "] concat" << endl << endl;

	switch(type){
	case colorimage:
		outi << "/str1 1 string def" << endl << endl;
		outi << "% display color image" << endl;
		outi << width << " " << height << " " << bits
			<< " % width, height, bits/component" << endl;
		outi << "[ 1 0 0 1 0 0 ] %image matrix" << endl;
		outi << "{currentfile str1 readhexstring pop} % decoding procedure" << endl;
		outi << "false " << ncomp
			 << " % has many sources, number of color components" << endl;
		outi << "% number of data " << nextfreedataitem << endl;
		outi << "colorimage" << endl;
		{for(unsigned int i=0; i<nextfreedataitem; i++){
			if(i%(12*ncomp) == 0) outi  << endl; // debug " " << dec << i << endl;
			outi << setw(2) << setfill('0') << hex << (int)data[i];
		}}
		break;
    case imagemask:
    // just treat it as a normal image for the moment
    case normalimage:
		outi << "/str1 1 string def" << endl << endl;
		outi << "% display normal image" << endl;
		outi << width << " " << height << " " << bits
			 << " % width, height, bits/component" << endl;
		outi << "% number of data " << nextfreedataitem << endl;
		outi << "[ 1 0 0 1 0 0 ] %image matrix" << endl;
		outi << "{currentfile str1 readhexstring pop} % decoding procedure" << endl;
		outi << "image" << endl;
		{for(unsigned int i=0; i<nextfreedataitem; i++){
			if(i%(12*3) == 0) outi << endl;
			outi << setw(2) << setfill('0') << hex << (unsigned int)data[i];
		}}
		break;
    default:
		break;
    }

    outi << endl << endl;
    outi << "% restore previous state" << endl;
    outi << "end " << endl;
    outi << "grestore" << endl << endl;
    outi << "%%Trailer" << endl;
    outi << "%%EOF" << endl;
    outi << dec ;
}




DriverDescription::DriverDescription(const char * s_name, 
			const char * expl, 
			const char * suffix_p, 
			const bool 	backendSupportsSubPathes_p,
			const bool 	backendSupportsCurveto_p,
			const bool 	backendSupportsMerging_p, // merge a separate outline and filling of a polygon -> 1. element
			const bool 	backendSupportsText_p,
			const bool 	backendSupportsImages_p,  // supports bitmap images
			const opentype  backendFileOpenType_p,
			const bool	backendSupportsMultiplePages_p,
			checkfuncptr checkfunc_p):
		symbolicname(s_name),
		explanation(expl),
		suffix(suffix_p),
		additionalInfo( (checkfunc_p !=0) ? (checkfunc_p() ? "":"(license key needed, see pstoedit manual)") : ""),
		backendSupportsSubPathes(backendSupportsSubPathes_p),
		backendSupportsCurveto(backendSupportsCurveto_p),
		backendSupportsMerging(backendSupportsMerging_p), // merge a separate outline and filling of a polygon -> 1. element
		backendSupportsText(backendSupportsText_p),
		backendSupportsImages(backendSupportsImages_p),  // supports bitmap images
		backendFileOpenType(backendFileOpenType_p),
		backendSupportsMultiplePages(backendSupportsMultiplePages_p),
		filename("built-in"),
		checkfunc(checkfunc_p)
{
	DescriptionRegister::getInstance().registerDriver( this );
}

drvbase * DriverDescription::CreateBackend (const char * driveroptions_P,
		     ostream & theoutStream, 
		     ostream & theerrStream,   
			 const char* nameOfInputFile,
	       	 const char* nameOfOutputFile,
			 const float scalefactor,
			 const RSString & pagesize
			 ) const	
{ 
	unused(driveroptions_P);
	unused(&theoutStream);
	unused(&theerrStream);
	unused(nameOfInputFile);
	unused(nameOfOutputFile);
	unused(&scalefactor);
	unused(&pagesize);
	return 0; 
} 

DescriptionRegister& DescriptionRegister::getInstance()
{
	static DescriptionRegister theSingleInstance;
	globalRp = &theSingleInstance;
	return theSingleInstance;
}
 
 
