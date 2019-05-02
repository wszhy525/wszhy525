#include<iostream>
#include<fstream>
#include<stdio.h>
#include<stdint.h>
#include<chrono>
#include<deque>
#include<opencv2/opencv.hpp>
#include<cmath>
#include"Strategies.h"
using namespace std;
using namespace cv;

namespace rm {

void Strategies::initPara()
{
    _size = 6;
    _average_limit[0]=1.0;
    _average_limit[1]=1.0;
    _varience_limit[0]=2.0;
    _varience_limit[1]=2.0;

}

bool Strategies::ifHitTarget()
{

    if(_angle_deque.size()<_size)
        return false;

    if(_size < 2)
    {
        //std::cout<<"deque size must not < 2"<<std::endl;
        return false;
    }
    float averageX = 0,averageY = 0;
    float varienceX = 0,varienceY = 0;
    for(size_t i = 0; i < _size - 1; ++i)
    {
        averageX += _angle_deviation[i][0];
        averageY += _angle_deviation[i][1];

    }
    averageX /= (_size - 1);
    averageY /= (_size - 1);

//    std::cout<<averageX<<std::endl;
//    std::cout<<averageY<<std::endl;
    if(std::abs(averageX)>_average_limit[0] || std::abs(averageY)>_average_limit[1])
    {
        return false;
    }

    for(size_t i = 0; i < _size - 1; ++i)
    {
        varienceX += pow(averageX - _angle_deviation[i][0],2);
        varienceY += pow(averageY - _angle_deviation[i][1],2);
    }
    varienceX /= (_size - 1);
    varienceY /= (_size - 1);
//
//    std::cout<<varienceX<<std::endl;
//    std::cout<<varienceY<<std::endl;
    if(varienceX > _varience_limit[0] || varienceY> _varience_limit[1])
    {
        return false;
    }
    else
    {
        return true;
    }

}

void Strategies::pushBackAngle(Vec2f targetAngle)
{
//    auto t1 = chrono::high_resolution_clock::now();
    if(_angle_deviation.size() == _size - 1)
    {
        _angle_deviation.pop_front();
    }

    if(_angle_deque.size())
        _angle_deviation.push_back(Vec2f(_angle_deque.back()[0] - targetAngle[0],_angle_deque.back()[1] - targetAngle[1]));

    if(_angle_deque.size() == _size)
    {
        _angle_deque.pop_front();
    }

    _angle_deque.push_back(targetAngle);
//    auto t2 = chrono::high_resolution_clock::now();
//    std::cout << "Capture period: " << (static_cast<chrono::duration<double, std::milli>>(t2 - t1)).count() << " ms" << std::endl;

}

void Strategies::popFrontAngle()
{
    if(_angle_deque.size()>0)
        _angle_deque.pop_front();
    if(_angle_deviation.size()>0)
        _angle_deviation.pop_front();
}

}


