#ifndef _POPTIONS_H
#define _POPTIONS_H
/*
   poptions.h : This file is part of pstoedit
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

#ifndef cppcomp_h
#include "cppcomp.h"
#endif

#include I_iostream
#include I_ostream
#include I_istream

USESTD

class DLLEXPORT IntValueExtractor {
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, int &result);
	static const char *gettypename() ;
};

class DLLEXPORT DoubleValueExtractor {
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, double &result) ;
	static const char *gettypename() ;
};


class DLLEXPORT CharacterValueExtractor {
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, char &result) ;
	static const char *gettypename() ;
};

class DLLEXPORT BoolBaseExtractor {
public:
	static const char *gettypename();
};

class DLLEXPORT BoolInvertingExtractor : public BoolBaseExtractor {
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, bool &result) ;
};

class DLLEXPORT BoolFalseExtractor : public BoolBaseExtractor{
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, bool &result) ;
};

class DLLEXPORT BoolTrueExtractor : public BoolBaseExtractor{
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, bool &result);
};



class DLLEXPORT OptionBase {
public:
	OptionBase(bool optional_p,const char *flag_p, const char *description_p):
	  flag(flag_p),
	  description(description_p),
	  optional(optional_p) {
	};
	virtual ~OptionBase() {}
	virtual ostream & writevalue(ostream & out) = 0;
	virtual bool copyvalue(const char *optname, const char *valuestring, unsigned int &currentarg) = 0;
	virtual const char *gettypename() = 0;

	//lint -esym(1540,OptionBase::flag) // not freed
	//lint -esym(1540,OptionBase::description) // not freed
	const char * const flag;		// -bf
	const char * const description;	// help text
	bool optional;

private:
	OptionBase();// disabled
	OptionBase(const OptionBase&);// disabled
	const OptionBase& operator=(const OptionBase&); // disabled
};

template <class ValueType, class ExtractorType >
class Option : public OptionBase {
public:
	Option < ValueType, ExtractorType > (bool optional_p,const char *flag_p, const char *description_p, const ValueType & initialvalue)	:
		OptionBase(optional_p,flag_p, description_p),
		value(initialvalue) {
	};
	Option < ValueType, ExtractorType > (bool optional_p, const char *flag_p, const char *description_p )	:
		OptionBase(optional_p,flag_p, description_p) 
	{
			//lint -esym(1401,*::value) // not initialized - we use the default ctor here
	};
	virtual ostream & writevalue(ostream & out);

	virtual bool copyvalue(const char *optname, const char *valuestring, unsigned int &currentarg) {
		return ExtractorType::getvalue(optname, valuestring, currentarg, value);
	}
	virtual const char *gettypename() {
		return ExtractorType::gettypename();
	}
	//lint -save -esym(1539,OptionBase::optional) // not assigned a value
	//lint -e(1763)
  	ValueType & operator()() { return value; }
	const ValueType & operator()() const { return value; }
	operator ValueType () const { return value; }
	/* const ValueType & */ void operator =(const ValueType & arg) { /* return */ value = arg; } //  cannot return a reference, because char*::operator= doesn't
	//lint -restore
	bool operator !=(const ValueType & arg) const { return value != arg; }
	bool operator ==(const ValueType & arg) const { return value == arg; }
	bool operator !() const { return !value ; }


	ValueType value;

private:
	Option();// disabled
	Option(const Option&);// disabled
	const Option& operator=(const Option&); // disabled
};


template <class ValueType, class ExtractorType >
ostream & Option<ValueType, ExtractorType>::writevalue(ostream & out) {
		out << value;
		return out;
}

class DLLEXPORT ProgramOptions {
public:
	ProgramOptions() : unhandledCounter(0), optcount(0)   { unhandledOptions[0]=0;alloptions[0]=0; };

	~ProgramOptions() {}
	unsigned int parseoptions(ostream & outstr, unsigned int argc, const char * const*argv) ;
	void showhelp(ostream & outstr) const ;
	void dumpunhandled(ostream & outstr) const ;	
	void showvalues(ostream & outstr) const ;

  protected:
	void add(OptionBase * op) ;

  public:
	unsigned int unhandledCounter;
	const char *unhandledOptions[100];

  private:
	unsigned int optcount;
	OptionBase *alloptions[100];

  private:
	ProgramOptions(const ProgramOptions&);// disabled
	const ProgramOptions& operator =(const ProgramOptions&); // disabled
};

#endif
 
 
