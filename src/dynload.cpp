/*
   dynload.h : This file is part of pstoedit
   declarations for dynamic loading of drivers

   Copyright (C) 1993 - 2003 Wolfgang Glunz, wglunz@pstoedit.net

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


// we need __linux__ instead of just linux since the latter is not defined when -ansi is used.

#if defined(__sparc) || defined(__linux) || defined(__linux__) || defined(__FreeBSD__) || defined(_WIN32) || defined(__OS2__) || (HAVE_DLFCN_H==1)
#define HAVESHAREDLIBS
#endif

#ifdef HAVESHAREDLIBS

#ifndef LEANDYNLOAD
#include "drvbase.h"
#endif

#include "dynload.h"

#include I_stdio
#include I_stdlib
#include I_string_h
#include I_iostream

// for DriverDescription types.

typedef void (*initfunctype) ();
#if defined(__linux) || defined(__linux__) 
#include <dlfcn.h>
//  static const char * const libsuffix = ".so";
#elif defined(__FreeBSD__)
#include <dlfcn.h>
#elif defined(__hpux)
#include <dlfcn.h>
#elif defined(__sparc)
#if defined(__SVR4)
#include <dlfcn.h>
#else
				// prototypes under SunOS 4.1.x are not compatible with C++
extern "C" {
	void *dlopen(const char *pathname, int mode);
	 DynLoader::fptr dlsym(void *handle, const char *name);
	char *dlerror(void);
	int dlclose(void *handle);
}
#define RTLD_LAZY       1
#endif
// typedef void (*initfunctype) ();
#elif defined(__OS2__)
#include <sys/types.h>
#include <dlfcn.h>
// typedef void (*initfunctype) ();
#elif defined(_WIN32)
static char dlerror()
{
	return ' ';
}

#include <windows.h>
#define WINLOADLIB LoadLibrary
#define WINFREELIB FreeLibrary
	//  static const char * const libsuffix = ".dll";
#else
#error "system unsupported so far"
#endif

DynLoader::DynLoader(const char *libname_P, int verbose_p):libname(libname_P), handle(0), verbose(verbose_p)
{
	if (verbose) cerr << "dlopening " << libname << endl;
	if (libname)
		open(libname);
}

void DynLoader::open(const char *libname_P )
{
	if (handle) {
		cerr << "error: DynLoader has already opened a library" << endl;
		exit(1);
	}
	char *fulllibname = new char[strlen(libname_P) + 1];
	strcpy(fulllibname, libname_P);
	// not needed strcat(fulllibname_P,libsuffix);

#if defined(__linux) || defined(__linux__) 
	int loadmode = RTLD_LAZY;	// RTLD_NOW
	handle = dlopen(fulllibname, loadmode);
#elif defined(__FreeBSD__)
    	int loadmode = RTLD_LAZY;       // RTLD_NOW
   	 handle = dlopen(fulllibname, loadmode);
#elif defined(__hpux)
	int loadmode = RTLD_LAZY;	// RTLD_NOW
	handle = dlopen(fulllibname, loadmode);
#elif defined(__sparc)
	int loadmode = RTLD_LAZY;	// RTLD_NOW
	handle = dlopen(fulllibname, loadmode);
#elif defined(__OS2__)
	int loadmode = RTLD_LAZY;	// RTLD_NOW
	handle = dlopen(fulllibname, loadmode);
#elif defined(_WIN32)
	handle = WINLOADLIB(fulllibname);
#else
	system unsupported so far
#endif
	if (handle == 0) {
		cerr << "Problem during opening " << fulllibname << ":" << dlerror()
			<< endl;
		delete [] fulllibname;
		return;
	}
	{
	if (verbose) cerr << "loading dynamic library " << fulllibname << " completed successfully" << endl;
#if defined(__hpux) && defined (__GNUG__)
	// in HPUX g++ this is called "_GLOBAL__DI" - on other systems it may be something like "_init"
	// under HP-UX automatic initialization of dlopened shared libs doesn't work - at least not when compiled with g++
		initfunctype fptr = (initfunctype) dlsym(handle, "_GLOBAL__DI");
		if (fptr) {
			if (verbose) cerr << "Found an init function for the library, so execute it " << endl;
			fptr();
		}
#endif
	}
	delete[]fulllibname;
}

void DynLoader::close()
{
	if (handle) {
#if defined(__linux) || defined(__linux__) 
		dlclose(handle);
#elif defined(__FreeBSD__)
        dlclose(handle);
#elif defined(__hpux)
		dlclose(handle);
#elif defined(__sparc)
		dlclose(handle);
#elif defined(__OS2__)
		dlclose(handle);
#elif defined(_WIN32)
		WINFREELIB((HINSTANCE) handle);
#elif
		system unsupported so far
#endif
			handle = 0;
	}
}

DynLoader::~DynLoader()
{
	close();
}

int DynLoader::knownSymbol(const char *name) const
{
	return (getSymbol(name, 0) != 0);
}

DynLoader::fptr DynLoader::getSymbol(const char *name, int check) const
{
#if defined(__linux) || defined(__linux__) 
	DynLoader::fptr rfptr = (DynLoader::fptr) dlsym(handle, name);	//lint !e611 //: Suspicious cast
#elif defined(__FreeBSD__)
    DynLoader::fptr rfptr = (DynLoader::fptr) dlsym(handle, name);  //lint !e611 //: Suspicious cast
#elif defined(__hpux)
	DynLoader::fptr rfptr = (DynLoader::fptr) dlsym(handle, name);	//lint !e611 //: Suspicious cast
#elif defined(__sparc)
	DynLoader::fptr rfptr = (DynLoader::fptr) dlsym(handle, name);	//lint !e611 //: Suspicious cast
#elif defined(__OS2__)
	DynLoader::fptr rfptr = (DynLoader::fptr) dlsym(handle, name);	//lint !e611 //: Suspicious cast
#elif defined(_WIN32)
	DynLoader::fptr rfptr = (DynLoader::fptr) GetProcAddress((HINSTANCE) handle, name);	//lint !e611 //: Suspicious cast
#else
	system unsupported so far
#endif
	if ((rfptr == 0) && check) {
		cerr << "error during getSymbol for " << name << ":" << dlerror()
			<< endl;
	}
	return rfptr;
}


#ifndef LEANDYNLOAD
// LEANDYNLOAD means just the DynLoader class

// now the stuff handling search for plugins and loading

// OK, I could take a vector<DynLoader *> but that is not supported with all compilers yet.
class PluginVector {
public:
	enum { maxPlugins = 100 };
	PluginVector():curindex(0u) {
		for (unsigned int i = 0; i < maxPlugins; i++)
			pluginPtr[i] = 0;
	} ~PluginVector() {
		for (unsigned int i = 0; i < maxPlugins; i++) {
			delete pluginPtr[i];
			pluginPtr[i] = 0;
		}
	}
	void add(DynLoader * newelem) {
		pluginPtr[curindex] = newelem;
		curindex++;
	}
	DynLoader *pluginPtr[maxPlugins];
	unsigned int curindex;

	NOCOPYANDASSIGN(PluginVector)
};

static PluginVector LoadedPlugins;

static void loadaPlugin(const char *filename, ostream & errstream,bool verbose)
{
	DriverDescription::currentfilename = filename;
	DynLoader *dynloader = new DynLoader(filename,verbose);
	if (!dynloader->valid()) {
		delete dynloader;
		errstream << "Problem during opening of pstoedit driver plugin: " << filename << ". This is no problem as long the driver in this library is not needed. Possibly you need to install further libraries and/or extend the LD_LIBRARY_PATH (*nix) or PATH (Windows) environment variables." << endl;
		return;
	}
	LoadedPlugins.add(dynloader);

// this seems to be obsolete. but not with .dll's
//          globalRp->explainformats(errstream);
	if (dynloader->knownSymbol("getglobalRp")) {
		getglobalRpFuncPtr dyngetglobalRpFunc =
			(getglobalRpFuncPtr) dynloader->getSymbol("getglobalRp");
		if (dyngetglobalRpFunc == 0) {
			errstream << "could not find getglobalRp " << endl;
			return;
		}
		DescriptionRegister *dynglobalRp = dyngetglobalRpFunc();
		if (dynglobalRp == 0) {
			errstream << " didn't find any registered Drivers " << endl;
		} else if (dynglobalRp != getglobalRp()) {
//                      globalRp->explainformats(errstream);
			getglobalRp()->mergeRegister(errstream, *dynglobalRp, filename);
		}
	} else {
		// didn't fint getglobalRp
		// so assume that the plug-in does not have
		// it's own copy of globalRp but uses the
		// one from the caller
	}
//          globalRp->explainformats(errstream);
}


#if defined(__linux) || defined(__linux__)  || defined(__FreeBSD__) || defined(__sparc) || defined(__hpux) || defined(__OS2__)
// for directory search
#if HAVE_DIRENT_H
#include <dirent.h>
#elif HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#elif HAVE_SYS_DIR_H
#include <sys/dir.h>
#elif HAVE_NDIR_H
#include <ndir.h>
#else
// last chance
#include <direct.h>
#endif

void loadPlugInDrivers(const char *pluginDir, ostream & errstream,bool verbose)
{
	DIR *dirp;
	struct dirent *direntp;

	dirp = opendir(pluginDir);
	if (!dirp)
		return;
	while ((direntp = readdir(dirp)) != NIL) {
//      cout <<  direntp->d_name << endl;
		unsigned int flen = strlen(direntp->d_name);
#ifdef __OS2__
		char *expectedpositionofsuffix = direntp->d_name + flen - 4;
#else
		char *expectedpositionofsuffix = direntp->d_name + flen - 3;
#endif
//              if ( local filename starts with drv or plugins and ends with .so)
		if (((strstr(direntp->d_name, "libp2edrv") == direntp->d_name) ||
			 (strstr(direntp->d_name, "plugin") == direntp->d_name)
			) &&
#ifdef __OS2__
			(strstr(expectedpositionofsuffix, ".dll") == expectedpositionofsuffix)) {
#else
	#ifdef __hpux
			(strstr(expectedpositionofsuffix, ".sl") == expectedpositionofsuffix)) {
	#else
			(strstr(expectedpositionofsuffix, ".so") == expectedpositionofsuffix)) {
	#endif
#endif
			char *fullname = new char[strlen(pluginDir) + flen + 2];
			strcpy(fullname, pluginDir);
			strcat(fullname, "/");
			strcat(fullname, direntp->d_name);
//          cout <<  direntp->d_name  << " " << fullname << endl;
			loadaPlugin(fullname, errstream, verbose);
			delete[]fullname;
		}

	}							// while
	closedir(dirp);
}
#elif defined(_WIN32)


void loadPlugInDrivers(const char *pluginDir, ostream & errstream,bool verbose)
{
	char szExePath[1000];
	szExePath[0] = '\0';
	P_GetPathToMyself("pstoedit", szExePath, sizeof(szExePath));

	WIN32_FIND_DATA finddata;
//   = getPluginDirectory(errstream);
	if (pluginDir != 0) {
		char pattern[] = "/*.dll";
		char *searchpattern = new char[strlen(pluginDir) + strlen(pattern) + 1];
		strcpy(searchpattern, pluginDir);
		strcat(searchpattern, pattern);
		HANDLE findHandle = FindFirstFile(searchpattern, &finddata);
		if (findHandle == INVALID_HANDLE_VALUE) {
			errstream << "Could not open plug-in directory (" << pluginDir
				<< " or didn't find any plugin there" << endl;
		} else {
			BOOL more = FALSE;
			do {

				char *fullname = new char[strlen(pluginDir) + strlen(finddata.cFileName) + 3];
				strcpy(fullname, pluginDir);
				strcat(fullname, "\\");
				strcat(fullname, finddata.cFileName);
//              errstream << "szExePath " << szExePath << endl;
#ifdef xx
#define stricmp _stricmp
#endif
				if ( (stricmp(fullname, szExePath) != 0) && (stricmp(finddata.cFileName, "pstoedit.dll") !=0 ) ) {
					// avoid loading dll itself again
					//                 errstream << "loading " << fullname << endl;
					loadaPlugin(fullname, errstream, verbose);
				} else {
					//                 errstream << "ignoring myself " << finddata.cFileName << endl;
				}
				delete[]fullname;
				more = FindNextFile(findHandle, &finddata);
			} while (more);
			FindClose(findHandle);
		}
		delete[]searchpattern;
	}
}
#else
#error "system unsupported so far"
#endif

#endif 
// LEAN


#else
// no shared libs
#ifdef HAVESTL
#include <iosfwd>
using namespace std;
#else
class ostream;
#endif
void loadPlugInDrivers(const char *pluginDir, ostream & errstream,bool verbose)
{
}								// just a dummy Version
#endif
 
