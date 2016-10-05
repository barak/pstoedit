/*
   miscutil.cpp : This file is part of pstoedit
   misc utility functions

   Copyright (C) 1998 Wolfgang Glunz, wglunz@geocities.com

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

#include I_stdio

// #ifdef _MSC_VER
// for getcwd ( at least for Visual C++)

#if defined(unix) || defined(__unix__) || defined(_unix) || defined(__unix) || defined(__EMX__)
#include <unistd.h>
#else
#include <direct.h>
#endif

#include I_stdlib

#include I_string_h


#if defined(_WIN32)
	#include <windows.h>
#endif


#if defined(unix) || defined(__unix__) || defined(_unix) || defined(__unix) || defined(__EMX__)
void convertBackSlashes(char* string) { unused(string); }
// nothing to do on systems with unix style file names ( / for directories)
#else
void convertBackSlashes(char* string) {

    char* c;

    while ((c = strchr(string,'\\')) != NULL)
       *c = '/';
}
#endif

#if defined(__STRICT_ANSI__)
// normally this is in stdio.h except if __STRICT_ANSI__ is defined (GNU specific)
extern "C" char *tempnam(const char *, const char *pfx);
#endif

#if defined(riscos) 
// rcw2: tempnam doesn't seem to be defined in UnixLib 3.7b for RiscOS
char *tempnam(const char *, const char *pfx)
{
	char tmp[1024];

	strcpy(tmp,"<Wimp$ScrapDir>.");
	strcat(tmp,pfx);
	return strdup(tmp);
}
#endif

#ifdef __WATCOMC__
// tempnam doesn't seem to be defined in Watcoms clibs
char *tempnam(const char *, const char *pfx)
{
	const char* path;
	char		tmp[1024];
	
	// simply take c: as temp dir, if environment not defined
	if( (path=getenv("TEMP")) == 0L &&
		(path=getenv("TMP")) == 0L )
	{
		strcpy(tmp,"C:\\");
	}
	else
	{
		// take temp dir from environment
		strcpy(tmp,path);
		strcat(tmp,"\\");
	}

	strcat(tmp,pfx);

	// remove all backslashes
	convertBackSlashes(tmp);	

	return strdup(tmp);
}
#endif

// #define BUGGYTEMPNAM

#if defined(DJGPP) && defined(BUGGYTEMPNAM)
// tempnam under DJGPP behaves different than on all other systems
char *tempnam(const char *, const char *pfx)
{
	return strdup(tmpnam(0));
}
#endif

char * full_qualified_tempnam(const char * pref)
{
#if defined (__BCPLUSPLUS__) || defined (__TCPLUSPLUS__)
/* borland has a prototype that expects a char * as second arg */
	char * filename = tempnam(0,(char *) pref);
#else
	char * filename = tempnam(0,pref);
#endif
	// W95: Fkt. tempnam() erzeugt Filename+Pfad
	// W3.1: es wird nur der Name zurueckgegeben

// rcw2: work round weird RiscOS naming conventions
#ifdef riscos
	return filename;
#else
	convertBackSlashes(filename);
	if ( (strchr(filename,'\\')==0) &&
	     (strchr(filename,'/') ==0) ) { // keine Pfadangaben..
		char cwd[400];
		getcwd(cwd,400); 
		char * result = new char [strlen(filename) + strlen(cwd) + 2];
		strcpy(result,cwd);
		strcat(result,"/");
		strcat(result,filename);
		free(filename);
		return result;
	} else {
		return filename;
	}
#endif
}

// a strdup which uses new instead of malloc
char * cppstrdup(const char * src)
{
	char * ret = new char [strlen(src) + 1];
	strcpy(ret,src);
	return ret;
}

#if defined(_WIN32)
char * tryregistry(HKEY hKey,LPCSTR subkeyn, LPCSTR key)
{
	HKEY subkey;
	const long ret = RegOpenKeyEx (
		hKey, // HKEY_LOCAL_MACHINE, //HKEY hKey,
		subkeyn, // LPCSTR lpSubKey,
		0l , // DWORD ulOptions,
		KEY_READ , // REGSAM samDesired,
		&subkey //PHKEY phkResult
    );
	if ( ret !=  ERROR_SUCCESS) {
		// errstream << "RegOpenKeyEx failed with error code " << ret << endl;
		return 0;
	} else {
		const int maxvaluelength = 1000;
		BYTE value[maxvaluelength];
		DWORD bufsize = maxvaluelength;
		DWORD valuetype ;
		const long retv = RegQueryValueEx (
				subkey, // HKEY_LOCAL_MACHINE, //HKEY hKey,
				key, // "SOFTWARE\\wglunz\\pstoedit\\plugindir", //LPCSTR lpValueName,
				NULL, // LPDWORD lpReserved,
				&valuetype, // LPDWORD lpType,
				value, // LPBYTE lpData,
				&bufsize // LPDWORD lpcbData
				);
		RegCloseKey(subkey);
		if ( retv !=  ERROR_SUCCESS) {
//			errstream << "RegQueryValueEx failed with error code " << retv << endl;
			return 0;
		} else {
//			errstream << "result is " << dirname << endl;
			return cppstrdup( (const char *) value);
		}
	}
}

#endif


char * getRegistryValue(ostream& errstream, const char * typekey, const char * key) {
#if defined(_WIN32)

//	CString subkey = CString("SOFTWARE\\wglunz\\") + CString(product);
	char subkeyn[1000];
	subkeyn[0] = '\0';
	strcat(subkeyn,"SOFTWARE\\wglunz\\");
	strcat(subkeyn,typekey);
	char * result = tryregistry(HKEY_CURRENT_USER,subkeyn,key);
	if (!result) result = tryregistry(HKEY_LOCAL_MACHINE ,subkeyn,key);
	return result;
#else
//
// UNIX version
// Just ask the environment
//
#if 0
//getenv version
	char envname[1000];
	envname[0] = '\0';
	strcat(envname,typekey);
	strcat(envname,"_");
	strcat(envname,key);
//	cout << "checking " << envname << endl;
	char * envvalue = getenv(envname);
	if (envvalue != 0) {
		char * r = cppstrdup(envvalue);
//		cout << "found " << r << endl;
		return r;
	} else	return 0;
#else
	char envname[1000];
	envname[0] = '\0';
	strcat(envname,typekey);
	strcat(envname,"/");
	strcat(envname,key);
//	cout << "checking " << envname << endl;
	const char * homedir = getenv("HOME");
	if (!homedir) return 0;
	const int len = strlen(homedir) + 20;
	char * obuf = new char[len ];
	strcpy(obuf,homedir);
	strcat(obuf,"/.pstoedit.reg");
	ifstream regfile(obuf);
//	cout << "opened " << obuf << endl;
	delete []obuf;
	if (!regfile) return 0;
	char line [1000];
	while (!regfile.eof()) {
		regfile.getline(line,1000);
//		cout << line << endl;
		if (strstr(line,envname)) {
			char * r = cppstrdup(line+strlen(envname)+1);
//			cout << "found " << r << endl;
			return r;
		}
	}
	return 0;
#endif
#endif
}

void copy_file(istream& infile,ostream& outfile) 
{
#if 1
	outfile << infile.rdbuf();
#else
// long version. should do the same as above
	unsigned char c;
	while (infile.get(c)) {
		outfile.put(c);
	}
#endif
}

ostream & operator <<(ostream & out, const Argv & a)
{
	for (unsigned int i = 0; i < (unsigned) a.argc; i++) {
		out << a.argv[i]; out << endl;
	}
	return out;
}


TempFile::TempFile()  
{ 
	tempFileName = tempnam(0,"pstmp"); 
	// cout << "constructed " << tempFileName << endl; 
}

TempFile::~TempFile() 
{ 	
	// cout << "detructed " << tempFileName << endl; 
	close(); 
	remove(tempFileName); 
	free(tempFileName); 
}

ofstream & TempFile::asOutput() 
{ 
	close(); 
	outFileStream.open(tempFileName); 
	if (outFileStream.fail()) cerr << "openening " << tempFileName << "failed " << endl;
	return outFileStream; 
}

ifstream & TempFile::asInput()  
{ 
	close(); 
	inFileStream.open(tempFileName);  
	if (outFileStream.fail()) cerr << "openening " << tempFileName << "failed " << endl;
	return  inFileStream; 
}

void TempFile::close() 
{ 
//#ifdef HAVE_IS_OPEN
//#define IS_OPEN(file) file.is_open()
//#else
//#define IS_OPEN(file) file
//#endif
// commented, since it also works portably if we call clear()

//	if (IS_OPEN(inFileStream))  {
		inFileStream.close(); 
//		if (inFileStream.fail()) cerr << "closing inFileStream failed " << endl;
//	}
	inFileStream.clear(); 
//	if (IS_OPEN(outFileStream))  {
		outFileStream.close(); 
//		if (outFileStream.fail()) cerr << "closing outFileStream  failed " << endl;
//	}
	outFileStream.clear(); 
}


#if 0
void freeconst(const void *ptr) 
{
	free((char *)ptr); // just to have the warning about casting away constness
		   // once (here)
}
#endif

unsigned long P_GetPathToMyself(const char *name,char * returnbuffer, unsigned long buflen)
{
#if defined(_WIN32)
	return	GetModuleFileName(GetModuleHandle(name),returnbuffer,buflen);
#else 
	if (*name == '/' ) { // starts with / 
		strcpy(returnbuffer,name);
		return strlen(returnbuffer);
	} else {
		const char * EnvPath = getenv("PATH");
		if (!EnvPath) return 0;
		char * path = cppstrdup(EnvPath);
		char * colon = path;
		char * lastbegin = path;
		while (*colon) {
			while (*colon && (*colon !=':')) colon++;
			if (*colon) {
				*colon='\0';
				RSString test(lastbegin);
				test+="/";
				test+=name;
				if (fileExists(test.value())) {
					strcpy(returnbuffer,test.value());
					delete [] path;
					return strlen(returnbuffer);
				}
			}
			colon++;
			lastbegin=colon;
		}
		delete [] path;
	}
	return 0; // not found
#endif
}

void errorMessage(const char * text)
{
#if defined(_WIN32)
	MessageBox(NULL, text, "pstoedit", MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
#else 
	cerr << text << endl;
#endif
}

// a very very simple resizing string
RSString::RSString(const char * arg) : 
			content(0),
			allocatedLength(0) { 
				if (arg) this->copy(arg); 
				// cerr << "{ constructed" << (void*) this << endl;
		}
RSString::RSString(const RSString & s) : content(0),allocatedLength(0) {
			assert(this != &s);
			this->copy(s.value());
		}
RSString::~RSString() { 
			// cerr << (void*) this << "deleted }" << endl;
#if !(defined(_MSC_VER) && defined(_DEBUG) )
			delete  content;
			// for some strange reasons we get an assertion violation during the
			// delete if the RSString destructor is called from with the
			// virtual destructor of a backend that was created from within
			// a .dll (plugin). So we have to live with this memory leak.
#endif

			content = 0; 
			allocatedLength = 0;
		}
		
const RSString& RSString::operator+= (const RSString &rs) {
	unsigned int newlen = strlen(content)+ strlen(rs.content) + 1;
	char * newstring = new char[newlen];
	strcpy(newstring,content);
	strcat(newstring,rs.content);
	delete[] content;
	content = newstring;
	allocatedLength = newlen;
	return *this;
}
//		const char * value() const { return content; }
void RSString::copy(const char *src) {
//			cerr << "copy " << src << " to " << (void *) this << endl;
			if ( content && ((strlen(src)+1) <= allocatedLength)) {
				// we have enough space
				::strcpy(content,src);
			} else {
				// resize
//				cerr << "content before delete is " << (void *) content << endl;
#if !(defined(_MSC_VER) && defined(_DEBUG) )
				if (content) delete content;
			// for some strange reasons we get an assertion violation during the
			// delete if the RSString destructor is called from with the
			// virtual destructor of a backend that was created from within
			// a .dll (plugin). So we have to live with this memory leak.
#endif
				
				allocatedLength = strlen(src) + 1;
				content = new char[allocatedLength];
//				cerr << "content after new is " << (void *) content << endl;
				::strcpy(content,src);
			}
		}

bool fileExists (const char * filename)
{
#ifdef HAVESTL
	std::ifstream test(filename);
    return test.is_open();
#else
	ifstream test(filename, ios::in	| ios::nocreate);
	// MSVC neede ios::nocreate if used for testing whether file exists
	return  test ? true : false;
#endif
}


//FontMapper::~FontMapper() { 
	// cerr << "destroying FontMapper" << endl;

//}

static void skipws(char * & lineptr) {
	while ((*lineptr != '\0') && (*lineptr == ' ') || (*lineptr == '\t')) lineptr++;
	return ;
}

static char * readword( char * & lineptr) {
	char * result = 0;
	if (*lineptr == '"') {
		result = strtok(lineptr,"\"");
	} else {
		result = strtok(lineptr,"\t ");
	}
	if (result) { 
		lineptr = result + strlen(result) + 1;
	}	
	return result;
}

void FontMapper::readMappingTable(ostream & errstream, const char * filename)
{
	if (!fileExists(filename)) {
		errstream << "Could not open font map file " << filename << endl;
		return;
	}
	ifstream inFile(filename);
	const int linesize = 255;
	char line[linesize];
	char save[linesize];
	unsigned int linenr = 0;
	while (!inFile.getline(line,linesize).eof()) {
		linenr++;
		strcpy(save,line);
#ifdef HAVESTL
		// Notes regarding ANSI C++ version (from KB)
		// istream::get( char* pch, int nCount, char delim ) is different in three ways: 
		// When nothing is read, failbit is set.
		// An eos is always stored after characters extracted (this happens regardless of the outcome).
		// A value of -1 for nCount is an error.

		// If the line contains just a \n then the failbit
		// is set in the ANSI version
		if (inFile.gcount() == 0) { inFile.clear(); continue; } 
#endif
		if (line[0] == '%') continue;
		char * lineptr = line;
		//skip initial spaces
		skipws(lineptr);
		char * original = readword(lineptr);
		skipws(lineptr);
		char * replacement = readword(lineptr);
		if (original && replacement) {
			// errstream << "\"" << original << "\" \"" << replacement <<"\""<< endl;
			insert(original, replacement);
		} else {
			errstream << "wrongly formatted line(" << linenr << ") found in fontmap: " << save << endl;
		}
	}
}
 
const char * FontMapper::mapFont(const RSString & fontname)
{
#if 0
	FontMapping * curEntry = firstEntry;
	// cerr << "Trying to remap font" << fontname << endl;
	while (curEntry != 0) {
		// cerr << "comparing with" << curEntry->original << endl;
		if (curEntry->original == RSString(fontname) ) {
			// cerr << "mapped to " << curEntry->replacement.value() << endl;
			return curEntry->replacement.value();
		}
		curEntry=curEntry->nextEntry;
	}
	return 0;
#endif
	const RSString * r = getValue(fontname);
	if (r) return r->value(); else return 0;
} 
 
 
