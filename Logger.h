/**
 * Logger.h
 * Contains the Logger class that is part of the
 * Filesystem_tools namespace. This class is used to
 * easily create log files for logging important data
 * related to the program that is running
 */
#pragma once
#include <fstream>

#include "FilesystemTools.h"

class FilesystemTools::Logger
{
public:
	Logger();
	Logger(std::string directory);				// if you want to save the logs to a different directory
	void CreateLog(std::string logName);		// to name the log file something else
	void WriteData(std::string data);			// write data to the log file
	void FinishCurrentLog();					// Finish writing data to the log file
	void SetDirectory(std::string directory);	// set the new directory to write a log file in
	std::string GetDirectory();					// get the current directory working in
	~Logger();

private:
	std::ofstream logFile;						// The Log File to write data too
	std::string currentLogName;					// The name of the current log file
	std::string directory;						// The directory to write the log file too
};
