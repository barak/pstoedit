/*
   drvWMF.c : This file is part of pstoedit
	 Backend for Windows Meta File (WMF)

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

// drvwmf.cpp:
// Fatal F:\BC5\INCLUDE\services/wsysinc.h 27: Error directive: if windows.h is included before
//  services/wsysinc.h, STRICT must be defined
// *** 1 errors in Compile ***
#ifndef STRICT
#define STRICT
#endif

//JW fuer Zugriff auf Windows-API (WMF-Funktionen)
#include <windows.h>
#include <owl/gdiobjec.h>
// #include <winsys/geometry.h> // fuer tagPOINT (TPoint)

//JW
#include <string.h>
#include <alloc.h>
#include <fstream.h>
#include <math.h>
#include <dir.h>
#include <iostream.h>
// rcw2: work round case insensitivity in RiscOS
#ifdef riscos
  #include "unix:string.h"
#else
  #include <string.h>
#endif
#include <stdlib.h>

#include <process.h> // fuer exec-Fkt.
#include <direct.h> // fuer getcwd-Fkt.
#include <time.h> // fuer Zeitfunktionen

#include <owl/dc.h>


#include "drvwmf.h"

#define GSFONTMAPENV "GS_LIB"
#define GSFONTMAPFILE "FONTMAP"

#define FONT_ID 1
#define DRAW_FILLED 1
#define DRAW_UNFILLED 0
#define DRAW_CLOSED 1
#define DRAW_OPEN 0
#define READ_pNumers 0

#define MAXPALENTRIES 256

struct WmfPrivate {
// This struct contains all private data of drvwmf.
// These were moved to here, in order to keep
// drvwmf.h independent from OWL

	DRVWMFSETUP *pDrvWMFsetup;
	TMetaFileDC *pMetaDC;
	TDesktopDC *pDesktopDC;
	TRect *pRect;

	LPLOGPALETTE MyLogPalette; // JW
	TPalette *pThePalette; // JW

	TPen *pColoredPen; // JW Zeichenstift
	LOGPEN PenData; // JW Datenstruktur des Zeichenstiftes

	TFont *pMyFont;
	LOGFONT  TheFontRec; // Struktur zur Vorgabe der Fontattribute

	TBrush *pColoredBrush; // JW Pinsel zum Flaechenfuellen
	LOGBRUSH BrushData; // JW Pinsel-Daten

	long FontsHeight, FontsWidth;

	long showFontList;
	char lastSelectedFontName[maxFontNamesLength];
	long cntPalEntries;

	long palStart;
	long maxPalEntries;

};

// Short Cuts
#define pDrvWMFsetup pd->pDrvWMFsetup
#define pMetaDC pd->pMetaDC
#define pRect pd->pRect
#define pDesktopDC pd->pDesktopDC
#define MyLogPalette pd->MyLogPalette
#define pThePalette pd->pThePalette
#define pColoredPen pd->pColoredPen
#define PenData pd->PenData
#define pMyFont pd->pMyFont
#define TheFontRec pd->TheFontRec
#define pColoredBrush pd->pColoredBrush
#define BrushData pd->BrushData
#define FontsHeight pd->FontsHeight
#define FontsWidth pd->FontsWidth
#define showFontList pd->showFontList
#define lastSelectedFontName pd->lastSelectedFontName
#define cntPalEntries pd->cntPalEntries
#define palStart pd->palStart
#define maxPalEntries pd->maxPalEntries


const int NumColors = 48;

const TColor Colors[NumColors] = {
  0x00FFFFFFl,0x00E0E0E0l,0x00C0C0FFl,0x00C0E0FFl,0x00E0FFFFl,0x00C0FFC0l,
  0x00FFFFC0l,0x00FFC0C0l,0x00FFC0FFl,0x000000C0l,0x000040C0l,0x0000C0C0l,
  0x0000C000l,0x00C0C000l,0x00C00000l,0x00C000C0l,
  0x00C0C0C0l,0x00404040l,0x008080FFl,0x0080C0FFl,0x0080FFFFl,0x0080FF80l,
  0x00FFFF80l,0x00FF8080l,0x00FF80FFl,0x00000080l,0x00004080l,0x00008080l,
  0x00008000l,0x00808000l,0x00800000l,0x00800080l,
  0x00808080l,0x00000000l,0x000000FFl,0x000080FFl,0x0000FFFFl,0x0000FF00l,
  0x00FFFF00l,0x00FF0000l,0x00FF00FFl,0x00000040l,0x00404080l,0x00004040l,
  0x00004000l,0x00404000l,0x00400000l,0x00400040l
};

static void strRemove(char *toRemoveStr, char *pRemoves)
{
	int length = strlen(toRemoveStr);
	for (int i=0; i<length;i++)
	   if (strchr(pRemoves,toRemoveStr[i]))
	   {
		memmove((void*)&toRemoveStr[i],
			(void*)&toRemoveStr[i+1],length-1-i);
		toRemoveStr[--length] = '\0';
	   }
}


drvWMF::drvWMF(ostream & theoutStream,
		        ostream & theerrStream,
			float magnification,
			char *driveroptions_p,
			DRVWMFSETUP *pDrvWMFsetup_p):
	drvbase(driveroptions_p,theoutStream,theerrStream,0,0,0)
	// Flags: SubPathes
	//        Curveto
  //        Merging
{
	POINT PenWidth; // Datenstruktur fuer die Stiftbreite
	int farben, xres, yres, fontzahl; // Device-Info's
	int i;
	const char description[]="generated by wmf backend of pstoedit";
//now moved to private data	TRect *pRect;

	pd = new WmfPrivate();
	pDrvWMFsetup = pDrvWMFsetup_p;

	pDesktopDC = new TDesktopDC();
	farben =  pDesktopDC->GetDeviceCaps(NUMCOLORS);
	xres =  pDesktopDC->GetDeviceCaps(HORZRES);
	yres =  pDesktopDC->GetDeviceCaps(VERTRES);
	fontzahl = pDesktopDC->GetDeviceCaps(NUMFONTS);
	if (pDrvWMFsetup->info) {
   		errf << "Number of Colors: " << farben << endl;
   		errf << "X-Resolution: " << xres << endl;
   		errf << "Y-Resolution: " << yres << endl;
   		errf << "Number of Fonts: " << fontzahl << endl;
	}

	pRect =  new TRect(0,0,xres,yres); // Screen-Size

	if (pDrvWMFsetup->enhanced)
   // enhanced-Metafile (.emf) fuer W95 oder NT
		pMetaDC = new TMetaFileDC((TDC&)(*(pDesktopDC)),
   					pDrvWMFsetup->pOutFileName,
                  pRect,
                  description);  // Metafile erzeugen
   // Standard-Metafile fuer Win3.1
	else pMetaDC = new TMetaFileDC(pDrvWMFsetup->pOutFileName);

	if(!pMetaDC)
   		errf << "ERROR: could not open metafile: " <<
			pDrvWMFsetup->pOutFileName << endl;
	else {
		if(pDrvWMFsetup->enhanced)	errf << "enhanced metafile opened: " <<
			pDrvWMFsetup->pOutFileName << endl;
		else errf << "standard metafile opened: " <<
			pDrvWMFsetup->pOutFileName << endl;
	}

	y_offset = 0;
	x_offset = 0;
	scale = magnification; // 4.0 for PU_HIENGLISH

   // in height und width werden die Extremwerte (Maxima) gespeichert
	pDrvWMFsetup->height=0;
	pDrvWMFsetup->width=0;
   pDrvWMFsetup->maxstatus=0; // 0, bis erster Wert gefunden
   // in origin_x und origin_y werden die Extremwerte (Minima) gespeichert
	pDrvWMFsetup->origin_x=0;
	pDrvWMFsetup->origin_y=0;
   pDrvWMFsetup->minstatus=0; // 0, bis erster Wert gefunden

	if (pDrvWMFsetup->info) errf << "Open driver for: " << pDrvWMFsetup->pOutFileName << endl;

	errf << "allocating " << (sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * MAXPALENTRIES) << endl;
	char * memspace = new char[sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * MAXPALENTRIES];
	MyLogPalette = (LOGPALETTE *) memspace;
  	MyLogPalette->palVersion = 0x300;

//	MyLogPalette->palNumEntries=2;
   MyLogPalette->palNumEntries = NumColors; // initial color table

   for (i = 2; i < NumColors; ++i) {
		MyLogPalette->palPalEntry[i].peRed = Colors[i].Red();
		MyLogPalette->palPalEntry[i].peGreen = Colors[i].Green();
		MyLogPalette->palPalEntry[i].peBlue = Colors[i].Blue();
		MyLogPalette->palPalEntry[i].peFlags = PC_RESERVED;
   }

	MyLogPalette->palPalEntry[0].peRed = 0xff;
	MyLogPalette->palPalEntry[0].peGreen = 0xff;
	MyLogPalette->palPalEntry[0].peBlue = 0xff;
	MyLogPalette->palPalEntry[1].peRed = 0x0;
	MyLogPalette->palPalEntry[1].peGreen =  0x0;
	MyLogPalette->palPalEntry[1].peBlue = 0x0;

	cntPalEntries = NumColors;

//   cntPalEntries = 2;

	maxPalEntries=MAXPALENTRIES;

	pThePalette = new TPalette(MyLogPalette);
	if(pThePalette) {
		pMetaDC->SelectObject(*pThePalette);
		pMetaDC->RealizePalette();
	}
	else errf << "ERROR: Can't create palette!" << endl;

   // Festlegung der Farbe fuer Zeichenfunktionen
	PenData.lopnStyle=PS_SOLID;
	PenWidth.x=0; // y-Wert wird nicht verwendet
	PenData.lopnWidth=PenWidth; // 0 heisst 1 Pixel breit
	PenData.lopnColor=PALETTEINDEX(1);
	pColoredPen = NULL;

   // Pinsel
	BrushData.lbStyle=BS_SOLID;
	BrushData.lbColor=PALETTEINDEX(1);
	BrushData.lbHatch=NULL;
	pColoredBrush = NULL; // wird spaeter aktiviert

 	const char * const defaultfontname = "Times New Roman";
  	setCurrentFontName(defaultfontname,false);
	strcpy(lastSelectedFontName,defaultfontname);
	pMyFont=NULL;
}



drvWMF::~drvWMF()

{
  // Ursprung und Groesse aendern

   TPoint Origin(pDrvWMFsetup->origin_x,pDrvWMFsetup->origin_y); // Fensterursprung
   TPoint OldOrigin;
	pMetaDC->SetWindowOrg(Origin, &OldOrigin); // Fensterursprung vorgeben

   TSize Extend(pDrvWMFsetup->width-pDrvWMFsetup->origin_x,
   	pDrvWMFsetup->height-pDrvWMFsetup->origin_y);
   TSize OldExtend;
   pMetaDC->SetWindowExt(Extend, &OldExtend);

	if (pDrvWMFsetup->info) {
   	errf << "Bounding Box " << endl;
   	errf << "Origin: " << pDrvWMFsetup->origin_x << " , " <<
      		pDrvWMFsetup->origin_y << endl;
   	errf << "Size: " << pDrvWMFsetup->width-pDrvWMFsetup->origin_x << " , "
   			<< pDrvWMFsetup->height-pDrvWMFsetup->origin_y << endl;
   }

	// Metafile speichern
	delete pMetaDC;

   // JW neue Palette loeschen
//   pMetaDC->RestorePalette();
	if(MyLogPalette) delete[] (char *) MyLogPalette;

	// JW Stift loeschen, alten Stift nehmen
//   pMetaDC->RestorePen();
	if(pColoredPen) delete pColoredPen;

	// JW Pinsel loeschen, alten Pinsel nehmen
 //  pMetaDC->RestoreBrush();
	if(pColoredBrush) delete pColoredBrush;

//   pMetaDC->RestoreFont();
	if(pMyFont) delete pMyFont;

	delete pThePalette;
	delete pDesktopDC;
	delete pRect;
	delete pd;
}


long drvWMF::searchPalEntry(float r, float g, float b)
{
	BYTE rot, gruen, blau;
   rot=(BYTE)(r*255);
   gruen=(BYTE)(g*255);
   blau=(BYTE)(b*255);

	for(int i=0; i<cntPalEntries; i++)
      if ( (MyLogPalette->palPalEntry[i].peRed==rot) &&
           (MyLogPalette->palPalEntry[i].peGreen==gruen) &&
           (MyLogPalette->palPalEntry[i].peBlue==blau) ) return i;

	return -1;                              // Not found!
}

void drvWMF::setColor(float r, float g, float b)

// die Farben fuer die Pen- und Brush-Strukturen werdeb gesetzt; sie werden erst
// wirksam, wenn der Stift oder Pinsel erzeugt und Selektiert wurde

{
   long index = searchPalEntry(r,g,b);
   POINT PenWidth;

	if (index < 0) {        // Not found!

		if (cntPalEntries<maxPalEntries) {
	   	MyLogPalette->palPalEntry[cntPalEntries].peRed = (BYTE)(r*255);
	    	MyLogPalette->palPalEntry[cntPalEntries].peGreen = (BYTE)(g*255);
	    	MyLogPalette->palPalEntry[cntPalEntries].peBlue = (BYTE)(b*255);

			if(pThePalette->SetPaletteEntries(0, cntPalEntries+1,&(MyLogPalette->palPalEntry[0]) )) {

				MyLogPalette->palNumEntries = cntPalEntries+1;
        		pMetaDC->RestorePalette();
        		if(pThePalette) delete pThePalette;
        		pThePalette=new TPalette(MyLogPalette);
			if(!pThePalette) {
				errf << "ERROR: Can't create new palette!" << endl;
				return;
			}
        		pMetaDC->SelectObject(*pThePalette);
		  		pMetaDC->RealizePalette();

        		PenData.lopnColor=PALETTEINDEX(cntPalEntries);
        		BrushData.lbColor=PALETTEINDEX(cntPalEntries);

				if (pDrvWMFsetup->info) errf << "Set palette entry: " << cntPalEntries << endl;
				if (pDrvWMFsetup->info) errf << "r: " << (int)(r*255) <<
            	" g: " << (int)(g*255) << " b: " << (int)(b*255) << endl;

        		cntPalEntries++;

			} else errf << "ERROR: Can't set palette entries!" << endl;

    } // end of maxPalEntries-condition
		else errf << "WARNING: Max. number of colorentries ("<< maxPalEntries <<") exceeded!" << endl;
		// am naechsten kommende Farbe nehmen
   	PenData.lopnColor=PALETTERGB((BYTE)(r*255),(BYTE)(g*255),(BYTE)(b*255));
   	BrushData.lbColor=PALETTERGB((BYTE)(r*255),(BYTE)(g*255),(BYTE)(b*255));

	} // end of not-found-cond.

  else {
   if (pDrvWMFsetup->info) errf << "Color found! Index: " << index << endl;
   if (pDrvWMFsetup->info) errf << "r: " << r << " g: " << g << " b: " << b << endl;

   PenData.lopnColor=PALETTEINDEX(index);
   BrushData.lbColor=PALETTEINDEX(index); }

	PenWidth.x=(int)(((float)currentLineWidth())*scale);
   PenData.lopnWidth=PenWidth;

   //  pMetaDC->RestorePen();
   if(pColoredPen) delete pColoredPen;
   pColoredPen=new TPen(&PenData);
	if(!pColoredPen) errf << "ERROR: setColor: could not create pen !" << endl;
   else pMetaDC->SelectObject(*pColoredPen);

//   pMetaDC->RestoreBrush();
   if(pColoredBrush) delete pColoredBrush;
   pColoredBrush=new TBrush(&BrushData);
	if(!pColoredBrush) errf << "ERROR: setColor: could not create brush !" << endl;
   else pMetaDC->SelectObject(*pColoredBrush);


}


int drvWMF::FetchFont(const char *pFamilyName, short int height, short int angle)
{

  const char *pOrgFamilyName;
  char 	tmpSearchName[100];

  pOrgFamilyName = pFamilyName;

  TheFontRec.lfHeight = height;
  TheFontRec.lfWidth = 0; // optimal anpassen
  TheFontRec.lfEscapement = angle;
  TheFontRec.lfOrientation = 0; // nicht kursiv
  TheFontRec.lfWeight = 0; // FW_BOLD;
  TheFontRec.lfItalic = 0;
  TheFontRec.lfUnderline = 0;
  TheFontRec.lfStrikeOut = 0;
  TheFontRec.lfCharSet = ANSI_CHARSET;
  TheFontRec.lfOutPrecision = OUT_DEFAULT_PRECIS;
  TheFontRec.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  TheFontRec.lfQuality = PROOF_QUALITY;
  TheFontRec.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;

  strcpy(tmpSearchName,pFamilyName);
//  strRemove(tmpSearchName," -");

  strcpy(TheFontRec.lfFaceName,tmpSearchName);

  pMetaDC->RestoreFont();
  if(pMyFont) delete pMyFont;
  pMyFont=new TFont(&TheFontRec);
  pMetaDC->SelectObject(*pMyFont);

  if (pDrvWMFsetup->info) {
		errf << "Font requested: ";
		errf << pOrgFamilyName << ", FONTMAP maps to: ";
		errf << TheFontRec.lfFaceName << endl;
		errf << "Font data: " << endl;
		errf << "Height: " << TheFontRec.lfHeight << endl;
		errf << "Width: " << TheFontRec.lfWidth << endl;
		errf << "Angle: " << TheFontRec.lfEscapement << endl;
		errf << "Orientation: " << TheFontRec.lfOrientation << endl;
  }

  return 0;

}


void drvWMF::print_coords(TPoint *aptlPoints)

// TPoint-Array fuellen
{
	const int numberOfPoints = numberOfElementsInPath();

	for (int n = 0; n < numberOfPoints; n++)  {
		aptlPoints[n] = TPoint( transX(pathElement(n).getPoint(0).x_),
				transY(pathElement(n).getPoint(0).y_) );
      if(pDrvWMFsetup->maxstatus) {
      	if(aptlPoints[n].X() < pDrvWMFsetup->origin_x)
         	pDrvWMFsetup->origin_x=aptlPoints[n].X();
      	if(aptlPoints[n].Y() < pDrvWMFsetup->origin_y)
         	pDrvWMFsetup->origin_y=aptlPoints[n].Y(); }
      else {
			pDrvWMFsetup->origin_x=aptlPoints[n].X();
         pDrvWMFsetup->origin_y=aptlPoints[n].Y();
         pDrvWMFsetup->maxstatus=1; }

      if(pDrvWMFsetup->minstatus) {
      	if(aptlPoints[n].X() > pDrvWMFsetup->width)
      		pDrvWMFsetup->width=aptlPoints[n].X();
      	if(aptlPoints[n].Y() > pDrvWMFsetup->height)
         	pDrvWMFsetup->height=aptlPoints[n].Y(); }
      else {
      	pDrvWMFsetup->width=aptlPoints[n].X();
         pDrvWMFsetup->height=aptlPoints[n].Y(); 
         pDrvWMFsetup->minstatus=1;
      }
   }
}


void drvWMF::close_page()
{
	if (pDrvWMFsetup->info) errf << "End page: " << currentPageNumber << endl;
}

 void drvWMF::open_page()
{
	if (pDrvWMFsetup->info) errf << "Start page: " << currentPageNumber << endl;
}

void drvWMF::show_path() {
	TPoint *aptlPoints = NULL;

	if (pDrvWMFsetup->info) errf << "Path-ShowType: " << currentShowType() << endl;
	drawPoly(0, aptlPoints, (isPolygon() ? DRAW_CLOSED : DRAW_OPEN));
};

void drvWMF::show_text(const TextInfo & textinfo)
{

	short int height;
	short int angle;
	int x,y;

	if (pDrvWMFsetup->draw_noText) return;

  // JW PALETTERGB sucht einen Coloref-Wert aus der aktuellen Palette, der dem
  //    gewuenschten Farbwert am naechsten kommt

	pMetaDC->SetTextColor(PALETTERGB(textinfo.currentR,textinfo.currentG,
								textinfo.currentR));

	if (strcmp(lastSelectedFontName,textinfo.currentFontName.value())) {

		height = (short int)(textinfo.currentFontSize*scale);
		angle = (short int)(textinfo.currentFontAngle);

	   FetchFont(textinfo.currentFontName.value(), height, angle);
		strcpy(lastSelectedFontName,textinfo.currentFontName.value());
	}

	x = transX(textinfo.x);
	y = transY(textinfo.y);

	pMetaDC->TextOut(x, y, textinfo.thetext, strlen(textinfo.thetext));

}

void drvWMF::drawPoly(int cntPoints, TPoint *aptlPoints, int closed)
{
   int ptsFollow;

	if (pDrvWMFsetup->draw_noGraphic) return;

	setColor(currentR(),currentG(),currentB());

	// preparing polygon array
	if (!cntPoints) {		// transfer from pNumbers
		if ( (aptlPoints=new TPoint[numberOfElementsInPath()] )==NULL) {
				errf << "ERROR: Cannot allocate memory for point-array" << endl;
        return; }
		print_coords( aptlPoints ); // Punkte eintragen
      ptsFollow=numberOfElementsInPath();
      // print-coords liefert die Anzahl der Punkte
	}
   else ptsFollow = cntPoints;

	int filled = (currentShowType()==fill) || (currentShowType()==eofill);

	if (filled && (!pDrvWMFsetup->draw_noFill) && (ptsFollow>=2) ) {
   // draw filled polygon
	  if (pDrvWMFsetup->info)
		  errf << "Fillmode: " << ((currentShowType()==fill) ? WINDING : ALTERNATE) << endl;
		if(!pMetaDC->SetPolyFillMode((currentShowType()==fill) ? WINDING : ALTERNATE))
			errf << "ERROR: could not set fill mode" << endl;
		if (!pMetaDC->Polygon(aptlPoints,ptsFollow)) {
			errf << "ERROR: Polygon could not be drawn: " << endl;
			errf << "Number of Points: " << ptsFollow << endl;
			for(int i=0; i < ptsFollow; i++)
			 errf << aptlPoints[i].X() << "," << aptlPoints[i].Y() << endl;
		}
	} // ende fuellmodus

	else {
   // draw not filled polygon (polyline)
  		if(!pMetaDC->Polyline(aptlPoints,ptsFollow)) {
			errf << "ERROR: Polyline could not be drawn: " << endl;
	 		errf << "Number of Points: " << ptsFollow << endl;
			for(int i=0; i < ptsFollow; i++)
				errf << aptlPoints[i].X() << "," << aptlPoints[i].Y() << endl;
		}

		if (closed) {
	  		if(!pMetaDC->MoveTo(aptlPoints[ptsFollow-1].X(), aptlPoints[ptsFollow-1].Y()) )
				errf << "ERROR: MoveTo: " << aptlPoints[ptsFollow-1].X() << "," << aptlPoints[ptsFollow-1].Y() << endl;
		  	if (!pMetaDC->LineTo(aptlPoints[0].X(), aptlPoints[0].Y()))  	// and close it
				if (pDrvWMFsetup->info) errf << "ERROR: LineTo: " << aptlPoints[0].X() << "," << aptlPoints[0].Y() << endl;
			else if (pDrvWMFsetup->info) errf << "Info: LineTo: " << aptlPoints[0].X() << "," << aptlPoints[0].Y() << endl;
    	}

	} // end of else

	if (pDrvWMFsetup->info) {
	  errf << "Info: Poly:" << endl;
	  errf << "Number of Points: " << ptsFollow << endl;
	  for(int i=0; i < ptsFollow; i++)
		  errf << aptlPoints[i].X() << "," << aptlPoints[i].Y() << endl;
  }

 	if (!cntPoints) delete[] aptlPoints;

}

void drvWMF::show_polyline()
{
	TPoint *aptlPoints = NULL;

	if (pDrvWMFsetup->info) errf << "show_Polyline: howToShow: " << currentShowType() << endl;

	drawPoly(0, aptlPoints, DRAW_OPEN);
};

void drvWMF::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
#if 0
// wogl old 
	TPoint *Points = new TPoint[4];

   // Punkte-Array einrichten
	Points[0]=TPoint(transX(llx),transY(lly));
	Points[1]=TPoint(transX(urx),transY(lly));
	Points[2]=TPoint(transX(urx),transY(ury));
	Points[3]=TPoint(transX(llx),transY(ury));
	if (pDrvWMFsetup->info) errf << "Rectangle-howToShow: " << currentShowType() << endl;

	drawPoly(4L, Points, DRAW_CLOSED);
#endif
// just do show_path for a first guess
	show_path();
}

void drvWMF::show_polygon()
{
	TPoint *aptlPoints = NULL;

	if (pDrvWMFsetup->info) errf << "show_Polygon: howToShow: " << currentShowType() << endl;

	drawPoly(0, aptlPoints, DRAW_CLOSED);
}
