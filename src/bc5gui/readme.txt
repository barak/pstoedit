This directory contains the sources for the GUI to pstoedit.
It can be compiled with Borland C++ only. The sources are
released under the terms of the GNU General Public License.
See the file copying from the pstoedit main directory for more
details.

The sources in this directory are copyrighted by Jens Weber, rz47b7@PostAG.DE.

There are two .ide (Borland Projects Files) in here.
*	winp2ein.ide is a version, where pstoedit is linked into the
	GUI program as "subroutine". This version contains a yet unsolved
	problem. For some reason, all files created by the program get
	created in read-only mode. This causes problems when running
	the program more the once, since even the tmp-files are read-only 
	and thus are not deleted and therefore cannot be overwritten 
	by a second run of the program. 
	If you find out what causes this behaviour, let us know.

*	winp2eex.ide is a version, where pstoedit is called as an external
	program. Therefore you need to build pstoedit as well and install
	it somewhere along your PATH.
