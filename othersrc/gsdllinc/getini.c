/* Copyright (C) 1998, Russell Lang.  All rights reserved.
  
	Find the path to the gsview32.ini. Special version for pstoedit.

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

#ifndef _WIN32
#include <dirent.h>
#endif

static void getini(int verbose,ostream & errstream, char* szIniFile,const char *INIFILEname,DWORD sizeofIniFile)
{

#if 0
	/* get path to EXE */
	GetModuleFileName(phInstance, szExePath, sizeof(szExePath));
	if ((p = strrchr(szExePath,'\\')) != (char *)NULL)
	    p++;
	else
	    p = szExePath;
	*p = '\0';
#endif

	/* get path to INI file */
	szIniFile[0] = '\0';
	/* strcpy(szIniFile, szExePath); */
#ifdef _WIN32
	DWORD version = GetVersion();
	bool is_win4 = false;
	if (LOBYTE(LOWORD(version)) >= 4) is_win4 = true;
	/* allow for user profiles */
	if (is_win4) {
	    LONG rc;
	    HKEY hkey;
	    DWORD keytype;
	    DWORD cbData;
	    DWORD fa;
	    /* Find the user profile directory */
	    rc = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\ProfileReconciliation", 0, KEY_READ, &hkey);
	    if (rc == ERROR_SUCCESS) {
			cbData = sizeofIniFile - strlen(INIFILEname);
//			cbData = sizeof(szIniFile)-sizeof(INIFILEname);
//			cout << "cbdata" << (int) cbData << endl;
			keytype =  REG_SZ;
			rc = RegQueryValueEx(hkey, "ProfileDirectory", 0, &keytype, (LPBYTE)szIniFile, &cbData);
			RegCloseKey(hkey);
	    }
	    if (rc == ERROR_SUCCESS) {
			if (verbose) { 
				errstream << "Found ProfileDirectory ! " << endl;
			}
			fa = GetFileAttributes(szIniFile);
			if ((fa != 0xffffffff) && (fa & FILE_ATTRIBUTE_DIRECTORY))
			  strcat(szIniFile, "\\");
			else
				szIniFile[0] = '\0';
			}
	    else {
		    /* If we didn't succeed, use the Windows directory */
		    szIniFile[0] = '\0';
	    }
	}
	if (szIniFile[0] == '\0') {
	    DWORD fa;
	    /* If we didn't succeed, try %USERPROFILE% */
	    char *p = getenv("USERPROFILE");
	    if (p && *p) {
			strcpy(szIniFile, p);
#ifdef __BORLANDC__
			OemToCharBuff(szIniFile, szIniFile, lstrlen(szIniFile));
#endif
			p = szIniFile + strlen(szIniFile) - 1;
			if ((*p == '\\') || (*p == '/'))
			   *p = '\0';
		/* check if USERPROFILE contains a directory name */
			fa = GetFileAttributes(szIniFile);
			if ((fa != 0xffffffff) && (fa & FILE_ATTRIBUTE_DIRECTORY))
				strcat(szIniFile, "\\");
			else
				szIniFile[0] = '\0';
	    }
	}
#else
	{
	    char *p = getenv("USERPROFILE");

	    if (p && *p) {
			strcpy(szIniFile, p); 
#ifdef __BORLANDC__
			OemToAnsiBuff(szIniFile, szIniFile, lstrlen(szIniFile));
#endif
			p = szIniFile + strlen(szIniFile) - 1;
			if ((*p == '\\') || (*p == '/'))
			   *p = '\0';
		/* check if USERPROFILE contains a directory name */

			DIR *d;
			d = opendir(szIniFile);
			if (d) {
			    closedir(d);
			    strcat(szIniFile, "\\");
			}
	    }
	}
#endif
	strcat(szIniFile, INIFILEname);
}
