#include "AngleSolver.hpp"
#include "../Armor/ArmorDetector.h"
//#include "Predictor.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#define JHAngletest
#define DEBUG
using namespace cv;
using namespace std;

void onSharpChange(int sharp, void* capture)
{
	((cv::VideoCapture*)capture)->set(CV_CAP_PROP_SHARPNESS, sharp - 5);
}
void onExpChange(int exposure, void* capture)
{
	((cv::VideoCapture*)capture)->set(CV_CAP_PROP_EXPOSURE, exposure - 10);
}

int main()
{

	cv::VideoCapture capture(1);//open default camera

								/*cv::VideoWriter write;
								string outfile = "E://samples.avi";
								double r = capture.get(CV_CAP_PROP_FPS);*/

	capture.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
	cv::Size fram_size = cv::Size(capture.get(CV_CAP_PROP_FRAME_WIDTH), capture.get(CV_CAP_PROP_FRAME_HEIGHT));
	//write.open(outfile, -1, r, fram_size, true);

	//size_t frame_cnt = 0;

	int exposure = 0, sharpness = 0;
	cv::namedWindow("control panel");
	cv::createTrackbar("exposure", "control panel", &exposure, 10, onExpChange, &capture);
	cv::createTrackbar("sharpness", "control panel", &sharpness, 10, onSharpChange, &capture);
	//capture.set(CV_CAP_PROP_EXPOSURE, -7);

	rm::ArmorParam param;
	rm::ArmorDetector armorDetector(param);
	/*	VideoCapture capture;
	String PathName1 = "E:/roborobo/Robomaster_sample/official/�����ز������Һ�Խ�-ev-0.MOV";
	String PathName2 = "E:/roborobo/old_samples/video_pieces/������Ƶ1.mp4";
	String PathName3 = "E:/roborobo/Robomaster_sample/Samples/car1.mp4";
	capture.open(PathName3);*/
	int cam_number = 4;
	AngleSolverParam param2;
	param2.readFile(cam_number);
	AngleSolver angleSolver(param2);
	AngleFlag solve_result;

	Point2f targetCenter;
	std::vector<cv::Point2f> targetPoints;
	std::vector<cv::Point2f> targetEmpty;
	for (int tt = 0; tt <= 3; tt++)
		targetEmpty.push_back(cv::Point2f(0.0, 0.0));

	//Predictor predictor;
	//int frame_num = 0;

	if (!capture.isOpened())
	{
		std::cerr << "can't open camera" << std::endl;
		system("pause");
		return -1;
	}

	while (1)
	{
		Mat frame;
		capture >> frame;
		//waitKey(0);
		if (frame.empty()) continue;



		armorDetector.loadImg(frame);
		armorDetector.detectArmors();
		if (angleSolver.showAlgorithm() == 0)
		{
			targetCenter = armorDetector.getTargetArmorCenter();
			if (targetCenter == Point2f(0.0, 0.0)) {
				std::cout << "no armor detected" << std::endl; continue;
			}
			angleSolver.setTarget(targetCenter, 1);
		}
		if (angleSolver.showAlgorithm() == 1)
		{
			targetPoints = armorDetector.getTargetArmor4Points();
			if (targetPoints == targetEmpty)
			{
				std::cout << "no armor detected" << std::endl;
				continue;
			}
			angleSolver.setTarget(targetPoints, 1);
		}

#ifdef DEBUG
		angleSolver.showPoints2dOfArmor();
#endif // DEBUG

		/*
		if (!(angleSolver.isTargetGiven()))
		{
		std::cout << "no armor detected." << std::endl;
		continue;
		}
		*/

		solve_result = angleSolver.solve();

#ifdef DEBUG

		std::cout << solve_result << std::endl;
		//angleSolver.showTvec();
		angleSolver.showAngle();
		angleSolver.showEDistance();
#endif // DEBUG
	}

	system("pause");
	return 0;
}
/*
Anglesolver���ʹ��˵����
ʹ�ò��裺
1.�������׵Ĳ�����AngleSolverParam ʵ�����£�
int cam_number = 2;
AngleSolverParam param2;
param2.readFile(cam_number);
AngleFlag solve_result;
2.����Anglesolver�� ʵ�����£�
AngleSolver angleSolver(param2);
3.������ز��� ʵ�����£�
angleSolver.setResolution(imagesize);
angleSolver.setUserType(1);
angleSolver.setBulletSpeed();
4.����ʶ��� ��ʶ��Ϊ�ղ�����Ƕ� ʵ�����£�
if (angleSolver.showAlgorithm() == 0)
{
targetCenter = armorDetector.getTargetArmorCenter();
if (targetCenter == Point2f(0.0, 0.0)) {
std::cout << "no armor detected" << std::endl;
continue;
}
angleSolver.setTarget(targetCenter, 1);
}
if (angleSolver.showAlgorithm() == 1)
{
targetPoints = armorDetector.getTargetArmor4Points();
if (targetPoints == targetEmpty)
{
std::cout << "no armor detected" << std::endl;
continue;
}
angleSolver.setTarget(targetPoints, 1);
}
if (angleSolver.showAlgorithm() == 2)......angleSolver.setTarget(runePoints);
5.����Ƕ� ʵ�����£�
solve_result = angleSolver.solve();
(5.5.������Ҫ ���Կ����������� ����Ϊ��ͬ�ĳ��ߴ���в�ͬ ���ܻ���ɲ�׼ȷ
angleSolver.getCompensateAngle()��
)
6.��ý�� ʵ�����£�
alpha=angleSolver.getAngle();
dis=angleSolver.getDistance();
*/
