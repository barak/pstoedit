/*
   drvIDRAW.cpp : This file is part of pstoedit
   Backend for idraw files
   Contributed by: Scott Pakin <pakin@uiuc.edu>

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
#include "drvidraw.h"
#include I_fstream
#include I_stdio
#include I_stdlib
#include <math.h>


// Add a point unless it's the same as the previous point
// Used by drvIDRAW::print_coords() for convenience
#define ADDPOINT(X,Y) do {						\
  if (!newtotalpoints ||						\
      iscale(X) != iscale(newpointlist[newtotalpoints-1]->x_) ||	\
      iscale(Y) != iscale(newpointlist[newtotalpoints-1]->y_))		\
    newpointlist[newtotalpoints++] = new Point(X,Y);			\
} while (0)


drvIDRAW::derivedConstructor(drvIDRAW):
//(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream): // Constructor
#if 0
        drvbase(driveroptions_p,theoutStream,theerrStream,outPath_p,outName_p,
                0, // if backend supports subpathes, else 0
                   // if subpathes are supported, the backend must deal with
                   // sequences of the following form
                   // moveto (start of subpath)
                   // lineto (a line segment)
                   // lineto
                   // moveto (start of a new subpath)
                   // lineto (a line segment)
                   // lineto
                   //
                   // If this argument is set to 0 each subpath is drawn
                   // individually which might not necessarily represent
                   // the original drawing.

                1,  // if backend supports curves, else 0
                1   // if backend supports elements with fill and edges
			)
#endif
			constructBase

{
  IDRAW_SCALING = 0.799705f * scalefactor_p;
  // Initialize the color table
  color[0].red = 0;
  color[0].green = 0;
  color[0].blue = 0;
  color[0].name = "Black";

  color[1].red = 0.647059;
  color[1].green = 0.164706;
  color[1].blue = 0.164706;
  color[1].name = "Brown";

  color[2].red = 1;
  color[2].green = 0;
  color[2].blue = 0;
  color[2].name = "Red";

  color[3].red = 1;
  color[3].green = 0.647059;
  color[3].blue = 0;
  color[3].name = "Orange";

  color[4].red = 1;
  color[4].green = 1;
  color[4].blue = 0;
  color[4].name = "Yellow";

  color[5].red = 0;
  color[5].green = 1;
  color[5].blue = 0;
  color[5].name = "Green";

  color[6].red = 0;
  color[6].green = 0;
  color[6].blue = 1;
  color[6].name = "Blue";

  color[7].red = 0.74902;
  color[7].green = 0;
  color[7].blue = 1;
  color[7].name = "Indigo";

  color[8].red = 0.933333;
  color[8].green = 0.509804;
  color[8].blue = 0.933333;
  color[8].name = "Violet";

  color[9].red = 1;
  color[9].green = 1;
  color[9].blue = 1;
  color[9].name = "White";

  color[10].red = 0.764706;
  color[10].green = 0.764706;
  color[10].blue = 0.764706;
  color[10].name = "LtGray";

  color[11].red = 0.501961;
  color[11].green = 0.501961;
  color[11].blue = 0.501961;
  color[11].name = "DkGray";


  // Write idraw PostScript header
  outf << "%!PS-Adobe-2.0 EPSF-1.2\n";
  outf << "%%Creator: idraw\n";
  outf << "%%DocumentFonts: Times-Roman Times-Bold Times-Italic\n";
  outf << "%%+ Helvetica Helvetica-Bold Helvetica-Oblique Helvetica-BoldOblique\n";
  outf << "%%+ Courier Courier-Bold Courier-Oblique Courier-BoldOblique\n";
  outf << "%%+ Symbol\n";
  outf << "%%Pages: 1\n";
  outf << "%%BoundingBox: 0 0 612 792\n";    // Assume full page
  outf << "%%EndComments\n";
  outf << "\n";
  outf << "/arrowHeight 10 def\n";
  outf << "/arrowWidth 5 def\n";
  outf << "\n";
  outf << "/IdrawDict 51 dict def\n";
  outf << "IdrawDict begin\n";
  outf << "\n";
  outf << "/reencodeISO {\n";
  outf << "dup dup findfont dup length dict begin\n";
  outf << "{ 1 index /FID ne { def }{ pop pop } ifelse } forall\n";
  outf << "/Encoding ISOLatin1Encoding def\n";
  outf << "currentdict end definefont\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/ISOLatin1Encoding [\n";
  outf << "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n";
  outf << "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n";
  outf << "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n";
  outf << "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n";
  outf << "/space/exclam/quotedbl/numbersign/dollar/percent/ampersand/quoteright\n";
  outf << "/parenleft/parenright/asterisk/plus/comma/minus/period/slash\n";
  outf << "/zero/one/two/three/four/five/six/seven/eight/nine/colon/semicolon\n";
  outf << "/less/equal/greater/question/at/A/B/C/D/E/F/G/H/I/J/K/L/M/N\n";
  outf << "/O/P/Q/R/S/T/U/V/W/X/Y/Z/bracketleft/backslash/bracketright\n";
  outf << "/asciicircum/underscore/quoteleft/a/b/c/d/e/f/g/h/i/j/k/l/m\n";
  outf << "/n/o/p/q/r/s/t/u/v/w/x/y/z/braceleft/bar/braceright/asciitilde\n";
  outf << "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n";
  outf << "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n";
  outf << "/.notdef/dotlessi/grave/acute/circumflex/tilde/macron/breve\n";
  outf << "/dotaccent/dieresis/.notdef/ring/cedilla/.notdef/hungarumlaut\n";
  outf << "/ogonek/caron/space/exclamdown/cent/sterling/currency/yen/brokenbar\n";
  outf << "/section/dieresis/copyright/ordfeminine/guillemotleft/logicalnot\n";
  outf << "/hyphen/registered/macron/degree/plusminus/twosuperior/threesuperior\n";
  outf << "/acute/mu/paragraph/periodcentered/cedilla/onesuperior/ordmasculine\n";
  outf << "/guillemotright/onequarter/onehalf/threequarters/questiondown\n";
  outf << "/Agrave/Aacute/Acircumflex/Atilde/Adieresis/Aring/AE/Ccedilla\n";
  outf << "/Egrave/Eacute/Ecircumflex/Edieresis/Igrave/Iacute/Icircumflex\n";
  outf << "/Idieresis/Eth/Ntilde/Ograve/Oacute/Ocircumflex/Otilde/Odieresis\n";
  outf << "/multiply/Oslash/Ugrave/Uacute/Ucircumflex/Udieresis/Yacute\n";
  outf << "/Thorn/germandbls/agrave/aacute/acircumflex/atilde/adieresis\n";
  outf << "/aring/ae/ccedilla/egrave/eacute/ecircumflex/edieresis/igrave\n";
  outf << "/iacute/icircumflex/idieresis/eth/ntilde/ograve/oacute/ocircumflex\n";
  outf << "/otilde/odieresis/divide/oslash/ugrave/uacute/ucircumflex/udieresis\n";
  outf << "/yacute/thorn/ydieresis\n";
  outf << "] def\n";
  outf << "/Times-Roman reencodeISO def\n";
  outf << "/Times-Bold reencodeISO def\n";
  outf << "/Times-Italic reencodeISO def\n";
  outf << "/Times-BoldItalic reencodeISO def\n";
  outf << "/Helvetica reencodeISO def\n";
  outf << "/Helvetica-Bold reencodeISO def\n";
  outf << "/Helvetica-Oblique reencodeISO def\n";
  outf << "/Helvetica-BoldOblique reencodeISO def\n";
  outf << "/Courier reencodeISO def\n";
  outf << "/Courier-Bold reencodeISO def\n";
  outf << "/Courier-Oblique reencodeISO def\n";
  outf << "/Courier-BoldOblique reencodeISO def\n";
  outf << "/Symbol reencodeISO def\n";
  outf << "\n";
  outf << "/none null def\n";
  outf << "/numGraphicParameters 17 def\n";
  outf << "/stringLimit 65535 def\n";
  outf << "\n";
  outf << "/Begin {\n";
  outf << "save\n";
  outf << "numGraphicParameters dict begin\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/End {\n";
  outf << "end\n";
  outf << "restore\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/SetB {\n";
  outf << "dup type /nulltype eq {\n";
  outf << "pop\n";
  outf << "false /brushRightArrow idef\n";
  outf << "false /brushLeftArrow idef\n";
  outf << "true /brushNone idef\n";
  outf << "} {\n";
  outf << "/brushDashOffset idef\n";
  outf << "/brushDashArray idef\n";
  outf << "0 ne /brushRightArrow idef\n";
  outf << "0 ne /brushLeftArrow idef\n";
  outf << "/brushWidth idef\n";
  outf << "false /brushNone idef\n";
  outf << "} ifelse\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/SetCFg {\n";
  outf << "/fgblue idef\n";
  outf << "/fggreen idef\n";
  outf << "/fgred idef\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/SetCBg {\n";
  outf << "/bgblue idef\n";
  outf << "/bggreen idef\n";
  outf << "/bgred idef\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/SetF {\n";
  outf << "/printSize idef\n";
  outf << "/printFont idef\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/SetP {\n";
  outf << "dup type /nulltype eq {\n";
  outf << "pop true /patternNone idef\n";
  outf << "} {\n";
  outf << "dup -1 eq {\n";
  outf << "/patternGrayLevel idef\n";
  outf << "/patternString idef\n";
  outf << "} {\n";
  outf << "/patternGrayLevel idef\n";
  outf << "} ifelse\n";
  outf << "false /patternNone idef\n";
  outf << "} ifelse\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/BSpl {\n";
  outf << "0 begin\n";
  outf << "storexyn\n";
  outf << "newpath\n";
  outf << "n 1 gt {\n";
  outf << "0 0 0 0 0 0 1 1 true subspline\n";
  outf << "n 2 gt {\n";
  outf << "0 0 0 0 1 1 2 2 false subspline\n";
  outf << "1 1 n 3 sub {\n";
  outf << "/i exch def\n";
  outf << "i 1 sub dup i dup i 1 add dup i 2 add dup false subspline\n";
  outf << "} for\n";
  outf << "n 3 sub dup n 2 sub dup n 1 sub dup 2 copy false subspline\n";
  outf << "} if\n";
  outf << "n 2 sub dup n 1 sub dup 2 copy 2 copy false subspline\n";
  outf << "patternNone not brushLeftArrow not brushRightArrow not and and { ifill } if\n";
  outf << "brushNone not { istroke } if\n";
  outf << "0 0 1 1 leftarrow\n";
  outf << "n 2 sub dup n 1 sub dup rightarrow\n";
  outf << "} if\n";
  outf << "end\n";
  outf << "} dup 0 4 dict put def\n";
  outf << "\n";
  outf << "/Circ {\n";
  outf << "newpath\n";
  outf << "0 360 arc\n";
  outf << "patternNone not { ifill } if\n";
  outf << "brushNone not { istroke } if\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/CBSpl {\n";
  outf << "0 begin\n";
  outf << "dup 2 gt {\n";
  outf << "storexyn\n";
  outf << "newpath\n";
  outf << "n 1 sub dup 0 0 1 1 2 2 true subspline\n";
  outf << "1 1 n 3 sub {\n";
  outf << "/i exch def\n";
  outf << "i 1 sub dup i dup i 1 add dup i 2 add dup false subspline\n";
  outf << "} for\n";
  outf << "n 3 sub dup n 2 sub dup n 1 sub dup 0 0 false subspline\n";
  outf << "n 2 sub dup n 1 sub dup 0 0 1 1 false subspline\n";
  outf << "patternNone not { ifill } if\n";
  outf << "brushNone not { istroke } if\n";
  outf << "} {\n";
  outf << "Poly\n";
  outf << "} ifelse\n";
  outf << "end\n";
  outf << "} dup 0 4 dict put def\n";
  outf << "\n";
  outf << "/Elli {\n";
  outf << "0 begin\n";
  outf << "newpath\n";
  outf << "4 2 roll\n";
  outf << "translate\n";
  outf << "scale\n";
  outf << "0 0 1 0 360 arc\n";
  outf << "patternNone not { ifill } if\n";
  outf << "brushNone not { istroke } if\n";
  outf << "end\n";
  outf << "} dup 0 1 dict put def\n";
  outf << "\n";
  outf << "/Line {\n";
  outf << "0 begin\n";
  outf << "2 storexyn\n";
  outf << "newpath\n";
  outf << "x 0 get y 0 get moveto\n";
  outf << "x 1 get y 1 get lineto\n";
  outf << "brushNone not { istroke } if\n";
  outf << "0 0 1 1 leftarrow\n";
  outf << "0 0 1 1 rightarrow\n";
  outf << "end\n";
  outf << "} dup 0 4 dict put def\n";
  outf << "\n";
  outf << "/MLine {\n";
  outf << "0 begin\n";
  outf << "storexyn\n";
  outf << "newpath\n";
  outf << "n 1 gt {\n";
  outf << "x 0 get y 0 get moveto\n";
  outf << "1 1 n 1 sub {\n";
  outf << "/i exch def\n";
  outf << "x i get y i get lineto\n";
  outf << "} for\n";
  outf << "patternNone not brushLeftArrow not brushRightArrow not and and { ifill } if\n";
  outf << "brushNone not { istroke } if\n";
  outf << "0 0 1 1 leftarrow\n";
  outf << "n 2 sub dup n 1 sub dup rightarrow\n";
  outf << "} if\n";
  outf << "end\n";
  outf << "} dup 0 4 dict put def\n";
  outf << "\n";
  outf << "/Poly {\n";
  outf << "3 1 roll\n";
  outf << "newpath\n";
  outf << "moveto\n";
  outf << "-1 add\n";
  outf << "{ lineto } repeat\n";
  outf << "closepath\n";
  outf << "patternNone not { ifill } if\n";
  outf << "brushNone not { istroke } if\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/Rect {\n";
  outf << "0 begin\n";
  outf << "/t exch def\n";
  outf << "/r exch def\n";
  outf << "/b exch def\n";
  outf << "/l exch def\n";
  outf << "newpath\n";
  outf << "l b moveto\n";
  outf << "l t lineto\n";
  outf << "r t lineto\n";
  outf << "r b lineto\n";
  outf << "closepath\n";
  outf << "patternNone not { ifill } if\n";
  outf << "brushNone not { istroke } if\n";
  outf << "end\n";
  outf << "} dup 0 4 dict put def\n";
  outf << "\n";
  outf << "/Text {\n";
  outf << "ishow\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/idef {\n";
  outf << "dup where { pop pop pop } { exch def } ifelse\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/ifill {\n";
  outf << "0 begin\n";
  outf << "gsave\n";
  outf << "patternGrayLevel -1 ne {\n";
  outf << "fgred bgred fgred sub patternGrayLevel mul add\n";
  outf << "fggreen bggreen fggreen sub patternGrayLevel mul add\n";
  outf << "fgblue bgblue fgblue sub patternGrayLevel mul add setrgbcolor\n";
  outf << "eofill\n";
  outf << "} {\n";
  outf << "eoclip\n";
  outf << "originalCTM setmatrix\n";
  outf << "pathbbox /t exch def /r exch def /b exch def /l exch def\n";
  outf << "/w r l sub ceiling cvi def\n";
  outf << "/h t b sub ceiling cvi def\n";
  outf << "/imageByteWidth w 8 div ceiling cvi def\n";
  outf << "/imageHeight h def\n";
  outf << "bgred bggreen bgblue setrgbcolor\n";
  outf << "eofill\n";
  outf << "fgred fggreen fgblue setrgbcolor\n";
  outf << "w 0 gt h 0 gt and {\n";
  outf << "l b translate w h scale\n";
  outf << "w h true [w 0 0 h neg 0 h] { patternproc } imagemask\n";
  outf << "} if\n";
  outf << "} ifelse\n";
  outf << "grestore\n";
  outf << "end\n";
  outf << "} dup 0 8 dict put def\n";
  outf << "\n";
  outf << "/istroke {\n";
  outf << "gsave\n";
  outf << "brushDashOffset -1 eq {\n";
  outf << "[] 0 setdash\n";
  outf << "1 setgray\n";
  outf << "} {\n";
  outf << "brushDashArray brushDashOffset setdash\n";
  outf << "fgred fggreen fgblue setrgbcolor\n";
  outf << "} ifelse\n";
  outf << "brushWidth setlinewidth\n";
  outf << "originalCTM setmatrix\n";
  outf << "stroke\n";
  outf << "grestore\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/ishow {\n";
  outf << "0 begin\n";
  outf << "gsave\n";
  outf << "fgred fggreen fgblue setrgbcolor\n";
  outf << "/fontDict printFont printSize scalefont dup setfont def\n";
  outf << "/descender fontDict begin 0 [FontBBox] 1 get FontMatrix end\n";
  outf << "transform exch pop def\n";
  outf << "/vertoffset 1 printSize sub descender sub def {\n";
  outf << "0 vertoffset moveto show\n";
  outf << "/vertoffset vertoffset printSize sub def\n";
  outf << "} forall\n";
  outf << "grestore\n";
  outf << "end\n";
  outf << "} dup 0 3 dict put def\n";
  outf << "/patternproc {\n";
  outf << "0 begin\n";
  outf << "/patternByteLength patternString length def\n";
  outf << "/patternHeight patternByteLength 8 mul sqrt cvi def\n";
  outf << "/patternWidth patternHeight def\n";
  outf << "/patternByteWidth patternWidth 8 idiv def\n";
  outf << "/imageByteMaxLength imageByteWidth imageHeight mul\n";
  outf << "stringLimit patternByteWidth sub min def\n";
  outf << "/imageMaxHeight imageByteMaxLength imageByteWidth idiv patternHeight idiv\n";
  outf << "patternHeight mul patternHeight max def\n";
  outf << "/imageHeight imageHeight imageMaxHeight sub store\n";
  outf << "/imageString imageByteWidth imageMaxHeight mul patternByteWidth add string def\n";
  outf << "0 1 imageMaxHeight 1 sub {\n";
  outf << "/y exch def\n";
  outf << "/patternRow y patternByteWidth mul patternByteLength mod def\n";
  outf << "/patternRowString patternString patternRow patternByteWidth getinterval def\n";
  outf << "/imageRow y imageByteWidth mul def\n";
  outf << "0 patternByteWidth imageByteWidth 1 sub {\n";
  outf << "/x exch def\n";
  outf << "imageString imageRow x add patternRowString putinterval\n";
  outf << "} for\n";
  outf << "} for\n";
  outf << "imageString\n";
  outf << "end\n";
  outf << "} dup 0 12 dict put def\n";
  outf << "\n";
  outf << "/min {\n";
  outf << "dup 3 2 roll dup 4 3 roll lt { exch } if pop\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/max {\n";
  outf << "dup 3 2 roll dup 4 3 roll gt { exch } if pop\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/midpoint {\n";
  outf << "0 begin\n";
  outf << "/y1 exch def\n";
  outf << "/x1 exch def\n";
  outf << "/y0 exch def\n";
  outf << "/x0 exch def\n";
  outf << "x0 x1 add 2 div\n";
  outf << "y0 y1 add 2 div\n";
  outf << "end\n";
  outf << "} dup 0 4 dict put def\n";
  outf << "\n";
  outf << "/thirdpoint {\n";
  outf << "0 begin\n";
  outf << "/y1 exch def\n";
  outf << "/x1 exch def\n";
  outf << "/y0 exch def\n";
  outf << "/x0 exch def\n";
  outf << "x0 2 mul x1 add 3 div\n";
  outf << "y0 2 mul y1 add 3 div\n";
  outf << "end\n";
  outf << "} dup 0 4 dict put def\n";
  outf << "\n";
  outf << "/subspline {\n";
  outf << "0 begin\n";
  outf << "/movetoNeeded exch def\n";
  outf << "y exch get /y3 exch def\n";
  outf << "x exch get /x3 exch def\n";
  outf << "y exch get /y2 exch def\n";
  outf << "x exch get /x2 exch def\n";
  outf << "y exch get /y1 exch def\n";
  outf << "x exch get /x1 exch def\n";
  outf << "y exch get /y0 exch def\n";
  outf << "x exch get /x0 exch def\n";
  outf << "x1 y1 x2 y2 thirdpoint\n";
  outf << "/p1y exch def\n";
  outf << "/p1x exch def\n";
  outf << "x2 y2 x1 y1 thirdpoint\n";
  outf << "/p2y exch def\n";
  outf << "/p2x exch def\n";
  outf << "x1 y1 x0 y0 thirdpoint\n";
  outf << "p1x p1y midpoint\n";
  outf << "/p0y exch def\n";
  outf << "/p0x exch def\n";
  outf << "x2 y2 x3 y3 thirdpoint\n";
  outf << "p2x p2y midpoint\n";
  outf << "/p3y exch def\n";
  outf << "/p3x exch def\n";
  outf << "movetoNeeded { p0x p0y moveto } if\n";
  outf << "p1x p1y p2x p2y p3x p3y curveto\n";
  outf << "end\n";
  outf << "} dup 0 17 dict put def\n";
  outf << "\n";
  outf << "/storexyn {\n";
  outf << "/n exch def\n";
  outf << "/y n array def\n";
  outf << "/x n array def\n";
  outf << "n 1 sub -1 0 {\n";
  outf << "/i exch def\n";
  outf << "y i 3 2 roll put\n";
  outf << "x i 3 2 roll put\n";
  outf << "} for\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/SSten {\n";
  outf << "fgred fggreen fgblue setrgbcolor\n";
  outf << "dup true exch 1 0 0 -1 0 6 -1 roll matrix astore\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/FSten {\n";
  outf << "dup 3 -1 roll dup 4 1 roll exch\n";
  outf << "newpath\n";
  outf << "0 0 moveto\n";
  outf << "dup 0 exch lineto\n";
  outf << "exch dup 3 1 roll exch lineto\n";
  outf << "0 lineto\n";
  outf << "closepath\n";
  outf << "bgred bggreen bgblue setrgbcolor\n";
  outf << "eofill\n";
  outf << "SSten\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/Rast {\n";
  outf << "exch dup 3 1 roll 1 0 0 -1 0 6 -1 roll matrix astore\n";
  outf << "} def\n";
  outf << "\n";
  outf << "%%EndProlog\n";
  outf << "\n";
  outf << "%%BeginIdrawPrologue\n";
  outf << "/arrowhead {\n";
  outf << "0 begin\n";
  outf << "transform originalCTM itransform\n";
  outf << "/taily exch def\n";
  outf << "/tailx exch def\n";
  outf << "transform originalCTM itransform\n";
  outf << "/tipy exch def\n";
  outf << "/tipx exch def\n";
  outf << "/dy tipy taily sub def\n";
  outf << "/dx tipx tailx sub def\n";
  outf << "/angle dx 0 ne dy 0 ne or { dy dx atan } { 90 } ifelse def\n";
  outf << "gsave\n";
  outf << "originalCTM setmatrix\n";
  outf << "tipx tipy translate\n";
  outf << "angle rotate\n";
  outf << "newpath\n";
  outf << "arrowHeight neg arrowWidth 2 div moveto\n";
  outf << "0 0 lineto\n";
  outf << "arrowHeight neg arrowWidth 2 div neg lineto\n";
  outf << "patternNone not {\n";
  outf << "originalCTM setmatrix\n";
  outf << "/padtip arrowHeight 2 exp 0.25 arrowWidth 2 exp mul add sqrt brushWidth mul\n";
  outf << "arrowWidth div def\n";
  outf << "/padtail brushWidth 2 div def\n";
  outf << "tipx tipy translate\n";
  outf << "angle rotate\n";
  outf << "padtip 0 translate\n";
  outf << "arrowHeight padtip add padtail add arrowHeight div dup scale\n";
  outf << "arrowheadpath\n";
  outf << "ifill\n";
  outf << "} if\n";
  outf << "brushNone not {\n";
  outf << "originalCTM setmatrix\n";
  outf << "tipx tipy translate\n";
  outf << "angle rotate\n";
  outf << "arrowheadpath\n";
  outf << "istroke\n";
  outf << "} if\n";
  outf << "grestore\n";
  outf << "end\n";
  outf << "} dup 0 9 dict put def\n";
  outf << "\n";
  outf << "/arrowheadpath {\n";
  outf << "newpath\n";
  outf << "arrowHeight neg arrowWidth 2 div moveto\n";
  outf << "0 0 lineto\n";
  outf << "arrowHeight neg arrowWidth 2 div neg lineto\n";
  outf << "} def\n";
  outf << "\n";
  outf << "/leftarrow {\n";
  outf << "0 begin\n";
  outf << "y exch get /taily exch def\n";
  outf << "x exch get /tailx exch def\n";
  outf << "y exch get /tipy exch def\n";
  outf << "x exch get /tipx exch def\n";
  outf << "brushLeftArrow { tipx tipy tailx taily arrowhead } if\n";
  outf << "end\n";
  outf << "} dup 0 4 dict put def\n";
  outf << "\n";
  outf << "/rightarrow {\n";
  outf << "0 begin\n";
  outf << "y exch get /tipy exch def\n";
  outf << "x exch get /tipx exch def\n";
  outf << "y exch get /taily exch def\n";
  outf << "x exch get /tailx exch def\n";
  outf << "brushRightArrow { tipx tipy tailx taily arrowhead } if\n";
  outf << "end\n";
  outf << "} dup 0 4 dict put def\n";
  outf << "\n";
  outf << "%%EndIdrawPrologue\n";
  outf << "\n";
  outf << "%I Idraw 10 Grid 8 8 \n";
  outf << "\n";
  outf << "%%Page: 1 1\n";
  outf << "\n";
  outf << "Begin\n";
  outf << "%I b u\n";
  outf << "%I cfg u\n";
  outf << "%I cbg u\n";
  outf << "%I f u\n";
  outf << "%I p u\n";
  outf << "%I t\n";
  outf << "[ " << IDRAW_SCALING << " 0 0 " << IDRAW_SCALING << " 0 0 ] concat\n";
  outf << "/originalCTM matrix currentmatrix def\n";
  outf << "\n";
}

drvIDRAW::~drvIDRAW() {
  // Write idraw PostScript trailer
  outf << "End %I eop\n\n";
  outf << "showpage\n\n";
  outf << "%%Trailer\n\n";
  outf << "end\n";
}

// Return the name of closest matching color
const char *drvIDRAW::rgb2name(double red, double green, double blue)
{
  const char *name = 0;
  double bestquality = 1e+100;       // Arbitrary large number

  // Linear search for a match
  for (int i=0; i<IDRAW_NUMCOLORS; i++) {
    double quality = (red-color[i].red)*(red-color[i].red) +
                     (green-color[i].green)*(green-color[i].green) +
                     (blue-color[i].blue)*(blue-color[i].blue);
    if (quality < bestquality) {
      bestquality = quality;
      name = color[i].name;
    }
  }
  return name;
}

// Output a standard idraw object header
void drvIDRAW::print_header(const char *objtype)
{
  int i;
  unsigned int j;

  // Object type
  outf << "Begin %I " << objtype << endl;

  // Special case for text
  if (!strcmp(objtype,"Text")) {
    // Foreground color
    outf << "%I cfg " << rgb2name(currentR(),currentG(),currentB()) << endl;
    outf << currentR() << ' ' << currentG() << ' ' << currentB() << " SetCFg\n";
    return;
  }

  // Dash pattern
  outf << "%I b ";
  double dash[4];
  int dashpieces = sscanf(dashPattern(),"[ %lf %lf %lf %lf",
                          &dash[0],&dash[1],&dash[2],&dash[3]);
  if (dashpieces) {
    unsigned short dashbits = 0;
    for (i=0; i<4; i++) {
      unsigned int numbits = iscale(dash[i % dashpieces]);
      for (j=0; j<numbits; j++)
        dashbits = dashbits<<1 | (~i & 1);
    }
    outf << dashbits << endl;
    outf << iscale(currentLineWidth()) << " 0 0 [";
    for (i=0; i<dashpieces-1; i++)
      outf << iscale(dash[i]) << ' ';
    outf << iscale(dash[i]) << "] ";
  }
  else {
    outf << 65535 << endl;
    outf << iscale(currentLineWidth()) << " 0 0 [] ";
  }
  outf << "0 SetB" << endl;     // Hard-code an offset of 0 into the pattern

  // Foreground color
  outf << "%I cfg " << rgb2name(currentR(),currentG(),currentB()) << endl;
  outf << currentR() << ' ' << currentG() << ' ' << currentB() << " SetCFg\n";

  // Background color
  outf << "%I cbg " << rgb2name(currentR(),currentG(),currentB()) << endl;
  outf << currentR() << ' ' << currentG() << ' ' << currentB() << " SetCBg\n";

  // Pattern
  if (currentShowType() == drvbase::stroke)
    outf << "none SetP %I p n" << endl;
  else {
    outf << "%I p" << endl;
    outf << "0 SetP" << endl;
  }

  // Transposition matrix
  outf << "%I t" << endl;
  outf << "[ 1 -0 -0 1 0 0 ] concat" << endl;    // No translation/rotation
}

void drvIDRAW::print_coords()
{
  unsigned int pathelts = numberOfElementsInPath();
  bool closed;                // True if shape is closed
  bool curved;                // True if shape is curved
  const Point *firstpoint;    // First and last points in shape
  const Point *lastpoint;
  unsigned int totalpoints;   // Total number of points in shape
  const Point dummypoint(-123.456f,-789.101112f);  // Used to help eliminate duplicates
  const Point **pointlist;    // List of points
  unsigned int i,j;

  // First, try to figure out what type of shape we have
  closed = false;
  curved = false;
  for (i=0; i<pathelts; i++) {
    if (pathElement(i).getType() == curveto)
      curved = true;
    else
      if (pathElement(i).getType() == closepath)
        closed = true;
  }
  pointlist = new const Point*[pathelts*3];  // Allocate a conservative amount
  firstpoint = NULL;
  lastpoint = &dummypoint;
  totalpoints = 0;
  for (i=0; i<pathelts; i++) {
    const basedrawingelement &pelt = pathElement(i);

    if ((pelt.getType() == moveto || pelt.getType() == lineto) &&
        !(pelt.getPoint(0) == *lastpoint))
      lastpoint = pointlist[totalpoints++] = &pelt.getPoint(0);
    else
      if (pelt.getType() == curveto)
        for (j=0; j<3; j++)
          lastpoint = pointlist[totalpoints++] = &pelt.getPoint(j);
  }
  if (totalpoints)
    firstpoint = pointlist[0];
  else
    return;
  if (firstpoint->x_==lastpoint->x_ && firstpoint->y_==lastpoint->y_)
    closed = true;

  // Find points on the curve for curved lines
  if (curved) {
    const unsigned int pt_per_cp = 5;    // PostScript points per control point
    const unsigned int min_innerpoints = 2;  // Minimum # of points to add
    unsigned int innerpoints;            // Number of points to add
    const Point **newpointlist;          // List of points on curve
    unsigned int newtotalpoints = 0;     // Number of points in curve

    // ASSUMPTION: Curve is moveto+curveto+curveto+curveto+...
    newpointlist = new const Point*[pathelts*3000/pt_per_cp];  // Allocate a conservative amount
    for (i=0; i<totalpoints-3; i+=3) {
      float x0 = pointlist[i]->x_;
      float y0 = pointlist[i]->y_;
      float x1 = pointlist[i+1]->x_;
      float y1 = pointlist[i+1]->y_;
      float x2 = pointlist[i+2]->x_;
      float y2 = pointlist[i+2]->y_;
      float x3 = pointlist[i+3]->x_;
      float y3 = pointlist[i+3]->y_;
      float cx = (x1-x0)*3;
      float cy = (y1-y0)*3;
      float bx = (x2-x1)*3 - cx;
      float by = (y2-y1)*3 - cy;
      float ax = x3 - x0 - cx - bx;
      float ay = y3 - y0 - cy - by;
      float t;

      // Longer lines get more control points
      innerpoints = (unsigned int) (sqrt((y1-y0)*(y1-y0) + (x1-x0)*(x1-x0)) +
                                    sqrt((y2-y1)*(y2-y1) + (x2-x1)*(x2-x1)) +
                                    sqrt((y3-y2)*(y3-y2) + (x3-x2)*(x3-x2))) /
                                   pt_per_cp;
      if (innerpoints < min_innerpoints)
	innerpoints = min_innerpoints;

      // Add points to the list
      ADDPOINT(x0,y0);
      for (j=1; j<=innerpoints; j++) {
        t = (float)j / (float)innerpoints;
        float newx = (((ax*t)+bx)*t+cx)*t+x0;
        float newy = (((ay*t)+by)*t+cy)*t+y0;
        ADDPOINT(newx,newy);
      }
      ADDPOINT(x3,y3);
    }

    delete[] pointlist;
    pointlist = newpointlist;
    totalpoints = newtotalpoints;
  }

  // Straight lines, not closed
  if (!closed && !curved) {
    if (totalpoints == 2) {            // Special case for single line
      print_header("Line");
      outf << "%I" << endl;
      outf << iscale(firstpoint->x_) << ' ' << iscale(firstpoint->y_) << ' ';
      outf << iscale(lastpoint->x_)  << ' ' << iscale(lastpoint->y_)  << ' ';
      outf << "Line" << endl;
      outf << "%I 1" << endl;
      outf << "End" << endl << endl;
    }
    else {                             // Otherwise, output a multiline
      print_header("MLine");           // (Should have a special case for Rect)
      outf << "%I " << totalpoints << endl;
      for (i=0; i<totalpoints; i++) {
        outf << iscale(pointlist[i]->x_) << ' ';
        outf << iscale(pointlist[i]->y_) << endl;
      }
      outf << totalpoints << " MLine" << endl;
      outf << "%I 1" << endl;
      outf << "End" << endl << endl;
    }
  }

  // Straight lines, closed */
  if (closed && !curved) {
    unsigned int numpoints;

    numpoints = totalpoints==1 ? 1 : totalpoints-1;
    print_header("Poly");               // Output a polygon
    outf << "%I " << numpoints << endl;
    for (i=0; i<totalpoints; i++) {
      outf << iscale(pointlist[i]->x_) << ' ';
      outf << iscale(pointlist[i]->y_) << endl;
    }
    outf << numpoints << " Poly" << endl;
    outf << "End" << endl << endl;
  }

  // Curved lines, not closed
  if (!closed && curved) {
    print_header("BSpl");               // Output a B-spline
    outf << "%I " << totalpoints << endl;
    for (i=0; i<totalpoints; i++) {
      outf << iscale(pointlist[i]->x_) << ' ';
      outf << iscale(pointlist[i]->y_) << endl;
    }
    outf << totalpoints << " BSpl" << endl;
    outf << "%I 1" << endl;
    outf << "End" << endl << endl;
  }

  // Curved lines, closed
  if (closed && curved) {
    unsigned int numpoints;

    numpoints = totalpoints==1 ? 1 : totalpoints-1;
    print_header("CBSpl");              // Output a closed B-spline
    outf << "%I " << numpoints << endl;
    for (i=0; i<numpoints; i++) {
      outf << iscale(pointlist[i]->x_) << ' ';
      outf << iscale(pointlist[i]->y_) << endl;
    }
    outf << numpoints << " CBSpl" << endl;
    outf << "End" << endl << endl;
  }
}


// idraw doesn't support more than one page of graphics
void drvIDRAW::open_page()
{
}

// idraw doesn't support more than one page of graphics
void drvIDRAW::close_page()
{
}

// Convert a PostScript font name to an XLFD font name
const char *drvIDRAW::psfont2xlfd(const char *psfont)
{
  // Times Roman family
  if (!strcmp(psfont,"Times-Roman"))
    return "-*-times-medium-r-*-*-";
  if (!strcmp(psfont,"Times-Bold"))
    return "-*-times-bold-r-*-*-";
  if (!strcmp(psfont,"Times-Italic"))
    return "-*-times-medium-i-*-*-";
  if (!strcmp(psfont,"Times-BoldItalic"))
    return "-*-times-bold-i-*-*-";

  // Helvetica family
  if (!strcmp(psfont,"Helvetica"))
    return "-*-helvetica-medium-r-*-*-";
  if (!strcmp(psfont,"Helvetica-Bold"))
    return "-*-helvetica-bold-r-*-*-";
  if (!strcmp(psfont,"Helvetica-Oblique"))
    return "-*-helvetica-medium-o-*-*-";
  if (!strcmp(psfont,"Helvetica-BoldOblique"))
    return "-*-helvetica-bold-o-*-*-";

  // Courier family
  if (!strcmp(psfont,"Courier"))
    return "-*-courier-medium-r-*-*-";
  if (!strcmp(psfont,"Courier-Bold"))
    return "-*-courier-bold-r-*-*-";
  if (!strcmp(psfont,"Courier-Oblique"))
    return "-*-courier-medium-o-*-*-";
  if (!strcmp(psfont,"Courier-BoldOblique"))
    return "-*-courier-bold-o-*-*-";

  // Symbol family
  if (!strcmp(psfont,"Symbol"))
    return "-*-symbol-medium-r-*-*-";

  // None of the above -- assume Times Roman is close enough
  return "-*-times-medium-r-*-*-";
}

void drvIDRAW::show_text(const TextInfo &textinfo)
{
  // Output the text header
  print_header("Text");

  // Output the name of the font to use on screen
  outf << "%I f " << psfont2xlfd(textinfo.currentFontName.value());
  outf << iscale(textinfo.currentFontSize);
  outf << "-*-*-*-*-*-*-*" << endl;

  // Output the name of the font to print
  outf << textinfo.currentFontName.value() << ' ';
  outf << iscale(textinfo.currentFontSize);
  outf << " SetF" << endl;

  // Output the next part of the text setup boilerplate
  outf << "%I t" << endl;
  const float toRadians = 3.14159265359f / 180.0f;
  const float angle = textinfo.currentFontAngle * toRadians;
  const float xoffset = textinfo.currentFontSize * (float) -sin(angle);
  const float yoffset = textinfo.currentFontSize * (float) cos(angle);
  outf << "[ " << cos(angle) << ' ' << sin(angle) << ' ';
  outf << -sin(angle) << ' ' << cos(angle) << ' ';
  outf << (unsigned int) (0.5 + xoffset + textinfo.x/IDRAW_SCALING) << ' ';
  outf << (unsigned int) (0.5 + yoffset + textinfo.y/IDRAW_SCALING);
  outf << " ] concat" << endl;
  outf << "%I" << endl;
  outf << "[" << endl;

  // Output the string, escaping parentheses with backslashes
  outf << '(';
  for (const char *c=textinfo.thetext.value(); *c; c++)
    switch (*c) {
      case '(':
        outf << "\\(";
        break;
      case ')':
        outf << "\\)";
        break;
      default:
        outf << *c;
        break;
    }
  outf << ')' << endl;
  outf << "] Text" << endl;
  outf << "End" << endl << endl;
}

// Show the current path (i.e. shape)
void drvIDRAW::show_path()
{
  print_coords();
};

// Does this ever get called?
void drvIDRAW::show_rectangle(const float, const float, const float, const float)
{
show_path();
}

static DriverDescriptionT<drvIDRAW> D_idraw(
		"idraw","Interviews draw format","idraw",
		false, // if backend supports subpathes, else 0
		   // if subpathes are supported, the backend must deal with
		   // sequences of the following form
		   // moveto (start of subpath)
		   // lineto (a line segment)
		   // lineto 
		   // moveto (start of a new subpath)
		   // lineto (a line segment)
		   // lineto 
		   //
		   // If this argument is set to 0 each subpath is drawn 
		   // individually which might not necessarily represent
		   // the original drawing.

		true, // if backend supports curves, else 0
		true, // if backend supports elements with fill and edges
		true, // if backend supports text, else 0
		false, // if backend supports Images
		DriverDescription::normalopen,
		false); // if format supports multiple pages in one file
 
