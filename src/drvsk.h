#ifndef __drvSK_h
#define __drvSK_h

/* 
   drvsk.h
*/

/*

   Copyright (C) 1999 by Bernhard Herzog

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

class drvSK : public drvbase {

public:

    derivedConstructor(drvSK);

    ~drvSK();

#include "drvfuncs.h"

    virtual void    show_image(const Image & imageinfo);

 protected:
    virtual bool pathsCanBeMerged(const PathInfo &, const PathInfo &) const;

 private:
	 void print_coords();
    int id;
    int getid();

    struct {
	int leftbits;
	unsigned int leftchar;
	int column;
    } state;

    int write_base64(const unsigned char * buf, int length);
    void close_base64();

};


inline int drvSK::getid()
{
    return id++;
}
#endif
 
 
