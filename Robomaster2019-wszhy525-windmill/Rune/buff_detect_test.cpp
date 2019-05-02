/*********************************************************
*������
*	����ͨ���Ź����Աߵ�10�����ֿ�����λ�Ź����λ��
*�������
*	�����У���ʱ�벻�����������
*ԭ��
*	����õ�ͼƬб��һ���̶�ʱ�򣬾���ֵ����ᵼ��5�����ֿ�
*	��һ�������Աߵ�ǽ����һ��ʹ�ÿ򲻳�����ת���Σ��γ�©��,
*	����бһЩ��ʱ��ᵼ��һ������л��ֿ鶼��ⲻ����
*���ߣ�
*	ʩ��, <213162637@seu.edu.cn>
*��������:
*	2018/4/21
***********************************************************/

#include "BuffDetector.h"
#include "../General/opencv_extended.h"
#include "../General/numeric_rm.h"

int count = 0;

//#define SHOW_THRESHOLD
//#define SHOW_CONTOURS
//#define SHOW_PROCESS_FOUND_RECT
//#define SHOW_FOUND_RECTS
#define SHOW_LAST_RECTS
//#define SHOW_ROTATEDRECT_BUFFROI

cv::Mat srcBuff;
cv::RotatedRect avrRect;
bool located_half = false;//��¼�Ƿ���һ��5����������


enum locatedSituation
{
	LOCATED_NO = 0,//���ߵ�5�����ζ�û����
	LOCATED_PART = 1,//���ߵ�5��������һ�������ˣ�����һ��û����
	LOCATED_FULL = 2,//10�����ζ�������
};

int located_situation = LOCATED_NO;

////�жϸ���������ת�����п���ڸߵ������࣬���Ǹߴ��ڿ��������
//double Widthnum_MoreThan_Heightnum(std::vector<cv::RotatedRect> RotatedRects)
//{
//	double agree = 0;
//	for (int i = 0; i < RotatedRects.size(); ++i)
//	{
//		if (RotatedRects[i].size.width > RotatedRects[i].size.height)
//			agree++;
//		else if (RotatedRects[i].size.width < RotatedRects[i].size.height)
//			agree--;
//	}
//	return agree;
//}


cv::RotatedRect& adjustRec(cv::RotatedRect& rec)
{
	using std::swap;
	cv::Size2f& size = rec.size;
	float& angle = rec.angle;
	if (size.width < size.height)
	{
		swap(size.width, size.height);
		angle += 90.0;
	}
	while (angle >= 90.0) angle -= 180.0;
	while (angle < -90.0) angle += 180.0;
	if (size.height >= 0.5*size.width)
	{
		if (-90.0 <= angle && angle < -45.0)
		{
			swap(size.width, size.height);
			angle += 90.0;
		}
		else if (45.0 < angle && angle <= 90.0)
		{
			swap(size.width, size.height);
			angle -= 90.0;
		}
	}
	return rec;
}

//������groupRectangles�����¡�TODO.....
void calcAvrRect(std::vector<cv::RotatedRect> final_rects)
{
	float avrAngle = 0.0;
	float avrWidth = 0.0;
	float avrHeight = 0.0;
	float avrArea = 0.0;
	cv::Point2f avrCenter = (0, 0);
	for (auto rect : final_rects)
	{
		avrAngle += rect.angle;
		avrWidth += rect.size.width;
		avrHeight += rect.size.height;
		avrArea += rect.size.area();
		avrCenter += rect.center;
	}
	avrRect.angle = avrAngle / final_rects.size();
	avrRect.size.width = avrWidth / final_rects.size();
	avrRect.size.height = avrHeight / final_rects.size();
	avrRect.center = cv::Point(avrCenter.x / final_rects.size(), avrCenter.y / final_rects.size());
}

void buff_FindRoi(cv::Mat gray)
{

	cv::Mat show_contours = gray.clone();
	cv::threshold(gray, gray, 100, 255, CV_THRESH_OTSU);
	//cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 1));
	//cv::morphologyEx(gray, gray, cv::MORPH_OPEN, element);
	//cv::morphologyEx(gray, gray, cv::MORPH_ERODE, element);

#ifdef SHOW_THRESHOLD
		cv::imshow("threshold", gray);
		cv::waitKey(1);
#endif // SHOW_THRESHOLD
	//std::cout << count++ << std::endl;

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::RotatedRect> RotatedRects;
	cv::findContours(gray, contours, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
	//����֮ǰ�����ŵľ��ε�ƽ��������ɸѡ��һ�ֵ�����
	for (int i = 0; i < contours.size(); ++i)
	{
		cv::RotatedRect rect = adjustRec(cv::minAreaRect(contours[i]));
		if (located_situation == LOCATED_FULL || located_situation == LOCATED_PART)
		{
			if (rmnum::absApproxEqual(rect.angle, avrRect.angle, 15) //�Ƕ����һ����Χ֮��
				//&& rmnum::relApproxEqual(rect.size.area(), avrRect.size.area(), 0.5) //��С������һ����Χ��
				&& rmnum::relApproxEqual(rect.size.width, avrRect.size.width, 0.4)
				&& rmnum::relApproxEqual(rect.size.height, avrRect.size.height, 0.4))
				//&& rmnum::relApproxEqual(rect.size.width / rect.size.height, avrRect.size.width / avrRect.size.height, 0.5)) //����ȵ���������һ����Χ֮��
			{
				RotatedRects.push_back(rect);
			}
		}
		else
		{
			if (rect.size.area() < 500 || rect.size.area() > 10000
				|| rect.size.width / rect.size.height < 0.25 
				|| rect.size.width / rect.size.height > 4)
			{
				continue;
			}
			RotatedRects.push_back(rect);
		}
	}


	std::vector<cv::RotatedRect> RotaRects_choose_nums(RotatedRects);

#ifdef SHOW_CONTOURS
	cvex::showRectangles("show_contours", RotatedRects, srcBuff, show_contours, cvex::GREEN, 1);
#endif // SHOW_CONTOURS

	located_situation = LOCATED_NO;
	//�ҵ��Ա�10������
	std::vector<cv::RotatedRect> found_rects;
	std::vector<cv::RotatedRect> final_rects;

	//�ӵ�һ���������ң����ֻʣ��1������ֱ����ȥ��
	while (RotatedRects.size() > 1)
	{
		//�Ѿ�����10���Աߵľ���
		/*if (found_rects.size() == 10 )
			break;*/
			//ѡ����һ��������Ϊ������
		cv::RotatedRect based_rect = adjustRec(*(RotatedRects.begin()));
		RotatedRects.erase(RotatedRects.begin());
		found_rects.push_back(based_rect);
		for (int i = 0; i < found_rects.size() - 1; ++i)
		{
			if (found_rects[i].center == based_rect.center)
				found_rects.pop_back();
		}
		int found_rectCount = 1;
#ifdef SHOW_PROCESS_FOUND_RECT
		cv::Mat show_found_rect = srcBuff.clone();
		cvex::showRectangle("found_rect", based_rect, srcBuff, show_found_rect, cvex::GREEN, 1);
#endif // SHOW_PROCESS_FOUND_RECT
		//������ɸѡ�����о��ζ����������ز����ıȽ�

		for (auto compared_rectIdx = RotatedRects.begin() ; compared_rectIdx != RotatedRects.end() ; )
		{

			cv::RotatedRect compared_rect = adjustRec(*compared_rectIdx);
#ifdef SHOW_PROCESS_FOUND_RECT
			cv::Mat show_finding_rect = srcBuff.clone();
			cvex::showRectangle("show_finding_rect", *compared_rectIdx, srcBuff, show_finding_rect, cvex::RED, 100);
#endif // SHOW_PROCESS_FOUND_RECT
			//�ҵ�һ������������ľ���
			if (rmnum::absApproxEqual(based_rect.angle, compared_rect.angle, 10) //�Ƕ����һ����Χ֮��
				//&& rmnum::relApproxEqual(based_rect.size.area(), compared_rect.size.area(), 0.35) //��С������0.3����
				&& rmnum::relApproxEqual(based_rect.size.width / based_rect.size.height, compared_rect.size.width / compared_rect.size.height, 0.3) //����ȵ���������0.2֮��				
				&& rmnum::absApproxEqual(based_rect.center.x, compared_rect.center.x, based_rect.size.width) //���ĵ����������ڰ�������
				&& rmnum::absApproxEqual(based_rect.center.y, compared_rect.center.y, based_rect.size.height, based_rect.size.height * 3))
			{
				found_rects.push_back(compared_rect);
				++found_rectCount;
				based_rect = compared_rect;
				compared_rectIdx = RotatedRects.erase(compared_rectIdx);

#ifdef SHOW_PROCESS_FOUND_RECT
				cvex::showRectangle("found_rect", *compared_rectIdx, srcBuff, show_found_rect, cvex::BLUE, 1);
#endif // SHOW_PROCESS_FOUND_RECT
			}
			//û�ҵ��������û����5���������ҵ���һ�������е�Զ���������Ƶ�
			else if (rmnum::absApproxEqual(based_rect.angle, compared_rect.angle, 10) //�Ƕ����һ����Χ֮��
				&& rmnum::relApproxEqual(based_rect.size.width / based_rect.size.height, compared_rect.size.width / compared_rect.size.height, 0.3) //����ȵ���������0.2֮��				
				&& rmnum::absApproxEqual(based_rect.center.x, compared_rect.center.x, based_rect.size.width) //���ĵ�����������һ�������
				&& rmnum::absApproxEqual(based_rect.center.y, compared_rect.center.y, based_rect.size.height, based_rect.size.height * 9)
				&& found_rectCount < 5)
			{
				found_rects.push_back(compared_rect);
				++found_rectCount;
				based_rect = compared_rect;
				compared_rectIdx = RotatedRects.erase(compared_rectIdx);
			}
			else
				compared_rectIdx++;
		}
		if (found_rectCount < 3)
		{
			found_rects.erase(found_rects.end() - found_rectCount, found_rects.end());
		}
		else if (found_rectCount > 5)
		{
			std::vector<double> dist;
			for (auto i = found_rects.end() - found_rectCount; i != found_rects.end() - 1; ++i)
			{
				double distance = cvex::distance((*i).center, (*(i + 1)).center);
				dist.push_back(distance);
			}

		}
		if (found_rectCount == 5)
		{
			//����м�ȱһ���������ҵ�һ����ô�죿��TODO....
			located_half = true;
			for (auto i = found_rects.end() - 5; i != found_rects.end(); ++i)
			{
				final_rects.push_back(*i);
			}
			found_rects.erase(found_rects.end() - found_rectCount, found_rects.end());
		}
	}
#ifdef SHOW_FOUND_RECTS
	cv::Mat show_found_rects;
	cvex::showRectangles("show_found_rects", found_rects, srcBuff, show_found_rects, cvex::GREEN, 1);

#endif // SHOW_FOUND_RECTS

	if (located_half == false)
	{
		located_situation = LOCATED_NO;
	}
	else
	{
		//���������ҵ��ľ��ε�ƽ���ߴ�
		calcAvrRect(final_rects);
		if (final_rects.size() == 10)
		{
			located_situation = LOCATED_FULL;
		}
		else
		{
			located_situation = LOCATED_PART;
			//��һ�ߵ�5���Ѿ��ҵ�����һ������ĳ��ԭ���Ҳ�ȫ�������Ƶľ��ηŽ�ȥ�����Ҽ����㼸��
			if (found_rects.size() < 10)
			{
				for (auto i : found_rects)
				{
					if (/*rmnum::relApproxEqual(i.size.area(), avrRect.size.area(), 0.35) //��С������ͬ
						&&*/ rmnum::relApproxEqual(i.size.width / i.size.height, avrRect.size.width / avrRect.size.height, 0.5) //����Ȼ�����ͬ			
						/*&& rmnum::absApproxEqual(i.center.y, avrRect.center.y, avrRect.size.height * 6)*/)
					{
						final_rects.push_back(i);
					}
				}
			}
			//�ҵ���3�ж���5�����Ƶľ��Σ���Ƚ�ÿһ���ƽ����С��ȡ�Ͻӽ���������Ϊ���ľ���
			else
			{

			}
			//�������10�������ﳤ��ȴ�С���̫���ԣ��������found_rects���ҿ��ܵ�5��
			/*if (!rmnum::relApproxEqual((*final_rects.begin()).size.width / (*final_rects.begin()).size.height,
				(*(final_rects.begin() + 6)).size.width / (*(final_rects.begin() + 6)).size.height, 0.5))
			{
				for (auto i = found_rects.begin(); i != found_rects.end();)
				{
				}
			}*/
			cv::Mat avr;
			cvex::showRectangle("avrRect", avrRect, srcBuff, avr, cvex::RED, 1);
			calcAvrRect(final_rects);
		}
		//avrRect.size.area() = avrArea / found_rects.size();
	}
#ifdef SHOW_LAST_RECTS
	cv::Mat show_final_rects;
	cvex::showRectangles("show_last_rects", final_rects, srcBuff, show_final_rects, cvex::GREEN, 1);
	//cv::waitKey();
#endif // SHOW_LAST_RECTS

	if (final_rects.size() != 10)
	{
		cv::waitKey();
		//cv::Mat avr;
		//cvex::showRectangle("avrRect", avrRect, srcBuff, avr, cvex::RED, 0);
	}

	

	//���ROI����
	/*if (final_rects.empty())return;
	std::vector<cv::Point2f> saved_points;
	for (auto i = final_rects.begin(); i != final_rects.end(); ++i)
	{	
		cv::Point2f point[4];
		(*i).points(point);
		for (int k = 0; k < 4; ++k)
		{
			saved_points.push_back(point[k]);
		}
	}
	cv::RotatedRect RotaRect_buffRoi = cv::minAreaRect(saved_points);
	cv::Point2f points[4];
	RotaRect_buffRoi.points(points);
	std::vector<cv::Point2f>roi_4corners(points,points+4);
	//cv::Point2f* points1= roi_4corners.data;

	/*cv::Point2f roi_4corners[4];
	RotaRect_buffRoi.points(roi_4corners);

#ifdef SHOW_ROTATEDRECT_BUFFROI
	cv::Mat RotatedRect_buffRoi = srcBuff.clone();
	cvex::showRectangle("buffRoi", RotaRect_buffRoi, srcBuff, RotatedRect_buffRoi, cvex::GREEN, 1);
#endif // SHOW_ROTATEDRECT_BUFFROI

	//ɸѡ���Ź���
	std::vector<cv::RotatedRect> buffnums;
	for (auto & roaRect : RotaRects_choose_nums)
	{
		//�����ת���ε����ĵ㲻�ڿ����roi�������������������϶����ǾŹ����������
		if (!RotaRect_buffRoi.boundingRect().contains(roaRect.center))
			continue;	

		if (cv::pointPolygonTest(roi_4corners, roaRect.center, false) != 1)
			continue;
		//cv::rectangle(srcBuff, RotaRect_buffRoi.boundingRect(), cvex::GREEN, 2);
		//cv::imshow("roi", srcBuff);
		buffnums.push_back(roaRect);
		//ȥ���Աߵ�10�����κ���10��������Ĳ��־��Σ�
		int low_than_base = 0;
		int high_than_base = 0;
		for (int i = 0; i < final_rects.size(); ++i)
		{	
			if (final_rects[i].center == roaRect.center)
			{
				buffnums.pop_back();
				break;
			}
				
		}	
	}
	/*for (auto i = buffnums.begin(); i != buffnums.end();)
	{

	}*/
	//cvex::showRectangles("buffnum", buffnums, srcBuff, srcBuff, cvex::GREEN, 0);
}

/*
int main()
{
	cv::VideoCapture capture("../Samples/buff1.mp4");
	if (!capture.isOpened()) return -1;
	while (capture.read(srcBuff)) 
	{
		count++;
		//if (count < 810)continue;
		if (srcBuff.empty()) continue;
		cv::Mat workImg = srcBuff.clone();
		cv::cvtColor(workImg, workImg, CV_BGR2GRAY);
		buff_FindRoi(workImg);
		cv::waitKey(1);
	}
	//srcBuff = cv::imread("buff.jpg");
	return 0;
}
*/