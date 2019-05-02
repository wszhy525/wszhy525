#pragma once
#include "opencv2/core/core.hpp"
#include<iostream>
#include <opencv2/opencv.hpp>
#include"../General/General.h"
//#define DEBUG

namespace rm
{


struct AngleSolverParam
{
	cv::Mat CAM_MATRIX;			// ����ڲ�
	cv::Mat DISTORTION_COEFF;	//����������
	//��ά����ĵ�λ�Ǻ��� 
	static std::vector<cv::Point3f> POINT_3D_OF_ARMOR_BIG;
	static std::vector<cv::Point3f> POINT_3D_OF_ARMOR_SMALL;
	static std::vector<cv::Point3f> POINT_3D_OF_RUNE;
	double Y_DISTANCE_BETWEEN_GUN_AND_CAM = 0;//If gun is under the cam, this variable is positive.
	cv::Size CAM_SIZE = cv::Size(1920, 1080);
	double GUARD_HIGHT = 4000;

	/*
	* @brief �����������
	* @param Input int id 1-4��Ӧ��Ӧ����ͷ��5��Ӧ�ٷ�ͼ����0��Ӧ������˫Ŀ����ͷ��Ŀ
	*/
	void readFile(const int id);
};

/**
*	solve by PNP, that is, using four points to detect the angle and distance.
*	It's not very useful if the armor is far.If it' far try solve by one point
*/
class AngleSolver
{
public:
    float history_ErrAngle;
    float history_PreAngle;
    float history_PrePreAngle;
    float history_PrePrePreAngle;
    const cv::Vec2f predict(cv::Vec2f now_ErrAngle,double timeStamp);
    void initPredictor();

    AngleSolver();
    AngleSolver(const AngleSolverParam& AngleSolverParam);

    /*
     *  Initialize with parameters
     */
    void init(const AngleSolverParam& AngleSolverParam);

	enum AngleFlag
	{
		ANGLE_ERROR = 0,                //�������
		ONLY_ANGLES = 1,				//ֻ�ܵõ��Ƕ���Ϣ
		TOO_FAR = 2,					//����̫Զ��������ܲ�׼ȷ
		ANGLES_AND_DISTANCE = 3			//�ǶȺ;��붼׼ȷ
	};

	
	/*
	* @brief ���ö�άװ�װ��,���Խ�����������Ҳ���Խ����Ľ�����
	* @param Input armorPoints/centerPoint
	*/
	void setTarget(const std::vector<cv::Point2f> objectPoints, int objectType);  //���ö�άװ�װ�������
	void setTarget(const cv::Point2f centerPoint, int objectType);//��ֵ���ĵ�����
	//void setTarget(const std::vector<cv::Point2f> runePoints);  //���ö�ά�����
															

	/*
	* @brief ����ѡ�����㷨���ͽ���Ƕ�
    */
	AngleFlag solve();

	/*
	*      z: direction of the shooter
	*     /
	*  O /______x
	*    |
	*    |
	*    y
	*/
	void compensateOffset();
	void compensateGravity();

	/*
	* @brief ���ø���ͼ��ֱ���
	*/
	void setResolution(const cv::Size2i& image_resolution);

	/*
	* @brief �������г���ĳ������࣬0�����ڱ���1��������2����Ӣ�ۣ�
	*/
	void setUserType(int usertype);

	/*
	* @brief ���õ������ͣ�0����Сװ�װ壬1�����װ�װ壻
	*/
	void setEnemyType(int enemytype);

	/*
	* @brief �����ӵ��ٶȣ�
	*/
	void setBulletSpeed(int bulletSpeed);

	/*
	* @brief ��ʾ����ĽǶȽ��
	*/
	const cv::Vec2f getAngle();

	/*
	* @brief ������������
	*/
	//const cv::Vec2f getCompensateAngle();

    //const cv::Vec2f getPredictedAngle();

	/*
	* @brief ��ʾ����ľ�����������PNPʱ
	*/
    double getDistance();

    const cv::Vec2f getCompensateAngle();

#ifdef DEBUG
	/*
	* @briefչʾװ�װ��ĽǵĶ�ά����
	*/
	void showPoints2dOfArmor();

	/*
	* @brief չʾ�������
	*/
	void showTvec();

	/*
	* @brief ����ǶȺ�ŷ�Ͼ���
	*/
	void showEDistance();

	/*
	* @brief ���װ�װ���������
	*/
	void showcenter_of_armor();

	/*
	* @brief ��ʾ����ǶȽ��
	*/
	void showAngle();

	/*
	* @brief �����㷨��Ϣ
	*/
	int showAlgorithm();
#endif // DEBUG

private:
	AngleSolverParam _params;
	cv::Mat _rVec = cv::Mat::zeros(3, 1, CV_64FC1);//��ʼ��������� 
	cv::Mat _tVec = cv::Mat::zeros(3, 1, CV_64FC1);//��ʼ���������
	std::vector<cv::Point2f> point_2d_of_armor;
	std::vector<cv::Point2f> point_2d_of_rune;
	enum solverAlg
	{
		ONE_POINT = 0,
		PNP4 = 1
	};
	int angle_solver_algorithm = 1;//if 1 ,using PNP solution, if 0 using OnePoint solution,if 2 solving the rune by pnp.
	cv::Point2f centerPoint;
	std::vector<cv::Point2f> target_nothing;
	double _xErr, _yErr, _euclideanDistance;
	cv::Size2i image_size = cv::Size2i(1920, 1080);
	int user_type = 1;//���г���ĳ������࣬0�����ڱ���1��������2����Ӣ�ۣ�
	int enemy_type = 1;//�������ͣ�0����Сװ�װ壬1�����װ�װ壻
	double _bullet_speed = 22000;
	double _rune_compensated_angle = 0;
	int is_shooting_rune = 0;
	cv::Mat _cam_instant_matrix;// ����ڲ�
    //std::vector<cv::Point2f> _x_time_points;
	//std::vector<cv::Point2f> _y_time_points;
	//int framecount=0;
};

}
