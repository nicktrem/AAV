/**
 * Letter_Identifier.h
 * Used for detecting the letters and correctly
 * identifying them. This class is going to drastically
 * change as development continues as new and improved
 * methods of letter detection are discovered after
 * each competition
 */
#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <string>
#include <math.h>
#include "Constants.h"
#include "FilesystemTools.h"
#include "Logger.h"

class LetterIdentifier
{
public:
	LetterIdentifier(std::string directory);	// the Constructor
	void FindLetter(const std::vector<cv::Point>& contour, FilesystemTools::Logger& logger);	// Find the letter and return it
private:

	std::string directory;										// the directory to all of the images
	std::vector<std::vector<cv::Point> > actualLetterContours;	// the contours for the actual letters
	std::vector<std::string> actualImageLocations;				// The image Locations of the actual letters

	// Helper functions
	std::vector<cv::Point> GetActualLetterContour(const cv::Mat& letter_image);		// Gets the contour for an actual letter
	void resizeImage(cv::Mat &, double , double , int , cv::Mat &);
	void rgbThreshold(cv::Mat &, double [], double [], double [], cv::Mat &);
	void findContours(cv::Mat &, bool , std::vector<std::vector<cv::Point> > &);
	void filterContours(std::vector<std::vector<cv::Point> > &, double , double , double ,
			double , double , double , double [], double , double , double ,
			double , std::vector<std::vector<cv::Point> > &);

};
