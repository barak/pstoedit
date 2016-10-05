#ifndef __drvwmf_h__
#define __drvwmf_h__

/*
   drvwmf.h : This file is part of pstoedit
   Class declaration for the WMF output driver.
   The implementation can be found in drvwmf.cpp

   Copyright (C) 1998 Thorsten Behrens and Bjoern Petersen
   Copyright (C) 2000 Thorsten Behrens

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


#ifdef _WIN32

#include <windows.h>
#undef min		// is declared inline in drvbase... (sigh)
#undef max

#else

#include <libemf.h>

#endif

#include "drvbase.h"


class drvWMF : public drvbase {

public:

	derivedConstructor(drvWMF);		// Constructor

	~drvWMF();						// Destructor

#include "drvfuncs.h"

	virtual void show_image(const Image & imageinfo); 

private:

	enum polyType {TYPE_FILL,TYPE_LINES};

	void	drawPoly			(POINT*, int*, polyType type);

	void	setDrawAttr			();

	int		fetchFont			(const TextInfo & textinfo, short int, short int);


// This contains all private data of drvwmf.

	HDC				metaDC;
	HDC				desktopDC;

	LOGPEN			penData;
	HPEN			coloredPen;
	HPEN			oldColoredPen;

	LOGBRUSH		brushData;
	HBRUSH			coloredBrush;
	HBRUSH			oldColoredBrush;

	LOGFONT			theFontRec;
	HFONT			myFont;
	HFONT			oldFont;

	long			fontsHeight, fontsWidth;

	long			showFontList;
	WORD			cntPalEntries;

	long			maxX, maxY;
	long			minX, minY;
	long			maxStatus, minStatus;

	bool			enhanced;	
	bool			mapToArial;
	bool			emulateNarrowFonts;	
	bool			drawBoundingBox;
	bool			pruneLineEnds;
	char*			tempName;
	FILE*			outFile;	
};

#endif /* #ifndef __drvwmf_h__ */
 
 

 
