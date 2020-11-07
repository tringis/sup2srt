/*
 * paletteSegment.cpp
 *
 *  Created on: Nov 4, 2020
 *      Author: blazer
 */

#include "paletteSegment.h"
#include "pgsUtil.h"
#include <algorithm>

paletteSegment::paletteSegment()
{
	this->paletteEntryID = -1;
	this->luminance = -1;
	this->colorDiffRed = -1;
	this->colorDiffBlue = -1;
	this->transparency = -1;
	this->r = -1;
	this->g = -1;
	this->b = -1;
}

paletteSegment::paletteSegment(char * paletteEntryID, char * luminance, char * colorDiffRed, char * colorDiffBlue, char * transparency)
{
	this->paletteEntryID = pgsUtil::cleanChar(paletteEntryID[0]);
	this->luminance = pgsUtil::cleanChar(luminance[0]);
	this->colorDiffRed = pgsUtil::cleanChar(colorDiffRed[0]);
	this->colorDiffBlue = pgsUtil::cleanChar(colorDiffBlue[0]);
	this->transparency = pgsUtil::cleanChar(transparency[0]);
	this->r = this->calcRed(this->luminance, this->colorDiffBlue, this->colorDiffRed);
	this->g = this->calcGreen(this->luminance, this->colorDiffBlue, this->colorDiffRed);
	this->b = this->calcBlue(this->luminance, this->colorDiffBlue, this->colorDiffRed);
}

paletteSegment::~paletteSegment() {
	// TODO Auto-generated destructor stub
}

unsigned char paletteSegment::calcRed(unsigned char Y, unsigned char Cb, unsigned char Cr)
{
	unsigned char r = Y + 1.40200 * (Cr - 0x80);
	std::max(std::min(r, pgsUtil::cleanChar(255)), pgsUtil::cleanChar(0));
	return r;
}

unsigned char paletteSegment::calcGreen(unsigned char Y, unsigned char Cb, unsigned char Cr)
{
	unsigned char g = Y - 0.34414 * (Cb - 0x80) - 0.71414 * (Cr - 0x80);
	std::max(std::min(g, pgsUtil::cleanChar(255)), pgsUtil::cleanChar(0));
	return g;
}

unsigned char paletteSegment::calcBlue(unsigned char Y, unsigned char Cb, unsigned char Cr)
{
	unsigned char b = Y + 1.77200 * (Cb - 0x80);
	std::max(std::min(b, pgsUtil::cleanChar(255)), pgsUtil::cleanChar(0));
	return b;
}

unsigned int paletteSegment::get32BitPixel()
{
	return 0;
}

