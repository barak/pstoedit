#ifndef __drvMET_h
#define __drvMET_h
/*
   drvMET.c : This file is part of pstoedit
   Backend for OS/2 Meta Files.
   Contributed by : Christoph Jaeschke (JAESCHKE@de.ibm.com)
   Modified by : Thomas Hoffmann (thoffman@zappa.sax.de)

   Copyright (C) 1993 - 2001 Wolfgang Glunz, wglunz@pstoedit.net

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

#define INCL_WIN
#define INCL_DOSMEMMGR
// #define INCL_WINDIALOGS
#define INCL_DOS
#define INCL_DEV
#define INCL_GPILCIDS           // Fonts
#define INCL_GPILOGCOLORTABLE
#define INCL_GPIPRIMITIVES
#define INCL_GPIPATHS
#define INCL_GPIPOLYGON
#define INCL_GPIMETAFILES
#define INCL_GPIBITMAPS
#include <os2.h>

#include "drvbase.h"

class INTFONTMAP; // Just a simple forward

#define maxMapEntries 255

// GNU OS/2 headers looks like a bit different, so...

#ifdef __GNUG__
#define PXCHAR unsigned char*
#define PCXCHAR unsigned char*
#else
#define PXCHAR char*
#define PCXCHAR const char*
#endif

extern int yylex(drvbase*);

class drvMET : public drvbase {

private:
	HPS hps;
	HDC hdc;
	HPAL hpal;
	PFONTMETRICS	pfm;
	PFATTRS	pfat;
	long cntFonts;
	long showFontList;
	char lastSelectedFontName[maxFontNamesLength];
	long cntPalEntries;
	ULONG *alTable;
	INTFONTMAP *pIntFontmap;
	long maxPalEntries;
	long palStart;
	long get_print_coords(POINTL aptlPoints[], LONG aIsLineTo[]); 
	void drawPoly(int cntPoints, PPOINTL aptlPoints, PLONG aIsLineTo, int closed);
	long exit;
	long draw_noPath;
	long draw_noColor;
	long draw_noFill;
	long draw_noText;
	long draw_noGraphic;

	long maxX, maxY;
	long minX, minY;
	long maxStatus, minStatus;


public:
	long info;
	derivedConstructor(drvMET);
	//(const char * driveroptions_P,ostream & theoutStream,ostream & theerrStream );
	~drvMET();

	virtual void show_image(const Image & imageinfo); 

	long palEntry(float, float, float);
	long searchPalEntry(long);
	void setColor(float, float, float);
	int FetchFont (SHORT idFont, const char * pFontName, long selection, PFATTRS *pfat);

	inline int transX(float k) { return (int)((k + x_offset)*scale);};
	inline int transY(float k) { return (int)((k + y_offset)*scale);};

        //overwrite the upside-down transformation from the base class
	inline long l_transY			(float y) const	{
		return (long)((y + y_offset) + .5);	// rounded long	
	}


#include "drvfuncs.h"

};

class FONTMAPENTRY {
	char gsName[maxFontNamesLength];
	char pmName[maxFontNamesLength];

public:
	FONTMAPENTRY(char* , char* );
	~FONTMAPENTRY() {};
	inline char* getpmName() {return pmName;};
	inline char* getgsName() {return gsName;};
	inline void reAllocpmName(char* initName) {strcpy(pmName,initName);};
	inline void reAllocgsName(char* initName) {strcpy(gsName,initName);};
	// void reAllocgsName(char*);
};

class INTFONTMAP {
	FONTMAPENTRY *mapEntry[maxMapEntries];
	long cntMapEntries;
	char* fontmapFile;
	char* scanEnv(char*, char*);
	void readFontmap();
	void addEntry(char *pLine);
	//	ostream *pOutf;
public:
	//	INTFONTMAP(ostream*);
	INTFONTMAP();
	~INTFONTMAP();
	const char* searchMapEntry(const char*);
};

#endif 
 
 
