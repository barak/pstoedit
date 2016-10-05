#ifdef __OS2__
// this file compiles only under OS/2
/*
   drvMET.cpp : This file is part of pstoedit
   Backend for OS/2 Meta Files.
   Contributed by : Christoph Jaeschke (jaeschke@imbe05.imbe.uni-bremen.de)

   Copyright (C) 1993,1994,1995,1996,1997 Wolfgang Glunz, Wolfgang.Glunz@mchp.siemens.de

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
HMQ  hmq;
HWND hwndClient;
HWND hwndFrame;
QMSG qmsg;
DRVMETSETUP *pDrvMETsetup;

void strRemove(char *toRemoveStr, char *pRemoves) {

  int length = strlen(toRemoveStr);

  for (int i=0; i<length;i++)
    if (strchr(pRemoves,toRemoveStr[i]))
      {
	memmove((void*)&toRemoveStr[i],
		(void*)&toRemoveStr[i+1],length-1-i);
	toRemoveStr[--length] = '\0';
      }
}

FONTMAPENTRY::FONTMAPENTRY(char* gsInitName, char* pmInitName) {

  strcpy(gsName,gsInitName);
  strcpy(pmName,pmInitName);
}

INTFONTMAP::INTFONTMAP(ostream *pOutstream) {

  pOutf = pOutstream;

  cntMapEntries = 0;
  if (NULL!=(fontmapFile = scanEnv(GSFONTMAPENV, GSFONTMAPFILE)) ) {
    readFontmap();
    if (pDrvMETsetup->info) {
      *pOutf << endl << "Extracted FONTMAP table:" << endl;
      for(int i=0; i<cntMapEntries; i++)
	*pOutf << mapEntry[i]->getgsName() << "->" << mapEntry[i]->getpmName() << endl;
      *pOutf << "Fontmap table end." << endl << endl;
    }
  }
}
INTFONTMAP::~INTFONTMAP() {

  for(int i=0; i<cntMapEntries; i++) {
    delete mapEntry[i];
  }
}

const char* INTFONTMAP::searchMapEntry(const char *gsName) {

  for(int i=0; i<cntMapEntries; i++)
    if (!strcmp(gsName,mapEntry[i]->getgsName()))
      return mapEntry[i]->getpmName(); // Found!
  return gsName;
}

void INTFONTMAP::addEntry(char *pLine) {

  char *gsName;
  char *pmName;

  if (cntMapEntries < maxMapEntries) {
    gsName = strtok(pLine," \t"); // First Token
    pmName = strtok(NULL," \t");	// Second Token
    if (pmName)
      if (pmName[0]=='/') {		// Thats an alias
	mapEntry[cntMapEntries++] = new FONTMAPENTRY(&gsName[1],&pmName[1]);
	// Now: gsName is an alias for pmName!
	// First search existing alias in List and replace
	// match with further alias
	for(int i=0; i<cntMapEntries-1; i++)
	  if (!strcmp(mapEntry[i]->getpmName(),&gsName[1]))
	    mapEntry[i]->reAllocpmName(&pmName[1]);
      }
  } else *pOutf << "WARNING: Max. number of FONTMAP-Entries succeeded: " << cntMapEntries << endl;
}

void INTFONTMAP::readFontmap() {

  const int maxLine=255;
  char pLine[maxLine];
  char c;

  ifstream mapf(fontmapFile);
  while (mapf.get(pLine,maxLine,'\n')) {
    mapf.get(c);
    if (pLine[0]=='/') addEntry(pLine);
  }
  mapf.close();
}

char* INTFONTMAP::scanEnv(char* envName, char* fileName) {

  PCXCHAR resPointer;
  static BYTE      resBuffer[128];
  APIRET    rc;

  if(!(rc=DosScanEnv((PXCHAR)envName, &resPointer))) {
    if (pDrvMETsetup->info)
      *pOutf << envName << " is " << resPointer << endl;
  } else { 		// not found
    *pOutf << "WARNING: " << envName << " not set, rc=" << rc << endl;
    return (char*) 0;
  }   if(!(rc=DosSearchPath(SEARCH_ENVIRONMENT,(PXCHAR)envName,
			    (PXCHAR)fileName,resBuffer,sizeof(resBuffer)))) {
    if (pDrvMETsetup->info)
      *pOutf << "Found desired file: "<< resBuffer << endl;
    return (char*) resBuffer;
  }
  else { 		// not found
    *pOutf << "WARNING: Desired file " << fileName << " in " << resPointer
	   << " not found, rc=" << rc << endl;
    return (char*) 0;
  }
}

drvMET::drvMET(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream): 
drvbase(driveroptions_p,theoutStream,theerrStream,1,0,0) {

  long cFonts;
  SIZEL size={0,0};
  const char *dData[2] =  { 0, "DISPLAY"};
  
  y_offset = 0;
  x_offset = 0;
  cntFonts = 0;
  scale = 1; // 4.0 for PU_HIENGLISH
  showFontList = 1;
  switch (pDrvMETsetup->draw_target) {
  case DRVMETSETUP::to_WINDOW : {
    hps = WinGetPS(hwndClient);
    GpiErase(hps);
    scale = 1;
    break;
  }
  case DRVMETSETUP::to_META : {
    hdc = DevOpenDC(hab, OD_METAFILE, (PSZ)"*", 2L, (PDEVOPENDATA)dData, 0L);
    if(!hdc) outf << "ERROR: Can't open DC!"	<< endl;
    hps = GpiCreatePS(hab, hdc, &size, PU_PELS|GPIF_DEFAULT|GPIT_MICRO|GPIA_ASSOC);
    if(!hps) outf << "ERROR: Can't open PS!"	<< endl;;
    break;
  }
  }
  DevQueryCaps(GpiQueryDevice(hps), CAPS_COLOR_INDEX, 1L, &maxPalEntries);

  alTable = new ULONG[maxPalEntries];

  long mp = maxPalEntries;
  for(int i=0; i<maxPalEntries;i++) alTable[i]= palEntry(i/mp, i/mp, i/mp);
  alTable[0] = palEntry(1,1,1);
  alTable[1] = palEntry(0,0,0);
  cntPalEntries = 2;
  outf.seekp(0);
  if (pDrvMETsetup->info)
    outf << "Open driver for: " << pDrvMETsetup->pMetaFileName << endl;
  if (!GpiCreateLogColorTable(hps,LCOL_PURECOLOR,LCOLF_CONSECRGB,0L,maxPalEntries-1,(long*)alTable))
    outf << "ERROR: Can't update logical palette!"	<< endl;
  if (!GpiSetColor(hps,1))
    outf << "ERROR: Can't set color: 1" << endl;

  // Now Count and retrieve Fontinformations

  pIntFontmap = new INTFONTMAP(&outf);

  cFonts = GpiQueryFonts (hps, QF_PUBLIC,(PSZ) NULL, &cntFonts,
			  sizeof (FONTMETRICS),(PFONTMETRICS) NULL);
  cntFonts = cFonts;
  
  pfm = (PFONTMETRICS) malloc(sizeof (FONTMETRICS) * cFonts);
  if (!pfm)
    outf << "ERROR: Can't alloc memory for fontmetrics" << endl;
  GpiQueryFonts (hps, QF_PUBLIC,(PSZ) NULL,
		 &cntFonts, sizeof (FONTMETRICS),pfm);
  const char * const defaultfontname = "TimesNewRoman";
  setCurrentFontName(defaultfontname,false);
  FetchFont (FONT_ID,defaultfontname, 0L, &pfat);
  strcpy(lastSelectedFontName,defaultfontname);
}

drvMET::~drvMET() {

  HMF hmf;

  free(pfm);
  switch (pDrvMETsetup->draw_target) {
  case DRVMETSETUP::to_WINDOW : {
    WinReleasePS(hps);
    break;
  }
  case DRVMETSETUP::to_META : {
    GpiAssociate(hps, NULLHANDLE);
    if(!GpiDestroyPS(hps))
      outf << "ERROR: Can't destroy PS" << endl;
    hmf = (HMF)DevCloseDC(hdc);
    DosDelete((PXCHAR)pDrvMETsetup->pMetaFileName);
    if(GpiSaveMetaFile(hmf,(PXCHAR) pDrvMETsetup->pMetaFileName)) {
      if (pDrvMETsetup->info) outf << "Metafile saved!" << endl;
    } else
      outf << "ERROR: Can't save metafile." << endl;
    GpiDeleteMetaFile(hmf);
    break;
  }
  }
  delete alTable;
  delete pIntFontmap;
}

long drvMET::palEntry(float r, float g, float b) {

  long lr, lg, lb;

  lr = (long)(r*255);
  lg = (long)(g*255);
  lb = (long)(b*255);
  if (pDrvMETsetup->draw_noColor) lr = lg = lb = (lr + lg + lb ) / 3;
  return (/*((PC_RESERVED | PC_EXPLICIT )*16777216)*/ + (lr*65536) + (lg*256) + lb);
}

long drvMET::searchPalEntry(long palEntry) {

  for(int i=0; i<cntPalEntries; i++)
    if (palEntry==alTable[i]) return i; // Found!
  return -1;                              // Not found!
}

void drvMET::setColor(float r, float g, float b) {

  long index;

  index = searchPalEntry(palEntry(r,g,b));
  if (index < 0) {		// Not found!
    if (cntPalEntries<maxPalEntries) {
      alTable[cntPalEntries]=palEntry(r,g,b);
      if (GpiCreateLogColorTable(hps,LCOL_PURECOLOR,LCOLF_CONSECRGB,0L,maxPalEntries,(long*)alTable)) {
	if (pDrvMETsetup->info)
	  outf << "Added Colorindex: " << cntPalEntries << endl;
      } else
	outf << "ERROR: Can't update logical palette!" << endl;
      if (!GpiSetColor(hps, cntPalEntries))
	outf << "ERROR: Can't set color: " << cntPalEntries << endl;
      cntPalEntries++;
    } else outf << "WARNING: Max. number of colorentries ("<< maxPalEntries <<") exceeded!" << endl;
  } else 	// Found!
    if (!GpiSetColor(hps, index))
      outf << "ERROR: Can't set color: " << index << endl;
}

int drvMET::FetchFont (SHORT idFont, const char * pFamilyName, long selection, PFATTRS *pfat) {

  FATTRS	fat;
  PFONTMETRICS	pFontMetrics;
  APIRET rc;
  long	i;
  long tableEntry;
  const char *pOrgFamilyName;
  char 	lastOutfName[FACESIZE];
  char 	tmpIsName[FACESIZE];
  char 	tmpSearchName[FACESIZE];

  pOrgFamilyName = pFamilyName;
  pFamilyName = pIntFontmap->searchMapEntry(pFamilyName);
  if (pDrvMETsetup->info) {
    outf << "Font requested: ";
    if (pOrgFamilyName != pFamilyName)
      outf << pOrgFamilyName << ", FONTMAP maps to: ";
    outf << pFamilyName << endl;
  }
  strcpy(tmpSearchName,pFamilyName);
  strRemove(tmpSearchName," -");
  lastOutfName[0] = '\0';
  if (showFontList) {
    if (pDrvMETsetup->info) outf << "Table of installed PM-Fonts: " << endl;
    tableEntry = 0;
    pFontMetrics = pfm;
    for (i=0; i<cntFonts; i++) {
      if (strcmp(pFontMetrics->szFacename,lastOutfName)) {
	if (pDrvMETsetup->info)
	  outf << ++tableEntry << ": " << pFontMetrics->szFacename << endl;
	strcpy(lastOutfName,pFontMetrics->szFacename);
      }
      pFontMetrics++;
    }
    if (pDrvMETsetup->info) outf << "End Table of PM-Fonts." << endl;
  }
  showFontList = 0;	// Show only once
  pFontMetrics = pfm;
  for (i=0; i<cntFonts; i++) {   // Real search
    strcpy(tmpIsName,pFontMetrics->szFacename);
    strRemove(tmpIsName," -");
    if (!strcmp(tmpIsName,tmpSearchName)) break; // Font found in List!
    pFontMetrics++;
  }
  if (i != cntFonts) {
    strcpy (fat.szFacename, pFontMetrics->szFacename);
    fat.usRecordLength = sizeof(FATTRS); /* sets size of structure   */
    fat.fsSelection = (USHORT)selection;
    fat.lMatch = pFontMetrics->lMatch;
    fat.idRegistry = 0;          /* uses default registry            */
    fat.usCodePage = 0;          /* code-page 0                      */
    fat.lMaxBaselineExt = 0L;   /* requested default font height */
    fat.lAveCharWidth = 0L;     /* requested default font width */
    fat.fsType = 0;              /* uses default type                */
    fat.fsFontUse = FATTR_FONTUSE_OUTLINE;/* outline font */
    rc = GpiCreateLogFont (hps,NULL,idFont,&fat);
    *pfat = &fat;
    if (pDrvMETsetup->info) {
      outf << "Found PM name: " << pFontMetrics->szFacename
	   << " (attributes: " << selection << ")" << endl;
      outf << "CreateFont (";
      if (rc==FONT_MATCH) outf << "selected)" << endl;
      if (rc==FONT_DEFAULT) outf << "not selected -> defaultet)" << endl;
    }
    if (rc==GPI_ERROR) {
      outf << "ERROR: Found PM name: " << pFontMetrics->szFacename
	   << ", but CreateFont yields to GPI_ERROR)" << endl;
      return FALSE;
    }
    return TRUE;
  } else {
    outf << "WARNING: Font request: " << pFamilyName<< " not found!" << endl;
    return 0;
  }
}

long drvMET::get_print_coords(POINTL aptlPoints[], LONG aIsLineTo[]) {
  PLONG koords = (PLONG) aptlPoints;
  LONG  lastMovePoint, savedIsLineTo;
  for (int i = 0; i < numberOfElementsInPath(); i++) {
    koords[2*i] = transX(pathElement(i).getPoint(0).x_);
    koords[(2*i)+1] = transY(pathElement(i).getPoint(0).y_);
    aIsLineTo[i] = pathElement(i).getType();
    if (aIsLineTo[i]==moveto)
      /* mark the last moveto for a closepath operation */
      lastMovePoint = i;
    if (aIsLineTo[i]==closepath) {
      /* closepath's seems to have no valid koords,
	 substituted here by the last moveto-koord's,
	 finally settings the line-type to move */
      koords[2*i] = koords[2*lastMovePoint];
      koords[(2*i)+1] = koords[(2*lastMovePoint)+1];
      aIsLineTo[i] = lineto;
    }
  }
  long pts = numberOfElementsInPath();
  for (int n = 0; n < pts-1; n++)
    if ((aptlPoints[n].x == aptlPoints[n+1].x)
	&& (aptlPoints[n].y == aptlPoints[n+1].y)) {// same coords
      if (pDrvMETsetup->info) outf << "melted points due to coord.-transform, removing..." << endl;
      memmove((void*)&aptlPoints[n],
	      (void*)&aptlPoints[n+1],sizeof(POINTL) * (pts-1-n));
      savedIsLineTo = aIsLineTo[n];
      memmove((void*)&aIsLineTo[n],
	      (void*)&aIsLineTo[n+1], sizeof(LONG) * (pts-1-n));
      /* because keep the owerwriten line-type */
      aIsLineTo[n] = savedIsLineTo; 
      pts--;
    }
  return pts;
}

void drvMET::close_page() {
  if (pDrvMETsetup->info) outf << "End page: " << currentPageNumber << endl;
}

void drvMET::open_page() {
  LONG  plOutCount;
  PBYTE pbOutData;
  long rc;

  plOutCount = 0;
  pbOutData = NULL;
  if (pDrvMETsetup->info) outf << "Start page: " << currentPageNumber << endl;
  if (currentPageNumber > 1) {
    rc = DevEscape(hdc, DEVESC_NEWFRAME, 0L, NULL, &plOutCount, pbOutData);
    if (rc==DEVESC_ERROR) outf << "New page error!" << endl;
    if (rc==DEVESC_NOTIMPLEMENTED) outf << "WARNING: New page not implemented!" << endl;
    if ((rc==DEV_OK) && (pDrvMETsetup->info)) outf << "New page ok" << endl;
  }
}

void    drvMET::show_text(const TextInfo & textinfo) {
// void drvMET::show_textstring(float x, float y, const char *const thetext) {
  SIZEF SizeBox;
  POINTL ptl;
  GRADIENTL grad;

  if (pDrvMETsetup->draw_noText) return;
  setColor(textinfo.currentR,textinfo.currentG,textinfo.currentB);
  if (strcmp(lastSelectedFontName,textinfo.currentFontName.value()))
    FetchFont (FONT_ID, textinfo.currentFontName.value() , 0L, &pfat);
  strcpy(lastSelectedFontName,textinfo.currentFontName.value());
  SizeBox.cx = MAKEFIXED ((textinfo.currentFontSize*scale),0);
  SizeBox.cy = MAKEFIXED ((textinfo.currentFontSize*scale),0);
  const float toRadians = 3.14159265359 / 180.0;
  grad.x = (long) (100 * cos(textinfo.currentFontAngle * toRadians ));
  grad.y = (long) (100 * sin(textinfo.currentFontAngle * toRadians ));
  GpiSetCharAngle(hps, &grad);
  GpiSetCharBox (hps,&SizeBox);
  GpiSetCharSet (hps,FONT_ID);
  ptl.x = transX(textinfo.x);
  ptl.y = transY(textinfo.y);
  GpiCharStringAt(hps,&ptl,strlen(textinfo.thetext),(PCH)textinfo.thetext);
}

void drvMET::drawPoly(int cntPoints, PPOINTL aptlPoints, PLONG aIsLineTo, int closed) {
  POLYGON polygon;
  LONG ptsFollow, width, startLinePoint, endPoint, toMovePoint;

  if (pDrvMETsetup->draw_noGraphic) return;
  setColor(currentR(),currentG(),currentB());
  if (!cntPoints) {		// transfer from pNumbers
    aptlPoints = (PPOINTL) malloc(8*numberOfElementsInPath());
    if (!aptlPoints)
      outf << "ERROR: Can't alloc memory for point-array (1)" << endl;
    aIsLineTo = (PLONG) malloc(4*numberOfElementsInPath());
    if (!aIsLineTo)
      outf << "ERROR: Can't alloc memory for point-array (2)" << endl;
    ptsFollow = get_print_coords( aptlPoints, aIsLineTo) -1;
  } else
    ptsFollow = cntPoints -1;
  polygon.ulPoints = ptsFollow;
  polygon.aPointl = &aptlPoints[1];
  int filled = (currentShowType()==fill) || (currentShowType()==eofill);
  if (pDrvMETsetup->info) {
    if (currentShowType()==eofill)
      outf << "Poly-ShowType: " << "eof-fill" <<  endl;
    else
      outf << "Poly-ShowType: " << currentShowType() << endl;
  }
  if (filled && (!pDrvMETsetup->draw_noFill) && (ptsFollow>1) ) {
    if (GPI_ERROR==GpiMove(hps,&aptlPoints[0]))
      outf << "ERROR: GpiMove: " << aptlPoints[0].x << "," << aptlPoints[0].y << endl;
    if (GPI_ERROR==GpiPolygons(hps,1,&polygon, POLYGON_NOBOUNDARY | 
			       ((currentShowType()==fill) ? POLYGON_WINDING : POLYGON_ALTERNATE),
			       POLYGON_EXCL )) {
      outf << "ERROR: GpiPolygons: " << endl;
      for(int i=0; i < ptsFollow; i++)
	outf << aptlPoints[i+1].x << "," << aptlPoints[i+1].y << endl;
    }
  }
  else {
    if (!pDrvMETsetup->draw_noPath) {
      width = (int)(((float)currentLineWidth())*2.0*scale);
      if (GPI_ERROR==GpiSetLineWidthGeom(hps, width))
	outf << "ERROR: GpiSetLineWidthGeom: " << width << endl;
      if (GPI_ERROR==GpiBeginPath(hps,1L))
	outf << "ERROR: GpiBeginPath" << endl;
    }
    endPoint = 0;
    if (moveto!=aIsLineTo[endPoint])
      outf << "*** no first moveto !!! *** "<< endl;
    do {
      while ((endPoint < ptsFollow) && (moveto==aIsLineTo[endPoint])) {
 	/* skip to a lineto (or end of path),
 	   this is done at least once: aIsLineTo[0] is moveto */
 	endPoint++;
      }
      startLinePoint = endPoint;
      toMovePoint = startLinePoint-1;
      if (GPI_ERROR==GpiMove(hps,&aptlPoints[toMovePoint]))
	outf << "ERROR: GpiMove: " << aptlPoints[toMovePoint].x <<
	  "," << aptlPoints[toMovePoint].y << endl;
      while ((endPoint < ptsFollow) && (lineto==aIsLineTo[endPoint+1]))
	/* extend lineto till any moveto (or end of path) */
	endPoint++;
      if (endPoint >= startLinePoint) { /* if lines are to be drawn */
	if (GPI_ERROR==GpiPolyLine(hps,endPoint-startLinePoint+1,&aptlPoints[startLinePoint])) {
	  outf << "ERROR: GpiPolyLine: " << endl;
	  for(int i=startLinePoint; i < endPoint+1; i++)
	    outf << aptlPoints[i].x << "," << aptlPoints[i].y << endl;
	}
	endPoint++;
      }
    } while (endPoint < ptsFollow);
    if (!pDrvMETsetup->draw_noPath) {
      if (GPI_ERROR==GpiEndPath(hps))
	outf << "ERROR: GpiBeginPath" << endl;
      if (GPI_ERROR==GpiSetLineJoin(hps,LINEJOIN_MITRE))
	outf << "ERROR: GpiSetLineJoin" << endl;
      if (GPI_ERROR==GpiSetLineEnd(hps,LINEEND_FLAT))
	outf << "ERROR: GpiSetLineEnd" << endl;
      if (GPI_ERROR==GpiStrokePath(hps,1L,0L))
	outf << "ERROR: GpiStrokePath" << endl;
    }
  }
  if (!cntPoints) free((PVOID)aptlPoints);
  if (!cntPoints) free((PVOID)aIsLineTo);
}

void drvMET::show_path() {
  PPOINTL pPoints = NULL;
  PLONG  pIsLineTo = NULL;

  if (pDrvMETsetup->info) outf << "Path-ShowType: " << currentShowType() << endl;
  drawPoly(READ_pNumers, pPoints, pIsLineTo, (isPolygon() ? DRAW_CLOSED : DRAW_OPEN));
};

void drvMET::show_rectangle(const float llx, const float lly, const float urx, const float ury) {
  POINTL Points[4];
  LONG  isLineTo[4];

  Points[0].x = transX(llx);Points[0].y = transY(lly);isLineTo[0] = 1;
  Points[1].x = transX(urx);Points[1].y = transY(lly);isLineTo[1] = 1;
  Points[2].x = transX(urx);Points[2].y = transY(ury);isLineTo[2] = 1;
  Points[3].x = transX(llx);Points[3].y = transY(ury);isLineTo[3] = 1;
  if (pDrvMETsetup->info) outf << "Rectangle-ShowType: " << currentShowType() << endl;
  drawPoly(4L, &Points[0], &isLineTo[0], DRAW_CLOSED);
}

ofstream os2win_outStream;

#ifndef __GNUG__
extern "OPTLINK" {
#endif

void OS2WIN_WM_COMMAND_THREAD(void *hwnd) {

  drvbase * outputdriver;

  outputdriver = new drvMET(0,os2win_outStream,cerr);
  {
  PSFrontEnd fe(cerr,*outputdriver);
  fe.run(0);
  }
  delete outputdriver;
  if (pDrvMETsetup->draw_target != DRVMETSETUP::to_WINDOW)
    WinPostMsg( (HWND)hwnd, WM_QUIT, (MPARAM)0,(MPARAM)0 );
  _endthread();
}
#ifndef __GNUG__
}
#endif

MRESULT EXPENTRY OS2WIN_WinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 ) {

  static int thread_id;

  switch( msg )
    {
    case WM_CREATE:
      {
	WinPostMsg( hwnd, WM_COMMAND, (MPARAM)0,(MPARAM)0 );
	break;
      }
    case WM_COMMAND: {
	thread_id = _beginthread(OS2WIN_WM_COMMAND_THREAD,NULL,32768,(void *)hwnd);
      if ( thread_id < 0)
	DosBeep(2000,2000);
      break;
    }
    case WM_CLOSE:
      WinPostMsg( hwnd, WM_QUIT, (MPARAM)0,(MPARAM)0 );
      break;
    case WM_DESTROY:
      DosWaitThread((PTID)&thread_id, DCWW_WAIT);
      break;
    default:
      return WinDefWindowProc( hwnd, msg, mp1, mp2 );
    }
  return (MRESULT)FALSE;
}

void OS2WIN::abort(HWND hwndFrame, HWND hwndClient) {

  PERRINFO  pErrInfoBlk;
  PSZ       pszOffSet;
  PSZ       pszErrMsg;
  void      stdprint(void);

  DosBeep(1000,1000);
  if ((pErrInfoBlk = WinGetErrorInfo(hab)) != (PERRINFO)NULL)
    {
      pszOffSet = ((PSZ)pErrInfoBlk) + pErrInfoBlk->offaoffszMsg;
      pszErrMsg = ((PSZ)pErrInfoBlk) + *((PSHORT)pszOffSet);
      if((INT)hwndFrame && (INT)hwndClient)
	WinMessageBox(HWND_DESKTOP,hwndFrame,(PSZ)pszErrMsg,
		      (PXCHAR)"Error from PS2MET",
		      MSGBOXID,MB_MOVEABLE | MB_CUACRITICAL | MB_CANCEL );
      WinFreeErrorInfo(pErrInfoBlk);
    }
  WinPostMsg(hwndClient, WM_QUIT, (MPARAM)NULL, (MPARAM)NULL);
}

OS2WIN::OS2WIN() {

  ULONG flCreate;

  os2win_outStream.open("drvMET.out");	// used only for errors & warnings
  if (os2win_outStream.fail() ) {
    cerr << "Could not open file " << "drvMET.out" << " for output" << endl;
    exit(1);
  }

  if ((hab = WinInitialize(0)) == 0L)
    abort(hwndFrame, hwndClient);
  if ((hmq = WinCreateMsgQueue( hab, 0 )) == 0L)
    abort(hwndFrame, hwndClient);
  if (!WinRegisterClass(hab,(PSZ)"PS2MET-Window",(PFNWP)OS2WIN_WinProc,
			CS_SIZEREDRAW,0))
    abort(hwndFrame, hwndClient);
  flCreate = 	FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | // FCF_SHELLPOSITION |
    FCF_TASKLIST ;
  long visible = ((pDrvMETsetup->draw_target == DRVMETSETUP::to_WINDOW)?WS_VISIBLE:0L);
  if ((hwndFrame = WinCreateStdWindow(HWND_DESKTOP,visible,&flCreate,
				      (PXCHAR)"PS2MET-Window",(PXCHAR)"PS to MET Converter", visible,
				      (HMODULE)0L, ID_WINDOW, &hwndClient )) == 0L)
    abort(hwndFrame, hwndClient);

  if (pDrvMETsetup->draw_target == DRVMETSETUP::to_WINDOW) {
    RECTL rect;
    WinQueryWindowRect(HWND_DESKTOP, &rect);
    WinSetWindowPos( hwndFrame, HWND_TOP, rect.xLeft+15, rect.yBottom+15,
		     rect.xRight-30,rect.yTop-30,
		     SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW );
  }
}

OS2WIN::~OS2WIN() {

  WinDestroyWindow(hwndFrame);
  WinDestroyMsgQueue( hmq );
  WinTerminate( hab );
  os2win_outStream.close();
}

void OS2WIN::run() {

  while( WinGetMsg( hab, &qmsg, 0L, 0, 0 ) )
    WinDispatchMsg( hab, &qmsg );
}

DRVMETSETUP::DRVMETSETUP(char* optstring) {

  draw_target = to_META;
  draw_noPath = false;
  draw_noFill = false;
  draw_noColor = false;
  draw_noText = false;
  draw_noGraphic = false;
  info = false;
  exit = false;
  if (optstring) {
    while (*(optstring) != '\0') {
      if (*optstring =='w') {
	draw_target = to_WINDOW;
      } else if (*optstring =='p') {
	draw_noPath = true;
      } else if (*optstring =='l') {
	draw_noFill = true;
      } else if (*optstring =='c') {
	draw_noColor = true;
      } else if (*optstring =='t') {
	draw_noText = true;
      } else if (*optstring =='g') {
	draw_noGraphic = true;
      } else if (*optstring =='v') {
	info = true;
      } else if ((*optstring =='?') || (*optstring =='h')) {
	exit = true;
      } else if (*optstring !=' ') {
	exit = true;
	cerr << "unknown option: " << *optstring << endl;
	cerr << endl;
      }
      optstring++;
    }
    if (exit) {
      cerr << "Metafile-Backend for pstoedit, (C) 1996 by Christoph Jaeschke" << endl;
      cerr << "email: jaeschke@item.uni-bremen.de" << endl;
      cerr << "w	Draw into an opened window instead of a metafile" << endl;
      cerr << "p	Draw no geometric linewidths, all lines with width 0" << endl;
      cerr << "l	No filling of polygon interiors" << endl;
      cerr << "c	No colors, only greyscales" << endl;
      cerr << "t	Draw no text" << endl;
      cerr << "g	Draw no graphics" << endl;
      cerr << "v	Verbose infos in drvMET.out" << endl;
    }
  }
}

#endif
