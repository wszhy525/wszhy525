#pragma once

#include<iostream>
#include<fstream>
#include<stdio.h>
#include<stdint.h>
#include<mutex>
#include<chrono>
#include<thread>
#include<deque>
#include<opencv2/opencv.hpp>

using namespace cv;

namespace rm {

class Strategies
{
public:
    Strategies(){};
    //Strategies(size_t dequeSize);
    ~Strategies(){}

    std::deque<Vec2f> _angle_deque;
    std::deque<Vec2f> _angle_deviation;
    size_t _size;


    //parameters
    float _average_limit[2];
    float _varience_limit[2];

public:
    void initPara();
    bool ifHitTarget();
    void pushBackAngle(Vec2f targetAngle);
    void popFrontAngle();

};





}
