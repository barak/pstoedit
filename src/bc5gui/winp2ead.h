//----------------------------------------------------------------------------
//  Project Winp2e
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    Winp2e Application
//  FILE:         winp2ead.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TWinp2eAboutDlg (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(winp2ead_h)              // Sentry, use file only if it's not already included.
#define winp2ead_h

#include <owl/static.h>

#include "winp2eap.rh"                  // Definition of all resources.


//{{TDialog = TWinp2eAboutDlg}}
class TWinp2eAboutDlg : public TDialog {
  public:
    TWinp2eAboutDlg(TWindow* parent, TResId resId = IDD_ABOUT, TModule* module = 0);
    virtual ~TWinp2eAboutDlg();

//{{TWinp2eAboutDlgVIRTUAL_BEGIN}}
  public:
    void SetupWindow();
//{{TWinp2eAboutDlgVIRTUAL_END}}
};    //{{TWinp2eAboutDlg}}


// Reading the VERSIONINFO resource.
//
class TProjectRCVersion {
  public:
    TProjectRCVersion(TModule* module);
    virtual ~TProjectRCVersion();

    bool GetProductName(LPSTR& prodName);
    bool GetProductVersion(LPSTR& prodVersion);
    bool GetCopyright(LPSTR& copyright);
    bool GetDebug(LPSTR& debug);

  protected:
    uint8 far*  TransBlock;
    void far*   FVData;

  private:
    // Don't allow this object to be copied.
    //
    TProjectRCVersion(const TProjectRCVersion&);
    TProjectRCVersion& operator = (const TProjectRCVersion&);
};


#endif  // winp2ead_h sentry.
