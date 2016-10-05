/*
   poptions.cpp : This file is part of pstoedit
   program option handling 

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
#include "poptions.h"

#include I_string_h
#include I_stdlib

USESTD

#define UNUSEDARG(p)


bool IntValueExtractor::getvalue(const char *optname, const char *instring, unsigned int &currentarg,
								 int &result)
{
	if (instring) {
		result = atoi(instring);
		currentarg++;
		return true;
	} else {
		cout << "missing integer argument for " << optname << " option:" << endl;
		return false;
	}
};
const char *IntValueExtractor::gettypename()
{
	return "integer";
}



bool DoubleValueExtractor::getvalue(const char *optname, const char *instring, unsigned int &currentarg,
								   double &result)
{
	if (instring) {
		result =  atof(instring);
		currentarg++;
		return true;
	} else {
		cout << "missing double argument for " << optname << " option:" << endl;
		return false;
	}
};
const char *DoubleValueExtractor::gettypename()
{
	return "double";
}




bool CharacterValueExtractor::getvalue(const char *optname, const char *instring, unsigned int &currentarg,
									   char &result)
{
	if (instring) {
		result = instring[0];
		currentarg++;
		return true;
	} else {
		cout << "missing character argument for " << optname << " option" << endl;
		return false;
	}
}

const char *CharacterValueExtractor::gettypename()
{
	return "character";
}



const char *BoolBaseExtractor::gettypename()
{
	return "boolean";
}
 
bool BoolInvertingExtractor::getvalue(const char *UNUSEDARG(optname),
									  const char *UNUSEDARG(instring), unsigned int &UNUSEDARG(currentarg),
									  bool &result)
{
	result = !result;			// just invert
	return true;
}


bool BoolFalseExtractor::getvalue(const char *UNUSEDARG(optname), const char *UNUSEDARG(instring),
								  unsigned int &UNUSEDARG(currentarg), bool &result)
{
	result = false;
	return true;
}


bool BoolTrueExtractor::getvalue(const char *UNUSEDARG(optname), const char *UNUSEDARG(instring),
								 unsigned int &UNUSEDARG(currentarg), bool &result)
{
	result = true;
	return true;
}


void ProgramOptions::showvalues(ostream & outstr) const
{
	for (unsigned int i = 0; i < optcount; i++) {
		outstr << alloptions[i]->flag << "\t : " << alloptions[i]->gettypename() << "\t : " 
			<< alloptions[i]->description << "\t : ";
		(void)alloptions[i]->writevalue(outstr);
		outstr << endl;
	}
}

unsigned int ProgramOptions::parseoptions(ostream & outstr, unsigned int argc, const char * const *argv)
{
	unsigned int i = 1;					// argv[0] is not of interest
//	outstr << argc << endl;

	while (i < argc) {
		bool found = false;
		for (unsigned int j = 0; j < optcount; j++) {
			const char *optid = alloptions[j]->flag;
			if (strcmp(optid, argv[i]) == 0) {
				//debug outstr << " found arg:" << i << " " << argv[i] << endl;
				const char *nextarg = (i < argc) ? argv[i + 1] : (const char *) 0;
				if (!alloptions[j]->copyvalue(argv[i], nextarg, i)) {
					outstr << "Error in option " << argv[i] << endl;
				}
				found = true;
				//debug outstr << " after found arg:" << i << " " << argv[i] << endl;
				break;
			}
		}
		if (!found) {
			if ((strlen(argv[i]) > 1) && (argv[i])[0] == '-') {
				outstr << "unknown option " << argv[i] << endl;
			} else {
				unhandledOptions[unhandledCounter] = argv[i];
				unhandledCounter++;
			}
		}
		i++;
	}
	return unhandledCounter;
}

void ProgramOptions::showhelp(ostream & outstr) const
{
	for (unsigned int i = 0; i < optcount; i++) {
		if (alloptions[i]->optional) outstr << "[";
		(void) outstr.width(20) ; outstr << alloptions[i]->flag << "\t : " << alloptions[i]->gettypename() << "\t : " << alloptions[i]->description   ;
		if (alloptions[i]->optional) outstr << "]";
		outstr << endl;
	}
}

void ProgramOptions::dumpunhandled(ostream & outstr) const
{
	if (unhandledCounter) {
		outstr << "unhandled parameters " << endl;
		for (unsigned int i = 0; i < unhandledCounter; i++) {
			outstr << '\t' << unhandledOptions[i] << endl;
		}
	} else {
		outstr << "no unhandled parameters" << endl;
	}
}

void ProgramOptions::add(OptionBase * op)
{
	alloptions[optcount] = op;
	optcount++;
}
 
