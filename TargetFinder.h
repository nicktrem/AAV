/**
 * Target_Finder.h
 * Uses OpenCV to identify the image targets and
 * get them ready for processing and classification
 * based on the values these targets identify with
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

class TargetFinder
{
public:
	TargetFinder();
	void FindPotentialTargets(cv::Mat& source0);				// Find the potential targets from the initial filter that is applied
	void FindPotentialLetters(cv::Mat& source0);							// Find Potential letters in an image
	cv::Mat* GetTargetZoomedInImage(const std::vector<cv::Point>& contour, const cv::Mat& image);	// Zoom in on a target
	cv::Mat* GetResizeImageOutput();										// Get the resize the image
	cv::Mat* GetRgbThresholdOutput();										// Get the image after the RGB filter is applied
	std::vector<std::vector<cv::Point> >* GetPTContours();					// PT for potential target
	std::vector<std::vector<cv::Point> >* GetLetterContours();

private:
	cv::Mat resizeImageOutput;
	cv::Mat rgbThresholdOutput;
	std::vector<std::vector<cv::Point> > ptContours;		// contours of the potential target
	std::vector<std::vector<cv::Point> > letterContours;	// contours of the potential letters

	// Helper functions
	void resizeImage(cv::Mat &, double , double , int , cv::Mat &);
	void rgbThreshold(cv::Mat &, double [], double [], double [], cv::Mat &);
	void findContours(cv::Mat &, bool , std::vector<std::vector<cv::Point> > &);
	void filterContours(std::vector<std::vector<cv::Point> > &, double , double , double , double , double , double , double [], double , double , double , double , std::vector<std::vector<cv::Point> > &);
};


