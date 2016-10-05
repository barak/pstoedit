#ifndef __drvSAMPL_h
#define __drvSAMPL_h

/* 
   drvsampl.h : This file is part of pstoedit
   Class declaration for a sample output driver with no additional attributes
   and methods (minimal interface)

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

#include "drvbase.h"

class drvSAMPL : public drvbase {

public:

	derivedConstructor(drvSAMPL);
	//(const char * driveroptions_P,ostream & theoutStream,ostream & theerrStream ); // Constructor

	~drvSAMPL(); // Destructor

#include "drvfuncs.h"

public:

	virtual void    show_image(const Image & imageinfo); 

private:
	void print_coords();
	int	      imgcount;

};

#endif
 
 
 
