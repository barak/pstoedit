/*
   drvWMF.h : This file is part of pstoedit
	 Header-File for drvWMF.cpp

   Copyright (C) 1996,1997 Jens Weber, rz47b7@PostAG.DE

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

#ifndef __drvwmf_h
#define __drvwmf_h
#include "drvbase.h"


#define VZX 1.0 // JW
#define VZY -1.0 // JW



struct DRVWMFSETUP {
  long exit;
	long info;
	long draw_noPath;
	long draw_noColor;
//	long draw_digColor;
	long draw_target;
	enum targetType {to_META, to_WINDOW};
	long draw_noFill;
	long draw_noText;
	long draw_noGraphic;
//	long noLogPal;
	char *pOutFileName;
	char *pInFileName;
	char wmf_options[100];
	char *infile;
	int enhanced;
	int height, width;
	int origin_x, origin_y;
	int maxstatus, minstatus;
 };

struct WmfPrivate; // forward to the internal data

#ifndef _BYTE_DEFINED
typedef unsigned char BYTE;
#endif
class  TPoint;

class drvWMF : public drvbase {

private:

	void print_coords(TPoint *);
	void drawPoly(int, TPoint *, int);

public:
	drvWMF(ostream &, ostream &,float , char *, DRVWMFSETUP *);
	~drvWMF();

private:
	WmfPrivate * pd; // the real private data

	long searchPalEntry(float, float, float);
	void setColor(float, float, float);
//	void setGrayLevel(const float grayLevel);

	int FetchFont (const char *pFontName, short int, short int);

	inline int transX(float k) {
		int value=(int)((VZX*k + x_offset)*scale);
		if(value<0) value=0;
		return value;		};

	inline int transY(float k) {
		int value=(int)((VZY*k + y_offset)*scale);
//		int value=(int)((800 -k)*scale); // just for test
//		if(value<0) value=0;
		return value; }

public:
	void show_polyline();
	void show_polygon();

  // Abstrakte Funktionen aus drvbase
	virtual void open_page();
	virtual void close_page();
	virtual void show_text(const TextInfo &);
	virtual void show_rectangle(const float llx, const float lly, const float urx, const float ury);
	virtual void show_path();

};


#endif
