/* 
   drvCGM.cpp : This file is part of pstoedit
   Backend for CGM format

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

#include <stdio.h>
// rcw2: work round case insensitivity in RiscOS
#ifdef riscos
  #include "unix:string.h"
#else
  #include <string.h>
#endif
#include <stdlib.h>
#include "drvcgm.h"

#include "cd.h"

#ifdef __TCPLUSPLUS__
#include <alloc.h>
#endif
// for sin and cos
#include <math.h>

// static const unsigned int imageHeight = 850;
// static const unsigned int imageWidth  = 600;

// predefined Fonts from CD-library
static const char *CGM_CD_Fonts[] = { 
	    // although the cd-library uses the names without - we need it here
	    // in order to recognize the PostScript names
	    // !!! order of -Bold and -Italic is reversed wrt. ASC_Fonts !!!
	"Times-Roman", 
	"Times-Bold",
	"Times-Italic", 
	"Times-BoldItalic", 
	"Helvetica", 
	"Helvetica-Bold",
	"Helvetica-Oblique", 
	"Helvetica-BoldOblique",
	"Courier", 
	"Courier-Bold",
	"Courier-Oblique", 
	"Courier-BoldOblique", 
// not supported by cd	"Symbol",
	0
};

// for the ASCII - backend
static const char *CGM_ASC_Fonts[] = { 
	"Times-Roman", 
	"Times-Italic", 
	"Times-Bold",
	"Times-BoldItalic", 
	"Helvetica", 
	"Helvetica-Oblique", 
	"Helvetica-Bold",
	"Helvetica-BoldOblique", 
	"Courier", 
	"Courier-Oblique", 
	"Courier-Bold",
	"Courier-BoldOblique", 
	"Symbol" ,
	0
};

const char * const *    drvCGM::knownFontNames() const 
{
	if (binary_) {
		return CGM_CD_Fonts;
	} else {
		return CGM_ASC_Fonts;
	}
}

static int getFontNumber(const char * const fontname,const char * Fnames[]) {
    const unsigned int fntlength = strlen(fontname);
    const char * const * fiter = Fnames;
    int index = 0;
    while (*fiter) {
    //for (unsigned int i=0; i < numberOfFonts; i++) 
    	const unsigned int cgmFntLengh = strlen(*fiter);
	if (fntlength == cgmFntLengh ) { 
	    if (strncmp(fontname,*fiter,fntlength) == 0) {
		return index + 1; // cgm counts from 1
	    }
	}
	fiter++; index++;
    }
    return -1;
}

#define cdcall(func,params) if (! func params)  {\
	errf << "call to cd-library " #func " failed" << endl;\
	return ; \
	}

drvCGM::drvCGM(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream,bool binary):
	drvbase(driveroptions_p,theoutStream,theerrStream,(!binary),0,1),
	// only ascii version supports subpaths so far
	binary_(binary),
	cdpoints(0),
	cgmimage(0)
{
	scale = 10.0; // only logical scale to avoid rounding problems
// creating the image deffered to open_page, because we need access to the DeviceWidth/Height
// but these are only available after the PostScript interpreter has finished.
}

drvCGM::~drvCGM() 
{
	if (binary_) {
		// free points, so new memory is there for the 
		// cd write functions
		delete [] cdpoints;

		if (cdImageCgm) {
			FILE * outfile = tmpfile();
			cdcall(cdImageCgm,(cgmimage, outfile));
			cdcall(cdImageDestroy,(cgmimage));
			cgmimage = 0;

			rewind(outfile);
			filebuf outbuf(fileno(outfile));
			outf << &outbuf; // This copies outbuf to outf
		} else {
			errf << "No image created " << endl;
		}
	} else {
		cdcall(cdImageDestroy,(cgmimage));
		outf << "EndMF;\n";
	}
} 

//static float rnd(const float f,const float roundnumber)
//{
//	return ((long int) ( (f * roundnumber) + 0.5 )) / roundnumber;
//}
// static inline float RND3(const float f) { return rnd(f,1000);}
static inline int RND(const float f) { return (int) (f+0.5);}

void drvCGM::print_coords()
{
#if 0
    for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
        const Point & p = pathElement(n).getPoint(0);
        outf << " (" << (int)( p.x_ + x_offset) << ',' 
		    << (int)( p.y_ + y_offset) << ')';
    }
#else
    bool newpath = true;
    bool lastwasclosepath = false;
    Point lastMoveto(0.0f,0.0f);
    unsigned int lastelemnr = numberOfElementsInPath();
    for (unsigned int n = 0; n < lastelemnr; n++) {
	const basedrawingelement & elem = pathElement(n);
	if (newpath) {
    		switch (currentShowType() ) {
 		case drvbase::stroke : 
      			// draw as line
			// terminate and start a new Line;
    			outf << " Line " ;
		      	break;
		case drvbase::fill :
			// draw each segment as a polygon
			outf << " Polygon " ;
			break;
		case drvbase::eofill :
			// dump the whole path as a large polygon set
	    		outf << " PolygonSet " ;
			break;
		default: 
			// cannot happen
			errf << "unexpected ShowType " << (int) currentShowType() << " in drvcgm.cc" << endl;
			exit(1);
			break;
		}
		newpath = 0;
	}
	switch (elem.getType()) {
	case moveto: 
		if ( (n != 0) && (n != lastelemnr -1) ) {
		// ignore moveto, if it's the first
		// or the last part
    			switch (currentShowType() ) {
 			case drvbase::stroke : 
      				// draw as line
				// terminate and start a new Line;
				outf << " ;\n";
    				outf << " Line " ;
			      	break;
			case drvbase::fill :
				// draw each segment as a polygon
				outf << " ;\n";
				outf << " Polygon " ;
				break;
			case drvbase::eofill :
				// dump the whole path as a large polygon set
				if (!lastwasclosepath) {
					outf << " closeinvis ";
				}
				break;
			default: 
				// cannot happen
				errf << "unexpected ShowType " << (int) currentShowType() << " in drvcgm.cc" << endl;
				exit(1);
				break;
			}
		}
		if ( n != lastelemnr -1 ) {
			const Point & p = elem.getPoint(0);
			lastMoveto=p;
			outf  	<< RND(p.x_ + x_offset) << " " 
	     			<< RND(p.y_ + y_offset) << " " ;
		}
		break;
	case lineto: 
		if (currentShowType() == drvbase::eofill) {
			outf << "  vis ";
		} else {
			outf << "  ";
		}
		{
			const Point & p = elem.getPoint(0);
        		outf  << RND(p.x_ + x_offset) << " " 
	     			<< RND(p.y_ + y_offset) << " " ;
		}
		break;
	case closepath: 
		switch (currentShowType() ) {
		case drvbase::stroke : 
			// draw a line to the start of the current line 
			// and prepare for a new line
        		outf << RND(lastMoveto.x_ + x_offset) << " " 
	     		     << RND(lastMoveto.y_ + y_offset) << " " ;
				outf << ";\n";
			newpath=1;
			break;
		case drvbase::fill :
			// close and prepare for a new path.
			outf << " ;\n";
			newpath=1;
			break;
		case drvbase::eofill :
			// normal close
			outf << " closevis ";
			break;
		default: 
			// cannot happen
			errf << "unexpected ShowType " << (int) currentShowType() << " in drvcgm.cc" << endl;
			exit(1);
			break;
    		}
		break;

	case curveto:
		errf << "Fatal: unexpected case in drvcgm " << endl;
		exit(1);
		break;
	default:
		errf << "Fatal: unexpected case in drvcgm " << endl;
		exit(1);
		break;
	}
	outf << '\n';

	lastwasclosepath = (elem.getType() == closepath);
    }
    if (lastwasclosepath) {
// a closepath was the last statement, so the path is really closed
// except in the case of an eofilled path
   	if (currentShowType() == drvbase::eofill) {
    		outf << " ;\n" ;
	}
    } else {
// implicit close path
    	if (currentShowType() == drvbase::stroke) {
		outf << " ;\n";
	} else {
		outf << " ;\n";
	}
    }
#endif
}


void drvCGM::open_page()
{
// creating the image deffered to here, because we need access to the DeviceWidth/Height
// but these are only available after the PostScript interpreter has finished.

// we allocate a cdImage also for the ascii backend, because we use
// the color functions from the cd library
	if (!cgmimage) { // no image so far
// 	errf  << "W " << currentDeviceWidth << " H " << currentDeviceHeight << endl;
#ifdef CD12X
	cgmimage = cdImageCreate(RND(currentDeviceWidth), RND(currentDeviceHeight));
	// cgmimage = cdImageCreate(imageWidth, imageHeight);
#else
	/* Create an image*/
	cgmimage = cdImageStartCgm();
	cdcall(cdImageSetSize,(cgmimage,RND(currentDeviceWidth), RND(currentDeviceHeight)));
	/* set Line, Marker, and Edge specification modes to absolute (0)
	 * the default is scaled (1) */
	cdcall(cdImageSetLineSpec,(cgmimage, 0));
	cdcall(cdImageSetMarkerSpec,(cgmimage, 0));
	cdcall(cdImageSetEdgeSpec,(cgmimage, 0));
	/* start the picture */
	cdcall(cdCgmHeader,(cgmimage));
	cdcall(cdCgmPic,(cgmimage, 2));
#endif
	if (binary_) {
		cdImageColorAllocate(cgmimage,255,255,255); // set background color
		cdpoints = new cdPoint[maxElements];
		if (!cdpoints) {
			errf << "Could not allocate space for points" << endl;
			exit(1);
		}
	} else {
		outf << "BegMF \"picture made with pstoedit\";\n";
		outf << "MFVersion 1;\n";
		outf << "mfelemlist 'DRAWINGPLUS';\n";
		outf << "colrprec 255;\n";
		outf << "colrindexprec 127;\n";
		outf << "maxcolrindex 255;\n";
		outf << "fontlist " << endl;
		const char * const * fontiter = CGM_ASC_Fonts;
		while (*fontiter) {
			outf << "'" << *fontiter << "'"; 
			fontiter++;
			if (*fontiter) { 
				outf << "," << endl; 
			}  else {
				outf << ";" << endl; 
			}
		}
	}
	}
	if (binary_ ) {
		if (currentPageNumber > 1) {
         		// we have to re-allocate the colors
			// but keep the other defaults
        		cdcall(cdCgmNewPic,(cgmimage, 2));
	 
       	 		// allocate some colors (Again!) 
       	 		// the first color allocated is the background color 
			cdImageColorAllocate(cgmimage,255,255,255); // set background color
		}
	} else {
		outf << " BegPic \"page:  " << currentPageNumber << " \";\n";
		// outf << " vdcext (0,0) (" << imageWidth << ',' << imageHeight << ");\n";
		outf << " vdcext (0,0) (" << RND(currentDeviceWidth) << ',' << RND(currentDeviceHeight) << ");\n";
		outf << " colrmode indexed;\n";
		outf << " linewidthmode abstract;\n";
//ralcgm does not like vdc		outf << " edgewidthmode vdc;\n";
		outf << " edgewidthmode abstract;\n";
//    outf << " ScaleMode Metric 0.1;\n";
		outf << " BegPicBody;\n";
		getcdcolor(1.0f,1.0f,1.0f); // write white color index
    		outf << " edgevis on;\n"; // needed, otherwise vis/invis has no effect
	}
}

void drvCGM::close_page()
{
	if (binary_) {
	} else {
		outf << " EndPic;\n";
	}
}
unsigned int drvCGM::getcdcolor(float R, float G, float B)
{
	const unsigned int r = (unsigned int) (255 * R);
	const unsigned int g = (unsigned int) (255 * G) ;
	const unsigned int b = (unsigned int) (255 * B) ;
	int cdcolor = cdImageColorExact(cgmimage,r,g,b);
	if (cdcolor < 0)  {
		// not stored so far
		cdcolor = cdImageColorAllocate(cgmimage,r,g,b);
		if (cdcolor < 0)  {
			cdcolor = cdImageColorClosest(cgmimage,r,g,b);
		} else {
			// newly allocated color
			if (! binary_) {
  				outf << " colrtable " << cdcolor << ' ' 
					<< cdImageRed(cgmimage,cdcolor) << ' ' 
				 	<< cdImageGreen(cgmimage,cdcolor) << ' '
					<< cdImageBlue(cgmimage,cdcolor) << ";\n";
			}
		}
    	}
	return cdcolor;
}

void drvCGM::show_text(const TextInfo & textinfo)
{
	const unsigned int cdcolor = getcdcolor(textinfo.currentR,textinfo.currentG,textinfo.currentB);
	const float toRadians = 3.14159265359f / 180.0f;
	const float angleInRadians = textinfo.currentFontAngle * toRadians;
	const int xbase = (int) (1000*cos(angleInRadians));
	const int ybase = (int) (1000*sin(angleInRadians));
	const int xup=-ybase;
	const int yup=xbase;

	if (binary_) {
		int CGMFontNum = getFontNumber(textinfo.currentFontName.value(),CGM_CD_Fonts);
		if (CGMFontNum == -1) {
			CGMFontNum = 1;
			errf << "replacing font: " << textinfo.currentFontName.value() << " with " << CGM_CD_Fonts[CGMFontNum] << endl;
		}
		// errf << "Using "  << CGMFontNum << " for " << textinfo.currentFontName.value() << endl;
		const int height = RND(textinfo.currentFontSize);
		cdSetTextAttrib(cgmimage,CGMFontNum,cdcolor,height);
		cdSetTextOrient(cgmimage,xup,yup,xbase,ybase);
		cdText(cgmimage,RND(textinfo.x),RND(textinfo.y),textinfo.thetext);
	} else {
		int CGMFontNum = getFontNumber(textinfo.currentFontName.value(),CGM_ASC_Fonts);
		if (CGMFontNum == -1) {
			CGMFontNum = 1;
			errf << "replacing font: " << textinfo.currentFontName.value() << " with " << CGM_ASC_Fonts[CGMFontNum] << endl;
		}
		outf << "  textcolr " << cdcolor << ";\n";
		outf << "  charheight " << textinfo.currentFontSize << ";\n";
		outf << "  charori " << xup << ' ' << yup << ' ' << xbase << ' ' << ybase << ";\n";
		outf << "  TEXT (" << RND(textinfo.x)  << ',' << RND(textinfo.y)  << ") final '" ;
		{ 	const char * cp = textinfo.thetext;
			while (*cp) {
				if (*cp == '\'') outf << '\'';
				outf << *cp;
				cp++;
			}
		}
		outf << "';\n";
	}
}

void drvCGM::show_path()
{
	const unsigned int cdedgecolor = getcdcolor(edgeR(),edgeG(),edgeB());
	const unsigned int cdfillcolor = getcdcolor(fillR(),fillG(),fillB());
//	errf << "EdgeColor " << cdedgecolor << " FillColor " << cdfillcolor << endl;
	if (binary_) {
		for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
			const Point & p = pathElement(n).getPoint(0);
			cdpoints[n].x =  RND(p.x_);
			cdpoints[n].y =  RND(p.y_);
		}

//#ifdef __TCPLUSPLUS__
//		if (verbose) errf << "core left " << coreleft() << " " << farcoreleft() << endl;
//#endif
		const unsigned int cdLineWidth = (unsigned int) RND(currentLineWidth());
		const bool visible = (currentLineWidth() > 0.0);
//		const int solid_line = 1;
//		const int dashed_line = 2;
		const int hollow_fill = 0;
		const int solid_fill  = 1;

		const int linestyle = (int) currentLineType() + 1; // what a luck, the encoding is simple
		switch ( currentShowType() ) {
		case  drvbase::stroke:
			cdcall(cdSetLineAttrib,(cgmimage,linestyle,cdLineWidth,cdedgecolor));
			// we have to set the ShapeFillAttrib as well in case
			// the drawing is done via cdPolygon
			cdcall(cdSetShapeFillAttrib,(cgmimage,hollow_fill,cdfillcolor,1));
			if (isPolygon()) {
				// errf << "cdPolygon " << numberOfElementsInPath()-1 << endl;
				cdcall(cdPolygon,(cgmimage,cdpoints,numberOfElementsInPath()-1));
			} else {
				// errf << "cdPolyLine " << numberOfElementsInPath() << endl;
				cdcall(cdPolyLine,(cgmimage,cdpoints,numberOfElementsInPath()));
			}
			break;
		case  drvbase::eofill:
			// This may be wrong (treating fill same as eofill), but there is no other way 
			// at the moment
		case  drvbase::fill:
			cdcall(cdSetLineAttrib,(cgmimage,linestyle,cdLineWidth,cdedgecolor));
			cdcall(cdSetShapeFillAttrib,(cgmimage,solid_fill,cdfillcolor,1));
			cdcall(cdSetShapeEdgeAttrib,(cgmimage,linestyle,cdLineWidth,cdedgecolor,visible));
			if (isPolygon()) {
				// errf << "cdPolygon " << numberOfElementsInPath()-1 << endl;
				cdcall(cdPolygon,(cgmimage,cdpoints,numberOfElementsInPath()-1));
			} else {
				// errf << "cdPolyLine " << numberOfElementsInPath() << endl;
				// draw a non closed/filled polyline as polygon
				cdcall(cdPolygon,(cgmimage,cdpoints,numberOfElementsInPath()));
			}
			break;
		default:
			errf << "Unexpected case in drvcgm.cc " << endl;
			break;
		}

	} else {
		switch ( currentShowType() ) {
		case  drvbase::stroke:
			outf << " intstyle solid;\n";
			break;
		case  drvbase::fill:
			outf << " intstyle solid;\n";
			break;
		case  drvbase::eofill:
			outf << " intstyle solid;\n";
			break;
		default:
			break;
		}
		outf << " linecolr " << cdedgecolor << ";\n";
		outf << " fillcolr " << cdfillcolor << ";\n";
		outf << " linewidth " <<  currentLineWidth() << ";\n";
		outf << " edgewidth " <<  currentLineWidth() << ";\n";
		if (currentLineWidth() >0 ) {
			outf << " edgevis on;\n";
		} else {
			outf << " edgevis off;\n";
			
		}

#if 0
// no subpaths
    	if (isPolygon) {
    		numberOfElementsInPath()--; // CGM does not need end=start
		outf << " Polygon";
		print_coords();
		outf << ";\n";
		numberOfElementsInPath()++; // restore old value for proper cleanup
	} else { 
		outf << " Line";
		print_coords();
		outf << ";\n";
	}
#else
		// dump with subpaths
		print_coords();
#endif
	}
}

void drvCGM::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
	// just do show_polyline for a first guess
	unused(&llx);
	unused(&lly);
	unused(&urx);
	unused(&ury);
	show_path();
}
