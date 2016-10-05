// DO NOT CHANGE THIS FILE. THIS FILE IS GENERATED FROM pstoedit.pro 
// You can get pstoedit.pro from the author of pstoedit
// pstoedit.pro contains a lot more comments and explanations than pstoedit.ph
const char * PS_prologue[] = 
{
//Title: pstoedit.pro / pstoedit.ph; This file is part ot pstoedit
// 
// This file contains some redefinitions of PostScript(TM) operators
// useful for the conversion of PostScript into a vector format via Ghostscript
//
// Copyright (C) 1993,1994,1995,1996,1997 Wolfgang Glunz, Wolfgang.Glunz@mchp.siemens.de
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
 " /pstoeditcopyright (Copyright \\(C\\) 1993,1994,1995,1996 Wolfgang Glunz) def ",
 " currentdict /maptoisolatin1 known not { /maptoisolatin1 true def} if ",
 " currentdict /textastext known not { /textastext true def} if ",
 " currentdict /flatnesstouse known not { /flatnesstouse 1.0 def} if ",
 " currentdict /escapetext known not { /escapetext false def} if ",
 " currentdict /redirectstdout known not { /redirectstdout false def} if ",
 " currentdict /usepdfmark known not { /usepdfmark false def} if ",
 " currentdict /outputfilename known not { /outputfilename (%stdout) def} if ",
 " currentdict /inputfilename  known not { /inputfilename  (%stdin) def} if ",
 " currentdict /doflatten      known not { /doflatten      true  def} if ",
 " currentdict /replacementfont      known not { /replacementfont      (Courier)  def} if ",
 " /soverload { systemdict begin bind def end } def ",
 " /overload { bind def } bind def ",
 " /outstream outputfilename (w) file def ",
 " redirectstdout { ",
 " /errstream (%stderr) (w) file def ",
 " /print { errstream exch writestring } overload ",
 " /flush { errstream flushfile } overload ",
 " } if ",
 " /-print  { outstream exch writestring } def ",
 " /-fill   /fill load def ",
 " /-eofill /eofill load def ",
 " /-stroke /stroke load def ",
 " /-awidthshow   /awidthshow load def ",
 " /-showpage /showpage load def ",
 " /-string /string load def ",
 " /-charpath /charpath load def ",
 " languagelevel 1 gt { ",
 " /-glyphshow /glyphshow load def ",
 " } if ",
 " /pstoedit.replacedfont false store  % if true, font was replaced with Courier ",
 " /pstoedit.ignoredraw false store  % if true, ignore all fill, eofill, stroke ",
 " /pstoedit.x 1 store ",
 " /pstoedit.y 1 store ",
 " /pstoedit.dumpstring () store ",
 " /pstoedit.dumpop {} store ",
 " /pstoedit.y1 1 store ",
 " /pstoedit.x1 1 store ",
 " /pstoedit.y2 1 store ",
 " /pstoedit.x2 1 store ",
 " /pstoedit.inputchar 0 store ",
 " /pstoedit.outputchar 0 store ",
 " /pstoedit.glyphname () store ",
 " /pstoedit.inverseDict 0 store ",
 " /pstoedit.currentstring () store ",
 " /pstoedit.normalizedstring () store ",
 " /pstoedit.stringindex 0 store ",
 " /pstoedit.angle 0 store ",
 " /pstoedit.pathnumber 0 def ",
 " /pstoedit.deviceheight 0 def ",
 " /pstoedit.deviceinfoshown false def ",
 " /pstoedit.devicedpi 0 def ",
 " /pstoedit.dpiscale 1 def ",
 " /pstoedit.currentscale 1 def ",
 " /pstoedit.glyphname 0 store ",
 " /pstoedit.glyphcounter 0 def ",
 " /-findfont /findfont load def ",
 " /pstoedit.lastfindfont /pstoedit.defaultrootfont def ",
 " /-definefont /definefont load def ",
 " /-currentrgbcolor /currentrgbcolor load def ",
 " /-concat /concat load def ",
 " /s100 100 -string def ",
 " /s1000 1000 -string def ",
 " usepdfmark { ",
 " systemdict begin ",
 " /pdfmark {  ",
 " ([ ) -print  ",
 " ] { showpdfmarkelement ( ) -print } forall % close array and dump all entries ",
 " (pdfmark\\n) -print  ",
 " } def ",
 " end ",
 " /showpdfmarkelement { ",
 " dup type pdftype begin exec end ",
 " } def ",
 " /pdftype 20 dict def ",
 " pdftype begin ",
 " /arraytype { ([ ) -print { showpdfmarkelement ( ) -print } forall ( ]) -print } def  ",
 " /booleantype { s1000 cvs -print } def ",
 " /conditiontype { pop } def ",
 " /devicetype { pop } def ",
 " /dicttype { pop } def ",
 " /filetype { pop } def ",
 " /fonttype { pop } def ",
 " /gstatetype { pop } def ",
 " /integertype { s1000 cvs -print } def ",
 " /locktype { pop } def ",
 " /marktype { pop } def ",
 " /nametype { ( /) -print s1000 cvs -print } def ",
 " /nulltype { pop } def ",
 " /operatortype { pop } def ",
 " /packedarraytype { pop } def ",
 " /realtype { s1000 cvs -print } def ",
 " /savetype { pop } def ",
 " /stringtype { (\\() -print -print (\\)) -print } def ",
 " end ",
 " /pdfmark systemdict /pdfmark get def % overwrite pdfmark in userdict ",
 " /currentdistillerparams 1 dict def ",
 " currentdistillerparams begin ",
 " /CoreDistVersion 2000 def ",
 " end ",
 " } if ",
 " /debugprint { ",
 " (\\% PRINT ) -print  ",
 " -print ",
 " ( MSG\\n) -print ",
 " } def ",
 " /onechar 1 -string def ",
 " /inttosinglechar { ",
 " onechar exch 0 exch put  ",
 " onechar 1 -string cvs ",
 " } def ",
 " /InverseISO 512 dict def ",
 " /InversePDF 512 dict def ",
 " /unmappablecharacter (#) def ",
 " /pstoedit.initialize { ",
 " 0 0 transform  ",
 " /pstoedit.deviceheight exch store ",
 " pop ",
 " /pstoedit.deviceinfoshown false store ",
 " /deviceinfo where { ",
 " pop % the dict ",
 " deviceinfo /HWResolution get 0 get /pstoedit.devicedpi exch store ",
 " pstoedit.devicedpi 72 div /pstoedit.dpiscale exch store % normalize dpi to 72 ",
 " } { ",
 " /pstoedit.dpiscale 1 store ",
 " } ifelse ",
 " /pstoedit.glyphcounter 0 store ",
 " ISOLatin1Encoding ",
 " { ",
 " /pstoedit.glyphname exch store ",
 " InverseISO pstoedit.glyphname known not ",
 " {  ",
 " InverseISO pstoedit.glyphname pstoedit.glyphcounter inttosinglechar put  ",
 " InversePDF pstoedit.glyphname pstoedit.glyphcounter inttosinglechar put  ",
 " }  ",
 " if ",
 " /pstoedit.glyphcounter pstoedit.glyphcounter 1 add store ",
 " } forall ",
 " 32 1 127 { ",
 " dup /pstoedit.glyphcounter exch store  ",
 " inttosinglechar cvn /pstoedit.glyphname exch store ",
 " InverseISO pstoedit.glyphname known not ",
 " {  ",
 " InverseISO pstoedit.glyphname pstoedit.glyphcounter inttosinglechar put  ",
 " InversePDF pstoedit.glyphname pstoedit.glyphcounter inttosinglechar put  ",
 " }  ",
 " if ",
 " } for  ",
 " InverseISO 11 inttosinglechar cvn  (ff) put  ",
 " InverseISO 12 inttosinglechar cvn  (fi) put  ",
 " InverseISO 13 inttosinglechar cvn  (fl) put  ",
 " InversePDF 11 inttosinglechar cvn  (ff) put  ",
 " InversePDF 12 inttosinglechar cvn  (fi) put  ",
 " InversePDF 13 inttosinglechar cvn  (fl) put  ",
 " InverseISO 	begin ",
 " /.notdef unmappablecharacter def ",
 " /quotesinglbase (,) def ",
 " /florin (f) def ",
 " /quotedblbase (,,) def ",
 " /ellipsis (...) def ",
 " /perthousand (%o) def ",
 " /guilsinglleft (<) def ",
 " /OE (OE) def ",
 " /quotedblleft (\") def ",
 " /quotedblright (\") def ",
 " /bullet (*) def ",
 " /endash (-) def ",
 " /emdash (-) def ",
 " /trademark (TM) def ",
 " /guilsinglright (>) def ",
 " /oe (oe) def ",
 " /Ydieresis (Y) def ",
 " /fraction (/) def ",
 " /fi (fi) def ",
 " /fl (fl) def ",
 " /hyphen (-) def ",
 " end ",
 " InversePDF 	begin ",
 " /.notdef unmappablecharacter def ",
 " /breve 24 inttosinglechar def ",
 " /caron 25 inttosinglechar def ",
 " /circumflex 26 inttosinglechar def ",
 " /dotaccent 27 inttosinglechar def ",
 " /hungarumlaut 28 inttosinglechar def ",
 " /ogonek 29 inttosinglechar def ",
 " /ring 30 inttosinglechar def ",
 " /tilde 31 inttosinglechar def ",
 " /quotesingle 39 inttosinglechar def ",
 " /grave 96 inttosinglechar def ",
 " /bullet 128 inttosinglechar def ",
 " /dagger 129 inttosinglechar def ",
 " /daggerdbl 130 inttosinglechar def ",
 " /ellipsis 131 inttosinglechar def ",
 " /emdash 132 inttosinglechar def ",
 " /endash 133 inttosinglechar def ",
 " /florin 134 inttosinglechar def ",
 " /fraction 135 inttosinglechar def ",
 " /guilsinglleft 136 inttosinglechar def ",
 " /guilsinglright 137 inttosinglechar def ",
 " /minus 138 inttosinglechar def ",
 " /perthousand 139 inttosinglechar def ",
 " /quotedblbase 140 inttosinglechar def ",
 " /quotedblleft 141 inttosinglechar def ",
 " /quotedblright 142 inttosinglechar def ",
 " /quoteleft 143 inttosinglechar def ",
 " /quoteright 144 inttosinglechar def ",
 " /quotesinglbase 145 inttosinglechar def ",
 " /trademark 146 inttosinglechar def ",
 " /fi 147 inttosinglechar def ",
 " /fl 148 inttosinglechar def ",
 " /Lslash 149 inttosinglechar def ",
 " /OE 150 inttosinglechar def ",
 " /Scaron 151 inttosinglechar def ",
 " /Ydieresis 152 inttosinglechar def ",
 " /Zcaron 153 inttosinglechar def ",
 " /dotlessi 154 inttosinglechar def ",
 " /lslash 155 inttosinglechar def ",
 " /oe 156 inttosinglechar def ",
 " /scaron 157 inttosinglechar def ",
 " /zcaron 158 inttosinglechar def ",
 " /hyphen 45 inttosinglechar def ",
 " /caron 25 inttosinglechar def ",
 " /space 32 inttosinglechar def ",
 " end ",
 " } def ",
 " /getfrominverse	 { ",
 " /pstoedit.inverseDict exch store	 ",
 " 0 get % get first and only char of string as int ",
 " /pstoedit.inputchar exch store ",
 " currentfont /Encoding get pstoedit.inputchar get /pstoedit.glyphname exch store ",
 " pstoedit.inverseDict pstoedit.glyphname known  ",
 " {  ",
 " pstoedit.inverseDict pstoedit.glyphname get  ",
 " dup 0 get 0 eq {  ",
 " (unable to map ) print pstoedit.glyphname   ",
 " 1000 string cvs print ( from ) print  ",
 " pstoedit.inputchar 1000 string cvs print  ",
 " (\\n) print ",
 " pop unmappablecharacter  ",
 " } if ",
 " } ",
 " {  ",
 " (unable to map ) print pstoedit.glyphname  1000 string cvs print ( from ) print pstoedit.inputchar 1000 string cvs print (\\n) print ",
 " unmappablecharacter }  ",
 " ifelse ",
 " } def ",
 " /normalizecharacter { ",
 " currentfont /Encoding get 65 get /A eq maptoisolatin1 and { ",
 " usepdfmark  ",
 " { InversePDF } ",
 " { InverseISO }  ",
 " ifelse  ",
 " getfrominverse ",
 " }  ",
 " {  ",
 " } ",
 " ifelse ",
 " } def ",
 " /normalizestring { ",
 " dup  ",
 " length 2 mul string /pstoedit.normalizedstring exch store ",
 " /pstoedit.stringindex 0 store % position in output string ",
 " { % in input string ",
 " inttosinglechar ",
 " normalizecharacter ",
 " dup length exch % save for later adjustment of index ",
 " pstoedit.normalizedstring exch ",
 " pstoedit.stringindex exch ",
 " putinterval ",
 " pstoedit.stringindex add /pstoedit.stringindex exch store  ",
 " } forall ",
 " pstoedit.normalizedstring 0 pstoedit.stringindex getinterval ",
 " } def ",
 " /getglyphaschar { ",
 " usepdfmark  ",
 " { InversePDF } ",
 " { InverseISO }  ",
 " ifelse  ",
 " 2 copy ",
 " exch known  ",
 " { ",
 " exch get ",
 " } { ",
 " pop pop ",
 " unmappablecharacter ",
 " } ifelse ",
 " } def ",
 " /FontHistory 256 dict def ",
 " /pstoedit.lastfindfont () def ",
 " /findfont { ",
 " dup /pstoedit.lastfindfont exch store ",
 " -findfont ",
 " } overload ",
 " /definefont { ",
 " exch ",
 " dup type /dicttype eq { ",
 " } { ",
 " dup s100 cvs (/) -print -print ( /) -print pstoedit.lastfindfont s100 cvs -print ( pstoedit.newfont\\n) -print ",
 " dup FontHistory exch pstoedit.lastfindfont put ",
 " } ifelse ",
 " exch ",
 " -definefont ",
 " } overload ",
 " /rootFont { ",
 " dup  ",
 " FontHistory exch known { ",
 " FontHistory exch get rootFont ",
 " }  ",
 " if ",
 " } def ",
 " /printTOS { ",
 " ( ) -print s100 cvs -print  ",
 " } def ",
 " /prpoints { ",
 " -1 1 { pop        % the loop control variable ",
 " transform  % transform to device coordinates ",
 " false ",
 " { ",
 " exch ",
 " pstoedit.dpiscale div  ",
 " printTOS   % the x value ",
 " pstoedit.deviceheight exch sub % fix flipped y coordinate ",
 " pstoedit.dpiscale div  ",
 " printTOS   % the y value ",
 " } ",
 " { ",
 " matrix defaultmatrix  ",
 " itransform ",
 " exch ",
 " printTOS   % the x value ",
 " printTOS   % the y value ",
 " } ",
 " ifelse ",
 " } for ",
 " } def ",
 " /getcurrentscale {  ",
 " 100 0 transform % x1' y1' ",
 " /pstoedit.y1 exch store ",
 " /pstoedit.x1 exch store ",
 " 0 0 transform % x2' y2' ",
 " /pstoedit.y2 exch store ",
 " /pstoedit.x2 exch store ",
 " pstoedit.x1 pstoedit.x2 sub dup mul  ",
 " pstoedit.y1 pstoedit.y2 sub dup mul  ",
 " add sqrt ",
 " 100 div abs ",
 " pstoedit.dpiscale div ",
 " } def ",
 " /printstate { ",
 " gsave ",
 " pstoedit.deviceinfoshown not { ",
 " /pstoedit.deviceinfoshown true store ",
 " languagelevel 1 gt { ",
 " deviceinfo /PageSize known { ",
 " deviceinfo /PageSize get ",
 " pstoedit.deviceheight 0.0 ne { ",
 " dup 1 get  ",
 " pstoedit.dpiscale mul  ",
 " /pstoedit.deviceheight exch store ",
 " } if ",
 " { ",
 " printTOS ",
 " } forall ",
 " ( setPageSize\\n) -print ",
 " } if ",
 " } if ",
 " } if ",
 " currentlinecap ",
 " printTOS ",
 " ( setlinecap\\n) -print  ",
 " currentlinejoin ",
 " printTOS ",
 " ( setlinejoin\\n) -print  ",
 " currentdash ",
 " exch ",
 " ( [) -print ",
 " { ",
 " getcurrentscale mul ",
 " printTOS ",
 " } forall % dump array ",
 " ( ]) -print ",
 " printTOS %index ",
 " ( setdash\\n) -print ",
 " currentlinewidth ",
 " getcurrentscale mul ",
 " printTOS ",
 " ( setlinewidth\\n) -print ",
 " -currentrgbcolor ",
 " 3 1 roll exch ",
 " printTOS ",
 " printTOS ",
 " printTOS ",
 " ( setrgbcolor\\n) -print ",
 " grestore ",
 " } def ",
 " /print-path { ",
 " { ",
 " { 1 prpoints ( moveto\\n)    -print } ",
 " { 1 prpoints ( lineto\\n)    -print } ",
 " { 3 prpoints ( curveto\\n)   -print } ",
 " {            ( closepath\\n) -print } ",
 " pathforall ",
 " } stopped { ",
 " } if ",
 " } def ",
 " /dumppath { ",
 " /pstoedit.dumpstring exch store ",
 " /pstoedit.dumpop exch store ",
 " (\\n) -print ",
 " (\\%) -print ",
 " /pstoedit.pathnumber pstoedit.pathnumber 1 add store ",
 " pstoedit.pathnumber printTOS ",
 " ( pathnumber\\n) -print ",
 " printstate ",
 " -print ",
 " gsave ",
 " { ",
 " doflatten  ",
 " { ",
 " flatnesstouse setflat ",
 " flattenpath % replaces curves by lines ",
 " } if ",
 " } stopped { ",
 " grestore ",
 " gsave ",
 " } if ",
 " (newpath\\n) -print ",
 " print-path ",
 " pstoedit.dumpstring -print (\\n) -print ",
 " grestore ",
 " pstoedit.dumpop ",
 " } def ",
 " /charpath { ",
 " /pstoedit.ignoredraw true  store ",
 " -charpath ",
 " /pstoedit.ignoredraw false  store ",
 " } overload ",
 " /eofill	{ ",
 " pstoedit.ignoredraw { ",
 " -eofill ",
 " } { ",
 " (\\% eofilledpath\\n) {-eofill} (eofill) dumppath ",
 " } ifelse ",
 " } overload ",
 " /fill  	{ ",
 " pstoedit.ignoredraw { ",
 " -fill ",
 " } { ",
 " (\\% filledpath\\n) {-fill} (fill) dumppath ",
 " } ifelse ",
 " } overload ",
 " /stroke	{ ",
 " pstoedit.ignoredraw { ",
 " -stroke ",
 " } { ",
 " (\\% strokedpath\\n) {-stroke} (stroke) dumppath ",
 " } ifelse ",
 " } overload ",
 " /pstoedit.forcedrawtext { ",
 " currentfont begin ",
 " /FontName where { ",
 " pop % the dict ",
 " FontName 100 -string cvs % FontName is a name or is already a string ",
 " cvn dup  ",
 " /pstoedit.knownFontNames where { ",
 " pop % the dict ",
 " pstoedit.knownFontNames exch known { ",
 " pop ",
 " false % don't force drawing mode ",
 " } { ",
 " rootFont  ",
 " pstoedit.knownFontNames exch known { ",
 " false % don't force drawing mode ",
 " } { ",
 " true ",
 " } ifelse ",
 " } ifelse ",
 " } { ",
 " pop pop ",
 " false % if pstoedit.knownFontNames is not defined, 'show' text ",
 " } ifelse ",
 " } { ",
 " true % if FontName is not known ",
 " } ifelse  ",
 " end ",
 " } def ",
 " /awidthshow { ",
 " printstate ",
 " /pstoedit.currentstring exch store ",
 " 5 copy % save for real operator ",
 " gsave ",
 " textastext 			% text shall be shown as text and ",
 " currentfont /FontType get 1 eq 	% current font is type 1 ",
 " pstoedit.forcedrawtext not	% and not forced draw due to font restrictions ",
 " and ",
 " currentfont /FontType get 3 eq 	% current font is type 3 ",
 " currentfont /BitMaps known   	% and contains a BitMaps entry (as dvips provides) ",
 " and				% Bitmap fonts cannot be \"drawn\", anyway ",
 " or				% T1 or (t3 and Bitmap) ",
 " and				% textastext ",
 " { ",
 " currentfont begin ",
 " /FontName where { ",
 " pop FontName dup  ",
 " Fontmap exch known { ",
 " 100 -string cvs % FontName is a name or is already a string ",
 " } { ",
 " 100 -string cvs % FontName is a name or is already a string ",
 " dup ",
 " cvn ",
 " rootFont  ",
 " (\\% ) -print ",
 " s100 cvs -print ( ) -print dup -print  ",
 " ( nonstandard font\\n) -print ",
 " } ifelse ",
 " /pstoedit.replacedfont false store   ",
 " } { ",
 " replacementfont ",
 " /pstoedit.replacedfont true  store   ",
 " } ifelse  ",
 " (/) -print ",
 " -print ",
 " end ",
 " ( findfont ) -print ",
 " pstoedit.replacedfont { ",
 " pstoedit.currentstring stringwidth ",
 " dtransform ",
 " dup mul exch dup mul add sqrt ",
 " gsave ",
 " replacementfont cvn findfont 10 scalefont setfont ",
 " pstoedit.currentstring normalizestring stringwidth  ",
 " dtransform ",
 " dup mul exch dup mul add sqrt ",
 " grestore ",
 " div 10 mul getcurrentscale mul  ",
 " } { ",
 " currentfont begin ",
 " false ",
 " { ",
 " gsave ",
 " currentpoint translate ",
 " currentfont /FontMatrix get -concat  ",
 " 1000 1000 dtransform dup mul exch dup mul add 2 div sqrt ",
 " grestore ",
 " }  ",
 " { ",
 " FontMatrix 0 get % get the x-scale of the font matrix ",
 " 0 ",
 " dtransform ",
 " dup mul exch dup mul add sqrt ",
 " 1000 mul % standard fonts have factor of .001 (let's hope) ",
 " }  ",
 " ifelse ",
 " pstoedit.dpiscale div ",
 " end ",
 " } ",
 " ifelse ",
 " s100 cvs -print ",
 " ( scalefont setfont\\n) -print ",
 " currentfont begin ",
 " /FontInfo where { ",
 " pop  ",
 " FontInfo begin ",
 " FontInfo /FamilyName known { FamilyName } { (unknown) } ifelse ",
 " (\\% ) -print -print  ( FamilyName\\n) -print ",
 " FontInfo /FullName   known { FullName   } { (unknown) } ifelse    ",
 " (\\% ) -print -print  ( FullName\\n) -print ",
 " FontInfo /Weight     known { Weight     } { (unknown) } ifelse ",
 " (\\% ) -print -print  ( Weight\\n) -print ",
 " end ",
 " } if ",
 " end ",
 " /pstoedit.angle 1 0 dtransform exch atan -1 mul 360 add store ",
 " gsave ",
 " currentpoint translate ",
 " currentfont /FontMatrix get -concat 0 0 transform ",
 " grestore  ",
 " itransform ",
 " /pstoedit.y exch store /pstoedit.x exch store ",
 " pstoedit.x pstoedit.y 1 prpoints ( moveto\\n)    -print ",
 " pstoedit.angle 360.0 ne { ",
 " pstoedit.x pstoedit.y 1 prpoints ( translate\\n) -print ",
 " ( ) -print ",
 " pstoedit.angle s100 cvs -print ( rotate\\n) -print ",
 " }  ",
 " { ",
 " ( ) -print ",
 " 0             s100 cvs -print ( rotate\\n) -print ",
 " } ",
 " ifelse ",
 " getcurrentscale dup /pstoedit.currentscale exch store ",
 " mul ",
 " exch ",
 " pstoedit.currentscale mul ",
 " 5 -2 roll  ",
 " pstoedit.currentscale mul ",
 " exch ",
 " pstoedit.currentscale mul ",
 " printTOS printTOS % c ",
 " 3 -1 roll ",
 " printTOS	% char ",
 " printTOS printTOS % a ",
 " ( setshowparams\\n) -print ",
 " (\\() -print ",
 " escapetext ",
 " { ",
 " pstoedit.currentstring { ",
 " inttosinglechar ",
 " normalizecharacter ",
 " dup (\\() eq { (\\\\) -print } if ",
 " dup (\\)) eq { (\\\\) -print } if ",
 " dup (\\\\) eq { (\\\\) -print } if ",
 " -print ",
 " } forall ",
 " } ",
 " { ",
 " pstoedit.currentstring { ",
 " inttosinglechar ",
 " normalizecharacter ",
 " dup dup (\\r) eq exch (\\n) eq or ",
 " { pop }  ",
 " { -print } ",
 " ifelse ",
 " } forall ",
 " } ",
 " ifelse ",
 " (\\) awidthshow\\n) -print ",
 " pstoedit.angle 360.0 ne { ",
 " ( ) -print ",
 " pstoedit.angle neg s100 cvs -print ( rotate\\n) -print ",
 " pstoedit.x pstoedit.y 1 prpoints ",
 " ( ntranslate\\n) -print ",
 " } if ",
 " } ",
 " { ",
 " pop pop pop pop pop ",
 " currentpoint ",
 " stroke % dump current path and start new path ",
 " moveto ",
 " pstoedit.currentstring ",
 " true charpath  ",
 " eofill  ",
 " } ",
 " ifelse ",
 " grestore ",
 " pstoedit.currentstring ",
 " /pstoedit.ignoredraw true  store ",
 " -awidthshow ",
 " /pstoedit.ignoredraw false store ",
 " } overload ",
 " /widthshow  {  ",
 " 0 exch 0 exch awidthshow } overload ",
 " /ashow      { 0 0 32  ",
 " 6 3 roll awidthshow } overload ",
 " /show	    { 0 0 32 0 0    6 -1 roll awidthshow } overload ",
 " /pstoedit.kshowproc {} store ",
 " /pstoedit.kshowclast 1  store ",
 " /kshow  {       ",
 " exch /pstoedit.kshowproc exch store  ",
 " true  % for first ",
 " exch ",
 " { ",
 " exch ",
 " { ",
 " dup /pstoedit.kshowclast exch store ",
 " inttosinglechar ",
 " show ",
 " pstoedit.kshowclast ",
 " false ",
 " } { ",
 " dup /pstoedit.kshowclast exch store ",
 " pstoedit.kshowproc ",
 " pstoedit.kshowclast inttosinglechar ",
 " show ",
 " pstoedit.kshowclast ",
 " false ",
 " } ifelse ",
 " } forall ",
 " pop pop % char and first ",
 " } overload ",
 " /pstoedit.xyshowindex 0  store ",
 " /pstoedit.xyshowarray [ 0 ]  store ",
 " /xshow { ",
 " dup type /arraytype eq { ",
 " /pstoedit.xyshowarray exch store ",
 " /pstoedit.xyshowindex 0  store ",
 " { ",
 " currentpoint ",
 " 3 -1 roll ",
 " inttosinglechar ",
 " show ",
 " exch ",
 " pstoedit.xyshowarray pstoedit.xyshowindex get add exch ",
 " moveto ",
 " /pstoedit.xyshowindex pstoedit.xyshowindex 1 add store ",
 " } ",
 " forall ",
 " } { ",
 " pop show ",
 " } ifelse ",
 " } overload ",
 " /yshow { ",
 " dup type /arraytype eq { ",
 " /pstoedit.xyshowarray exch store ",
 " /pstoedit.xyshowindex 0  store ",
 " { ",
 " currentpoint ",
 " 3 -1 roll ",
 " inttosinglechar ",
 " show ",
 " pstoedit.xyshowarray pstoedit.xyshowindex get add  ",
 " moveto ",
 " /pstoedit.xyshowindex pstoedit.xyshowindex 1 add store ",
 " } ",
 " forall ",
 " } { ",
 " pop show ",
 " } ifelse ",
 " } overload ",
 " /xyshow { ",
 " dup type /arraytype eq { ",
 " /pstoedit.xyshowarray exch store ",
 " /pstoedit.xyshowindex 0  store ",
 " { ",
 " currentpoint ",
 " 3 -1 roll ",
 " inttosinglechar ",
 " show ",
 " pstoedit.xyshowarray pstoedit.xyshowindex 1 add get add exch ",
 " pstoedit.xyshowarray pstoedit.xyshowindex get add exch ",
 " moveto ",
 " /pstoedit.xyshowindex pstoedit.xyshowindex 2 add store ",
 " } ",
 " forall ",
 " } { ",
 " pop show ",
 " } ifelse ",
 " } overload ",
 " /glyphshow { ",
 " getglyphaschar show ",
 " } overload ",
 " /showpage { ",
 " (showpage\\n) -print ",
 " -showpage ",
 " } soverload ",
 " pstoedit.initialize ",
 " (\\%!\\n) -print ",
 " (\\% generated by pstoedit\\n) -print ",
 " (/setPageSize { pop pop } def\\n) -print ",
 " (/ntranslate { neg exch neg exch translate } def\\n) -print ",
 " (/setshowparams { } def\\n) -print ",
 " (/pstoedit.newfont { ) -print ",
 " (80 string cvs ) -print ",
 " ( findfont ) -print ",
 " ( dup length dict begin {1 index /FID ne {def} {pop pop} ifelse} forall ) -print ",
 " ( /Encoding ISOLatin1Encoding def  ) -print ",
 " ( dup 80 string cvs /FontName exch def ) -print ",
 " ( currentdict end ) -print ",
 " ( definefont pop) -print ",
 " ( } def\\n) -print ",
 " NOBIND ",
 " { /bind /.bind load def } ",
 " if ",
 " currentdict /nameOfIncludeFile known  ",
 " { nameOfIncludeFile run } if ",
 " inputfilename run ",
 " flush ",
 " (\\% normal end reached by pstoedit.pro\\n) -print ",
0
};
