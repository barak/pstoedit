/*
   dynload.h : This file is part of pstoedit
   declarations for dynamic loading of drivers

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

class DynLoader 
{
public:
	DynLoader(const char * libname_P = 0);
	~DynLoader();
	void open(const char * libname);
	void close();
	void * getSymbol(const char * name, int check = 1) const;
	int  knownSymbol(const char * name) const;
private:
	const char * libname;
	void * handle;
};

#ifdef HAVESTL
#include <iosfwd>
using namespace std;
#else
class ostream;
#endif
void loadPlugInDrivers(char * pluginDir,ostream& errstream);
 
