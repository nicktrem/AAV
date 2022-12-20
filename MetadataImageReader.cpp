#include "MetadataImageReader.h"

/**
 * Constructor:
 * Takes the path to the metadata config file and reads
 * it. It stores the metadata code names and the user assigned
 * metadata names that are assosiated with the actual metadata
 * @param: metadataConfigFileLocation - the location of the
 * metadata config file
 */
MetadataImageReader::MetadataImageReader(std::string metadataConfigFileLocation)
{
	// Check to make sure the config file is actually a file
	if(!FilesystemTools::isFile(metadataConfigFileLocation))
			FilesystemTools::FATAL_ERROR("The metadata configuration file was not found at: " + metadataConfigFileLocation);

	// Init the data
	imageFilePath = "";
	imageSet = new bool;
	*imageSet = false;
	this->metadataConfigFileLocation = metadataConfigFileLocation;

	// Init and open the metadata config file
	std::ifstream metadataConfigFile;
	metadataConfigFile.open(metadataConfigFileLocation);
	if(!metadataConfigFile.is_open())
		FilesystemTools::FATAL_ERROR("Could not open the metadata config file!!");

	// Read the metadata config file and get the necessary data from it
	std::string line = "";
	std::string temp = "";
	bool firstQuote, secondQuote;
	while(std::getline(metadataConfigFile, line))
	{
		temp = "";
		firstQuote = false;
		secondQuote = false;
		unsigned int i = 0;
		for(; i < line.length() && !(firstQuote && secondQuote); i++)
		{
			if(line[i] == '"' && !firstQuote)
			{
				firstQuote = true;
				continue;
			}
			else if(line[i] == '"' && firstQuote && !secondQuote)
				secondQuote = true;

			else if(firstQuote && !secondQuote)
				temp += line[i];
		}
		// Check to make sure that the file the program is reading is formatted correctly
		if(!firstQuote && !secondQuote)
		{
			FilesystemTools::WARNING("Please look at the documentation for this program to see how the metadata should be formatted!!");
			FilesystemTools::FATAL_ERROR("The formatting of the metadata config file is not corrent, look at the documentation and adjust!!");
		}
		metadataName.push_back(temp);		// Store the user defined name for the metadata
		temp = "";
		for(; i < line.length(); i++)
		{
			if(line[i] != ' ')
				temp += line[i];
		}
		metadataCodename.push_back(temp);	// Store the metadata code name for the metadata
	}
	metadataConfigFile.close();				// Close the file

	// Make sure the number of user defined metadata names is the same as the number of metadata code names
	if(metadataName.size() != metadataCodename.size())
		FilesystemTools::FATAL_ERROR("Error while reading the metadata config file!!");

}

/**
 * MetadataImageReader::SetImage
 * Sets the current image to get the metadata from and
 * reads and stores the metadata as well for later processing
 * @param: imagePath - the path to the image to get
 * the metadata from
 */
void MetadataImageReader::SetImage(std::string imagePath)
{
	if(!FilesystemTools::isFile(imagePath))	// make sure the image is a valid file
		FilesystemTools::FATAL_ERROR("Could not find the image to get metadata from");

	image = Exiv2::ImageFactory::open(imagePath);	// open the image

	// Read all of the metadata and store it
	image->readMetadata();
	exifData = image->exifData();
	EOMD = exifData.end();
	*imageSet = true;
}

/**
 * MetadataImageReader::GetAllData
 * Gets all of the metadata that the metadata config file
 * was configured to get. It returns it in the format:
 * <metadata User Name> : <Actual metadata value>
 * @return: All of the metadata that the metadata config
 * file was configured to receive
 */
std::string MetadataImageReader::GetAllData()
{
	if(!*imageSet)
		FilesystemTools::FATAL_ERROR("Trying to get metadata when no image is set!!");

	std::string allData = "";
	for(unsigned int i = 0; i < metadataName.size(); i++)
		allData += metadataName[i] + ":\t" + MetadataImageReader::GetDataPiece(metadataCodename[i]) + "\n";
	return allData;
}

/**
 * MetadataImageReader::GetDataPiece
 * Gets the metadata the is associated with the given
 * metadata code name
 * @param: metadataCodename - the codename associated with the metadata
 * @return: the metadata associated with the given codename
 */
std::string MetadataImageReader::GetDataPiece(std::string metadataCodename)
{
	if(!*imageSet)	// Make sure the image is set before extracting the metadata
		FilesystemTools::FATAL_ERROR("Trying to get metadata when no image is set!!");

	std::stringstream ss;
	for (metadataPiece = exifData.begin(); metadataPiece != EOMD; metadataPiece++)
	{
		if(metadataCodename == metadataPiece->key())	// Look for the metadata code name
		{
			ss << metadataPiece->value() << std::endl;	// Return the metadata associated with that code name
			return ss.str();
		}
	}

	return "NONE!!";	// If the metadata is not found then return NONE!!
}

/**
 * Destructor
 * Cleans up the dynamically allocated variables
 */
MetadataImageReader::~MetadataImageReader()
{
	delete imageSet;
	imageSet = nullptr;
}
