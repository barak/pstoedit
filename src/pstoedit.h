/*
   pstoedit.h : This file is part of pstoedit
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
#ifndef cppcomp_h
#include "cppcomp.h"
#endif

//#include <iostream.h>
#ifdef HAVESTL
#include <iosfwd>
USESTD
#else
#if defined (__GNUG__)  && (__GNUC__>=3) 
// if we dont define HAVESTL for g++ > 3.0, then we can use the simple forwards
	#include <iostream.h>
#else
	class istream;
	class ostream;
#endif
#endif

typedef int (*execute_interpreter_function)(int argc, const char * const argv[]);
typedef const char * (*whichPI_type)(ostream &,int);


class DescriptionRegister;

// for "C" functions we cannot use "bool"

extern "C" DLLEXPORT 
int pstoedit(	int argc,
				const char * const argv[],
				ostream& errstream,
  				execute_interpreter_function call_PI,
				whichPI_type whichPI,
				const DescriptionRegister* const pushinsPtr =0
			);

extern "C" DLLEXPORT 
int pstoeditwithghostscript(int argc,
							const char * const argv[],
							ostream& errstream,
							const DescriptionRegister* const pushinsPtr=0
							);

extern "C" DLLEXPORT 
int pstoedit_plainC(int argc,
					const char * const argv[],
					const char * const psinterpreter  // if 0, then pstoedit will look for one using whichpi()
					);

struct DriverDescription_S;

//extern "C" DLLEXPORT const class DriverDescription* const * getPstoeditDriverInfo(ostream& errstream);
extern "C" DLLEXPORT DriverDescription_S* getPstoeditDriverInfo_plainC(void);
extern "C" DLLEXPORT int pstoedit_checkversion (unsigned int callersversion);									 
void ignoreVersionCheck(void); // not exported to the DLL interface, just used internally

#if defined(_WIN32) || defined(__OS2__)
#include "cbstream.h"
extern "C" DLLEXPORT void setPstoeditOutputFunction(void * cbData,write_callback_type* cbFunction);
#endif

 
 
 
