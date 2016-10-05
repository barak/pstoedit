PSTOEDIT 
Copyright (C) 1993,1994,1995,1996,1997 Wolfgang Glunz, Wolfgang.Glunz@mchp.siemens.de

pstoedit converts Postscript(TM) and PDF files to other vector graphic
formats so that they can be edited graphically. See pstoedit.man or
manual.html for more details on which formats are supported by pstoedit.

The architecture of pstoedit consists of a PostScript frontend which
needs to call a PostScript interpreter like Ghostscript and the
individual backends which are pugged into a kind of framework.

This framework can be used independently from the PostScript frontend
from any other program. The framework provides a uniform interface to
all different backends. Get in contact with the author if you need
more information on how to use this framework.

If you just find this program useful, have made some improvements or 
implemented other backends please send email to Wolfgang.Glunz@mchp.siemens.de.

If this programs saves you a lot of work consider sending a contribution
of any kind.

If you include this program on a CDROM, please send me a copy of the CD,
or if it goes with a book, of the book.

My home address is:

	Dr. Wolfgang Glunz             
	81825 Muenchen / Germany  
	Josef Brueckl Str. 32    

Installing pstoedit:
--------------------
You need a C++ compiler, e.g., g++ to compile pstoedit.

* cd to src
* edit the makefile 
	- change  BINDIR and MANDIR according to your local environment
	- Uncomment the platform specific flags corresponding to your 
	  platform.
	- if you want to include the CGM driver, you must have a copy 
	  of the cd-library from 
	  Here is how to get cd (quoted from an article of the author of cd).

Documentation for cd is included with the package, and is available from
http://speckle.ncsl.nist.gov/~lorax/cgm/cd.html
This distribution may be retrieved via ftp from
zing.ncsl.nist.gov in the directory "cgm"  It will have the
name cd followed by the version number.
The current version is 1.3 and is at:
ftp://zing.ncsl.nist.gov/cgm/cd1.3.tar.gz          


	  After getting cd, uncomment the lines following lines in the makefile
	#CDDIR=../cd1.3
	#LOCALDEFINES=-I$(CDDIR) -DWITHCGM
	#LOCALDRIVERS=drvcgm.$(OBJEXT) cdlib.$(OBJEXT)

	  Note: You don't need to build the cd-library separately.


* type: make clean; make; make install; (for *nix like systems)
	nmake /f makefile 	(in a DOS box)

There are several test cases included. To run them type `make test'.
This works under *nix only.

If you want a GUI and have Borland C++ you can build one using the sources
provided in the src/bc5gui directory. See src/bc5gui/readme.txt for more
details. Many thanks to Jens Weber for this contribution.

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

A template for safer.ps can be found in the src subdirectory.
This way pstoedit can open all the file it needs (the input file and an
output file). After that then -- via the included file -- all files
operations are disabled and the input file is processed. Any file operation
that is executed be the user's PostScript file is disabled this way.


Using pstoedit:
---------------
Before you can use pstoedit you must have a working installation
of GhostScript (either GNU or Aladdin).

The rest is descibed in the manual page in src/pstoedit.man or src/manual.html.

pstoedit works reasonable with PostScript files containing
	* line drawings
	* text with standard fonts 

Try to run it on golfer.ps or tiger.ps that comes with ghostscript, e.g., 
pstoedit -f <your format> <local path where GhostScript is installed>/examples/tiger.ps tiger.<suffix>

In particular pstoedit does not support
	* bitmap images
	* general fill patterns
	* clipping
	* ... 

Special note about the Java backend:
------------------------------------
The java backend generates a java source file that needs other files
in order to be compiled and usable. These files are not part of pstoedit
but can be obtained from the author.
These other files are Java classes (one applet and support classes) that 
allow to step through the individual pages of the converted PostScript
document. This applet can easily be activated from a html-document.

Extending pstoedit:
-------------------
To implement a new backend you can start from drvsampl.cc.
Please don't forget to send any new backend that might be of interest
for others as well to the author (Wolfgang.Glunz@mchp.siemens.de) so that
it can be incorporated into future versions of pstoedit.

Acknowledgements:
-----------------

See manual page in src/pstoedit.man or src/manual.html for a list of contributors.

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

The cgm backend within pstoedit is based on the cd-library available from
http://speckle.ncsl.nist.gov/~lorax/cgm/cd.html . The source contains the
following copyright notice:
	cd software produced by NIST, an agency of the U.S. government,
	is by statute not subject to copyright in the United States.
	Recipients of this software assume all responsibilities associated
	with its operation, modification and maintenance.
