/**
 * MetadataImageReader.h
 * This class is used to read the image metadata
 * from an image file. This class used the open source
 * library exif2 to assist with this task
 */
#pragma once
#include <iostream>
#include <sstream>
#include <exiv2/exiv2.hpp>
#include <vector>
#include <fstream>

#include "FilesystemTools.h"

class MetadataImageReader
{
public:
	MetadataImageReader(std::string metadataConfigFileLocation);		// Constructor: reads the metadata config file
	void SetImage(std::string filepath);								// Sets the image to get the data from
	std::string GetAllData();											// Gets all the metadata the config files needs
	std::string GetDataPiece(std::string metadata_name);				// Gets the metadata assosiated with the metadata code name
	~MetadataImageReader();												// Destructor

private:
	Exiv2::Image::AutoPtr image;										// image to get the metadata from
	Exiv2::ExifData exifData;											// data from image
	Exiv2::ExifData::const_iterator EOMD;								// END OF METADATA
	Exiv2::ExifData::const_iterator metadataPiece;						// piece of the metadata
	std::string imageFilePath;											// path to the image file
	std::string metadataConfigFileLocation;								// path to the metadata config file
	bool* imageSet;														// if the image was set or not
	std::vector<std::string> metadataName;								// The user assigned metadata name associated with the data
	std::vector<std::string> metadataCodename;							// The metadata code name associated with the data
};
