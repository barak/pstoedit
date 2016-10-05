#ifndef __drvPIC_h
#define __drvPIC_h

/* 
   drvpic.h : This file is part of pstoedit
   Class declaration for PIC output driver

   Copyright (C) 1999 Egil Kvaleberg, egil@kvaleberg.no

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

class drvPIC : public drvbase {

public:
	 derivedConstructor(drvPIC);
	~drvPIC(); // Destructor

#include "drvfuncs.h"

private:
	void print_coords();
	float x_coord(float x, float y);
	float y_coord(float x, float y);
	void ps_begin();
	void ps_end();

	int troff_mode;         // troff, as opposed to groff
	int landscape;          // original is in landscape mode
	int keep_font;          // print unrecognized literally
	int text_as_text;       // try not to make pictures from running text
	int debug;              // obvious...

	float largest_y;
	float pageheight;
	int withinPS;
};

#endif
 
 
 
