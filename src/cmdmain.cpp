/*
   cmdmain.cpp : This file is part of pstoedit
   main program for command line usage

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

#include <stdlib.h>
// rcw2: work round case insensitivity in RiscOS
#ifdef riscos
  #include "unix:string.h"
#else
  #include <string.h>
#endif
#include <iostream.h>
#include "pstoedit.h"

// rcw2:Can't put angle brackets onto commandline for g++ on RiscOS :(
#ifdef riscos
  #ifndef DEFAULTGS
  #define DEFAULTGS <GhostScript$Dir>.ghost
  #endif
#endif

static const char * whichPI(ostream & errstream)
{
// determines which PostScript interpreter to use
#ifdef DEFAULTGS
#define str(x) #x
#define xstr(x) str(x)
	static const char * const defaultgs = xstr(DEFAULTGS);
#else
	static const char * const defaultgs = "";
#endif
	const char * gstocall = getenv("GS");
	if ( gstocall  == 0 ) {
		// env var GS not set, so try default
		if ( strlen(defaultgs) > 0 ) {
			gstocall = defaultgs;
		} else {
			errstream << "Fatal: don't know which interpreter to call. "
		     	     << "Either setenv GS or compile again with -DDEFAULTGS=..." << endl;
			exit(1);
		}
	}
	return gstocall;
}

#if 0
static const char * whichPI(ostream & errstream) 
{
	return "gswin32.exe"  ;
}
#endif

#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
static int callgs(const char * commandline)
{
	int gsresult = 0;

	STARTUPINFO MyStartupInfo; // Prozessdaten
	memset(&MyStartupInfo,'\0',sizeof(MyStartupInfo));
	MyStartupInfo.cb = sizeof(STARTUPINFO);
	MyStartupInfo.wShowWindow=SW_SHOWMINNOACTIVE;

	PROCESS_INFORMATION MyProcessInformation;
         // wird von CreateProcess gefuellt
         DWORD gs_status = 0;

//            MessageBox(NULL,commandline,"Kommandozeile",MB_OK); // wogl NULL inserted

	  int 	status=(int)CreateProcess(
              NULL, // Application Name -> wird in der naechsten Zeile mitgegeben
              (LPSTR)commandline,
              NULL, // Prozessattribute (NULL == Default)
              NULL, // Thread-Atrribute (Default)
              FALSE, // InheritHandles
              CREATE_NEW_PROCESS_GROUP, // CreationFlags
              NULL, // Environment (NULL: same as calling-process)
              NULL, // Current Directory (NULL: same as calling process)
              (LPSTARTUPINFO)(&MyStartupInfo), // Windows-state at Startup
 				  // das Fenster der aufzurufenden Funktion wird verkleinert + nicht aktiv
				  // dargestellt
             (LPPROCESS_INFORMATION)(&MyProcessInformation)
            );

            if(status==TRUE) gsresult=0;
            else gsresult=-1; // Failure

         while(0==0) {
         	status=GetExitCodeProcess(MyProcessInformation.hProcess, &gs_status);
            if(status==FALSE) {  // Proze¤-Status konnte nicht ermittelt werden
              gsresult=-1;
              break;  }

				if(gs_status==STILL_ACTIVE) { // gswin arbeitet noch...
						Sleep(500); // Wartezeit in msec
				}
            else break; // Prozess beendet
			}
	if (gsresult != 0) {
		char tmpstring[100];
		sprintf(tmpstring,"Interpreter failure: %d", gsresult);
		//MessageBox(NULL,tmpstring,"ERROR",MB_OK);
		cerr << tmpstring << endl;
	}
	return gsresult;
}
#else
static int callgs(const char * commandline) { return system(commandline); }
#endif

int main(int argc, char **argv) 
{
	// on UNIX like systems, we can use cerr as error stream
	// and the system-function to call GhostScript
	return pstoedit(argc,argv,cerr,callgs,whichPI);
}
