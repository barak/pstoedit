%%Title: pstoedit.pro, Version 2.3.3, November 1995
% This file contains some redefinitions of PostScript(TM) operators
% useful for the conversion of PostScript into a vector format via Ghostscript
%
% Copyright (C) 1993,1994,1995 Wolfgang Glunz, Wolfgang.Glunz@zfe.siemens.de
%
%    This program is free software; you can redistribute it and/or modify
%    it under the terms of the GNU General Public License as published by
%    the Free Software Foundation; either version 2 of the License, or
%    (at your option) any later version.
%
%    This program is distributed in the hope that it will be useful,
%    but WITHOUT ANY WARRANTY; without even the implied warranty of
%    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%    GNU General Public License for more details.
%
%    You should have received a copy of the GNU General Public License
%    along with this program; if not, write to the Free Software
%    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
%
%
%%History:
%
% xx.10.95 : fixed calculation of current scale factor
%            added color support
%            escape ( and ) in text for pure PostScript backend
% 08.09.95 : reset rotation angle for non rotated text to 0
% 05.05.95 : Fixed calculation of text angle. Pure text files were not handled correctly. 
%          : Redirect prints from the input PostScript to stderr.
% 27.04.95 : Add support for rotated text (IJMP)
% 24.04.95 : Added some checks for unavailable entries in FontInfo dictionary
% 24.02.95 : some cleanup for version 2.1 (removed unnecessay procs, ...)
% 30.09.94 : fixed some setgray problems (due to grestore)
% 05.05.94 : added support for MIF backend
% 07.09.93 : added widthshow, kshow, awidthshow, showpage
% 28.05.93 : calculate effective line with in setlinewidth
% 22.04.93 : added ashow
% 20.04.93 : fixed a problem in stroke, fill, eofill (use {..} instead of /-... )
%
% 19.04.93 : Version 1.0 released to comp.sources.postscript
%
%%Acknowledgements:
% David B. Rosen <rosen@murmur> for some ideas and some PostScript code 
% from his ps2aplot program.

/-print  /print load def
/-fill   /fill load def
/-eofill /eofill load def
/-stroke /stroke load def
/-show   /show load def
/-showpage /showpage load def
/s100 100 string def
/s1000 1000 string def
/onechar 1 string def

systemdict begin
/pdfmark { 
	([ ) -print 
	] { showpdfmarkelement ( ) -print } forall % close array and dump all entries
	(pdfmark\n) -print 
} def
end

/showpdfmarkelement {
	dup type pdftype begin exec end
} def

/pdftype 20 dict def
pdftype begin
/arraytype { ([ ) -print { showpdfmarkelement ( ) -print } forall ( ]) -print } def 
/booleantype { s1000 cvs -print } def
/conditiontype { pop } def
/devicetype { pop } def
/dicttype { pop } def
/filetype { pop } def
/fonttype { pop } def
/gstatetype { pop } def
/integertype { s1000 cvs -print } def
/locktype { pop } def
/marktype { pop } def
/nametype { ( /) -print s1000 cvs -print } def
/nulltype { pop } def
/operatortype { pop } def
/packedarraytype { pop } def
/realtype { s1000 cvs -print } def
/savetype { pop } def
/stringtype { (\() -print -print (\)) -print } def
end

/pdfmark systemdict /pdfmark get def % overwrite pdfmark in userdict
/currentdistillerparams 1 dict def
currentdistillerparams begin
/CoreDistVersion 2000 def
end

/print {
 (\% PRINT ) -print 
 -print
 ( MSG\n) -print
} def

/debugprint {
% ( MSG\n) -print % for debugging
 (\n) -print % for normal mode
} def

/printTOS {
    ( ) -print s100 cvs -print 
} def


/prpoints {
    -1 1 { pop        % the loop control variable
	   transform  % transform to device coordinates
	   exch
	   printTOS   % the x value
	   deviceheight exch sub % fix flipped y coordinate
	   printTOS   % the y value
	 } for
} def

/getcurrentscale { 
% returns current scale on stack
	100 0 transform % x1' y1'
	  /y1 exch def
	  /x1 exch def
	  0 0 transform % x2' y2'
	  /y2 exch def
	  /x2 exch def
	x1 x2 sub dup mul 
	y1 y2 sub dup mul 
	add sqrt
	100 div abs
%	dup ( scale is ) -print == (\n) -print
} def

/printstate {
	% should be triggerd by (g)restore
	gsave

	currentlinewidth
	getcurrentscale mul
	printTOS
	( setlinewidth \n) -print

	currentgray
	printTOS
	( setgray \n) -print

	currentrgbcolor
	3 1 roll exch
	printTOS
	printTOS
	printTOS
	( setrgbcolor \n) -print

	grestore
} def

/print-path {
	printstate
	% savely do pathforall
	{
	    { 1 prpoints ( moveto\n)    -print }
	    { 1 prpoints ( lineto\n)    -print }
	    { 3 prpoints ( curveto\n)   -print }
	    {            ( closepath\n) -print }
	    pathforall
	} stopped {
	} if
} def

/dumppath {
	printstate
	/dumpstring exch def
	/dumpop exch def
	(\n) -print
	gsave
	{
		flattenpath % replaces curves by lines
	} stopped {
		grestore
		gsave
	} if
	(newpath \n) -print
	print-path
	dumpstring -print ( \n) -print
	grestore
	dumpop
} def

/eofill	{
%	 (in pstoedit.pro ) currentgray pstack pop pop
	printstate
	{-eofill} (eofill) dumppath
	} def

/fill  	{
	printstate
	{-fill} (fill) dumppath
	} def

/stroke	{
	printstate
	{-stroke} (stroke) dumppath
	} def

/show {
	printstate
	/currentstring exch def
	gsave
	textastext
	{
		% dump text as text  (not as polygon)
		% dump font information (taken from ps2aplot from "David B. Rosen" <rosen@unr.edu> )

		% FONTNAME command 
		(/) -print
		currentfont begin
		/FontName where {
			pop FontName s100 cvs % FontName is a name or is already a string
		} {
			(Courier)
		} ifelse -print
		end
		( findfont ) -print


		% FONTSIZE command 
		currentfont begin
		FontMatrix 0 get % get the x-scale of the font matrix
		0
		% we have the delta vector dx (original xscale) 0 on the stack
		dtransform
		% now we have dx' and dy' on stack
		% scale is sqrt ( dx' ** 2 + dy' ** 2)
		dup mul exch dup mul add sqrt
		1000 mul % standard fonts have factor of .001 (let's hope)
%		round 
		s100 cvs -print
		end
		( scalefont setfont\n) -print

		% FontFamily, FullName and Weight 
		currentfont begin
		/FontInfo where {
			pop 
			FontInfo begin
			FontInfo /FamilyName known { FamilyName } { (unknown) } ifelse
				(\% ) -print -print  ( FamilyName \n) -print
			FontInfo /FullName   known { FullName   } { (unknown) } ifelse   
				(\% ) -print -print  ( FullName \n) -print
			FontInfo /Weight     known { Weight     } { (unknown) } ifelse
				(\% ) -print -print  ( Weight \n) -print
			end
		} if
		end

% debug		1 0 dtransform (\% vector ) -print exch ( x ) -print printTOS ( y ) -print printTOS debugprint
		% Rotation angle
		/pstoeditangle 1 0 dtransform exch atan -1 mul 360 add def
% debug		(\% angle ) -print pstoeditangle printTOS debugprint
		currentpoint /pstoedity exch def /pstoeditx exch def
		pstoeditx pstoedity 1 prpoints ( moveto\n)    -print
		pstoeditangle 360.0 ne {
			pstoeditx pstoedity 1 prpoints ( translate\n) -print
			( ) -print
			pstoeditangle s100 cvs -print ( rotate\n) -print
		} 
		{
			( ) -print
			0             s100 cvs -print ( rotate\n) -print
		}
		ifelse
		(\() -print
		escapetext
		{
			% PostScript backend selected, so escape '(' ')' '\'
			currentstring {
			dup (\() 0 get eq { (\\) -print } if
			dup (\)) 0 get eq { (\\) -print } if
			dup (\\) 0 get eq { (\\) -print } if
			% convert integer (from forall) back to string
			% for this put it into the one element string onechar
			% put does not return the modified string, so we have to push it again
			onechar exch 0 exch put onechar -print 
			} forall
		}
		{
			% No need to escape special PostScript characters
			currentstring -print 
		}
		ifelse

		(\) show\n) -print
		pstoeditangle 360.0 ne {
			( ) -print
			pstoeditangle neg s100 cvs -print ( rotate\n) -print
			pstoeditx pstoedity 1 prpoints
			( ntranslate\n) -print
		} if
	}
	{
		% convert text to polygons
		currentpoint
		/currenty exch def
		/currentx exch def
		stroke % dump current path and start new path

		currentx currenty moveto
		currentstring
		true charpath 
		eofill 
	}
	ifelse
	grestore
	currentstring
	-show
} def

/widthshow  {exch pop exch pop exch pop show} def
/awidthshow {exch pop exch pop exch pop exch pop exch pop show} def
/kshow      {exch pop show} def
/ashow      {exch pop exch pop show} def

/showpage {
    (showpage \n) -print
    -showpage
} def

% determine device height
0 0 transform 
/deviceheight exch def
/devicewidth exch def  % not used so far

%300 300 gssetresolution

(\%!\n) -print
(\% generated by pstoedit\n) -print
(/ntranslate { neg exch neg exch translate } def\n) -print

%/textastext true def
%/escapetext true def
