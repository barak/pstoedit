/* 
   drvexample.c : This file is part of pstoedit
   Skeleton for the implementation of new backends

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

#include <stdio.h>
#include <string.h>
#include "drvexample.h"

drvexample::drvexample()
{
// driver specific initializations
}

drvexample::~drvexample() {}

void drvexample::setGrayLevel(const float grayLevel)
{
    currentGrayLevel = grayLevel;
 /*       FillValue = ???? */
}


void drvexample::print_coords()
{
}

void drvexample::close_page()
{
}

void drvexample::open_page()
{
}

void drvexample::show_textstring(const char *const thetext)
{
}

void drvexample::print_header()
{
}

void drvexample::print_trailer()
{
}

void drvexample::show_polyline(const int fillpat)
{
};

void drvexample::show_rectangle(const int fillpat, const float llx, const float lly, const float urx, const float ury)
{
 // just do show_polyline for a first guess
 show_polyline(fillpat);
}

void drvexample::show_polygon(const int fillpat)
{
 // just do show_polyline for a first guess
 show_polyline(fillpat);
}
