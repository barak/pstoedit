#ifndef PSTOEDITOPTIONS_H
#define PSTOEDITOPTIONS_H

/*
   pstoeditoptions.h : This file is part of pstoedit
   definition of program options 

   Copyright (C) 1993 - 2003 Wolfgang Glunz, wglunz@pstoedit.net

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


#include "miscutil.h"
#include "poptions.h"

class DLLEXPORT RSStringValueExtractor {
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, RSString &result) {
	if (instring) {
		result = instring;
		currentarg++;
		return true;
	} else {
		cout << "missing string argument for " << optname << " option" << endl;
		return false;
	}
	}
	static const char *gettypename() { return "string"; }
};
class DLLEXPORT charstringValueExtractor {
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, char* &result) {
	if (instring) {
		result = cppstrdup(instring);
		currentarg++;
		return true;
	} else {
		cout << "missing string argument for " << optname << " option" << endl;
		return false;
	}
	}
	static const char *gettypename() { return "string"; }
};

 
template < >
inline ostream & Option< char *, charstringValueExtractor>::writevalue(ostream & out) {
		out << (value ? value : "");
		return out;
}

class DLLEXPORT constcharstringValueExtractor {
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, const char* &result) {
	if (instring) {
		result = instring;
		currentarg++;
		return true;
	} else {
		cout << "missing string argument for " << optname << " option" << endl;
		return false;
	}
	}
	static const char *gettypename() { return "string"; }
};

 
template < >
inline ostream & Option< const char *, constcharstringValueExtractor>::writevalue(ostream & out) {
	out << (value ? value : "");
		return out;
}

class DLLEXPORT ArgvExtractor {
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, Argv  &result) {
	if (instring) {
		result.addarg(instring);
		currentarg++;
		return true;
	} else {
		cout << "missing string argument for " << optname << " option" << endl;
		return false;
	}
	}
	static const char *gettypename() { return "string"; }
};



class  PsToEditOptions : public ProgramOptions {
public:
		// cannot be const  because it needs to be changed on non UNIX systems (convertBackSlashes)
	char *nameOfInputFile  ; //= 0;
	char *nameOfOutputFile ; //= 0;	// can contain %d for page splitting

	Option < const char *,constcharstringValueExtractor> nameOfIncludeFile ;// = 0;	// name of an option include file
	Option < const char *,constcharstringValueExtractor> replacementfont;// = "Courier";
 	Option < bool, BoolFalseExtractor > maptoisolatin1 ;//= true;
	Option < bool, BoolTrueExtractor > withdisplay ;//= false;
	Option < bool, BoolFalseExtractor> doquit ;//= true;
	Option < bool, BoolTrueExtractor > nocurves ;//= false;		// normally curves are shown as curves if backend supports
	Option < bool, BoolTrueExtractor > nosubpathes ;//= false;	// normally we use subpathes if the backend support them
	Option < bool, BoolTrueExtractor > merge ;//= false;
	Option < bool, BoolTrueExtractor > drawtext ;//= false;
	Option < bool, BoolTrueExtractor > autodrawtext ;//= false;
	Option < bool, BoolTrueExtractor > disabledrawtext ;//= false;
	Option < bool, BoolTrueExtractor > correctdefinefont ;//= false;

	Option < bool, BoolTrueExtractor > ptioption;
	Option < bool, BoolTrueExtractor > ptaoption;
	unsigned int precisiontext ;//= 0; // derived from the above pti ->1 pta -> 2 sonst 0

	Option < bool, BoolTrueExtractor > splitpages ;//= false;
	Option < bool, BoolTrueExtractor > verbose ;//= false;
	Option < bool, BoolTrueExtractor > simulateSubPaths ;//= false;
	Option < const char *,constcharstringValueExtractor> unmappablecharstring ;//= 0;
	Option < bool, BoolFalseExtractor > loadplugins ;//= true;
	Option < bool, BoolTrueExtractor > nobindversion ;//= false;	// use old NOBIND instead of DELAYBIND
	Option < int, IntValueExtractor > pagetoextract ;//= 0;		// 0 stands for all pages
	Option < double, DoubleValueExtractor > flatness ;//= 1.0f;		// used for setflat
	Option < bool, BoolTrueExtractor > simulateClipping ;//= false;	// simulate clipping most useful in combination with -dt
	Option < bool, BoolTrueExtractor > useRGBcolors ;//= false;
	Option < bool, BoolTrueExtractor > noclip ;//= false;
	Option < bool, BoolTrueExtractor > t2fontsast1 ;//= false;	// handle T2 fonts (often come as embedded fonts in PDF files) same as T1
	Option < bool, BoolTrueExtractor > keepinternalfiles ;//= false;
	Option < bool, BoolTrueExtractor > justgstest ;//= false;
	Option < bool, BoolTrueExtractor > pscover ;//= false;
	Option < bool, BoolTrueExtractor > nofontreplacement ;//= false;
	Option < int, IntValueExtractor > rotation ;//= 0;
	Option < const char *,constcharstringValueExtractor> explicitFontMapFile ;//= 0;
	Option < RSString, RSStringValueExtractor > outputPageSize;//("");
	Option < bool, BoolTrueExtractor > fromgui;


	Option < double, DoubleValueExtractor >  magnification ;//= 1.0f;
	Option < bool, BoolTrueExtractor > showdrvhelp ;//= false;
	Option < bool, BoolTrueExtractor > dumphelp ;//= false;

	Option < bool, BoolTrueExtractor > backendonly ;//= false;	// used for easier debugging of backends
	// directly read input file by backend
	// bypass ghostscript. The input file
	// is assumed to be produced by a
	// previous call with -f debug

	Option <Argv, ArgvExtractor > psArgs;				// Pass through arguments to PostScript interpreter
	Option < char *,charstringValueExtractor> drivername ;//= 0;

	PsToEditOptions() :

	nameOfInputFile  (0),
	nameOfOutputFile (0),	// can contain %d for page splitting

	nameOfIncludeFile	(true, "-include","name of an option include file" , 0),	// 
	replacementfont		(true, "-df","default replacement font for raster fonts", "Courier"),
	maptoisolatin1		(true, "-nomaptoisolatin1","don't map to ISO-Latin 1 encoding" , true),
	withdisplay			(true, "-dis","let GhostScript display the file during conversion" , false),
	doquit				(true, "-nq","don't quit GhostScript after PostScript processing - for debugging only" , true),
	nocurves			(true, "-nc","normally curves are shown as curves if backend supports. This options forces curves to be always converted to line segments.", false),		// 
	nosubpathes			(true, "-nsp","normally subpathes are used if the backend support them. Thus option turns off subpathes." , false),	// 
	merge				(true, "-merge","merge adjacent pathes if one is a stroke and the other is a fill. This depends on the capabilities of the selected backend" , false),
	drawtext			(true, "-dt","draw text, i.e. convert text to polygons" , false),
	autodrawtext		(true, "-adt","automatic draw text. This draws text only for text that uses fonts with non standard encodings" , false),
	disabledrawtext		(true, "-ndt","fully disable any \"intelligence\" for drawing text" , false),
	correctdefinefont	(true, "-correctdefinefont","apply some \"corrective\" actions to definefont - use this for ChemDraw generated PostScript files", false),

	ptioption			(true, "-pti","precision text - individual. Places text character by character - but only if non standard chararater widths are used",false),
	ptaoption			(true, "-pta","precision text - always. Places text character by character",false),
	precisiontext (0), //= 0; // derived from the above pti ->1 pta -> 2 sonst 0

	splitpages			(true, "-split","split multipage documents into single pages" , false),
	verbose				(true, "-v","turns on verbose mode", false),
	simulateSubPaths	(true, "-ssp","simulate subpaths" , false),
	unmappablecharstring(true, "-uchar","when a character cannot be mapped to one of the standard encoding vector, use this as replacement" , 0),
	loadplugins			(true, "-dontloadplugins","internal option - not relevant for normal user" , true),
	nobindversion		(true, "-nb","use old NOBIND instead of DELAYBIND - try this if GhostScript has problems", false),	// 
	pagetoextract		(true, "-page","extract a specific page: 0 means all pages" , 0),		// 0 stands for all pages
	flatness			(true, "-flat","the precision use for approximating curves by lines if needed" , 1.0),		// used for setflat
	simulateClipping	(true, "-sclip","simulate clipping - probably you need to set this if you use -dt" , false),	// simulate clipping
	useRGBcolors		(true, "-rgb","use RGB colors instead of CMYK" , false),
	noclip				(true, "-noclip","don't use clipping (relevant only if backend supports clipping at all)" , false),
	t2fontsast1			(true, "-t2fontsast1","handle T2 fonts (often come as embedded fonts in PDF files) same as T1" , false),	// handle T2 fonts (often come as embedded fonts in PDF files) same as T1
	keepinternalfiles	(true, "-keep","keep the internal files - for debug purposes only" ,false),
	justgstest			(true, "-gstest","perform a basic test for the interworking with GhostScript" ,false),
	pscover				(true, "-pscover","perform coverage statistics about the pstoedit PostScript proloque - for debug and test only" , false),
	nofontreplacement	(true, "-nfr","don't replace non standard encoded fonts with a replacement font" , false),
	rotation			(true, "-rotate","rotate the image" , 0),
	explicitFontMapFile	(true, "-fontmap","use a font mapping from a file" , 0),
	outputPageSize		(true, "-pagesize","set page size (e.g. a4)",""),
	fromgui				(true, "-fromgui","internal - not for normal user",false),
	magnification		(true, "-scale","scale result",1.0), 
	showdrvhelp			(true, "-help","show the help information",false) ,
	dumphelp			(true, "-dumphelp","show all options of all drivers",false), 
	backendonly			(true, "-bo","backend only - assumes an input file in the internal dump format, e.g. produced by a previous run using -f dump" , false),	// used for easier debugging of backends
	psArgs				(true, "-psarg", "additional arguments to be passed to GhostScript directly" ),
	drivername			(false,"-f","target format identifier" ,0)
	{

	// nameOfInputFile (0);
	// nameOfOutputFile (0),	// can contain %d for page splitting

	add(&nameOfIncludeFile);
	add(&replacementfont);
	add(&maptoisolatin1);
	add(&withdisplay);
	add(&doquit);
	add(&nocurves );
	add(&nosubpathes);	
	add(&merge);
	add(&drawtext);
	add(&autodrawtext);
	add(&disabledrawtext);
	add(&correctdefinefont);

	add(&ptioption);
	add(&ptaoption);
	// add(&precisiontext (0); //= 0; // derived from the above pti ->1 pta -> 2 sonst 0

	add(&splitpages);
	add(&verbose );
	add(&simulateSubPaths);
	add(&unmappablecharstring);
	add(&loadplugins);
	add(&nobindversion );
	add(&pagetoextract);	
	add(&flatness);		
	add(&simulateClipping);	
	add(&useRGBcolors);
	add(&noclip);
	add(&t2fontsast1);	
	add(&keepinternalfiles);
	add(&justgstest);
	add(&pscover);
	add(&nofontreplacement);
	add(&rotation );
	add(&explicitFontMapFile);
	add(&outputPageSize);
	add(&fromgui);
	add(&magnification); 
	add(&showdrvhelp) ;
	add(&dumphelp ); 
	add(&backendonly);	
	add(&psArgs);	

	add(&drivername);
}

	~PsToEditOptions() {
		delete drivername.value;
		delete nameOfInputFile ;
		delete nameOfOutputFile  ;	
	}

//		AutoDeleter < char >DeleterFordrivername(drivername, true);

};

#endif
 
