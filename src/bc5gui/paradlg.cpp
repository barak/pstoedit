/*
   paradlg.cpp : This file is part of pstoedit
   parameter dialog

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


#include "winp2e.h"
#include "config.h"


// TParaDlg-Konstruktor: Instanzieren von Edit-Feldern
// zur Parametereingabe

TParaDlg::TParaDlg(TWindow* AParent, TResId resId, TParameter *pzeiger,
									TMyTransferStruct& ts)
				 :TDialog(AParent, resId), TWindow(AParent)
{
	para_zeiger = pzeiger;
   ts_zeiger = &ts;

  //Defaultwerte vorbereiten
	sprintf(ts.NameEdit1,"%f",pzeiger->magnification);
	sprintf(ts.NameEdit2,"%f",pzeiger->flatness);
	sprintf(ts.NameEdit3,"%s",pzeiger->resolution_x);
	sprintf(ts.NameEdit4,"%s",pzeiger->resolution_y);

	// HINWEIS: die Zuordnung ts.button n zu den CheckBoxes n
	// ist nicht explizit festgelegt
	if(para_zeiger->merging) ts.button1=BF_CHECKED;
	else ts.button1=BF_UNCHECKED;
	if(para_zeiger->drawtext) ts.button2=BF_CHECKED;
	else ts.button2=BF_UNCHECKED;
	if(para_zeiger->display) ts.button3=BF_CHECKED;
	else ts.button3=BF_UNCHECKED;
	if(para_zeiger->verbose) ts.button4=BF_CHECKED;
	else ts.button4=BF_UNCHECKED;

	if(para_zeiger->backendonly) ts.button5=BF_CHECKED;
	else ts.button5=BF_UNCHECKED;
	// notice nomaplatin is used inversely !!
	// the button says 'map to iso latin1'
	if(para_zeiger->nomaplatin) ts.button6=BF_UNCHECKED;
	else ts.button6=BF_CHECKED;
	if(para_zeiger->noquit) ts.button7=BF_CHECKED;
	else ts.button7=BF_UNCHECKED;
	if(para_zeiger->enhanced) ts.button8=BF_CHECKED;
	else ts.button8=BF_UNCHECKED;

	new TEdit(this, ID_NAMEEDIT1,sizeof(ts.NameEdit1) );
	new TEdit(this, ID_NAMEEDIT2,sizeof(ts.NameEdit2) );
	new TEdit(this, ID_NAMEEDIT3,sizeof(ts.NameEdit3) );
	new TEdit(this, ID_NAMEEDIT4,sizeof(ts.NameEdit4) );

	cb1 = new TCheckBox(this, ID_CHECK1, NULL);
	cb2 = new TCheckBox(this, ID_CHECK2, NULL);
	cb3 = new TCheckBox(this, ID_CHECK3, NULL);
	cb4 = new TCheckBox(this, ID_CHECK4, NULL);
	cb5 = new TCheckBox(this, ID_CHECK5, NULL);
	cb6 = new TCheckBox(this, ID_CHECK6, NULL);
	cb7 = new TCheckBox(this, ID_CHECK7, NULL);
	cb8 = new TCheckBox(this, ID_CHECK8, NULL);

	Combo1 = new TComboBox(this, ID_LISTBOX, 10);

#if WITHIDRAW
	ts.ComboBoxData.AddString("idraw");
#endif
#if WITHTGIF
	ts.ComboBoxData.AddString("tgif");
#endif
#if WITHRPL
	ts.ComboBoxData.AddString("rpl");
#endif
#if WITHLWO
	ts.ComboBoxData.AddString("lwo");
#endif
#if WITHRIB
	ts.ComboBoxData.AddString("rib");
#endif
#if WITHMIF
	ts.ComboBoxData.AddString("mif");
#endif
#if WITHFIG
	ts.ComboBoxData.AddString("fig");
#endif
#if WITHPDF
	ts.ComboBoxData.AddString("pdf");
#endif
#if WITHGNUPLOT
	ts.ComboBoxData.AddString("gnuplot");
#endif
	ts.ComboBoxData.AddString("ps");
	ts.ComboBoxData.AddString("debug");
	ts.ComboBoxData.AddString("dump");
#if WITHMET
	ts.ComboBoxData.AddString("met");
#endif
#if WITHWMF
	ts.ComboBoxData.AddString("wmf");
#endif
#if WITHDXF
	ts.ComboBoxData.AddString("dxf");
#endif
#if WITHCGM
	ts.ComboBoxData.AddString("cgm");
	ts.ComboBoxData.AddString("cgmt");
#endif
#if WITHJAVA
	ts.ComboBoxData.AddString("java");
#endif
#if WITHSAMPLE
	ts.ComboBoxData.AddString("sample");
#endif

	SetTransferBuffer(&ts);

	// Selektion vorgeben (aus para_zeiger->format)
	ts.ComboBoxData.SelectString(para_zeiger->format);
}

TParaDlg::~TParaDlg()
{

	delete Combo1;
   (ts_zeiger->ComboBoxData).Clear();

}



bool TParaDlg::CanClose()

{

  FillBuffers();

  Combo1->GetString(para_zeiger->format, Combo1->GetSelIndex());

	if(cb1->GetCheck())
		para_zeiger->merging=TRUE;
	else
    para_zeiger->merging=FALSE;


	if(cb2->GetCheck())
		para_zeiger->drawtext=TRUE;
	else
    para_zeiger->drawtext=FALSE;

	if(cb3->GetCheck())
		para_zeiger->display=TRUE;
	else
    para_zeiger->display=FALSE;

	if(cb4->GetCheck())
		para_zeiger->verbose=TRUE;
	else
    para_zeiger->verbose=FALSE;

	if(cb5->GetCheck())
		para_zeiger->backendonly=TRUE;
	else
    para_zeiger->backendonly=FALSE;

	// notice nomaplatin is used inversely !!
	// the button says 'map to iso latin1'
	if(cb6->GetCheck())
		para_zeiger->nomaplatin=FALSE;
	else
    para_zeiger->nomaplatin=TRUE;

	if(cb7->GetCheck())
		para_zeiger->noquit=TRUE;
	else
    para_zeiger->noquit=FALSE;

	if(cb8->GetCheck())
		para_zeiger->enhanced=TRUE;
	else
    para_zeiger->enhanced=FALSE;

	if ( (Name1[0]) && (Name2[0])  )
	{
		if ( ValidField1() && ValidField2() && ValidField3() && ValidField4()) {
			return TRUE; }
  }
  else
    MessageBox("Please fullfill complete","Input Error",
					MB_OK | MB_ICONEXCLAMATION);
  return FALSE;

}


void TParaDlg::FillBuffers()
{

	 GetDlgItemText(ID_NAMEEDIT1, Name1, NAMELENGTH);
	 GetDlgItemText(ID_NAMEEDIT2, Name2, NAMELENGTH);
	 GetDlgItemText(ID_NAMEEDIT3, Name3, NAMELENGTH);
	 GetDlgItemText(ID_NAMEEDIT4, Name4, NAMELENGTH);

}

bool TParaDlg::ValidField1()

// Vergroesserungsfaktor

{
  float d;

  if(!(sscanf(Name1,"%f",&d))) return FALSE;

  para_zeiger->magnification = d;


  if(d==0.0)  {
			MessageBox("Magnification must have a nonzero value","Input Error",
				MB_OK | MB_ICONEXCLAMATION);
			return FALSE;  }

	return TRUE;

}

bool TParaDlg::ValidField2()

// Flatness

{

	float d;

  if(!(sscanf(Name2,"%f",&d))) return FALSE;

  para_zeiger->flatness = d;


  if(d==0.0)  {
			MessageBox("Flatness must have a nonzero value","Input Error",
				MB_OK | MB_ICONEXCLAMATION);
      return FALSE;  }

	return TRUE;

}

bool TParaDlg::ValidField3()

// Resolution-Option (x)

{
   int d;

   if(!(sscanf(Name3,"%d",&d))) {
			MessageBox("x-Resolution must be an integer","Input Error",
				MB_OK | MB_ICONEXCLAMATION);
         return FALSE; }

   if(d<=0)  {
		MessageBox("x-Resolution must be greater zero","Input Error",
				MB_OK | MB_ICONEXCLAMATION);
      return FALSE;  }

	strcpy(para_zeiger->resolution_x,Name3);

	return TRUE;

}

bool TParaDlg::ValidField4()

// Resolution-Option (y)

{
   int d;

   if(!(sscanf(Name4,"%d",&d))) {
			MessageBox("y-Resolution must be an integer","Input Error",
				MB_OK | MB_ICONEXCLAMATION);
         return FALSE; }

   if(d<=0)  {
		MessageBox("y-Resolution must be greater zero","Input Error",
				MB_OK | MB_ICONEXCLAMATION);
      return FALSE;  }

	strcpy(para_zeiger->resolution_y,Name4);

	return TRUE;

}

