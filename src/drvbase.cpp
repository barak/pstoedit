/* 
   drvbase.cpp : This file is part of pstoedit
   Basic, driver independent output routines

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

#include <stdlib.h>
#include <iostream.h>
// rcw2: work round case insensitivity in RiscOS
#ifdef riscos
  #include "unix:string.h"
#else
  #include <string.h>
#endif
#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__OS2__)
#include <strstream.h>
#else
#include <strstrea.h>
#endif
#include "drvbase.h"

drvbase::drvbase(const char * driveroptions_p,ostream & theoutStream, 
		 ostream & theerrStream,
		 bool backendSupportsSubPathes_p,
		 bool backendSupportsCurveto_p, 
		 bool backendSupportsMerging_p) 
		: // constructor
    backendSupportsSubPathes(backendSupportsSubPathes_p),
    backendSupportsCurveto(backendSupportsCurveto_p),
    backendSupportsMerging(backendSupportsMerging_p),
    outf                (theoutStream),
    errf                (theerrStream),

    d_argc		(0),
    d_argv		(0),
    scale               (1.0f),
    // set some common defaults
    currentDeviceHeight (792.0f * scale),
    currentDeviceWidth  (640 * scale),
    x_offset            (0.0f),
    y_offset            (0.0f),
    currentPageNumber   (0),
    verbose    		(false),
    domerge    		(false),
    page_empty          (1),
    driveroptions	(0),
    // default for p1 and p2
    currentPath		(0),
    lastPath		(0),
    outputPath		(0)
    // default for textInfo_
{ 
    // init segment info for first segment
    // all others will be updated with each newsegment
    verbose = (getenv("PSTOEDITVERBOSE") != 0);
    if (verbose) { 
	errf << "verbose mode turned on\n" << endl;
    }


    // preparse driveroptions and build d_argc and d_argv
    if (driveroptions_p) {
    	driveroptions = strdup(driveroptions_p);

	istrstream optstream(driveroptions,strlen(driveroptions));
	const long startOfStream = optstream.tellg();
	char currentarg[100];
    	// first count number of arguments
    	while(!optstream.eof()) {
		optstream.width(sizeof(currentarg));
		optstream >> currentarg;
		d_argc++;
	}
	d_argv = new const char *[d_argc+1];
	// now fill d_args array;
	optstream.seekg(startOfStream); // reposition to start
	optstream.clear();
	d_argc=0;
    	while(!optstream.eof()) {
		optstream >> currentarg;
		d_argv[d_argc] = strdup(currentarg);
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

    currentPath = &p1;
    lastPath    = &p2;
    outputPath  = currentPath;
    
    if ((p1.path == 0) || (p2.path == 0) ) {
	errf << "new failed in drvbase::drvbase " << endl;
	exit(1);
    }

    setCurrentFontName("Courier",1);
    setCurrentFontFamilyName("Courier");
    setCurrentFontWeight("Regular");
    setCurrentFontFullName("Courier");
    setCurrentFontSize(10.0f);
}

drvbase::~drvbase() { 
	currentPath = 0;
	lastPath    = 0;
	outputPath  = 0;
	for (unsigned int i = 0; i < d_argc ; i++ ) {
		freeconst(d_argv[i]); // use free because of strdup uses malloc
	}
	delete [] d_argv;
	if (driveroptions) {
		free((char *) driveroptions);
	}
}

void drvbase::startup(bool merge)
{
	domerge = false; // default
	if (merge) {
		if ( backendSupportsMerging ) {
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

bool drvbase::pathsCanBeMerged(const PathInfo & path1, const PathInfo & path2)
{
//	return 0;
	// two paths can be merged if one of them is a stroke and the
	// other a fill AND
	// all pathelements are the same
//	errf << path1.numberOfElementsInPath << " " << path2.numberOfElementsInPath << endl;
	if (((path1.currentShowType == stroke && path2.currentShowType == fill)  ||
	     (path2.currentShowType == stroke && path1.currentShowType == fill)  )
	   && (path1.numberOfElementsInPath == path2.numberOfElementsInPath)  ) {
//		errf << "Pathes seem to be mergeable" << endl;
		for (unsigned int i = 0; i < path1.numberOfElementsInPath ; i++) {
			const basedrawingelement *bd1 = path1.path[i];
			const basedrawingelement *bd2 = path2.path[i];
//			if (! *(path1.path[i]) == *(path2.path[i]) ) return 0;
			if (! (*bd1 == *bd2) ) return 0;
		}
//		errf << "Pathes are mergeable" << endl;
		return 1;
	} else {
		return 0;
	}
}

const basedrawingelement & drvbase::pathElement(unsigned int index) const 
{
	return *(outputPath->path[index]);
}
bool operator==(const basedrawingelement & bd1, const basedrawingelement & bd2) 
		{ 
			if (bd1.getType() != bd2.getType() ) {
				return 0;
			} else {
				for (unsigned int i = 0; i < bd1.size; i++ ) {
					if (! (bd1.getPoint(i) == bd2.getPoint(i)) ) return 0;
				}
			}
			return 1;
		}

void drvbase::dumpText(const char * const thetext,const float x, const float y)
{
  if (strlen(thetext) > 0) {
  	dumpPath(); // dump last path to avoid wrong sequence of text and graphics
  	add_to_page();
  	textInfo_.x = x;
  	textInfo_.y = y;
  	textInfo_.thetext = thetext;
  	show_text(textInfo_);
  }
}

void drvbase::setCurrentWidthParams(float ax,float ay,int Char,float cx,float cy)
{
	textInfo_.ax = ax;
	textInfo_.ay = ay;
	textInfo_.Char = Char;
	textInfo_.cx = cx;
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
#if 0
// TODO, Not yet implemented
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
#else
	return 0;
#endif
}

void drvbase::add_to_page()
{
    if (page_empty) {
	page_empty = 0;
	currentPageNumber++;
	open_page();
    }
}

void drvbase::guess_linetype()
{
	const char * pattern = dashPattern();
	// first count number of ' ' in pattern to determine number of entries
	// we normally have one less than number of blanks
	// line looks like: " [ 2.25 6.75 ] 0.0 setdash"
	drvbase::linetype curtype = solid;
	int nr_of_entries = -1;
	while ((*pattern) && (*pattern != ']' ) ) {
		if (*pattern == ' ') nr_of_entries++;
		pattern++;
	}
	pattern = dashPattern();
	// errf << nr_of_entries << " entries found in " << pattern << endl;
	if (nr_of_entries > 0) {
		// now get the numbers
		// repeat the numbers, if number of entries is odd
		int rep = nr_of_entries % 2; // rep is 1 for odd numbers 0 for even
		float *d_numbers = new float[nr_of_entries *(rep + 1)];
		int cur = 0;
#ifndef USEISTREAM
		for (int i = 0; i <= rep ; i++ ) {
			pattern = dashPattern();
			while ((*pattern) && (*pattern != ']' ) ) {
				if (*pattern == ' ' && ( *(pattern+1) != ']' ) ) {
					float f = (float) atof(pattern);
					d_numbers[cur] = f;
				 	// errf << d_numbers[cur] << endl;
					cur++;
				}
				pattern++;
			}
		}
#else
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
		// now guess a pattern from
		// solid, dashed, dotted, dashdot, dashdotdot ; // corresponding to the CGM patterns
		switch ( nr_of_entries *(rep + 1) ) {
		case 2: 
			if (d_numbers[0] < 2.0f) {
				// if on is < 2 then always dotted
				// ok we miss '.             .             .'
				curtype = drvbase::dotted;
			} else {
				curtype = drvbase::dashed;
			}
			break;
		case 4:
			if ( (d_numbers[0] < 2.0f) || (d_numbers[2] < 2.0f) ) {
				curtype = drvbase::dashdot;
			} else {
				curtype = drvbase::dashed;
			}
			break;
		case 6:
			if ( (d_numbers[0] < 2.0f) || (d_numbers[2] < 2.0f)  || (d_numbers[2] < 2.0f) ) {
				curtype = drvbase::dashdotdot;
			} else {
				curtype = drvbase::dashed;
			}
			break;
		default:
			curtype = drvbase::dashed;
			break;
		}
		delete [] d_numbers;
	} else {
		// no entry
		curtype = drvbase::solid;
	}
	// errf << "linetype from " << dashPattern() << " is " << curtype << endl;
	setCurrentLineType(curtype);
}

void drvbase::dumpPath()
{
    guess_linetype(); // needs to be done here, because we must write to currentpath
    if (currentPath->numberOfElementsInPath == 2) {
		// a polygon with two points is drawn as a line
		currentPath->isPolygon=false;
		currentPath->currentShowType=drvbase::stroke;
    }
    if (currentPath->currentShowType!=drvbase::stroke)
    {
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
			{
#if 0
			float llx,lly,urx,ury;

// TODO, Not yet implemented
			llx = min( min(pNumber(0),pNumber(2)), min(pNumber(4),pNumber(6)));
			urx = max( max(pNumber(0),pNumber(2)), max(pNumber(4),pNumber(6)));
			lly = min( min(pNumber(1),pNumber(3)), min(pNumber(5),pNumber(7)));
			ury = max( max(pNumber(1),pNumber(3)), max(pNumber(5),pNumber(7)));

			show_rectangle(llx,lly,urx,ury);
#endif
	       		}
		} else {
			show_path();
		}
    	} else { /* PolyLine */
		show_path();
    	};
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

void drvbase::PathInfo::clear() {
			for (unsigned int i = 0 ; i < numberOfElementsInPath; i++ ) {
				delete path[i];
				path[i] = 0;
			}
			numberOfElementsInPath = 0;
		}
void drvbase::PathInfo::copyInfo(const PathInfo & p){
			// copies the whole path state except the path array
			currentShowType = p.currentShowType;
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
		}

ostream & operator<<(ostream & out,const basedrawingelement &elem)
{
	out << "type: " << (int) elem.getType() << " params: " ;
	for (unsigned int i = 0 ; i < elem.size ; i++ ) {
		out << elem.getPoint(i).x_ << " "
		    << elem.getPoint(i).y_ << " "; 
	}
	out << endl;
	return out;
}
ColorTable::ColorTable(const char * const * defaultColors,
		   const unsigned int numberOfDefaultColors,
		   makeColorNameType makeColorName)  :
		defaultColors_(defaultColors),
		numberOfDefaultColors_(numberOfDefaultColors),
		makeColorName_(makeColorName) 
{ 
	for (unsigned int i = 0 ; i< maxcolors; i++) newColors[i] = 0;
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

void copy_file(istream& infile,ostream& outfile) 
{
#if 1
  outfile << infile.rdbuf();
#else
// long version. should be the same as above
  unsigned char c;
  while (infile.get(c)) {
	outfile.put(c);
  }
#endif
}


TempFile::TempFile()  
{ 
	tempFileName = tempnam(0,"pstmp"); 
	// cout << "constructed " << tempFileName << endl; 
}
TempFile::~TempFile() 
{ 	
	// cout << "detructed " << tempFileName << endl; 
	close(); 
	remove(tempFileName); 
	free(tempFileName); 
}
ofstream & TempFile::asOutput() 
{ 
	close(); 
	outFileStream.open(tempFileName); 
	if (outFileStream.fail()) cerr << "openening " << tempFileName << "failed " << endl;
	return outFileStream; 
}
ifstream & TempFile::asInput()  
{ 
	close(); 
	inFileStream.open(tempFileName);  
	if (outFileStream.fail()) cerr << "openening " << tempFileName << "failed " << endl;
	return  inFileStream; 
}
void TempFile::close() 
{ 
//#ifdef HAVE_IS_OPEN
//#define IS_OPEN(file) file.is_open()
//#else
//#define IS_OPEN(file) file
//#endif
// commented, since it also works portably if we call clear()

//	if (IS_OPEN(inFileStream))  {
		inFileStream.close(); 
//		if (inFileStream.fail()) cerr << "closing inFileStream failed " << endl;
//	}
	inFileStream.clear(); 
//	if (IS_OPEN(outFileStream))  {
		outFileStream.close(); 
//		if (outFileStream.fail()) cerr << "closing outFileStream  failed " << endl;
//	}
	outFileStream.clear(); 
}
void freeconst(const void *ptr) 
{
	free((char *)ptr); // just to have the warning about casting away constness
		   // once (here)
}
