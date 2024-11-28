/*
   drvsvm.cpp : This file is part of pstoedit
	 Backend for OpenOffice metafile (SVM - StarView metafile)

   Copyright (C) 2005 Thorsten Behrens (tbehrens at acm.org)

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
#include "drvsvm.h"
#include "svm_actions.h"

#include I_string_h
#include I_fstream
#include I_stdio
#include I_stdlib

#include <math.h>
#include <utility>
#include <vector> 

/* TODO section:
 * 
 * Check the following example files again:
 * - largetextspace.ps
 * - colrtest.ps
 * - images
 * - bounding box
 */

/* Most of the information necessary to read/write SVM files is
 * available only by reading the code - for the parts used here, this
 * is vcl/inc/metaact.hxx, vcl/inc/font.hxx, vcl/inc/vclenum.hxx and
 * the corresponding source files in vcl/source/gdi. The actual
 * reading/writing of SVMs resides in vcl/source/gdi/gdimtf.cxx (all
 * files relative to the OpenOffice.org source tree, of course).
 */

namespace {
   // const char description[] = "generated by SVM backend of pstoedit\0input\0\0";

    template< typename T > void writePod(ostream& outf, 
                                         const T  value)
    {
        outf.write( const_cast<char*>(
                        reinterpret_cast<const char*>(&value)),
                    sizeof(value) );
    }

#ifdef _BIG_ENDIAN
// taken from OpenOffice.org, sal/inc/osl/endian.h (LGPLed)
# define MAKEWORD(bl, bh)    ((drvSVM::uInt16)((bl) & 0xFF) | (((drvSVM::uInt16)(bh) & 0xFF) << 8))
# define LOBYTE(w)           ((drvSVM::uInt8)((drvSVM::uInt16)(w) & 0xFF))
# define HIBYTE(w)           ((drvSVM::uInt8)(((drvSVM::uInt16)(w) >> 8) & 0xFF))
# define MAKEDWORD(wl, wh)   ((drvSVM::uInt32)((wl) & 0xFFFF) | (((drvSVM::uInt32)(wh) & 0xFFFF) << 16))
# define LOWORD(d)           ((drvSVM::uInt16)((drvSVM::uInt32)(d) & 0xFFFF))
# define HIWORD(d)           ((drvSVM::uInt16)(((drvSVM::uInt32)(d) >> 16) & 0xFFFF))
# define SWAPWORD(w)         MAKEWORD(HIBYTE(w),LOBYTE(w))

    // have to override >8 bit writes, since SVM is little-endian -
    // note further that we _don't_ need to override the signed types,
    // too - integral promotion does happen for normal method lookup
    // (compared to choosing a template specialization)

    void writePod(ostream& 	outf, 
                  drvSVM::uInt16  value)
    {
        value = SWAPWORD(value);
        outf.write( const_cast<char*>(
                        reinterpret_cast<const char*>(&value)),
                    sizeof(drvSVM::uInt16) );
    }

    void writePod(ostream& 	outf, 
                  drvSVM::uInt32  value)
    {
        value = MAKEDWORD(SWAPWORD(HIWORD(value)),
                          SWAPWORD(LOWORD(value)));
        outf.write( const_cast<char*>(
                        reinterpret_cast<const char*>(&value)),
                    sizeof(drvSVM::uInt32) );
    }
#endif

    static void fakeVersionCompat(ostream& outf, 
						   drvSVM::uInt16 	versionId,
                           drvSVM::uInt32	len)
    {
        writePod(outf, versionId);
        writePod(outf, len);
    }
}

drvSVM::derivedConstructor(drvSVM) :
    constructBase,
    headerPos(0),
    actionCount(0),
    isDriverOk(close_output_file_and_reopen_in_binary_mode())
{
    // setup driver base class
    const char *const defaultfontname = "System";
    setCurrentFontName(defaultfontname, false /* is standard font */ );

	x_offset = 0.0;
	y_offset = currentDeviceHeight; // need to mirror in y


    // write SVM file header
    // ---------------------

    outf << "VCLMTF";
    fakeVersionCompat(outf, 1, 0x31);

	// stream compress mode
    writePod(outf, (uInt32)0); 

    headerPos = outf.tellp();

    // pref mapmode (place holder, gets written again in destructor)
    fakeVersionCompat(outf, 1, 0x1b);
    writePod(outf, (uInt16)0); // map unit: 100th mm
    writePod(outf, (Int32)0);  // origin x
    writePod(outf, (Int32)0);  // origin y
    writePod(outf, (Int32)1);  // scale x numerator
    writePod(outf, (Int32)1);  // scale x denominator
    writePod(outf, (Int32)1);  // scale y numerator
    writePod(outf, (Int32)1);  // scale y denominator
    writePod(outf, (uInt8)0);  // 'simple' mapmode flag

	// pref size
    writePod(outf, (Int32)0); // prefsize x
    writePod(outf, (Int32)0); // prefsize y

	// action count
    writePod(outf, (uInt32)0);

    // set PostScript-compatible text alignment
    writePod(outf, 
             (uInt16)META_TEXTALIGN_ACTION);
    fakeVersionCompat(outf, 1, 0);
    writePod(outf, (uInt16)1); // alignment: baseline
    ++actionCount;
}

drvSVM::~drvSVM()
{
	const BBox& psBBox( getCurrentBBox() );

    // write out pref mapmode (can only do that now reliably, as
    // during construction, input might not have been fully parsed)
    outf.seekp(headerPos);

    if (Verbose()) 
        errf << "calculated Bounding Box: " 
             << l_transX(psBBox.ll.x())
             << " " 
             << l_transY(psBBox.ur.y()) 
             << " "
             << l_transX(psBBox.ur.x())
             << " " 
             << l_transY(psBBox.ll.y()) << endl;

    // pref mapmode
    fakeVersionCompat(outf, 1, 0x1b);
    writePod(outf, (uInt16)0); // map unit: 100th mm
    writePod(outf, 
             (Int32)l_transX(psBBox.ll.x())); // origin x
    writePod(outf, 
             (Int32)l_transY(psBBox.ur.y())); // origin y

    // convert between pt and 100th mm (factor 35.14598)
    writePod(outf, (Int32)3514598); // scale x numerator
    writePod(outf, (Int32)100000);  // scale x denominator
    writePod(outf, (Int32)3514598); // scale y numerator
    writePod(outf, (Int32)100000);  // scale y denominator
    writePod(outf, (uInt8)0); // clear 'simple' mapmode flag

	// pref size
    writePod(outf, (Int32)abs(
                 l_transX(psBBox.ll.x()) - l_transX(psBBox.ur.x())) + 1 ); // prefsize x
    writePod(outf, (Int32)abs(
                 l_transY(psBBox.ll.y()) - l_transY(psBBox.ur.y())) + 1 ); // prefsize y

	// action count
    writePod(outf, (uInt32)actionCount);
}

void drvSVM::setAttrs( LineColorAction eLineAction, 
                       FillColorAction eFillAction )
{
    // write MetaLineColorAction
    writePod(outf, 
             (uInt16)META_LINECOLOR_ACTION);
    fakeVersionCompat(outf, 1, 0);
    writePod(outf,
             (uInt8)(edgeB()*255 + .5));
    writePod(outf,
             (uInt8)(edgeG()*255 + .5));
    writePod(outf,
             (uInt8)(edgeR()*255 + .5));
    writePod(outf,(uInt8)0); // dummy

    switch( eLineAction )
    {
        case lineColor:
            // switch on line color
            writePod(outf, (uInt8)1);
            break;

        case noLineColor:
            // switch off line color
            writePod(outf, (uInt8)0);
            break;

        default:
            assert (0 && "Unknown line color action");
            break;
    }

    ++actionCount;

    // write MetaFillColorAction
    writePod(outf, 
             (uInt16)META_FILLCOLOR_ACTION);
    fakeVersionCompat(outf, 1, 0);
    writePod(outf,
             (uInt8)(fillB()*255 + .5));
    writePod(outf,
             (uInt8)(fillG()*255 + .5));
    writePod(outf,
             (uInt8)(fillR()*255 + .5));
    writePod(outf,(uInt8)0); // dummy

    switch( eFillAction )
    {
        case fillColor:
            // switch on fill color
            writePod(outf, (uInt8)1);
            break;

        case noFillColor:
            // switch off fill color
            writePod(outf, (uInt8)0);
            break;

        default:
            assert (0 && "Unknown fill color action");
            break;
    }

    ++actionCount;
}

void drvSVM::ClipPath(cliptype /* clipmode*/)
{
}

void drvSVM::Save()
{
}

void drvSVM::Restore()
{
}

void drvSVM::close_page()
{
	// NOOP in drvsvm
}


void drvSVM::open_page()
{
	// NOOP in drvsvm
}

void  drvSVM::write_path( VectorOfVectorOfPoints const& polyPolygon,
                          VectorOfVectorOfFlags const&  polyPolygonFlags )
{
    // write MetaPolyPolygonAction
    writePod(outf, 
             (uInt16)META_POLYPOLYGON_ACTION);
    fakeVersionCompat(outf, 2, 0);
    const std::size_t numPolies( polyPolygon.size() );

    // write polyPolygon.size() empty polygons
    writePod(outf, (uInt16)numPolies);
	{for( std::size_t i=0; i<numPolies; ++i )
		writePod(outf, (uInt16)0);}
    
    // write polyPolygon.size() polygons, possibly with curves
    writePod(outf, (uInt16)numPolies);
	{for( std::size_t i=0; i<numPolies; ++i )
    {
        // write out index (the polygons written here are 'replacing'
        // the dummies written above, at the given index)
        writePod(outf, (uInt16)i);

        fakeVersionCompat(outf, 1, 0);
        writePod(outf, (uInt16)polyPolygon[i].size());
        outf.write( reinterpret_cast<char*>(
                        const_cast<IntPoint*>( &polyPolygon[i][0] )),
                        sizeof(IntPoint)*polyPolygon[i].size() );
        writePod(outf, (uInt8)1); // flag: have flag array
        outf.write( reinterpret_cast<char*>(
                        const_cast<uInt8*>( &polyPolygonFlags[i][0] )),
                        sizeof(uInt8)*polyPolygonFlags[i].size() ); 
	}}

    ++actionCount;
}

void  drvSVM::write_polyline( VectorOfVectorOfPoints const& polyPolygon,
                              VectorOfVectorOfFlags const&  polyPolygonFlags )
{
    const std::size_t numPolies( polyPolygon.size() );
    for( std::size_t currPoly=0; currPoly<numPolies; ++currPoly ) {
        // write MetaPolyLineAction
        writePod(outf, 
                 (uInt16)META_POLYLINE_ACTION);
        fakeVersionCompat(outf, 3, 0);

        // write empty polygon
        writePod(outf, (uInt16)0);
    
        // write LineInfo
        fakeVersionCompat(outf, 1, 0); // TODO(F2): support V2 dash/dot parameterization
        switch (currentLineType()) {
            case dotted:
            case dashed:
            case dashdot:
            case dashdotdot:
                writePod(outf, (uInt16)2);
                break;

            case solid:
                writePod(outf, (uInt16)1);
                break;

            default:
                assert (0 && "Unknown line pattern type");
                break;
        }
        writePod(outf, (Int32)(currentLineWidth() + .5));        

        // write out actual polygon data
        writePod(outf, (uInt8)1); // flag, enabling following polygon

        fakeVersionCompat(outf, 1, 0);
        writePod(outf, (uInt16)polyPolygon[currPoly].size());
        outf.write( reinterpret_cast<char*>(
                        const_cast<IntPoint*>( &polyPolygon[currPoly][0] )),
                        sizeof(IntPoint)*polyPolygon[currPoly].size() );
        writePod(outf, (uInt8)1); // flag: have flag array
        outf.write( reinterpret_cast<char*>(
                        const_cast<uInt8*>( &polyPolygonFlags[currPoly][0] )),
                        sizeof(uInt8)*polyPolygonFlags[currPoly].size() ); 

        ++actionCount;
    }
}

void drvSVM::show_path()
{
    // create poly-polygon from path info
    // ----------------------------------

    VectorOfVectorOfPoints polyPolygon;
    VectorOfPoints		   currPolygon;
    VectorOfVectorOfFlags  polyPolygonFlags;
    VectorOfFlags  		   currPolygonFlags;

    const unsigned int numElems(numberOfElementsInPath());
	for(unsigned int n=0; n<numElems; ++n) {
		const basedrawingelement& elem( pathElement(n) );
		switch (elem.getType()) {
		case moveto:
        {
            // TODO(P3): lots of copying here...
            if( !currPolygon.empty() )
            {
                polyPolygon.push_back(currPolygon);
                polyPolygonFlags.push_back(currPolygonFlags);
                currPolygon.clear();
                currPolygonFlags.clear();
            }
        }

        // fall through
		case lineto:
        {
            const Point& p( elem.getPoint(0) );
            currPolygon.push_back( 
                std::make_pair( (Int32)l_transX(p.x()),
                                (Int32)l_transY(p.y()) ));
            currPolygonFlags.push_back(0);
        }
        break;

		case closepath:
        {
            if( !currPolygon.empty() )
            {
                // append first point again
                currPolygon.push_back( currPolygon.front() );
                currPolygonFlags.push_back( currPolygonFlags.front() );

                // TODO(P3): lots of copying here...
                polyPolygon.push_back(currPolygon);
                polyPolygonFlags.push_back(currPolygonFlags);
                currPolygon.clear();
                currPolygonFlags.clear();
            }
        }
        break;

		case curveto:
        {
            const Point& c1( elem.getPoint(0) );
            currPolygon.push_back( 
                std::make_pair( (Int32)l_transX(c1.x()),
                                (Int32)l_transY(c1.y()) ));
            currPolygonFlags.push_back(2);

            const Point& c2( elem.getPoint(1) );
            currPolygon.push_back( 
                std::make_pair( (Int32)l_transX(c2.x()),
                                (Int32)l_transY(c2.y()) ));
            currPolygonFlags.push_back(2);

            const Point& p2( elem.getPoint(2) );
            currPolygon.push_back( 
                std::make_pair( (Int32)l_transX(p2.x()),
                                (Int32)l_transY(p2.y()) ));
            currPolygonFlags.push_back(0);
        }
        break;

		default:
            assert (0 && "Unknown path element type");
            break;
		}
	}

    // finish last active polygon
    if( !currPolygon.empty() )
    {
        // TODO(P3): lots of copying here...
        polyPolygon.push_back(currPolygon);
        polyPolygonFlags.push_back(currPolygonFlags);
        currPolygon.clear();
        currPolygonFlags.clear();
    }


	// determine path type: fill or line
    // ---------------------------------
    const bool need_line_info( currentLineType() != solid ||
                               currentLineWidth() > 0 );
	switch (currentShowType()) {
        case drvbase::stroke:
        {
            setAttrs( lineColor, noFillColor );

            if (need_line_info)
            {
                write_polyline( polyPolygon, polyPolygonFlags );
            }
            else
            {
                write_path( polyPolygon, polyPolygonFlags );
            }
        }
        break;
            
        case drvbase::fill:
        case drvbase::eofill:
        {
            if (pathWasMerged())
            {
                setAttrs( lineColor, fillColor ); 

                write_path( polyPolygon, polyPolygonFlags );
                if (need_line_info)
                    write_polyline( polyPolygon, polyPolygonFlags );
            }
            else
            {
                setAttrs( noLineColor, fillColor ); 
                write_path( polyPolygon, polyPolygonFlags );
            }
        }
        break;
            
        default:
            assert (0 && "Unknown path show type");
            break;
	}
}


void drvSVM::show_text(const TextInfo& textinfo)
{
    if (fontchanged())
    {
        static const char* symbolName = "symbol";

        // TODO(F2): evaluate textinfo.FontMatrix, and emulate
        // advancements. Or: abort with error, and require user to
        // re-run with '-pti' given (see largetextspace.ps example).

        // write out MetaFontAction (selecting the new font into VCL
        // OutputDevice)
        const Int16 fontHeight = (short int) (textinfo.currentFontSize + .5);
        const Int16 fontAngle = (short int) (10 * textinfo.currentFontAngle + .5);	
    
        Int16 		fontWidth = 0;
        uInt16		fontWeigth = 0;	// default: don't care
        uInt16		fontItalic = 0;	// default: no italics
        uInt16		charSet = 0;	// default: don't know charset

        const char* fontName = nullptr;

        if (strstr(textinfo.currentFontWeight.c_str(), "Regular"))
            fontWeigth = 4;	// semi light weight

        if (strstr(textinfo.currentFontWeight.c_str(), "Normal"))
            fontWeigth = 5;	// normal weight

        if (strstr(textinfo.currentFontWeight.c_str(), "Medium"))
            fontWeigth = 6;	// medium weight

        if (options->emulateNarrowFonts) {
            if (strstr(textinfo.currentFontWeight.c_str(), "Thin") ||
                strstr(textinfo.currentFontName.c_str(), "Thin") ||
                strstr(textinfo.currentFontFullName.c_str(), "Thin")) {
                fontWidth = fontHeight / 3;	// narrow font emulation (trial and error value for Arial font)
            }

            if (strstr(textinfo.currentFontWeight.c_str(), "Extralight") ||
                strstr(textinfo.currentFontName.c_str(), "Extralight") ||
                strstr(textinfo.currentFontFullName.c_str(), "Extralight")) {
                fontWidth = fontHeight / 4;	// narrow font emulation (trial and error value for Arial font)
            }

            if (strstr(textinfo.currentFontWeight.c_str(), "Ultralight") ||
                strstr(textinfo.currentFontName.c_str(), "Ultralight") ||
                strstr(textinfo.currentFontFullName.c_str(), "Ultralight")) {
                fontWidth = fontHeight / 4;	// narrow font emulation (trial and error value for Arial font)
            }

            if (strstr(textinfo.currentFontWeight.c_str(), "Light") ||
                strstr(textinfo.currentFontName.c_str(), "Light") ||
                strstr(textinfo.currentFontFullName.c_str(), "Light") ||
                strstr(textinfo.currentFontWeight.c_str(), "Condensed") ||
                strstr(textinfo.currentFontName.c_str(), "Condensed") ||
                strstr(textinfo.currentFontFullName.c_str(), "Condensed")) {
                fontWidth = fontHeight / 3;	// narrow font emulation (trial and error value for Arial font)
            }
        }
        else {
            if (strstr(textinfo.currentFontWeight.c_str(), "Thin"))
                fontWeigth = 1;	// thin

            if (strstr(textinfo.currentFontWeight.c_str(), "Extralight"))
                fontWeigth = 1;	// thin

            if (strstr(textinfo.currentFontWeight.c_str(), "Ultralight"))
                fontWeigth = 2;	// ultra light

            if (strstr(textinfo.currentFontWeight.c_str(), "Light") ||
                strstr(textinfo.currentFontWeight.c_str(), "Condensed"))
                fontWeigth = 3;	// light
        }

        if (strstr(textinfo.currentFontWeight.c_str(), "Semibold") ||
            strstr(textinfo.currentFontName.c_str(), "Semibold") ||
            strstr(textinfo.currentFontFullName.c_str(), "Semibold"))
            fontWeigth = 7;	// semibold

        if (strstr(textinfo.currentFontWeight.c_str(), "Demibold") ||
            strstr(textinfo.currentFontName.c_str(), "Demibold") ||
            strstr(textinfo.currentFontFullName.c_str(), "Demibold"))
            fontWeigth = 7;	// semibold

        if (strstr(textinfo.currentFontWeight.c_str(), "Bold") ||
            strstr(textinfo.currentFontName.c_str(), "Bold") ||
            strstr(textinfo.currentFontFullName.c_str(), "Bold"))
            fontWeigth = 8;	// bold

        if (strstr(textinfo.currentFontWeight.c_str(), "Extrabold") ||
            strstr(textinfo.currentFontName.c_str(), "Extrabold") ||
            strstr(textinfo.currentFontFullName.c_str(), "Extrabold"))
            fontWeigth = 8;	// bold

        if (strstr(textinfo.currentFontWeight.c_str(), "Ultrabold") ||
            strstr(textinfo.currentFontName.c_str(), "Ultrabold") ||
            strstr(textinfo.currentFontFullName.c_str(), "Ultrabold"))
            fontWeigth = 9;	// ultrabold

        if (strstr(textinfo.currentFontWeight.c_str(), "Heavy") ||
            strstr(textinfo.currentFontName.c_str(), "Heavy") ||
            strstr(textinfo.currentFontFullName.c_str(), "Heavy"))
            fontWeigth = 9;	// ultrabold

        if (strstr(textinfo.currentFontWeight.c_str(), "Black") ||
            strstr(textinfo.currentFontName.c_str(), "Black") ||
            strstr(textinfo.currentFontFullName.c_str(), "Black"))
            fontWeigth = 10;	// black

        if ((strstr(textinfo.currentFontName.c_str(), "Italic") != nullptr) ||
            (strstr(textinfo.currentFontFullName.c_str(), "Italic") != nullptr))
            fontItalic = 2; // normal italics

        if ((strstr(textinfo.currentFontName.c_str(), "Oblique") != nullptr) ||
            (strstr(textinfo.currentFontFullName.c_str(), "Oblique") != nullptr))
            fontItalic = 1; // oblique italics

        if ((strstr(textinfo.currentFontFullName.c_str(), "Symbol") != nullptr) ||
            (strstr(textinfo.currentFontFullName.c_str(), "symbol") != nullptr)) {
            charSet = 10; // symbol charset
            fontName = symbolName;
        } else {
            charSet = 11; // ASCII-US charset
            fontName = textinfo.currentFontName.c_str();
        }

        // write MetaFontAction
        writePod(outf, 
                 (uInt16)META_FONT_ACTION);
        fakeVersionCompat(outf, 1, 0);

        // serialize Font
        fakeVersionCompat(outf, 2, 0);

        // font name
        const size_t stringLen = strlen(fontName);
        writePod(outf,
                 (uInt16)stringLen);
        outf.write(fontName,stringLen);

        // font style
        writePod(outf,
                 (uInt16)0);

        // font size
        writePod(outf,
                 (Int32)fontWidth);
        writePod(outf,
                 (Int32)(-fontHeight));

        // charset
        writePod(outf,
                 (uInt16)charSet);

        // TODO(F3): font family
        writePod(outf,
                 (uInt16)0);
        
        // font pitch
        writePod(outf,
                 (uInt16)0);

        // font weight
        writePod(outf,
                 (uInt16)fontWeigth);

        // font underline
        writePod(outf,
                 (uInt16)0);

        // font strikeout
        writePod(outf,
                 (uInt16)0);

        // font italic
        writePod(outf,
                 (uInt16)fontItalic);

        // text language
        writePod(outf,
                 (uInt16)0);

        // width type
        writePod(outf,
                 (uInt16)0);

        // font orientation
        writePod(outf,
                 (uInt16)fontAngle);

        // word line
        writePod(outf,
                 (uInt8)0);

        // TODO(F3): detect outline mode
        writePod(outf,
                 (uInt8)0);

        // shadow
        writePod(outf,
                 (uInt8)0);

        // kerning
        writePod(outf,
                 (uInt8)0);

        // relief mode
        writePod(outf,
                 (uInt8)0);

        // CJK language
        writePod(outf,
                 (uInt16)0);

        // vertical writing mode
        writePod(outf,
                 (uInt8)0);

        // emphasis marks (esp. for CJK languages)
        writePod(outf,
                 (uInt16)0);

        ++actionCount;
    }

    // write MetaTextColorAction
    writePod(outf, 
             (uInt16)META_TEXTCOLOR_ACTION);
    fakeVersionCompat(outf, 1, 0);
    writePod(outf,
             (uInt8)(textinfo.currentB*255 + .5));
    writePod(outf,
             (uInt8)(textinfo.currentG*255 + .5));
    writePod(outf,
             (uInt8)(textinfo.currentR*255 + .5));
    writePod(outf,(uInt8)0); // dummy

    ++actionCount;

    // write text
    if( textinfo.thetext.c_str() )
    {
        writePod(outf, 
                 (uInt16)META_TEXT_ACTION);
        fakeVersionCompat(outf, 1, 0);
        writePod(outf, 
                 (uInt32)l_transX(textinfo.x()));
        writePod(outf, 
                 (uInt32)l_transY(textinfo.y()));

        const size_t textLen = strlen(textinfo.thetext.c_str());
        writePod(outf,
                 (uInt16)textLen);
        outf.write(textinfo.thetext.c_str(),
                   textLen);
        writePod(outf,
                 (uInt16)0);
        writePod(outf,
                 (uInt16)textLen);

        // TODO(F1): SVM is Unicode-capable, yank version compat above
        // to 2 and write out UTF-16 string here

        ++actionCount;
    }
}


void drvSVM::show_image(const PSImage& imageinfo)
{
	// first retrieve bounding box
	Point lowerLeft, upperRight;
	imageinfo.getBoundingBox(lowerLeft, upperRight);

	const Int32 width  = abs(l_transX(upperRight.x()) - 
                             l_transX(lowerLeft.x()));
	const Int32 height = abs(l_transY(upperRight.y()) - 
                             l_transY(lowerLeft.y()));

	// calc long-padded size of scanline 
	const long int scanlineLen = ((width * 3) + 3) & ~3L;
	const long int maskScanlineLen = ((((width + 7) & ~7L) >> 3L) + 3) & ~3L;

	// now lets get some mem
	auto output = new unsigned char[scanlineLen * height];
	output[0] = 0; // init for coverity
	auto outputMask = new unsigned char[maskScanlineLen * height];
	outputMask[0] = 0; // init for coverity

	// setup inverse transformation matrix
	const float matrixScale(imageinfo.normalizedImageCurrentMatrix[0] *
							imageinfo.normalizedImageCurrentMatrix[3] -
							imageinfo.normalizedImageCurrentMatrix[2] *
							imageinfo.normalizedImageCurrentMatrix[1]);
	const float inverseMatrix[] = {
		imageinfo.normalizedImageCurrentMatrix[3] / matrixScale,
		-imageinfo.normalizedImageCurrentMatrix[1] / matrixScale,
		-imageinfo.normalizedImageCurrentMatrix[2] / matrixScale,
		imageinfo.normalizedImageCurrentMatrix[0] / matrixScale,
		(imageinfo.normalizedImageCurrentMatrix[2] *
		 imageinfo.normalizedImageCurrentMatrix[5] -
		 imageinfo.normalizedImageCurrentMatrix[4] *
		 imageinfo.normalizedImageCurrentMatrix[3]) / matrixScale,
		(imageinfo.normalizedImageCurrentMatrix[4] *
		 imageinfo.normalizedImageCurrentMatrix[1] -
		 imageinfo.normalizedImageCurrentMatrix[0] *
		 imageinfo.normalizedImageCurrentMatrix[5]) / matrixScale
	};

        if (Verbose()) {
           errf << "Image matrix: " 
                << "0: " << imageinfo.normalizedImageCurrentMatrix[0] << " "
                << "1: " << imageinfo.normalizedImageCurrentMatrix[1] << " "
                << "2: " << imageinfo.normalizedImageCurrentMatrix[2] << " "
                << "3: " << imageinfo.normalizedImageCurrentMatrix[3] << " "
                << "4: " << imageinfo.normalizedImageCurrentMatrix[4] << " "
                << "5: " << imageinfo.normalizedImageCurrentMatrix[5] << " "
                << endl;
	}

    // TODO(F2): interpolate
    // TODO(P3): avoid transformation _at all_ if scale and transform suffices
    // TODO(P3): avoid mask for the obvious cases

	// now transform image
	for (long int y=0; y < height; y++) {
		unsigned char* currOutput = &output[scanlineLen * y];
		unsigned char* currMaskOutput = &outputMask[maskScanlineLen * y] - 1;

		for (long int x=0; x < width; x++) {
			// now transform from device coordinate space to image space
			const Point& currPoint( Point(x + lowerLeft.x(),
										  y + lowerLeft.y()).transform(inverseMatrix) );

			// round to integers
			const long int sourceX = (long int) (currPoint.x() + .5);
			const long int sourceY = (long int) (currPoint.y() + .5);

			// is the pixel within source bitmap bounds?
			if (sourceX >= 0L && (unsigned long) sourceX < imageinfo.width &&
				sourceY >= 0L && (unsigned long) sourceY < imageinfo.height) {
				// okay, fetch source pixel value into 
				// RGB triplet

				unsigned char r(255), g(255), b(255);

				// how many components?
				switch (imageinfo.ncomp) {
				case 1:
					r = g = b = imageinfo.getComponent(sourceX, sourceY, 0);
					break;

				case 3:
					r = imageinfo.getComponent(sourceX, sourceY, 0);
					g = imageinfo.getComponent(sourceX, sourceY, 1);
					b = imageinfo.getComponent(sourceX, sourceY, 2);
					break;

				case 4: {
					unsigned char C = imageinfo.getComponent(sourceX, sourceY, 0);
					unsigned char M = imageinfo.getComponent(sourceX, sourceY, 1);
					unsigned char Y = imageinfo.getComponent(sourceX, sourceY, 2);
					const unsigned char K = imageinfo.getComponent(sourceX, sourceY, 3);

					// account for key
					C += K;
					M += K;
					Y += K;

					// convert color
					r = 255 - C;
					g = 255 - M;
					b = 255 - Y;
					}
					break;

                    default: 
                        assert (0 && "Unexpected bitmap format");
                        break;
				}

				// set color triple
				*currOutput++ = b;
				*currOutput++ = g;
				*currOutput++ = r;

                if( (x & 7L) == 0 )
                    ++currMaskOutput;

                // set mask to opaque
                *currMaskOutput &= ~(1L << (7L - (x & 7L)));
			}
            else
            {
                // pixel outside source bitmap bounds - set to
                // white/transparent
				*currOutput++ = 255;
				*currOutput++ = 255;
				*currOutput++ = 255;

                if( (x & 7L) == 0 )
                    ++currMaskOutput;

                // set mask to transparent
                *currMaskOutput |= 1L << (7L - (x & 7L));
            }
		}
	}

    // write BitmapEx action
    writePod(outf, 
             (uInt16)META_BMPEXSCALE_ACTION);
    fakeVersionCompat(outf, 1, 0);

    // write bitmap
    // ------------

    // file header
	writePod(outf, 
             (uInt16)0x4D42);
	writePod(outf, 
             (uInt32)(14 + 40 + scanlineLen*height));
	writePod(outf, 
             (uInt16)0);
	writePod(outf, 
             (uInt16)0);
	writePod(outf, 
             (uInt32)14 + 40);
    
    // info header
	writePod(outf, 
             (uInt32)40);
	writePod(outf, 
             (uInt32)width);
	writePod(outf, 
             (uInt32)height);
	writePod(outf, 
             (uInt16)1);
	writePod(outf, 
             (uInt16)24);
	writePod(outf, 
             (uInt32)0);
	writePod(outf, 
             (uInt32)0);
	writePod(outf, 
             (uInt32)0);
	writePod(outf, 
             (uInt32)0);
	writePod(outf, 
             (uInt32)0);
	writePod(outf, 
             (uInt32)0);

    // actual bitmap data
    outf.write( (char*)output,
                scanlineLen*height );
    
    // magics
	writePod(outf, 
             (uInt32)0x25091962);
	writePod(outf, 
             (uInt32)0xACB20201);
	writePod(outf, 
             (uInt8)2); // bitmap with mask flag


    // write mask bitmap
    // -----------------

    // file header
	writePod(outf, 
             (uInt16)0x4D42);
	writePod(outf, 
             (uInt32)(14 + 40 + 8 + maskScanlineLen*height));
	writePod(outf, 
             (uInt16)0);
	writePod(outf, 
             (uInt16)0);
	writePod(outf, 
             (uInt32)14 + 40 + 8);
    
    // info header
	writePod(outf, 
             (uInt32)40);
	writePod(outf, 
             (uInt32)width);
	writePod(outf, 
             (uInt32)height);
	writePod(outf, 
             (uInt16)1);
	writePod(outf, 
             (uInt16)1);
	writePod(outf, 
             (uInt32)0);
	writePod(outf, 
             (uInt32)0);
	writePod(outf, 
             (uInt32)0);
	writePod(outf, 
             (uInt32)0);
	writePod(outf, 
             (uInt32)2);
	writePod(outf, 
             (uInt32)2);

    // bitmap palette: black and white 
    writePod(outf,
             (uInt32)0);
    writePod(outf,
             (uInt32)0x00FFFFFF);

    // actual bitmap data
    outf.write( (char*)outputMask,
                maskScanlineLen*height );

    // output position
    writePod(outf,
             (Int32)l_transX(lowerLeft.x()));
    writePod(outf,
             (Int32)l_transY(upperRight.y()));

    // output scale
    writePod(outf,
             (Int32)width);
    writePod(outf,
             (Int32)height);

    ++actionCount;

	delete[] output;
	delete[] outputMask;
}


static DriverDescriptionT < drvSVM > D_svm("svm",
                                           "StarView/OpenOffice.org metafile", 
                                           "StarView/OpenOffice.org metafile, readable from OpenOffice.org 1.0/StarOffice 6.0 and above.",
                                           "svm", 
                                           true,	// backend supports subpaths
										   true,	// backend does support curves
										   true,	// backend supports elements which are filled and have edges 
										   true,	// backend supports text
										   DriverDescription::imageformat::memoryeps,  // no support for PNG file images
										   DriverDescription::opentype::normalopen, // we open output file ourselves
										   false,	// if format supports multiple pages in one file (DEFINETELY not) 
										   true     // clipping
										   );
 
