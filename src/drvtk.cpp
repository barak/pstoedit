/* 
   drvtk.cpp - Driver to output Tcl/Tk canvas
             - written by Christopher Jay Cox (ccox@airmail.net) - 9/22/97
               http://www.tcltk.com/tclets/impress/
               Based on... 

   drvsample.cpp : Backend for TK

   Copyright (C) 1993,1994,1995,1996,1997,1998 Wolfgang Glunz, wglunz@geocities.com

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

#include "drvtk.h"
#include I_fstream
#include I_stdio
#include I_stdlib

// for sin and cos
#include <math.h>


static const char * tagNames = "notagname";

static const char * colorstring(float r, float g, float b)
{
static char buffer[10];
	sprintf(buffer,"%s%.2x%.2x%.2x","#", (int) (r * 255), (int) ( g * 255) ,  (int) (b * 255));
	return buffer;
}

drvTK::derivedConstructor(drvTK):
	constructBase,
	buffer(tempFile.asOutput()),
	objectId            (1)
{
    // set tgif specific values
    scale = 1.0;		/* tk default internal scale factor */

    x_offset = 0.0;		/* set to fit to tgif page    */
    y_offset = 0.0;		/*          "                 */

    // cannot write any header part, since we need the total number of pages
    // in the header
    for (unsigned int i = 0; i < d_argc ; i++ ) {
        if (verbose) outf << "% " << d_argv[i] << endl;
        if (strcmp(d_argv[i],"-n") == 0) {
            tagNames = d_argv[i+1];
        }
    }  
}

void drvTK::canvasCreate()
{
	outf << "#!/bin/sh" << endl
	<< "# restart trick \\" << endl
	<< "exec wish \"$0\" \"$@\"" << endl
	<< "if {![info exists Global(CurrentCanvas)]} {" << endl
	<< "    frame .f" << endl
	<< "    frame .f.top" << endl
	<< "    frame .f.bottom" << endl
	<< "	frame .f.buttons" << endl
	<< "    canvas .f.c -bg white -scrollregion {0i 0i 8.5i 11i} \\" << endl
	<< "		-xscrollcommand \".f.scrollx set\" -yscrollcommand \".f.scrolly set\"" << endl
	<< "	button .f.origin -text Origin -command \"originObject .f.c all\"" << endl
	<< "	button .f.fit -text Fit -command \"scaleObject .f.c all [winfo fpixels .f.c 8.5i] [winfo fpixels .f.c 11i]\"" << endl
	<< "	button .f.dismiss -text Dismiss -command \"exit\"" << endl
	<< "    scrollbar .f.scrolly -bd 2 -width 15 -command \".f.c yview\"" << endl
	<< "    scrollbar .f.scrollx -orient h -bd 2 -width 15 -command \".f.c xview\"" << endl
	<< "    frame .f.corner -height 20 -width 20 -bd 2" << endl
	<< "    wm maxsize . [winfo pixels . 8.5i] [winfo pixels . 11i]" << endl
	<< "    set Global(CurrentCanvas) .f.c" << endl
	<< "    pack .f.c -in .f.top -side left -fill both -expand true" << endl
	<< "    pack .f.scrolly -in .f.top -anchor se -side right -fill y" << endl
	<< "    pack .f.scrollx -in .f.bottom -anchor se -side left -expand true -fill x" << endl
	<< "    pack .f.corner -in .f.bottom -anchor ne -in .f.bottom -side right" << endl
	<< "	pack .f.origin -side left -in .f.buttons" << endl
	<< "	pack .f.fit -side left -in .f.buttons" << endl
	<< "	pack .f.dismiss -side right -in .f.buttons" << endl
	<< "    pack .f.top -side top -in .f -expand true -fill both" << endl
	<< "    pack .f.bottom -side top -in .f -anchor sw -fill x" << endl
	<< "    pack .f.buttons -side top -in .f -anchor sw -fill x" << endl
	<< "    pack .f -fill both -expand true" << endl
	<< "    bind $Global(CurrentCanvas) <2> \"%W scan mark %x %y\"" << endl
	<< "    bind $Global(CurrentCanvas) <B2-Motion> \"%W scan dragto %x %y\"   " << endl
	<< "	proc scaleObject {c name width height} {" << endl
	<< "		set bb [$c bbox $name]" << endl
	<< "		set x [lindex $bb 2]" << endl
	<< "		set y [lindex $bb 3]" << endl
	<< "		if {$x > $y} {" << endl
	<< "			set scale [expr $width / $x]" << endl
	<< "		} else {" << endl
	<< "			set scale [expr $height / $y]" << endl
	<< "		}" << endl
	<< "		$c scale $name 0 0 $scale $scale" << endl
	<< "	}" << endl
	<< "	proc originObject { c name } {" << endl
	<< "		set bb [$c bbox $name]" << endl
	<< "		set x [lindex $bb 0]" << endl
	<< "		set y [lindex $bb 1]" << endl
	<< "		if {$x > 0} {" << endl
	<< "			set nx -$x" << endl
	<< "		} else {" << endl
	<< "			set nx [expr abs($x)]" << endl
	<< "		}" << endl
	<< "		if {$y > 0} {" << endl
	<< "			set ny -$y" << endl
	<< "		} else {" << endl
	<< "			set ny [expr abs($y)]" << endl
	<< "		}" << endl
	<< "		$c move $name $nx $ny" << endl
	<< "	}" << endl
	<< "}" << endl;
}

drvTK::~drvTK() 
{
	canvasCreate();
	// now we can copy the buffer the output
	ifstream & inbuffer = tempFile.asInput();
	copy_file(inbuffer,outf);
}

void drvTK::print_coords()
{
	float pc_x;
	float pc_y;	
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const Point & p = pathElement(n).getPoint(0);
		pc_x = (p.x_ + x_offset) * scale;
		pc_y = (currentDeviceHeight - p.y_ + y_offset) * scale;
    	buffer << pc_x;
		buffer << ' ' << pc_y;
		if (n != numberOfElementsInPath() - 1 ) {
			// it is not the last point
			buffer << ' '; 
		}
		if ((!((n + 1) % 8)) && ((n + 1) != numberOfElementsInPath())) {
	    	buffer << "\\" << endl;
		}
    }
}
void drvTK::close_page()
{
}

void drvTK::open_page()
{
//    buffer << "page(" << currentPageNumber << ",\"\",1).\n";
}

void drvTK::show_text(const TextInfo & textinfo)
{
#if 0
// Not implemented
    buffer << "text('" <<  colorstring(textinfo.currentR,textinfo.currentG,textinfo.currentB) << "'";
    buffer << "," << textinfo.x + x_offset;
    buffer << "," << currentDeviceHeight - textinfo.y + y_offset;
    buffer << ",'" << textinfo.currentFontName << "'";
    int boldfont   =  (strstr(textinfo.currentFontName,"Bold") != NULL);
    int italicfont = ((strstr(textinfo.currentFontName,"Italic") != NULL) ||
		      (strstr(textinfo.currentFontName,"Oblique") != NULL));
    int fonttype = 0;
    if (boldfont) {
	if (italicfont) fonttype = 3;
	else            fonttype = 1;
    } else {
	if (italicfont) fonttype = 2;
	else            fonttype = 0;
    }
    buffer << "," << fonttype <<  "," << (int) (textinfo.currentFontSize + 0.5) << ",1,0,0,1,70," << textinfo.currentFontSize << "," <<  objectId++ << ",0,14,3,0,0,0,0,0,0,0,0,0,0,\"\",0,";

    const bool rotated = textinfo.currentFontAngle != 0.0;
    if (rotated) {
    	buffer << "1,0,[" << endl;
    } else {
    	buffer << "0,0,[" << endl;
    }
    if (rotated) {
	const float toRadians = 3.14159265359 / 180.0;
	const float angleInRadians = textinfo.currentFontAngle * toRadians;
	const float cosphi = cos(angleInRadians);
	const float sinphi = sin(angleInRadians);
//	const float Sx = textinfo.currentFontSize;
//	const float Sy = textinfo.currentFontSize;
//	const int len = strlen(textinfo.thetext);
	buffer << '\t'<< textinfo.x + x_offset;
	buffer << "," << currentDeviceHeight - textinfo.y + y_offset;

	// the obbox stuff
#ifdef OLDTK
	buffer << "," << textinfo.x + x_offset  ;
	buffer << "," << currentDeviceHeight - textinfo.y + y_offset;
	buffer << "," << textinfo.x + x_offset + len * textinfo.currentFontSize ;
	buffer << "," << currentDeviceHeight - textinfo.y + textinfo.currentFontSize + y_offset;
#else
	// starting with tgif 3.0 pl 7. an all 0 bounding box causes tgif to recalculate it
	buffer << "," << 0;
	buffer << "," << 0;
	buffer << "," << 0;
	buffer << "," << 0;
#endif

	// the CTM stuff
	buffer << "," << cosphi * 1000 ;
	buffer << "," << -sinphi * 1000 ;
	buffer << "," << sinphi * 1000 ;
	buffer << "," << cosphi * 1000 ;
	buffer << "," << 0 ; // no translate
	buffer << "," << 0 ; // no translate

	// the bbox stuff
#ifdef OLDTK
	buffer << "," << textinfo.x + x_offset  ;
	buffer << "," << currentDeviceHeight - textinfo.y + y_offset;
	buffer << "," << textinfo.x + x_offset + len * textinfo.currentFontSize ;
	buffer << "," << currentDeviceHeight - textinfo.y + y_offset  + textinfo.currentFontSize + y_offset ;
#else
	// starting with tgif 3.0 pl 7. an all 0 bounding box causes tgif to recalculate it
	buffer << "," << 0;
	buffer << "," << 0;
	buffer << "," << 0;
	buffer << "," << 0;
#endif

	buffer << "],[" << endl;
    } 
    buffer <<  "\t\"" << textinfo.thetext << "\"]\n";
#else
	unused(&textinfo);
#endif
}

static const int Fill   = 1;
static const int noFill = 0;
void drvTK::show_path()
{
//     buffer << "# Path " << pathnumber << " " << numberOfElementsInPath() << endl;
    const int fillpat = (currentShowType() == drvbase::stroke) ? noFill : Fill;
    if (isPolygon()) {
    	buffer << "set i [$Global(CurrentCanvas) create polygon ";
		print_coords();
		if (fillpat == 1) {
			buffer << " -fill \""
			<< colorstring(currentR(),currentG(),currentB())
			<< "\"";
		} else {
			buffer << " -fill \"\"";
		}
		buffer << " -outline \""
	 	<< colorstring(currentR(),currentG(),currentB())
	 	<< "\""
	 	<< " -width "
	 	<< (currentLineWidth()?currentLineWidth():1)
		<< "p"
	 	<< " -tags \""
	 	<< tagNames
	 	<< "\" ]"
	 	<< endl;
    } else {
		if (fillpat == 1) {
    		buffer << "set i [$Global(CurrentCanvas) create polygon ";
			print_coords();
			buffer << " -fill \""
			<< colorstring(currentR(),currentG(),currentB())
			<< "\"";
			buffer << " -outline \""
	 		<< colorstring(currentR(),currentG(),currentB())
	 		<< "\""
	 		<< " -width "
	 		<< (currentLineWidth()?currentLineWidth():1)
			<< "p"
	 		<< " -tags \""
	 		<< tagNames
	 		<< "\" ]"
	 		<< endl;
		} else {
			buffer << "set i [$Global(CurrentCanvas) create line ";
			print_coords();
			buffer << " -fill \""
			<< colorstring(currentR(),currentG(),currentB())
			<< "\""
			<< " -width "
	 		<< (currentLineWidth()?currentLineWidth():1)
			<< "p"
			<< " -tags \""
			<< tagNames
			<< "\" ]"
			<< endl;
		}
   	}
	buffer << "set Group($i) \""
	<< tagNames
	<< "\""
	<< endl;
};

void drvTK::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
	unused(&llx);
	unused(&lly);
	unused(&urx);
	unused(&ury);
show_path();
#if 0
// Not implemented....
    buffer << "box('" <<  colorstring(currentR(),currentG(),currentB()) << "'";
    buffer << "," << llx + x_offset;
    buffer << "," <<  currentDeviceHeight - lly + y_offset;
    buffer << "," <<  urx + x_offset;
    buffer << "," <<  currentDeviceHeight - ury + y_offset;
    const int fillpat = (currentShowType() == drvbase::stroke) ? noFill : Fill;
    buffer << ","
	 << fillpat
	 << ","
	 << currentLineWidth() 
	 << ","
	 << Fill
	 << ","
	 << objectId++
	 << ",0,0,0,0,0,'1',["
	 << endl
	 << "])."
	 << endl;
#endif
}

static DriverDescriptionT<drvTK> D_tk(
			"tk","tk and/or tk applet source code","tk",
		false, // backend supports subpathes
		   // if subpathes are supported, the backend must deal with
		   // sequences of the following form
		   // moveto (start of subpath)
		   // lineto (a line segment)
		   // lineto 
		   // moveto (start of a new subpath)
		   // lineto (a line segment)
		   // lineto 
		   //
		   // If this argument is set to false each subpath is drawn 
		   // individually which might not necessarily represent
		   // the original drawing.

		false, // backend supports curves
		false, // backend supports elements which are filled and have edges
		false, // backend supports text
		false, // backend supports Images
		DriverDescription::normalopen,	
		false); // backend support multiple pages

 
