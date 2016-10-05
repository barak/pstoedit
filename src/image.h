#ifndef __image_h
#define __image_h
/*
   image.h : This file is part of pstoedit.
  
   Copyright (C) 1997,1998 Wolfgang Glunz, wglunz@geocities.com

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
enum ImageType { colorimage, normalimage, imagemask }; 
	// maybe do subclassing later

class Image
{
public:
	ImageType	type;	// the image type
	unsigned int height; 	// height
	unsigned int width;	// width
	short int bits; 	// bits per component
	short int ncomp; 	// number of color components 1 (grey), 3 (RGB) or 4 (CMYB)
	float imageMatrix[6];	// the matrix given as argument to the *image* operator in PostScript
	float normalizedImageCurrentMatrix[6];	// the effective matrix that transforms the ideal image into the device space. 
	bool polarity;		// used for imagemask only
	unsigned char * data;   	// the array of values

	unsigned int nextfreedataitem;  // the current write index into the data array


	Image(): type(colorimage),height(0),width(0),bits(0),ncomp(0),
		polarity(true),data(0),nextfreedataitem(0) 
		{ 
			for (int i = 0; i < 6 ; i++) 
				imageMatrix[i] = normalizedImageCurrentMatrix[i] = 0.0f;
		}
	~Image() { delete [] data; data = 0; nextfreedataitem = 0;}
	void writeEPSImage(ostream & outi) const;
	void getBoundingBox(Point & ll_p, Point & ur_p) const
		{ ll_p = ll; ur_p = ur; }
	void calculateBoundingBox();

private:
	// Bounding Box
	Point ll;
	Point ur;
};

#endif
