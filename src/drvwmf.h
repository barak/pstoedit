#ifndef __drvwmf_h__
#define __drvwmf_h__

/*
   drvwmf.h : This file is part of pstoedit
   Class declaration for the WMF output driver.
   The implementation can be found in drvwmf.cpp

   Copyright (C) 1998 Thorsten Behrens and Bjoern Petersen

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


#include <windows.h>
#undef min		// is declared inline in drvbase... (sigh)
#undef max

#include "drvbase.h"


struct WmfPrivate; // forward to the internal data


class drvWMF : public drvbase {

public:

	derivedConstructor(drvWMF);		// Constructor

	~drvWMF();						// Destructor

#include "drvfuncs.h"

	virtual void show_image(const Image & imageinfo); 
//	virtual bool driverOK() const;

private:

	#define TYPE_LINES			1
	#define TYPE_FILL			2
	void	print_coords		(POINT*, int*, int type);
	void	drawPoly			(int, POINT*, int);

	long	searchPalEntry		(float, float, float);
	void	setColor			(float, float, float);

	int		FetchFont			(const TextInfo & textinfo, short int, short int);

	inline int transX			(float k) 
		{
			return (int)((k + x_offset) + .5);	// rounded int	
		}

	inline int transY			(float k) 
		{
			return (int)((-1.0*k + y_offset) + .5);	// rounded int, mirrored

		}


// This here contains all private data of drvwmf.

	HDC				MetaDC;
	HDC				DesktopDC;
	RECT			rect;

	LPLOGPALETTE	MyLogPalette;
	HPALETTE		ThePalette;
	HPALETTE		OldPalette;

	LOGPEN			PenData;
	HPEN			ColoredPen;
	HPEN			OldColoredPen;

	LOGBRUSH		BrushData;
	HBRUSH			ColoredBrush;
	HBRUSH			OldColoredBrush;

	LOGFONT			TheFontRec;
	HFONT			MyFont;
	HFONT			OldFont;

	long			FontsHeight, FontsWidth;

	long			showFontList;
	char			lastSelectedFontName[maxFontNamesLength];
	long			lastSelectedFontHeight;
	long			lastSelectedFontAngle;
	WORD			cntPalEntries;

	bool			palette;	
	long			palStart;
	long			maxPalEntries;

	long			height, width;
	long			origin_x, origin_y;
	long			maxstatus, minstatus;

	bool			enhanced;	
	char*			tempName;
	FILE*			outFile;	
};

#endif /* #ifndef __drvwmf_h__ */
 
