#ifdef __OS2__
// this file compiles only under OS/2
/*
   drvMET.cpp : This file is part of pstoedit
   Backend for OS/2 Meta Files.
   Contributed by : Christoph Jaeschke (JAESCHKE@de.ibm.com)
   Modified by: Thomas Hoffmann (thoffman@zappa.sax.de)

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

#include <iostream.h>
#include <fstream.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "drvMET.h"
#include "psfront.h"

#define GSFONTMAPENV "GS_LIB"
#define GSFONTMAPFILE "FONTMAP"

#define FONT_ID 1
#define DRAW_CLOSED 1
#define DRAW_OPEN 0
#define READ_pNumers 0

HAB hab;
HMQ hmq;
QMSG qmsg;
drvMET *pDrvMET;

void strRemove(char *toRemoveStr, const char *pRemoves)
{

	int length = strlen(toRemoveStr);

	for (int i = 0; i < length; i++)
		if (strchr(pRemoves, toRemoveStr[i])) {
			memmove((void *) &toRemoveStr[i], (void *) &toRemoveStr[i + 1], length - 1 - i);
			toRemoveStr[--length] = '\0';
		}
}

FONTMAPENTRY::FONTMAPENTRY(char *gsInitName, char *pmInitName)
{

	strcpy(gsName, gsInitName);
	strcpy(pmName, pmInitName);
}

//INTFONTMAP::INTFONTMAP(ostream *pOutstream) {

INTFONTMAP::INTFONTMAP()
{

	cntMapEntries = 0;
	if (NULL != (fontmapFile = scanEnv(GSFONTMAPENV, GSFONTMAPFILE))) {
		readFontmap();
		if (pDrvMET->info) {
			cerr << endl << "Extracted FONTMAP table:" << endl;
			for (int i = 0; i < cntMapEntries; i++)
				cerr << mapEntry[i]->getgsName() << "->" << mapEntry[i]->getpmName() << endl;
			cerr << "Fontmap table end." << endl << endl;
		}
	}
}

INTFONTMAP::~INTFONTMAP()
{

	for (int i = 0; i < cntMapEntries; i++) {
		delete mapEntry[i];
	}
}

const char *INTFONTMAP::searchMapEntry(const char *gsName)
{

	for (int i = 0; i < cntMapEntries; i++)
		if (!strcmp(gsName, mapEntry[i]->getgsName()))
			return mapEntry[i]->getpmName();	// Found!
	return gsName;
}

void INTFONTMAP::addEntry(char *pLine)
{

	char *gsName;
	char *pmName;

	if (cntMapEntries < maxMapEntries) {
		gsName = strtok(pLine, " \t");	// First Token
		pmName = strtok(NULL, " \t");	// Second Token
		if (pmName)
			if (pmName[0] == '/') {	// Thats an alias
				mapEntry[cntMapEntries++] = new FONTMAPENTRY(&gsName[1], &pmName[1]);
				// Now: gsName is an alias for pmName!
				// First search existing alias in List and replace
				// match with further alias
				for (int i = 0; i < cntMapEntries - 1; i++)
					if (!strcmp(mapEntry[i]->getpmName(), &gsName[1]))
						mapEntry[i]->reAllocpmName(&pmName[1]);
			}
	} else
		cerr << "WARNING: Max. number of FONTMAP-Entries succeeded: " << cntMapEntries << endl;
}

void INTFONTMAP::readFontmap()
{

	const int maxLine = 255;
	char pLine[maxLine];
	char c;

	ifstream mapf(fontmapFile);
	while (mapf.get(pLine, maxLine, '\n')) {
		mapf.get(c);
		if (pLine[0] == '/')
			addEntry(pLine);
	}
	mapf.close();
}

char *INTFONTMAP::scanEnv(char *envName, char *fileName)
{

	PCXCHAR resPointer;
	static BYTE resBuffer[128];
	APIRET rc;

	if (!(rc = DosScanEnv((PXCHAR) envName, &resPointer))) {
		if (pDrvMET->info)
			cerr << envName << " is " << resPointer << endl;
	} else {					// not found
		cerr << "WARNING: " << envName << " not set, rc=" << rc << endl;
		return (char *) 0;
	} if (!(rc = DosSearchPath(SEARCH_ENVIRONMENT, (PXCHAR) envName,
							   (PXCHAR) fileName, resBuffer, sizeof(resBuffer)))) {
		if (pDrvMET->info)
			cerr << "Found desired file: " << resBuffer << endl;
		return (char *) resBuffer;
	} else {					// not found
		cerr << "WARNING: Desired file " << fileName << " in " <<
			resPointer << " not found, rc=" << rc << endl;
		return (char *) 0;
	}
}

drvMET::derivedConstructor(drvMET):
  //(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream): 
    constructBase
{

	long cFonts;
	SIZEL size = { 0, 0 };
	const char *dData[2] = { 0, "DISPLAY" };

	y_offset = 0;
	x_offset = 0;
	cntFonts = 0;
	scale = 1;					// 4.0 for PU_HIENGLISH
	showFontList = 1;

	draw_noPath = false;
	draw_noFill = false;
	draw_noColor = false;
	draw_noText = false;
	draw_noGraphic = false;
	info = false;
	exit = false;

	if (driveroptions_p != NULL) {
		if (strchr(driveroptions_p, 'p') != NULL) {
			draw_noPath = true;
		}
		if (strchr(driveroptions_p, 'l') != NULL) {
			draw_noFill = true;
		}
		if (strchr(driveroptions_p, 'c') != NULL) {
			draw_noColor = true;
		}
		if (strchr(driveroptions_p, 't') != NULL) {
			draw_noText = true;
		}
		if (strchr(driveroptions_p, 'g') != NULL) {
			draw_noGraphic = true;
		}
		if (strchr(driveroptions_p, 'v') != NULL) {
			info = true;
		}
		if (strchr(driveroptions_p, 'h') != NULL || strchr(driveroptions_p, '?') != NULL) {
			exit = true;
		}
	}

	if (exit) {
		cerr << "Metafile-Backend for pstoedit, (C) 1996 by Christoph Jaeschke" << endl;
    cerr << "modified 2001 by Thomas Hoffmann" << endl;
    cerr << "email: thoffman@zappa.sax.de" << endl;
		cerr << "p	Draw no geometric linewidths, all lines with width 0" << endl;
		cerr << "l	No filling of polygon interiors" << endl;
		cerr << "c	No colors, only greyscales" << endl;
		cerr << "t	Draw no text" << endl;
		cerr << "g	Draw no graphics" << endl;
		cerr << "v	Verbose infos to STDERR" << endl;
	}


	pDrvMET = this;

	hab = WinInitialize(0);

	hdc = DevOpenDC(hab, OD_METAFILE, (const unsigned char *) "*", 2L, (PDEVOPENDATA) dData, 0L);
	if (!hdc)
		cerr << "ERROR: Can't open DC!" << endl;
	hps = GpiCreatePS(hab, hdc, &size, PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);
	if (!hps)
		cerr << "ERROR: Can't open PS!" << endl;;

	DevQueryCaps(GpiQueryDevice(hps), CAPS_COLOR_INDEX, 1L, &maxPalEntries);
	alTable = new ULONG[maxPalEntries];
	long mp = maxPalEntries;
	for (int i = 0; i < maxPalEntries; i++)
		alTable[i] = palEntry(i / mp, i / mp, i / mp);
	alTable[0] = palEntry(1, 1, 1);
	alTable[1] = palEntry(0, 0, 0);
	cntPalEntries = 2;
	if (info)
		cerr << "Open driver for: " << outFileName << endl;
	if (!GpiCreateLogColorTable
		(hps, LCOL_PURECOLOR, LCOLF_CONSECRGB, 0L, maxPalEntries - 1, (long *) alTable))
		cerr << "ERROR: Can't update logical palette!" << endl;
	if (!GpiSetColor(hps, 1))
		cerr << "ERROR: Can't set color: 1" << endl;

	// Now Count and retrieve Fontinformations

	pIntFontmap = new INTFONTMAP();

	cFonts = GpiQueryFonts(hps, QF_PUBLIC, (PSZ) NULL, &cntFonts,
						   sizeof(FONTMETRICS), (PFONTMETRICS) NULL);
	cntFonts = cFonts;

	pfm = (PFONTMETRICS) malloc(sizeof(FONTMETRICS) * cFonts);
	if (!pfm)
		cerr << "ERROR: Can't alloc memory for fontmetrics" << endl;
	GpiQueryFonts(hps, QF_PUBLIC, (PSZ) NULL, &cntFonts, sizeof(FONTMETRICS), pfm);
	const char *const defaultfontname = "TimesNewRoman";
	setCurrentFontName(defaultfontname, false);
	FetchFont(FONT_ID, defaultfontname, 0L, &pfat);
	strcpy(lastSelectedFontName, defaultfontname);
}

drvMET::~drvMET()
{

	HMF hmf;

	free(pfm);
	GpiAssociate(hps, NULLHANDLE);
	if (!GpiDestroyPS(hps))
		cerr << "ERROR: Can't destroy PS" << endl;
	hmf = (HMF) DevCloseDC(hdc);
	DosDelete((PXCHAR) outFileName.value());
	if (GpiSaveMetaFile(hmf, (PXCHAR) outFileName.value())) {
		if (info)
			cerr << "Metafile saved!" << endl;
	} else
		cerr << "ERROR: Can't save metafile." << endl;
	GpiDeleteMetaFile(hmf);
	delete alTable;
	delete pIntFontmap;
	WinTerminate(hab);
}

long drvMET::palEntry(float r, float g, float b)
{

	long lr, lg, lb;

	lr = (long) (r * 255);
	lg = (long) (g * 255);
	lb = (long) (b * 255);
	if (pDrvMET->draw_noColor)
		lr = lg = lb = (lr + lg + lb) / 3;
	return ( /*((PC_RESERVED | PC_EXPLICIT )*16777216) */ +(lr * 65536) +
			(lg * 256) + lb);
}

long drvMET::searchPalEntry(long palEntry)
{

	for (int i = 0; i < cntPalEntries; i++)
		if ((unsigned long) palEntry == alTable[i])
			return i;			// Found!
	return -1;					// Not found!
}

void drvMET::setColor(float r, float g, float b)
{

	long index;

	index = searchPalEntry(palEntry(r, g, b));
	if (index < 0) {			// Not found!
		if (cntPalEntries < maxPalEntries) {
			alTable[cntPalEntries] = palEntry(r, g, b);
			if (GpiCreateLogColorTable
				(hps, LCOL_PURECOLOR, LCOLF_CONSECRGB, 0L, maxPalEntries, (long *) alTable)) {
				if (pDrvMET->info)
					cerr << "Added Colorindex: " << cntPalEntries << endl;
			} else
				cerr << "ERROR: Can't update logical palette!" << endl;
			if (!GpiSetColor(hps, cntPalEntries))
				cerr << "ERROR: Can't set color: " << cntPalEntries << endl;
			cntPalEntries++;
		} else
			cerr << "WARNING: Max. number of colorentries (" <<
				maxPalEntries << ") exceeded!" << endl;
	} else						// Found!
	if (!GpiSetColor(hps, index))
		cerr << "ERROR: Can't set color: " << index << endl;
}

int drvMET::FetchFont(SHORT idFont, const char *pFamilyName, long selection, PFATTRS * pfat)
{

	FATTRS fat;
	PFONTMETRICS pFontMetrics;
	APIRET rc;
	long i;
	long tableEntry;
	const char *pOrgFamilyName;
	char lastOutfName[FACESIZE];
	char tmpIsName[FACESIZE];
	char tmpSearchName[FACESIZE];

	pOrgFamilyName = pFamilyName;
	pFamilyName = pIntFontmap->searchMapEntry(pFamilyName);
	if (pDrvMET->info) {
		cerr << "Font requested: ";
		if (pOrgFamilyName != pFamilyName)
			cerr << pOrgFamilyName << ", FONTMAP maps to: ";
		cerr << pFamilyName << endl;
	}
	strcpy(tmpSearchName, pFamilyName);
	strRemove(tmpSearchName, " -");
	lastOutfName[0] = '\0';
	if (showFontList) {
		if (pDrvMET->info)
			cerr << "Table of installed PM-Fonts: " << endl;
		tableEntry = 0;
		pFontMetrics = pfm;
		for (i = 0; i < cntFonts; i++) {
			if (strcmp(pFontMetrics->szFacename, lastOutfName)) {
				if (pDrvMET->info)
					cerr << ++tableEntry << ": " << pFontMetrics->szFacename << endl;
				strcpy(lastOutfName, pFontMetrics->szFacename);
			}
			pFontMetrics++;
		}
		if (info)
			cerr << "End Table of PM-Fonts." << endl;
	}
	showFontList = 0;			// Show only once
	pFontMetrics = pfm;
	for (i = 0; i < cntFonts; i++) {	// Real search
		strcpy(tmpIsName, pFontMetrics->szFacename);
		strRemove(tmpIsName, " -");
		if (!strcmp(tmpIsName, tmpSearchName))
			break;				// Font found in List!
		pFontMetrics++;
	}
	if (i != cntFonts) {
		strcpy(fat.szFacename, pFontMetrics->szFacename);
		fat.usRecordLength = sizeof(FATTRS);	/* sets size of structure   */
		fat.fsSelection = (USHORT) selection;
		fat.lMatch = pFontMetrics->lMatch;
		fat.idRegistry = 0;		/* uses default registry            */
		fat.usCodePage = 0;		/* code-page 0                      */
		fat.lMaxBaselineExt = 0L;	/* requested default font height */
		fat.lAveCharWidth = 0L;	/* requested default font width */
		fat.fsType = 0;			/* uses default type                */
		fat.fsFontUse = FATTR_FONTUSE_OUTLINE;	/* outline font */
		rc = GpiCreateLogFont(hps, NULL, idFont, &fat);
		*pfat = &fat;
		if (pDrvMET->info) {
			cerr << "Found PM name: " << pFontMetrics->szFacename
				<< " (attributes: " << selection << ")" << endl;
			cerr << "CreateFont (";
			if (rc == FONT_MATCH)
				cerr << "selected)" << endl;
			if (rc == FONT_DEFAULT)
				cerr << "not selected -> defaultet)" << endl;
		}
		if (rc == GPI_ERROR) {
			cerr << "ERROR: Found PM name: " << pFontMetrics->szFacename
				<< ", but CreateFont yields to GPI_ERROR)" << endl;
			return FALSE;
		}
		return TRUE;
	} else {
		cerr << "WARNING: Font request: " << pFamilyName << " not found!" << endl;
		return 0;
	}
}

long drvMET::get_print_coords(POINTL aptlPoints[], LONG aIsLineTo[])
{
	PLONG koords = (PLONG) aptlPoints;
	LONG lastMovePoint = 0, savedIsLineTo;
	for (unsigned int i = 0; i < numberOfElementsInPath(); i++) {
		aIsLineTo[i] = pathElement(i).getType();
		if (aIsLineTo[i] == moveto)
			/* mark the last moveto for a closepath operation */
			lastMovePoint = i;
		if (aIsLineTo[i] == closepath) {
			/* closepath's seems to have no valid koords,
			   substituted here by the last moveto-koord's,
			   finally settings the line-type to move */
			koords[2 * i] = koords[2 * lastMovePoint];
			koords[(2 * i) + 1] = koords[(2 * lastMovePoint) + 1];
			aIsLineTo[i] = lineto;
		}
    else {
      koords[2 * i] = transX(pathElement(i).getPoint(0).x_);
      koords[(2 * i) + 1] = transY(pathElement(i).getPoint(0).y_);
    }
	}
	long pts = numberOfElementsInPath();
	for (int n = 0; n < pts - 1; n++)
		if ((aptlPoints[n].x == aptlPoints[n + 1].x)
			&& (aptlPoints[n].y == aptlPoints[n + 1].y)) {	// same coords
			if (pDrvMET->info)
				cerr << "melted points due to coord.-transform, removing..." << endl;
			memmove((void *) &aptlPoints[n], (void *) &aptlPoints[n + 1],
					sizeof(POINTL) * (pts - 1 - n));
			savedIsLineTo = aIsLineTo[n];
			memmove((void *) &aIsLineTo[n],
					(void *) &aIsLineTo[n + 1], sizeof(LONG) * (pts - 1 - n));
			/* because keep the owerwriten line-type */
			aIsLineTo[n] = savedIsLineTo;
			pts--;
		}
	return pts;
}

void drvMET::close_page()
{
	if (pDrvMET->info)
		cerr << "End page: " << currentPageNumber << endl;
}

void drvMET::open_page()
{
	LONG plOutCount;
	PBYTE pbOutData;
	long rc;

	plOutCount = 0;
	pbOutData = NULL;
	if (pDrvMET->info)
		cerr << "Start page: " << currentPageNumber << endl;
	if (currentPageNumber > 1) {
		rc = DevEscape(hdc, DEVESC_NEWFRAME, 0L, NULL, &plOutCount, pbOutData);
		if (rc == DEVESC_ERROR)
			cerr << "New page error!" << endl;
		if (rc == DEVESC_NOTIMPLEMENTED)
			cerr << "WARNING: New page not implemented!" << endl;
		if ((rc == DEV_OK) && (pDrvMET->info))
			cerr << "New page ok" << endl;
	}
}

void drvMET::show_text(const TextInfo & textinfo)
{
  // void drvMET::show_textstring(float x, float y, const char *const thetext) {
	SIZEF SizeBox;
	POINTL ptl;
	GRADIENTL grad;

	if (pDrvMET->draw_noText)
		return;
	setColor(textinfo.currentR, textinfo.currentG, textinfo.currentB);
	if (strcmp(lastSelectedFontName, textinfo.currentFontName.value()))
		FetchFont(FONT_ID, textinfo.currentFontName.value(), 0L, &pfat);
	strcpy(lastSelectedFontName, textinfo.currentFontName.value());
	SizeBox.cx = MAKEFIXED((textinfo.currentFontSize * scale), 0);
	SizeBox.cy = MAKEFIXED((textinfo.currentFontSize * scale), 0);
	const float toRadians = 3.14159265359 / 180.0;
	grad.x = (long) (100 * cos(textinfo.currentFontAngle * toRadians));
	grad.y = (long) (100 * sin(textinfo.currentFontAngle * toRadians));
	GpiSetCharAngle(hps, &grad);
	GpiSetCharBox(hps, &SizeBox);
	GpiSetCharSet(hps, FONT_ID);
	ptl.x = transX(textinfo.x);
	ptl.y = transY(textinfo.y);
	GpiCharStringAt(hps, &ptl, strlen(textinfo.thetext.value()),
					(const unsigned char *) textinfo.thetext.value());
}

void drvMET::drawPoly(int cntPoints, PPOINTL aptlPoints, PLONG aIsLineTo, int closed)
{
	POLYGON polygon;
	LONG ptsFollow, width, startLinePoint, endPoint, toMovePoint;

	if (pDrvMET->draw_noGraphic)
		return;
	setColor(currentR(), currentG(), currentB());
	if (!cntPoints) {			// transfer from pNumbers
		aptlPoints = (PPOINTL) malloc(8 * numberOfElementsInPath());
		if (!aptlPoints)
			cerr << "ERROR: Can't alloc memory for point-array (1)" << endl;
		aIsLineTo = (PLONG) malloc(4 * numberOfElementsInPath());
		if (!aIsLineTo)
			cerr << "ERROR: Can't alloc memory for point-array (2)" << endl;
		ptsFollow = get_print_coords(aptlPoints, aIsLineTo) - 1;
	} else
		ptsFollow = cntPoints - 1;
	polygon.ulPoints = ptsFollow;
	polygon.aPointl = &aptlPoints[1];
	int filled = (currentShowType() == fill)
		|| (currentShowType() == eofill);
	if (pDrvMET->info) {
		if (currentShowType() == eofill)
			cerr << "Poly-ShowType: " << "eof-fill" << endl;
		else
			cerr << "Poly-ShowType: " << currentShowType() << endl;
	}
	if (filled && (!pDrvMET->draw_noFill) && (ptsFollow > 1)) {
		if (GPI_ERROR == GpiMove(hps, &aptlPoints[0]))
			cerr << "ERROR: GpiMove: " << aptlPoints[0].x << "," << aptlPoints[0].y << endl;
		if (GPI_ERROR ==
			GpiPolygons(hps, 1, &polygon,
						POLYGON_NOBOUNDARY | ((currentShowType() == fill) ?
											  POLYGON_WINDING : POLYGON_ALTERNATE), POLYGON_EXCL)) {
			cerr << "ERROR: GpiPolygons: " << endl;
			for (int i = 0; i < ptsFollow; i++)
				cerr << aptlPoints[i + 1].x << "," << aptlPoints[i + 1].y << endl;
		}
	} else {
		if (!pDrvMET->draw_noPath) {
			width = (int) (((float) currentLineWidth()) * 2.0 * scale);
			if (GPI_ERROR == GpiSetLineWidthGeom(hps, width))
				cerr << "ERROR: GpiSetLineWidthGeom: " << width << endl;
			if (GPI_ERROR == GpiBeginPath(hps, 1L))
				cerr << "ERROR: GpiBeginPath" << endl;
		}
		endPoint = 0;
		if (moveto != aIsLineTo[endPoint])
			cerr << "*** no first moveto !!! *** " << endl;
		do {
			while ((endPoint < ptsFollow)
				   && (moveto == aIsLineTo[endPoint])) {
				/* skip to a lineto (or end of path),
				   this is done at least once: aIsLineTo[0] is moveto */
				endPoint++;
			}
			startLinePoint = endPoint;
			toMovePoint = startLinePoint - 1;
			if (GPI_ERROR == GpiMove(hps, &aptlPoints[toMovePoint]))
				cerr << "ERROR: GpiMove: " << aptlPoints[toMovePoint].x <<
					"," << aptlPoints[toMovePoint].y << endl;
			while ((endPoint < ptsFollow)
				   && (lineto == aIsLineTo[endPoint + 1]))
				/* extend lineto till any moveto (or end of path) */
				endPoint++;
			if (endPoint >= startLinePoint) {	/* if lines are to be drawn */
				if (GPI_ERROR ==
					GpiPolyLine(hps, endPoint - startLinePoint + 1, &aptlPoints[startLinePoint])) {
					cerr << "ERROR: GpiPolyLine: " << endl;
					for (int i = startLinePoint; i < endPoint + 1; i++)
						cerr << aptlPoints[i].x << "," << aptlPoints[i].y << endl;
				}
				endPoint++;
			}
		} while (endPoint < ptsFollow);
		if (!pDrvMET->draw_noPath) {
			if (GPI_ERROR == GpiEndPath(hps))
				cerr << "ERROR: GpiBeginPath" << endl;
			if (GPI_ERROR == GpiSetLineJoin(hps, LINEJOIN_MITRE))
				cerr << "ERROR: GpiSetLineJoin" << endl;
			if (GPI_ERROR == GpiSetLineEnd(hps, LINEEND_FLAT))
				cerr << "ERROR: GpiSetLineEnd" << endl;
			if (GPI_ERROR == GpiStrokePath(hps, 1L, 0L))
				cerr << "ERROR: GpiStrokePath" << endl;
		}
	}
	if (!cntPoints)
		free((PVOID) aptlPoints);
	if (!cntPoints)
		free((PVOID) aIsLineTo);
}

void drvMET::show_path()
{
	PPOINTL pPoints = NULL;
	PLONG pIsLineTo = NULL;

	if (pDrvMET->info)
		cerr << "Path-ShowType: " << currentShowType() << endl;
	drawPoly(READ_pNumers, pPoints, pIsLineTo, (isPolygon()? DRAW_CLOSED : DRAW_OPEN));
};

void drvMET::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
	POINTL Points[4];
	LONG isLineTo[4];

	Points[0].x = transX(llx);
	Points[0].y = transY(lly);
	isLineTo[0] = 1;
	Points[1].x = transX(urx);
	Points[1].y = transY(lly);
	isLineTo[1] = 1;
	Points[2].x = transX(urx);
	Points[2].y = transY(ury);
	isLineTo[2] = 1;
	Points[3].x = transX(llx);
	Points[3].y = transY(ury);
	isLineTo[3] = 1;
	if (pDrvMET->info)
		cerr << "Rectangle-ShowType: " << currentShowType() << endl;
	drawPoly(4L, &Points[0], &isLineTo[0], DRAW_CLOSED);
}

void drvMET::show_image(const Image & image)
{
	// first retrieve bounding box
	Point lowerLeft, upperRight;
	image.getBoundingBox(lowerLeft, upperRight);

	//  cerr << "bounding box of image:" << lowerLeft.x_ << "," << lowerLeft.y_ 
	//       << "," << upperRight.x_ << "," << upperRight.y_ << endl;

	// not only bounding box must account for scale,
	// but also transformation matrix!

	// scale bounding box
	lowerLeft.x_ *= scale;
	lowerLeft.y_ *= scale;
	upperRight.x_ *= scale;
	upperRight.y_ *= scale;

	const long width = abs(l_transX(upperRight.x_) - l_transX(lowerLeft.x_));
	const long height = abs(l_transY(upperRight.y_) - l_transY(lowerLeft.y_));

	// calculate bounding box
	//
	const int xMin = (int) min(l_transX(upperRight.x_), l_transX(lowerLeft.x_));
	const int xMax = (int) max(l_transX(upperRight.x_), l_transX(lowerLeft.x_));
	const int yMin = (int) min(l_transY(upperRight.y_), l_transY(lowerLeft.y_));
	const int yMax = (int) max(l_transY(upperRight.y_), l_transY(lowerLeft.y_));

	if (minStatus) {
		if (xMin < minX)
			minX = xMin;

		if (yMin < minY)
			minY = yMin;
	} else {
		minX = xMin;
		minY = yMin;
		minStatus = 1;
	}

	if (maxStatus) {
		if (xMax > maxX)
			maxX = xMax;

		if (yMax > maxY)
			maxY = yMax;
	} else {
		maxX = xMax;
		maxY = yMax;
		maxStatus = 1;
	}

	// calc long-padded size of scanline 
	const long scanlineLen = ((width * 3) + 3) & ~3L;

	// now lets get some mem
	unsigned char *const output = new unsigned char[scanlineLen * height];

	for (long i = 0; i < scanlineLen * height; i++)
		output[i] = 255;		// default is background (white)    

	if (!output) {
		errf << "ERROR: Cannot allocate memory for image" << endl;
		return;
	}
	// setup inverse transformation matrix (scaled, too!)
	const float matrixScale(image.normalizedImageCurrentMatrix[0] *
							image.normalizedImageCurrentMatrix[3] -
							image.normalizedImageCurrentMatrix[2] *
							image.normalizedImageCurrentMatrix[1]);

	const float inverseMatrix[] = {
		image.normalizedImageCurrentMatrix[3] / matrixScale / scale,
		-image.normalizedImageCurrentMatrix[1] / matrixScale / scale,
		-image.normalizedImageCurrentMatrix[2] / matrixScale / scale,
		image.normalizedImageCurrentMatrix[0] / matrixScale / scale,
		(image.normalizedImageCurrentMatrix[2] *
		 image.normalizedImageCurrentMatrix[5] -
		 image.normalizedImageCurrentMatrix[4] *
		 image.normalizedImageCurrentMatrix[3]) / matrixScale,
		(image.normalizedImageCurrentMatrix[4] *
		 image.normalizedImageCurrentMatrix[1] -
		 image.normalizedImageCurrentMatrix[0] *
		 image.normalizedImageCurrentMatrix[5]) / matrixScale
	};

	// now transform image
	for (long y = 0; y < height; y++) {
		// buffer current output scanline (saves us some multiplications)
		unsigned char *const currOutput = &output[scanlineLen * y];

		for (long x = 0; x < width; x++) {
			// now transform from device coordinate space to image space

			// apply transformation
			const Point currPoint = Point(x + lowerLeft.x_,
										  y + lowerLeft.y_).transform(inverseMatrix);

			// round to integers
			const long sourceX = (long) (currPoint.x_ + .5);
			const long sourceY = (long) (currPoint.y_ + .5);

			// is the pixel out of bounds? If yes, no further processing necessary
			if (sourceX >= 0L && (unsigned long) sourceX < image.width &&
				sourceY >= 0L && (unsigned long) sourceY < image.height) {
				// okay, fetch source pixel value into 
				// RGB triplet

				unsigned char r(255), g(255), b(255), c, m, y, k;

				// how many components?
				switch (image.ncomp) {
				case 1:
					r = g = b = image.getComponent(sourceX, sourceY, 0);
					break;

				case 3:
					r = image.getComponent(sourceX, sourceY, 0);
					g = image.getComponent(sourceX, sourceY, 1);
					b = image.getComponent(sourceX, sourceY, 2);
					break;

				case 4:
					c = image.getComponent(sourceX, sourceY, 0);
					m = image.getComponent(sourceX, sourceY, 1);
					y = image.getComponent(sourceX, sourceY, 2);
					k = image.getComponent(sourceX, sourceY, 3);

					// account for key
					c += k;
					m += k;
					y += k;

					// convert color
					r = 255 - c;
					g = 255 - m;
					b = 255 - y;
					break;

				default:
					errf << "\t\tFatal: unexpected case in drvMET (line "
						<< __LINE__ << ")" << endl;
					abort();
					return;
				}

				// set color triple
				currOutput[3 * x] = b;
				currOutput[3 * x + 1] = g;
				currOutput[3 * x + 2] = r;
			}
		}
	}

	// draw OS/2 PM DI bitmap
	PBITMAPINFO2 bmi;
	long lhits;

	if ((bmi = new BITMAPINFO2)) {
		/* setup BITMAPINFOHEADER2 */
		bmi->cbFix = 16;		//sizeof(BITMAPINFOHEADER2);
		bmi->cx = width;
		bmi->cy = height;
		bmi->cPlanes = 1;
		bmi->cBitCount = 24;	// always truecolor output

		//      cerr << "bm2: w x h=" << width << "," << height << endl;
		/*
		   3              1
		   src          target
		   2              0

		 */

		POINTL aptl[4] = {
			{l_transX(lowerLeft.x_),
			 l_transY(lowerLeft.y_)}
			,					/*lower-left corner of target */
			{l_transX(upperRight.x_),
			 l_transY(upperRight.y_)}
			,					/* upper-right corner of target */
			{0, 0}
			,
			{width, height}
		};						/* lower-left corner of source             */

		lhits = GpiDrawBits(hps, (VOID *) output, bmi, 4L, aptl, ROP_SRCCOPY, BBO_IGNORE);

		//      cerr << "bm3a:" << lhits << endl;

		if (lhits >= 0) {
			delete bmi;
			delete[]output;

			// done
			return;
		}

		delete bmi;

	}


	delete[]output;

	errf << "ERROR: Cannot draw bitmap" << endl;

}


static DriverDescriptionT < drvMET > D_met ("met", "OS/2 metafile", "met", true,	// backend supports subpathes
										   // if subpathes are supported, the backend must deal with
										   // sequences of the following form
										   // moveto (start of subpath)
										   // lineto (a line segment)
										   // lineto 
										   // moveto (start of a new subpath)
										   // lineto (a line segment)
										   // lineto 
										   //
										   // If this argument is set to false each subpath is drawn 
										   // individually which might not necessarily represent
										   // the original drawing.
										   false,	// backend does not support curves
										   true,	// backend supports elements which are filled and have edges 
										   true,	// backend supports text
										   true,	// backend supports Images
					   false, // backend supports png Images
										   DriverDescription::noopen,	// we open output file ourselves
										   false,	// if format supports multiple pages in one file (DEFINETELY not) 
										   false, /*clipping */
										   nodriverspecificoptions);


#endif
 
