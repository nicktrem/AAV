#include "Logger.h"

/**
 * Default Constructor
 * Just used to initialize all the class
 * variables to nothing
 */
FilesystemTools::Logger::Logger()
{
	currentLogName = "";
	directory = "";
}

/**
 * This Constructor initializes all of the values
 * but takes an input directory which can be used
 * to automatically set the directory during the
 * creation of this class
 * @param: directory - the directory to write log
 * files too
 */
FilesystemTools::Logger::Logger(std::string directory)
{
	currentLogName = "";
	if(directory[directory.size() - 1] != '/')
		directory += '/';
	if(!FilesystemTools::isDirectory(directory))
		FilesystemTools::FATAL_ERROR("Logger class unable to find the directory: " + directory);

	this->directory = directory;
}

/**
 * Filesystem_tools::Logger::CreateLog
 * Creates a log file
 * @param: logName - the name of the log file to
 * create and write data too
 */
void FilesystemTools::Logger::CreateLog(std::string logName)
{
	// If a log file is already open and this method is called, then something has gone wrong
	if(logFile.is_open())
	{
		FilesystemTools::WARNING("Try closing the current log file that is open before writing another one!!");
		FilesystemTools::FATAL_ERROR("Trying to create another log file while another log file is already opened");
	}
	currentLogName = logName;					// Update the current logName
	logFile.open(directory + currentLogName);
	if(!logFile.is_open())						// Make sure the log file was able to open properly
		FilesystemTools::FATAL_ERROR("Unable to open the log file: " + directory + currentLogName);
}

/**
 * Filesystem_tools::Logger::WriteData
 * Writes data to the log file that has been created
 * @param: data - the data to write to the log file
 */
void FilesystemTools::Logger::WriteData(std::string data)
{
	if(!logFile.is_open())	// Check to make sure the log file is opened before you write to it
		FilesystemTools::FATAL_ERROR("Unable to write data to the log file: " + directory + currentLogName +
				" because it is not opened!");
	logFile << data;
}

/**
 * Filesystem_tools::Logger::FinishCurrentLog
 * Closes the log file that was being worked on
 */
void FilesystemTools::Logger::FinishCurrentLog()
{
	if(!logFile.is_open())		// If the log file is not opened ad this method is called, something went wrong
		FilesystemTools::FATAL_ERROR("Trying to close a log file when it is not open in the first place!!");
	logFile.close();
}

/**
 * Filesystem_tools::Logger::SetDirectory
 * Used to change the directory as to where to write log files
 * @param: directory - the directory to switch over too
 */
void FilesystemTools::Logger::SetDirectory(std::string directory)
{
	if(directory[directory.size() - 1] != '/')
			directory += '/';
	if(!FilesystemTools::isDirectory(directory))	// Check to make sure the directory exists
			FilesystemTools::FATAL_ERROR("Logger class unable to find the directory: " + directory);

	this->directory = directory;
}

/**
 * Filesystem_tools::Logger::GetDirectory
 * Gets the current directory that the logger is writing logs too
 * @return: directory - the directory that the logger is using
 */
std::string FilesystemTools::Logger::GetDirectory()
{
	return directory;
}

/**
 * Destructor
 * Used to make sure that the user closed all log files
 * If the user did not, the log file for them and tell
 * them to fix the problem in the future
 */
FilesystemTools::Logger::~Logger()
{
	if(logFile.is_open())	// If the log File is still opened then close it for them
	{
		FilesystemTools::WARNING("You did not close the most recent logger File!!");
		logFile.close();
		FilesystemTools::WARNING("This has been resolved but should be fixed in the code");
		FilesystemTools::WARNING("^ So that you close the log file before the logger class gets deallocated!");
	}
}
