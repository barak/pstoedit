/* 
   drvgeneral.h : This file is part of pstoedit
   Class declaration for every specific output driver. See drvTGIF.h on how this is used.

   Copyright (C) 1993,1994,1995 Wolfgang Glunz, Wolfgang.Glunz@zfe.siemens.de

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

class CLASSNAME : public drvbase {

private:

	void print_coords();
public:

	CLASSNAME(ostream & theoutStream, float theMagnification); // Constructor

	~CLASSNAME(); // Destructor

	void setGrayLevel(const float grayLevel);

	void open_page();

	void close_page();

	void show_textstring(const char * const thetext) ;

	void print_header();

	void print_trailer();

	void show_polyline(const int fillpat); 

	void show_rectangle(const int fillpat, const float llx, const float lly, const float urx, const float ury);

	void show_polygon(const int fillpat); 

};
