PSTOEDIT 
Copyright (C) 1993 - 1999 Wolfgang Glunz, wglunz@geocities.com

pstoedit converts Postscript(TM) and PDF files to other vector graphic
formats so that they can be edited graphically. See pstoedit.htm or
index.htm for more details on which formats are supported by pstoedit.

The architecture of pstoedit consists of a PostScript frontend which
needs to call a PostScript interpreter like Ghostscript and the
individual backends which are plugged into a kind of framework.

This framework can be used independently from the PostScript frontend
from any other program. The framework provides a uniform interface to
all different backends. Get in contact with the author if you need
more information on how to use this framework.

If you just find this program useful, have made some improvements or 
implemented other backends please send an email to wogl@geocities.com.

If this programs saves you a lot of work consider sending a contribution
of any kind.

If you include this program on a CDROM, please send me a copy of the CD,
or if it goes with a book, of the book.

My home address is:

	Dr. Wolfgang Glunz             
	81825 Muenchen / Germany  
	Josef Brueckl Str. 32    

Compiling pstoedit:
-------------------
You need a C++ compiler, e.g., g++ to compile pstoedit.

* cd to src
* edit the makefile 
	- change  BINDIR  according to your local environment
	- Uncomment the platform specific flags corresponding to your 
	  platform.
* Note: pstoedit is configured to use the ANSI C++ headers if
  it "thinks" that the compiler supports them. Currently it assumes
  that GNU g++ supports the ANSI C++ headers and the STL. If your
  installation of g++ does not (e.g. because libstdc++ is missing)
  eliminate the #define HAVESTL in cppcomp.h. 
  See the head of drvlplot.cpp for more details about compiling with
  or without the GNU plotting library.

  A note regarding Visual C++. In principle the HAVESTL can be activated
  for Visual C++ (>= 5.0). However, I noticed a drastical increase both in 
  compilation time and object size. This is probably due to the template 
  based iostream library. Therefore I switched it off again. If you need
  the MetaPost backend under Windows, just turn the HAVESTL on and compile
  again.
  

* type: make clean; make; make install; (for *nix like systems)
	nmake /f makefile 	(in a DOS box)

There are several test cases included. To run them type `make test'.
This works under *nix only.

Installing pstoedit under Windows 9x/NT:
----------------------------------------

* create a directory "pstoedit" in parallel to the gsview and gs5.xx directories, e.g.
  c:\gstools\pstoedit.
* copy the pstoedit.exe and pstoedit.dll to this directory.
* if you plan to call pstoedit from a DOS box, you might consider to extend your
  PATH variable in order to avoid the need to call pstoedit always with with 
  full path name.
* if you have installed gsview 2.6 or later, you are done, if not you need
  to set the variable GS to point to either the gswin32.dll or gswin32c.exe found
  in the gsx.xx directory. In addition you might need to set GS_LIB (see GhostScript
  manual for details).


pstoedit and the -dSAFER option of Ghostscript:
-----------------------------------------------
GhostScript provides an option -dSAFER that disables all file access
functions of PostScript. Some administrators even install a wrapper
like to following instead of ghostscript directly
#!/bin/sh
gs.real -dSAFER $*

So when a user uses gs he/she actually runs this script. However,
pstoedit needs to have access to files for it's operation. So
it is not possible to use this wrapper for gs in combination with pstoedit.
You would get an error message like "Error: /invalidfileaccess in (w)".

As an alternative the following can be done:
1. Install the binary of pstoedit as pstoedit.real
2. Create the following wrapper and name it pstoedit
#!/bin/sh
GS=gs.real
export GS
pstoedit.real -include /??????/local/safer.ps $*

A template for safer.ps can be found in the misc subdirectory.
This way pstoedit can open all the file it needs (the input file and an
output file). After that then -- via the included file -- all files
operations are disabled and the input file is processed. Any file operation
that is executed be the user's PostScript file is disabled this way.


Using pstoedit:
---------------
Before you can use pstoedit you must have a working installation
of GhostScript (either GNU or Aladdin).

The rest is descibed in the manual page in /pstoedit.htm.

pstoedit works reasonable with PostScript files containing
	* line drawings
	* text with standard fonts 

Try to run it on golfer.ps or tiger.ps that comes with ghostscript, e.g., 
pstoedit -f <your format> <local path where GhostScript is installed>/examples/tiger.ps tiger.<suffix>

In particular pstoedit does not support
	* bitmap images (just for xfig and MIF)
	* general fill patterns
	* clipping
	* ... 

Special note about the Java backend:
------------------------------------
The java backend generates a java source file that needs other files
in order to be compiled and usable. See the file java/readme_java.txt 
for more details.

Extending pstoedit:
-------------------
To implement a new backend you can start from drvsampl.cc.
Please don't forget to send any new backend that might be of interest
for others as well to the author (wglunz@geocities.com) so that
it can be incorporated into future versions of pstoedit. Such
new backends will then be available with the GPL as well.

Acknowledgements:
-----------------

See manual page in pstoedit.htm for a list of contributors.

License: 
--------

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



----------------------------------------------------------------------------
 
