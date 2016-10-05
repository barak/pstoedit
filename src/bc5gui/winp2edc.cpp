/*
   winp2edc.cpp : This file is part of pstoedit
   Source file for implementation of TWinp2eDlgClient (TDialog).

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

#include "winp2edc.h"


//{{TWinp2eDlgClient Implementation}}


//--------------------------------------------------------
// TWinp2eDlgClient
// ~~~~~~~~~~
// Construction/Destruction handling.
//
TWinp2eDlgClient::TWinp2eDlgClient(TWindow* parent, TResId resId, TModule* module)
:
  TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.
}


TWinp2eDlgClient::~TWinp2eDlgClient()
{
  Destroy();

  // INSERT>> Your destructor code here.
}
