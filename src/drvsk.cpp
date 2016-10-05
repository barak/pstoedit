/* 
   drvSK.cpp

   */

/*

   Copyright (C) 1999 by Bernhard Herzog

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


    The base64 code is a modified version of the code in Python's
    binascii module, which came with the following license:

    Copyright 1991, 1992, 1993, 1994 by Stichting Mathematisch Centrum,
Amsterdam, The Netherlands.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Stichting Mathematisch
Centrum or CWI or Corporation for National Research Initiatives or
CNRI not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior
permission.

While CWI is the initial source for this software, a modified version
is made available by the Corporation for National Research Initiatives
(CNRI) at the Internet address ftp://ftp.python.org.

STICHTING MATHEMATISCH CENTRUM AND CNRI DISCLAIM ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL STICHTING MATHEMATISCH
CENTRUM OR CNRI BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.


*/


#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <iomanip.h>
#if (defined(unix) || defined(__unix__) || defined(_unix) || defined(__unix) || defined(__EMX__) || defined (NetBSD)  ) && !defined(DJGPP)
#include <strstream.h>
#else
#include <strstrea.h>
#endif

#include "drvsk.h"

#ifndef PI
#define PI 3.14159265358979323846264338327
#endif


drvSK::derivedConstructor(drvSK)
:	constructBase, id(0)
{
	outf << "##Sketch 1 0\n";
	outf << "document()\n";
	outf << "layer('Layer 1',1,1,0,0)\n";
	outf << "guess_cont()\n";
}

drvSK::~drvSK()
{
}

void drvSK::print_coords()
{
	int first_subpath = 1;
	Point start;

	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:
			{
				if (!first_subpath) {
					outf << "bn()\n";
				}
				first_subpath = 0;
				start = elem.getPoint(0);
				outf << "bs(" << start.x_ << "," << start.y_ << ",0)\n";
			}
			break;

		case lineto:
			{
				const Point & p = elem.getPoint(0);
				outf << "bs(" << p.x_ << "," << p.y_ << ",0)\n";
			}
			break;

		case closepath:
			{
				outf << "bs(" << start.x_ << "," << start.y_ << ",0)\n";
				outf << "bC()\n";
			}
			break;

		case curveto:
			{
				const Point & p1 = elem.getPoint(0);
				const Point & p2 = elem.getPoint(1);
				const Point & p3 = elem.getPoint(2);

				outf << "bc(" << p1.x_ << "," << p1.y_ << ","
					<< p2.x_ << "," << p2.y_ << "," << p3.x_ << "," << p3.y_ << ",0)\n";
			}
			break;

		default:
			cerr << "\t\tFatal: unexpected case in drvsk\n";
			abort();
			break;
		}
	}
}


// ignore open_page and close_page since sketch supports only a single
// page
// XXX multiple pages could be faked by putting each page on its own layer.
void drvSK::open_page()
{
}

void drvSK::close_page()
{
}

static void save_solid_fill(ostream & outf, float r, float g, float b)
{
	outf << "fp((" << r << "," << g << "," << b << "))\n";
}

static void
save_line(ostream & outf, float r, float g, float b, float width,
		  int cap, int join, const char *dash_pattern_string)
{
	DashPattern dash_pattern(dash_pattern_string);

	outf << "lp((" << r << "," << g << "," << b << "))\n";
	if (width > 0)
		outf << "lw(" << width << ")\n";
	if (cap != 0)
		outf << "lc(" << cap + 1 << ")\n";
	if (join != 0)
		outf << "lj(" << join << ")\n";
	if (dash_pattern.nrOfEntries > 0) {
		if (width <= 0)
			width = 1.0;

		int num = dash_pattern.nrOfEntries;
		num = num * (num % 2 + 1);
		outf << "ld((" << dash_pattern.numbers[0] / width;
		for (int i = 1; i < num; i++) {
			outf << "," << dash_pattern.numbers[i] / width;
		}
		outf << "))\n";
	}
}

static void save_string(ostream & outf, const char *str)
{
	outf << '"';
	while (*str) {
		int c = *str++ & 0xFF;
		if (c >= 0 && c <= 127 && isprint(c)) {
			if (c == '"')
				outf << '\\';
			outf << char (c);
		} else {
			outf << '\\' << oct << setw(3) << setfill('0') << c;
		}
	}
	outf << '"';
}

void drvSK::show_text(const TextInfo & info)
{
	save_solid_fill(outf, fillR(), fillG(), fillB());
	outf << "Fn(\"" << info.currentFontName.value() << "\")\n";
	outf << "Fs(" << info.currentFontSize << ")\n";
	outf << "txt(";
	save_string(outf, info.thetext.value());
	outf << ",(";
	if (info.currentFontAngle) {
		double angle = info.currentFontAngle * PI / 180.0;
		double c = cos(angle);
		double s = sin(angle);
		outf << c << "," << s << "," << -s << "," << c << ",";
	}
	outf << info.x << ", " << info.y << "))\n";
}

void drvSK::show_path()
{
	switch (currentShowType()) {
	case drvbase::stroke:
		save_line(outf, currentR(), currentG(), currentB(),
				  currentLineWidth(), currentLineCap(), currentLineJoin(), dashPattern());
		outf << "fe()\n";
		break;

	case drvbase::fill:
		//cerr << "fill treated as eofill" << endl;
		// fall through into next case...
	case drvbase::eofill:
		save_solid_fill(outf, fillR(), fillG(), fillB());
		//cerr << "currentLineWidth =" << currentLineWidth() << endl;
		if (pathWasMerged()) {
			save_line(outf, edgeR(), edgeG(), edgeB(),
					  currentLineWidth(), currentLineCap(), currentLineJoin(), dashPattern());
			//outf << "#merged\n";
		} else
			outf << "le()\n";
		break;

	default:
		// cannot happen
		cerr << "unexpected ShowType " << (int) currentShowType() << '\n';
		break;
	}

	outf << "b()\n";
	print_coords();

};

void drvSK::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
	cerr << "Rectangle\n";
	outf << "r(" << urx - llx << ",0,0," << ury - lly << "," << llx << "," << lly << ")\n";
}


bool drvSK::pathsCanBeMerged(const PathInfo & path1, const PathInfo & path2) const
{
	const PathInfo *first;
	const PathInfo *last;

	if (path1.nr < path2.nr) {
		first = &path1;
		last = &path2;
	} else {
		first = &path2;
		last = &path1;
	}

	if ((first->currentShowType == fill || first->currentShowType == eofill)
		&& last->currentShowType == stroke
		&& first->numberOfElementsInPath == last->numberOfElementsInPath) {
		for (unsigned int i = 0; i < last->numberOfElementsInPath; i++) {
			const basedrawingelement *bd1 = first->path[i];
			const basedrawingelement *bd2 = last->path[i];
			if (!(*bd1 == *bd2))
				return 0;
		}
//      cerr << "Merge: first " << first->nr
//           << (first->currentShowType == stroke ? " stroke" : " fill")
//           << ", last " << last->nr
//           << (last->currentShowType == stroke ? " stroke" : " fill")
//           << endl;
		return 1;
	} else {
		return 0;
	}
}



#define BASE64_MAXASCII 76		/* Max chunk size (76 char line) */

static const unsigned char table_b2a_base64[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define FBUFLEN 2048
#define BASE64_PAD '='

int drvSK::write_base64(const unsigned char *buf, int length)
{
	unsigned char encoded[FBUFLEN];
	unsigned char *ascii_data = encoded;
	const unsigned char *bin_data = buf;
	unsigned char this_ch;
	int ascii_left;
	int bin_len = ((FBUFLEN / 4) * 3);

	if (bin_len > length)
		bin_len = length;

	/* first, fill the ascii buffer and don't care about max. line length */
	for (; bin_len > 0; bin_len--, bin_data++) {
		/* Shift the data into our buffer */
		state.leftchar = (state.leftchar << 8) | *bin_data;
		state.leftbits += 8;

		/* See if there are 6-bit groups ready */
		while (state.leftbits >= 6) {
			this_ch = (state.leftchar >> (state.leftbits - 6)) & 0x3f;
			state.leftbits -= 6;
			*ascii_data++ = table_b2a_base64[this_ch];
		}
	}

	/* now output the ascii data line by line */
	ascii_left = ascii_data - encoded;
	while (ascii_left > 0) {
		int linelength = BASE64_MAXASCII - state.column;

		if (ascii_left < linelength)
			linelength = ascii_left;
		outf.write((char *) (ascii_data - ascii_left), linelength);	// SUN CC needs this cast
		ascii_left -= linelength;
		state.column += linelength;
		if (state.column >= BASE64_MAXASCII) {
			outf.put('\n');
			state.column = 0;
		}
	}
	return bin_data - (const unsigned char *) buf;
}

void drvSK::close_base64()
{
	unsigned char buf[4];
	unsigned char *ascii_data = buf;

	if (state.leftbits == 2) {
		*ascii_data++ = table_b2a_base64[(state.leftchar & 3) << 4];
		*ascii_data++ = BASE64_PAD;
		*ascii_data++ = BASE64_PAD;
	} else if (state.leftbits == 4) {
		*ascii_data++ = table_b2a_base64[(state.leftchar & 0xf) << 2];
		*ascii_data++ = BASE64_PAD;
	}

	if (ascii_data > buf || state.column != 0) {
		*ascii_data++ = '\n';	/* Append a courtesy newline */
	}
	if (ascii_data > buf) {
		outf.write((char *) buf, ascii_data - buf);
	}
}





void drvSK::show_image(const Image & imageinfo)
{
	if (imageinfo.ncomp > 3) {
		cerr << "image with " << imageinfo.ncomp << " components not supported\n";
		return;
	}

	ostrstream ppm;

	switch (imageinfo.type) {
	case colorimage:
		if (imageinfo.ncomp != 3 || imageinfo.bits != 8) {
			cerr << "color images must have 8 bits/component " "and 3 components\n";
			cerr << "(image has " << imageinfo.ncomp << " with "
				<< imageinfo.bits << " bits/component)\n";
			return;
		}
		ppm << "P6\n";
		break;

	case normalimage:
		if (imageinfo.bits != 8) {
			cerr << "gray images must have 8 bits/component ";
			cerr << "(image has " << imageinfo.bits << " bits/component)\n";
			return;
		}
		ppm << "P5\n";
		break;
	case imagemask:
		ppm << "P4\n";
		break;
	default:
		return;
	}

	ppm << imageinfo.width << " " << imageinfo.height << '\n';
	if (imageinfo.type != imagemask) {
		// bug according to Jeff Dairiki ppm << imageinfo.bits << '\n';
		ppm << ((1 << imageinfo.bits) -1)  << '\n';
	}

	int imageid = getid();
	outf << "bm(" << imageid << ")\n";

	state.leftbits = 0;
	state.leftchar = 0;
	state.column = 0;

	write_base64((unsigned char *) ppm.str(), ppm.pcount());
	// ppm.freeze(0);
	ppm.rdbuf()->freeze(0);

	int bytes_left = imageinfo.nextfreedataitem;
	unsigned char *data = imageinfo.data;
	while (bytes_left) {
		int written = write_base64(data, bytes_left);
		data += written;
		bytes_left -= written;
	}

	close_base64();

	outf << "-\n";

	outf << "im((";
	outf << imageinfo.normalizedImageCurrentMatrix[0] << ",";
	outf << imageinfo.normalizedImageCurrentMatrix[1] << ",";
	outf << -imageinfo.normalizedImageCurrentMatrix[2] << ",";
	outf << -imageinfo.normalizedImageCurrentMatrix[3] << ",";
	outf << (imageinfo.normalizedImageCurrentMatrix[2] * imageinfo.height
			 + imageinfo.normalizedImageCurrentMatrix[4]) << ",";
	outf << (imageinfo.normalizedImageCurrentMatrix[3] * imageinfo.height
			 + imageinfo.normalizedImageCurrentMatrix[5]);
	outf << ")," << imageid << ")\n";

}

static DriverDescriptionT < drvSK > D_sampl("sk", "Sketch Format", "sk", true,	// backend supports subpaths
											true,	// backend supports curves
											true,	// backend supports elements which are filled and stroked
											true,	// backend supports text
											true,	// backend supports Images
											false,	// no support for PNG file images
											DriverDescription::normalopen, false,	// if format supports multiple pages in one file
											false, /*clipping */
											nodriverspecificoptions);
 
