#ifndef __psfront_h
#define __psfront_h
/*
   psfront.h : This file is part of pstoedit
   contains the class responsible for reading the dumped PostScript format
  
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
/*
// ============================================================================
//
// = LIBRARY
//     pstoedit
//
// = FILENAME
//     psfront.h
//
// = RCSID
//     $Id$
*/

#include "drvbase.h"

class PSFrontEnd {
public:
	PSFrontEnd(ostream & errstream,drvbase & backend_p);
	~PSFrontEnd();

	void 		run(bool merge);	// do the conversion

private:

	void            addNumber(float value); // add a number to the current path

	float           pop(); // pops and returns last value on stack

	int 		yylex();    // read the input and call the backend specific
				    // functions
	void 		yylexcleanup(); // called from destructor


private:
	ostream &	errf;           // the error stream
	drvbase &	backend;
	float *         numbers; // The number stack [maxpoints]
	unsigned int	nextFreeNumber;
	unsigned int	pathnumber; // number of path (for debugging)
	bool 		non_standard_font;

	// Inhibitors (declared, but not defined)
	PSFrontEnd(const PSFrontEnd &);
	PSFrontEnd & operator=(const PSFrontEnd&);
};


#endif
