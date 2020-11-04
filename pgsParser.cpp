/*
 * pgsParser.cpp
 *
 *  Created on: Nov 1, 2020
 *      Author: blazer
 */

#include <fstream>
#include <bits/stdc++.h>
#include <tesseract/baseapi.h>
#include <libavcodec/avcodec.h>
#include <string>
#include <stdlib.h>
#include "pgsUtil.h"
#include "pgsParser.h"
#include "pgsSegment.h"
#include "pgsSegmentHeader.h"
#include "presentationCompositionSegment.h"
#include "compositionObject.h"

pgsParser::pgsParser(std::string filename)
{
	this->open(filename);

};

pgsParser::pgsParser(){};

pgsParser::~pgsParser()
{
	if (this->pgsData.is_open()) { this->pgsData.close(); }
};

void pgsParser::open(std::string filename)
{
	this->pgsData.open(filename, std::ifstream::binary);
	this->parseNextSegment();
};

pgsSegment pgsParser::parseNextSegment()
{
	char * buffer = new char [13];
	this->pgsData.read(buffer, 13);
	pgsSegmentHeader header = this->parseHeader(buffer);
	// Segment Type
	switch (header.SEGMENT_TYPE)
	{
		case PDS :
		{
			//statements;
			break;
		}
		case ODS :
		{
			//statements;
			break;
		}
		case PCS :
		{
			char * buffer = new char [11];
			this->pgsData.read(buffer, 11);
			presentationCompositionSegment pcs = this->parsePCS(buffer);
			break;
		}
		case WDS :
		{
			//statements;
			break;
		}
		case END :
		{
			//statements;
			break;
		}
		case ERR :
		{
			std::cout << "Failed to find magic number. Either stream is corrupt or you dun fukt up son :|" << std::endl;
			exit (EXIT_FAILURE);
			break;
		}
	}
	pgsSegment segment = pgsSegment(header);
	return segment;

};

pgsSegmentHeader pgsParser::parseHeader(char * head)
{
	// Check for magic number
	std::string magicNumber (pgsUtil::subArray(head, 2));
	if (magicNumber == "PG")
	{
		// Read Header
		char * pts = pgsUtil::subArray(head, 4, 2);
		char * dts = pgsUtil::subArray(head, 4, 6);
		char * type = pgsUtil::subArray(head, 1, 10);
		char * size = pgsUtil::subArray(head, 2, 11);
		return pgsSegmentHeader(pts, dts, type, size);
	}
	else
	{
		return pgsSegmentHeader();
	}

}

presentationCompositionSegment pgsParser::parsePCS(char * buffer)
{
	char * width = pgsUtil::subArray(buffer, 2);
	char * height = pgsUtil::subArray(buffer, 2, 2);
	char * framerate = pgsUtil::subArray(buffer, 1, 4);
	char * compNumber = pgsUtil::subArray(buffer, 2, 5);
	char * compState = pgsUtil::subArray(buffer, 1, 7);
	char * paletteUpdateFlag = pgsUtil::subArray(buffer, 1, 8);
	char * paletteID = pgsUtil::subArray(buffer, 1, 9);
	char * compObjectCount = pgsUtil::subArray(buffer, 1, 10);
	unsigned int count = pgsUtil::cleanChar(compObjectCount[0]);
	compositionObject * compObjects = new compositionObject[count];
	// Parse Objects
	for(int i = 0; i < count; i++)
	{
		// Read only first 4 so we know how big the object is before reading the rest
		char * objBuffer = new char[4];
		this->pgsData.read(objBuffer, 4);
		char * objID = pgsUtil::subArray(objBuffer, 2);
		char * windowID = pgsUtil::subArray(objBuffer, 1, 2);
		char * objCropFlag = pgsUtil::subArray(objBuffer, 1, 3);
		int nextCount = 2;
		if(pgsUtil::cleanChar(objCropFlag[0]) == 0x40) { nextCount += 4; }
		char * objBuffer2 = new char[nextCount];
		this->pgsData.read(objBuffer2, nextCount);
		char * objXPos = pgsUtil::subArray(objBuffer2, 2);
		char * objYPos = pgsUtil::subArray(objBuffer2, 2, 2);
		if(pgsUtil::cleanChar(objCropFlag[0]) == 0x40)
		{
			char * objCropXPos = pgsUtil::subArray(objBuffer2, 2, 4);
			char * objCropYPos = pgsUtil::subArray(objBuffer2, 2, 6);
			char * objCropWidth = pgsUtil::subArray(objBuffer2, 2, 8);
			char * objCropHeight = pgsUtil::subArray(objBuffer2, 2, 10);
			compObjects[i] = compositionObject(objID, windowID, objCropFlag, objXPos, objYPos, objCropXPos, objCropYPos, objCropWidth, objCropHeight);
		}
		else{ compObjects[i] = compositionObject(objID, windowID, objCropFlag, objXPos, objYPos); }
	}
	return presentationCompositionSegment(width, height, framerate, compNumber, compState, paletteUpdateFlag, paletteID, compObjectCount, compObjects);
}

std::map<int, pgsSegment> pgsParser::parseAllSegments()
{
	int count = 0;
	while (!this->pgsData.eof())
	{
		this->PGS_SEGMENTS[count] = this->parseNextSegment();
		this->pgsData.peek();
		count++;
	}
	return this->PGS_SEGMENTS;
};
