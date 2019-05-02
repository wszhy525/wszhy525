#include "AngleSolver.hpp"
#include "opencv2/opencv.hpp"
#include "math.h"

using namespace cv;
using namespace std;

namespace rm
{
void AngleSolver::initPredictor()
{
    history_ErrAngle=0;//history_ErrAngle[0]=0,history_ErrAngle[1]=0;
    history_PreAngle=0;//[0]=0,history_PreAngle[1]=0;
    history_PrePreAngle=0;//[0]=0,history_PrePreAngle[1]=0;
    history_PrePrePreAngle=0;//[0]=0,history_PrePrePreAngle[1]=0;
}

const cv::Vec2f AngleSolver::predict(cv::Vec2f now_ErrAngle,double timeStamp)
{
    //if(!timeStamp)//move while lost
    //    {
    //        cout<<"armor is lost:    ";
    //        if(abs(history_ErrAngle[0])>8)//try to change here if it is too easy to stop after lost
    //        {
    //            history_ErrAngle[0]=history_ErrAngle[0]/10;//slow down
    //            cout<<"slow down: down to history_Errangle[0]="<<history_ErrAngle<<endl;
    //            return history_ErrAngle;
    //        }
    //        else//keep the speed
    //        {
    //            cv::Vec2f predict_angle;
    //            predict_angle=(history_ErrAngle+history_PrePreAngle)+
    //                    ((history_ErrAngle+history_PrePreAngle)-(history_ErrAngle+history_PrePrePreAngle))*2;

    //            history_ErrAngle=history_ErrAngle;
    //            history_PrePrePreAngle=history_PrePreAngle;
    //            history_PrePreAngle=history_PreAngle;
    //            history_PreAngle=predict_angle;
    //            cout<<"keep the speed: now_ErrAngle[0]="<<now_ErrAngle[0]<<"  predict_angle[0]="<<predict_angle<<endl;
    //            return history_ErrAngle+7*history_PreAngle+history_PrePreAngle;
    //        }
    //    }
        //else
            if(abs(now_ErrAngle[0])>=10)
        {
            history_ErrAngle=now_ErrAngle[0];
            history_PreAngle=now_ErrAngle[0];
            history_PrePreAngle=0,history_PrePreAngle=0;
            history_PrePrePreAngle=0,history_PrePrePreAngle=0;
            cout<<"Changing the target: now_ErrAngle[0]="<<now_ErrAngle[0]<<"  predict_angle[0]="<<now_ErrAngle<<endl;
            return now_ErrAngle;
        }
        else
        if(!history_ErrAngle)//waiting for new digiitals
        {
            history_ErrAngle=now_ErrAngle[0];
            history_PrePrePreAngle=history_PrePreAngle;
            history_PrePreAngle=history_PreAngle;
            history_PreAngle=now_ErrAngle[0];
            cout<<"Starting the predictor: now_ErrAngle[0]="<<now_ErrAngle[0]<<"  predict_angle[0]="<<now_ErrAngle<<endl;
            return now_ErrAngle;
        }
//        else if(abs(history_ErrAngle[0])+abs(history_PreAngle[0])+abs(history_PrePreAngle[0])<0.5)
//        {
//            cv::Vec2f predict_angle;
//            predict_angle=(now_ErrAngle+history_PrePreAngle)+
//                    ((now_ErrAngle+history_PrePreAngle)-(history_ErrAngle+history_PrePrePreAngle))*2;

//            history_ErrAngle=now_ErrAngle;
//            history_PrePrePreAngle=history_PrePreAngle;
//            history_PrePreAngle=history_PreAngle;
//            history_PreAngle=predict_angle;
//            cout<<"Moving from static: now_ErrAngle[0]="<<now_ErrAngle[0]<<"  predict_angle[0]="<<predict_angle<<endl;
//            return now_ErrAngle+7*history_PreAngle+history_PrePreAngle;//added speed to increase the speed
//        }
        else
        {
            float predictAngle;
            predictAngle=(now_ErrAngle[0]+history_PrePreAngle)+
                    ((now_ErrAngle[0]+history_PrePreAngle)-(history_ErrAngle+history_PrePrePreAngle))*2;
            history_ErrAngle=now_ErrAngle[0];
            history_PrePrePreAngle=history_PrePreAngle;
            history_PrePreAngle=history_PreAngle;
            history_PreAngle=predictAngle;
    //        history_PreAngle=predict_angle;
    //        history_ErrAngle[0]=now_ErrAngle[0];
    //        history_PrePrePreAngle[0]=history_PrePreAngle[0];
    //        history_PrePreAngle[0]=history_PreAngle[0];
    //        history_PreAngle[0]=predict_angle;
            cout<<"Normal predicting: now_ErrAngle[0]="<<now_ErrAngle[0]<<"  predict_angle[0]="<<history_PreAngle<<endl;
            return cv::Vec2f(now_ErrAngle[0]+history_PreAngle+history_PrePrePreAngle,now_ErrAngle[1]);
        }
}

std::vector<cv::Point3f> AngleSolverParam::POINT_3D_OF_ARMOR_BIG = std::vector<cv::Point3f>
{

        cv::Point3f(-105, -55, 0),	//tl
        cv::Point3f(105, -55, 0),	//tr
        cv::Point3f(105, 55, 0),	//br
        cv::Point3f(-105, 55, 0)	//bl
};
 std::vector<cv::Point3f> AngleSolverParam::POINT_3D_OF_RUNE = std::vector<cv::Point3f>
	{
		cv::Point3f(-370, -220, 0),
		cv::Point3f(0, -220, 0),
		cv::Point3f(370, -220, 0),
		cv::Point3f(-370, 0, 0),
		cv::Point3f(0, 0, 0),
		cv::Point3f(370, 0, 0),
		cv::Point3f(-370, 220, 0),
		cv::Point3f(0, 220, 0),
		cv::Point3f(370, 220, 0)
	};

std::vector<cv::Point3f> AngleSolverParam::POINT_3D_OF_ARMOR_SMALL = std::vector<cv::Point3f>
	{
		cv::Point3f(-65, -35, 0),	//tl
		cv::Point3f(65, -35, 0),	//tr
		cv::Point3f(65, 35, 0),		//br
		cv::Point3f(-65, 35, 0)		//bl
	};

AngleSolver::AngleSolver()
{
    for (int ll = 0; ll <= 3; ll++)
        target_nothing.push_back(cv::Point2f(0.0, 0.0));
}

AngleSolver::AngleSolver(const AngleSolverParam & AngleSolverParam)
{
	_params = AngleSolverParam;
	_cam_instant_matrix = _params.CAM_MATRIX.clone();
	for (int ll = 0; ll <= 3; ll++)
		target_nothing.push_back(cv::Point2f(0.0, 0.0));
}

void AngleSolver::init(const AngleSolverParam& AngleSolverParam)
{
    _params = AngleSolverParam;
    _cam_instant_matrix = _params.CAM_MATRIX.clone();
}

void AngleSolver::setTarget(const std::vector<cv::Point2f> objectPoints, int objectType)
{
    if (objectType == 0 || objectType == 1)
	{
		if (angle_solver_algorithm == 0 || angle_solver_algorithm == 2) {
				angle_solver_algorithm = 1; cout << "algorithm is reset to PNP Solution" << endl;
			}
		point_2d_of_armor = objectPoints;
        if (objectType == 0)
			enemy_type = 0;
		else
			enemy_type = 1;
		return;
	}
	if (objectType == 3 || objectType == 4)
	{
		angle_solver_algorithm = 2;
		point_2d_of_rune = objectPoints;
	}

}

void AngleSolver::setTarget(const cv::Point2f Center_of_armor, int objectPoint)
{
	if (angle_solver_algorithm == 1 || angle_solver_algorithm == 2) {
		angle_solver_algorithm = 0; cout << "algorithm is reset to One Point Solution" << endl;
	}
	centerPoint = Center_of_armor;
	if (objectPoint == 3 || objectPoint == 4)
		is_shooting_rune = 1;
	else {
		is_shooting_rune = 0;
		_rune_compensated_angle = 0;
	}
}

/*
void AngleSolver::setTarget(const std::vector<cv::Point2f> runePoints)
{
	angle_solver_algorithm = 2;
	point_2d_of_rune = runePoints;
}
*/

#ifdef DEBUG

void AngleSolver::showPoints2dOfArmor()
{
	cout << "the point 2D of armor is" << point_2d_of_armor << endl;
}


void AngleSolver::showTvec()
{
	cv::Mat tvect;
	transpose(_tVec, tvect);
	cout << "the current _tVec is:" << endl << tvect << endl;
}

void AngleSolver::showEDistance()
{
	cout << "  _euclideanDistance is  " << _euclideanDistance / 1000 << "m" << endl;
}

void AngleSolver::showcenter_of_armor()
{
	cout << "the center of armor is" << centerPoint << endl;
}

void AngleSolver::showAngle()
{
	cout << "_xErr is  " << _xErr << "  _yErr is  " << _yErr << endl;
}

int AngleSolver::showAlgorithm()
{
	return angle_solver_algorithm;
}
#endif // DEBUG


AngleSolver::AngleFlag AngleSolver::solve()
{
	if (angle_solver_algorithm == 1)
	{
		if (enemy_type == 1)
			solvePnP(_params.POINT_3D_OF_ARMOR_BIG, point_2d_of_armor, _cam_instant_matrix, _params.DISTORTION_COEFF, _rVec, _tVec, false, CV_ITERATIVE);
		if (enemy_type == 0)
			solvePnP(_params.POINT_3D_OF_ARMOR_SMALL, point_2d_of_armor, _cam_instant_matrix, _params.DISTORTION_COEFF, _rVec, _tVec, false, CV_ITERATIVE);
        //_tVec.at<double>(1, 0) -= _params.Y_DISTANCE_BETWEEN_GUN_AND_CAM;
		_xErr = atan(_tVec.at<double>(0, 0) / _tVec.at<double>(2, 0)) / 2 / CV_PI * 360;
		_yErr = atan(_tVec.at<double>(1, 0) / _tVec.at<double>(2, 0)) / 2 / CV_PI * 360;
		_euclideanDistance = sqrt(_tVec.at<double>(0, 0)*_tVec.at<double>(0, 0) + _tVec.at<double>(1, 0)*_tVec.at<double>(1, 0) + _tVec.at<double>(2, 0)* _tVec.at<double>(2, 0));
		if (_euclideanDistance >= 8500) {
			return TOO_FAR;
		}
/*
                if (_x_time_points.size < 5) _x_time_points.push_back(cv::Point2f(framecount,_xErr));
		else {
			_x_time_points[0] = _x_time_points[1];
			_x_time_points[1] = _x_time_points[2];
			_x_time_points[2] = _x_time_points[3];
			_x_time_points[3] = _x_time_points[4];
			_x_time_points[4] = cv::Point2f(framecount, _xErr);
		}
		if (_y_time_points.size < 5) _y_time_points.push_back(cv::Point2f(framecount, _yErr));
		else {
			_y_time_points[0] = _y_time_points[1];
			_y_time_points[1] = _y_time_points[2];
			_y_time_points[2] = _y_time_points[3];
			_y_time_points[3] = _y_time_points[4];
			_y_time_points[4] = cv::Point2f(framecount, _yErr);
		}
*/
		return ANGLES_AND_DISTANCE;
	}
	if (angle_solver_algorithm == 0)
	{
        double x1, x2, y1, y2, r2, k1, k2, p1, p2, y_ture;
		x1 = (centerPoint.x - _cam_instant_matrix.at<double>(0, 2)) / _cam_instant_matrix.at<double>(0, 0);
        y1 = (centerPoint.y - _cam_instant_matrix.at<double>(1, 2)) / _cam_instant_matrix.at<double>(1, 1);
        //cout<<"x:"<<x1<<endl;
//        cout<<x1<<"   "<<_cam_instant_matrix.at<double>(1, 2)<<"   "<<_cam_instant_matrix.at<double>(0, 0)<<endl;
        r2 = x1 * x1 + y1 * y1;
		k1 = _params.DISTORTION_COEFF.at<double>(0, 0);
		k2 = _params.DISTORTION_COEFF.at<double>(1, 0);
		p1 = _params.DISTORTION_COEFF.at<double>(2, 0);
		p2 = _params.DISTORTION_COEFF.at<double>(3, 0);
		x2 = x1 * (1 + k1 * r2 + k2 * r2*r2) + 2 * p1*x1*y1 + p2 * (r2 + 2 * x1*x1);
		y2 = y1 * (1 + k1 * r2 + k2 * r2*r2) + 2 * p2*x1*y1 + p1 * (r2 + 2 * y1*y1);
        _xErr = atan(x2) / 2 / CV_PI * 360;
        _yErr = atan(y2 + 0.01) / 2 / CV_PI * 360;//-0.065/0.8



		if (is_shooting_rune) _yErr -= _rune_compensated_angle;
/*
                if (_x_time_points.size < 5) _x_time_points.push_back(cv::Point2f(framecount, _xErr));
		else {
			_x_time_points[0] = _x_time_points[1];
			_x_time_points[1] = _x_time_points[2];
			_x_time_points[2] = _x_time_points[3];
			_x_time_points[3] = _x_time_points[4];
			_x_time_points[4] = cv::Point2f(framecount, _xErr);
		}
		if (_y_time_points.size < 5) _y_time_points.push_back(cv::Point2f(framecount, _yErr));
		else {
			_y_time_points[0] = _y_time_points[1];
			_y_time_points[1] = _y_time_points[2];
			_y_time_points[2] = _y_time_points[3];
			_y_time_points[3] = _y_time_points[4];
			_y_time_points[4] = cv::Point2f(framecount, _yErr);
		}
*/
		return ONLY_ANGLES;
	}
	if (angle_solver_algorithm == 2)
	{
//        cout<<"!!!!!!!!!"<<endl;
		std::vector<Point2f> runeCenters;
		std::vector<Point3f> realCenters;
		for (size_t i = 0; i < 9; i++)
		{
			if (point_2d_of_rune[i].x > 0 && point_2d_of_rune[i].y > 0)
			{
				runeCenters.push_back(point_2d_of_rune[i]);
				realCenters.push_back(_params.POINT_3D_OF_RUNE[i]);
			}
		}

		solvePnP(realCenters, runeCenters, _cam_instant_matrix, _params.DISTORTION_COEFF, _rVec, _tVec, false, CV_ITERATIVE);
		_tVec.at<double>(1, 0) -= _params.Y_DISTANCE_BETWEEN_GUN_AND_CAM;
		_xErr = atan(_tVec.at<double>(0, 0) / _tVec.at<double>(2, 0)) / 2 / CV_PI * 360;
		_yErr = atan(_tVec.at<double>(1, 0) / _tVec.at<double>(2, 0)) / 2 / CV_PI * 360;
		_euclideanDistance = sqrt(_tVec.at<double>(0, 0)*_tVec.at<double>(0, 0) + _tVec.at<double>(1, 0)*_tVec.at<double>(1, 0) + _tVec.at<double>(2, 0)* _tVec.at<double>(2, 0));
		if (_euclideanDistance >= 8500) {
			return TOO_FAR;
		}
/*
                if (_x_time_points.size < 5) _x_time_points.push_back(cv::Point2f(framecount, _xErr));
		else {
			_x_time_points[0] = _x_time_points[1];
			_x_time_points[1] = _x_time_points[2];
			_x_time_points[2] = _x_time_points[3];
			_x_time_points[3] = _x_time_points[4];
			_x_time_points[4] = cv::Point2f(framecount, _xErr);
		}
		if (_y_time_points.size < 5) _y_time_points.push_back(cv::Point2f(framecount, _yErr));
		else {
			_y_time_points[0] = _y_time_points[1];
			_y_time_points[1] = _y_time_points[2];
			_y_time_points[2] = _y_time_points[3];
			_y_time_points[3] = _y_time_points[4];
			_y_time_points[4] = cv::Point2f(framecount, _yErr);
		}
*/
		return ANGLES_AND_DISTANCE;
	}
    return ANGLE_ERROR;

	

}


void AngleSolver::setResolution(const cv::Size2i& image_resolution)
{
	image_size = image_resolution;
	//cout << _cam_instant_matrix.at<double>(0, 2) << "  " << _cam_instant_matrix.at<double>(1, 2) << endl;
    _cam_instant_matrix.at<double>(0, 2) = _params.CAM_MATRIX.at<double>(0, 2) -(1280 / 2 - image_size.width / 2);
    _cam_instant_matrix.at<double>(1, 2) = _params.CAM_MATRIX.at<double>(1, 2) -(720 / 2 - image_size.height / 2);
    _cam_instant_matrix.at<double>(0, 0) = _params.CAM_MATRIX.at<double>(0, 0) /(720/image_size.height);
        _cam_instant_matrix.at<double>(1, 1) = _params.CAM_MATRIX.at<double>(1, 1) /(720/image_size.height);
        //cout << _cam_instant_matrix.at<double>(0, 2) << "  " << _cam_instant_matrix.at<double>(1, 2) << endl;
	//cout << _params.CAM_MATRIX.at<double>(0, 2) << "  " << _params.CAM_MATRIX.at<double>(1, 2) << endl;
	//cout << endl;
}

void AngleSolver::setUserType(int usertype)
{
	user_type = usertype;
}

void AngleSolver::setEnemyType(int enemytype)
{
	enemy_type = enemytype;
}

const cv::Vec2f AngleSolver::getCompensateAngle()
{
/*
	double height;
	double g = 9.8;
	double dis = _euclideanDistance / 1000;
	if (user_type == 0) {
		if (enemy_type == 0)
			height = (_params.GUARD_HIGHT - 100) / 1000;
		else
			height = _params.GUARD_HIGHT / 1000 - 0.5;
	};
	if (user_type == 1) {
		if (enemy_type == 0)
			height = 0.4;
		else
			height = 0.1;
	};

	if (user_type == 2) {
		if (enemy_type == 0)
			height = 0.4;
		else
			height = 0.1;
	};
	if (angle_solver_algorithm == 2) height = 0.1;
	double a = dis * dis / height / height;
	double b = 2 * sqrt(dis*dis - height * height) / height * (g*(dis*dis - height * height) / height / bullet_speed / bullet_speed - 1);
	double c = pow((g*(dis*dis - height * height) / height / bullet_speed / bullet_speed - 1), 2) - 1;
	cout << "a " << a << "b " << b << "c " << c << endl;
	double sin2alpha = (-b + sqrt(b*b - 4 * a*c)) / 2 / a;
	double alpha = asin(sin2alpha) / 2;
	double compensatedAngle = (asin(height / dis) - alpha) / 2 / CV_PI * 360;
	if (angle_solver_algorithm == 2) _rune_compensated_angle = compensatedAngle;
#ifdef DEBUG
	cout << "compensated angle is:" << compensatedAngle << endl;
#endif//DEBUG
	return cv::Vec2f(_xErr, _yErr - compensatedAngle);
*/
//    return cv::Vec2f(_xErr - 0.5, _yErr - 8.2);
//    return cv::Vec2f(_xErr, _yErr);

    return cv::Vec2f(_xErr, _yErr -3.3);
}
/*
const cv::Vec2f AngleSolver::getPredictedAngle()
{
        if(_x_time_points<4) return cv::Vec2f(_xErr - 0.5, _yErr - 8.2);
        if(_y_time_points<4) return cv::Vec2f(_xErr - 0.5, _yErr - 8.2);
	cv::Vec4f line_para_x,line_para_y;
	cv::fitLine(_x_time_points, line_para_x, cv::DIST_L2, 0, 1e-2, 1e-2);
	cv::fitLine(_y_time_points, line_para_y, cv::DIST_L2, 0, 1e-2, 1e-2);

	//\BB\F1ȡ\B5\E3бʽ\B5ĵ\E3\BA\CDб\C2\CA  
	cv::Point point_0_x,point_0_y;
	point_0_x.x = line_para_x[2];
	point_0_y.y = line_para_y[3];

	double k_x = line_para_x[1] / line_para_x[0];
	double k_y = line_para_y[1] / line_para_y[0];
  
	cv::Point2f point1, point2;
	point1.x = framecount;
	point1.y = k_x * (framecount - point_0_x.x) + point_0_x.y;
	point2.x = framecount;
	point2.y = k_y * (framecount - point_0_y.x) + point_0_y.y;
	if (_xErr <= 2 &&_xErr>=-2) point1.y = 0;
	if (_yErr <= 2 &&_yErr>=-2 ) point2.y = 0;
	return cv::Vec2f(_xErr+point1.y-0.5, _yErr+point2.y-8.2);
}
*/
void AngleSolver::setBulletSpeed(int bulletSpeed)
{
    _bullet_speed = bulletSpeed;
}

const cv::Vec2f AngleSolver::getAngle()
{
    return cv::Vec2f(_xErr, _yErr);
}


double AngleSolver::getDistance()
{
	return _euclideanDistance;
}

//void AngleSolver::selectAlgorithm(const int t)
//{
//	if (t == 0 || t == 1)
//		angle_solver_algorithm = t;
//
//}

void AngleSolverParam::readFile(const int id)
{
    cv::FileStorage fsread("/home/nvidia/Robomaster/Robomaster2019/Pose/angle_solver_params.xml", cv::FileStorage::READ);
	if (!fsread.isOpened())
	{
        std::cerr << "failed to open xml" << std::endl;
		return;
	}
	fsread["Y_DISTANCE_BETWEEN_GUN_AND_CAM"] >> Y_DISTANCE_BETWEEN_GUN_AND_CAM;

#ifdef DEBUG 
	std::cout << Y_DISTANCE_BETWEEN_GUN_AND_CAM << std::endl;
#endif // DEBUG

	switch (id)
	{
    case 0:
    {
        fsread["CAMERA_MARTRIX_0"] >> CAM_MATRIX;
        fsread["DISTORTION_COEFF_0"] >> DISTORTION_COEFF;
        return;
    }
    case 1:
	{
		fsread["CAMERA_MARTRIX_1"] >> CAM_MATRIX;
		fsread["DISTORTION_COEFF_1"] >> DISTORTION_COEFF;
		return;
	}
	case 2:
	{
		fsread["CAMERA_MARTRIX_2"] >> CAM_MATRIX;
		fsread["DISTORTION_COEFF_2"] >> DISTORTION_COEFF;
		return;
	}
	case 3:
	{
		fsread["CAMERA_MARTRIX_3"] >> CAM_MATRIX;
		fsread["DISTORTION_COEFF_3"] >> DISTORTION_COEFF;
		return;
	}
	case 4:
	{
		fsread["CAMERA_MARTRIX_4"] >> CAM_MATRIX;
		fsread["DISTORTION_COEFF_4"] >> DISTORTION_COEFF;
		return;
	}
	case 5:
	{
		fsread["CAMERA_MARTRIX_OFFICIAL"] >> CAM_MATRIX;
		fsread["DISTORTION_COEFF_OFFICIAL"] >> DISTORTION_COEFF;
		return;
	}
    case 6:
    {
        fsread["CAMERA_MARTRIX_6"] >> CAM_MATRIX;
        fsread["DISTORTION_COEFF_6"] >> DISTORTION_COEFF;
        return;
    }
    case 7:
    {
        fsread["CAMERA_MARTRIX_7"] >> CAM_MATRIX;
        fsread["DISTORTION_COEFF_7"] >> DISTORTION_COEFF;
        return;
    }
    case 8:
    {
        fsread["CAMERA_MARTRIX_8"] >> CAM_MATRIX;
        fsread["DISTORTION_COEFF_8"] >> DISTORTION_COEFF;
        return;
    }
    case 9:
    {
        fsread["CAMERA_MARTRIX_9"] >> CAM_MATRIX;
        fsread["DISTORTION_COEFF_9"] >> DISTORTION_COEFF;
        return;
    }
    case 10:
    {
        fsread["CAMERA_MARTRIX_DH1"] >> CAM_MATRIX;
        fsread["DISTORTION_COEFF_DH1"] >> DISTORTION_COEFF;
        return;
    }
	default:
		std::cout << "wrong cam number given." << std::endl;
		return;
	}
	/*
	if (id == 1) {
	fsread["CAMERA_MARTRIX_1"] >> CAM_MATRIX;
	fsread["DISTORTION_COEFF_1"] >> DISTORTION_COEFF;
	return;
	}
	if (id == 2) {
	fsread["CAMERA_MARTRIX_2"] >> CAM_MATRIX;
	fsread["DISTORTION_COEFF_2"] >> DISTORTION_COEFF;
	return;
	}
	if (id == 3) {
	fsread["CAMERA_MARTRIX_3"] >> CAM_MATRIX;
	fsread["DISTORTION_COEFF_3"] >> DISTORTION_COEFF;
	return;
	}
	if (id == 4) {
	fsread["CAMERA_MARTRIX_4"] >> CAM_MATRIX;
	fsread["DISTORTION_COEFF_4"] >> DISTORTION_COEFF;
	return;
	}
	if (id == 5) {
	fsread["CAMERA_MARTRIX_OFFICIAL"] >> CAM_MATRIX;
	fsread["DISTORTION_COEFF_OFFICIAL"] >> DISTORTION_COEFF;
	return;
	}
	if (id == 0) {
	fsread["CAMERA_MARTRIX_0"] >> CAM_MATRIX;
	fsread["DISTORTION_COEFF_0"] >> DISTORTION_COEFF;
	return;
	}
	std::cout << "wrong cam number given." << std::endl;*/
}
}

