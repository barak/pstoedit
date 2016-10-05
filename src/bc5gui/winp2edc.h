//----------------------------------------------------------------------------
//  Project Winp2e
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    Winp2e Application
//  FILE:         winp2edc.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TWinp2eDlgClient (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(winp2edc_h)              // Sentry, use file only if it's not already included.
#define winp2edc_h

#include "winp2eap.rh"                  // Definition of all resources.


//{{TDialog = TWinp2eDlgClient}}
class TWinp2eDlgClient : public TDialog {
  public:
    TWinp2eDlgClient(TWindow* parent, TResId resId = IDD_CLIENT, TModule* module = 0);
    virtual ~TWinp2eDlgClient();

};    //{{TWinp2eDlgClient}}


#endif  // winp2edc_h sentry.
