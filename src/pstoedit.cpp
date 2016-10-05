/*
   pstoedit.cpp : This file is part of pstoedit
   main control procedure 

   Copyright (C) 1993 - 2001 Wolfgang Glunz, wglunz@pstoedit.net

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

#define WITHCLIPSUPPORT 1

#include "cppcomp.h"

#include I_fstream
#include I_stdio
#include I_stdlib
#include I_string_h

#include <assert.h>

//#if defined(_WIN32)
#if 0
#include "POptions.h"
#endif

#include "pstoedit.h"

// for the DLL export data types (the description struct)
#include "pstoedll.h"

#include "version.h"

#include "drvbase.h"

#include "dynload.h"

#define strequal(s1,s2) (strcmp(s1,s2) == 0)

#ifndef USEPROLOGFROMFILE
#include "pstoedit.ph"
#endif

#include "psfront.h"

extern const char *defaultPIoptions(ostream & errstream, int verbose);	// in callgs.cpp

static void writeFileName(ostream & outstream, const char *const filename)
{
	const unsigned int len = strlen(filename);
	for (unsigned int i = 0; i < len; i++) {
		if (filename[i] == '\\') {
			outstream << '/';	// '/' works on DOS also
		} else {
			outstream << filename[i];
		}
	}
}


static void usage(ostream & errstream)
{
	errstream <<
		"usage: pstoedit "
		"[-help] "
		"[-bo] "
		"[-df fontname] "
		"[-dt] "
		"[-adt] "
		"[-dis] "
		"[-flat nn] "
		"[-fontmap mapfile] "
		"[-gstest] "
		"[-include file] "
		"[-merge] "
		"[-pagesize pagesize(e.g. a4)] " "[-scale nn] " "[-nb] " "[-nc] " "[-nomaptoisolatin1] "
#if WITHCLIPSUPPORT
		"[-noclip] "
#endif
		"[-nq] "
		"[-nfr]"
		"[-page nn] "
		"[-psarg string] "
		"[-pti] "
		"[-pta] "
		"[-rgb] "
		"[-rotate angle] "
		"[-sclip] "
		"[-split] "
		"[-ssp] "
		"[-t2fontsast1] " "[-uchar char ] " "[-v] " "-f format [infile [outfile]]" << endl;
}


// cannot make this static, because it's used in a template, that might be
// stored in a repository (separate .o file)
static int checkNextArgument(int argc, const char *const argv[], int arg, ostream & errstream)
{
	// if there is no next arg, usage is printed and
	// the program is terminated
	if ((arg + 1 >= argc) || (argv[arg + 1] == 0)) {
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
static void getfvalue(int argc, const char *const argv[], int arg,
					  float &value, ostream & errstream)
{
	// get value for option arg (value is next argument)
	if (checkNextArgument(argc, argv, arg, errstream)) {

// Work around for Linux libg++ bug (use atof instead of strstream)
		value = (float) atof(argv[arg + 1]);
// C++'ish  istrstream strvalue(argv[arg+1]);
//      strvalue >> value;

	}
}

static void getivalue(int argc, const char *const argv[], int arg, int &value, ostream & errstream)
{
	// get value for option arg (value is next argument)
	if (checkNextArgument(argc, argv, arg, errstream)) {

// Work around for Linux libg++ bug (use atof instead of strstream)
		value = (int) atoi(argv[arg + 1]);
// C++'ish  istrstream strvalue(argv[arg+1]);
//      strvalue >> value;

	}
}

static int grep(const char *const matchstring, const char *const filename, ostream & errstream)
{
// for call: gsresult = grep("% normal end reached by pstoedit.pro",gsout);
	ifstream inFile;
#if defined (__GNUG__)
	inFile.open(filename);		// for some reasons adding flags does not work correctly with  g++
#else
	inFile.open(filename, ios::binary | ios::nocreate);
#endif

	if (inFile.fail()) {
		errstream << "Could not open file " << filename << " in grep" << endl;
		return 1;				// fail
	} else {
		const size_t matchlen = strlen(matchstring);
		const size_t bufferlen = matchlen + 1;
		// allocate buffer for reading begin of lines
		char *buffer = new char[bufferlen];


		while ((void) inFile.get(buffer, bufferlen, '\n'),
			   // debug            errstream << " read in grep :`" << buffer << "'" << inFile.gcount() << " " << matchlen << " eof:"<< inFile.eof() << endl,
			   !inFile.eof() /* && !inFile.fail() */ ) {
// debug                errstream << " read in grep :`" << buffer << "'" << inFile.gcount() << " " << matchlen << endl;
			// gcount returns int on MSVC 
			// make a temp variable to avoid a compiler warning on MSVC
			// (signed/unsigned comparison)
			const unsigned int inFile_gcountresult = inFile.gcount();
#ifdef HAVESTL
			// Notes regarding ANSI C++ version (from KB)
			// istream::get( char* pch, int nCount, char delim ) is different in three ways: 
			// When nothing is read, failbit is set.
			// An eos is always stored after characters extracted (this happens regardless of the outcome).
			// A value of -1 for nCount is an error.

			// If the line contains just a \n then the failbit
			// is set in the ANSI version

//
// Note: THis caused again problems with g++3.0 in non STL mode, so this is avoided now by never writing an empty line
//

			if (inFile_gcountresult == 0) {
				inFile.clear();
			} else
#endif

			if ((inFile_gcountresult == matchlen) && (strcmp(buffer, matchstring) == 0)) {
				delete[]buffer;
				return 0;
			}
			if (inFile.peek() == '\n')
				(void) inFile.ignore();
		}
		delete[]buffer;
	}
	return 1;					// fail
}

// *INDENT-OFF*
class drvNOBACKEND : public drvbase {	// not really needed - just as template argument
public:
	derivedConstructor(drvNOBACKEND);	// Constructor
	~drvNOBACKEND() {
	} // Destructor   
	
	bool withbackend() const {
		return false;
	}
private:						// yes these are private, library users should use the public interface
	// provided via drvbase
	void open_page() {
	}
	void close_page() {
	}
	void show_text(const TextInfo & /* textInfo */ ) {
	}
	void show_path() {
	}
	void show_rectangle(const float /* llx */ , const float /* lly */ , const float /* urx */ ,
						const float /* ury */ ) {
	}
};
// *INDENT-ON*

drvNOBACKEND::derivedConstructor(drvNOBACKEND):constructBase
{
}

static DriverDescriptionT < drvNOBACKEND > D_psf("psf", "Flattened PostScript (no curves)",
												 "fps", 1, 0, 1, 1, 1, false,
												 DriverDescription::normalopen, true, true,nodriverspecificoptions);
static DriverDescriptionT < drvNOBACKEND > D_ps("ps", "PostScript", "fps", 1, 1, 1, 1, 1,
												false, DriverDescription::normalopen, true, true,nodriverspecificoptions);
static DriverDescriptionT < drvNOBACKEND > D_debug("debug", "for test purposes", "dbg", 1, 1,
												   1, 1, 1, true, DriverDescription::normalopen,
												   true, true,nodriverspecificoptions);
static DriverDescriptionT < drvNOBACKEND > D_dump("dump", "for test purposes (same as debug)",
												  "dbg", 1, 1, 1, 1, 1, true,
												  DriverDescription::normalopen, true, true,nodriverspecificoptions);
static DriverDescriptionT < drvNOBACKEND > D_gs("gs",
												"any device that GhostScript provides - use gs:format, e.g. gs:pdfwrite",
												"gs", 1, 1, 1, 1, 1, false,
												DriverDescription::normalopen, true, true,nodriverspecificoptions);
static const OptionDescription driveroptionsAI[] = {
	OptionDescription("-88",0,"generate AI88 format"),
	endofoptions};
static DriverDescriptionT < drvNOBACKEND > D_ps2ai("ps2ai",
												   "Adobe Illustrator via ps2ai.ps of GhostScript",
												   "ai", 1, 1, 1, 1, 1, false,
												   DriverDescription::normalopen, false, true,
												   driveroptionsAI);

class Closer {
public:
	Closer():fromgui(false) {
	};
	~Closer() {
		if (fromgui) {
			cerr << "Program finished, please press CR to close window\n";
			(void) cin.get();
		}
	}
	bool fromgui;
};



static void loadpstoeditplugins(const char *progname, ostream & errstream)
{
	static bool pluginsloaded = false;
	if (pluginsloaded)
		return;
	RSString plugindir = getRegistryValue(errstream, "common", "plugindir");
	if (plugindir.value() && strlen(plugindir.value() )) {
		loadPlugInDrivers(plugindir.value(), errstream);	// load the driver plugins
		pluginsloaded = true;
	}
	// also look in the directory where the pstoedit .exe/dll was found
	char szExePath[1000];
	szExePath[0] = '\0';
	const unsigned long r = P_GetPathToMyself(progname, szExePath, sizeof(szExePath));
//  errstream << "r : " << r << " " << szExePath<< endl;
	char *p = 0;
	if (r && (p = strrchr(szExePath, directoryDelimiter)) != 0) {
		*p = '\0';
		if (strcmp(szExePath, plugindir.value() ? plugindir.value() : "") != 0) {
			loadPlugInDrivers(szExePath, errstream);
			pluginsloaded = true;
		}
	}

	// delete[]plugindir;
}


extern FILE *yyin;				// used by lexer 
						// This has to be declared here because of the extern "C"
						// otherwise we could declare it locally where it is used



extern "C" DLLEXPORT
	int pstoedit(int argc, const char *const argv[], ostream & errstream,
				 execute_interpreter_function call_PI,
				 whichPI_type whichPI, const DescriptionRegister * const pushinsPtr)
{

// Experimental
//  if (0) {
//      POptions options;
//      getOptionsViaDialog(options);
//      exit(1);
//  }


	Closer closerObject;

	// cannot be const  because it needs to be changed on non UNIX systems (convertBackSlashes)
	char *nameOfInputFile = 0;
	AutoDeleter < char >DeleterForNameOfInputFile(nameOfInputFile, true);
	char *nameOfOutputFile = 0;	// can contain %d for page splitting
	AutoDeleter < char >DeleterForNameOfOutputFile(nameOfOutputFile, true);
	//const char * psArgs = 0; // Pass through arguments to PostScript interpreter
	Argv psArgs;				// Pass through arguments to PostScript interpreter
	const char *nameOfIncludeFile = 0;	// name of an option include file
	const char *replacementfont = "Courier";
	// can be changed via -r
	bool maptoisolatin1 = true;
	bool withdisplay = false;
	bool doquit = true;
	bool nocurves = false;		// normally curves are shown as curves if backend supports
	bool nosubpathes = false;	// normally we use subpathes if the backend support them
	bool merge = false;
	bool drawtext = false;
	bool autodrawtext = false;
	unsigned int precisiontext = 0;
	bool splitpages = false;
	bool verbose = false;
	bool simulateSubPaths = false;
	const char *unmappablecharstring = 0;
	bool loadplugins = true;
	bool nobindversion = false;	// use old NOBIND instead of DELAYBIND
	int pagetoextract = 0;		// 0 stands for all pages
	float flatness = 1.0f;		// used for setflat
	bool useFlatness = false;
	bool simulateClipping = false;	// simulate clipping
	// most useful in combination with -dt
	bool useRGBcolors = false;
	bool dumpclippathifneeded = true;
	bool t2fontsast1 = false;	// handle T2 fonts (often come as embedded fonts in PDF files) same as T1
	bool keepinternalfiles = false;
	bool justgstest = false;
	bool pscover = false;
	bool nofontreplacement = false;
	int rotation = 0;
	const char *explicitFontMapFile = 0;
	RSString outputPageSize("");
	char *drivername = 0;
	AutoDeleter < char >DeleterFordrivername(drivername, true);

	float magnification = 1.0f;
	bool showhelp = false;
	bool dumphelp = false;

	bool backendonly = false;	// used for easier debugging of backends
	// directly read input file by backend
	// bypass ghostscript. The input file
	// is assumed to be produced by a
	// previous call with -f debug

	errstream << "pstoedit: version " << version << " / DLL interface " <<
		drvbaseVersion << " (build " << __DATE__ << ")" <<
		" : Copyright (C) 1993 - 2001 Wolfgang Glunz\n";
	int arg = 1;
	drvbase::verbose = false;	// init

//  obsolete now because of <driver>.fmp look-up
//  char * fontmapfile = getRegistryValue(errstream,"common","fontmap");
//  if (fontmapfile != 0) drvbase::theFontMapper.readMappingTable(errstream,fontmapfile);

	while (arg < argc) {
		if ((strlen(argv[arg]) > 1) && (argv[arg][0] == '-')) {
			// it is an option : Note - for itself is taken as filename
			if (strcmp(argv[arg], "-scale") == 0) {
				getfvalue(argc, argv, arg, magnification, errstream);
				arg++;
			} else if (strcmp(argv[arg], "-dumphelp") == 0) { // this is an internal option
				dumphelp = true;
			} else if (strcmp(argv[arg], "-help") == 0) {
				showhelp = true;
			} else if (strcmp(argv[arg], "-pscover") == 0) {
				pscover = true;
			} else if (strcmp(argv[arg], "-gstest") == 0) {
				justgstest = true;
			} else if (strcmp(argv[arg], "-page") == 0) {
				getivalue(argc, argv, arg, pagetoextract, errstream);
				arg++;
			} else if (strcmp(argv[arg], "-rotate") == 0) {
				getivalue(argc, argv, arg, rotation, errstream);
				arg++;
			} else if (strcmp(argv[arg], "-flat") == 0) {
				useFlatness = true;
				getfvalue(argc, argv, arg, flatness, errstream);
				arg++;
			} else if (strcmp(argv[arg], "-v") == 0) {
				verbose = true;
				drvbase::verbose = verbose;
			} else if (strcmp(argv[arg], "-ssp") == 0) {
				simulateSubPaths = true;
			} else if (strcmp(argv[arg], "-sclip") == 0) {
				simulateClipping = true;
			} else if (strcmp(argv[arg], "-rgb") == 0) {
				useRGBcolors = true;
			} else if (strcmp(argv[arg], "-noclip") == 0) {
				dumpclippathifneeded = false;
			} else if (strcmp(argv[arg], "-bo") == 0) {
				backendonly = true;
			} else if (strcmp(argv[arg], "-merge") == 0) {
				merge = true;
			} else if (strcmp(argv[arg], "-keep") == 0) {
				keepinternalfiles = true;
			} else if (strcmp(argv[arg], "-dt") == 0) {
				drawtext = true;
			} else if (strcmp(argv[arg], "-adt") == 0) {
				autodrawtext = true;
			} else if (strncmp(argv[arg], "-t2fontsast1", 3) == 0) {
				t2fontsast1 = true;
			} else if (strcmp(argv[arg], "-pta") == 0) {
				precisiontext = 2;
			} else if (strcmp(argv[arg], "-pti") == 0) {
				precisiontext = 1;
			} else if (strcmp(argv[arg], "-nfr") == 0) {
				nofontreplacement = true;
			} else if (strcmp(argv[arg], "-split") == 0) {
				splitpages = true;
			} else if (strcmp(argv[arg], "-dis") == 0) {
				withdisplay = true;
			} else if (strncmp(argv[arg], "-nomaptoisolatin1", 6) == 0) {
				maptoisolatin1 = false;
			} else if (strncmp(argv[arg], "-dontloadplugins", 8) == 0) {
				loadplugins = false;
			} else if (strncmp(argv[arg], "-guimode", 8) == 0) {
				closerObject.fromgui = true;
			} else if (strcmp(argv[arg], "-nc") == 0) {
				nocurves = true;
			} else if (strcmp(argv[arg], "-nsp") == 0) {
				nosubpathes = true;
			} else if (strcmp(argv[arg], "-nq") == 0) {
				doquit = false;
			} else if (strcmp(argv[arg], "-nb") == 0) {
				nobindversion = true;
			} else if (strcmp(argv[arg], "-uchar") == 0) {
				if (checkNextArgument(argc, argv, arg, errstream)) {
					unmappablecharstring = argv[arg + 1];
					arg++;
				}
			} else if (strcmp(argv[arg], "-df") == 0) {
				if (checkNextArgument(argc, argv, arg, errstream)) {
					replacementfont = argv[arg + 1];
					arg++;
				}
			} else if (strcmp(argv[arg], "-f") == 0) {
				if (checkNextArgument(argc, argv, arg, errstream)) {
					drivername = cppstrdup(argv[arg + 1]);
					arg++;
				}
			} else if (strcmp(argv[arg], "-pagesize") == 0) {
				if (checkNextArgument(argc, argv, arg, errstream)) {
					outputPageSize = argv[arg + 1];
					arg++;
				}
			} else if (strcmp(argv[arg], "-fontmap") == 0) {
				if (checkNextArgument(argc, argv, arg, errstream)) {
					//fontmapfile = cppstrdup(argv[arg+1]);
					//drvbase::theFontMapper.readMappingTable(errstream,argv[arg+1]);
					explicitFontMapFile = argv[arg + 1];
					arg++;
				}
			} else if (strcmp(argv[arg], "-psarg") == 0) {
				if (checkNextArgument(argc, argv, arg, errstream)) {
					psArgs.addarg(argv[arg + 1]);
//                  errstream<<psArgs;
					arg++;
				}
			} else if (strcmp(argv[arg], "-include") == 0) {
				if (checkNextArgument(argc, argv, arg, errstream)) {
					nameOfIncludeFile = argv[arg + 1];
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
	//
	// if input or output filename is -, then set it back to 0 in order
	// to keep the logic below unchanged
	//
	if (nameOfInputFile && strequal(nameOfInputFile, "-")) {
		delete[]nameOfInputFile;
		nameOfInputFile = 0;
	}
	if (nameOfOutputFile && strequal(nameOfOutputFile, "-")) {
		delete[]nameOfOutputFile;
		nameOfOutputFile = 0;
	}

	if (loadplugins) {
		loadpstoeditplugins(argv[0], errstream);	// load the driver plugins
	}

	if ((pushinsPtr != 0) && (pushinsPtr != getglobalRp())) {
		getglobalRp()->mergeRegister(errstream, *pushinsPtr, "push-ins");
	}

	if (showhelp) {
		usage(cout);
		const char *gstocall = whichPI(cout, verbose);
		if (gstocall != 0) {
			cout << "Default interpreter is " << gstocall << endl;
		}
		getglobalRp()->explainformats(cout);
		return 1;
	}
	if (dumphelp) {
		getglobalRp()->explainformats(cout,true);
		return 1;
	}
	if (justgstest) {
		const char *gstocall = whichPI(errstream, verbose);
		if (gstocall == 0) {
			return 3;
		}
		Argv commandline;
		commandline.addarg(gstocall);
// rcw2: Current RiscOS port of gs can't find its fonts without this...
#ifdef riscos
		commandline.addarg("-I<GhostScript$Dir>");
#endif
		const char *pioptions = defaultPIoptions(errstream, verbose);
		if (pioptions && (strlen(pioptions) > 0)) {
			commandline.addarg(pioptions);
		}

		if (!verbose)
			commandline.addarg("-q");

		if (nobindversion) {
			// NOBIND disables bind in, e.g, gs_init.ps
			// these files are loaded before pstoedit.pro
			// so any already bound call to, e.g., show could
			// not be intercepted by pstoedit's show

			commandline.addarg("-dNOBIND");
		} else {
			commandline.addarg("-dDELAYBIND");
		}
		commandline.addarg("-dWRITESYSTEMDICT");


		if (verbose) {
			commandline.addarg("-dESTACKPRINT");
			// commandline.addarg("-sDEBUG=true");
		}
		if (withdisplay) {
			commandline.addarg("-dNOPAUSE");
		} else {
			commandline.addarg("-dNODISPLAY");
		}

		for (unsigned int psi = 0; psi < psArgs.argc; psi++) {
			commandline.addarg(psArgs.argv[psi]);
		}
		if (nameOfInputFile) {
			commandline.addarg(nameOfInputFile);
		}
		if (!verbose)
			commandline.addarg("quit.ps");
		//if (verbose)
		errstream << "now calling the interpreter via: " << commandline << endl;
		// gsresult = system(commandline);
		const int gsresult = call_PI(commandline.argc, commandline.argv);
		errstream << "Interpreter finished. Return status " << gsresult << endl;

		return gsresult;
	}

	if (drivername == 0) {
		errstream << "No backend specified" << endl;
		usage(errstream);
		return 1;
	} else {
		char *driveroptions = strchr(drivername, ':');
		if (driveroptions) {
			*driveroptions = '\0';	// replace : with 0 to separate drivername
			driveroptions++;
		}
		const DriverDescription *currentDriverDesc = getglobalRp()->getdriverdesc(drivername);
		if (currentDriverDesc == 0) {
			errstream << "Unsupported driver " << drivername << endl;
			getglobalRp()->explainformats(errstream);
			return 1;
		}

		if ( currentDriverDesc->backendFileOpenType!=DriverDescription::normalopen && !nameOfOutputFile ) {
			errstream << "This driver cannot write to standard output because it writes binary data" << endl;
			return 1;
		}


		if (driveroptions && (strcmp(driveroptions, "-help") == 0)) {
			errstream << "This driver supports the following additional options: (specify using -f \"format:-option1 -option2\")" << endl;
			const OptionDescription * iter = currentDriverDesc->optionDescription;
			while (iter && (iter->Name != 0)) {
				errstream << iter->Name;
				if (iter->Parameter!=0) errstream << " " << iter->Parameter;
				if (iter->Description!=0) errstream << " //" << iter->Description;
				errstream << endl;
				iter++;
			}
			return 1;
		}
		if (strcmp(drivername, "gs") == 0) {
// TODO:
			// Check for input file (exists, or stdin) stdout handling
			if (!nameOfInputFile) {
				errstream << "Cannot read from standard input if GS drivers are selected" << endl;
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

			if (!nameOfOutputFile) {
				errstream <<
					"Cannot write to standard output if GS drivers are selected" << endl;
				return 1;
			}

			if (backendonly) {
				errstream << "The -bo option cannot be used if GS drivers are selected " << endl;
				return 1;
			}

			if (!driveroptions) {
				errstream <<
					"The gs output driver needs a gs-device as argument, e.g. gs:pdfwrite" << endl;
				return 1;
			}
			// special handling of direct ghostscript drivers
			Argv commandline;
//          char commandline[1000];
			// TODO check for overflow
//          commandline[0]= '\0';
			const char *gstocall = whichPI(errstream, verbose);
			if (gstocall == 0) {
				return 3;
			}
			commandline.addarg(gstocall);
// rcw2: Current RiscOS port of gs can't find its fonts without this...
#ifdef riscos
			commandline.addarg("-I<GhostScript$Dir>");
#endif
			if (!verbose)
				commandline.addarg("-q");

			const char *pioptions = defaultPIoptions(errstream, verbose);
			if (pioptions && (strlen(pioptions) > 0)) {
				commandline.addarg(pioptions);
			}
			commandline.addarg("-dNOPAUSE");
			commandline.addarg("-dBATCH");
			char tempbuffer[1000];
			tempbuffer[0] = '\0';
			strcat(tempbuffer, "-sDEVICE=");
			strcat(tempbuffer, driveroptions);	// e.g., pdfwrite ;
			commandline.addarg(tempbuffer);
			for (unsigned int psi = 0; psi < psArgs.argc; psi++) {
				commandline.addarg(psArgs.argv[psi]);
			}
			tempbuffer[0] = '\0';
			strcat(tempbuffer, "-sOutputFile=");
			strcat(tempbuffer, nameOfOutputFile);
			commandline.addarg(tempbuffer);
			commandline.addarg("-c");
			commandline.addarg("save");
			commandline.addarg("pop");
			commandline.addarg("-f");
			commandline.addarg(nameOfInputFile);
			if (verbose)
				errstream << "Now calling the interpreter via: " << commandline << endl;
			// gsresult = system(commandline);
			const int gsresult = call_PI(commandline.argc, commandline.argv);
			errstream << "Interpreter finished. Return status " << gsresult << endl;
			return gsresult;
		} else {
			//istream *inputFilePtr = 0;
			// cannot be const because nameOfInputFile cannot be const
			// because it needs to be changed on non UNIX systems (convertBackSlashes)
			const char stdinFileName[] = "%stdin";
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
			char *nameOfOutputFilewithoutpercentD = 0;

			{
				// setup pstoeditHome directory
				char szExePath[1000];
				szExePath[0] = '\0';
				const int r = P_GetPathToMyself(argv[0], szExePath, sizeof(szExePath));
				if (r && verbose) {
					errstream << " path to myself: " << szExePath << endl;
				}
				char *p;
				if (r && (p = strrchr(szExePath, directoryDelimiter)) != 0) {
					*p = '\0';
					drvbase::pstoeditHomeDir() = RSString(szExePath);
				} else {
					drvbase::pstoeditHomeDir() = "";
				}
				if (verbose)  errstream << "pstoedit home directory : " << drvbase::pstoeditHomeDir().value() << endl;

				drvbase::pstoeditDataDir() = drvbase::pstoeditHomeDir();

#if (defined(unix) || defined(__unix__) || defined(_unix) || defined(__unix) || defined (NetBSD) ) && !defined(DJGPP)
#ifdef PSTOEDITDATADIR
		// usually something like /usr/share/pstoedit
				drvbase::pstoeditDataDir() = PSTOEDITDATADIR;
#else
				drvbase::pstoeditDataDir() +=  "/../share/pstoedit";
#endif
#endif

				if (verbose)  errstream << "pstoedit data directory : " << drvbase::pstoeditDataDir().value() << endl;
			}


			if (nameOfOutputFile) {
				splitpages = splitpages || (!currentDriverDesc->backendSupportsMultiplePages);
				// if -split is given or if backend does not support multiple pages
				// BUT, do this only if a real output file is given, not when writing to stdout.
				outputFilePtr = &outFile;
				convertBackSlashes(nameOfOutputFile);
				nameOfOutputFilewithoutpercentD = cppstrdup(nameOfOutputFile, 20);	// reserve 20 chars for page number
				sprintf(nameOfOutputFilewithoutpercentD, nameOfOutputFile, 1);	//first page is page 1
				if (currentDriverDesc->backendFileOpenType != DriverDescription::noopen) {
					if (currentDriverDesc->backendFileOpenType == DriverDescription::binaryopen) {
#if (defined(unix) || defined(__unix__) || defined(_unix) || defined(__unix) || defined(__EMX__) || defined (NetBSD)  ) && !defined(DJGPP)
// binary is not available on UNIX, only on PC
						outFile.open(nameOfOutputFilewithoutpercentD, ios::out);
#else
						// use redundant ios::out because of bug in djgpp
						outFile.open(nameOfOutputFilewithoutpercentD, ios::out | ios::binary);
#endif
						// errstream << "opened " << newname << " for binary output" << endl;
					} else {
						outFile.open(nameOfOutputFilewithoutpercentD);
						// errstream << "opened " << newname << " for output" << endl;
					}
					if (outFile.fail()) {
						errstream << "Could not open file " << nameOfOutputFilewithoutpercentD <<
							" for output" << endl;
						return 1;
					}			// fail
				}				// backend opens file by itself
				outputdriver =
					currentDriverDesc->CreateBackend(driveroptions,
													 *outputFilePtr,
													 errstream,
													 nameOfInputFile,
													 nameOfOutputFilewithoutpercentD, magnification,
													 outputPageSize);
			} else {
				if (splitpages) {
					errstream << "Cannot split pages if output goes to standard output" << endl;
					return 1;
				} else {
					outputFilePtr = &cout;
					outputdriver =
						currentDriverDesc->CreateBackend(driveroptions,
														 cout, errstream,
														 nameOfInputFile,
														 0, magnification, outputPageSize);
				}
			}

			if ((!outputdriver) || (outputdriver && (!outputdriver->driverOK()))) {
				errstream << "Creation of driver failed " << endl;
				return 1;
			}


			if (explicitFontMapFile) {
				if (fileExists(explicitFontMapFile)) {
					if (verbose) {
						errstream << "Loading fontmap from " << explicitFontMapFile << endl;
					}
					drvbase::theFontMapper().readMappingTable(errstream, explicitFontMapFile);
				} else {
					errstream << "Warning: Fontmap file " <<
						explicitFontMapFile << " not found. Option ignored." << endl;
				}
			} else {
				// look for a driver specific fontmap 
				// also look in the directory where the pstoedit .exe/dll was found

				/*
				   char szExePath[1000];
				   szExePath[0] = '\0';
				   const int r = P_GetPathToMyself(argv[0], szExePath,
				   sizeof(szExePath));
				   if (r && verbose) {
				   errstream << " path to myself: " << szExePath << endl;
				   }
				   char *p;
				 */
				if (drvbase::pstoeditDataDir() != "") {
					// *p = '\0';
					RSString test(drvbase::pstoeditDataDir());
					test += directoryDelimiter;
					test += drivername;
					test += ".fmp";
					// errstream << test.value() << endl;
					if (fileExists(test.value())) {
						if (verbose) {
							errstream <<
								"loading driver specific fontmap from " << test.value() << endl;
						}
						drvbase::theFontMapper().readMappingTable(errstream, test.value());
					}
				}
			}

			char *gsoutName = 0;
			const char *gsout = 0;
			int gsresult = 0;
			if (backendonly) {
				if ( outputdriver && !outputdriver->withbackend() ) {
					errstream << "the -bo option cannot be used with this non native pstoedit driver" << endl;
					return 1;
				}
				gsout = nameOfInputFile;
				gsresult = 0;	// gs was skipped, so there is no problem
			} else {
				char *gsin = full_qualified_tempnam("psin");
				const char *successstring;	// string that indicated success of .pro
				ofstream inFileStream(gsin);
				inFileStream << "/pagetoextract " << pagetoextract << " def" << endl;
				if (!maptoisolatin1) {
					inFileStream << "/maptoisolatin1 false def" << endl;
				}
				if (t2fontsast1) {
					inFileStream << "/t2fontsast1 true def" << endl;
				} else {
					inFileStream << "/t2fontsast1 false def" << endl;
				}
				if (useRGBcolors) {
					inFileStream << "/pstoedit.useRGBcolors true def" << endl;
				}
#if WITHCLIPSUPPORT
				if (currentDriverDesc->backendSupportsClipping && dumpclippathifneeded) {
					inFileStream << "/pstoedit.dumpclippath true def" << endl;
				}
#endif
				if (verbose) {
					inFileStream << "/verbosemode true def" << endl;
				}
				if (nofontreplacement) {
					inFileStream << "/pstoedit.checkfontnames false def " << endl;
				}
				if (pscover) {
					inFileStream << "/withpscover true def" << endl;
				}
				if (useFlatness) {
					inFileStream << "/flatnesstouse " << flatness << " def" << endl;
				}
				if (drawtext || (!currentDriverDesc->backendSupportsText)) {
					inFileStream << "/textastext false def" << endl;
				} else {
					inFileStream << "/textastext true def" << endl;
				}
				if (autodrawtext) {
					inFileStream << "/autodrawtext true def" << endl;
				}
				if (unmappablecharstring && strlen(unmappablecharstring)) {
					inFileStream << "/globalunmappablecharacter (" <<
						unmappablecharstring[0] << ") def" << endl;
				}
				inFileStream << "/precisiontext " << precisiontext << " def" << endl;

				if (simulateClipping) {
					inFileStream << "/simulateclipping true def" << endl;
				}
				inFileStream << "/pstoedit.rotation " << rotation << " def" << endl;
				if (strequal(drivername, "ps")
					|| strequal(drivername, "psf")) {
					inFileStream << "/escapetext true def" << endl;
				}
				if ((strequal(drivername, "debug")) || (strequal(drivername, "pdf"))) {
					inFileStream << "/usepdfmark true def" << endl;
				}

				inFileStream << "/replacementfont (" << replacementfont << ") def" << endl;

				gsout = gsoutName = full_qualified_tempnam("psout");
				assert((gsin != gsout)
					   && ("You seem to have a buggy version of tempnam" != 0));
				// tempnam under older version of DJGPP are buggy
				// see search for BUGGYTEMPNAME in this file !! 
				if (nameOfOutputFile) {
					inFileStream << "/pstoedit.targetoutputfile (";
					writeFileName(inFileStream, nameOfOutputFile);
					inFileStream << ") def" << endl;
					inFileStream << "/pstoedit.nameOfOutputFilewithoutpercentD (";
					writeFileName(inFileStream, nameOfOutputFilewithoutpercentD);
					inFileStream << ") def" << endl;
				} else {
					inFileStream << "/redirectstdout true def" << endl;
					inFileStream << "/pstoedit.targetoutputfile (";
					writeFileName(inFileStream, "stdout");
					inFileStream << ") def" << endl;
				}
				inFileStream << "/outputfilename (";
				writeFileName(inFileStream, gsout);
				inFileStream << ") def" << endl;

				inFileStream << "/inputfilename  (";
				writeFileName(inFileStream, nameOfInputFile);
				inFileStream << ") def" << endl;

				if (nameOfIncludeFile) {
					ifstream filetest(nameOfIncludeFile);
					if (!filetest) {
						errstream << "Could not open file " <<
							nameOfIncludeFile << " for inclusion" << endl;
						return 1;
					}
					filetest.close();
					inFileStream << "/nameOfIncludeFile  (";
					writeFileName(inFileStream, nameOfIncludeFile);
					inFileStream << ") def" << endl;
				}

				if (currentDriverDesc->backendSupportsImages) {
					inFileStream << "/withimages true def" << endl;
				}
				if (currentDriverDesc->backendSupportsPNGFileImages) {
					if (!nameOfOutputFile) {
						errstream <<
							"Warning: some types of raster images in the input file cannot be converted if the output is sent to standard output"
							<< endl;
					} else {
						inFileStream << "/backendSupportsPNGFileImages true def" << endl;
					}
				}
				if ((!nocurves)
					&& currentDriverDesc->backendSupportsCurveto) {
					inFileStream << "/doflatten false def" << endl;
				} else {
					inFileStream << "/doflatten true def" << endl;
				}
				if (doquit) {
					inFileStream << "/pstoedit.quitprog { quit } def" << endl;
				} else {
					inFileStream << "/pstoedit.quitprog { } def" << endl;
				}
				if (nobindversion) {
					inFileStream << "/delaybindversion  false def" << endl;
				} else {
					inFileStream << "/delaybindversion  true def" << endl;
				}
				delete[]nameOfOutputFilewithoutpercentD;
				if (outputdriver && (outputdriver->knownFontNames() != 0)) {
					const char *const *fnames = outputdriver->knownFontNames();
					unsigned int size = 0;
					while (*fnames) {
						size++;
						fnames++;
					}
					inFileStream << "/pstoedit.knownFontNames " << size << " dict def" << endl;

					inFileStream << "pstoedit.knownFontNames begin" << endl;
					fnames = outputdriver->knownFontNames();
					while (*fnames) {
						inFileStream << "/" << *fnames << " true def" << endl;
						fnames++;
					}
					inFileStream << "end" << endl;
				}
				if (strcmp(drivername, "ps2ai") == 0) {
					successstring = "%EOF";	// This is written by the ps2ai.ps 
					// showpage in ps2ai does quit !!!
					// ps2ai needs special headers
					inFileStream << "/pstoedit.preps2ai where not { \n"
						// first run through this file (before ps2ai.ps)
						"	/jout true def \n" "	/joutput outputfilename def \n";
					if (driveroptions && (strcmp(driveroptions, "-88") == 0)) {
						inFileStream << "	/jtxt3 false cdef\n";
					}
					inFileStream << "	/textastext where { pop textastext not {/joutln true def } if } if \n" "	/pstoedit.preps2ai false def \n" "}{ \n" "	pop\n"	// second run (after ps2ai.ps)
						"	inputfilename run \n"
						//  "   (\\" << successstring << "\\n) jp" 
						//  "   pstoedit.quitprog \n"
						"} ifelse \n" << endl;

				} else {
					successstring = "% normal end reached by pstoedit.pro";
#ifdef	USEPROLOGFROMFILE
					ifstream prologue("pstoedit.pro");
					//     errstream << " copying prologue file to " << gsin << endl;
					copy_file(prologue, inFileStream);
#else
					const char *const *prologueline = PS_prologue;
					while (prologueline && *prologueline) {
						inFileStream << *prologueline << '\n';
						prologueline++;
					}
#endif
				}
				inFileStream.close();
				// now call ghostscript

				Argv commandline;

				const char *gstocall = whichPI(errstream, verbose);
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
				char *gsargs = getRegistryValue(errstream, "common", "GS_LIB");
				if (gsargs) {
					char *inclDirective = new char[strlen(gsargs) + 3];
					strcpy(inclDirective, "-I");
					strcat(inclDirective, gsargs);
					commandline.addarg(inclDirective);
					delete[]inclDirective;
					delete[]gsargs;
				}
#endif
				const char *pioptions = defaultPIoptions(errstream, verbose);
				if (pioptions && (strlen(pioptions) > 0)) {
					commandline.addarg(pioptions);
				}
				if (!verbose)
					commandline.addarg("-q");
				if (strcmp(drivername, "ps2ai") != 0) {	// not for ps2ai
					if (nobindversion) {
						// NOBIND disables bind in, e.g, gs_init.ps
						// these files are loaded before pstoedit.pro
						// so any already bound call to, e.g., show could
						// not be intercepted by pstoedit's show

						commandline.addarg("-dNOBIND");
					} else {
						commandline.addarg("-dDELAYBIND");
					}
					commandline.addarg("-dWRITESYSTEMDICT");
				}

				if (verbose) {
					commandline.addarg("-dESTACKPRINT");
				}
				if (withdisplay) {
					commandline.addarg("-dNOPAUSE");
				} else {
					commandline.addarg("-dNODISPLAY");
				}
				for (unsigned int psi = 0; psi < psArgs.argc; psi++) {
					commandline.addarg(psArgs.argv[psi]);
				}
				if (strcmp(drivername, "ps2ai") == 0) {
					// ps2ai needs special headers
					commandline.addarg(gsin);	// the first time to set the paramters for ps2ai.ps
					commandline.addarg("ps2ai.ps");
					commandline.addarg(gsin);	// again, but this time it'll run the conversion
				} else {
					commandline.addarg(gsin);
				}
				if (verbose)
					errstream << "now calling the interpreter via: " << commandline << endl;
				// gsresult = system(commandline);
				gsresult = call_PI(commandline.argc, commandline.argv);
				errstream << "Interpreter finished. Return status " << gsresult << endl;
				// ghostscript seems to return always 0, so
				// check whether the normal end was reached by pstoedit.pro
				if (!keepinternalfiles)
					(void) remove(gsin);
				free(gsin);
				// if really returned !0 don't grep
				if (!gsresult) {
					if (verbose)
						errstream << "Now checking the temporary output" << endl;
					gsresult = grep(successstring, gsout, errstream);
				}
			}
			if (gsresult != 0) {
				errstream << "The interpreter seems to have failed, cannot proceed !" << endl;
				if (!keepinternalfiles)
					(void) remove(gsout);
				free(gsoutName);
				return 1;
			} else {
				if (outputdriver->withbackend()) {

					if (backendonly && (strcmp(nameOfInputFile, stdinFileName) == 0)) {
						// we need to copy stdin because we need to scan it twice
						// the first time for the boundingboxes and than a second time for the
						// real processing.
						gsout = full_qualified_tempnam("pssi");
						if (verbose) {
							errstream << "copying stdin to " << gsout << endl;
						}
						ofstream copyofstdin(gsout);
						copy_file(cin, copyofstdin);
					}

					yyin = fopen(gsout, "rb");	// ios::binary | ios::nocreate
					if (!yyin) {
						errstream << "Error opening file " << gsout << endl;
						return 1;
					}

					{
						// local scope to force delete before delete of driver
						outputdriver->setdefaultFontName(replacementfont);
						//      if (nosubpathes) ((DriverDescription*) outputdriver->Pdriverdesc)->backendSupportsSubPathes=false;
						outputdriver->simulateSubPaths = simulateSubPaths;
						PSFrontEnd fe(outFile,
									  errstream,
									  nameOfInputFile,
									  nameOfOutputFile,
									  magnification,
									  outputPageSize,
									  currentDriverDesc, driveroptions, splitpages, outputdriver);
						if (verbose)
							errstream << "now reading BoundingBoxes" << endl;
						/* outputdriver-> */ drvbase::totalNumberOfPages =
							fe.readBBoxes( /* outputdriver-> */ drvbase::bboxes());
						if (verbose) {
							errstream << " got " <<	/* outputdriver-> */
								drvbase::totalNumberOfPages << " page(s)" << endl;
							for (unsigned int i = 0;
								 i < /* outputdriver-> */ drvbase::totalNumberOfPages; i++) {
								errstream << /* outputdriver-> */ drvbase::bboxes()[i].
									ll << " " << /* outputdriver-> */ drvbase::bboxes()[i].ur << endl;
							}
						}
						fclose(yyin);
						yyin = fopen(gsout, "rb");
						if (verbose)
							errstream << "now postprocessing the interpreter output" << endl;
						fe.run(merge);
					}
					if (verbose)
						errstream << "postprocessing the interpreter output finished" << endl;
// now delete is done in fe.run                 delete outputdriver;

					// now we can close it in any case - since we took a copy
					fclose(yyin);
					if (backendonly && (strcmp(nameOfInputFile, stdinFileName) == 0)) {
						(void) remove(gsout);
					}
				} else {
					// outputdriver has no backend
					// Debug or PostScript driver
					ifstream gsoutStream(gsout);
					if (verbose)
						errstream << "now copying  '" << gsout << "' to '"
							<< (nameOfOutputFile ? nameOfOutputFile : "standard output ") << "' ";
					copy_file(gsoutStream, *outputFilePtr);
					if (verbose)
						errstream << " done \n";
					delete outputdriver;
				}
				if (!backendonly) {
					if (!keepinternalfiles)
						(void) remove(gsout);
				}
			}
			free(gsoutName);
		}
	}							// no backend specified
//  delete [] drivername;
//  delete [] nameOfInputFile;
//  delete [] nameOfOutputFile;
	return 0;
}

extern int callgs(int argc, const char *const argv[]);
extern const char *whichPI(ostream & errstream, int verbose);

static bool versioncheckOK = false;

extern "C" DLLEXPORT int pstoedit_checkversion(unsigned int callersversion)
{
	versioncheckOK = (callersversion == pstoeditdllversion) ||
		((callersversion == 300) && (pstoeditdllversion == 301));
	return versioncheckOK;
}

void ignoreVersionCheck()
{
	versioncheckOK = true;
}

extern "C" DLLEXPORT
	int pstoeditwithghostscript(int argc,
								const char *const argv[],
								ostream & errstream, const DescriptionRegister * const pushinsPtr)
{
	if (!versioncheckOK) {
		errorMessage("wrong version of pstoedit");
		return -1;
	}
	return pstoedit(argc, argv, errstream, callgs, whichPI, pushinsPtr);
}

//
// the following functions provide the interface for gsview
//
static const char *givenPI = 0;
static const char *returngivenPI(ostream & errstream, int verbose)
{
	unused(&errstream);
	unused(&verbose);
	return givenPI;
}

extern "C" DLLEXPORT
	int pstoedit_plainC(int argc, const char *const argv[], const char *const psinterpreter)
{
	if (!versioncheckOK) {
		errorMessage("wrong version of pstoedit");
		return -1;
	}
	if (psinterpreter != 0) {
		givenPI = psinterpreter;
		return pstoedit(argc, argv, cerr, callgs, returngivenPI, 0);
	} else {
		return pstoedit(argc, argv, cerr, callgs, whichPI, 0);
	}
}

extern "C" DLLEXPORT DriverDescription_S * getPstoeditDriverInfo_plainC()
{
	if (!versioncheckOK) {
		errorMessage("wrong version of pstoedit");
		return 0;
	}
	loadpstoeditplugins("pstoedit", cerr);

	const int dCount = getglobalRp()->nrOfDescriptions();
	/* use malloc to be compatible with C */
	DriverDescription_S *result =
		(DriverDescription_S *) malloc((dCount + 1) * sizeof(DriverDescription_S));
	DriverDescription_S *curR = result;
	const DriverDescription *const *dd = getglobalRp()->rp;
	while (dd && (*dd)) {
		const DriverDescription *currentDD = *dd;
		assert(currentDD);
		curR->symbolicname = (char *) currentDD->symbolicname;
		curR->explanation = (char *) currentDD->explanation;
		curR->suffix = (char *) currentDD->suffix;
		curR->additionalInfo = (char *) currentDD->additionalInfo;
		curR->backendSupportsSubPathes = (int) currentDD->backendSupportsSubPathes;
		curR->backendSupportsCurveto = (int) currentDD->backendSupportsCurveto;
		curR->backendSupportsMerging = (int) currentDD->backendSupportsMerging;
		curR->backendSupportsText = (int) currentDD->backendSupportsText;
		curR->backendSupportsImages = (int) currentDD->backendSupportsImages;
		curR->backendSupportsMultiplePages = (int) currentDD->backendSupportsMultiplePages;
		curR++;
		dd++;
	}
	curR++->symbolicname = 0;	// indicator for end

	return result;
}

//
// function to clear the memory allocated by the getPstoeditDriverInfo_plainC
// function. This avoids conflicts with different heaps.
//
extern "C" DLLEXPORT void clearPstoeditDriverInfo_plainC(DriverDescription_S * ptr)
{
	free(ptr);
	ptr = 0;
}

#if defined(_WIN32) || defined(__OS2__)
extern void set_gs_write_callback(write_callback_type * new_cb);	// defined in d[wp]mainc.c

extern "C" DLLEXPORT void setPstoeditOutputFunction(void *cbData, write_callback_type * cbFunction)
{
	if (!versioncheckOK) {
		errorMessage("wrong version of pstoedit");
		return;
	}
	set_gs_write_callback(cbFunction);	// for the gswin.DLL
	static callbackBuffer cbBuffer(0, 0);	// default /dev/null 
	(void) cbBuffer.set_callback(cbData, cbFunction);
#if defined(HAVESTL) || defined(__OS2__)
	cerr.rdbuf(&cbBuffer);
#else
	cerr = &cbBuffer;
#endif
}
#endif
 
