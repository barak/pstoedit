#ifndef __drvplot_h
#define __drvplot_h

/* 
   drvplot.h : This file is part of pstoedit
   Backend for GNU metafile format and all other output formats produced
   by the GNU libplot/libplotter vector graphics library

   For documentation, see drvplot.cpp.

   Written by Robert S. Maier <rsm@math.arizona.edu> and Wolfgang Glunz
   <wglunz@geocities.com>.

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

class drvplot : public drvbase 
{
 public:
  derivedConstructor (drvplot);	// ctor
  ~drvplot();			// dtor
  
#include "drvfuncs.h"
  virtual void show_image (const Image& imageinfo); 
  
private:
  class Plotter *plotter;	// either declared in plotter.h, or faked
  bool physical_page;		// can map PS onto a page of specified size?
  int page_type;		// if so, one of PAGE_{ANSI_A,ISO_A4...}

  void set_line_style (void);
  void set_filling_and_edging_style (void);
  void close_output_file_and_reopen_in_binary_mode (void);
};

#endif
 
