//----------------------------------------------------------------------------
//  Project Wmfconv
//  
//  Copyright © 1996. Alle Rechte vorbehalten.
//
//  SUBSYSTEM:    Wmfconv Application
//  FILE:         winp2eap.h
//  AUTHOR:       Jens Weber
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TWmfconvApp (TApplication).
//
//----------------------------------------------------------------------------
#if !defined(winp2eap_h)              // Sentry, use file only if it's not already included.
#define winp2eap_h

#include <owl/controlb.h>
#include <owl/docking.h>
#include <owl/opensave.h>


#include "winp2eap.rh"            // Definition of all resources.
#include "winp2e.h"            


//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = TSDIDecFrame}}
class TSDIDecFrame : public TDecoratedFrame {
  public:
    TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection = false, TModule* module = 0);
    ~TSDIDecFrame();

//{{TWmfconvAppVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{TWmfconvAppVIRTUAL_END}}
};    //{{TSDIDecFrame}}


//{{TApplication = TWmfconvApp}}
class TWinp2eApp : public TApplication {
  private:

    void SetupSpeedBar(TDecoratedFrame* frame);
    void ProcessCmdLine(char * CmdLine);
    void RegisterInfo();
    void UnRegisterInfo();

  public:
    TWinp2eApp();
    virtual ~TWinp2eApp();

    THarbor*        ApxHarbor;

    TOpenSaveDialog::TData InFileData;                    // Data to control open/saveas standard dialog.

    //JW
	 TParameter Parameter;
    TMyTransferStruct AnotherTransferStruct;
//wogl	 char OutFileName[200];
//wogl	 char MyCmdLine[1000];
//wogl 	 int i, status;
	 int WMFMain(char *);

   //{{TWmfconvAppVIRTUAL_BEGIN}}
  public:
    virtual void InitMainWindow();
    virtual void InitInstance();
//{{TWmfconvAppVIRTUAL_END}}

//{{TWmfconvAppRSP_TBL_BEGIN}}
  protected:
    void CmConvert();
    void CmParameter();
    void CmHelpAbout();
//{{TWmfconvAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TWinp2eApp);
};    //{{TWmfconvApp}}


#endif  // winp2eap_h sentry.
