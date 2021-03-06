/*******************************************************************************************************************
Copyright 2017 Dajiang Innovations Technology Co., Ltd (DJI)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
documentation files(the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software, and 
to permit persons to whom the Software is furnished to do so, subject to the following conditions : 

The above copyright notice and this permission notice shall be included in all copies or substantial portions of
the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*******************************************************************************************************************/
//testing
#include "Predictor.hpp"
#include "opencv2/opencv.hpp"
#include <algorithm>
#include "../General/General.h"
#include <iostream>
#include <math.h>
using namespace std;
using namespace cv;
Predictor::Predictor()
{
    history_ErrAngle=0;//history_ErrAngle[0]=0,history_ErrAngle[1]=0;
    history_PreAngle=0;//[0]=0,history_PreAngle[1]=0;
    history_PrePreAngle=0;//[0]=0,history_PrePreAngle[1]=0;
    history_PrePrePreAngle=0;//[0]=0,history_PrePrePreAngle[1]=0;
}

//bool Predictor::setRecord(double value, double time){
//	/*如果链表中没有5个数据，直接添加数据*/
//    if (history_value.size() < history_size){
//        history_time.push_back(time);
//        history_value.push_back(value);
//    }
//	/*否则，删除链首数据，添加链尾数据*/
//    else {
//        history_time.push_back(time);    //插入链尾
//        history_value.push_back(value);
//        history_time.pop_front();   //删除链首（pop删除）
//        history_value.pop_front();
//    }
//}

const cv::Vec2f Predictor::predict(cv::Vec2f now_ErrAngle,double timeStamp)
{
//    if(1)
//        return now_ErrAngle;
//    if(!timeStamp)//move while lost
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
//    else
//        if(abs(now_ErrAngle[0])>=10)
//    {
//        history_ErrAngle=now_ErrAngle;
//        history_PreAngle=now_ErrAngle;
//        history_PrePreAngle[0]=0,history_PrePreAngle[1]=0;
//        history_PrePrePreAngle[0]=0,history_PrePrePreAngle[1]=0;
//        cout<<"Changing the target: now_ErrAngle[0]="<<now_ErrAngle[0]<<"  predict_angle[0]="<<now_ErrAngle<<endl;
//        return now_ErrAngle;
//    }
//    else
//    if(!history_ErrAngle[0])//waiting for new digiitals
//    {
//        history_ErrAngle=now_ErrAngle;
//        history_PrePrePreAngle=history_PrePreAngle;
//        history_PrePreAngle=history_PreAngle;
//        history_PreAngle=now_ErrAngle;
//        cout<<"Starting the predictor: now_ErrAngle[0]="<<now_ErrAngle[0]<<"  predict_angle[0]="<<now_ErrAngle<<endl;
//        return now_ErrAngle;
//    }
//    else if(abs(history_ErrAngle[0])+abs(history_PreAngle[0])+abs(history_PrePreAngle[0])<0.5)
//    {
//        cv::Vec2f predict_angle;
//        predict_angle=(now_ErrAngle+history_PrePreAngle)+
//                ((now_ErrAngle+history_PrePreAngle)-(history_ErrAngle+history_PrePrePreAngle))*2;

//        history_ErrAngle=now_ErrAngle;
//        history_PrePrePreAngle=history_PrePreAngle;
//        history_PrePreAngle=history_PreAngle;
//        history_PreAngle=predict_angle;
//        cout<<"Moving from static: now_ErrAngle[0]="<<now_ErrAngle[0]<<"  predict_angle[0]="<<predict_angle<<endl;
//        return now_ErrAngle+7*history_PreAngle+history_PrePreAngle;//added speed to increase the speed
//    }
//    else
    {

        history_PreAngle=0;
        //now_ErrAngle[0]+history_PrePreAngle)+((now_ErrAngle[0]+history_PrePreAngle)-(history_ErrAngle+history_PrePrePreAngle))*2;
        cout<<"ok"<<endl;
        history_ErrAngle=now_ErrAngle[0];
        history_PrePrePreAngle=history_PrePreAngle;
        history_PrePreAngle=history_PreAngle;
//        history_PreAngle=predict_angle;
//        history_ErrAngle[0]=now_ErrAngle[0];
//        history_PrePrePreAngle[0]=history_PrePreAngle[0];
//        history_PrePreAngle[0]=history_PreAngle[0];
//        history_PreAngle[0]=predict_angle;
        cout<<"Normal predicting: now_ErrAngle[0]="<<now_ErrAngle[0]<<"  predict_angle[0]="<<history_PreAngle<<endl;
        return cv::Vec2f(now_ErrAngle[0]+history_PreAngle+history_PrePrePreAngle,now_ErrAngle[1]);
    }
//    list<double>::const_iterator it_in = history_time.begin();
//    double latest_value = history_value.back();
//    if(abs(latest_value) < 5.0 || history_value.size() < history_size)
//        return latest_value;
//    if(history_time.back() - *it_in > 150.0)
//        return latest_value;
//    list<double>::const_iterator it_out = history_value.begin();
//    list<double>::const_iterator prev_out = it_out;  //prev_out 前一个输出值
//    double max_o = -500000, min_o = 500000;
//    for(list<double>::const_iterator it = it_out, it_i = it_in; it != history_value.end(); ++it, ++it_i){
//        if(max_o < *it)
//            max_o = *it;
//        if(min_o > *it)
//            min_o = *it;
//        if(abs(*it - *prev_out) > 5.0)  //判断相邻值是否大于5
//		{
//            return latest_value;
//        }
//        prev_out = it;
//        //printf("(%2f,%2f) ", *it, *it_i);
//    }
//   // printf("\n");
//    if (max_o - min_o < 3.0)      // angle gap must lager than 3 degree
//        return latest_value;

//    Mat A(history_size,3,CV_64F);  //创建一个矩阵 （行数，列数，存储类型）
//    Mat b(history_size,1,CV_64F);
//    double * b_data = (double *) b.data;
//    double * A_data = (double *) A.data;

//    for (; it_in != history_time.end(); ++A_data, ++b_data, ++it_in, ++it_out){
//        *A_data = (*it_in-time) * (*it_in-time);    //时间差的平方
//        *(++A_data) = (*it_in-time);  // 时间差
//        *(++A_data) = 1;   //上述三行是二次方程
//        *b_data = *it_out;  //输出是位置，这就是时间和位置的二次方程
//    }

//    // A*w = b  =>  w = (A_t * A).inverse * b
//    Mat A_t = A.t();   // t()函数  求转置矩阵
//    Mat w = (A_t*A).inv()*A_t * b;
//    Mat q = (Mat_<double>(1,3) << 0, 0, 1);
//    Mat ret = q*w;

//    double predict_angel = ret.at<double>(0);  //at函数：访问mat类中的元素
//    const double max_gap = 3.0;     //这个3度定义是什么意思，为什么相差3度之内，就采用预测值，超出就采用之前的值＋3
//    if(predict_angel - latest_value > max_gap)
//        predict_angel = latest_value + max_gap;
//    else if(predict_angel - latest_value < -max_gap)
//        predict_angel = latest_value - max_gap;
//    return predict_angel;

//}

//double Predictor::testpredict(double time){
//    list<double>::const_iterator it_in = history_time.begin();
//    double latest_value = history_value.back();
//    if(abs(latest_value) < 5.0 || history_value.size() < history_size)
//        return latest_value;
//    if(history_time.back() - *it_in > 150.0)
//        return latest_value;
//    list<double>::const_iterator it_out = history_value.begin();
//    list<double>::const_iterator prev_out = it_out;  //prev_out 前一个输出值
//    double max_o = -500000, min_o = 500000;
//    for(list<double>::const_iterator it = it_out, it_i = it_in; it != history_value.end(); ++it, ++it_i){
//        if(max_o < *it)
//            max_o = *it;
//        if(min_o > *it)
//            min_o = *it;
//        if(abs(*it - *prev_out) > 5.0)  //判断相邻值是否大于5
//        {
//            return latest_value;
//        }
//        prev_out = it;
//        //printf("(%2f,%2f) ", *it, *it_i);
//    }
//   // printf("\n");
//    if (max_o - min_o < 3.0)      // angle gap must lager than 3 degree
//        return latest_value;

//    Mat A(history_size,3,CV_64F);  //创建一个矩阵 （行数，列数，存储类型）
//    Mat b(history_size,1,CV_64F);
//    double * b_data = (double *) b.data;
//    double * A_data = (double *) A.data;

//    for (; it_in != history_time.end(); ++A_data, ++b_data, ++it_in, ++it_out){
//        *A_data = (*it_in-time) * (*it_in-time);    //时间差的平方
//        *(++A_data) = (*it_in-time);  // 时间差
//        *(++A_data) = 1;   //上述三行是二次方程
//        *b_data = *it_out;  //输出是位置，这就是时间和位置的二次方程
//    }

//    // A*w = b  =>  w = (A_t * A).inverse * b
//    Mat A_t = A.t();   // t()函数  求转置矩阵
//    Mat w = (A_t*A).inv()*A_t * b;
//    Mat q = (Mat_<double>(1,3) << 0, 0, 1);
//    Mat ret = q*w;
//    std::cout<<"W="<<w<<std::endl;

//    double predict_angel = ret.at<double>(0);  //at函数：访问mat类中的元素
//    const double max_gap = 3.0;     //这个3度定义是什么意思，为什么相差3度之内，就采用预测值，超出就采用之前的值＋3
//    if(predict_angel - latest_value > max_gap)
//        predict_angel = latest_value + max_gap;
//    else if(predict_angel - latest_value < -max_gap)
//        predict_angel = latest_value - max_gap;
//    return predict_angel;
}
