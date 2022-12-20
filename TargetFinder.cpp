#include "TargetFinder.h"

TargetFinder::TargetFinder() {}

/**
 * Target_Finder::FindPotentialTargets
 * Runs the initial filter to identify objects in
 * the images that stand out. These objects that
 * stand out are the potential targets that get
 * further evaluated later
 * @param: sourceImage - the image
 * to run the filter on
 */
void TargetFinder::FindPotentialTargets(cv::Mat& sourceImage)
{
	//Step1 Resize_Image:
	//input
	cv::Mat resizeImageInput = sourceImage;
	double resizeImageWidth = 640.0;  // default Double
	double resizeImageHeight = 480.0;  // default Double
	int resizeImageInterpolation = cv::INTER_CUBIC;
	resizeImage(resizeImageInput, resizeImageWidth, resizeImageHeight, resizeImageInterpolation, this->resizeImageOutput);

	//Step2 RGB_Threshold:
	//input
	cv::Mat rgbThresholdInput = resizeImageOutput;
	double rgbThresholdRed[] = {0.0, 140};
	double rgbThresholdGreen[] = {0.0, 255.0};
	double rgbThresholdBlue[] = {0.0, 185.73005093378612};
	rgbThreshold(rgbThresholdInput, rgbThresholdRed, rgbThresholdGreen, rgbThresholdBlue, this->rgbThresholdOutput);

	//Step3 Find_Contours:
	//input
	cv::Mat findContoursInput = rgbThresholdOutput;
	bool findContoursExternalOnly = false;  // default Boolean
	std::vector<std::vector<cv::Point> > findContoursOutputPT;
	findContours(findContoursInput, findContoursExternalOnly, findContoursOutputPT);

	//Step4 Filter_Contours:
	//input
	std::vector<std::vector<cv::Point> > filterContoursContoursDirt = findContoursOutputPT;
	double filterContoursMinArea = 0.0;  // default Double
	double filterContoursMinPerimeter = 0.0;  // default Double
	double filterContoursMinWidth = 4.0;  // default Double
	double filterContoursMaxWidth = 20.0;  // default Double
	double filterContoursMinHeight = 2.0;  // default Double
	double filterContoursMaxHeight = 20.0;  // default Double
	double filterContoursSolidity[] = {0, 100};
	double filterContoursMaxVertices = 1000000.0;  // default Double
	double filterContoursMinVertices = 0.0;  // default Double
	double filterContoursMinRatio = 0.0;  // default Double
	double filterContoursMaxRatio = 1000.0;  // default Double
	filterContours(filterContoursContoursDirt, filterContoursMinArea, filterContoursMinPerimeter,
			filterContoursMinWidth, filterContoursMaxWidth, filterContoursMinHeight,
			filterContoursMaxHeight, filterContoursSolidity, filterContoursMaxVertices,
			filterContoursMinVertices, filterContoursMinRatio, filterContoursMaxRatio,
			this->ptContours);
}

/**
 * Target_Finder::FindPotentialLetters
 * Takes the source image and applies a filter that is
 * used to detect the target letter
 * @param sourceImage - the image to process
 */
void TargetFinder::FindPotentialLetters(cv::Mat& sourceImage)
{
	//Step1 RGB_Threshold:
	//input
	cv::Mat rgbThresholdInput = sourceImage;
	double rgbThresholdRed[] = {142.17625899280574, 255.0};
	double rgbThresholdGreen[] = {188.03956834532377, 255.0};
	double rgbThresholdBlue[] = {167.4010791366906, 255.0};
	rgbThreshold(rgbThresholdInput, rgbThresholdRed, rgbThresholdGreen, rgbThresholdBlue, this->rgbThresholdOutput);

	//Step2 Find_Contours:
	//input
	cv::Mat findContoursInput = rgbThresholdOutput;
	bool findContoursExternalOnly = false;  // default Boolean
	std::vector<std::vector<cv::Point> > findContoursOutputLetters;
	findContours(findContoursInput, findContoursExternalOnly, findContoursOutputLetters);

	//Step3 Filter_Contours:
	//input
	std::vector<std::vector<cv::Point> > filterContoursContoursLetters = findContoursOutputLetters;
	double filterContoursMinArea = 1000.0;  // default Double
	double filterContoursMinPerimeter = 0.0;  // default Double
	double filterContoursMinWidth = 100.0;  // default Double
	double filterContoursMaxWidth = 500.0;  // default Double
	double filterContoursMinHeight = 100.0;  // default Double
	double filterContoursMaxHeight = 350.0;  // default Double
	double filterContoursSolidity[] = {0, 100};
	double filterContoursMaxVertices = 1000000.0;  // default Double
	double filterContoursMinVertices = 0.0;  // default Double
	double filterContoursMinRatio = 0.0;  // default Double
	double filterContoursMaxRatio = 1000.0;  // default Double
	filterContours(filterContoursContoursLetters, filterContoursMinArea, filterContoursMinPerimeter,
		filterContoursMinWidth, filterContoursMaxWidth, filterContoursMinHeight,
		filterContoursMaxHeight, filterContoursSolidity, filterContoursMaxVertices,
		filterContoursMinVertices, filterContoursMinRatio, filterContoursMaxRatio,
		this->letterContours);
}

/**
 * Get a zoomed in image of the potential target
 * @param contour - the contour of the potential target
 * @param image - the image that the potential target is in
 * @return a zoomed in image of the potential target that is
 * extracted from the input image
 */
cv::Mat* TargetFinder::GetTargetZoomedInImage(const std::vector<cv::Point>& contour, const cv::Mat& image)
{
	cv::Point TLPoint, BRPoint;		//top left and bottom right corner points for contour

	TLPoint.x = RESIZE_PROC_WIDTH;
	BRPoint.x = 0;
	TLPoint.y = RESIZE_PROC_LENGTH;
	BRPoint.y = 0;

	// Get all of the key points for that contour
	for(cv::Point point: contour)
	{
		if(point.x < TLPoint.x)
			TLPoint.x = point.x;
		if(point.y < TLPoint.y)
			TLPoint.y = point.y;

		if(point.x > BRPoint.x)
			BRPoint.x = point.x;
		if(point.y > BRPoint.y)
			BRPoint.y = point.y;
	}
	// Get the points in relation to the larger image
	TLPoint.x = (TLPoint.x / RESIZE_PROC_WIDTH) * image.cols;
	TLPoint.y = (TLPoint.y / RESIZE_PROC_LENGTH) * image.rows;
	BRPoint.x = (BRPoint.x / RESIZE_PROC_WIDTH) * image.cols;
	BRPoint.y = (BRPoint.y / RESIZE_PROC_LENGTH) * image.rows;

	// crop the image and keep the dimensions so it looks zoomed in
	cv::Rect myROI(TLPoint.x, TLPoint.y, BRPoint.x - TLPoint.x, BRPoint.y - TLPoint.y);
	cv::Mat target = image(myROI);
	cv::Mat* resized_target = new cv::Mat;
	resize(target, *resized_target, cv::Size(RESIZE_PROC_WIDTH, RESIZE_PROC_LENGTH));

	return resized_target;	// return the zoomed in target
}

/**
 * This method is a generated getter for the output of a Filter_Contours.
 * @return ContoursReport output from Filter_Contours.
 */
std::vector<std::vector<cv::Point> >* TargetFinder::GetPTContours(){
	return &(this->ptContours);
}

/**
 * This method is a generated getter for the output of a Filter_Contours.
 * @return ContoursReport output from Filter_Contours.
 */
std::vector<std::vector<cv::Point> >* TargetFinder::GetLetterContours(){
	return &(this->letterContours);
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
void TargetFinder::resizeImage(cv::Mat &input, double width, double height, int interpolation, cv::Mat &output) {
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
void TargetFinder::rgbThreshold(cv::Mat &input, double red[], double green[], double blue[], cv::Mat &output) {
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
void TargetFinder::findContours(cv::Mat &input, bool externalOnly, std::vector<std::vector<cv::Point> > &contours) {
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
void TargetFinder::filterContours(std::vector<std::vector<cv::Point> > &inputContours, double minArea, double minPerimeter, double minWidth, double maxWidth, double minHeight, double maxHeight, double solidity[], double maxVertexCount, double minVertexCount, double minRatio, double maxRatio, std::vector<std::vector<cv::Point> > &output) {
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

