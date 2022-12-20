#include "LetterIdentifier.h"
#include "FilesystemTools.h"

/**
 * The Constructor for Letter_Identifier
 * Reads the pictures of actual letters and gets their
 * contour values so they can be compared to the targets
 * identified to determine if there is a match or not
 * @param directory: The directory to all of the actual letters
 */
LetterIdentifier::LetterIdentifier(std::string directory)
{
	// Check to make sure the directory is valid
	if(!FilesystemTools::isDirectory(directory))
		FilesystemTools::FATAL_ERROR("The directory: " + directory + " could not be found!!");

	this->directory = directory;

	std::cout << "Just started the init" << std::endl;

	// Get the image locations
	actualImageLocations = FilesystemTools::GetDirectoryContents(directory);
	if(actualImageLocations.size() == 0)
		FilesystemTools::FATAL_ERROR("No images in the directory: " + directory);

	// Get the contour values for all of the actual letters and use them to compare
	// to the target later
	cv::Mat letter_image;
	std::cout << "About to enter the for loop" << std::endl;
	// For each of the actual letter images
	for(std::string image_location : actualImageLocations)
	{
		// read them and get their contour values
		letter_image = cv::imread(image_location, 1);
		std::cout << image_location << std::endl;
		std::vector<cv::Point> letterContour = LetterIdentifier::GetActualLetterContour(letter_image);
		if(letterContour.size() == 0)
			FilesystemTools::FATAL_ERROR("Could not detect any contours for one of the actual letters: " + image_location);
		actualLetterContours.push_back(letterContour);
	}

	// Make sure there is one contour per image! If not then something went wrong!!
	if(actualImageLocations.size() != actualLetterContours.size())
		FilesystemTools::FATAL_ERROR("In letter Identifier class, The number of images is not equal to the number of contours!! fix the problem!");

	std::cout << "Letter Identifier Initialization successful, contours loaded for actual images!!" << std::endl;
}

/**
 * Letter_Identifier::FindLetter
 * Attempts to match a potential letter to one of the actual
 * letters using HuMoments. If there is a match, it is logged
 * in the corresponding log file for this potential target
 * @param: contour - the potential letter to be compared
 * @param: logger - a reference to a logger class so that this function
 * can write data to the logs about this potential letter
 */
void LetterIdentifier::FindLetter(const std::vector<cv::Point>& contour, FilesystemTools::Logger& logger)
{
	// Setup the variables needed for HuMoment calculation
	cv::Moments actualMoments;
	double actualHuMoments[7];
	cv::Moments inputMoments;
	double inputHuMoments[7];
	std::vector<double> HuMoment5Difference;
	double min5 = std::numeric_limits<double>::max();
	int minIndex = 0;
	int i = 0;

	// for each of the actual letter contours
	for(std::vector<cv::Point> actualLetterContour : actualLetterContours)
	{
		// get the actual letter HuMoment Values
		actualMoments = moments(actualLetterContour, false);
		HuMoments(actualMoments, actualHuMoments);

		// get the potential letter HuMoment values
		inputMoments = moments(contour, false);
		HuMoments(inputMoments, inputHuMoments);

		// Logarithmically normalize the data for easier comparison
		std::string data = "";
		logger.WriteData("For " + actualImageLocations[i] + "\n");
		for(int x = 0; x < 7; x++)
		{
			actualHuMoments[x] = -1 * copysign(1.0, actualHuMoments[x]) * log10(abs(actualHuMoments[x]));
			inputHuMoments[x] = -1 * copysign(1.0, inputHuMoments[x]) * log10(abs(inputHuMoments[x]));
			// Write each of the HuMoments for the actual letter and the potential letter to the log file
			logger.WriteData("actualHuMoments[" + std::to_string(x) + "]: " + std::to_string(actualHuMoments[x]) + "\n");
			logger.WriteData("inputHuMoments[" + std::to_string(x) + "]: " + std::to_string(inputHuMoments[x]) + "\n");
		}
		logger.WriteData("\n");

		// make a list of the differences in HuMoments from all of the actual images
		HuMoment5Difference.push_back(abs(actualHuMoments[5] - inputHuMoments[5]));

/*		std::cout << "For " << actualImageLocations[i] << std::endl;
		double retVal1 = cv::matchShapes(contour, actualLetterContour, 1, 0.0);
		double retVal2 = cv::matchShapes(contour, actualLetterContour, 2, 0.0);
		double retVal3 = cv::matchShapes(contour, actualLetterContour, 3, 0.0);
		std::cout << "From Match Shapes function, the value 1 is: " << retVal1 << std::endl;
		std::cout << "From Match Shapes function, the value 2 is: " << retVal2 << std::endl;
		std::cout << "From Match Shapes function, the value 3 is: " << retVal3 << std::endl;*/

		i++;
	}
	/** ===== The CORE OF LETTER DETECTION ===== **/
	// Use the 5th HuMoment for identifying letters, NOTE: This system should be improved
	// and changed as new methods of better detecting letters get discovered

	min5 = std::numeric_limits<double>::max();
	minIndex = 0;
	// find the smallest difference to get close to the desired letter
	for(unsigned int i = 0; i < HuMoment5Difference.size(); i++)
	{
		if (min5 > HuMoment5Difference[i])
		{
			min5 = HuMoment5Difference[i];
			minIndex = i;
		}
	}
	// If a letter was found
	if (min5 <= HUMOMENT5_THRESHOLD)
	{
		// Say it and write it to the logs
		std::cout << "This image is most similar to: " << actualImageLocations[minIndex] << std::endl;
		logger.WriteData("\nThis image is most similar to: " + actualImageLocations[minIndex] + "\n");
	}
	// If a letter was not found
	else
	{
		// Say it and write it to the logs
		std::cout << "Unknown symbol detected, ignoring" << std::endl;
		logger.WriteData("\nUnknown symbol detected, ignoring\n");
	}
}

/**
 * Letter_Identifier::GetActualLetterContour
 * Gets the contour for an actual letter image
 * @param: actualLetterImage - the image of the actual letter
 * to process
 * @return: The contour of the actual letter for the given image
 */
std::vector<cv::Point> LetterIdentifier::GetActualLetterContour(const cv::Mat& actualLetterImage)
{
	//Step1 Resize_Image:
	//input
	cv::Mat resizeImageInput = actualLetterImage;
	double resizeImageWidth = 640;  // default Double
	double resizeImageHeight = 480;  // default Double
	int resizeImageInterpolation = cv::INTER_CUBIC;
	cv::Mat rgbThresholdInput;
	resizeImage(resizeImageInput, resizeImageWidth, resizeImageHeight, resizeImageInterpolation, rgbThresholdInput);

	//Step2 RGB_Threshold:
	//input
	double rgbThresholdRed[] = {188.0, 255.0};
	double rgbThresholdGreen[] = {202.0, 255.0};
	double rgbThresholdBlue[] = {186.0, 255.0};
	cv::Mat findContoursInput;
	rgbThreshold(rgbThresholdInput, rgbThresholdRed, rgbThresholdGreen, rgbThresholdBlue, findContoursInput);

	//Step3 Find_Contours:
	//input
	bool findContoursExternalOnly = false;  // default Boolean
	std::vector<std::vector<cv::Point> > filterContoursContours;
	findContours(findContoursInput, findContoursExternalOnly, filterContoursContours);

	//Step4 Filter_Contours:
	//input
	double filterContoursMinArea = 0;
	double filterContoursMinPerimeter = 0;
	double filterContoursMinWidth = 0;
	double filterContoursMaxWidth = 625.0;
	double filterContoursMinHeight = 0;
	double filterContoursMaxHeight = 500.0;
	double filterContoursSolidity[] = {0, 100};
	double filterContoursMaxVertices = 1000000;
	double filterContoursMinVertices = 0;
	double filterContoursMinRatio = 0;
	double filterContoursMaxRatio = 1000;
	std::vector<std::vector<cv::Point> > filterContoursOutput;
	filterContours(filterContoursContours, filterContoursMinArea, filterContoursMinPerimeter,
			filterContoursMinWidth, filterContoursMaxWidth, filterContoursMinHeight,
			filterContoursMaxHeight, filterContoursSolidity, filterContoursMaxVertices,
			filterContoursMinVertices, filterContoursMinRatio, filterContoursMaxRatio,
			filterContoursOutput);

	// If there is more than one contour for this image the combine them into one
	if(filterContoursOutput.size() > 1)
	{
		std::vector<cv::Point> combined_contour;
		for(std::vector<cv::Point> contour : filterContoursOutput)
		{
			// by combining the points, the contours get combined
			for(cv::Point point : contour)
				combined_contour.push_back(point);
		}
		return combined_contour;
	}
	// If there is only one contour then return it
	else if(filterContoursOutput.size() == 1)
		return filterContoursOutput[0];

	std::vector<cv::Point> empty;
	return empty;

}


/**
 * Scales and image to an exact size.
 *
 * @param input The image on which to perform the Resize.
 * @param width The width of the output in pixels.
 * @param height The height of the output in pixels.
 * @param interpolation The type of interpolation.
 * @param output The image in which to store the output.
 */
void LetterIdentifier::resizeImage(cv::Mat &input, double width, double height, int interpolation, cv::Mat &output) {
	cv::resize(input, output, cv::Size(width, height), 0.0, 0.0, interpolation);
}

/**
 * Segment an image based on color ranges.
 *
 * @param input The image on which to perform the RGB threshold.
 * @param red The min and max red.
 * @param green The min and max green.
 * @param blue The min and max blue.
 * @param output The image in which to store the output.
 */
void LetterIdentifier::rgbThreshold(cv::Mat &input, double red[], double green[], double blue[], cv::Mat &output) {
	cv::cvtColor(input, output, cv::COLOR_BGR2RGB);
	cv::inRange(output, cv::Scalar(red[0], green[0], blue[0]), cv::Scalar(red[1], green[1], blue[1]), output);
}

/**
 * Finds contours in an image.
 *
 * @param input The image to find contours in.
 * @param externalOnly if only external contours are to be found.
 * @param contours vector of contours to put contours in.
 */
void LetterIdentifier::findContours(cv::Mat &input, bool externalOnly, std::vector<std::vector<cv::Point> > &contours) {
	std::vector<cv::Vec4i> hierarchy;
	contours.clear();
	int mode = externalOnly ? cv::RETR_EXTERNAL : cv::RETR_LIST;
	int method = cv::CHAIN_APPROX_SIMPLE;
	cv::findContours(input, contours, hierarchy, mode, method);
}


/**
 * Filters through contours.
 * @param inputContours is the input vector of contours.
 * @param minArea is the minimum area of a contour that will be kept.
 * @param minPerimeter is the minimum perimeter of a contour that will be kept.
 * @param minWidth minimum width of a contour.
 * @param maxWidth maximum width.
 * @param minHeight minimum height.
 * @param maxHeight  maximimum height.
 * @param solidity the minimum and maximum solidity of a contour.
 * @param minVertexCount minimum vertex Count of the contours.
 * @param maxVertexCount maximum vertex Count.
 * @param minRatio minimum ratio of width to height.
 * @param maxRatio maximum ratio of width to height.
 * @param output vector of filtered contours.
 */
void LetterIdentifier::filterContours(std::vector<std::vector<cv::Point> > &inputContours, double minArea, double minPerimeter, double minWidth, double maxWidth, double minHeight, double maxHeight, double solidity[], double maxVertexCount, double minVertexCount, double minRatio, double maxRatio, std::vector<std::vector<cv::Point> > &output) {
	std::vector<cv::Point> hull;
	output.clear();
	for (std::vector<cv::Point> contour: inputContours) {
		cv::Rect bb = boundingRect(contour);
		if (bb.width < minWidth || bb.width > maxWidth) continue;
		if (bb.height < minHeight || bb.height > maxHeight) continue;
		double area = cv::contourArea(contour);
		if (area < minArea) continue;
		if (arcLength(contour, true) < minPerimeter) continue;
		cv::convexHull(cv::Mat(contour, true), hull);
		double solid = 100 * area / cv::contourArea(hull);
		if (solid < solidity[0] || solid > solidity[1]) continue;
		if (contour.size() < minVertexCount || contour.size() > maxVertexCount)	continue;
		double ratio = (double) bb.width / (double) bb.height;
		if (ratio < minRatio || ratio > maxRatio) continue;
		output.push_back(contour);
	}
}

