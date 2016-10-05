#ifndef __drvDXF_h
#define __drvDXF_h

/* 
   drvDXF.h : This file is part of pstoedit
   Interface for new driver backends

   Copyright (C) 1993 - 2003 Wolfgang Glunz, wglunz@pstoedit.net

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

class drvDXF : public drvbase {

public:

        derivedConstructor(drvDXF);
	// (const char * driveroptions_P,ostream & theoutStream ,ostream & theerrStream ); // Constructor

        ~drvDXF(); // Destructor

private:
		void drawVertex(const Point & p, bool withlinewidth, int val70 = 0);
		void drawLine(const Point & start,const Point & end);
		void curvetoAsOneSpline(const basedrawingelement & elem, const Point & currentpoint);
		void curvetoAsMultiSpline(const basedrawingelement & elem, const Point & currentpoint);
		void curvetoAsPolyLine(const basedrawingelement & elem, const Point & currentpoint);
		void curvetoAsNurb(const basedrawingelement & elem, const Point & currentpoint);
		void curvetoAsBSpline(const basedrawingelement & elem, const Point & currentpoint);
		void curvetoAsBezier(const basedrawingelement & elem, const Point & currentpoint);
		void writehandle();
		
		void printPoint(const Point & p, unsigned short add );
		void writesplinetype(const unsigned short stype);

		enum dxfsplinetype {aspolyline, assinglespline, asmultispline, asnurb, asbspline, asbezier} splinemode;
        	int polyaslines; // dump polygons as lines
 		bool mm; //wether to use mm instead of points
		bool formatis14 ; // whether to use acad 14 format
		unsigned int splineprecision;
		const char * header;
		const char * trailer;
		long handleint;

#include "drvfuncs.h"

		void show_text(const TextInfo & textInfo);

};



#endif

 
 
 
