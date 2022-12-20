#include "FilesystemTools.h"

/**
 * Filesystem_tools::GetDirectoryContents
 * Gets and returns the list of paths to the contents that are in a directory
 * @param: directory - the directory to get the list of contents from
 * @return: a list (in the form of a std::vector) of the contents that are in a directory
 */
std::vector<std::string> FilesystemTools::GetDirectoryContents(std::string directory)
{
	if(!FilesystemTools::isDirectory(directory))	// Check to make sure the directory exists
		FilesystemTools::FATAL_ERROR("The directory: " + directory + " does not exist");

	// Use the ls command and pipe it to a variable to store
	std::string command = "ls " + directory;
	FILE *fpipe;
	std::vector<std::string> PathContentList;

	char c = 0;

	if ((fpipe = (FILE*)popen(command.c_str(), "r")) == 0)
		FilesystemTools::FATAL_ERROR("In Filesystem_tools: Unable to open a pipe to get the directory contents");

	std::string temp = "";
	while (fread(&c, sizeof c, 1, fpipe))
	{
		if(c == '\n')
		{
			// Create the path to the items in the directory and add them to the list
			if(directory[directory.length() - 1] == '/')
				PathContentList.push_back(directory + temp);
			else
				PathContentList.push_back(directory + "/" + temp);
			temp = "";
			continue;
		}
		temp += c;
	}
	pclose(fpipe);	// close the pipe

	return PathContentList;		// return the list
}

/**
 * Filesystem_tools::GetTimeAndDate
 * Gets the current data and time and returns
 * it in a string format
 * @return: the date and time in the form of a string
 */
std::string FilesystemTools::GetTimeAndDate()
{
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream timeStream;

	timeStream << std::put_time(std::localtime(&in_time_t), "%m-%d-%Y_%X");
	std::string dateTime = timeStream.str();

	std::replace(dateTime.begin(), dateTime.end(), ':', '-');	// replace and : with - to make it easier to read
	return dateTime;
}

/**
 * Filesystem_tools::GetFilenameFromPath
 * Takes a path to a file and just returns the filename
 * @param: path - the path to a file
 * @return: the filename at the end of the path
 */
std::string FilesystemTools::GetFilenameFromPath(std::string path)
{
	if(FilesystemTools::isDirectory(path))	// If the path doesn't lead to a file return ""
		return "";
	unsigned int index = path.find_last_of("/\\");	// Get the string data from the last / in the path
	if(index + 1 >= path.length())
		return "";

	return path.substr(index + 1);	// return the file from that filepath
}

/**
 * Filesystem_tools::isDirectory
 * Checks to see if a item is a directory
 * @param: path - the path to the item
 * @return: if the item is a directory or not
 */
bool FilesystemTools::isDirectory(std::string path)
{
	struct stat itemStatus;
	if(stat(path.c_str(), &itemStatus) == 0 && (itemStatus.st_mode & S_IFDIR))
			return true;
	return false;
}

/**
 * Filesystem_tools::isFile
 * Checks to see if an item is a file
 * @param: path - the path to the item
 * @return: if that item is a file or not
 */
bool FilesystemTools::isFile(std::string path)
{
	struct stat itemStatus;
	if(stat(path.c_str(), &itemStatus) == 0 && (itemStatus.st_mode & S_IFREG))
		return true;
	return false;
}

/**
 * Filesystem_tools::FATAL_ERROR
 * Displays to the user that an error has occurred
 * and then aborts the program
 * @param: message - the error message to display to the user
 */
void FilesystemTools::FATAL_ERROR(std::string message)
{
	std::cout << "ERROR: " << message << std::endl;
	exit(-1);
}

/**
 * Filesystem_tools::WARNING
 * Displays to the user that a warning has occurred
 * @param: message - the warning message to display to the user
 */
void FilesystemTools::WARNING(std::string message)
{
	std::cout << "WARNING: " << message << std::endl;
}





