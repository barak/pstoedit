#ifndef __drvKIllustrator_h
#define __drvKIllustrator_h

/* 
   drvsampl.h : This file is part of pstoedit
   Class declaration for a KIllustrator output driver.

   Copyright (C) 1993,1994,1995,1996,1997 Wolfgang Glunz, Wolfgang.Glunz@mchp.siemens.de
   Copyright (C) 1998 Kai-Uwe Sattler, kus@iti.cs.uni-magdeburg.de

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

class drvKIllustrator : public drvbase {
public:
   derivedConstructor(drvKIllustrator);
  ~drvKIllustrator (); // Destructor

private:
  static int cvtColor (float c);

#include "drvfuncs.h"
};

#endif
 
