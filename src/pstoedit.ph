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
// Copyright (C) 1993 - 1999 Wolfgang Glunz, wglunz@geocities.com
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
 " /pstoeditcopyright (Copyright \\(C\\) 1993 - 1999 Wolfgang Glunz) def ",
 " currentdict /maptoisolatin1 	known not { /maptoisolatin1 true def} if ",
 " currentdict /textastext 	known not { /textastext true def} if ",
 " currentdict /flatnesstouse 	known not { /flatnesstouse 1.0 def} if ",
 " currentdict /escapetext 	known not { /escapetext false def} if ",
 " currentdict /redirectstdout 	known not { /redirectstdout false def} if ",
 " currentdict /usepdfmark 	known not { /usepdfmark false def} if ",
 " currentdict /outputfilename 	known not { /outputfilename (%stdout) def} if ",
 " currentdict /inputfilename  	known not { /inputfilename  (%stdin) def} if ",
 " currentdict /doflatten      	known not { /doflatten      true  def} if ",
 " currentdict /withimages     	known not { /withimages     false  def} if ",
 " currentdict /verbosemode    	known not { /verbosemode    false  def} if ",
 " currentdict /replacementfont 	known not { /replacementfont      (Courier)  def} if ",
 " currentdict /delaybindversion	known not { /delaybindversion      true  def} if ",
 " currentdict /scalefontversion	known not { /scalefontversion      false  def} if ",
 " currentdict /simulateclipping	known not { /simulateclipping      false  def} if ",
 " currentdict /pagetoextract      known not { /pagetoextract    0  def} if  ",
 " delaybindversion { ",
 " revision 353 ge { ",
 " NOBIND /DELAYBIND where { pop DELAYBIND or } if ",
 " { ",
 " systemdict begin ",
 " /bind /.bind load ",
 " /.forcedef where { pop .forcedef } { def } ifelse ",
 " end ",
 " } if ",
 " } if ",
 " } if ",
 " /.forcedef where { pop } { /.forcedef /def load def } ifelse ",
 " delaybindversion { ",
 " /redef { systemdict begin 1 index exch .makeoperator //.forcedef end } bind def ",
 " /soverload {bind redef} bind def ",
 " /overload  {bind redef} bind def ",
 " systemdict /.setglobal known { .currentglobal true .setglobal } if  ",
 " } { ",
 " systemdict /.setglobal known { .currentglobal true .setglobal } if  ",
 " /soverload { systemdict begin bind def end } def ",
 " /overload { bind def } bind def ",
 " } ifelse ",
 " /outstream outputfilename (w) file def ",
 " redirectstdout { ",
 " /errstream (%stderr) (w) file def ",
 " /print { errstream exch writestring } overload ",
 " /flush { errstream flushfile } overload ",
 " } if ",
 " /-printalways  {  ",
 " outstream exch writestring  ",
 " } def ",
 " /-print  {  ",
 " pagetoextract 0 eq pagetoextract pstoedit.pagenr cvi eq or ",
 " { ",
 " -printalways ",
 " }  ",
 " { pop } ifelse ",
 " } def ",
 " /-fill   /fill load def ",
 " /-eofill /eofill load def ",
 " /-stroke /stroke load def ",
 " /-image /image load def ",
 " /-imagemask /imagemask load def ",
 " /-colorimage /colorimage load def ",
 " /-awidthshow   /awidthshow load def ",
 " /-showpage /showpage load def ",
 " /-string /string load def ",
 " /-lt /lt load def ",
 " /-charpath /charpath load def ",
 " /-stringwidth /stringwidth load def ",
 " languagelevel 1 gt { ",
 " /-glyphshow /glyphshow load def ",
 " } if ",
 " /pstoedit.pagenr (1           ) store ",
 " /pstoedit.replacedfont false store  % if true, font was replaced with Courier ",
 " /pstoedit.ignoredraw false store  % if true, ignore all fill, eofill, stroke ",
 " /pstoedit.ignoreimage false store  % if true, ignore all *image* ",
 " /pstoedit.textshown false store ",
 " /pstoedit.fontunit 1000 store % standard fonts have a unit of 1/1000 ",
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
 " /pstoedit.dpiscale 1 def ",
 " /pstoedit.currentscale 1 def ",
 " /pstoedit.glyphname 0 store ",
 " /pstoedit.glyphcounter 0 def ",
 " /-findfont /findfont load def ",
 " /-definefont /definefont load def ",
 " /-currentrgbcolor /currentrgbcolor load def ",
 " /-concat /concat load def ",
 " /s100 100 -string def ",
 " /s1000 1000 -string def ",
 " /globals100 { currentglobal true setglobal 100 -string exch setglobal } def ",
 " usepdfmark { ",
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
 " /pstoedit.DEVICEINFO { ",
 " /deviceinfo where { ",
 " pop % the dict ",
 " revision 500 -lt { ",
 " deviceinfo ",
 " }{ ",
 " systemdict begin currentpagedevice  end ",
 " } ifelse ",
 " } { ",
 " systemdict begin currentpagedevice  end ",
 " } ifelse ",
 " } def ",
 " /pstoedit.computeDPIscale { ",
 " languagelevel 1 gt { ",
 " pstoedit.DEVICEINFO /HWResolution known { ",
 " pstoedit.DEVICEINFO /HWResolution get 0 get  ",
 " 72 div /pstoedit.dpiscale exch store % normalize dpi to 72 ",
 " } { ",
 " /pstoedit.dpiscale 1 store ",
 " } ifelse ",
 " } { ",
 " /pstoedit.dpiscale 1 store ",
 " } ifelse ",
 " } def ",
 " /pstoedit.initialize { ",
 " 0 0 transform  ",
 " /pstoedit.deviceheight exch store ",
 " pop ",
 " /pstoedit.deviceinfoshown false store ",
 " pstoedit.computeDPIscale  ",
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
 " /quotesingle (') def ",
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
 " /pstoedit.quit { ",
 " flush ",
 " (\\% normal end reached by pstoedit.pro\\n) -printalways ",
 " (\\%\\%Pages: ) -printalways pstoedit.pagenr cvi 100 -string cvs -printalways (\\n) -printalways ",
 " (\\%\\%EOF\\n) -printalways ",
 " pstoedit.quitprog ",
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
 " 1000 -string cvs print ( from ) print  ",
 " pstoedit.inputchar 1000 -string cvs print (\\n) print ",
 " pop unmappablecharacter  ",
 " } if ",
 " } ",
 " {  ",
 " (unable to map ) print pstoedit.glyphname   ",
 " 1000 -string cvs print ( from ) print  ",
 " pstoedit.inputchar 1000 -string cvs print (\\n) print ",
 " unmappablecharacter  ",
 " }  ",
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
 " verbosemode { ",
 " currentfont /FontName get 100 -string cvs print ",
 " ( seems to be a non text font ') print  ",
 " dup 100 -string cvs print ('\\n) print ",
 " } if ",
 " } ",
 " ifelse ",
 " } def ",
 " /normalizestring { ",
 " dup  ",
 " length 2 mul -string /pstoedit.normalizedstring exch store ",
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
 " /pstoedit.lastfindfont replacementfont def ",
 " /findfont { ",
 " dup /pstoedit.lastfindfont exch store ",
 " -findfont ",
 " } overload ",
 " /definefont { ",
 " exch  % font key ",
 " dup type /dicttype eq { ",
 " } { ",
 " (\\() -printalways ",
 " dup s100 cvs -printalways ",
 " (\\) cvn) -printalways ",
 " ( /) -printalways  ",
 " pstoedit.lastfindfont s100 cvs -printalways  ",
 " ( pstoedit.newfont\\n) -printalways ",
 " dup FontHistory  ",
 " exch  ",
 " pstoedit.lastfindfont globals100 cvs  ",
 " put ",
 " } ifelse ",
 " exch ",
 " -definefont ",
 " } overload ",
 " /rootFont { ",
 " dup  ",
 " FontHistory exch known { ",
 " dup dup FontHistory exch get  ",
 " ne { FontHistory exch get rootFont } if            ",
 " }  ",
 " if ",
 " } def ",
 " /printTOS { ",
 " ( ) -print s100 cvs -print  ",
 " } def ",
 " /prpoints { ",
 " -1 1 {   ",
 " 2 mul -2 roll     % use the loop control variable to roll up a point (x,y) from bottom ",
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
 " pstoedit.computeDPIscale  ",
 " gsave ",
 " pstoedit.deviceinfoshown not { ",
 " /pstoedit.deviceinfoshown true store ",
 " languagelevel 1 gt { ",
 " pstoedit.DEVICEINFO /PageSize known { ",
 " pstoedit.DEVICEINFO /PageSize get ",
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
 " currentmiterlimit ",
 " printTOS ",
 " ( setmiterlimit\\n) -print  ",
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
 " simulateclipping { clip clippath } if ",
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
 " /stringwidth { ",
 " /pstoedit.ignoredraw true  store ",
 " -stringwidth ",
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
 " /getorigfont { ",
 " dup /OrigFont known  ",
 " { /OrigFont get getorigfont }  ",
 " if ",
 " } def ",
 " /awidthshow { ",
 " /pstoedit.ignoreimage true  store %ignore all image* ops called from here ",
 " printstate ",
 " /pstoedit.currentstring exch store ",
 " 5 copy % save for real operator ",
 " gsave ",
 " textastext 			% text shall be shown as text and ",
 " currentfont /FontType get 1 eq 	% current font is type 1 ",
 " currentfont /FontType get 42 eq % current font is type 42 (TT) ",
 " or ",
 " pstoedit.forcedrawtext not	% and not forced draw due to font restrictions ",
 " and ",
 " currentfont /FontType get 3 eq 	% current font is type 3 ",
 " currentfont /BitMaps known   	% and contains a BitMaps entry (as dvips provides) ",
 " and				% Bitmap fonts cannot be \"drawn\", anyway ",
 " or				% (T1 and not force)  or (t3 and Bitmap) ",
 " and				% textastext ",
 " dup /pstoedit.textshown exch store % ",
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
 " (\\() -print ",
 " -print ",
 " (\\) cvn) -print ",
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
 " scalefontversion { ",
 " s100 cvs -print ( scalefont setfont\\n) -print ",
 " } ",
 " { ",
 " ([ ) -print dup printTOS 0 printTOS 0 printTOS printTOS  ",
 " gsave ",
 " currentpoint translate ",
 " currentfont /FontMatrix get -concat 0 0 transform ",
 " grestore ",
 " itransform ",
 " 1 prpoints ",
 " ( ] makefont setfont\\n) -print ",
 " } ",
 " ifelse ",
 " } { ",
 " currentfont begin ",
 " currentfont /FontType get 42 eq { ",
 " /pstoedit.fontunit 1 store ",
 " }{ ",
 " /pstoedit.fontunit 1000 store ",
 " } ifelse ",
 " scalefontversion %wogl ",
 " { ",
 " FontMatrix 0 get % get the x-scale of the font matrix ",
 " 0 ",
 " dtransform ",
 " dup mul exch dup mul add sqrt ",
 " pstoedit.fontunit mul  ",
 " pstoedit.dpiscale div ",
 " s100 cvs -print ( scalefont setfont\\n) -print ",
 " }  ",
 " { ",
 " gsave ",
 " currentpoint translate ",
 " currentfont /FontMatrix get -concat ",
 " currentfont getorigfont /FontMatrix get  matrix invertmatrix -concat ",
 " matrix currentmatrix matrix defaultmatrix matrix invertmatrix matrix concatmatrix ",
 " grestore ",
 " ([ ) -print { printTOS } forall ( ] makefont setfont\\n) -print ",
 " }  ",
 " ifelse ",
 " end ",
 " } ",
 " ifelse ",
 " currentfont begin ",
 " /FontInfo where { ",
 " pop  ",
 " FontInfo /FamilyName known { FontInfo /FamilyName get 100 -string cvs} { (unknown) } ifelse ",
 " (\\% ) -print -print  ( FamilyName\\n) -print ",
 " FontInfo /FullName   known { FontInfo /FullName get 100 -string cvs} { (unknown) } ifelse    ",
 " (\\% ) -print -print  ( FullName\\n) -print ",
 " FontInfo /Weight     known { FontInfo /Weight get 100 -string cvs} { (unknown) } ifelse ",
 " (\\% ) -print -print  ( Weight\\n) -print ",
 " } if ",
 " end ",
 " scalefontversion { ",
 " /pstoedit.angle 1 0 dtransform exch atan -1 mul 360 add store ",
 " languagelevel 1 gt { ",
 " pstoedit.DEVICEINFO dup /Orientation known { ",
 " /Orientation get  ",
 " -90 mul pstoedit.angle add /pstoedit.angle exch store  ",
 " pstoedit.angle 0 -lt { pstoedit.angle 360 add /pstoedit.angle exch store } if ",
 " } {  ",
 " pop % the duped DEVICEINFO ",
 " } ifelse ",
 " } if ",
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
 " } % for scale font version ",
 " { ",
 " (  0 0 moveto\\n)    -print ",
 " } ",
 " ifelse  ",
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
 " /pstoedit.ignoreimage false store  ",
 " pstoedit.textshown ",
 " { ",
 " gsave ",
 " currentpoint translate ",
 " currentfont /FontMatrix get -concat 0 0 transform ",
 " grestore  ",
 " itransform ",
 " 1 prpoints ",
 " currentfont /Encoding get 65 get /A eq maptoisolatin1 and { ",
 " ( 1 ) ",
 " } { ( 0 ) }  ifelse -print ",
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
 " scalefontversion { ",
 " pstoedit.angle 360.0 ne { ",
 " ( ) -print ",
 " pstoedit.angle neg s100 cvs -print ( rotate\\n) -print ",
 " pstoedit.x pstoedit.y 1 prpoints ",
 " ( ntranslate\\n) -print ",
 " } if ",
 " } if ",
 " } if ",
 " } overload ",
 " /widthshow  {  ",
 " 0 exch 0 exch awidthshow } overload ",
 " /ashow      { 0 0 32  ",
 " 6 3 roll awidthshow } overload ",
 " /show	    { 0 0 32 0 0   6 -1 roll awidthshow } overload ",
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
 " pagetoextract pstoedit.pagenr cvi eq  ",
 " { ",
 " pstoedit.quit ",
 " } if ",
 " pstoedit.pagenr cvi 1 add 100 -string cvs pstoedit.pagenr copy pop  ",
 " verbosemode { ([) print pstoedit.pagenr cvi 100 -string cvs print (]) print } if ",
 " (\\%\\%Page: ) -print pstoedit.pagenr cvi 100 -string cvs dup -print ( ) -print -print (\\n) -print ",
 " } soverload ",
 " withimages { ",
 " /pstoedit.ncomp   0 store ",
 " /pstoedit.multi   0 store ",
 " /pstoedit.datasrc 0 store ",
 " /pstoedit.mat	  0 store ",
 " /pstoedit.bits    0 store ",
 " /pstoedit.height  0 store ",
 " /pstoedit.width   0 store  ",
 " /pstoedit.fourthparam 0 store ",
 " /pstoedit.procname 0 store ",
 " /pstoedit.isfile  true store ",
 " /pstoedit.stringprefix 0 store ",
 " /pstoedit.realproc 0 store ",
 " /printdumponly { ",
 " escapetext { pop } { -print } ifelse ",
 " } def ",
 " /colorimage { ",
 " pstoedit.ignoredraw pstoedit.ignoreimage or  ",
 " pstoedit.DEVICEINFO /HWResolution known not or { ",
 " -colorimage ",
 " } { ",
 " exch dup  ",
 " { ",
 " (Warning: colorimage with more than one datasrc not yet supported\\n) print ",
 " exch -colorimage ",
 " } { ",
 " escapetext { (gsave\\n)  -print  }  ",
 " {(\\%colorimage:begin\\n) -print } ",
 " ifelse ",
 " exch ",
 " /pstoedit.ncomp   exch store ",
 " /pstoedit.multi   exch store ",
 " /pstoedit.datasrc exch store ",
 " /pstoedit.mat	  exch store ",
 " /pstoedit.bits    exch store ",
 " /pstoedit.height  exch store ",
 " /pstoedit.width   exch store  ",
 " (% DC ) printdumponly pstoedit.width  printTOS (\\n) -print ",
 " (\\%colorimage:width\\n) printdumponly  ",
 " (% DC ) printdumponly pstoedit.height printTOS (\\n) -print ",
 " (\\%colorimage:height\\n) printdumponly  ",
 " (% DC ) printdumponly pstoedit.bits   printTOS (\\n) -print ",
 " (\\%colorimage:bits\\n) printdumponly  ",
 " escapetext not { ",
 " (% DC ) -print pstoedit.mat { printTOS } forall (\\n) -print ",
 " (\\%colorimage:imagematrix\\n) -print  ",
 " } if ",
 " gsave ",
 " pstoedit.mat matrix invertmatrix concat ",
 " matrix currentmatrix matrix defaultmatrix matrix invertmatrix matrix concatmatrix ",
 " escapetext { ([ ) -print } { (% DC ) -print } ifelse { printTOS } forall  ",
 " escapetext { (]  concat [ 1 0 0 1 0 0 ] ) -print } if ",
 " (\\n) -print ",
 " (\\%colorimage:normalized image currentmatrix\\n) printdumponly   ",
 " grestore ",
 " escapetext { ({ currentfile imagestring readhexstring pop }\\n)  -print } if ",
 " (% DC ) printdumponly  pstoedit.multi   printTOS (\\n) -print ",
 " (\\%colorimage:multi\\n) printdumponly  ",
 " (% DC ) printdumponly pstoedit.ncomp   printTOS (\\n) -print ",
 " (\\%colorimage:ncomp\\n) printdumponly ",
 " escapetext { (colorimage\\n) -print } if ",
 " pstoedit.width pstoedit.height pstoedit.bits pstoedit.mat  ",
 " {  ",
 " pstoedit.datasrc  ",
 " (\\%colorimage:data ) printdumponly  ",
 " dup { ",
 " dup 16 -lt { (0) -print } if ",
 " 16 2 -string cvrs -print  ",
 " } forall ",
 " (\\n) -print ",
 " } ",
 " pstoedit.multi pstoedit.ncomp -colorimage ",
 " escapetext { (grestore\\n)  -print  }  ",
 " { (\\%colorimage:end\\n) -print }ifelse ",
 " } ifelse ",
 " } ifelse % ignoredraw ",
 " } def ",
 " /ImageAndImagemask { ",
 " /pstoedit.fourthparam exch store ",
 " /pstoedit.procname exch store ",
 " /pstoedit.stringprefix exch store ",
 " /pstoedit.realproc exch store ",
 " pstoedit.ignoredraw pstoedit.ignoreimage or  ",
 " pstoedit.DEVICEINFO /HWResolution known not or { ",
 " (image operation ignored \\n) print ",
 " pstoedit.realproc ",
 " } { ",
 " dup type ",
 " dup /filetype eq /pstoedit.isfile exch store ",
 " /dicttype eq { ",
 " (Warning: Level 2 version of image and imagemask not yet supported\\n) print ",
 " pstoedit.realproc ",
 " } { ",
 " escapetext { (gsave\\n)  -print  }  ",
 " { pstoedit.stringprefix -print (begin\\n) -print } ifelse ",
 " /pstoedit.datasrc exch store ",
 " /pstoedit.mat	  exch store ",
 " escapetext not { ",
 " (% DC ) -print pstoedit.mat    { printTOS } forall (\\n) -print ",
 " pstoedit.stringprefix -print (imagematrix\\n) -print  ",
 " } if ",
 " /pstoedit.bits    exch store ",
 " /pstoedit.height  exch store ",
 " /pstoedit.width   exch store  ",
 " (% DC ) printdumponly pstoedit.width  printTOS (\\n) -print ",
 " pstoedit.stringprefix printdumponly (width\\n) printdumponly  ",
 " (% DC ) printdumponly pstoedit.height printTOS (\\n) -print ",
 " pstoedit.stringprefix printdumponly (height\\n) printdumponly  ",
 " (% DC ) printdumponly pstoedit.bits   printTOS (\\n) -print ",
 " pstoedit.stringprefix printdumponly pstoedit.fourthparam printdumponly (\\n) printdumponly ",
 " gsave ",
 " pstoedit.mat matrix invertmatrix concat ",
 " matrix currentmatrix matrix defaultmatrix matrix invertmatrix matrix concatmatrix ",
 " escapetext { ([ ) -print } { (% DC ) -print } ifelse  ",
 " { printTOS } forall  ",
 " escapetext { (]  concat [ 1 0 0 1 0 0 ] ) -print } if ",
 " (\\n) -print ",
 " pstoedit.stringprefix printdumponly (normalized image currentmatrix\\n) printdumponly   ",
 " escapetext { ({ currentfile imagestring readhexstring pop }\\n)  -print pstoedit.procname -print (\\n) -print  } if ",
 " grestore ",
 " pstoedit.width pstoedit.height pstoedit.bits pstoedit.mat {  ",
 " pstoedit.isfile  { ",
 " pstoedit.datasrc  read { ",
 " pstoedit.stringprefix printdumponly (data ) printdumponly   ",
 " dup   ",
 " dup 16 -lt { (0) -print } if  ",
 " 16 2 -string cvrs -print ",
 " inttosinglechar  ",
 " } {  ",
 " 0 -string  ",
 " } ifelse   ",
 " } {  ",
 " pstoedit.datasrc  ",
 " pstoedit.stringprefix printdumponly (data ) printdumponly  ",
 " dup { ",
 " dup 16 -lt { (0) -print } if ",
 " 16 2 -string cvrs -print  ",
 " } forall ",
 " } ifelse ",
 " (\\n) -print ",
 " } ",
 " pstoedit.realproc ",
 " escapetext { (grestore\\n)  -print  }  ",
 " { pstoedit.stringprefix -print (end\\n) -print } ifelse ",
 " } ifelse ",
 " } ifelse % ignoredraw ",
 " } def ",
 " /image {  ",
 " { -image } (\\%image:) (image) (bits) ImageAndImagemask  ",
 " } def ",
 " /imagemask {  ",
 " { -imagemask } (\\%imagemask:) (imagemask) (polarity) ImageAndImagemask  ",
 " } def ",
 " } if ",
 " pstoedit.initialize ",
 " (\\%!PS-Adobe-3.0\\n) -printalways ",
 " (\\%\\%Creator: pstoedit\\n) -printalways ",
 " (\\%\\%Pages: \\(atend\\)\\n) -printalways ",
 " (\\%\\%BeginProlog\\n) -printalways ",
 " (/setPageSize { pop pop } def\\n) -printalways ",
 " (/ntranslate { neg exch neg exch translate } def\\n) -printalways ",
 " (/setshowparams { pop pop pop} def\\n) -printalways ",
 " (/pstoedit.newfont { ) -printalways ",
 " (80 string cvs ) -printalways ",
 " ( findfont ) -printalways ",
 " ( dup length dict begin {1 index /FID ne {def} {pop pop} ifelse} forall ) -printalways ",
 " ( /Encoding ISOLatin1Encoding def  ) -printalways ",
 " ( dup 80 string cvs /FontName exch def ) -printalways ",
 " ( currentdict end ) -printalways ",
 " ( definefont pop) -printalways ",
 " ( } def\\n) -printalways ",
 " (/imagestring 1 string def\\n) -printalways ",
 " (\\%\\%EndProlog\\n) -printalways ",
 " (\\%\\%Page: 1 1\\n) -print ",
 " delaybindversion { ",
 " revision 353 ge { ",
 " .bindoperators ",
 " NOBIND currentdict systemdict ne and ",
 " { systemdict begin .bindoperators end } ",
 " if ",
 " /DELAYBIND where { pop DELAYBIND { .bindnow } if } if ",
 " } if ",
 " systemdict readonly pop ",
 " systemdict /.setglobal known { .setglobal } if  ",
 " } { ",
 " systemdict /.setglobal known { .setglobal } if  ",
 " NOBIND ",
 " { /bind /.bind load def } ",
 " if ",
 " } ifelse ",
 " languagelevel 1 gt { ",
 " << /PageSize [612 792 ] >> setpagedevice ",
 " } if ",
 " currentdict /nameOfIncludeFile known  ",
 " { nameOfIncludeFile run } if ",
 " inputfilename run ",
 " pstoedit.quit ",
0
};
 
