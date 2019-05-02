

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "kcftracker.hpp"

//#include <dirent.h>
#include "dirent_win.h" // For windows

//#include <unistd.h>
//#include <sys/time.h>
#include"ArmorDetector.h"

using namespace std;
using namespace cv;
using namespace cv::ml;

struct para
{
	float oldPeakValue[20];

	unsigned int crntCount = 0;

    unsigned int continueLost = 0;

    bool lost = false;
};

enum
{
	ARMOR_DETECT = 4,
	KCF_TRACE = 5,
	ARMOR_NO = 0,		//没找到
	ARMOR_LOST = 1,		//跟踪丢失
	ARMOR_GLOBAL = 2,	//全局检测得到装甲板
	ARMOR_LOCAL = 3		//局部（跟踪）得到装甲板
};

bool isTargetLost(float crntPeakValue, para* _para)
{
	const unsigned int k_count = 20;

	if (_para->lost)
	{
		return true;
	}
	else
	{
		if (_para->crntCount >= k_count)
		{
			double u = 0;
			for (unsigned int ui = 0; ui < k_count; ui++)
			{
				u += _para->oldPeakValue[ui];
			}
			u /= k_count;

			double sigma = 0;
			for (unsigned int ui = 0; ui < k_count; ui++)
			{
				sigma += (_para->oldPeakValue[ui] - u) * (_para->oldPeakValue[ui] - u);
			}
			sigma /= k_count;
			sigma = sqrt(sigma);

			double threshold = max(u - sigma * 4, 0.3);

			std::cout << "u: " << u << ", sigma: " << sigma << ", u-sigma*lamda: " << threshold << endl;

			if (crntPeakValue < threshold)
			{
				if (_para->continueLost > 3)
				{
					_para->lost = true;
					return true;
				}
				else
				{
					_para->continueLost++;
				}
			}
			else
			{
				_para->oldPeakValue[_para->crntCount % k_count] = crntPeakValue; // update peak value list
				_para->continueLost = 0;
			}
		}
		else
		{
			_para->oldPeakValue[_para->crntCount] = crntPeakValue;

			if (crntPeakValue < 0.5)
			{
				if (_para->continueLost > 3)
				{
					_para->lost = true;
					return true;
				}
				else
				{
					_para->continueLost++;
				}
			}
			else
			{
				_para->continueLost = 0;
			}
		}
	}

	_para->crntCount++;

	return false;
}

// Image change light
void lightChange(Mat & img, float alpha, float beta)
{
	for (int i = 0; i < img.rows; i++)
	{
		unsigned char * pdata = img.ptr(i);

		for (int j = 0; j < img.cols; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				pdata[3 * j + k] = saturate_cast<uchar>(pdata[3 * j + k] * alpha + beta);
			}
		}
	}
}

//Global variables 
int model = ARMOR_DETECT;


Rect getRect(std::vector<Point2f> vec)
{
	Rect rect;
	std::vector<Point2f> _vec;
	float x_01 = (vec[1].x - vec[0].x) / 3;
	float x_32 = (vec[2].x - vec[3].x) / 3;
	float y_01 = (vec[1].y - vec[0].y) / 3;
	float y_32 = (vec[2].y - vec[3].y) / 3;
	std::cout << "111" << endl;
	_vec.push_back(Point2f(vec[0].x + x_01, vec[0].y + y_01));
	_vec.push_back(Point2f(vec[1].x - x_01, vec[1].y - y_01));
	_vec.push_back(Point2f(vec[3].x + x_32, vec[3].y + y_32));
	_vec.push_back(Point2f(vec[2].x - x_32, vec[2].y - y_32));
	rect = boundingRect(_vec);
	return rect;
}

int main(int argc, char* argv[]) {

	if (argc > 5) return -1;

	bool HOG = true;
	bool FIXEDWINDOW = false;
	bool MULTISCALE = true;
	//bool SILENT = true;
	bool SILENT = false;
	bool LAB = false;

	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "hog") == 0)
			HOG = true;
		if (strcmp(argv[i], "fixed_window") == 0)
			FIXEDWINDOW = true;
		if (strcmp(argv[i], "singlescale") == 0)
			MULTISCALE = false;
		//if ( strcmp (argv[i], "show") == 0 )
		//	SILENT = false;
		if (strcmp(argv[i], "silent") == 0)
			SILENT = true;
		if (strcmp(argv[i], "lab") == 0)
		{
			LAB = true;
			HOG = true;
		}
		if (strcmp(argv[i], "gray") == 0)
			HOG = false;	
	}

	// Create KCFTracker object
	KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);

	// Frame readed
	Mat frame;

	VideoCapture capture;
	capture.open("151.avi");
	if (!capture.isOpened())
	{
		std::cout << "capture device failed to open!" << endl;
		return -1;
	}
	capture >> frame;
	
	// Tracker results
	Rect result;

	// Frame counter
	int nFrames = 0;
		
	float peak_value = 0; // With peak value

	para* _para = new para{{0}, 0, 0, false };
    
	//装甲板识别初始化
	Rect kcf_roi;
	rm::ArmorDetector test1;
	test1.setEnemyColor(2);
	
	while (!frame.empty())
	{
		switch (model)
		{
			//装甲板识别
			case ARMOR_DETECT:			
				test1.loadImg(frame);
				test1.detect();
				if (test1._flag == ARMOR_LOST)
				{
					model = KCF_TRACE;
					//kcf初始化
					tracker.init(kcf_roi, frame);
					cv::rectangle(frame, kcf_roi.tl(), kcf_roi.br(), Scalar(0, 255, 255), 1, 8);
				}
				if (test1._flag > 1)
				{
					kcf_roi = getRect(test1._vertex);
					for (int i = 0; i < 4; i++)
					{
						line(frame, test1._vertex[i], test1._vertex[(i + 1) % 4], Scalar(0, 255, 0));
					}
				}
				break;
			//KCF追踪
			case KCF_TRACE:
				result = tracker.update(frame, &peak_value); // With peak value
				cv::rectangle(frame, Point(result.x, result.y), Point(result.x + result.width, result.y + result.height), Scalar(0, 255, 255), 1, 8);
				cv::circle(frame, Point((result.x + result.width / 2), (result.y + result.height / 2)), 4, Scalar(0, 0, 255), 2); //
				std::cout << "Peak value: " << peak_value << endl; // With peak value
				if (isTargetLost(peak_value, _para))
				{
					std::cout << "target lost..." << endl;
					peak_value = 0;
					delete _para;
					_para = new para{ {0}, 0, 0, false };
					model = ARMOR_DETECT;
					nFrames = 0;
				}
				if (!SILENT) {
					//imshow("Image", frame);
					//waitKey(1);

					stringstream buf;
					buf << nFrames;
					//buf << " FPS:" << cvRound(tic / t);
					/*buf << " FPS:" << cvRound(1000.0 / t);*/
					string num = buf.str();
					putText(frame, num, Point(15, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(100, 0, 255), 3);
				}
				nFrames++;
		}
		imshow("Tracker", frame);
		capture >> frame;
		if (waitKey(1) == 'q')
			break;
	}
	

	//while(!frame.empty())
	//{
	//	// First frame, give the groundtruth to the tracker
	//	if (nFrames == 0)
	//	{
	//		//装甲板识别			
	//		test1.loadImg(frame);
	//		test1.detect();
	//		if (test1._flag > 1)
	//		{
	//			kcf_roi = getRect(test1._Vertex);
	//		}
	//		else
	//		{
	//			imshow("Tracker", frame);
	//			waitKey(1);
	//			capture >> frame;
	//			continue;
	//		}
	//		
	//		tracker.init(kcf_roi, frame);
	//		rectangle(frame, kcf_roi.tl(), kcf_roi.br(), Scalar(0, 255, 0), 1, 8);
	//	}
	//	// Update
	//	else
	//	{
	//		//result = tracker.update(frame);
	//		result = tracker.update(frame, &peak_value); // With peak value
	//		rectangle(frame, Point(result.x, result.y), Point(result.x + result.width, result.y + result.height), Scalar(0, 255, 255), 1, 8);
	//		circle(frame, Point((result.x + result.width / 2), (result.y + result.height / 2)), 4, Scalar(0, 0, 255), 2); //
	//		cout << "Peak value: " << peak_value << endl; // With peak value

	//		if (isTargetLost(peak_value, _para))
	//		{
	//			cout << "target lost..." << endl;
	//			nFrames = 0;
	//			peak_value = 0;
	//			delete _para;
	//			_para = new para{ {0}, 0, 0, false };
	//			capture >> frame;
	//			continue;
	//		}
	//	}

	//	cout << endl;

	//	//lightChange(frame, 1.5, 10);
	//}
}
