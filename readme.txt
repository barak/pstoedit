PSTOEDIT 
Copyright (C) 1993 - 2001 Wolfgang Glunz, wglunz@pstoedit.net

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
implemented other backends please send an email to wglunz@pstoedit.net.

If this programs saves you a lot of work consider sending a contribution
of any kind.

If you include this program on a CDROM, please send me a copy of the CD,
or if it goes with a book, of the book. 

You can also send a Linux pinguin (Tux) for my daughters. They love it.

My home address is:

	Dr. Wolfgang Glunz             
	81825 Muenchen / Germany  
	Josef Brueckl Str. 32    

Compiling pstoedit:
-------------------
You need a C++ compiler, e.g., g++ to compile pstoedit.

If you want to implement or extend a driver that supports PNG images,
you should have pnglib and zlib installed. Non of the standard
drivers (except the demo driver -f sample) support this at the moment, 
so is is not essentially needed. But in future I'm sure some drivers
will support PNG.
Further it is recommended to have libplotter installed. Then you
get a lot of additional formats for free.

If you have a Unix like system, try the following:
* cd config
* sh configure
* cd ../src; make clean; make; make install; 

If you have another system where running configure is not 
possible, e.g. Windows, do the following:
* cd to src
* edit the makefile.txt
	- change  BINDIR  according to your local environment
	- Uncomment the platform specific flags corresponding to your 
	  platform.
* nmake /f makefile.txt 	(in a DOS box)

* Note: pstoedit is configured to use the ANSI C++ headers. If your compiler
  support then ANSI C++ headers, you might try to compile with those. In
  this case #define HAVESTL in cppcomp.h

  A note regarding Visual C++. In principle the HAVESTL can be activated
  for Visual C++ (>= 5.0). However, I noticed a drastical increase both in 
  compilation time and object size. This is probably due to the template 
  based iostream library. Therefore I switched it off again. If you need
  the MetaPost backend under Windows, just turn the HAVESTL on and compile
  again.
  

There are several test cases included. To run them type `make test'.
This works under *nix only.

Installing pstoedit under Windows 9x/NT/2000:
---------------------------------------------

* create a directory "pstoedit" in parallel to the gsview directory, e.g.
  c:\gstools\pstoedit (if gsview is in c:\gstools\gsview )
* copy the pstoedit.exe and pstoedit.dll to that directory.
* if you plan to call pstoedit from a DOS box, you might consider to extend your
  PATH variable in order to avoid the need to call pstoedit always with with 
  full path name.
* if you have installed gsview 2.7 or later, you are done, if not you need
  to set the variable GS to point to either the gswin32.dll or gswin32c.exe found
  in the gsx.xx directory. In addition you might need to set GS_LIB (see GhostScript
  manual for details).

Installing pstoedit under OS/2:
-------------------------------

See the readme.os2 in the os2 directory. There you also find a makefile 
for OS/2.

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
	* bitmap images (just for some backends and only a subset of the PostScript image operators)
	* general fill patterns
	* clipping (only partially via the -sclip option)
	* ... 

Special note about the Java backend:
------------------------------------
The java backends generate a java source file that needs other files
in order to be compiled and usable. See the files java/java1/readme_java.txt 
and java/java2/readme_java2.htm for more details.

Extending pstoedit:
-------------------
To implement a new backend you can start from drvsampl.cc.
Please don't forget to send any new backend that might be of interest
for others as well to the author (wglunz@pstoedit.net) so that
it can be incorporated into future versions of pstoedit. Such
new backends will then be available with the GPL as well.

Acknowledgements:
-----------------

See manual page in pstoedit.htm and the changelog.htm for a list of contributors.

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
 
 
 
