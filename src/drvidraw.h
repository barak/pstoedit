#ifndef __drvIDRAW_h
#define __drvIDRAW_h

/* 
   drvidraw.h : This file is part of pstoedit
   Backend for idraw files
   Contributed by: Scott Pakin <pakin@uiuc.edu>

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

#include "drvbase.h"

static const float IDRAW_SCALING=0.799705;
enum {IDRAW_NUMCOLORS=12};

class drvIDRAW : public drvbase {

public:
  drvIDRAW(const char * driveroptions_P,ostream & theoutStream,ostream & theerrStream ); // Constructor

  ~drvIDRAW(); // Destructor

private:
  static const char *psfont2xlfd(const char *); // Map PS --> XLFD font name
  const char *rgb2name(double, double, double);    // Map RGB --> color name
  void print_header(const char *);    // Output idraw header
  struct {                            // Map of RGB to color name
    double red, green, blue;
    const char *name;
  } color[IDRAW_NUMCOLORS];

  // Scale a PostScript value to an idraw value
  static inline const unsigned int iscale(float invalue) {
    return (unsigned int) (invalue/IDRAW_SCALING + 0.5);
  }

#include "drvfuncs.h"

};

#endif
