#ifndef miscutil_h
#define miscutil_h
/*
   miscutil.h : This file is part of pstoedit
   header declaring misc utility functions

   Copyright (C) 1998 - 1999 Wolfgang Glunz, wglunz@geocities.com

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
#include "cppcomp.h"

#include I_iostream
#include I_ostream
#include I_istream
#include I_fstream
#include I_string_h

USESTD

#include <assert.h>

// used to eliminate compiler warnings about unused parameters
inline void unused(const void * const) { }

#if defined(riscos) || defined(__WATCOMC__)
// rcw2: tempnam doesn't seem to be defined in UnixLib 3.7b for RiscOS 
// and the Watcom clibs.
  char *tempnam(const char *, const char *);
#endif

#if defined(_WIN32)
const char directoryDelimiter = '\\';
#else
const char directoryDelimiter = '/';
#endif

bool fileExists (const char * filename);
char * full_qualified_tempnam(const char * pref);
void convertBackSlashes(char* string);

char * cppstrdup(const char * src);

char * getRegistryValue(ostream& errstream, const char * typekey, const char * key);
unsigned long P_GetPathToMyself(const char *name, char * returnbuffer, unsigned long buflen);
void errorMessage(const char * text); // display an error message (cerr or msgbox)

void copy_file(istream& infile,ostream& outfile) ;

// A temporary file, that is automatically removed after usage
class TempFile {
public:
	TempFile()  ;
	~TempFile() ;
	ofstream & asOutput();
	ifstream & asInput();
private:
	void close() ;
	char * tempFileName;
	ofstream outFileStream;
	ifstream inFileStream;
};

class Argv {
	enum { maxargs=1000 };
public:
	unsigned int argc;
	char * argv[maxargs];

	Argv() : argc(0) { for (unsigned int i = 0; i< (unsigned) maxargs; i++)  argv[i] = 0; }
	~Argv() { for (unsigned int i = 0; i< (unsigned) argc &&  i< (unsigned) maxargs ; i++) delete [] argv[i]; }

	void addarg(const char * arg) { assert(argc<maxargs); argv[argc] = cppstrdup(arg); argc++; }
};
ostream & operator <<(ostream & out, const Argv & a);


// a very very simple resizing string
// since STL is not yet available on all systems / compilers
class  RSString {
public:
		
	RSString(const char * arg = 0);
	RSString(const RSString & s);
	~RSString();	
	const char * value() const { return content; }
	void copy(const char *src) ;
	const RSString & operator = (const RSString & rs) {
		if (&rs != this) {
			copy(rs.value());
		}
		return *this;
	}
	const RSString& operator+= (const RSString &rs);
	bool operator==(const RSString & rs) const
	{ 	return strcmp(content,rs.content) == 0;	}
	bool operator!=(const RSString & rs) const 
	{ 	return strcmp(content,rs.content) != 0; }
	bool operator<(const RSString & rs) const
	{	return strcmp(content,rs.content) < 0; }
	friend ostream & operator<<(ostream & out,const RSString &outstring)
	{	return out << outstring.content; }

private:
	char * content;
	unsigned int allocatedLength;
};

//#define BUGGYGPP

#ifndef BUGGYGPP
template <class T> 
#else /* BUGGYGPP */
template <class T,class K_Type,class V_Type> 
#endif /* BUGGYGPP */
class Mapper {
public:
	Mapper() : firstEntry(0) {};
	~Mapper() {
		while (firstEntry != 0) {
			T * nextEntry = firstEntry->nextEntry;
			delete firstEntry;
			firstEntry=nextEntry;
		}
	}
public:
#ifndef BUGGYGPP
// define BUGGYGPP if your compiler complains about syntax error here.
// see above
	void insert(const T::K_Type & key, const  T::V_Type& value) {
#else /* BUGGYGPP */
	void insert(const K_Type & key, const  V_Type & value) {
#endif /* BUGGYGPP */
		firstEntry = new T(key,value,firstEntry);
	}
#ifndef BUGGYGPP
	const  T::V_Type* getValue(const  T::K_Type & key) {
#else /* BUGGYGPP */
	const  V_Type* getValue(const  K_Type & key) {
#endif /* BUGGYGPP */
		T * curEntry = firstEntry;
		while (curEntry != 0) {
			if (curEntry->key() == key ) {
				return &curEntry->value();
			}
			curEntry=curEntry->nextEntry;
		}
		return 0;
	}
	T * firstEntry;
};

template <class K, class V> 
class KeyValuePair
{
public:
		typedef K K_Type;
		typedef V V_Type;
		KeyValuePair(const K_Type & k,const V_Type & v, KeyValuePair<K,V> * next = 0):
			key_(k), value_(v), nextEntry(next) {}
		const K_Type & key()	const { return key_;}
		const V_Type & value()	const { return value_;}
private:
		K_Type key_;
		V_Type value_;
public:
		KeyValuePair<K,V> * nextEntry;
};


typedef KeyValuePair<RSString,RSString> FontMapping ;

#ifndef BUGGYGPP
class FontMapper: public Mapper<FontMapping>
#else /* BUGGYGPP */
class FontMapper: public Mapper<FontMapping,RSString,RSString>
#endif /* BUGGYGPP */
{
public:
	void readMappingTable(ostream & errstream,const char * filename);
	const char * mapFont(const RSString & fontname);
};

#endif
 
