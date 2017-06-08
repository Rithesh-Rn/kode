// main.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#include<ctime>
#include<fstream>
#include<conio.h>           // it may be necessary to change or remove this line if not using Windows



class Blob {
public:
	// member variables ///////////////////////////////////////////////////////////////////////////
	std::vector<cv::Point> currentContour;

	cv::Rect currentBoundingRect;

	std::vector<cv::Point> centerPositions;

	double dblCurrentDiagonalSize;
	double dblCurrentAspectRatio;

	bool blnCurrentMatchFoundOrNewBlob;

	bool blnStillBeingTracked;

	int intNumOfConsecutiveFramesWithoutAMatch;

	cv::Point predictedNextPosition;

	// function prototypes ////////////////////////////////////////////////////////////////////////
	Blob(std::vector<cv::Point> _contour);
	void predictNextPosition(void);

};

#define SHOW_STEPS            // un-comment or comment this line to show steps or not

// global variables ///////////////////////////////////////////////////////////////////////////////
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);
const cv::Scalar SCALAR_PINK = cv::Scalar(0.0, 223.0, 255.0);

// function prototypes ////////////////////////////////////////////////////////////////////////////
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs);
void addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex);
void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs);
double distanceBetweenPoints(cv::Point point1, cv::Point point2);
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName);
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName);
bool checkIfBlobsCrossedTheLine1(std::vector<Blob> &blobs, int &intHorizontalLinePosition, int &carCount, int &veh2, cv::Mat &imgFrame2Copy);
//bool checkIfBlobsCrossedTheLine3(std::vector<Blob> &blobs, int &intHorizontalLinePosition, int &carCount);
bool checkIfBlobsCrossedTheLine2(std::vector<Blob> &blobs, int &intVerticalLinePosition,int &intHorizontalLinePosition1, int &intHorizontalLinePosition2, int &carCount,  int &veh2, cv::Mat &imgFrame2Copy);
//bool checkIfBlobsCrossedTheLine4(std::vector<Blob> &blobs, int &intVerticalLinePosition, int &carCount);

void drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy);
void drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy);
void drawCarCountOnImage(int &carCount,int &veh2, cv::Mat &imgFrame2Copy);
void drawCarCountOnImage1(int &carCount, int &veh2, cv::Mat &imgFrame2Copy);

int intVerticalLinePosition11;// = (int)std::round((double)imgFrame1.cols * 0.50);
int intVerticalLinePosition12;// = (int)std::round((double)imgFrame1.cols * 0.62);

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {

	cv::VideoCapture capVideo;//cv is the namespace_videocapture is the name of the class_capvideo is variable(object)

	cv::Mat imgFrame1;//mat is array
	cv::Mat imgFrame2;

	std::vector<Blob> blobs;//vectors are dynamic array_array of objects of blob class type_name of array is blobs

	cv::Point crossingLine[8];//point is for coordinates

	int carCount1 = 0, carCount2 = 0, carCount3 = 0, carCount4 = 0 ;
	int veh2_1 = 0,veh2_2 = 0;

	capVideo.open("testVideo_Traffic.mp4");

	if (!capVideo.isOpened()) {                                                 // if unable to open video file
		std::cout << "error reading video file" << std::endl << std::endl;      // show error message
		_getch();                   // it may be necessary to change or remove this line if not using Windows
		return(0);                                                              // and exit program
	}

	if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 2) {
		std::cout << "error: video file must have at least two frames";
		_getch();                   // it may be necessary to change or remove this line if not using Windows
		return(0);
	}


	//decodes and returns  the next video frame
	capVideo.read(imgFrame1);//1st frame
	capVideo.read(imgFrame2);//2nd frame

	int intHorizontalLinePosition11 = (int)std::round((double)imgFrame1.rows * 0.80);//60//getting value of 1/3rd of screeen height
																				   //int intHorizontalLinePosition = (int)std::round((double)imgFrame1.rows * 0.70);
																				   //int intHorizontalLinePosition1 = (int)std::round((double)imgFrame1.cols * 0.60);////
	//int intHorizontalLinePosition12 = (int)std::round((double)imgFrame1.rows * 0.72);
	int intHorizontalLinePosition22 = (int)std::round((double)imgFrame1.rows * 0.70);
	int intHorizontalLinePosition21 = (int)std::round((double)imgFrame1.rows * 0.45);
	/*int intHorizontalLinePosition31 = (int)std::round((double)imgFrame1.rows * 0.35);
	int intHorizontalLinePosition32 = (int)std::round((double)imgFrame1.rows * 0.30);
	int intHorizontalLinePosition42 = (int)std::round((double)imgFrame1.rows * 0.52);
	int intHorizontalLinePosition = (int)std::round((double)imgFrame1.rows * 0.42);*/
	
	intVerticalLinePosition11 = (int)std::round((double)imgFrame1.cols * 0.45);//50//25
	intVerticalLinePosition12 = (int)std::round((double)imgFrame1.cols * 0.70);//70//75
	
	int intVerticalLinePosition2 = (int)std::round((double)imgFrame1.cols * 0.20);
	//int intVerticalLinePosition22 = (int)std::round((double)imgFrame1.cols * 0.33);
	/*int intVerticalLinePosition31 = (int)std::round((double)imgFrame1.cols * 0.40);
	int intVerticalLinePosition32 = (int)std::round((double)imgFrame1.cols * 0.50);
	int intVerticalLinePosition41 = (int)std::round((double)imgFrame1.cols * 0.70);
	int intVerticalLinePosition42 = (int)std::round((double)imgFrame1.cols * 0.80);*/


	crossingLine[0].x =  intVerticalLinePosition11;
	crossingLine[0].y = intHorizontalLinePosition11;
	
	crossingLine[1].x =  intVerticalLinePosition12;
	crossingLine[1].y = intHorizontalLinePosition11;

	crossingLine[2].x = intVerticalLinePosition2;
	crossingLine[2].y = intHorizontalLinePosition21;

	crossingLine[3].x = intVerticalLinePosition2;
	crossingLine[3].y = intHorizontalLinePosition22;

	/*crossingLine[4].x = intVerticalLinePosition31;
	crossingLine[4].y = intHorizontalLinePosition31;

	crossingLine[5].x = intVerticalLinePosition32;
	crossingLine[5].y = intHorizontalLinePosition32;

	crossingLine[6].x = intVerticalLinePosition41;
	crossingLine[6].y = intHorizontalLinePosition;//41

	crossingLine[7].x = intVerticalLinePosition42;
	crossingLine[7].y = intHorizontalLinePosition42;*/
	
	char chCheckForEscKey = 0;//intialize it to some value(preferabbly 0) so that its not 27 i.e ESC

	bool blnFirstFrame = true;

	int frameCount = 2;

	while (capVideo.isOpened() && chCheckForEscKey != 27) {//if video is playing and the user hasnt pressed esc

		std::vector<Blob> currentFrameBlobs;

		cv::Mat imgFrame1Copy = imgFrame1.clone();//entire array is copied
		cv::Mat imgFrame2Copy = imgFrame2.clone();

		cv::Mat imgDifference;
		cv::Mat imgThresh;

		cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);//Converts frames from color to gray
		cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);

		cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(5, 5), 0);//Blurs an image using a Gaussian filter.
		cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(5, 5), 0);//to smoothen i.e reduce noise & details
																		  //for (small)concept understanding 
		cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);//pixel value difference in each corresponding images
																 //may be like eliminating background.//1 : N ???????
		cv::imshow("imgdifference", imgDifference);//Iadded
		cv::threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY);

		cv::imshow("imgThresh", imgThresh);

		cv::Mat structuringElement3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
		cv::Mat structuringElement7x7 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
		cv::Mat structuringElement15x15 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));

		for (unsigned int i = 0; i < 2; i++) {//something like blob creation //rounding off images
			cv::dilate(imgThresh, imgThresh, structuringElement5x5);
			cv::dilate(imgThresh, imgThresh, structuringElement5x5);
			cv::erode(imgThresh, imgThresh, structuringElement5x5);
		}
		cv::imshow("imgafterstructuringelement", imgThresh);
		cv::Mat imgThreshCopy = imgThresh.clone();

		std::vector<std::vector<cv::Point> > contours;//2D dynamic array

		cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		//each contors(threads) stored as vector of points
		drawAndShowContours(imgThresh.size(), contours, "imgContours");

		std::vector<std::vector<cv::Point> > convexHulls(contours.size());

		for (unsigned int i = 0; i < contours.size(); i++) {
			cv::convexHull(contours[i], convexHulls[i]);//drawing elaborate boundry to contours
		}

		drawAndShowContours(imgThresh.size(), convexHulls, "imgConvexHulls");

		for (auto &convexHull : convexHulls) {//to iterate convexHulls.count() times
											  //to consider only contors with optimu size
			Blob possibleBlob(convexHull);    //and to eliminate unnecessary contours
											  //possibleBlob is object of Blob class
			if (possibleBlob.currentBoundingRect.area() > 200 &&//200
				possibleBlob.dblCurrentAspectRatio > 0.2 &&//0.2
				possibleBlob.dblCurrentAspectRatio < 4.0 &&//4.0
				possibleBlob.currentBoundingRect.width > 10 &&//10
				possibleBlob.currentBoundingRect.height > 10 &&//10
				possibleBlob.dblCurrentDiagonalSize > 5 &&//5
				(cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 0.50) {
				currentFrameBlobs.push_back(possibleBlob);
				//currentFrameBlobs is vector of blobs
			}
		}

		drawAndShowContours(imgThresh.size(), currentFrameBlobs, "imgCurrentFrameBlobs");

		if (blnFirstFrame == true) {//may be to push the content from currentframeblob to blobs
			for (auto &currentFrameBlob : currentFrameBlobs) {//possible blobs from 1st frame
				blobs.push_back(currentFrameBlob);/////////////////////////////////////////for 2 wheeler and 4 wheelerv
			}
		}
		else {
			matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);
		}//blobs -> blobs which are taken into account//currentFrameBlobs ->all available bolbs

		drawAndShowContours(imgThresh.size(), blobs, "imgBlobs");

		imgFrame2Copy = imgFrame2.clone(); // get another copy of frame 2 since we changed the previous frame 2 copy in the processing above

		drawBlobInfoOnImage(blobs, imgFrame2Copy);
		
		bool blnAtLeastOneBlobCrossedTheLine1 = checkIfBlobsCrossedTheLine1(blobs, intHorizontalLinePosition11, carCount1,veh2_1, imgFrame2Copy);

		if (blnAtLeastOneBlobCrossedTheLine1 == true) {//change color of line if the vehicle has crossed it.
			cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_GREEN, 2);
			
		}
		else {
			cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_RED, 2);
		}

		bool blnAtLeastOneBlobCrossedTheLine2 = checkIfBlobsCrossedTheLine2(blobs, intVerticalLinePosition2, intHorizontalLinePosition21 , intHorizontalLinePosition22, carCount2,veh2_2,imgFrame2Copy);

		if (blnAtLeastOneBlobCrossedTheLine2 == true) {//change color of line if the vehicle has crossed it.
			cv::line(imgFrame2Copy, crossingLine[2], crossingLine[3], SCALAR_GREEN, 2);
		}
		else {
			cv::line(imgFrame2Copy, crossingLine[2], crossingLine[3], SCALAR_RED, 2);
		}

		/*bool blnAtLeastOneBlobCrossedTheLine3 = checkIfBlobsCrossedTheLine3(blobs, intHorizontalLinePosition32, carCount3);

		if (blnAtLeastOneBlobCrossedTheLine3 == true) {//change color of line if the vehicle has crossed it.
			cv::line(imgFrame2Copy, crossingLine[4], crossingLine[5], SCALAR_GREEN, 2);
		}
		else {
			cv::line(imgFrame2Copy, crossingLine[4], crossingLine[5], SCALAR_RED, 2);
		}

		bool blnAtLeastOneBlobCrossedTheLine4 = checkIfBlobsCrossedTheLine4(blobs, intVerticalLinePosition42, carCount4);

		if (blnAtLeastOneBlobCrossedTheLine4 == true) {//change color of line if the vehicle has crossed it.
			cv::line(imgFrame2Copy, crossingLine[6], crossingLine[7], SCALAR_GREEN, 2);
		}
		else {
			cv::line(imgFrame2Copy, crossingLine[6], crossingLine[7], SCALAR_RED, 2);
		}*/
		//carCount1 = round(carCount1 / 2);
		drawCarCountOnImage(carCount1,veh2_1, imgFrame2Copy);//to display car count in top right corner
		drawCarCountOnImage1(carCount2,veh2_2, imgFrame2Copy);
		cv::imshow("imgFrame2Copy", imgFrame2Copy);

		//cv::waitKey(0);                 // uncomment this line to go frame by frame for debugging

		// now we prepare for the next iteration

		currentFrameBlobs.clear();

		imgFrame1 = imgFrame2.clone();           // move frame 1 up to where frame 2 is

		if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {
			capVideo.read(imgFrame2);//return next video frame
		}
		else {
			std::cout << "end of video\n";
			break;
		}

		blnFirstFrame = false;
		frameCount++;
		chCheckForEscKey = cv::waitKey(1);
	}

	if (chCheckForEscKey != 27) {               // if the user did not press esc (i.e. we reached the end of the video)
		cv::waitKey(0);                         // hold the windows open to allow the "end of video" message to show
	}
	// note that if the user did press esc, we don't need to hold the windows open, we can simply let the program end which will close the windows
	if (carCount1 > 10)
	{
		carCount1 = ((int)carCount1 * 0.5) + 5;
	}
	if (veh2_1 > 10)
	{
		veh2_1 = ((int)veh2_1 * 0.5) + 5;
	}
	if (carCount2 > 10)
	{
		carCount2 = ((int)carCount2 * 0.5) + 5;
	}
	if (veh2_2 > 10)
	{
		veh2_2 = ((int)veh2_2 * 0.5) + 5;
	}
	std::ofstream myfile;								//writing and appending text
	myfile.open("example.txt", std::ios_base::app);
	//time_t t = time(0);   // get time now
	//struct tm * now = localtime(&t);
	//myfile << (now->tm_year + 1900) << '-'
	//	<< (now->tm_mon + 1) << '-'
	//	<< now->tm_mday << "        ";
	time_t nnow = time(0);
	char* dt = ctime(&nnow);
	myfile << dt;
	myfile << "carcount " << carCount1 << std::endl;
	myfile << "2 wheeler " << veh2_1 << std::endl;
	myfile << "carcount " << carCount2 << std::endl;
	myfile << "2 wheeler " << veh2_2 << std::endl;
	myfile.close();

	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs) {

	for (auto &existingBlob : existingBlobs) {

		existingBlob.blnCurrentMatchFoundOrNewBlob = false;

		existingBlob.predictNextPosition();
	}

	for (auto &currentFrameBlob : currentFrameBlobs) {

		int intIndexOfLeastDistance = 0;
		double dblLeastDistance = 100000.0;

		for (unsigned int i = 0; i < existingBlobs.size(); i++) {

			if (existingBlobs[i].blnStillBeingTracked == true) {

				double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);

				if (dblDistance < dblLeastDistance) {
					dblLeastDistance = dblDistance;
					intIndexOfLeastDistance = i;
				}
			}
		}

		if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 0.8 ){//* 0.5) {
			addBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);
		}//a match to the current blob has been found in the existing blobs
		else {
			addNewBlob(currentFrameBlob, existingBlobs);
		}//a new blob in incurred

	}

	for (auto &existingBlob : existingBlobs) {

		if (existingBlob.blnCurrentMatchFoundOrNewBlob == false) {
			existingBlob.intNumOfConsecutiveFramesWithoutAMatch++;
		}

		if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >= 15) {
			existingBlob.blnStillBeingTracked = false;
		}

	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex) {

	existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
	existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;

	existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());

	existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
	existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;

	existingBlobs[intIndex].blnStillBeingTracked = true;
	existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs) {

	currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;

	existingBlobs.push_back(currentFrameBlob);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double distanceBetweenPoints(cv::Point point1, cv::Point point2) {

	int intX = abs(point1.x - point2.x);
	int intY = abs(point1.y - point2.y);

	return(sqrt(pow(intX, 2) + pow(intY, 2)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName) {
	cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

	cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);//fill contours

	cv::imshow(strImageName, image);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName) {

	cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

	std::vector<std::vector<cv::Point> > contours;

	for (auto &blob : blobs) {
		if (blob.blnStillBeingTracked == true) {
			contours.push_back(blob.currentContour);
		}
	}

	cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);

	cv::imshow(strImageName, image);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool checkIfBlobsCrossedTheLine1(std::vector<Blob> &blobs, int &intHorizontalLinePosition11, int &carCount, int &veh2, cv::Mat &imgFrame2Copy) {
	bool blnAtLeastOneBlobCrossedTheLine = false;

	for (auto blob : blobs) {

		if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) {
			int prevFrameIndex = (int)blob.centerPositions.size() -  2;
			int currFrameIndex = (int)blob.centerPositions.size() - 1;

			/*std::ofstream myfile;								//writing and appending text
			myfile.open("example.txt", std::ios_base::app);
			
			myfile << blob.centerPositions[prevFrameIndex].y << "_____________________" << blob.centerPositions[currFrameIndex].y << std::endl;
			
			myfile.close();*/
			if ((blob.centerPositions[prevFrameIndex].y > intHorizontalLinePosition11 && blob.centerPositions[currFrameIndex].y <= intHorizontalLinePosition11) &&
			(blob.centerPositions[prevFrameIndex].x > intVerticalLinePosition11 && blob.centerPositions[prevFrameIndex].x <= intVerticalLinePosition12))
			{
				
				cv::rectangle(imgFrame2Copy, blob.currentBoundingRect, SCALAR_YELLOW, 2);
				blnAtLeastOneBlobCrossedTheLine = true;
				if (blob.currentBoundingRect.area() < 400 )					
				{
					veh2++;
				}
				else
				{
					carCount++;
				}
				
			}
		}

	}

	return blnAtLeastOneBlobCrossedTheLine;
}

bool checkIfBlobsCrossedTheLine2(std::vector<Blob> &blobs, int &intVerticalLinePosition, int &intHorizontalLinePosition1, int &intHorizontalLinePosition2, int &carCount, int &veh2,cv::Mat &imgFrame2Copy)
{
	bool blnAtLeastOneBlobCrossedTheLine = false;

	for (auto blob : blobs) {

		if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) {
			int prevFrameIndex = (int)blob.centerPositions.size() - 2;
			int currFrameIndex = (int)blob.centerPositions.size() - 1;

			if ((blob.centerPositions[prevFrameIndex].x < intVerticalLinePosition && blob.centerPositions[currFrameIndex].x >= intVerticalLinePosition) &&
				(blob.centerPositions[prevFrameIndex].y > intHorizontalLinePosition1 && blob.centerPositions[prevFrameIndex].y <= intHorizontalLinePosition2))
			{	
				
				cv::rectangle(imgFrame2Copy, blob.currentBoundingRect, SCALAR_YELLOW, 2);
				blnAtLeastOneBlobCrossedTheLine = true;
				if (blob.currentBoundingRect.area() < 300)
				{
					veh2++; 
				}
				else
				{
					carCount++;
				}
			}
		}

	}

	return blnAtLeastOneBlobCrossedTheLine;
}
/*
bool checkIfBlobsCrossedTheLine2(std::vector<Blob> &blobs, int &intVerticalLinePosition, int &carCount) {
	bool blnAtLeastOneBlobCrossedTheLine = false;

	for (auto blob : blobs) {

		if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) {
			int prevFrameIndex = (int)blob.centerPositions.size() - 2;
			int currFrameIndex = (int)blob.centerPositions.size() - 1;

			if (blob.centerPositions[prevFrameIndex].x < intVerticalLinePosition && blob.centerPositions[currFrameIndex].x >= intVerticalLinePosition) {
				//if (blob.centerPositions[prevFrameIndex].y < intHorizontalLinePosition && blob.centerPositions[currFrameIndex].y >= intHorizontalLinePosition) {
				carCount++;
				blnAtLeastOneBlobCrossedTheLine = true;
			}
		}

	}

	return blnAtLeastOneBlobCrossedTheLine;
}

bool checkIfBlobsCrossedTheLine4(std::vector<Blob> &blobs, int &intVerticalLinePosition, int &carCount) {
	bool blnAtLeastOneBlobCrossedTheLine = false;

	for (auto blob : blobs) {

		if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) {
			int prevFrameIndex = (int)blob.centerPositions.size() - 2;
			int currFrameIndex = (int)blob.centerPositions.size() - 1;

			if (blob.centerPositions[prevFrameIndex].x > intVerticalLinePosition && blob.centerPositions[currFrameIndex].x <= intVerticalLinePosition) {
				//if (blob.centerPositions[prevFrameIndex].y < intHorizontalLinePosition && blob.centerPositions[currFrameIndex].y >= intHorizontalLinePosition) {
				carCount++;
				blnAtLeastOneBlobCrossedTheLine = true;
			}
		}

	}

	return blnAtLeastOneBlobCrossedTheLine;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////
void drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy) {

	for (unsigned int i = 0; i < blobs.size(); i++) {

		if (blobs[i].blnStillBeingTracked == true) {
			cv::rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, SCALAR_RED, 2);

			int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
			double dblFontScale = blobs[i].dblCurrentDiagonalSize / 50.0;//size of car_id
			int intFontThickness = (int)std::round(dblFontScale * 1.0);

			cv::putText(imgFrame2Copy, std::to_string(i), blobs[i].centerPositions.back(), intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
		}	//displaying car_id in main video
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawCarCountOnImage(int &carCount,int &veh2, cv::Mat &imgFrame2Copy) {

	int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
	double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 100000.0;
	int intFontThickness = (int)std::round(dblFontScale * 1.5);
	int x = carCount;
	int y = veh2;
	if(carCount > 10)
	{
		carCount = ((int)carCount * 0.5) + 5;
	}
	if (veh2 > 10)
	{
		veh2 = ((int)veh2 * 0.4) + 5;
	}
	cv::Size textSize = cv::getTextSize(std::to_string(carCount), intFontFace, dblFontScale, intFontThickness, 0);
	cv::Point ptTextBottomLeftPosition;
	ptTextBottomLeftPosition.x = imgFrame2Copy.cols - 1 - (int)((double)textSize.width * 1.25);
	ptTextBottomLeftPosition.y = imgFrame2Copy.rows - 1 - (int)((double)textSize.height * 1.25);
	cv::putText(imgFrame2Copy, std::to_string(carCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, SCALAR_PINK, intFontThickness);
	
	cv::Size textSize1 = cv::getTextSize(std::to_string(veh2), intFontFace, dblFontScale, intFontThickness, 0);
	cv::Point ptTextBottomLeftPosition1;
	ptTextBottomLeftPosition1.x = imgFrame2Copy.cols - 1 - (int)((double)textSize1.width * 1.25);
	ptTextBottomLeftPosition1.y = ptTextBottomLeftPosition.y - 1 - (int)((double)textSize1.height * 1.25);
	cv::putText(imgFrame2Copy, std::to_string(veh2), ptTextBottomLeftPosition1, intFontFace, dblFontScale, SCALAR_PINK, intFontThickness);

	
	carCount = x;
	veh2 = y;
}

void drawCarCountOnImage1(int &carCount, int &veh2, cv::Mat &imgFrame2Copy) {

	int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
	double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 100000.0;
	int intFontThickness = (int)std::round(dblFontScale * 1.5);
	int x = carCount;
	int y = veh2;
	if (carCount > 10)
	{
		carCount = ((int)carCount * 0.5) + 5;
	}
	if (veh2 > 10)
	{
		veh2 = ((int)veh2 * 0.5) + 5;
	}
	cv::Size textSize = cv::getTextSize(std::to_string(carCount), intFontFace, dblFontScale, intFontThickness, 0);
	cv::Point ptTextBottomLeftPosition;
	//ptTextBottomLeftPosition.x = (int)((double)textSize.width * 1.25);
	ptTextBottomLeftPosition.y = (int)((double)textSize.height * 1.25);
	cv::putText(imgFrame2Copy, std::to_string(carCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, SCALAR_PINK, intFontThickness);

	cv::Size textSize1 = cv::getTextSize(std::to_string(veh2), intFontFace, dblFontScale, intFontThickness, 0);
	cv::Point ptTextBottomLeftPosition1;
	//ptTextBottomLeftPosition1.x = ptTextBottomLeftPosition.x + 1 + (int)((double)textSize1.width * 1.25);
	ptTextBottomLeftPosition1.y = ptTextBottomLeftPosition.y + 1 + (int)((double)textSize1.height * 1.25);
	cv::putText(imgFrame2Copy, std::to_string(veh2), ptTextBottomLeftPosition1, intFontFace, dblFontScale, SCALAR_PINK, intFontThickness);


	carCount = x;
	veh2 = y;
}








