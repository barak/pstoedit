/*
   winp2eap.cpp : This file is part of pstoedit
   Source file for implementation of TWinp2eApp (TApplication).

   Copyright (C) 1996 Jens Weber, rz47b7@PostAG.DE

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



#include <owl/pch.h>

#include <owl/buttonga.h>
#include <owl/statusba.h>
#include <classlib/cmdline.h>
#include <winsys/registry.h>
#include <stdio.h>

#include <io.h>
#include <dos.h>   //JW fuer Fileatrribute des Fehlerfiles
#include <fcntl.h> // JW dito



// AppExpert-includes
#include "winp2eap.h"
#include "winp2edc.h"                        // Definition of client class.
#include "winp2ead.h"                        // Definition of about dialog.

#ifdef PSTOEDIT_INPROC
// call pstoedit as subprogram
#include "pstoedit.h"
#endif
//{{TWinp2eApp Implementation}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TWinp2eApp, TApplication)
//{{TWinp2eAppRSP_TBL_BEGIN}}
  EV_COMMAND(CM_CONVERT1, CmConvert),
  EV_COMMAND(CM_PARAMETER, CmParameter),
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
//{{TWinp2eAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TWinp2eApp
//
TWinp2eApp::TWinp2eApp() : TApplication("Winp2e")
{

  // INSERT>> Your constructor code here.
}


TWinp2eApp::~TWinp2eApp()
{
  // INSERT>> Your destructor code here.
}


void TWinp2eApp::SetupSpeedBar(TDecoratedFrame* frame)
{
  ApxHarbor = new THarbor(*frame);

  // Create default toolbar New and associate toolbar buttons with commands.
  //
  TDockableControlBar* cb = new TDockableControlBar(frame);
  cb->Insert(*new TButtonGadget(CM_CONVERT1, CM_CONVERT1));
  cb->Insert(*new TButtonGadget(CM_PARAMETER, CM_PARAMETER));


  // Add caption and fly-over help hints.
  //
  cb->SetCaption("Toolbar");
  cb->SetHintMode(TGadgetWindow::EnterHints);

  ApxHarbor->Insert(*cb, alTop);
}


//--------------------------------------------------------
// TWinp2eApp
// ~~~~~
// Application intialization.
//
void TWinp2eApp::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWNORMAL : nCmdShow;

  TSDIDecFrame* frame = new TSDIDecFrame(0, GetName(), 0, true);
  frame->SetFlag(wfShrinkToClient);

  // Assign ICON w/ this application.
  //
  frame->SetIcon(this, IDI_SDIAPPLICATION);
  frame->SetIconSm(this, IDI_SDIAPPLICATION);

  // Menu associated with window and accelerator table associated with table.
  //
  frame->AssignMenu(IDM_SDI);

  // Associate with the accelerator table.
  //
  frame->Attr.AccelTable = IDM_SDI;

  SetupSpeedBar(frame);

  TStatusBar* sb = new TStatusBar(frame, TGadget::Recessed,
                                  TStatusBar::CapsLock        |
                                  TStatusBar::NumLock         |
                                  TStatusBar::ScrollLock);
  frame->Insert(*sb, TDecoratedFrame::Bottom);

  SetMainWindow(frame);

  frame->SetMenuDescr(TMenuDescr(IDM_SDI));
}


//--------------------------------------------------------
// TWinp2eApp
// ~~~~~
// Application instance initialization.
//
void TWinp2eApp::InitInstance()
{
  TApplication::InitInstance();

  ProcessCmdLine(lpCmdLine);
	// Defaultwerte vorgeben

	Parameter.magnification = 1.0;
	Parameter.flatness = 1.0;
	Parameter.merging = FALSE;
	Parameter.drawtext = FALSE;
	Parameter.display = FALSE;
	Parameter.verbose = FALSE;
	Parameter.enhanced = TRUE;
	strcpy(Parameter.format, "wmf");
	Parameter.backendonly = FALSE;
	Parameter.nomaplatin = FALSE;
	strcpy(Parameter.resolution_x,"300");
	strcpy(Parameter.resolution_y,"300");
	Parameter.noquit = FALSE;

}


//--------------------------------------------------------
// TWinp2eApp
// ~~~~~
// Process command line parameters.
//
void TWinp2eApp::ProcessCmdLine(char * CmdLine)
{
  TCmdLine cmd(CmdLine);

  while (cmd.Kind != TCmdLine::Done) {
    if (cmd.Kind == TCmdLine::Option) {
      if (strnicmp(cmd.Token, "unregister", cmd.TokenLen) == 0) {
        UnRegisterInfo();
        return;
      }
    }
    cmd.NextToken();
  }

  RegisterInfo();
}


//--------------------------------------------------------
// TWinp2eApp
// ~~~~~
// Register application info.
//
void TWinp2eApp::RegisterInfo()
{
  TAPointer<char> buffer = new char[_MAX_PATH];

  GetModuleFileName(buffer, _MAX_PATH);

  TRegKey(TRegKey::ClassesRoot, "Winp2e.Application\\DefaultIcon").SetDefValue(0, REG_SZ, buffer, strlen(buffer));
  strcat(buffer, ",1");
}


//--------------------------------------------------------
// TWinp2eApp
// ~~~~~
// Unregister application info.
//
void TWinp2eApp::UnRegisterInfo()
{
  TAPointer<char> buffer = new char[_MAX_PATH];

  GetModuleFileName(buffer, _MAX_PATH);

  TRegKey(TRegKey::ClassesRoot, "Winp2e.Application").DeleteKey("DefaultIcon");
  TRegKey(TRegKey::ClassesRoot, "Winp2e.Document.1").DeleteKey("DefaultIcon");

  TRegKey::ClassesRoot.DeleteKey("Winp2e.Application");
}



//{{TSDIDecFrame Implementation}}


TSDIDecFrame::TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection, TModule* module)
:
  TDecoratedFrame(parent, title, !clientWnd ? new TWinp2eDlgClient(0) : clientWnd, trackMenuSelection, module)
{
  // INSERT>> Your constructor code here.

}


TSDIDecFrame::~TSDIDecFrame()
{
  // INSERT>> Your destructor code here.

}


void TSDIDecFrame::SetupWindow()
{
  TDecoratedFrame::SetupWindow();
  TRect  r;
  GetWindowRect(r);

//  r.bottom += 60;
  r.left = 10;
  r.top = 10;
  r.right = 410;
  r.bottom = 210;

  SetWindowPos(0, r, SWP_NOZORDER | SWP_NOMOVE);

  // INSERT>> Your code here.

}


//--------------------------------------------------------
// TWinp2eApp
// ~~~~~~~~~~~
// Menu Help About Winp2e command
//
void TWinp2eApp::CmHelpAbout()
{
  // Show the modal dialog.
  //
  TWinp2eAboutDlg(MainWindow).Execute();
}

void TWinp2eApp::CmConvert()
{

  char tmpstring[200]; // wogl
  char MyCmdLine[1000]; // wogl
  char OutFileName[200]; // wogl
  MyCmdLine[0]=NULL;

  InFileData.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
  InFileData.SetFilter("PostScript  Files (*.ps *.eps)|*.ps;*.eps|All Files (*.*)|*.*|");
  InFileData.DefExt = "ps";
  *InFileData.FileName = 0;

   if (TFileOpenDialog(GetMainWindow(), InFileData).Execute() == IDOK) {
      int i = strlen(InFileData.FileName)-1;
		for( ;i>=0;i--)
			if(InFileData.FileName[i]=='.') break;
		if(i > 0) {
			strncpy(OutFileName,InFileData.FileName,i);
			OutFileName[i]=NULL;
			}
		else strcpy(OutFileName,InFileData.FileName);

		if (strcmp(Parameter.format,"wmf") == 0) {
        if(Parameter.enhanced==TRUE) strcat(OutFileName,".emf");
        else strcat(OutFileName,".wmf");
		}
		else if (strcmp(Parameter.format,"pdf") == 0) strcat(OutFileName,".pdf");
		else if (strcmp(Parameter.format,"cgm") == 0) strcat(OutFileName,".cgm");
		else if (strcmp(Parameter.format,"cgmt") == 0) strcat(OutFileName,".cgm");
		else if (strcmp(Parameter.format,"mif") == 0) strcat(OutFileName,".mif");
		else if (strcmp(Parameter.format,"ps") == 0) strcat(OutFileName,".pss");
		else if (strcmp(Parameter.format,"dxf") == 0) strcat(OutFileName,".dxf");
		else if (strcmp(Parameter.format,"java") == 0) strcat(OutFileName,".java");
		else if (strcmp(Parameter.format,"debug") == 0) strcat(OutFileName,".dbg");
		else if (strcmp(Parameter.format,"dump") == 0) strcat(OutFileName,".dbg");
		else  strcat(OutFileName,".xxx");

		// Zusammenbauen der Commandline

	  if(Parameter.magnification!=1.0) {
		  sprintf(tmpstring,"-s %f ",Parameter.magnification);
		  strcat(MyCmdLine,tmpstring); }

		if(Parameter.flatness!=1.0) {
		  sprintf(tmpstring,"-flat %f ",Parameter.flatness);
		  strcat(MyCmdLine,tmpstring); }

	  if(Parameter.merging==TRUE) strcat(MyCmdLine,"-merge ");
		if(Parameter.display==TRUE) strcat(MyCmdLine,"-dis ");
		if(Parameter.drawtext==TRUE) strcat(MyCmdLine,"-dt ");
		if(Parameter.backendonly==TRUE) strcat(MyCmdLine,"-bo ");
		if(Parameter.noquit==TRUE) strcat(MyCmdLine,"-nq ");
		if(Parameter.nomaplatin==TRUE) strcat(MyCmdLine,"-nomaptoisolatin1 ");

      sprintf(tmpstring,"-psargs -r%sx%s ",Parameter.resolution_x,Parameter.resolution_y);
		strcat(MyCmdLine,tmpstring);

		strcat(MyCmdLine," -f ");
		strcat(MyCmdLine,Parameter.format);
		if(Parameter.verbose==TRUE || Parameter.enhanced==TRUE) { // Options
      	strcat(MyCmdLine,":");
      	if(Parameter.enhanced==TRUE) strcat(MyCmdLine,"e"); // emf-File
      	if(Parameter.verbose==TRUE) strcat(MyCmdLine,"v"); // verbose
      }
		strcat(MyCmdLine," ");

		strcat(MyCmdLine,InFileData.FileName);
		strcat(MyCmdLine," ");
		strcat(MyCmdLine,OutFileName);

	  ::SetCursor(::LoadCursor(0,IDC_WAIT)); // Cursor -> Sanduhr

	  const int	status=WMFMain(MyCmdLine);

	  ::SetCursor(::LoadCursor(0,IDC_ARROW)); // Cursor -> Pfeil

	}

}

void TWinp2eApp::CmParameter()
{
  // Show the modal dialog.
  //
  TParaDlg(GetMainWindow(),IDD_PARAMETER, &Parameter, AnotherTransferStruct).Execute();

}

static int callgs(const char * commandline)
{
	    int gsresult = 0;
       STARTUPINFO MyStartupInfo; // Prozessdaten
       memset(&MyStartupInfo,'\0',sizeof(MyStartupInfo));
       MyStartupInfo.cb = sizeof(STARTUPINFO);
       PROCESS_INFORMATION MyProcessInformation;
       // wird von CreateProcess gefuellt
       DWORD gs_status = 0;
       MyStartupInfo.wShowWindow=SW_SHOWMINNOACTIVE;

       MessageBox(NULL,commandline,"Kommandozeile",MB_OK); // wogl NULL inserted

	    int status=(int)CreateProcess(
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
            if(status==FALSE) {  // Prozeß-Status konnte nicht ermittelt werden
              gsresult=-1;
              break;
            }

				if(gs_status==STILL_ACTIVE) { // gswin arbeitet noch...
						Sleep(500); // Wartezeit in msec
				}
            else break; // Prozess beendet
		 }
		if (gsresult != 0) {
			char tmpstring[100];
			sprintf(tmpstring,"Postscript interpreter failure: %d", gsresult);
			MessageBox(NULL,tmpstring,"ERROR",MB_OK);
		}

		return gsresult;
}

#if 0
static const char * whichPI(ostream & errstream)
{
	return "gswin32.exe"  ;
}
#endif

static const char * whichPI(ostream & errstream)
{
// determines which PostScript interpreter to use
#ifdef DEFAULTGS
#define str(x) #x
#define xstr(x) str(x)
	static const char * const defaultgs = xstr(DEFAULTGS);
#else
	static const char * const defaultgs = "gswin32.exe";
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

int TWinp2eApp::WMFMain(char *TheCmdLine)

{
#ifdef PSTOEDIT_INPROC
	char * argv[100];
	int argc=0;
	argv[argc] = "pstoedit"; argc++;


   const char * errfilename = "makeedit.err";
   ofstream errorfile;
#if 0
   unsigned attrib; // Fileattribute

   if (!_dos_getfileattr(errfilename,&attrib))  // File existiert schon
  	{ // read only ? aendern !
  	if (attrib & _A_RDONLY) {
   	attrib &= ~_A_RDONLY;
  		if (_dos_setfileattr(errfilename,attrib) != 0) {
      	MessageBox(NULL,errfilename,"Could not delete read-only attribute:",MB_OK|MB_ICONEXCLAMATION);
      	return -1;
			}
   	}
	}
#endif

	errorfile.open(errfilename,O_RDWR|O_CREAT|O_TEXT);
   if(errorfile.fail())	{
      MessageBox(NULL,errfilename,"Could not open for error and message  output:",MB_OK|MB_ICONEXCLAMATION);
      return -1; }

	{
	const unsigned int newmask = 0; // don't take away any permission
	unsigned int oldmask = umask( newmask );
   	// errorfile <<  "Oldmask = 0x%.4x\n" <<  oldmask  << endl;
   	errorfile <<  "Oldmask = " <<  oldmask  << endl;
	}

   errorfile << "Commandline " << TheCmdLine << endl;
	// first quick hack,  turn TheCmdLine into a vector
	char * cp = TheCmdLine;
	argv[argc] = cp; argc++;
	while (*cp) {
		if (*cp == ' ') {
			*cp = '\0'; cp++;
			while ((*cp) && (*cp == ' ')) cp++;
			if (*cp) {
				argv[argc] = cp; argc++;
			} else {
				break; // while
			}
		}
		cp++;
	}

	for (int i= 0; i < argc ; i++) {
		errorfile << "argv["<< i << "]: " << argv[i] << endl;
	}
	const int result = pstoedit(argc,argv,errorfile,callgs,whichPI);
#else
//
// call pstoedit as separate process. This is a sort of workaround
// for the still open problem of files beeing opened read-only 
// when beeing called directly as subprogram.
//
	const char progname[] = "pstoedit.exe -guimode";
	char * cmdline = (char *) malloc(strlen(TheCmdLine) + 1 + strlen(progname));
	strcpy(cmdline,progname);
	strcat(cmdline,TheCmdLine);
	const int result = callgs(cmdline);
	free(cmdline);
#endif
   if (result == 0) {
   	// MessageBox(NULL,"Convertion complete","INFO",MB_OK);
   } else {
   	// this message is given directly after the return from gs
      // MessageBox(NULL,"Convertion failed","INFO",MB_OK);
   }

	return result;
}


int OwlMain(int , char* [])
{
  TWinp2eApp   app;
  return app.Run();
}
