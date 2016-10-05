/* 
   callgs.cpp : This file is part of pstoedit
   interface to GhostScript

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

#include "cppcomp.h"

#include I_iostream
#include I_fstream

#include "pstoedit.h"

#include I_stdlib

#include I_string_h


// rcw2:Can't put angle brackets onto commandline for g++ on RiscOS :(
#ifdef riscos
  #ifndef DEFAULTGS
  #define DEFAULTGS <GhostScript$Dir>.ghost
  #endif
#endif

extern char * getRegistryValue(ostream& errstream, const char * typekey, const char * key);


 
char * createCmdLine(int argc, const char * const argv[])
{
	// create a single string from all args
	unsigned int sizeneeded = 0;
	{for (unsigned int i = 0; i < (unsigned) argc; i++) sizeneeded += strlen(argv[i]) + 2;}
	char * result = new char[sizeneeded +1];
	*result = '\0';
	{for (unsigned int i = 0; i < (unsigned) argc; i++) {
		strcat(result,argv[i]);
		strcat(result," ");
	}}
	return result;
}


#ifdef _WIN32

#define WITHDLLSUPPORT
#ifdef WITHDLLSUPPORT

extern char * cppstrdup(const char * src); // defined in drvbase.cpp

// using GhostScript DLL
#define main calldll
#define _Windows

#include "dwmainc.c"

#undef main

#define WITHGETINI
#ifdef WITHGETINI
#include "getini.c"
#endif

static int callgsDLL(int argc, const char * const argv[])
{
#if 0
//old	char * argv[100];
//old	int argc=0;
	// no need to set argv[0]: it is already set
	// from pstoedit (main) (value is the result of whichPI)
	

	// first quick hack, turn commandline into a vector
	char * commandline = cppstrdup(commandline_p);
	char * cp = commandline;
	argv[argc] = cp; argc++;
	while (*cp) {
		if (*cp == ' ') {
			*cp = '\0'; cp++;
			while ((*cp) && (*cp == ' ')) cp++;
			if (*cp) {
				argv[argc] = cp; argc++;
			} else {
				break; // while
			}
		}
		cp++;
	}

#endif

#ifdef PSTOEDITDEBUG
	cerr << "Commandline " << endl; // old << commandline << endl;
	for (int i= 0; i < argc ; i++) {
		cerr  << "argv["<< i << "]: " << argv[i] << endl;
	}
#endif

	szDllName = argv[0];
//	cerr << szDllName << endl;
	const int result = calldll(argc,(char **)argv);
//old 	delete [] commandline; 
	return result;
}

#endif
// with DLL SUPPORT

// without using GhostScript DLL
#include <windows.h>
#include I_stdio
static int callgsEXE(int argc, const char * const argv[])
{
	int gsresult = 0;
//cerr << "running-win " << commandline << endl;

	STARTUPINFO MyStartupInfo; // Prozessdaten
	memset(&MyStartupInfo,'\0',sizeof(MyStartupInfo));
	MyStartupInfo.cb = sizeof(STARTUPINFO);
	MyStartupInfo.wShowWindow=SW_SHOWMINNOACTIVE;

	PROCESS_INFORMATION MyProcessInformation;
         // wird von CreateProcess gefuellt
    DWORD gs_status = 0;

	char * commandline = createCmdLine(argc,argv);

	BOOL status = CreateProcess(
              NULL, // Application Name -> wird in der naechsten Zeile mitgegeben
              (LPSTR)commandline,
              NULL, // Prozessattribute (NULL == Default)
              NULL, // Thread-Atrribute (Default)
              FALSE, // InheritHandles
              CREATE_NEW_PROCESS_GROUP, // CreationFlags
              NULL, // Environment (NULL: same as calling-process)
              NULL, // Current Directory (NULL: same as calling process)
              (LPSTARTUPINFO)(&MyStartupInfo), // Windows-state at Startup
 				  // das Fenster der aufzurufenden Funktion wird verkleinert + nicht aktiv
				  // dargestellt
             (LPPROCESS_INFORMATION)(&MyProcessInformation)
            );

    delete [] commandline;
	if (status) gsresult=0;
    else        gsresult=-1; // Failure

    while (1) {
        status=GetExitCodeProcess(MyProcessInformation.hProcess, &gs_status);
		if ( !status ) {  // Proze¤-Status konnte nicht ermittelt werden
			gsresult=-1;
			break;  
		} else if(gs_status==STILL_ACTIVE) { // gswin arbeitet noch...
			Sleep(500); // Wartezeit in msec
		} else break; // Prozess beendet
	}
	if (gsresult != 0) {
		cerr << "Interpreter failure: " << gsresult << endl;
	}
	CloseHandle(MyProcessInformation.hProcess);
	CloseHandle(MyProcessInformation.hThread);
	return gsresult;
}

int callgs(int argc, const char * const argv[]) { 


	// check the first arg in the command line whether it contains gsdll32.dll
	if (strstr(argv[0],"gsdll32.dll") != NULL) {
#ifdef WITHDLLSUPPORT
		return callgsDLL(argc,argv);
#else
		cerr << "Sorry, but DLL support was not enabled in this version of pstoedit" << endl;
		return 2;
#endif
	} else 
	return callgsEXE(argc,argv);
}


#else
// not a windows system
int callgs(int argc, const char * const argv[]) { 
//	cerr << "running " << commandline << endl;
	char * commandline = createCmdLine(argc,argv);
	const int result = system(commandline);
	delete [] commandline;
	return result;
}
#endif

#define str(x) #x
#define xstr(x) str(x)

const char * whichPI(ostream & errstream,int verbose)
{
// determines which PostScript interpreter to use
#ifdef DEFAULTGS
	static const char * const defaultgs = xstr(DEFAULTGS);
#else
	static const char * const defaultgs = "";
#endif
	const char * gstocall = getenv("GS");
	if ( gstocall  == 0 ) {
		if (verbose) errstream<< "GS not set, trying registry for common/gstocall" << endl;
		// env var GS not set, so try first registry value and then default
		char * gstocallfromregistry = getRegistryValue(errstream,"common","gstocall");
		if (gstocallfromregistry != 0) {
			if (verbose) errstream<< "found value in registry" << endl;
			static char buffer[2000];
			strcpy(buffer,gstocallfromregistry);
			delete [] gstocallfromregistry;
			gstocall = buffer;
		} else {
#if defined(_WIN32) && defined(WITHDLLSUPPORT)
#if 0
			DWORD GetPrivateProfileString( 
				LPCTSTR lpAppName, // points to section name 
				LPCTSTR lpKeyName, // points to key name 
				LPCTSTR lpDefault,// points to default string 
				LPTSTR lpReturnedString, // points to destination buffer 
				DWORD nSize,// size of destination buffer 
				LPCTSTR lpFileName // points to initialization filename 
			); 
#endif
			if (verbose) errstream<< "didn't find value in registry, trying gsview32.ini" << endl;
			// try gsview32.ini
			static char pathname[1000]; // static, since we return it
			const char inifilename[] = "gsview32.ini";
#ifdef WITHGETINI
			char fullinifilename[1000];
		//	getini(fullinifilename,inifilename);
			getini(verbose,errstream,fullinifilename,inifilename,sizeof(fullinifilename));
#else
			const char * fullinifilename = inifilename;
#endif
			if (verbose) errstream<< "looking in " << fullinifilename << endl;
			DWORD result = GetPrivateProfileString("Options",
					"GhostscriptDLL",
					"", //default
					pathname,
					1000,
					fullinifilename);
			if ( result > 0 ) {
				if (verbose) {
					errstream<< "found value in ";
					if (strcmp(inifilename,fullinifilename) == 0) {
						char sysdir[2000];
						sysdir[0] = '\0';
						UINT ret = GetWindowsDirectory( sysdir,2000);
						if (ret) errstream << sysdir << '\\';
					} 
					errstream << fullinifilename<< endl;
				}
				gstocall = pathname;
			} else
#endif
			{
				if (verbose) errstream<< "nothing found so far, trying default " << endl;
				if ( strlen(defaultgs) > 0 ) {
					gstocall = defaultgs;
				} else {
					errstream << "Fatal: don't know which interpreter to call. "
						      << "Either setenv GS or compile again with -DDEFAULTGS=..." << endl;
					gstocall = 0;
				}
			}
		}
	} else {
		if (verbose) errstream<< "GS is set to:" << gstocall << endl;
	}
	if (verbose && gstocall) errstream<< "Value found is:" << gstocall << endl;
	return gstocall;
}

const char * defaultPIoptions(ostream & errstream,int verbose)
{
// returns default options to be passed to the Postscript Interpreter
#ifdef GS_LIB
	static const char * const defaultPIOptions = xstr(GS_LIB);
#else
	static const char * const defaultPIOptions = "";
#endif			
	static char buffer[2000];
	const char * PIOptions = getenv("GS_LIB");
	if ( PIOptions  == 0 ) {
		if (verbose) errstream<< "GS_LIB not set, trying registry for common/GS_LIB" << endl;
		// env var GS_LIB not set, so try first registry value and then default
		char * PIOptionsfromregistry = getRegistryValue(errstream,"common","GS_LIB");
		if (PIOptionsfromregistry != 0) {
			if (verbose) errstream<< "found value in registry" << endl;
			strcpy(buffer,PIOptionsfromregistry);
			delete [] PIOptionsfromregistry;
			PIOptions = buffer;
		} else {
#if defined(_WIN32) && defined(WITHDLLSUPPORT)
			if (verbose) errstream<< "didn't find value in registry, trying gsview32.ini" << endl;
			// try gsview32.ini
//			static char returnoptions[1000]; // static, since we return it
			
			const char inifilename[] = "gsview32.ini";
#ifdef WITHGETINI
			char fullinifilename[1000];
			getini(verbose,errstream,fullinifilename,inifilename,sizeof(fullinifilename));
#else
			const char * fullinifilename = inifilename;
#endif
			if (verbose) errstream<< "looking in " << fullinifilename << endl;
			DWORD result = GetPrivateProfileString("Options",
					"GhostscriptInclude",
					"", //default
					buffer,
					1000,
					fullinifilename);
			if ( result > 0 ) {
				if (verbose) {
					errstream<< "found value in ";
					if (strcmp(inifilename,fullinifilename) == 0) {
						char sysdir[2000];
						sysdir[0] = '\0';
						UINT ret = GetWindowsDirectory( sysdir,2000);
						if (ret) errstream << sysdir << '\\';
					} 
					errstream << fullinifilename<< endl;
				}
				PIOptions = buffer;
			} else
#endif
			{
				if (verbose) errstream<< "nothing found so far, trying default " << endl;
				if ( strlen(defaultPIOptions) > 0 ) {
					PIOptions = defaultPIOptions;
				} else {
//					errstream << "Fatal: don't know which interpreter to call. "
//						      << "Either setenv GS or compile again with -DDEFAULTGS=..." << endl;
					PIOptions = 0;
				}
			}
		}
	} else {
		if (verbose) errstream<< "GS_LIB is set to:" << PIOptions << endl;
	}

	if (PIOptions && (PIOptions[0] != '-') && (PIOptions[1] != 'I')) {
		static char returnbuffer[2000];
		strcpy(returnbuffer,"-I");
		strcat(returnbuffer,PIOptions);
		PIOptions = returnbuffer;
	}
	if (verbose && PIOptions) errstream<< "Value returned :" << PIOptions << endl;
	return PIOptions;
}

const char * whichPINoVerbose(ostream & errstream)
{
	return whichPI(errstream,0);
}
 
