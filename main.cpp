// Target_finder 2.0

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include "Constants.h"
#include "FilesystemTools.h"
#include "LetterIdentifier.h"
#include "Logger.h"
#include "MetadataImageReader.h"
#include "TargetFinder.h"

using namespace cv;
using std::cout;
using std::endl;
using std::string;

/**
 * Display_Usage() function is called when the program was not called correctly
 * Its purpose is simply to display to the user how to call the program
 */
void Display_Usage()
{
	cout << "Invalid call!" << endl;
	cout << "Call it like: ./programName <directory of images> <metadata config file> <directory of actual letters>" << endl;
}

/** ================== MAIN FUNCTION ================== **/
int main(int NumArgs, char** args)
{
	// Display Version number and check to make sure the input arguments line up
	cout << "Target Finder Version: " << VERSION << endl;
	if(NumArgs != 4)
	{
		Display_Usage();
		return 1;
	}

	/** start the logging process **/
	std::string loggingDirectory = FilesystemTools::GetTimeAndDate();
	std::string command = "mkdir " + loggingDirectory;
	system(command.c_str());

	if(!FilesystemTools::isDirectory(loggingDirectory))	// make sure the directory was created properly
		FilesystemTools::FATAL_ERROR("Unable to make the logging directory: " + loggingDirectory +
				" make sure the file system is NOT Read-Only");

	/** Init the data and objects necessary for processing **/
	Mat image;
	std::vector<std::string> ImageLocations = FilesystemTools::GetDirectoryContents(args[1]);	// location of images to process
	TargetFinder targetFinder;
	MetadataImageReader MIR(args[2]);					// init the metadata reader
	LetterIdentifier letterIdentifier(args[3]);		// init the letter identifier
	FilesystemTools::Logger logger;					// have a logger to write data about what is happening

	int j = 0;	// j is used to numerically name and save the potential targets to the file system

	/** for each of the images to process **/
	for(unsigned int x = 0; x < ImageLocations.size(); x++)
	{
		/** read the image **/
		image = imread(ImageLocations[x], 1);	// get image from file system
		cout << ImageLocations[x] << endl;
		MIR.SetImage(ImageLocations[x]);		// give the metadata reader the location of the current image
		if(!image.data)							// Check to make sure the image is valid
			FilesystemTools::FATAL_ERROR("No image data in the file: " + ImageLocations[x]);

		/** Run the initial target detection to find the potential targets **/
		targetFinder.FindPotentialTargets(image);
		std::vector<std::vector<cv::Point> >* potentialTargetContours = targetFinder.GetPTContours();
		cout << "There are " << potentialTargetContours->size() << " potential targets" << endl;

		/** For each of these potential targets **/
		for (std::vector<cv::Point> potentialTarget: *potentialTargetContours)
		{
			/** Zoom in to get a better picture to process **/
			cv::Mat* zoomed_target = targetFinder.GetTargetZoomedInImage(potentialTarget, image);
			targetFinder.FindPotentialLetters(*zoomed_target);		// find the contours in this potential target

			std::vector<std::vector<cv::Point> >* letter_contours = targetFinder.GetLetterContours();
			// ^ Get the contours from the zoomed in image as they will be used for letter detection

			// Checks to make sure a contour was detected, if not then the zoomed-in image
			// did not have any valuable information hence the program skips it
			if(letter_contours->size() > 0)
			{
				/** For each of the contours detected in the zoomed in image **/
				for (std::vector<cv::Point> letter_contour : *letter_contours)
				{
					/** Make a directory to log the potential targets if one isn't already created **/
					if(!FilesystemTools::isDirectory(loggingDirectory + "/" + FilesystemTools::GetFilenameFromPath(ImageLocations[x])))
					{
						command = "mkdir " + loggingDirectory + "/" + FilesystemTools::GetFilenameFromPath(ImageLocations[x]);
						system(command.c_str());
					}

					/** Set up the logger to write data about the potential target **/
					logger.SetDirectory(loggingDirectory + "/" + FilesystemTools::GetFilenameFromPath(ImageLocations[x]));
					logger.CreateLog(std::to_string(j) + ".log");	// make the log file for this potential target
					// try to find a letter to match it with
					cout << endl << "A letter was found in " << ImageLocations[x] <<  "!!" << endl << endl;

					imwrite(loggingDirectory + "/" + FilesystemTools::GetFilenameFromPath(ImageLocations[x]) + "/" +
							std::to_string(j) + ".jpg", *zoomed_target);	// write the potential target to the file system

					/** Write the logging data to the log file **/
					logger.WriteData("This log piece is for: " + std::to_string(j) + ".jpg\n");
					letterIdentifier.FindLetter(letter_contour, logger);

					logger.WriteData("\n\nMetadata from Image:\n");
					logger.WriteData(MIR.GetAllData());				// write the metadata to the log file
					logger.FinishCurrentLog();

					// display the zoomed in potential target to the user
					namedWindow("Display Image", WINDOW_AUTOSIZE);
					imshow("Display Image", *zoomed_target);
					waitKey(0);

					j++;
				}
			}
			delete zoomed_target;
		}
	}
	return 0;
}
