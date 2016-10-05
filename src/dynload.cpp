/*
   dynload.h : This file is part of pstoedit
   declarations for dynamic loading of drivers

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

// we need __linux instead of just linux since the latter is not defined when -ansi is used.
#if defined(sparc) || defined(__linux) || defined(_WIN32)
	#define HAVESHAREDLIBS
#endif

#ifdef HAVESHAREDLIBS

#include "drvbase.h" 

#include "dynload.h"

#include I_iostream
#include I_stdio
#include I_stdlib
#include I_string_h

// for DriverDescription types.




#if defined(__linux)
	#include <dlfcn.h>
	typedef void (*initfunctype) ();

//	static const char * const libsuffix = ".so";
#elif defined(sparc)
	#if defined(__SVR4) 
                #include <dlfcn.h>
        #else
                // prototypes under SunOS 4.1.x are not compatible with C++
                extern "C" {
                        void *dlopen(const char *pathname, int mode);
                        void *dlsym(void *handle, const char *name);
                        char *dlerror(void);
                        int dlclose(void *handle);
                }
                #define RTLD_LAZY       1
        #endif
        typedef void (*initfunctype) ();
//        static const char * const libsuffix = ".so";   
#elif defined(_WIN32)
//#include <windef.h>
//#include <winbase.h>
	static char dlerror() { return ' ';}

	#include <windows.h>
//	static const char * const libsuffix = ".dll";
#else
	#error "system unsupported so far"
#endif

DynLoader::DynLoader(const char * libname_P):
	libname(libname_P), handle(0)
{ 	
	if (libname) open(libname); 
}

void DynLoader::open(const char * libname_P)
{
	if (handle) {
		cerr << "error: DynLoader has already opened a library" << endl;
		exit(1);
	}
	char * fulllibname = new char [strlen(libname_P) + 1];
	strcpy(fulllibname,libname_P);
	// not needed strcat(fulllibname_P,libsuffix);

#if defined(__linux)
	int loadmode = RTLD_LAZY; // RTLD_NOW
	handle = dlopen(fulllibname, loadmode);
#elif defined(sparc)
	int loadmode = RTLD_LAZY; // RTLD_NOW
	handle = dlopen(fulllibname, loadmode);
#elif defined(_WIN32)
	handle =LoadLibrary(fulllibname);
#else
	system unsupported so far
#endif

	if (handle == 0) {
		cerr <<  "error during opening " << fulllibname << ":" << dlerror() << endl;
		delete [] fulllibname;
		return;
	} 
	{
//		cerr << "loading dynamic library " << fulllibname << " completed successfully" << endl;
#if 0
		initfunctype fptr = (initfunctype) dlsym(handle, "_init");
		if (fptr) {
			cerr << "Found an init function for the library, so execute it " << endl;
			fptr();
		}
#endif		
	}
	delete [] fulllibname;
}

void DynLoader::close()
{
	if (handle) {
#if defined(__linux)
		dlclose(handle);
#elif defined(sparc)
		dlclose(handle);
#elif defined(_WIN32)
		FreeLibrary((HINSTANCE) handle);
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

int  DynLoader::knownSymbol(const char * name) const
{
	return (getSymbol(name,0) != 0);
}

void * DynLoader::getSymbol(const char * name, int check) const
{
#if defined(__linux)
	void * fptr = dlsym(handle, name);
#elif defined(sparc)
	void * fptr = dlsym(handle, name);
#elif defined(_WIN32)
	void * fptr = GetProcAddress((HINSTANCE)handle, name);
#else
	system unsupported so far
#endif
	if ((fptr == 0) && check) {
		cerr << "error during getSymbol for " << name << ":" << dlerror() << endl;
	}
	return fptr;
}




// now the stuff handling search for plugins and loading

// OK, I could take a vector<DynLoader *> but that is not supported with all compilers yet.
struct PluginVector {
	enum {maxPlugins = 100};
	PluginVector() :curindex(0u) { 
		for (unsigned int i = 0; i < maxPlugins; i++) pluginPtr[i]= 0;
	}
	~PluginVector() { 
		for (unsigned int i = 0; i < maxPlugins; i++) {
			delete pluginPtr[i];
			pluginPtr[i]= 0;
		}
	}
	void add(DynLoader * newelem) { 
		pluginPtr[curindex] = newelem; 
		curindex++; 
	}
	DynLoader * pluginPtr[maxPlugins];
	unsigned int curindex;
};

static PluginVector LoadedPlugins;

static void loadaPlugin(const char * filename,ostream& errstream)
{
		DynLoader *dynloader = new DynLoader(filename);
		LoadedPlugins.add(dynloader);

// this seems to be obsolete. but not with .dll's
//			globalRp->explainformats(errstream);
		if (dynloader->knownSymbol("getglobalRp")) {
			getglobalRpFuncPtr dyngetglobalRpFunc = (getglobalRpFuncPtr) dynloader->getSymbol("getglobalRp");
			if (dyngetglobalRpFunc == 0) {
				errstream << "could not find getglobalRp " << endl;
				return;
			}
			DescriptionRegister* dynglobalRp = dyngetglobalRpFunc();
			if (dynglobalRp == 0)  {
					errstream << " didn't find any registered Drivers " << endl;
			} else if (dynglobalRp != globalRp) {
//						globalRp->explainformats(errstream);
					globalRp->mergeRegister(errstream,*dynglobalRp,filename);
			}
		} else {
					// didn't fint getglobalRp
					// so assume that the plug-in does not have
					// it's own copy of globalRp but uses the
					// one from the caller
		}
//			globalRp->explainformats(errstream);
}

#if defined(__linux) || defined(sparc)
// for directory search
#include <dirent.h>

void loadPlugInDrivers(char * pluginDir,ostream& errstream)
{
	DIR *dirp;
	struct dirent *direntp;

	dirp = opendir( pluginDir );
	if (!dirp) return;
	while ( (direntp = readdir( dirp )) != NULL ) 
	{
//		cout <<  direntp->d_name << endl;
		unsigned int flen = strlen(direntp->d_name);
		char * expectedpositionofsuffix = direntp->d_name + flen - 3;
//              if ( local filename starts with drv or plugins and ends with .so)
                if (  (
                        (strstr(direntp->d_name,"drv") == direntp->d_name) ||
                        (strstr(direntp->d_name,"plugin") == direntp->d_name)
                      ) &&
                      (strstr(expectedpositionofsuffix ,".so") == expectedpositionofsuffix) ) {
			char * fullname = new char [ strlen(pluginDir) + flen + 2];
			strcpy(fullname,pluginDir);
			strcat(fullname,"/");
			strcat(fullname,direntp->d_name);
//			cout <<  direntp->d_name  << " " << fullname << endl;
			loadaPlugin(fullname,errstream);
			delete [] fullname;
		}

	} // while
	closedir( dirp );
}
#elif defined(_WIN32)


void loadPlugInDrivers(char * pluginDir, ostream& errstream)
{
	char szExePath[1000];
	szExePath[0]= '\0';
	P_GetPathToMyself("pstoedit",szExePath, sizeof(szExePath));

	WIN32_FIND_DATA finddata;
//	 = getPluginDirectory(errstream);
	if (pluginDir != 0) {
		char pattern[] = "/*.dll";
		char * searchpattern = new char[ strlen(pluginDir) + strlen(pattern) + 1];
		strcpy(searchpattern,pluginDir);
		strcat(searchpattern,pattern);
		HANDLE findHandle = FindFirstFile(searchpattern,&finddata);
		if (findHandle == INVALID_HANDLE_VALUE) {
			errstream << "Could not open plug-in directory (" << pluginDir << " or didn't find any plugin there" << endl;
		} else {
			BOOL more = FALSE;
			do {

				char * fullname = new char[ strlen(pluginDir) + strlen(finddata.cFileName) + 3];
				strcpy(fullname,pluginDir);
				strcat(fullname,"\\");
				strcat(fullname,finddata.cFileName);
//				errstream << "szExePath " << szExePath << endl;
#ifdef xx
#define stricmp _stricmp
#endif
				if (stricmp(fullname,szExePath) != 0) {
					// avoid loading dll itself again
 //					errstream << "loading " << fullname << endl;
					loadaPlugin(fullname,errstream);
				} else {
 //					errstream << "ignoring myself " << finddata.cFileName << endl;
				}
				delete [] fullname;
				more = FindNextFile(findHandle,&finddata);
			} while (more);
			FindClose(findHandle);
		}
		delete [] searchpattern;
	}
}
#else
	#error "system unsupported so far"
#endif


#else
// no shared libs
class ostream;
void loadPlugInDrivers(char * pluginDir,ostream& errstream) {} // just a dummy Version
#endif
 
 
