/*
   pstoedit.h : This file is part of pstoedit
   main control procedure

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

//#include <iostream.h>
#ifdef HAVESTL
#include <iosfwd>
USESTD
#else
class ostream;
class istream;
#endif

#ifdef _MSC_VER
#define DLLEXPORT __declspec( dllexport ) 
#else
#define DLLEXPORT 
#endif

typedef int (*execute_interpreter_function)(int argc, const char * const argv[]);


class DescriptionRegister;

// for "C" functions we cannot use "bool"

extern "C" DLLEXPORT 
int pstoedit(	int argc,
				const char * const argv[],
				ostream& errstream,
  				execute_interpreter_function call_PI,
				const char * (*whichPI)(ostream &,int),
				DescriptionRegister* const pushinsPtr =0
			);

extern "C" DLLEXPORT 
int pstoeditwithghostscript(int argc,
							const char * const argv[],
							ostream& errstream,
							DescriptionRegister* const pushinsPtr=0
							);

extern "C" DLLEXPORT 
int pstoedit_plainC(int argc,
					const char * const argv[],
					const char * const psinterpreter  // if 0, then pstoedit will look for one using whichpi()
					);

class DriverDescription;
struct DriverDescription_S;

//extern "C" DLLEXPORT const DriverDescription* const * getPstoeditDriverInfo(ostream& errstream);
extern "C" DLLEXPORT DriverDescription_S* getPstoeditDriverInfo_plainC(void);
extern "C" DLLEXPORT int pstoedit_checkversion (unsigned int callersversion);									 
void ignoreVersionCheck(void); // not exported to the DLL interface, just used internally

#if defined(_WIN32)
#include "cbstream.h"
extern "C" DLLEXPORT void setPstoeditOutputFunction(void * cbData,write_callback_type* cbFunction);
#endif

