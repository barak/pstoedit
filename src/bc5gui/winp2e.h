#ifndef __WMF_h
#define __WMF_h

// OWL-Includes
#include <owl/combobox.h>
#include <owl/checkbox.h>
#include <owl/edit.h>
#include <owl/opensave.h>

//wogl #include "drvwmf.h"
#include <stdio.h>
#include "menu.h"


typedef POINT * PPOINT; //JW

#define NAMELENGTH 100
struct TMyTransferStruct {
	char NameEdit1[NAMELENGTH];
	char NameEdit2[NAMELENGTH];
	char NameEdit3[NAMELENGTH];
	char NameEdit4[NAMELENGTH];
	uint button1, button2, button3, button4;
   uint button5, button6, button7, button8;
	TComboBoxData ComboBoxData;
   char CBDataEdit[NAMELENGTH];
};

// Struktur zur Speicherung der Dialag-Antworten

struct TParameter {
	float magnification;
	float flatness;
	bool merging;
	bool drawtext;
	bool display;
	bool verbose;
	char format[10];
	char resolution_x[100];
	char resolution_y[100];
	bool backendonly;
	bool nomaplatin;
	bool noquit;
   bool enhanced;
   };


_CLASSDEF(TParaDlg)
class TParaDlg : public TDialog {


public:
	char Name1[NAMELENGTH] ;
	char Name2[NAMELENGTH] ;
	char Name3[NAMELENGTH] ;
	char Name4[NAMELENGTH] ;

   TParameter *para_zeiger;  // Struktur zum Speichern der Parameter
   TMyTransferStruct *ts_zeiger;

	TCheckBox *cb1,*cb2,*cb3,*cb4;
	TCheckBox *cb5,*cb6,*cb7,*cb8;
	TComboBox *Combo1;

	TParaDlg(TWindow *AParent, TResId , TParameter *, TMyTransferStruct& );
	~TParaDlg();

private:
  void FillBuffers();
	bool ValidField1();
	bool ValidField2();
	bool ValidField3();
	bool ValidField4();

	virtual bool CanClose();

};

#endif

