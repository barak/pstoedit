/*
   drvFIG.cpp : This file is part of pstoedit
   Based on the skeleton for the implementation of new backends

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

/*
    FIG 3.1 driver by Ian MacPhedran (Ian_MacPhedran@engr.usask.ca)
    April 1995

    Color support and conversion to use C++ streams done by Wolfgang Glunz

    Object depth support by Gerhard Kircher <kircher@edvz.tuwien.ac.at>
    March 1996

    Curves and image support by Leszek Piotrowicz <zorro@delta.ds2.pg.gda.pl>
    July 1997

    Support for xfig format 3.2 (X-Splines) by Wolfgang Glunz

    Support for subpaths by Burkhard Plaum
    
*/

#include "drvfig.h"

// for sprintf
#include I_stdio
#include I_string_h
#include I_iostream
#include I_iomanip


static const float PntFig = 1200.0f / 72.0f;


static const char * colorstring(float r, float g, float b)
{
static char buffer[15];
	sprintf(buffer,"%s%.2x%.2x%.2x","#", (unsigned int) (r * 255), (unsigned int) ( g * 255) ,  (unsigned int) (b * 255));
//	cerr << buffer << " ";
//	cerr << r << " ";
//	cerr << (r * 255);
//	cerr << (unsigned int) (r * 255) << " ";
//	cerr << endl;
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

drvFIG::derivedConstructor(drvFIG):
	constructBase,
	buffer(tempFile.asOutput()),
	imgcount(1),
	format(32)
{
    // driver specific initializations
       float depth_in_inches = 11;
       bool show_usage_line = false;
       const char *paper_size = NULL;

       for (unsigned int i = 0; i < d_argc ; i++ ) {
               if (verbose) outf << "% " << d_argv[i] << endl;
               if (strcmp(d_argv[i],"-f31") == 0) {
                       format = 31;
               } else if (strcmp(d_argv[i], "-depth") == 0) {
                       i++;
                       if (i >= d_argc) {
                               errf << "-depth requires a depth in inches"
                                       << endl;
                               show_usage_line = true;
                       } else {
                               depth_in_inches = (float) atof(d_argv[i]);
                       }
               } else if (strcmp(d_argv[i], "-help") == 0) {
                       errf << "-help    Show this message" << endl;
                       errf << "-f31     Use xfig 3.1 format" << endl;
                       errf << "-depth # Set the page depth in inches" << endl;
                       show_usage_line = true;
               } else {
                       errf << "Unknown fig driver option: "
                               << d_argv[i] << endl;
                       show_usage_line = true;
               }
       }

       if (show_usage_line) {
               errf << "Usage -f 'fig: [-help] [-f31] [-depth #]'" << endl;
       }

    // Set the papersize
    if (!paper_size) paper_size = (depth_in_inches <= 11.0? "Letter": "A4");

    // set FIG specific values
    scale    = 1;
    currentDeviceHeight = depth_in_inches * 1200.0f * scale;
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
    if (format == 31) {
	    outf << "#FIG 3.1\nPortrait\nFlush left\nInches\n1200 2\n";
    } else {
//    	    outf << "#FIG 3.2\nPortrait\nFlush left\nInches\nA4\n100.00\nSingle\n0\n1200 2\n";
            outf << "#FIG 3.2\nPortrait\nFlush left\nInches\n"
                       << paper_size << "\n100.00\nSingle\n0\n1200 2\n";
    }

}

drvFIG::~drvFIG() {
	dumpnewcolors(outf);
	// now we can copy the buffer the output
	ifstream & inbuffer = tempFile.asInput();
	copy_file(inbuffer,outf);
}


unsigned int drvFIG::nrOfCurvetos() const
{
  unsigned int nr = 0;
  for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
    const basedrawingelement & elem = pathElement(n);
    if (elem.getType() == curveto)
      nr++ ;
  }
  return nr;
}




void drvFIG::print_polyline_coords()
{
  int j = 0;
  //  const Point & p;
  for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
    const basedrawingelement & elem = pathElement(n);
    if (j == 0) { buffer << "\t"; }
    switch(elem.getType()) {
        case lineto:
        case moveto: {
            const Point & p = pathElement(n).getPoint(0);
	    buffer << (int)(PntFig * p.x_) << " "
	           << (int)(y_offset - (PntFig * p.y_)) << " ";
            }
            break;
        case closepath: {
            const Point & p = pathElement(0).getPoint(0);
	    buffer << (int)(PntFig * p.x_) << " "
	           << (int)(y_offset - (PntFig * p.y_)) << " ";
            }
            break;
        default:
	    errf << "\t\tFatal: unexpected case in drvfig " << endl;
 	    abort();
	    break;

    }
    
    j++;
    if (j == 5) { j=0; buffer << "\n"; }
  }
  if (j != 0) { buffer << "\n"; }
}

void drvFIG::prpoint(ostream & os, const Point& p) const {
	os << (int)(PntFig * p.x_) << " " << (int)(y_offset - (PntFig * p.y_)) << " ";
}
// print segments without x-splines
void drvFIG::print_spline_coords_noxsplines(int linestyle,float localLineWidth)
{
//  int j = 0;
  Point firstPoint(0,0);
  bool  firstPointSet = false;
  Point currentPoint(0,0);

  for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
//    if (j == 0) { buffer << "\t"; }
    const basedrawingelement & elem = pathElement(n);
	if (!firstPointSet) { firstPoint=elem.getPoint(0); firstPointSet=true; }
    switch (elem.getType()) {
      case moveto: { currentPoint = elem.getPoint(0); break;}
      case lineto: {
		const Point & p = elem.getPoint(0);
		if (! (p == currentPoint)) {
		buffer << "# polyline\n";
		buffer << "2 1 " << linestyle << " " << (int)localLineWidth << " " ;
		const int color = registercolor(currentR(),currentG(),currentB());
		const int fill_or_nofill = (currentShowType() == drvbase::stroke) ? -1 : 20;
		if (objectId) objectId--; // don't let it get < 0
		buffer << color <<  " " << color << " " << objectId << " 0 "
              << fill_or_nofill <<  " " << "4.0" << " 0 0 0 0 0 ";
       // 4.0 is the gap spec. we could also derive this from the input
		buffer << 2 << "\n\t";
		prpoint(buffer,currentPoint);
		prpoint(buffer,p);
		currentPoint=p;
		buffer << endl;
		}
		break;
	}
      case curveto: {
		buffer << "# spline\n";
	 // 3 0 means "open approximated spline"
		buffer << "3 0 " << linestyle << " " << (int)localLineWidth << " " ;
		const int color = registercolor(currentR(),currentG(),currentB());
		const int fill_or_nofill = (currentShowType() == drvbase::stroke) ? -1 : 20;
		if (objectId) objectId--; // don't let it get < 0
		buffer << color <<  " " << color << " " << objectId << " 0 "
              << fill_or_nofill <<  " " << "4.0" << " 0 0 0 ";
     // 4.0 is the gap spec. we could also derive this from the input
   		buffer << 4 << "\n\t";
		prpoint(buffer,currentPoint);
		prpoint(buffer,elem.getPoint(0));
		prpoint(buffer,elem.getPoint(1));
		prpoint(buffer,elem.getPoint(2));
		currentPoint=elem.getPoint(2);
		buffer <<  endl;
		// 32 buffer << "\n\t0.000 1.000 1.000 0.000" << endl;
	break;
	}
      case closepath: {
		buffer << "# polyline\n";
		buffer << "2 1 " << linestyle << " " << (int)localLineWidth << " " ;
		const int color = registercolor(currentR(),currentG(),currentB());
		const int fill_or_nofill = (currentShowType() == drvbase::stroke) ? -1 : 20;
		if (objectId) objectId--; // don't let it get < 0
		buffer << color <<  " " << color << " " << objectId << " 0 "
              << fill_or_nofill <<  " " << "4.0" << " 0 0 0 0 0 ";
       // 4.0 is the gap spec. we could also derive this from the input
		buffer << 2 << "\n\t";
		prpoint(buffer,currentPoint);
		prpoint(buffer,firstPoint);
		currentPoint=firstPoint;
		buffer << endl;
		break;
	}
	break;
      default: {
	errf << "\t\tFatal: unexpected case in drvfig " << endl;
	abort();
	break;
        }
  }
}
}
// print edge points
void drvFIG::print_spline_coords1()
{
  int j = 0;

  for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
    if (j == 0) { buffer << "\t"; }
    const basedrawingelement & elem = pathElement(n);
    switch (elem.getType()) {
      case moveto:
      case lineto: {
	const Point & p = elem.getPoint(0);
	buffer << (int)(PntFig * p.x_) << " "
	       << (int)(y_offset - (PntFig * p.y_)) <<	" ";
	}
    	j++; if (j == 5) { j=0; buffer << "\n"; }
	break;
      case curveto: {
        if ((format == 31)) {
		const Point & p = elem.getPoint(2);
		buffer << (int)(PntFig * p.x_) << " "
		       << (int)(y_offset - (PntFig * p.y_)) <<	" ";
    		j++; if (j == 5) { j=0; buffer << "\n"; }
	} else {
		// put all points, middle points will have control value 1
		for (unsigned int cp = 0 ; cp < 3; cp++ ) {
			const Point & p = elem.getPoint(cp);
			buffer << (int)(PntFig * p.x_) << " "
		       	<< (int)(y_offset - (PntFig * p.y_)) <<	" ";
    			j++; if (j == 5) { j=0; buffer << "\n"; }
    			if ((j == 0) && (cp != 2) && ( n+1 != (numberOfElementsInPath()))) { buffer << "\t"; }
		}
	}
	}
	break;
      case closepath: {
	const Point & p = pathElement(0).getPoint(0);
	buffer << (int)(PntFig * p.x_) << " "
	       << (int)(y_offset - (PntFig * p.y_)) <<	" ";
	}
    	j++; if (j == 5) { j=0; buffer << "\n"; }
	break;
      default:
	errf << "\t\tFatal: unexpected case in drvfig " << endl;
	abort();
	break;
    }
  }
  if (format == 31) {
  	if (j != 0) { buffer << "\n"; }
  } else {
  	if (j != 0) { buffer << "\n"; }
	buffer << "\t";
  }
}

// print control points
void drvFIG::print_spline_coords2()
{
  int j = 0;
  Point lastp;
  int maxj = 8;
  if (format == 31) {
	maxj = 4; 
  }

  for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
    const basedrawingelement & elem = pathElement(n);
    switch (elem.getType()) {
      case moveto: {
        if (format == 31) {
		buffer << "\t0.0 0.0 ";
	} else {
		buffer << " 0 ";
	}
	j++; if (j == maxj) { 
		j=0; buffer << "\n"; 
    		if ( (n+1) != numberOfElementsInPath()) { buffer << "\t"; }
	}
	lastp = elem.getPoint(0);
	}
	break;
      case lineto: {
        if (format == 31) {
		const Point & p = elem.getPoint(0);
		buffer << (float)(PntFig * lastp.x_) << " "
		       << (float)(y_offset - (PntFig * lastp.y_)) <<  " ";
		j++; if (j == maxj) { 
			j=0; buffer << "\n\t"; 
		}
		buffer << (float)(PntFig * p.x_) << " "
		       << (float)(y_offset - (PntFig * p.y_)) <<	" ";
		j++; if (j == maxj) { 
			j=0; buffer << "\n"; 
    			if ( (n+1) != numberOfElementsInPath()) { buffer << "\t"; }
		}
		lastp = elem.getPoint(0);
	} else {
		buffer << " 0 ";
		j++; if (j == maxj) { 
			j=0; buffer << "\n"; 
    			if ((n+1) != numberOfElementsInPath()) { buffer << "\t"; }
		}
	}
	}
	break;
      case closepath: {
        if (format == 31) {
		const Point & p = pathElement(0).getPoint(0);
		buffer << (float)(PntFig * lastp.x_) << " "
		       << (float)(y_offset - (PntFig * lastp.y_)) <<  " ";
		j++; if (j == maxj) { 
			j=0; buffer << "\n\t"; 
		}
		buffer << (float)(PntFig * p.x_) << " "
		       << (float)(y_offset - (PntFig * p.y_)) <<	" ";
		j++; if (j == maxj) { 
			j=0; buffer << "\n"; 
    			if ( (n+1) != numberOfElementsInPath()) { buffer << "\t"; }
		}
		lastp = pathElement(0).getPoint(0);
	} else {
		buffer << " 0 ";
		j++; if (j == maxj) { 
			j=0; buffer << "\n"; 
    			if ( (n+1) != numberOfElementsInPath()) { buffer << "\t"; }
		}
	}
	}
	break;
      case curveto: {
        if (format == 31) {
	  	for (unsigned int cp = 0 ; cp < 2; cp++ ) {
		    	const Point & p = elem.getPoint(cp);
			    buffer << (float)(PntFig * p.x_) << " "
				   << (float)(y_offset - (PntFig * p.y_)) <<  " ";
		    	j++; if (j == maxj) { 
				j=0; buffer << "\n"; 
    		    		if (!((cp == 2) && ( (n+1) == numberOfElementsInPath()))) { buffer << "\t"; }
		    	}
		  }
	} else {
		// put all points, middle points will have control value 1
		const char * sk = "110";
		for (unsigned int i = 0 ; i < 3; i++ ){
			buffer << " " << sk[i] << " ";
		    	j++; if (j == maxj) { 
				j=0; buffer << "\n"; 
    		    		if (!((i == 2) && ((n+1) == numberOfElementsInPath()))) { buffer << "\t"; }
		    	}
		}
	}
	lastp = elem.getPoint(2);
	}
	break;
      default:
	errf << "\t\tFatal: unexpected case in drvfig " << endl;
	abort();
	break;
    }
  }
  if (format == 31) {
  	if (j == maxj) { j=0; buffer << "\n\t"; }
  	buffer << "0.0 0.0\n";
  } else {
	if (j != 0) { buffer << endl; }
  }
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

static int getfigfontnumber(const char * fname)
{
    size_t fntlength = strlen(fname);
    size_t MAXFNTNUM = sizeof(FigFonts)/(sizeof(char *)) - 1;
    for (unsigned int i=0; i<=MAXFNTNUM; i++) {
	if (fntlength == strlen(FigFonts[i])) {
	    if (strncmp(fname,FigFonts[i],fntlength) == 0)
		return i;
	}
    }
    return -1;
}


void drvFIG::show_text(const TextInfo & textinfo)
{
    const float toRadians = 3.14159265359f / 180.0f;

    int FigFontNum = getfigfontnumber( textinfo.currentFontName.value());
    if (FigFontNum == -1) {
      	errf << "Warning, unsupported font " << textinfo.currentFontName.value() << ", using ";
    	FigFontNum = getfigfontnumber( defaultFontName);
    	if (FigFontNum != -1) {
		errf << defaultFontName;
      	} else {
      		if (strstr (textinfo.currentFontName.value(), "Bold") == 0) {
		if (strstr (textinfo.currentFontName.value(), "Italic") == 0) {
		  errf << "Times-Roman";
		  FigFontNum = 0; // Times-Roman
		} else {
		  FigFontNum = 1;
		  errf << "Times-Italic";
		}
	      } else {
		if (strstr (textinfo.currentFontName.value(), "Italic") == 0) {
		  errf << "Times-Bold";
		  FigFontNum = 2; // Times-Bold
		} else {
		  FigFontNum = 3;
		  errf << "Times-BoldItalic";
		}
	      }
    	}
	errf << " instead." << endl;
    }

    const int color = registercolor(textinfo.currentR,textinfo.currentG,textinfo.currentB);
    float localFontSize = textinfo.currentFontSize;
    if (localFontSize <=  0.1) { localFontSize = 9; }
    localFontSize++; // There appears to be a reduction for some reason
    const float FigHeight = 1200.0f * localFontSize / 72.0f;
    const float FigLength = FigHeight * strlen(textinfo.thetext.value());
    buffer << "# text\n";
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
	 << textinfo.thetext.value() << "\\001\n";
}

void drvFIG::show_path()
{
    
    float localLineWidth = currentLineWidth();
    if ((localLineWidth < 0.0) ||
       ((localLineWidth > 0.0) && (localLineWidth <= 1.0)))
	       { localLineWidth = 1.0; }
    int linestyle = 0;
    switch (currentLineType()) {
		case solid		: linestyle = 0; break;
		case dashed		: linestyle = 1; break;
		case dashdot	: linestyle = 3; break;
		case dotted		: linestyle = 4; break;
		case dashdotdot : linestyle = 2; break;
    }

	unsigned int curvetos = nrOfCurvetos();
	if(curvetos == 0) // polyline
	{
		buffer << "# polyline\n";
		buffer << "2 1 " << linestyle << " " << (int)localLineWidth << " " ;
		const int color = registercolor(currentR(),currentG(),currentB());
		const int fill_or_nofill = (currentShowType() == drvbase::stroke) ? -1 : 20;
		if (objectId) objectId--; // don't let it get < 0
		buffer << color <<  " " << color << " " << objectId << " 0 " << fill_or_nofill <<  " " << "4.0" << " 0 0 0 0 0 ";
       // 4.0 is the gap spec. we could also derive this from the input
		buffer << (int)(numberOfElementsInPath()) << "\n";
		print_polyline_coords();
	}
	else // contains at least one curveto 
	{
	if (format != 31) {
		buffer << "# spline\n";
	 // 3 2 means "open interpolated spline"
		buffer << "3 4 " << linestyle << " " << (int)localLineWidth << " " ;
		const int color = registercolor(currentR(),currentG(),currentB());
		const int fill_or_nofill = (currentShowType() == drvbase::stroke) ? -1 : 20;
		if (objectId) objectId--; // don't let it get < 0
		buffer << color <<  " " << color << " " << objectId << " 0 " << fill_or_nofill <<  " " << "4.0" << " 0 0 0 ";
     // 4.0 is the gap spec. we could also derive this from the input
		if (format == 31) {
   			buffer << (int)(numberOfElementsInPath() - 1) << "\n";
		} else {
			buffer << numberOfElementsInPath() + 3 * curvetos  - curvetos << "\n";
		}
		print_spline_coords1();
		print_spline_coords2();
	} else {
		print_spline_coords_noxsplines(linestyle,localLineWidth);
	}
	}
}

void drvFIG::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
    // just do show_polyline for a first guess
    unused(&llx);
    unused(&lly);
    unused(&urx);
    unused(&ury);
    show_path();
}

void drvFIG::show_image(const Image & imageinfo)
{
    if(outDirName == NULL || outBaseName == NULL) {
	errf << "images cannot be handled via standard output. Use an output file " << endl;
    	return;
    }

    char* EPSoutFileName = new char [strlen(outBaseName)+21];
    char* EPSoutFullFileName = new char [strlen(outDirName)+strlen(outBaseName)+21];

    sprintf(EPSoutFileName, "%s%02d.eps", outBaseName, imgcount++);
    sprintf(EPSoutFullFileName, "%s%s", outDirName, EPSoutFileName);
    ofstream outi(EPSoutFullFileName);
    if(!outi)
    {
	errf << "Could not open file " << EPSoutFullFileName << " for output";
	exit(1);
    }

    // first output link to an external *.eps file into *.fig file
    buffer << "# image\n";
    buffer << "2 5 0 1 -1 -1 ";
    if (objectId) objectId--; // don't let it get < 0
    buffer << objectId << " 0 -1 0.000 0 0 -1 0 0 5\n";
    buffer << "\t0 " << EPSoutFileName << "\n";

    // remember, we have to flip the image from PostScript coord to fig coords
    Point ll,ur;
    imageinfo.getBoundingBox(ll,ur);
    Point fig_ur(PntFig*ur.x_,y_offset - PntFig*ll.y_);
    Point fig_ll(PntFig*ll.x_,y_offset - PntFig*ur.y_);

    buffer << "\t" << (int)fig_ll.x_ << " " << (int)fig_ll.y_ << " "
		   << (int)fig_ur.x_ << " " << (int)fig_ll.y_ << " "
		   << (int)fig_ur.x_ << " " << (int)fig_ur.y_ << " "
		   << (int)fig_ll.x_ << " " << (int)fig_ur.y_ << " "
		   << (int)fig_ll.x_ << " " << (int)fig_ll.y_;
    buffer << "\n";



    imageinfo.writeEPSImage(outi);
    outi.close();

    delete[] EPSoutFullFileName;
    delete[] EPSoutFileName;
}

static DriverDescriptionT<drvFIG> D_fig(
		"fig"  ,".fig format for xfig","fig",
	false,true,true,true,true,DriverDescription::normalopen,false);
static DriverDescriptionT<drvFIG> D_xfig(
		"xfig",".fig format for xfig","fig",
	false,true,true,true,true,DriverDescription::normalopen,false);

 
