/**
 * Filesystem_tools.h
 * Has valuable functions used for working with
 * the File System
 */
#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>

/**
 * A namespace that holds a group of functions
 * and a class definition that is useful for working
 * with the File system
 */
namespace FilesystemTools
{
	std::vector<std::string> GetDirectoryContents(std::string directory);		// Gets a list of paths of the contents in a directory
	std::string GetTimeAndDate();												// Gets the date and time
	std::string GetFilenameFromPath(std::string path);							// Gets the filename from a file path
	bool isDirectory(std::string directoryLocation);							// Checks if something is a directory
	bool isFile(std::string fileLocation);										// Checks if something is a file
	void FATAL_ERROR(std::string message);										// Displays an error message and quits the program
	void WARNING(std::string message);											// Displays a warning message
	class Logger;																// Forward definition of the Logger class
};
