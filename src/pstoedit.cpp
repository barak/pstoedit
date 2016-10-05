/*
   pstoedit.cpp : This file is part of pstoedit
   main control procedure

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
#include "cppcomp.h"

#include I_fstream
#include I_stdio
#include I_stdlib
#include I_string_h

#include <assert.h>

#include "pstoedit.h"

// for the DLL export data types (the description struct)
#include "pstoedll.h"

#include "version.h"

#include "miscutil.h"

#include "drvbase.h"

#include "dynload.h"

#define strequal(s1,s2) (strcmp(s1,s2) == 0)

#ifndef USEPROLOGFROMFILE
#include "pstoedit.ph"
#endif

#include "psfront.h"

extern const char * defaultPIoptions(ostream & errstream,int verbose); // in callgs.cpp

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


static void usage(ostream & errstream)
{
	errstream << "usage: pstoedit [-v] [-help] [-split] [-page nn] [-dt] "
		"[-merge] [-df fontname] [-pagesize pagesize(e.g. a4)] [-scale nn] [-dis] [-nomaptoisolatin1]"
		"[-nc] [-nq] [-nb] [-flat nn] [-bo] [-psarg string] [-include file] "
		"[-sclip] [-ssp] [-fontmap mapfile] -f format [infile [outfile]]" << endl;
}


// cannot make this static, because it's used in a template, that might be
// stored in a repository (separate .o file)
static int checkNextArgument(int argc, const char * const argv[], int arg,ostream & errstream)
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
static void getfvalue(int argc, const char * const argv[], int arg, float & value,ostream& errstream)
{
	// get value for option arg (value is next argument)
	if (checkNextArgument(argc,argv,arg,errstream)) {

// Work around for Linux libg++ bug (use atof instead of strstream)
		value = (float) atof(argv[arg+1]);
// C++'ish	istrstream strvalue(argv[arg+1]);
//		strvalue >> value;

	}
}
static void getivalue(int argc, const char * const argv[], int arg, int & value,ostream& errstream)
{
	// get value for option arg (value is next argument)
	if (checkNextArgument(argc,argv,arg,errstream)) {

// Work around for Linux libg++ bug (use atof instead of strstream)
		value = (int) atoi(argv[arg+1]);
// C++'ish	istrstream strvalue(argv[arg+1]);
//		strvalue >> value;

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
		const size_t matchlen = strlen(matchstring) ;
		const size_t bufferlen = matchlen + 1;
		// allocate buffer for reading begin of lines
		char * buffer = new char[bufferlen];


		while (inFile.get(buffer,bufferlen,'\n'), !inFile.eof() /* && !inFile.fail() */ ) {
// debug	errstream << " read in grep :`" << buffer << "'" << inFile.gcount() << " " << matchlen << endl;
			// gcount returns int on MSVC 
#ifdef HAVESTL
		// Notes regarding ANSI C++ version (from KB)
		// istream::get( char* pch, int nCount, char delim ) is different in three ways: 
		//•When nothing is read, failbit is set.
		//•An eos is always stored after characters extracted (this happens regardless of the outcome).
		//•A value of -1 for nCount is an error.

			// If the line contains just a \n then the failbit
			// is set in the ANSI version
			if (inFile.gcount() == 0) { inFile.clear(); } 
			else 
#endif
			if ((inFile.gcount() ==  matchlen) &&
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



static DriverDescription D_psf("psf","Flattened PostScript (no curves)","fps",1,0,1,1,1,DriverDescription::normalopen,true);
static DriverDescription D_ps("ps","PostScript","fps",1,1,1,1,1,DriverDescription::normalopen,true);
static DriverDescription D_debug("debug","for test purposes","dbg",1,1,1,1,1,DriverDescription::normalopen,true);
static DriverDescription D_dump("dump","for test purposes (same as debug)","dbg",1,1,1,1,1,DriverDescription::normalopen,true);
static DriverDescription D_gs("gs","any device that GhostScript provides - use gs:format, e.g. gs:pdfwrite","gs",1,1,1,1,1,DriverDescription::normalopen,true);
static DriverDescription D_ps2ai("ps2ai","Adobe Illustrator via ps2ai.ps of GhostScript","ai",1,1,1,1,1,DriverDescription::normalopen,false);

class Closer {
public:
	Closer() : fromgui(false) {};
	~Closer() {
			if (fromgui) {
				cerr << "Program finished, please press CR to close window\n";
				cin.get();
			}
		  }
	bool fromgui;
};



static void loadpstoeditplugins(const char * progname, ostream& errstream) {
	static bool pluginsloaded = false;
	if (pluginsloaded) return;
	char * plugindir = getRegistryValue(errstream,"common","plugindir");
	if(plugindir && strlen(plugindir) ) {
		loadPlugInDrivers(plugindir,errstream); // load the driver plugins
		pluginsloaded = true;
	}	

	// also look in the directory where the pstoedit .exe/dll was found
	char szExePath[1000];
	szExePath[0]= '\0';
	const int r = P_GetPathToMyself(progname,szExePath, sizeof(szExePath));
//	errstream << "r : " << r << " " << szExePath<< endl;
	char * p = 0;
	if ( r &&  (p = strrchr(szExePath,directoryDelimiter)) != 0) {
	   	*p = '\0';
		if (strcmp(szExePath,plugindir?plugindir:"") != 0) {
			loadPlugInDrivers(szExePath,errstream);
			pluginsloaded = true;
		}
	}

	delete [] plugindir;
}


extern FILE * yyin;	    // used by lexer 
					    // This has to be declared here because of the extern "C"
						// otherwise we could declare it locally where it is used

extern "C" DLLEXPORT 
int pstoedit(int argc,const char * const argv[],ostream& errstream,
		execute_interpreter_function call_PI,
		const char * (*whichPI)(ostream &,int),
		DescriptionRegister* const pushinsPtr
		)
{
	Closer closerObject;

	// cannot be const  because it needs to be changed on non UNIX systems (convertBackSlashes)
	      char * nameOfInputFile = 0;
		  char * nameOfOutputFile = 0; // can contain %d for page splitting
	//const char * psArgs = 0; // Pass through arguments to PostScript interpreter
			Argv psArgs; // Pass through arguments to PostScript interpreter
	const char * nameOfIncludeFile = 0; // name of an option include file
	const char * replacementfont = "Courier";
						      // can be changed via -r
	bool	maptoisolatin1 = true;
	bool	withdisplay = false;
	bool	doquit	    = true;
	bool	nocurves    = false; // normally curves are shown as curves if backend supports
	bool	merge	    = false;
	bool	drawtext    = false;
	bool	splitpages	= false;
	bool	verbose		= false;
	bool	simulateSubPaths = false;
	bool	loadplugins = true;
	bool	nobindversion = false; // use old NOBIND instead of DELAYBIND
	int     pagetoextract = 0; // 0 stands for all pages
	float	flatness    = 1.0f;			    // used for setflat
	bool	useFlatness = false;
	bool	simulateClipping = false; // simulate clipping
	const char*	explicitFontMapFile = 0;
	RSString outputPageSize("");

							    // in combination with -dt
	char	*drivername = 0;

	float	magnification = 1.0f;
	bool	showhelp = false;

	bool	backendonly = false;			    // used for easier debugging of backends
	// directly read input file by backend
	// bypass ghostscript. The input file
	// is assumed to be produced by a
	// previous call with -f debug

	errstream << "pstoedit: version " << version << " / DLL interface " << drvbaseVersion << " (build " << __DATE__ << ")" << " : Copyright (C) 1993 - 1999 Wolfgang Glunz\n" ;
	int arg = 1;
	drvbase::verbose=false; // init

//	obsolete now because of <driver>.fmp look-up
//	char * fontmapfile = getRegistryValue(errstream,"common","fontmap");
//	if (fontmapfile != 0) drvbase::theFontMapper.readMappingTable(errstream,fontmapfile);

	while (arg < argc) {
		if ( argv[arg][0] == '-' ) {
			// it is an option
			if (strncmp(argv[arg],"-scale",6) == 0) {
				getfvalue(argc,argv,arg,magnification,errstream);
				arg++;
			} else if (strncmp(argv[arg],"-help",2) == 0) {
				showhelp=true;
			} else if (strncmp(argv[arg],"-page",3) == 0) {
				getivalue(argc,argv,arg,pagetoextract,errstream);
				arg++;
			} else if (strncmp(argv[arg],"-flat",3) == 0) {
				useFlatness = true;
				getfvalue(argc,argv,arg,flatness,errstream);
				arg++;
			} else if (strncmp(argv[arg],"-v",2) == 0) {
				verbose = true;
			} else if (strncmp(argv[arg],"-ssp",4) == 0) {
				simulateSubPaths = true;
			} else if (strncmp(argv[arg],"-sclip",6) == 0) {
				simulateClipping = true;
			} else if (strncmp(argv[arg],"-bo",3) == 0) {
				backendonly = true;
			} else if (strncmp(argv[arg],"-merge",6) == 0) {
				merge = true;
			} else if (strncmp(argv[arg],"-dt",3) == 0) {
				drawtext = true;
			} else if (strncmp(argv[arg],"-split",6) == 0) {
				splitpages = true;
			} else if (strncmp(argv[arg],"-dis",4) == 0) {
				withdisplay = true;
			} else if (strncmp(argv[arg],"-nomaptoisolatin1",6) == 0) {
				maptoisolatin1 = false;
			} else if (strncmp(argv[arg],"-dontloadplugins",8) == 0) {
				loadplugins = false;
			} else if (strncmp(argv[arg],"-guimode",8) == 0) {
				closerObject.fromgui = true;
			} else if (strncmp(argv[arg],"-nc",3) == 0) {
				nocurves = true;
			} else if (strncmp(argv[arg],"-nq",3) == 0) {
				doquit = false;
			} else if (strncmp(argv[arg],"-nb",3) == 0) {
				nobindversion = true;
			} else if (strcmp(argv[arg],"-df") == 0) {
				if (checkNextArgument(argc,argv,arg,errstream)) {
					replacementfont = argv[arg+1];
					arg++;
				}			
			} else if (strcmp(argv[arg],"-f") == 0) {
				if (checkNextArgument(argc,argv,arg,errstream)) {
					drivername = cppstrdup(argv[arg+1]);
					arg++;
				}
			} else if (strcmp(argv[arg],"-pagesize") == 0) {
				if (checkNextArgument(argc,argv,arg,errstream)) {
					outputPageSize = argv[arg+1];
					arg++;
				}
			} else if (strcmp(argv[arg],"-fontmap") == 0) {
				if (checkNextArgument(argc,argv,arg,errstream)) {
					//fontmapfile = cppstrdup(argv[arg+1]);
					//drvbase::theFontMapper.readMappingTable(errstream,argv[arg+1]);
					explicitFontMapFile = argv[arg+1];
					arg++;
				}
			} else if (strcmp(argv[arg],"-psarg") == 0) {
				if (checkNextArgument(argc,argv,arg,errstream)) {
					psArgs.addarg(argv[arg+1]);
//					errstream<<psArgs;
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
				nameOfInputFile = cppstrdup(argv[arg]);
			} else if (nameOfOutputFile == 0) {
				nameOfOutputFile = cppstrdup(argv[arg]);
			} else {
				errstream << "more than two file arguments " << endl;
				usage(errstream);
				return 1;
			}
		}
		//     errstream << "argv[" << arg << "] = " << argv[arg] << endl;
		arg++;
	}
	if(loadplugins) {
		loadpstoeditplugins(argv[0],errstream); // load the driver plugins
	}
	if ( (pushinsPtr != 0) && (pushinsPtr != globalRp) ) {
		globalRp->mergeRegister(errstream,*pushinsPtr,"push-ins");
	}
	if (showhelp) {
		usage(errstream);
		const char * gstocall = whichPI(errstream,verbose);
		if (gstocall != 0) {
			errstream << "Default interpreter is " << gstocall << endl;
		}
		globalRp->explainformats(errstream);
		return 1;
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
		const DriverDescription *    currentDriverDesc = globalRp->getdriverdesc(drivername);
		if (currentDriverDesc == 0) {
			errstream << "unsupported driver " << drivername << endl;
			globalRp->explainformats(errstream);
			return 1;
		} else if (strcmp(drivername,"gs") == 0) {
// TODO:
			// Check for input file (exists, or stdin) stdout handling
			if (!nameOfInputFile) {
				errstream << "cannot read from standard input if GS drivers are selected" << endl;
				return 1;
			}
			// an input file was given as argument

			// just test whether InputFile is readable.
			// The file will be read directly from the PostScript
			// interpreter later on
			convertBackSlashes(nameOfInputFile);

			if (!fileExists(nameOfInputFile)) {
					errstream << "Could not open file " << nameOfInputFile << " for input" << endl;
					return 1;
			}
		
			if(!nameOfOutputFile) {
				errstream << "cannot write to from standard output if GS drivers are selected" << endl;
				return 1;
			}
			// special handling of direct ghostscript drivers
			Argv commandline;
//			char commandline[1000];
			// TODO check for overflow
//			commandline[0]= '\0';
			const char * gstocall = whichPI(errstream,verbose);
			if (gstocall == 0) {
				return 3;
			}
			commandline.addarg(gstocall);
// rcw2: Current RiscOS port of gs can't find its fonts without this...
#ifdef riscos
			commandline.addarg("-I<GhostScript$Dir>");
#endif
			if (!verbose) commandline.addarg("-q");

			const char * pioptions = defaultPIoptions(errstream,verbose);
			if (pioptions && (strlen(pioptions) > 0)) {
				commandline.addarg(pioptions);
			}
			commandline.addarg("-dNOPAUSE");
			commandline.addarg("-dBATCH");
			char tempbuffer[1000];
			tempbuffer[0] = '\0';
			strcat(tempbuffer,"-sDEVICE=");
			strcat(tempbuffer,driveroptions); // e.g., pdfwrite ;
			commandline.addarg(tempbuffer);
			for (unsigned int psi = 0; psi < psArgs.argc; psi++) {
				commandline.addarg(psArgs.argv[psi]);
			}
			tempbuffer[0] = '\0';
			strcat(tempbuffer,"-sOutputFile=");
			strcat(tempbuffer,nameOfOutputFile);
			commandline.addarg(tempbuffer);
			commandline.addarg("-c");
			commandline.addarg("save");
			commandline.addarg("pop");
			commandline.addarg("-f");
			commandline.addarg(nameOfInputFile);
			if (verbose) errstream << "now calling the interpreter via: " << commandline << endl;
			// gsresult = system(commandline);
			const int gsresult = call_PI(commandline.argc,commandline.argv);
			errstream << "Interpreter finished. Return status " << gsresult << endl;
			return gsresult; 
		} else {
			//istream *inputFilePtr = 0;
			// cannot be const because nameOfInputFile cannot be const
			// because it needs to be changed on non UNIX systems (convertBackSlashes)
			const char stdinFileName[] = "%stdin" ;
			if (nameOfInputFile) {
				// an input file was given as argument

				// just test whether InputFile is readable.
				// The file will be read directly from the PostScript
				// interpreter later on
				convertBackSlashes(nameOfInputFile);
				if (!fileExists(nameOfInputFile)) {
					errstream << "Could not open file " << nameOfInputFile << " for input" << endl;
					return 1;
				}
			} else {
				nameOfInputFile = cppstrdup(stdinFileName);
			}
			ostream *outputFilePtr = 0;
			ofstream outFile;
			drvbase *outputdriver = 0;

			splitpages = splitpages || (!currentDriverDesc->backendSupportsMultiplePages);
			// if -split is given or if backend does not support multiple pages

			if(nameOfOutputFile) {
				outputFilePtr = &outFile;
				convertBackSlashes(nameOfOutputFile);
				char newname[2000] ;
				sprintf(newname,nameOfOutputFile,1); //first page is page 1
				if (currentDriverDesc->backendFileOpenType != DriverDescription::noopen ){ 
					if (currentDriverDesc->backendFileOpenType == DriverDescription::binaryopen ) { 
#if (defined(unix) || defined(__unix__) || defined(_unix) || defined(__unix) || defined(__EMX__) ) && !defined(DJGPP)
// binary is not available on UNIX, only on PC
						outFile.open(newname,ios::out);
#else
						// use redundant ios::out because of bug in djgpp
						outFile.open(newname,ios::out | ios::binary);
#endif
						// errstream << "opened " << newname << " for binary output" << endl;
					} else {
						outFile.open(newname);
						// errstream << "opened " << newname << " for output" << endl;
					}
					if (outFile.fail() ) {
						errstream << "Could not open file " << newname << " for output" << endl;
						return 1;
					} // fail
				} // backend opens file by itself
				outputdriver = currentDriverDesc->CreateBackend(driveroptions,*outputFilePtr,errstream,nameOfInputFile,newname,magnification,outputPageSize);
			} else {
				if (splitpages) {
					errstream << "Cannot split pages if output goes to standard output" << endl;
					return 1;
				} else  {
					outputFilePtr = &cout;
					outputdriver  = currentDriverDesc->CreateBackend(driveroptions,cout,errstream,nameOfInputFile,0,magnification,outputPageSize);
				}
			}

			if (outputdriver && (!outputdriver->driverOK()) ) {
				errstream << "Creation of driver failed " << endl;
				return 1;
			}
			outputdriver->verbose=verbose;
			if (explicitFontMapFile) {
				if (fileExists(explicitFontMapFile)) {
					if(verbose) {
							errstream << "loading fontmap from " << explicitFontMapFile << endl;
					}
					outputdriver->theFontMapper.readMappingTable(errstream,explicitFontMapFile);
				} else {
					errstream << "Warning: Fontmap file " << explicitFontMapFile << " not found. Option ignored." << endl;
				}
			} else {
				// look for a driver specific fontmap 
				// also look in the directory where the pstoedit .exe/dll was found
				char szExePath[1000];
				szExePath[0]= '\0';
				const int r = P_GetPathToMyself(argv[0],szExePath, sizeof(szExePath));
				if( r && verbose) {
					errstream << " path to myself: " << szExePath << endl;
				}	
				char * p;
				if ( r &&  (p = strrchr(szExePath,directoryDelimiter)) != 0) {
				   	*p = '\0';
					RSString test(szExePath);
#if (defined(unix) || defined(__unix__) || defined(_unix) || defined(__unix) || defined(__EMX__) ) && !defined(DJGPP)
					test+="/../lib/";
					test+=drivername;
					test+=".fmp";
#else
					char delim[] = {directoryDelimiter,'\0'};
					test+=delim;
					test+=drivername;
					test+=".fmp";
#endif 
					// errstream << test.value() << endl;
					if (fileExists(test.value())) {
						if(verbose) {
							errstream << "loading driver specific fontmap from " << test.value() << endl;
						}	
						outputdriver->theFontMapper.readMappingTable(errstream,test.value());
					}
				}
			}

			char * gsoutName = 0;
			const char * gsout;
			int gsresult = 0;
			if (backendonly) {
				gsout = nameOfInputFile;
				gsresult = 0;			    // gs was skipped, so there is no problem
			} else {
				char * gsin = full_qualified_tempnam("psin");
				const char * successstring; // string that indicated success of .pro
				ofstream inFileStream(gsin);
				inFileStream << "/pagetoextract " << pagetoextract << " def" << endl;
				if (!maptoisolatin1) {
					inFileStream << "/maptoisolatin1 false def" << endl;
				}
				if (verbose) {
					inFileStream << "/verbosemode true def" << endl;
				}
				if (useFlatness) {
					inFileStream << "/flatnesstouse " << flatness << " def" << endl;
				}
				if (drawtext) {
					inFileStream << "/textastext false def" << endl;
				}
				if (simulateClipping) {
					inFileStream << "/simulateclipping true def" << endl;
				}
				if ( strequal(drivername,"ps") || strequal(drivername,"psf") ) {
					inFileStream << "/escapetext true def" << endl;
				}
				if ( (strequal(drivername,"debug")) ||
				     (strequal(drivername,"pdf")) ) {
					inFileStream << "/usepdfmark true def" << endl;
				}

				inFileStream << "/replacementfont (" << replacementfont << ") def" << endl;

				gsout = gsoutName = full_qualified_tempnam("psout");
				assert((gsin != gsout) && ("You seem to have a buggy version of tempnam" != 0));
				// tempnam under older version of DJGPP are buggy
				// see search for BUGGYTEMPNAME in this file !! 
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

				if ( currentDriverDesc->backendSupportsImages ) {
					inFileStream << "/withimages true def" << endl;
				}
				if ( (!nocurves) && currentDriverDesc->backendSupportsCurveto ) {
					inFileStream << "/doflatten false def" << endl;
				} else {
					inFileStream << "/doflatten true def" << endl;
				}
				if (doquit) {
					inFileStream << "/pstoedit.quitprog { quit } def" << endl;
				} else {
					inFileStream << "/pstoedit.quitprog { } def" << endl;
				}
				if ( nobindversion ) {
					inFileStream << "/delaybindversion  false def" << endl;
				} else {
					inFileStream << "/delaybindversion  true def" << endl;
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
				if (strcmp(drivername,"ps2ai") == 0) {
					successstring = "%EOF"; // This is written by the ps2ai.ps 
										    // showpage in ps2ai does quit !!!
					// ps2ai needs special headers
					inFileStream <<
							"/pstoedit.preps2ai where not { \n"
								// first run through this file (before ps2ai.ps)
							"	/jout true def \n" 
							"	/joutput outputfilename def \n" ;
					if (driveroptions && (strcmp(driveroptions,"-88") == 0) ) {
						inFileStream << "	/jtxt3 false cdef\n" ;
					}
					inFileStream <<
							"	/textastext where { pop textastext not {/joutln true def } if } if \n"
							"	/pstoedit.preps2ai false def \n"
							"}{ \n"
							"	pop\n" // second run (after ps2ai.ps)
							"	inputfilename run \n"
						//	"	(\\" << successstring << "\\n) jp" 
						//	"	pstoedit.quitprog \n"
							"} ifelse \n" << endl;
					
				} else {
					successstring = "% normal end reached by pstoedit.pro";
#ifdef	USEPROLOGFROMFILE
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
				}
				inFileStream.close();
				// now call ghostscript

				Argv commandline;

				const char * gstocall = whichPI(errstream,verbose);
				if (gstocall == 0) {
					return 3;
				}
				commandline.addarg(gstocall);
// rcw2: Current RiscOS port of gs can't find its fonts without this...
#ifdef riscos
				commandline.addarg("-I<GhostScript$Dir>");
#endif
#if 0
				// now handled by the next block (pioptions)
				char * gsargs = getRegistryValue(errstream,"common","GS_LIB");
				if (gsargs) {
					char * inclDirective = new char[strlen(gsargs) + 3];
					strcpy(inclDirective,"-I");
					strcat(inclDirective,gsargs);
					commandline.addarg(inclDirective);
					delete [] inclDirective;
					delete [] gsargs;
				}
#endif
				const char * pioptions = defaultPIoptions(errstream,verbose);
				if (pioptions && (strlen(pioptions) > 0)) {
					commandline.addarg(pioptions);
				}
				if (!verbose) commandline.addarg("-q");
				if (nobindversion) {
			// NOBIND disables bind in, e.g, gs_init.ps
			// these files are loaded before pstoedit.pro
			// so any already bound call to, e.g., show could
			// not be intercepted by pstoedit's show

					commandline.addarg("-dNOBIND");
				} { 
					commandline.addarg("-dDELAYBIND");
				}
				commandline.addarg("-dWRITESYSTEMDICT");
				if (verbose) {
					commandline.addarg("-dESTACKPRINT");
				}
				if (withdisplay) {
					commandline.addarg("-dNOPAUSE");
				} else {
					commandline.addarg("-dNODISPLAY");
				}
//				if (psArgs!=0) commandline.addarg(psArgs);
				for (unsigned int psi = 0; psi < psArgs.argc; psi++) {
					commandline.addarg(psArgs.argv[psi]);
				}
				if (strcmp(drivername,"ps2ai") == 0) {
					// ps2ai needs special headers
					commandline.addarg(gsin); // the first time to set the paramters for ps2ai.ps
					commandline.addarg("ps2ai.ps");
					commandline.addarg(gsin); // again, but this time it'll run the conversion
				} else {
					commandline.addarg(gsin);
				}
				if (verbose) errstream << "now calling the interpreter via: " << commandline << endl;
				// gsresult = system(commandline);
				gsresult = call_PI(commandline.argc,commandline.argv);
				errstream << "Interpreter finished. Return status " << gsresult << endl;
				// ghostscript seems to return always 0, so
				// check whether the normal end was reached by pstoedit.pro
				remove(gsin);
				free(gsin);
				// if really returned !0 don't grep
				if (!gsresult) {
					if (verbose) errstream << "Now checking the temporary output" << endl;
					gsresult = grep(successstring,gsout,errstream);
				}
			}
			if (gsresult != 0) {
				errstream << "The interpreter seems to have failed, cannot proceed !" << endl;
				remove(gsout);
				free(gsoutName);
				return 1;
			} else {
				if ( outputdriver != 0 )  {

					if ( backendonly && (strcmp(nameOfInputFile,stdinFileName) == 0) ) {
						yyin = stdin;
					} else {
						yyin = fopen(gsout,"r");
						if (!yyin) {
							errstream << "Error opening file " << gsout << endl;
							return 1;
						}
					}
					if (verbose) errstream << "now postprocessing the interpreter output" << endl;
					{
						// local scope to force delete before delete of driver
						outputdriver->setdefaultFontName(replacementfont);
						outputdriver->simulateSubPaths = simulateSubPaths;
						PSFrontEnd fe(outFile,
								errstream,
								nameOfInputFile,
								nameOfOutputFile,
								magnification,
								outputPageSize,
								currentDriverDesc,
								driveroptions,
								splitpages, 
								outputdriver);
						fe.run(merge);
					} 
					if (verbose) errstream << "postprocessing the interpreter output finished" << endl;
// now delete is done in fe.run					delete outputdriver;
					if ( !( backendonly && (strcmp(nameOfInputFile,stdinFileName) == 0) ) ) {
						fclose(yyin);
					}
				} else {
					// outputdriver is 0
					// Debug or PostScript driver
					ifstream gsoutStream(gsout);
					if (verbose) errstream << "now copying  '" << gsout << "' to '" <<
						(nameOfOutputFile ? nameOfOutputFile:"standard output ") <<"' ";
					copy_file(gsoutStream,*outputFilePtr);
					if (verbose) errstream << " done \n";
				}
				if ( !backendonly ) {
					remove(gsout);
				}
			}
			free(gsoutName);
		}
	} // no backend specified
	delete [] drivername;
	delete [] nameOfInputFile;
	delete [] nameOfOutputFile;
	return 0;
}

extern int callgs(int argc, const char * const argv[]);
extern const char * whichPI(ostream & errstream,int verbose);


static bool versioncheckOK = false;

extern "C" DLLEXPORT  int  pstoedit_checkversion (unsigned int callersversion )
{
	versioncheckOK = (callersversion == pstoeditdllversion);
	return versioncheckOK;
}

void ignoreVersionCheck() { versioncheckOK = true; }

extern "C" DLLEXPORT 
int pstoeditwithghostscript(int argc,
							const char * const argv[],
							ostream& errstream,
							DescriptionRegister* const pushinsPtr)
{
	if (!versioncheckOK) {
		errorMessage("wrong version of pstoedit");
		return -1;
	}
	return pstoedit(argc,argv,errstream,callgs,whichPI,pushinsPtr);
}

//
// the following functions provide the interface for gsview
//
static const char * givenPI =0;
static const char * returngivenPI(ostream & errstream,int verbose)
{
	unused(&errstream);
	unused(&verbose);
	return givenPI;
}

extern "C" DLLEXPORT 
int pstoedit_plainC(int argc,
					const char * const argv[],
					const char * const psinterpreter)
{
	if (!versioncheckOK) {
		errorMessage("wrong version of pstoedit");
		return -1;
	}
	if (psinterpreter != 0) {
		givenPI=psinterpreter;
		return pstoedit(argc,argv,cerr,callgs,returngivenPI,0);
	} else {
		return pstoedit(argc,argv,cerr,callgs,whichPI,0);
	}
}

extern "C" DLLEXPORT DriverDescription_S *  getPstoeditDriverInfo_plainC() 
{ 
	if (!versioncheckOK) {
		errorMessage("wrong version of pstoedit");
		return 0;
	}
	loadpstoeditplugins("pstoedit",cerr);

	const int dCount = globalRp->nrOfDescriptions();
	/* use malloc to be compatible with C */
	DriverDescription_S * result = (DriverDescription_S*) malloc ((dCount+1)* sizeof(DriverDescription_S));
	DriverDescription_S * curR = result;
	const DriverDescription * const * dd = globalRp->rp;
	while(dd && (*dd) ) {
			const DriverDescription * currentDD = *dd;
			curR->symbolicname					= (char *) currentDD->symbolicname;
			curR->explanation					= (char *) currentDD->explanation;
			curR->suffix						= (char *) currentDD->suffix;
			curR->additionalInfo				= (char *) currentDD->additionalInfo;
			curR->backendSupportsSubPathes		= (int) currentDD->backendSupportsSubPathes;
			curR->backendSupportsCurveto		= (int) currentDD->backendSupportsCurveto;
			curR->backendSupportsMerging		= (int) currentDD->backendSupportsMerging; 
			curR->backendSupportsText			= (int) currentDD->backendSupportsText;
			curR->backendSupportsImages			= (int) currentDD->backendSupportsImages;
			curR->backendSupportsMultiplePages	= (int) currentDD->backendSupportsMultiplePages;
			curR++;
			dd++;
	}
	curR++->symbolicname = 0; // indicator for end
	
	return result;
}

#if defined(_WIN32)
extern void set_gs_write_callback(write_callback_type * new_cb); // defined in dwmain.c

extern "C" DLLEXPORT void setPstoeditOutputFunction(void * cbData,write_callback_type* cbFunction) 
{ 
	if (!versioncheckOK) {
		errorMessage("wrong version of pstoedit");
		return ;
	}
	set_gs_write_callback(cbFunction); // for the gswin.DLL
	static callbackBuffer cbBuffer(0,0); // default /dev/null 
	cbBuffer.set_callback(cbData,cbFunction);
#ifdef HAVESTL
	cerr.rdbuf(&cbBuffer);
#else
	cerr = &cbBuffer;
#endif
}
#endif
 
 
 
 
