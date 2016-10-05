%{ 
/*
   makeedit.l : This file is part of pstoedit
   Simple parser to parse the intermediate flat PostScript and call the backend
   output routines.

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
// #include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>

#include "config.h"
#include "drvTGIF.h"
#include "drvMIF.h"
#include "drvFIG.h"

#ifdef WITHPDF
#include "drvPDF.h"
#endif

#ifdef WITHCGM
#include "drvCGM.h"
#endif


drvbase * outputdriver;

static float  origx, origy;
static char   *start_of_text;
static char   *end_of_text;
static float  number;

%}
%s READPATH
%%
[0-9.][0-9.\-e]* 	{ 
			 if (!sscanf(yytext,"%f",&number)) {
			   cerr << "sscanf failed: " << yytext << endl;
			 }; 
			 /* printf(" adding  %f\n",number); */
			 outputdriver->addNumber(number * outputdriver->get_scale());
			}
<INITIAL>"showpage" { 
			if (!outputdriver->page_empty) {
				outputdriver->close_page();
			}
			outputdriver->page_empty = 1;
			}
<INITIAL>"newpath" { 
			BEGIN(READPATH); 
			outputdriver->currentSegment().currenttype=drvbase::polyline; /* init */
			outputdriver->newSegment();
			}
<INITIAL>(.)*" findfont "    {
			/* /Helvetica findfont 14 scalefont setfont */
			start_of_text = strchr (yytext,(int)'/') + 1 ;
			end_of_text = strchr (yytext,(int)' ') ;
			*end_of_text = (char) 0;
			char * ATposition = 0; // look for a @ in name. This is inserted by groff.
					       // Replace @ with end-of-string
			// This is a temporary fix to the more general problem of font encodings
			if ((ATposition = strchr(yytext,(int)'@')) != 0 ) {
				*ATposition = (char) 0;
			}
			outputdriver->setCurrentFontName(start_of_text);
			}
"% "(.)*" FamilyName"   { 
			/* fprintf(stderr,"%s\n",yytext);  */
			start_of_text = yytext+2;
			end_of_text =  strrchr (yytext,(int)' ') ;
			*end_of_text = (char) 0;
			outputdriver->setCurrentFontFamilyName(start_of_text);
			}
"% "(.)*" FullName"     { 
			/* fprintf(stderr,"%s\n",yytext);  */
			start_of_text = yytext+2;
			end_of_text =  strrchr (yytext,(int)' ') ;
			*end_of_text = (char) 0;
			outputdriver->setCurrentFontFullName(start_of_text);
			}
"% "(.)*" Weight"       { 
			/* fprintf(stderr,"%s\n",yytext);  */
			start_of_text = yytext+2;
			end_of_text =  strrchr (yytext,(int)' ') ;
			*end_of_text = (char) 0;
			outputdriver->setCurrentFontWeight(start_of_text);
			}
"% "(.)*" MSG\n"	{
				cerr << yytext;
			}
"%"(.)*"\n"		{ }
<INITIAL>"scalefont setfont" {
			outputdriver->setCurrentFontSize(number * outputdriver->get_scale());
			outputdriver->pop(); 
			}
"setrgbcolor" {
			float B = outputdriver->pop() / outputdriver->get_scale();
			float G = outputdriver->pop() / outputdriver->get_scale();
			float R = outputdriver->pop() / outputdriver->get_scale();
			/* printf("%s %f %f %f\n","setting RGB to ",R,G,B); */
			outputdriver->setRGB(R,G,B);
			}
"setgray" {
			outputdriver->setGrayLevel(number);
			outputdriver->pop();
			}
"setlinewidth" {
			outputdriver->setcurrentLineWidth(number * outputdriver->get_scale() );
			outputdriver->pop(); 
			}
<INITIAL>\((.)*\)" show" 	{ 
			start_of_text = strchr (yytext,(int)'(') + 1 ;
			end_of_text = strrchr (yytext,(int)')') ;
			*end_of_text = (char) 0;
			outputdriver->showText(start_of_text);
			}
<INITIAL>" rotate"	{
			outputdriver->setCurrentFontAngle(number);
			outputdriver->pop();
			}
<INITIAL>" translate"	{
			/* Because this is in the preamble, only pop 
			   this has put something on the stack. */
			if (outputdriver->nrOfPointsInSegment(outputdriver->currentSegment()) >= 2)
				{ outputdriver->pop(); outputdriver->pop(); }
			}
<INITIAL>" ntranslate"	{
			/* Because this is in the preamble, only pop 
			   this has put something on the stack. */
			if (outputdriver->nrOfPointsInSegment(outputdriver->currentSegment()) >= 2)
				{ outputdriver->pop(); outputdriver->pop(); }
			}
<INITIAL>.|\n { /* ignore any other stuff */ }
<READPATH>"moveto" { 
		    	if (outputdriver->nrOfPointsInSegment(outputdriver->currentSegment()) >= 2) {
				origx = outputdriver->numbers[outputdriver->currentSegment().end-2];
				origy = outputdriver->numbers[outputdriver->currentSegment().end-1];
				// save coordinates for possible closepath
			    	if (outputdriver->nrOfPointsInSegment(outputdriver->currentSegment()) > 2) {
			       		/* this is an intermediate moveto */
					/* take away the last point */
				        outputdriver->pop(); 
				        outputdriver->pop(); 

				/*	outputdriver->showSegments(outputdriver->noFillValue); */
			 		outputdriver->newSegment();
					/* start a new segment */
					outputdriver->addNumber(origx);
					outputdriver->addNumber(origy);
		     		}
		     	}
		   }
<READPATH>"lineto" { }
<READPATH>"closepath"  	{
			 outputdriver->addNumber(origx);
			 outputdriver->addNumber(origy);
			 outputdriver->currentSegment().currenttype=drvbase::polygon;
			 outputdriver->newSegment();
			 /* show is done by next stroke, fill or moveto */
			}
<READPATH>"stroke"  	{ 
			 BEGIN(INITIAL);
			 outputdriver->showSegments(outputdriver->noFillValue);
                       	};
<READPATH>"fill"|"eofill"  { 
			 BEGIN(INITIAL);
			 /* don't show border with fill */
			 outputdriver->setcurrentLineWidth(0);  
			 outputdriver->showSegments(outputdriver->FillValue); 
                       	};
<READPATH>.|\n {}
%%

enum driverType { unknown, tgif, mif, fig, cgm, pdf };

int main(int argc, char* const * argv)
// g++ wants 'char * const *' here, but that's a problem in getopts prototype
// Suns SparcCompiler expects 'const char* const *' for getopt
{
    int     c;
    char    *nameOfOutputFile = 0;
    driverType driver = unknown;

//    extern int optind;
//   extern int opterr;
#ifdef __GNUG__
    extern char *optarg;
#endif
    float  magnification = 1.0;
    while ((c = getopt(argc, argv, "m:f:o:")) != EOF)
	switch (c) {
	case 'm':
	    if (!sscanf(optarg, "%f", &magnification)) {
		perror("scanning magnification");
	    }
	    break;
	case 'f':
	    if (*optarg == 't')
		driver = tgif;
	    if (*optarg == 'm')
		driver = mif;
	    if (*optarg == 'f')
		driver = fig;
	    if (*optarg == 'c')
		driver = cgm;
	    if (*optarg == 'p')
		driver = pdf;
	    break;
	case 'o':
	    {
		const int filenamelen = strlen(optarg);
		nameOfOutputFile = new char[filenamelen+1];
		strcpy(nameOfOutputFile,optarg);
		break;
	    }
	case '?':
	    cerr << "Unknown Option to makeoutput" << endl;
	    break;
	}
    if (driver == unknown) {
	cerr << "No backend specified" << endl;
	delete nameOfOutputFile;
	exit(1);
    } else {
	if (!nameOfOutputFile) {
		cerr << "No output file specified" << endl;
		exit(1);
	}
	ofstream outFile;
	outFile.open(nameOfOutputFile);
	if (outFile.fail() ) {
		cerr << "Could not open file " << nameOfOutputFile << " for output" << endl;
		delete nameOfOutputFile;
		exit(1);
	}
	switch (driver) {
	case unknown:
		// cannot happen
		break;
	case tgif:
		outputdriver = new drvTGIF(outFile,magnification);
		break;
	case mif:
		outputdriver = new drvMIF(outFile,magnification);
		break;
	case fig:
		outputdriver = new drvFIG(outFile,magnification);
		break;
	case cgm:
#ifdef WITHCGM
		outputdriver = new drvCGM(outFile,magnification);
#endif
		break;
	case pdf:
#ifdef WITHPDF
		outputdriver = new drvPDF(outFile,magnification);
#endif
		break;
	default:
		break;
	}
 	outputdriver->setGrayLevel((float) 0.0);
        outputdriver->print_header();
        yylex();
        outputdriver->print_trailer();
	delete outputdriver;
    }
    return 0;
}
