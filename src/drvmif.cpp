/* 
   drvMIF.cpp : This file is part of pstoedit
   Backend for Framemaker(TM) MIF format

   Copyright (C) 1993,1994,1995,1996,1997 Wolfgang Glunz, Wolfgang.Glunz@mchp.siemens.de

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

#include "drvmif.h"
#include <iostream.h>
// rcw2: work round case insensitivity in RiscOS
#ifdef riscos
  #include "unix:string.h"
#else
  #include <string.h>
#endif
#include <stdio.h>

static const unsigned int cannotMapToFrame = '#';
#define FRAMECODE(isocode,name,framecode)  framecode
// the other params are just documentation
static unsigned int framecodes[] = {
FRAMECODE(0,".notdef",0),
FRAMECODE(1,".notdef",1),
FRAMECODE(2,".notdef",2),
FRAMECODE(3,".notdef",3),
FRAMECODE(4,".notdef",4),
FRAMECODE(5,".notdef",5),
FRAMECODE(6,".notdef",6),
FRAMECODE(7,".notdef",7),
FRAMECODE(8,".notdef",8),
FRAMECODE(9,".notdef",9),
FRAMECODE(10,".notdef",10),
FRAMECODE(11,".notdef",11),
FRAMECODE(12,".notdef",12),
FRAMECODE(13,".notdef",13),
FRAMECODE(14,".notdef",14),
FRAMECODE(15,".notdef",15),
FRAMECODE(16,".notdef",16),
FRAMECODE(17,".notdef",17),
FRAMECODE(18,".notdef",18),
FRAMECODE(19,".notdef",19),
FRAMECODE(20,".notdef",20),
FRAMECODE(21,".notdef",21),
FRAMECODE(22,".notdef",22),
FRAMECODE(23,".notdef",23),
FRAMECODE(24,".notdef",24),
FRAMECODE(25,".notdef",25),
FRAMECODE(26,".notdef",26),
FRAMECODE(27,".notdef",27),
FRAMECODE(28,".notdef",28),
FRAMECODE(29,".notdef",29),
FRAMECODE(30,".notdef",30),
FRAMECODE(31,".notdef",31),
FRAMECODE(32,"space",32),
FRAMECODE(33,"exclam",33),
FRAMECODE(34,"quotedbl",34),
FRAMECODE(35,"numbersign",35),
FRAMECODE(36,"dollar",36),
FRAMECODE(37,"percent",37),
FRAMECODE(38,"ampersand",38),
FRAMECODE(39,"quoteright",39),
FRAMECODE(40,"parenleft",40),
FRAMECODE(41,"parenright",41),
FRAMECODE(42,"asterisk",42),
FRAMECODE(43,"plus",43),
FRAMECODE(44,"comma",44),
FRAMECODE(45,"minus",45),
FRAMECODE(46,"period",46),
FRAMECODE(47,"slash",47),
FRAMECODE(48,"zero",48),
FRAMECODE(49,"one",49),
FRAMECODE(50,"two",50),
FRAMECODE(51,"three",51),
FRAMECODE(52,"four",52),
FRAMECODE(53,"five",53),
FRAMECODE(54,"six",54),
FRAMECODE(55,"seven",55),
FRAMECODE(56,"eight",56),
FRAMECODE(57,"nine",57),
FRAMECODE(58,"colon",58),
FRAMECODE(59,"semicolon",59),
FRAMECODE(60,"less",60),
FRAMECODE(61,"equal",61),
FRAMECODE(62,"greater",62),
FRAMECODE(63,"question",63),
FRAMECODE(64,"at",64),
FRAMECODE(65,"A",65),
FRAMECODE(66,"B",66),
FRAMECODE(67,"C",67),
FRAMECODE(68,"D",68),
FRAMECODE(69,"E",69),
FRAMECODE(70,"F",70),
FRAMECODE(71,"G",71),
FRAMECODE(72,"H",72),
FRAMECODE(73,"I",73),
FRAMECODE(74,"J",74),
FRAMECODE(75,"K",75),
FRAMECODE(76,"L",76),
FRAMECODE(77,"M",77),
FRAMECODE(78,"N",78),
FRAMECODE(79,"O",79),
FRAMECODE(80,"P",80),
FRAMECODE(81,"Q",81),
FRAMECODE(82,"R",82),
FRAMECODE(83,"S",83),
FRAMECODE(84,"T",84),
FRAMECODE(85,"U",85),
FRAMECODE(86,"V",86),
FRAMECODE(87,"W",87),
FRAMECODE(88,"X",88),
FRAMECODE(89,"Y",89),
FRAMECODE(90,"Z",90),
FRAMECODE(91,"bracketleft",91),
FRAMECODE(92,"backslash",92),
FRAMECODE(93,"bracketright",93),
FRAMECODE(94,"asciicircum",94),
FRAMECODE(95,"underscore",95),
FRAMECODE(96,"quoteleft",96),
FRAMECODE(97,"a",97),
FRAMECODE(98,"b",98),
FRAMECODE(99,"c",99),
FRAMECODE(100,"d",100),
FRAMECODE(101,"e",101),
FRAMECODE(102,"f",102),
FRAMECODE(103,"g",103),
FRAMECODE(104,"h",104),
FRAMECODE(105,"i",105),
FRAMECODE(106,"j",106),
FRAMECODE(107,"k",107),
FRAMECODE(108,"l",108),
FRAMECODE(109,"m",109),
FRAMECODE(110,"n",110),
FRAMECODE(111,"o",111),
FRAMECODE(112,"p",112),
FRAMECODE(113,"q",113),
FRAMECODE(114,"r",114),
FRAMECODE(115,"s",115),
FRAMECODE(116,"t",116),
FRAMECODE(117,"u",117),
FRAMECODE(118,"v",118),
FRAMECODE(119,"w",119),
FRAMECODE(120,"x",120),
FRAMECODE(121,"y",121),
FRAMECODE(122,"z",122),
FRAMECODE(123,"braceleft",123),
FRAMECODE(124,"bar",124),
FRAMECODE(125,"braceright",125),
FRAMECODE(126,"asciitilde",126),
FRAMECODE(127,".notdef",127),
FRAMECODE(128,".notdef",128),
FRAMECODE(129,".notdef",129),
FRAMECODE(130,".notdef",130),
FRAMECODE(131,".notdef",131),
FRAMECODE(132,".notdef",132),
FRAMECODE(133,".notdef",133),
FRAMECODE(134,".notdef",134),
FRAMECODE(135,".notdef",135),
FRAMECODE(136,".notdef",136),
FRAMECODE(137,".notdef",137),
FRAMECODE(138,".notdef",138),
FRAMECODE(139,".notdef",139),
FRAMECODE(140,".notdef",140),
FRAMECODE(141,".notdef",141),
FRAMECODE(142,".notdef",142),
FRAMECODE(143,".notdef",143),
FRAMECODE(144,"dotlessi",0xf5),
FRAMECODE(145,"grave",'`'),
FRAMECODE(146,"acute",'\''),
FRAMECODE(147,"circumflex",0xf6),
FRAMECODE(148,"tilde",0xf7),
FRAMECODE(149,"macron",0xf8),
FRAMECODE(150,"breve",0xf9),
FRAMECODE(151,"dotaccent",0xfa),
FRAMECODE(152,"dieresis",0xac),
FRAMECODE(153,".notdef",153),
FRAMECODE(154,"ring",0xfb),
FRAMECODE(155,"cedilla",0xfc),
FRAMECODE(156,".notdef",156),
FRAMECODE(157,"hungarumlaut",0xfd),
FRAMECODE(158,"ogonek",0xfe),
FRAMECODE(159,"caron",cannotMapToFrame),
FRAMECODE(160,"space",' '),
FRAMECODE(161,"exclamdown",0xc1),
FRAMECODE(162,"cent",0xa2),
FRAMECODE(163,"sterling",0xa3),
FRAMECODE(164,"currency",0xdb),
FRAMECODE(165,"yen",0xb4),
FRAMECODE(166,"brokenbar",'|'),
FRAMECODE(167,"section",0xa4),
FRAMECODE(168,"dieresis",cannotMapToFrame),
FRAMECODE(169,"copyright",0xa9),
FRAMECODE(170,"ordfeminine",0xbb),
FRAMECODE(171,"guillemotleft",0xc7),
FRAMECODE(172,"logicalnot",0xc2),
FRAMECODE(173,"hyphen",'-'),
FRAMECODE(174,"registered",0xa8),
FRAMECODE(175,"macron",0xfe),
FRAMECODE(176,"degree",cannotMapToFrame),
FRAMECODE(177,"plusminus",cannotMapToFrame),
FRAMECODE(178,"twosuperior",cannotMapToFrame),
FRAMECODE(179,"threesuperior",cannotMapToFrame),
FRAMECODE(180,"acute",0xab),
FRAMECODE(181,"mu",cannotMapToFrame),
FRAMECODE(182,"paragraph",0xa6),
FRAMECODE(183,"periodcentered",0xe1),
FRAMECODE(184,"cedilla",cannotMapToFrame),
FRAMECODE(185,"onesuperior",cannotMapToFrame),
FRAMECODE(186,"ordmasculine",0xbc),
FRAMECODE(187,"guillemotright",0xc8),
FRAMECODE(188,"onequarter",cannotMapToFrame),
FRAMECODE(189,"onehalf",cannotMapToFrame),
FRAMECODE(190,"threequarters",cannotMapToFrame),
FRAMECODE(191,"questiondown",0xc0),
FRAMECODE(192,"Agrave",0xcb),
FRAMECODE(193,"Aacute",0xe7),
FRAMECODE(194,"Acircumflex",0xe5),
FRAMECODE(195,"Atilde",0xcc),
FRAMECODE(196,"Adieresis",0x80),
FRAMECODE(197,"Aring",0x81),
FRAMECODE(198,"AE",0xae),
FRAMECODE(199,"Ccedilla",0x82),
FRAMECODE(200,"Egrave",0xe9),
FRAMECODE(201,"Eacute",0x83),
FRAMECODE(202,"Ecircumflex",0xe6),
FRAMECODE(203,"Edieresis",0xe8),
FRAMECODE(204,"Igrave",0xed),
FRAMECODE(205,"Iacute",0xea),
FRAMECODE(206,"Icircumflex",0xeb),
FRAMECODE(207,"Idieresis",0xec),
FRAMECODE(208,"Eth",cannotMapToFrame),
FRAMECODE(209,"Ntilde",0x84),
FRAMECODE(210,"Ograve",0xf1),
FRAMECODE(211,"Oacute",0xee),
FRAMECODE(212,"Ocircumflex",0xef),
FRAMECODE(213,"Otilde",0xcd),
FRAMECODE(214,"Odieresis",0x85),
FRAMECODE(215,"multiply",cannotMapToFrame),
FRAMECODE(216,"Oslash",0xaf),
FRAMECODE(217,"Ugrave",0xf4),
FRAMECODE(218,"Uacute",0xf2),
FRAMECODE(219,"Ucircumflex",0xf3),
FRAMECODE(220,"Udieresis",0x86),
FRAMECODE(221,"Yacute",cannotMapToFrame),
FRAMECODE(222,"Thorn",cannotMapToFrame),
FRAMECODE(223,"germandbls",0xa7),
FRAMECODE(224,"agrave",0x88),
FRAMECODE(225,"aacute",0x87),
FRAMECODE(226,"acircumflex",0x89),
FRAMECODE(227,"atilde",0x8b),
FRAMECODE(228,"adieresis",0x8a),
FRAMECODE(229,"aring",0x8c),
FRAMECODE(230,"ae",0xbe),
FRAMECODE(231,"ccedilla",0x8d),
FRAMECODE(232,"egrave",0x8f),
FRAMECODE(233,"eacute",0x8e),
FRAMECODE(234,"ecircumflex",0x90),
FRAMECODE(235,"edieresis",0x91),
FRAMECODE(236,"igrave",0x93),
FRAMECODE(237,"iacute",0x92),
FRAMECODE(238,"icircumflex",0x94),
FRAMECODE(239,"idieresis",0x95),
FRAMECODE(240,"eth",cannotMapToFrame),
FRAMECODE(241,"ntilde",0x96),
FRAMECODE(242,"ograve",0x98),
FRAMECODE(243,"oacute",0x97),
FRAMECODE(244,"ocircumflex",0x99),
FRAMECODE(245,"otilde",0x9b),
FRAMECODE(246,"odieresis",0x9a),
FRAMECODE(247,"divide",cannotMapToFrame),
FRAMECODE(248,"oslash",0xbf),
FRAMECODE(249,"ugrave",0x9d),
FRAMECODE(250,"uacute",0x9c),
FRAMECODE(251,"ucircumflex",0x9e),
FRAMECODE(252,"udieresis",0x9f),
FRAMECODE(253,"yacute",cannotMapToFrame),
FRAMECODE(254,"thorn",cannotMapToFrame),
FRAMECODE(255,"ydieresis",0xd8)
};


/* not covered codes (Frame supports these, but pstoedit does not generate these)
dagger
bullet
trademark
florin
ellipsis
OE
oe
endash
emdash
quotedbleft
quotedbright
Ydieresis
fraction
guilsignlleft
guilsignlright
fi
fl
quotesinglebase
quotedbllbase
perthousand
*/
static const unsigned int codesize = sizeof(framecodes)/sizeof(unsigned int);

static const char * colorstring(float r, float g, float b)
{
static char buffer[15];
	sprintf(buffer,"R%.3dG%.3dB%.3d", (int) (r * 255), (int) ( g * 255) ,  (int) (b * 255));
	return buffer;
}

static ColorTable colorTable(0,0,colorstring);

static void writeColorCatEntry(ostream & out, float r, float g, float b)
{
// all ColorCatEntries are written directly to the output file.
// the other stuff is written to the buffer
if (!colorTable.isKnownColor(r,g,b)) {
// color not known so far
	int cyan    = (int) (100 * (1.0 - r)) ;
	int magenta = (int) (100 * (1.0 - g)) ;
	int yellow  = (int) (100 * (1.0 - b)) ;
	//int black   = ((cyan + magenta + yellow) == 300 ) ? 100 : 0; // black 0 if not black
	int black = 0;

	// remember, getColorString registers the new color
	out << " <Color "
	    << "<ColorTag " << colorTable.getColorString(r,g,b) << " > " 
	    << "<ColorCyan " << cyan  << " >"
	    << "<ColorMagenta " << magenta  << " >"
	    << "<ColorYellow " << yellow  << " >"
	    << "<ColorBlack " << black  << " >"
	    << ">" << endl;
} 
}

drvMIF::drvMIF(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream):
	drvbase(driveroptions_p,theoutStream,theerrStream,0,0,0),
    	buffer(tempFile.asOutput())
{
	outf << "<MIFFile 4.00>\n";
	outf << "<Units Upt >\n";
	outf << "<ColorCatalog\n";
				 // output buffer, needed because
				 // color entries must be written at
				 // top of output file, but are known
				 // only after processing the full input

}

drvMIF::~drvMIF() 
{

	outf << ">\n";// end of ColorCatalog
	ifstream & inbuffer = tempFile.asInput();
	copy_file(inbuffer,outf);
} 

void drvMIF::print_coords()
{
    for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
	const Point & p = pathElement(n).getPoint(0);
        buffer <<" <Point "  
	     << p.x_ + x_offset << " pt " 
	     << currentDeviceHeight - p.y_ + y_offset << " pt>" 
	     << endl;
    }
}

void drvMIF::close_page()
{
    buffer << "> # end of Frame\n";
    buffer << "> # end of Page\n";
}

#if 1
static const unsigned int Fill   = 0;  // solid fill
static const unsigned int noFill = 15; // no fill
#else
#define  Fill   0
#define  noFill 15
#endif

void drvMIF::open_page()
{
    buffer << "<Page\n";
    buffer << " <PageType BodyPage >\n";
    buffer << " <PageNum `" << currentPageNumber << "'>\n";
//    buffer << " <PageSize  612.0 pt 792.0 pt>\n";
    //     currentDeviceWidth), RND(currentDeviceHeight
    buffer << " <PageSize  " << currentDeviceWidth << " pt " << currentDeviceHeight  << " pt>\n";
    buffer << " <PageOrientation Portrait >\n";
    buffer << " <PageBackground `Default'>\n";
    buffer << "<Frame\n";
    buffer << "  <Pen 0>\n";
    buffer << "  <Fill " << noFill << ">\n"; 
    buffer << "  <PenWidth  1.0 pt>\n";
    buffer << "  <Separation 0>\n";
//    buffer << "  <BRect  0.0 pt 0.0 pt 612.0 pt 792.0 pt>\n";
    buffer << "  <BRect  0.0 pt 0.0 pt " << currentDeviceWidth << " pt " << currentDeviceHeight  << " pt>\n";
    buffer << "  <FrameType NotAnchored >\n";
}

void drvMIF::show_text(const TextInfo & textinfo)
{
    const char * start_of_text = textinfo.thetext;
    buffer << "  <TextLine \n";
    buffer << "   <TLOrigin  " 
	 << textinfo.x + x_offset << " pt " 
	 << currentDeviceHeight - textinfo.y + y_offset << " pt>\n";
    buffer << "   <TLAlignment Left >\n";
    buffer << "   <Angle " << textinfo.currentFontAngle << " >" << endl;
    buffer << "   <Font \n";
    buffer << "    <FTag `'>\n";
    buffer << "    <FFamily `";
    const char * cp = textinfo.currentFontName.value();
    // print up to first '-' or end of string 
    while (cp && *cp && (*cp != '-') ) {
	buffer << *cp;
	cp++;
    }
    buffer << "'>\n";

    buffer << "# currentFontFamilyName: " <<  textinfo.currentFontFamilyName.value() << endl;
    buffer << "# currentFontFullName: " << textinfo.currentFontFullName.value() << endl;
    buffer << "# currentFontName: " << textinfo.currentFontName.value() << endl;
    buffer << "    <FVar `Regular'>\n";
    buffer << "    <FWeight `" << textinfo.currentFontWeight.value() << "'>"<< endl;
    if (  (strstr(textinfo.currentFontName.value(),"Italic") != NULL)  ||
          (strstr(textinfo.currentFontName.value(),"Oblique") != NULL))  {
    	buffer << "    <FAngle `Italic'>\n";
    } else {
    	buffer << "    <FAngle `Regular'>\n";
    }
    buffer << "    <FSize  " << textinfo.currentFontSize << " pt>\n" ;
    buffer << "    <FUnderline No >\n";
    buffer << "    <FOverline No >\n";
    buffer << "    <FStrike No >\n";
    buffer << "    <FSupScript No >\n";
    buffer << "    <FSubScript No >\n";
    buffer << "    <FChangeBar No >\n";
    buffer << "    <FOutline No >\n";
    buffer << "    <FShadow No >\n";
    buffer << "    <FPairKern Yes >\n";
    buffer << "    <FDoubleUnderline No >\n";
    buffer << "    <FNumericUnderline No >\n";
    buffer << "    <FDX  0.0 pt>\n";
    buffer << "    <FDY  0.0 pt>\n";
    buffer << "    <FDW  0.0 pt>\n";
    buffer << "    <FSeparation 0>\n";
    writeColorCatEntry(outf,textinfo.currentR,textinfo.currentG,textinfo.currentB);
    buffer << "     <FColor `" << colorTable.getColorString(textinfo.currentR,textinfo.currentG,textinfo.currentB) << "'> ";
    buffer << "\n" << endl;
    buffer << "   > # end of Font\n";
    buffer << "   <String `";
    while (*start_of_text) {
	    unsigned char actchar = (unsigned char) *start_of_text;
//	    cout << *start_of_text << " " << (int)*start_of_text <<  " " << (int)actchar << " " << framecodes[*start_of_text] << endl;
	    if ((actchar <= codesize ) && 
		(framecodes[actchar] != actchar ) ) {
		// special code
	   	 buffer << "\\x" << hex << framecodes[actchar] << dec << ' ';
	    } else {
		// no special code
	     	 if ( (*start_of_text == '>') ||
	         	(*start_of_text == '\'')
					 ){
			buffer << "\\";
		 }
	   	 buffer << actchar;
	    }
	    start_of_text++;
    }
    buffer << "'>\n";
    buffer << "  > # end of TextLine\n";
}


void drvMIF::show_path()
{

    if (isPolygon()) {
    numberOfElementsInPath()--; // MIF does not need end=start
    buffer << "<Polygon\n <NumPoints " << numberOfElementsInPath() << ">\n";
    if (currentShowType() == drvbase::stroke) {
    	buffer << " <Fill " << noFill << ">\n";
    } else {
    	buffer << " <Fill " << Fill << ">\n";
    }
    buffer << " <Pen  " << Fill << ">\n";
    switch (currentLineCap()) {
	case 0:
		buffer << " <HeadCap Butt>\n" ;
		buffer << " <TailCap Butt>\n" ;
		break;
	case 1:
		buffer << " <HeadCap Round>\n" ;
		buffer << " <TailCap Round>\n" ;
		break;
	case 2:
		buffer << " <HeadCap Square>\n" ;
		buffer << " <TailCap Square>\n" ;
		break;
    }
//
// #define WITHSMOOTH
// this is inactive at the moment, since it does not produce 
// the results as expected. This needs further investigation
//
#if WITHSMOOTH
    if (currentLineJoin() == 0) {
		buffer << " <Smoothed No>\n" ;
    } else {
		// map both 1 and 2 to Smooth
		buffer << " <Smoothed Yes>\n" ;
    }
#endif
    buffer << " <PenWidth "  << currentLineWidth() << " pt>\n";
    writeColorCatEntry(outf,currentR(),currentG(),currentB()) ;
    buffer << " <ObColor `" << colorTable.getColorString(currentR(),currentG(),currentB()) << "'>";
    buffer << "\n" << endl;
    print_coords();
    buffer << "> # end of Polygon\n";
    numberOfElementsInPath()++; // restore old value for proper cleanup
    } else {
    buffer << "<PolyLine\n <NumPoints " << numberOfElementsInPath() << ">\n";
    if (currentShowType() == drvbase::stroke) {
    	buffer << " <Fill " << noFill << ">\n";
    } else {
    	buffer << " <Fill " << Fill << ">\n";
    }
    buffer << " <Pen " << Fill << ">\n";
    switch (currentLineCap()) {
	case 0:
		buffer << " <HeadCap Butt>\n" ;
		buffer << " <TailCap Butt>\n" ;
		break;
	case 1:
		buffer << " <HeadCap Round>\n" ;
		buffer << " <TailCap Round>\n" ;
		break;
	case 2:
		buffer << " <HeadCap Square>\n" ;
		buffer << " <TailCap Square>\n" ;
		break;
    }
#if WITHSMOOTH
    if (currentLineJoin() == 0) {
		buffer << " <Smoothed No>\n" ;
    } else {
		// map both 1 and 2 to Smooth
		buffer << " <Smoothed Yes>\n" ;
    }
#endif
    buffer << " <PenWidth " << currentLineWidth() << " pt>\n";
    writeColorCatEntry(outf,currentR(),currentG(),currentB()) ;
    buffer << " <ObColor `" << colorTable.getColorString(currentR(),currentG(),currentB()) << "'>";
    buffer << endl;
    print_coords();
    buffer << "> # end of PolyLine\n";
    }
}

void drvMIF::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
    buffer << "<Rectangle\n";
    if (currentShowType() == drvbase::stroke) {
    	buffer << " <Fill " << noFill << ">\n";
    } else {
    	buffer << " <Fill " << Fill << ">\n";
    }
    buffer << " <Pen  " << Fill << ">\n";
    buffer << " <PenWidth " << currentLineWidth() << " pt>\n";
    writeColorCatEntry(outf,currentR(),currentG(),currentB()) ;
    buffer << " <ObColor `" << colorTable.getColorString(currentR(),currentG(),currentB()) << "'>";
    buffer << "\n" << endl;
    buffer << " <BRect ";
    buffer <<  llx + x_offset << " pt ";
    buffer <<  currentDeviceHeight - ury + y_offset << " pt ";
    buffer <<  urx - llx << " pt "; /* dx */
    buffer <<  ury - lly << " pt "; /* dy */
    buffer << "> \n> # end of Rectangle\n";
}
