/*
   pstoedit.cpp : This file is part of pstoedit
   main control procedure

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

#include <iostream.h>
#include <fstream.h>
// rcw2: work round case insensitivity in RiscOS
#ifdef riscos
  #include "unix:string.h"
#else
  #include <string.h>
#endif
#include <stdlib.h>

// #ifdef _MSC_VER
// for getcwd ( at least for Visual C++)

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__EMX__)
#include <unistd.h>
#else
#include <direct.h>
#endif

#include "pstoedit.h"
#include "version.h"
#include "config.h"

#if WITHWMF
#include "drvwmf.h"
#endif    

#if WITHTGIF
#include "drvtgif.h"
#endif    

#if WITHIDRAW
#include "drvidraw.h"
#endif    

#if WITHRPL
#include "drvrpl.h"
#endif    

#if WITHRIB
#include "drvrib.h"
#endif    

#if WITHLWO
#include "drvlwo.h"
#endif    

#if WITHMIF
#include "drvmif.h"
#endif    

#if WITHFIG
#include "drvfig.h"
#endif    

#if WITHGNUPLOT
#include "drvgnplt.h"
#endif    

#if WITHDXF
#include "drvdxf.h"
#endif    

#if WITHMET && defined(__OS2__)
#include <io.h>
#include "drvMET.h"
#endif    

#if WITHPDF
#include "drvpdf.h"
#endif

#if WITHCGM
#include "drvcgm.h"
#endif

#if WITHJAVA
#include "drvjava.h"
#endif

// next include only for verifying that drvsampl is kept up to date
#if WITHSAMPLE
#include "drvsampl.h"
#endif


#ifndef USEPROLOGFROMFILE
#include "pstoedit.ph"
#endif

#include "psfront.h"

#ifdef riscos
// rcw2: tempnam doesn't seem to be defined in UnixLib 3.7b for RiscOS
char *tempnam(const char *, const char *pfx)
{
	char tmp[1024];
	
	strcpy(tmp,"<Wimp$ScrapDir>.");
	strcat(tmp,pfx);
	return strdup(tmp);
}
#endif

void usage(ostream & errstream) 
{
	errstream << "usage: pstoedit [-help] [-dt] [-merge] [-df fontname] [-s nn] [-dis] [-nomaptoisolatin1] [-nq] [-flat nn] [-bo] [-psargs string] [-include file] -f format [infile [outfile]]" << endl;
}


// cannot make this static, because it's used in a template, that might be
// stored in a repository (separate .o file)
int checkNextArgument(int argc, char ** argv, int arg,ostream & errstream) 
{
	// if there is no next arg, usage is printed and
	// the program is terminated
	if ((arg+1>= argc ) || (argv[arg+1] == 0)) {
		errstream << "missing value for option " << argv[arg] << endl;
		usage(errstream);
		exit(1);
	}
	return 1;
}


// made non template, because some compilers (cfront) have
// problems with instantiating this
// template <class T>
// void getvalue(int argc, char ** argv, int arg, T & value) 
void getvalue(int argc, char ** argv, int arg, float & value,ostream& errstream) 
{
	// get value for option arg (value is next argument)
	if (checkNextArgument(argc,argv,arg,errstream)) {

// Work around for Linux libg++ bug (use atof instead of strstream)
		value = (float) atof(argv[arg+1]);
// C++'ish	istrstream strvalue(argv[arg+1]);
//		strvalue >> value;

	}
}

static const char * full_qualified_tempnam(const char * pref)
{
#if defined (__BCPLUSPLUS__)
/* borland has a prototype that expects a char * as second arg */ 
	const char * filename = tempnam(0,(char *) pref);
#else
	const char * filename = tempnam(0,pref);
#endif
	// W95: Fkt. tempnam() erzeugt Filename+Pfad
  	// W3.1: es wird nur der Name zurueckgegeben

// rcw2: work round weird RiscOS naming conventions
#ifndef riscos
	if ( (strchr(filename,'\\')==0) &&
	     (strchr(filename,'/') ==0) ) { // keine Pfadangaben..
		char cwd[400];
	  	getcwd(cwd,400); //JW akt. Verzeichnis holen
		char * result = new char [strlen(filename) + strlen(cwd) + 2];
		strcpy(result,cwd);
		strcat(result,"/"); 
		strcat(result,filename);
		freeconst(filename);
		return result;
	} else 
#endif
	{
		return filename;
	}
}

static int grep(const char * const matchstring, const char * const filename,ostream& errstream)
{
// for call: gsresult = grep("% normal end reached by pstoedit.pro",gsout);
	ifstream inFile;
	inFile.open(filename);
	if (inFile.fail() ) {
		errstream << "Could not open file " << filename << " in grep" << endl;
		return 1; // fail
	} else {
		const unsigned int matchlen = strlen(matchstring) ;
		const unsigned int bufferlen = matchlen + 1;
		// allocate buffer for reading begin of lines
		char * buffer = new char[bufferlen];
		while (inFile.get(buffer,bufferlen,'\n'), !inFile.eof() ) {
//debugonly			errstream << " read in grep :`" << buffer << "'" << inFile.gcount() << " " << matchlen << endl;
			if ((inFile.gcount() == matchlen) && 
			    (strcmp(buffer,matchstring) == 0) ) {
				delete [] buffer;
				return 0;
			}
			if ( inFile.peek() == '\n' ) inFile.ignore();
		}
		delete [] buffer;
	}
	return 1; // fail
}


enum driverType { unknown, tgif, idraw, rpl, lwo, rib, mif, fig, cgm, cgmt, pdf, gnuplot, ps, debug, met, wmf, dxf,java, sample };

struct driverdescription_T {
	driverdescription_T(const char * s_name, const char * expl, driverType code, bool enabled_p):
		symbolicname(s_name),
		explanation(expl),
		internalcode(code),
		enabled(enabled_p) {}
	const char * const symbolicname;
	const char * const explanation;
	const driverType internalcode;
	const bool enabled;
};

static const driverdescription_T driverdescription [] = {
	driverdescription_T("tgif","Tgif .obj format (for tgif version >= 3)",tgif,WITHTGIF),
	driverdescription_T("rpl","Real3D Programming Language Format",rpl,WITHRPL),
	driverdescription_T("lwo","LightWave 3D Object Format",lwo,WITHLWO),
	driverdescription_T("rib","RenderMan Interface Bytestream",rib,WITHRIB),
	driverdescription_T("mif","(Frame)Maker Intermediate Format",mif,WITHMIF),
	driverdescription_T("fig" ,".fig format for xfig",fig,WITHFIG),
	driverdescription_T("xfig",".fig format for xfig",fig,WITHFIG),
	driverdescription_T("pdf","Adobe's Portable Document Format",pdf,WITHPDF),
	driverdescription_T("gnuplot","gnuplot format",gnuplot,WITHGNUPLOT),
	driverdescription_T("ps","Flattened PostScript",ps,1),
	driverdescription_T("debug","for test purposes ",debug,1),
	driverdescription_T("dump","for test purposes (same as debug)",debug,1),
	driverdescription_T("met","OS/2 meta files",met,WITHMET),
	driverdescription_T("wmf","Windows meta files",wmf,WITHWMF),
	driverdescription_T("dxf","CAD exchange format",dxf,WITHDXF),
	driverdescription_T("cgm","CGM Format (binary)",cgm,WITHCGM),
	driverdescription_T("cgmt","CGM Format (textual)",cgmt,WITHCGM),
	driverdescription_T("java","java applet source code",java,WITHJAVA),
	driverdescription_T("idraw","Interviews draw format",idraw,WITHIDRAW),
	driverdescription_T("sample","insert your new format here",sample,WITHSAMPLE)
};

driverType getdrivertype(const char * drivername)
{
	const unsigned int nrOfFormats = sizeof(driverdescription) / sizeof(driverdescription_T);
	for (unsigned int i = 0; i < nrOfFormats; i++ ) {
		if ( ( strcmp(drivername,driverdescription[i].symbolicname) == 0 ) &&
		       driverdescription[i].enabled)  {
			return  driverdescription[i].internalcode ;
		} 
	}
	return unknown;
}


static void explainformats(ostream & out)
{
	const unsigned int nrOfFormats = sizeof(driverdescription) / sizeof(driverdescription_T);
	{ 
	out << "Enabled formats :\n";
	for (unsigned int i = 0; i < nrOfFormats; i++ ) 
          if (driverdescription[i].enabled)  {
		out << '\t' << driverdescription[i].symbolicname << ":\t" ;
		if (strlen(driverdescription[i].symbolicname) <7) {
			out << '\t';
		}
		out << driverdescription[i].explanation << endl;
	}
	}
	{
	out << "Disabled formats (due to machine or compiler constraints, or manual configuration (config.h)):\n";
	for (unsigned int i = 0; i < nrOfFormats; i++ ) 
          if (!driverdescription[i].enabled)  {
		out << '\t' << driverdescription[i].symbolicname << ":\t" ;
		if (strlen(driverdescription[i].symbolicname) <7) {
			out << '\t';
		}
		out << driverdescription[i].explanation << endl;
	}
	}
}

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__EMX__)
void convertBackSlashes(char* string) { unused(string); }
// nothing to do on systems with unix style file names ( / for directories)
#else
void convertBackSlashes(char* string) {

    char* c;

    while ((c = strchr(string,'\\')) != NULL)
       *c = '/';
}
#endif

static void writeFileName(ostream & outstream, const char * const filename)
{
	const unsigned int len = strlen(filename);
	for (unsigned int i = 0 ; i < len; i++ ) {
		if (filename[i] =='\\') {
			outstream << '/' ; // '/' works on DOS also
		} else {
			outstream << filename[i];
		}
	}
}


class Closer {
public:
	Closer() : fromgui(false) {};
	~Closer() {
			if (fromgui) {
				cout << "Program finished, please press CR to close window\n";
				cin.get();
			}
		  }
	bool fromgui;
	
};

int pstoedit(int argc,char **argv,ostream& errstream, 
		execute_interpreter_function call_PI,
		const char * (*whichPI)(ostream &)
		)
{
	Closer closerObject;
	// cannot be const  because it needs to be changed on non UNIX systems (convertBackSlashes)
	      char * nameOfInputFile = 0;
	const char * nameOfOutputFile = 0;
	const char * psArgs = ""; // Pass through arguments to PostScript interpreter
	const char * nameOfIncludeFile = 0; // name of an option include file
//old	const char * resolution_option = "-r72x72"; // default
	const char * replacementfont = "Courier";
						      // can be changed via -r
	bool    maptoisolatin1 = true;
	bool    withdisplay = false;
	bool    doquit 	    = true;
	bool    merge       = false;
	bool    drawtext    = false;
	bool    escapetext  = false;                        // only for PostScript backend
	bool    usepdfmark  = false;                        // only for PDF backend
	float   flatness    = 1.0f;			    // used for setflat
							    // in combination with -dt
	char    *drivername = 0;
#if WITHWMF
   DRVWMFSETUP * pDrvWMFsetup = 0;
#endif
	float   magnification = 1.0f;

	bool    backendonly = false;                        // used for easier debugging of backends
	// directly read input file by backend
	// bypass ghostscript. The input file
	// is assumed to be produced by a
	// previous call with -f debug

	errstream << "pstoedit: version " << version << " : Copyright (C) 1993,1994,1995,1996,1997 Wolfgang Glunz\n" ;
	int arg = 1;
	while (arg < argc) {
		if ( argv[arg][0] == '-' ) {
			// it is an option
			if (strncmp(argv[arg],"-s",2) == 0) {
				getvalue(argc,argv,arg,magnification,errstream);
				arg++;
			} else if (strncmp(argv[arg],"-help",2) == 0) {
				usage(errstream);
				errstream << "Default interpreter is " << whichPI(errstream) << endl;
				explainformats(errstream);
				return 1;
			} else if (strncmp(argv[arg],"-flat",3) == 0) {
				getvalue(argc,argv,arg,flatness,errstream);
				arg++;
			} else if (strncmp(argv[arg],"-bo",3) == 0) {
				backendonly = true;
			} else if (strncmp(argv[arg],"-merge",6) == 0) {
				merge = true;
			} else if (strncmp(argv[arg],"-dt",3) == 0) {
				drawtext = true;
//			} else if (strncmp(argv[arg],"-r",2) == 0) {
//				resolution_option = argv[arg];
			} else if (strncmp(argv[arg],"-dis",4) == 0) {
				withdisplay = true;
			} else if (strncmp(argv[arg],"-nomaptoisolatin1",6) == 0) {
				maptoisolatin1 = false;
			} else if (strncmp(argv[arg],"-guimode",8) == 0) {
				closerObject.fromgui = true;
			} else if (strncmp(argv[arg],"-nq",3) == 0) {
				doquit = false;
			} else if (strcmp(argv[arg],"-df") == 0) {
				if (checkNextArgument(argc,argv,arg,errstream)) {
					replacementfont = argv[arg+1];
					arg++;
				}
			} else if (strcmp(argv[arg],"-f") == 0) {
				if (checkNextArgument(argc,argv,arg,errstream)) {
					drivername = argv[arg+1];
					arg++;
				}
			} else if (strcmp(argv[arg],"-psargs") == 0) {
				if (checkNextArgument(argc,argv,arg,errstream)) {
					psArgs = argv[arg+1];
					arg++;
				}
			} else if (strcmp(argv[arg],"-include") == 0) {
				if (checkNextArgument(argc,argv,arg,errstream)) {
					nameOfIncludeFile = argv[arg+1];
					arg++;
				}
			} else {
				errstream << "unknown option " << argv[arg] << endl;
				usage(errstream);
				return 1;
			}
		} else {
			if (nameOfInputFile == 0) {
				nameOfInputFile = argv[arg];
			} else if (nameOfOutputFile == 0) {
				nameOfOutputFile = argv[arg];
			} else {
				errstream << "more than two file arguments " << endl;
				usage(errstream);
				return 1;
			}
		}
		//     errstream << "argv[" << arg << "] = " << argv[arg] << endl;
		arg++;
	}
	if (drivername == 0) {
		errstream << "No backend specified" << endl;
		usage(errstream);
		return 1;
	} else {
		char * driveroptions = strchr(drivername,':');
		if (driveroptions) {
			*driveroptions = '\0'; // replace : with 0 to separate drivername
			driveroptions++;
		}
		driverType    currentDriver = getdrivertype(drivername);
		if (currentDriver == unknown) {
			errstream << "unsupported driver " << drivername << endl;
			explainformats(errstream);
			return 1;
		} else {
			//istream *inputFilePtr = 0;
			// cannot be const because nameOfInputFile cannot be const
			// because it needs to be changed on non UNIX systems (convertBackSlashes)
			char stdinFileName[10] ;
			strcpy(stdinFileName,"%stdin");  // for PostScript
//using 'char * const stdinFileName = "%stdin";' is "wrong" with non writeable strings
			if (nameOfInputFile) {
				// an input file was given as argument

				// just test whether InputFile is readable.
				// The file will be read directly from the PostScrip
                                // interpreter later on
                                convertBackSlashes(nameOfInputFile);
				ifstream inFile(nameOfInputFile);
				if (!inFile) {
					errstream << "Could not open file " << nameOfInputFile << " for input" << endl;
					return 1;
				}
				// done by destructor  inFile.close();
			} else {
				nameOfInputFile = stdinFileName;
			}
			ostream *outputFilePtr = 0;
			ofstream outFile;
			if (nameOfOutputFile) {
       	 			if (currentDriver != wmf ){ // no need to open file for wmf
       	 				if (currentDriver == cgm ||
					    currentDriver == lwo) {
#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__EMX__)
// binary is not available on UNIX, only on PC
						outFile.open(nameOfOutputFile);
#else
						outFile.open(nameOfOutputFile,ios::binary);
#endif
					} else 
					{
						outFile.open(nameOfOutputFile);
					}
					if (outFile.fail() ) {
						errstream << "Could not open file " << nameOfOutputFile << " for output" << endl;
						return 1;
					}
					outputFilePtr = &outFile;
				}
			} else {
				outputFilePtr = &cout;
			}

			drvbase * outputdriver = 0;
			switch (currentDriver) {
#if WITHCGM
			case cgm:
				outputdriver = new drvCGM(driveroptions,*outputFilePtr,errstream,1);
				break;
			case cgmt:
				outputdriver = new drvCGM(driveroptions,*outputFilePtr,errstream,0);
				break;
#endif
#if WITHDXF
			case dxf:
				outputdriver = new drvDXF(driveroptions,*outputFilePtr,errstream);
				break;
#endif 
			case debug:
				// no driver needed for debug/dump output
				usepdfmark = true;
				break;
#if WITHGNUPLOT
			case gnuplot:
				outputdriver = new drvGNUPLOT(driveroptions,*outputFilePtr,errstream);
				break;
#endif
#if WITHFIG
			case fig:
				outputdriver = new drvFIG(driveroptions,*outputFilePtr,errstream);
				break;
#endif
#if WITHSAMPLE
			case sample:
				outputdriver = new drvSAMPL(driveroptions,*outputFilePtr,errstream);
				break;
#endif
#if WITHJAVA
			case java:
				outputdriver = new drvJAVA(driveroptions,*outputFilePtr,errstream);
				break;
#endif
#if WITHLWO
			case lwo:
				outputdriver = new drvLWO(driveroptions,*outputFilePtr,errstream);
				break;
#endif
#if WITHRIB
			case rib:
				outputdriver = new drvRIB(driveroptions,*outputFilePtr,errstream);
				break;
#endif
#if WITHRPL
			case rpl:
				outputdriver = new drvRPL(driveroptions,*outputFilePtr,errstream);
				break;
#endif
#if WITHMIF
			case mif:
				outputdriver = new drvMIF(driveroptions,*outputFilePtr,errstream);
				break;
#endif
#if WITHMET && defined(__OS2__)
			case met:
				// driver specific options are appended to the drivers name
				// e.g. -f met:wp
				// The backend is responsible for parsing the additional option string
				outFile.close();
				pDrvMETsetup = new DRVMETSETUP(driveroptions);
				if (pDrvMETsetup->exit) {
				  	delete pDrvMETsetup;
				  	return 0;
				}
				if (!nameOfOutputFile) {
				  	nameOfOutputFile = "stdout.met";
				}
				pDrvMETsetup->pMetaFileName = new char[strlen(nameOfOutputFile)+1];
				strcpy(pDrvMETsetup->pMetaFileName,nameOfOutputFile);
				break;
#endif
// JW
#if WITHWMF
			case wmf:     {
				// driver specific options are appended to the drivers name
				// e.g. -f wmf:wp
				// The backend is responsible for parsing the additional option string
				// errstream << "wmf-driver selected" << endl;
//				outFile.close(); // MetaFileDC oeffnet sein eigenes File
            			pDrvWMFsetup = new DRVWMFSETUP ;
			
			      	memset(pDrvWMFsetup,'\0',sizeof(DRVWMFSETUP));

				if (pDrvWMFsetup->exit) {
				  	delete pDrvWMFsetup;
				  	return 0;
				}

        			if(driveroptions)
					strcpy(pDrvWMFsetup->wmf_options, driveroptions);

				if (nameOfOutputFile==0) {
				  	nameOfOutputFile="drvWMF.out";
				}
				pDrvWMFsetup->pOutFileName = new char[strlen(nameOfOutputFile)+1];
				strcpy(pDrvWMFsetup->pOutFileName,nameOfOutputFile);
				pDrvWMFsetup->pInFileName = new char[strlen(nameOfInputFile)+1];
				strcpy(pDrvWMFsetup->pInFileName,nameOfInputFile);

		    		errstream << "WMF Driver Options: " << pDrvWMFsetup->wmf_options << endl;
		    		for(int i=0;i<strlen(pDrvWMFsetup->wmf_options);i++)  {
			   		switch((int)pDrvWMFsetup->wmf_options[i]) {

				   	case('v'):
					   	errstream << "wmf: verbose option selected" << endl;
					    	pDrvWMFsetup->info=1;
					    	break;
				   	case('e'):
					   	errstream << "wmf: enhanced meta file format selected" << endl;
					    	pDrvWMFsetup->enhanced=1;
					    	break;
				    	default:
					   	errstream << "wmf: unknown option: '" << pDrvWMFsetup->wmf_options[i] << "'" << endl;
					   	break;

			    		}
		    		}
            			outputFilePtr=&cout;
		   		outputdriver = new drvWMF(*outputFilePtr,errstream ,magnification,pDrvWMFsetup->wmf_options ,pDrvWMFsetup);
            			}
				break;
#endif

#if WITHPDF
			case pdf:
				outputdriver = new drvPDF(driveroptions,*outputFilePtr,errstream);
				usepdfmark = true;
				break;
#endif
			case ps:
				escapetext = true;
				break;
#if WITHIDRAW
			case idraw:
				outputdriver = new drvIDRAW(driveroptions,*outputFilePtr,errstream);
				break;
#endif
#if WITHTGIF
			case tgif:
				outputdriver = new drvTGIF(driveroptions,*outputFilePtr,errstream,magnification);
				break;
#endif
			default:
				errstream << "unsupported driver " << drivername << endl;
				explainformats(errstream);
				return 1;
				// unreachable break;
			}

			const char * gsoutName = 0;
			const char * gsout;
			int gsresult = 0;
			if (backendonly) {
				gsout = nameOfInputFile;
				gsresult = 0;                       // gs was skipped, so there is no problem
			} else {
				const char * gsin = full_qualified_tempnam("psin");
				ofstream inFileStream(gsin);
				if (!maptoisolatin1) {
					inFileStream << "/maptoisolatin1 false def" << endl;
				}
				if (drawtext) {
					inFileStream << "/textastext false def" << endl;
					inFileStream << "/flatnesstouse " << flatness << " def" << endl;
				}
				if (escapetext) {
					inFileStream << "/escapetext true def" << endl;
				}
				if (usepdfmark) {
					inFileStream << "/usepdfmark true def" << endl;
				}

				inFileStream << "/replacementfont (" << replacementfont << ") def" << endl;
	
				gsout = gsoutName = full_qualified_tempnam("psout");
				if (!nameOfOutputFile) {
					inFileStream << "/redirectstdout true def" << endl;
				}
				inFileStream << "/outputfilename (" ;
				writeFileName(inFileStream,gsout);
				inFileStream <<") def" << endl;

				inFileStream << "/inputfilename  (" ;
				writeFileName(inFileStream,nameOfInputFile);
				inFileStream <<") def" << endl;

				if (nameOfIncludeFile) {
					ifstream filetest(nameOfIncludeFile);
					if (!filetest) {
						errstream << "Could not open file " << nameOfIncludeFile << " for inclusion" << endl;
						return 1;
					}
					filetest.close();
					inFileStream << "/nameOfIncludeFile  (" ;
					writeFileName(inFileStream,nameOfIncludeFile);
					inFileStream <<") def" << endl;
				}

				if (outputdriver && outputdriver->backendSupportsCurveto ) {
					inFileStream << "/doflatten false def" << endl;
				} 

				if (outputdriver && (outputdriver->knownFontNames() != 0) ) {
					const char * const * fnames = outputdriver->knownFontNames();
					unsigned int size = 0;
					while (*fnames) { size++; fnames++; }
					inFileStream << "/pstoedit.knownFontNames " << size << " dict def" << endl;

					inFileStream << "pstoedit.knownFontNames begin" << endl;
					fnames = outputdriver->knownFontNames();
					while (*fnames) {
						inFileStream << "/" << *fnames << " true def"<< endl;
						fnames++;
					}
					inFileStream << "end" << endl;
				} 
	
#ifdef 	USEPROLOGFROMFILE
				ifstream prologue("pstoedit.pro");
				//     errstream << " copying prologue file to " << gsin << endl;
					copy_file(prologue,inFileStream);
#else
				const char * * prologueline = PS_prologue;
				while (prologueline && *prologueline ) {
					inFileStream << *prologueline << '\n';
					prologueline++;
				}
#endif
				if (doquit) {
					inFileStream << "quit" << endl;
				}
				inFileStream.close();
				// now call ghostscript
				char commandline[1000];
				// TODO check for overflow
				commandline[0]= '\0';
				const char * gstocall = whichPI(errstream);
				strcat(commandline,gstocall);
// rcw2: Current RiscOS port of gs can't find its fonts without this...
#ifdef riscos
				strcat(commandline," -I<GhostScript$Dir> ");
#endif
				strcat(commandline," -q ");
				// NOBIND disables bind in, e.g, gs_init.ps
				// these files are loaded before pstoedit.pro
				// so any already bound call to, e.g., show could
				// not be intercepted by pstoedit's show
				strcat(commandline," -dNOBIND ");
//				strcat(commandline,resolution_option);
//				strcat(commandline," ");
#if 0
				if (usepdfmark) {
					// for overloading pdfmark, we need write access to systemdict
				}
#endif
				strcat(commandline,"-dWRITESYSTEMDICT ");
				if (withdisplay) {
					strcat(commandline,"-dNOPAUSE ");
				} else {
					strcat(commandline,"-dNODISPLAY ");
				}
				strcat(commandline,psArgs);
				strcat(commandline," ");
				strcat(commandline,gsin);
				errstream << "now calling the interpreter via: " << commandline << endl;
				// gsresult = system(commandline);
				gsresult = call_PI(commandline);
				errstream << "Interpreter finished. Return status " << gsresult << endl;
				// ghostscript seems to return always 0, so
				// check whether the normal end was reached by pstoedit.pro
				remove(gsin); 
				freeconst(gsin); 
				// if really returned !0 don't grep
				if (!gsresult) gsresult = grep("% normal end reached by pstoedit.pro",gsout,errstream);
			}
			if (gsresult != 0) {
				errstream << "The interpreter seems to have failed, cannot proceed !" << endl;
				remove(gsout);
				freeconst(gsoutName);
				return 1;
			} else {
			        if ((currentDriver != debug) && (currentDriver != ps )) {
					extern FILE * yyin;         // used by lexer
					if ( backendonly && (nameOfInputFile == stdinFileName) ) {
						yyin = stdin;
					} else {
						yyin = fopen(gsout,"r");
						if (!yyin) {
							errstream << "Error opening file " << gsout << endl;
							return 1;
						}
					}
					errstream << "now postprocessing the interpreter output" << endl;
					if (currentDriver != met && currentDriver != wmf) {  //JW
						{
						PSFrontEnd fe(errstream,*outputdriver);
						fe.run(merge);
						} // local scope to force delete before delete of driver
						delete outputdriver;
					}
// JW WITHWMF addiert
#if WITHWMF
					else if (currentDriver == wmf) { // driver is wmf

						errstream << "Processing driver wmf" << endl;

						// Input-Filenamen setzen
						pDrvWMFsetup->infile = new char[strlen(gsout)+1];
						strcpy(pDrvWMFsetup->infile, gsout);

						{
						PSFrontEnd fe(errstream,*outputdriver);
				    		fe.run(0); // 0: merging wird nicht unterstuetzt
						}

						delete [] pDrvWMFsetup->infile;
						delete [] pDrvWMFsetup->pOutFileName;
						delete [] pDrvWMFsetup->pInFileName;
						delete outputdriver;
						delete pDrvWMFsetup;


						errstream << "wmf-backend finished." << endl;

#if 0
						sprintf(tmpstring,"Output written to: '%s'",nameOfOutputFile);
						MessageBox(NULL,tmpstring,"Conversion finished", MB_OK);
#endif
					}
#endif
#if WITHMET && defined(__OS2__)
                			else if (currentDriver == met) {          // driver is met
   						OS2WIN os2win;
						os2win.run();
						delete pDrvMETsetup->pMetaFileName;
						delete pDrvMETsetup;
					}
#endif

					if ( !( backendonly && (nameOfInputFile == stdinFileName) ) ) {
						fclose(yyin);
					}
				} else {
					// Debug or PostScript driver
					ifstream gsoutStream(gsout);
			                // errstream << "now copying  " << gsout << " to output " << endl;
					copy_file(gsoutStream,*outputFilePtr);
					// errstream << " done \n";
				}
				if ( !backendonly ) {
					remove(gsout);
				}
			}
			freeconst(gsoutName);
		}
	}
	return 0;
}
