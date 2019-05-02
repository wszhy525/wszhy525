#include "Windmill.h"
#define DEBUG







Windmill::Windmill()
{
    MAXRANGE = 2500;  //设置最大边框检测范围
    MINRANGE = 500; //设置最小边框检测范围
    _DIFFER_DISTANCE = 99999999; //设置最小变动范围
    MAXMAXRANGE = 6000;  //整个扇叶的外包矩形面积
    _isfund=false; //初始化是否寻找到打击中心标志量为false
//    _isForeCast = false;//初始化能否预测打击为否
    Lead=40;                //set the lead
    Offset=0.4;

}

Windmill::~Windmill()
{

}
void Windmill:: setClockwise()
{
    _isclockwise = 0;      //shunshizheng  clockwise

}
void Windmill:: setAntiClockwise()
{
    _isclockwise = 1;      //nishizheng  clockwise

}

double Windmill:: Max(double a, double b, double c)
{
    double x = a;
    if (a > b)
        x = a;
    else
        x = b;
    if (x > c)
        x = x;
    else
        x = c;
    return x;
}
//void Windmill:: setIsfoundTrue()
//{
//    _isfound=true;
//}
//void Windmill:: setIsfoundFalse()
//{
//    _isfound=false;
//}
double Windmill::Min(double a, double b, double c)
{
    double x = a;
    if (a < b)
        x = a;
    else
        x = b;
    if (x < c)
        x = x;
    else
        x = c;
    return x;
}
void Windmill::calculateTheHSV(double R, double G, double B)
{
    double max = Max(R, G, B);
    double min = Min(R, G, B);
    V = Max(R, G, B);
    S = (max - min) / max;


    if (R == max) H = (G - B) / (max - min) * 60;
    if (G == max) H = 120 + (B - R) / (max - min) * 60;
    if (B == max) H = 240 + (R - G) / (max - min) * 60;
    if (H < 0) H = H + 360;
}

Mat Windmill::loadImg(Mat frame_)
{
    _srcImg = frame_;
//    if(_isfund)
//    {
//        Rect imgBound = Rect(cv_forecastCenter::Point(0, 0), _srcImg.size());
//        _roi = boundRect.back() & imgBound;
//        _srcImg = _srcImg(_roi).clone();
//      _isfund=false;
//        return _srcImg;
//    }
//    else
//   {
//        return _srcImg;
//        _isfund=false;
//    }
    return _srcImg;
}

bool Windmill::getDistance(const Vec3b& color)
{
    int judgement = 0;
    calculateTheHSV(color[2], color[1], color[0]);
//	双重检测，首先去掉过于亮或者暗的区域，再用HSV判断来精确定位
//    (color[2] < _largeThreshold)&&(color[1] < _largeThreshold)&&(color[0] < _largeThreshold)&&
//    if ((color[2] > _smallThreshold)&&(color[2] > _smallThreshold)&&(0<H<100||200<H<360)
//            &&(S > 0.3)&&(V > 16))  //去掉全是白光的灯光噪声   //V 的范围  //S的范围  //H的范围 //R必须大于100才是需要的地方
    if(((color[2]-color[0]) > 70||(color[2]-color[0]) < -70)&&(0<H<100||200<H<360)&&(S > 0.3)&&(V > 16))

    {
        return 1;
    }

    else
        return 0;
}
float Windmill::distance(Point2f Pointa, Point2f Pointb)
{
    float result = 0;
    result = hypotf(Pointa.x - Pointb.x, Pointa.y - Pointb.y);
    return result;

}
void Windmill::Detect()
{
//    float newLongth = 0;
    cv::Point2f points4[4];
    int count=0;  //作为计数器，统计detect出的装甲板中心，只有为1时才做返回
    RotatedRect rotated_rect;
    Rect bounding_rect;
    Mat _processedImg(_srcImg.size(), CV_8U, Scalar(0)); //初始化一张和原图等大的二值化图来装载红色结果区域。
    Mat_ <cv::Vec3b> ::const_iterator it = _srcImg.begin<Vec3b>();
    Mat_ <cv::Vec3b> ::const_iterator itend = _srcImg.end<Vec3b>();
    Mat_ <uchar>::iterator itout = _processedImg.begin<uchar>();
    for (it; it != itend; ++it, ++itout)
    {
        if (getDistance(*it))
        {
            *itout = 255;
        }
        else
        {
            *itout = 0;
        }
    }
//#ifdef DEBUG
//    imshow("hsv imshow", _processedImg);
//    waitKey(1);
//#endif

    Mat element = getStructuringElement(MORPH_RECT,Size(5,5));
    dilate(_processedImg,_processedImg,element);//通过膨胀使得轮廓更加清晰


//#ifdef DEBUG
//    imshow("dilate imshow", _processedImg);
//    waitKey(1);
//#endif

//    //将要打击的装甲板的颜色要比已经打击的装甲板更亮，所以经过滤波之后已经打击过的装甲板会有一部分成分的残缺，这不用管他，可以还会对后面的识别有所帮助。
    findContours(_processedImg, contours, RETR_TREE, CV_CHAIN_APPROX_NONE);
    vector<vector<Point> >::iterator itc = contours.begin();
    while (itc!=contours.end())
    {
        if(contourArea(*itc) > MAXMAXRANGE) //删除大扇叶里的三个小轮廓
        {
            itc++;
            while((itc!=contours.end())&&(contourArea(*itc) < MAXRANGE))
            {
                itc = contours.erase(itc);
            }
        }
        else if(contourArea(*itc) < MINRANGE) //删除杂质轮廓
        {
            itc = contours.erase(itc);
        }
            else
            {
                itc++;
            }
    }
    Mat _processedImg1(_srcImg.size(), CV_8U, Scalar(0));



//    drawContours(_processedImg1, contours, -1, Scalar(255), 1);

//#ifdef DEBUG
//    imshow("contours imshow", _processedImg1);
//    waitKey(1);
//#endif


    for(int i = 0; i<contours.size(); ++i)
    {


//    cout<<contourArea(contours[i])<<endl;
    if (contourArea(contours[i]) < MINRANGE)
        {
        continue;
        }


     if ((contourArea(contours[i]) > MINRANGE)&& (contourArea(contours[i]) < MAXRANGE))
        {

            cv::Point2f points4[4];
            RotatedRect rotated_rect = minAreaRect(contours[i]);
//            Rect bounding_rect=boundingRect(contours[i]);

            rotated_rect.points(points4);


            float y = rotated_rect.size.height / rotated_rect.size.width;
            if(rotated_rect.size.height>rotated_rect.size.width)
            {
                fanwidth=rotated_rect.size.width;
            }
            else
            {
                fanwidth=rotated_rect.size.height;
            }
            int z = -1;
            if ((y < 2.1)&&(y > 1.4))  //利用长宽比例筛选出装甲板
                z = 1;
            if ((y < 0.8)&&(y > 0.4))
                z = 1;
            if (z == 1)
            {
                count++;
                _fanCenter = rotated_rect.center;
//                float fan_x =  floor(rotated_rect.center.x/10)*10;
//                float fan_y =  floor(rotated_rect.center.y/10)*10;


//                _fanCenter.x = fan_x;
//                _fanCenter.y = fan_y;

//                for (int i = 0; i < 4; i++)
//                    // 注意Scala中存储顺序 BGRnewLongth
//                    line(_srcImg, points4[i], points4[(i + 1) % 4], Scalar(0, 255, 0), 5);
               /* circle(_srcImg, _fanCenter, 5, Scalar(0, 0, 255));*/

                 vector<Point2f> vector_centerPoint_temporary;//创建一个临时vector来储存不属于该扇叶运动轨迹的中心
                if (vector_centerPoint.empty())                                                         //如果后一个点在前一个点附近，则压入栈中。
                    vector_centerPoint.push_back(_fanCenter);
                else
                {
                    if (vector_centerPoint.size() == 1)
                    {
                        if (distance(vector_centerPoint[vector_centerPoint.size() - 1], _fanCenter) < _DIFFER_DISTANCE)   //待测点在倒数第一个点附近
                            vector_centerPoint.push_back(_fanCenter);
                    }
                    else
                    {
                        if (distance(vector_centerPoint[vector_centerPoint.size() - 1], _fanCenter) < _DIFFER_DISTANCE&&
                            distance(vector_centerPoint[vector_centerPoint.size() - 2], _fanCenter) < _DIFFER_DISTANCE)    //待测点在第一二个点附近
                            vector_centerPoint.push_back(_fanCenter);
                        else
                        {
                            vector_centerPoint_temporary.push_back(_fanCenter);
                            if (vector_centerPoint_temporary.size() > 3)
                            {
                                if (distance(vector_centerPoint_temporary[vector_centerPoint_temporary.size() - 1], _fanCenter) < _DIFFER_DISTANCE&&   //判断是否下一个扇叶亮起
                                    distance(vector_centerPoint_temporary[vector_centerPoint_temporary.size() - 2], _fanCenter) < _DIFFER_DISTANCE&&
                                    distance(vector_centerPoint_temporary[vector_centerPoint_temporary.size() - 3], _fanCenter) < _DIFFER_DISTANCE)
                                {
                                    vector_centerPoint.clear();
                                    vector_centerPoint.assign(vector_centerPoint_temporary.end()-3, vector_centerPoint_temporary.end());
                                    //通过判据，已经判得下一打击目标亮起，通过数值赋值，减少三帧图片时间
                                    vector_centerPoint_temporary.clear();

                                }
                            }
                        }
                    }
                }

//               close_circleCenter();
//               doCaculate();

              }
           }
    }


        if(count==1)
        {
            vector<float> length;
           _isfund = true;
          rotated_rect.points(points4);
          for (int i = 0; i < 4; i++)// 注意Scala中存储顺序 BGR
              line(_srcImg, points4[i], points4[(i + 1) % 4], Scalar(0, 255, 0), 5);

          circle(_srcImg, _fanCenter, 5, Scalar(0, 0, 255));
          /* 通过找到包含装甲板中心大contours中距离装甲板中心最远的点*/
          for (int i = 0; i<contours.size(); i++)
          {
              if(pointPolygonTest(contours[i],_fanCenter,false)==1)
              {
                  if(contourArea(contours[i])>MAXRANGE&&contourArea(contours[i])<MAXMAXRANGE) //在合适轮廓上寻找与合适方向向量的点
                  {
//                      float temporaryLongth=0;
                      for (int j = 0; j<contours[i].size(); j++)
                      {
                          length.push_back(distance(contours[i][j],_fanCenter));
//                          temporaryLongth = distance(contours[i][j],_fanCenter);


//                          if(temporaryLongth > newLongth)
//                          {
//                              newLongth=temporaryLongth;
//                              newDirection=contours[i][j];
////                              newDirection.x=contours[i][j].x;
////                              newDirection.y=contours[i][j].y;
//                          }
                              //cout << contours[i][j].x << "   " << contours[i][j].y << endl;
                      }
//                      cout<<"zhy输出"<<newLongth<<endl;

                      int MaxNum[10]={0};
                      int NUM = contours[i].size();
                      int n=0;
                      float MaxTemp = 0;
                      int Record=0;
                      while(n<8)
                      {
                          MaxTemp=length[0];
                          for( int t = 0;t < NUM;t++)
                          {
                              if(length[t]>MaxTemp)
                              {
                                  Record=t;
                                  MaxTemp=length[t];
                              }
                          }
                          length[Record]=0;
                          MaxNum[n]=Record;
                          n++;
                      }
                      newDirection = (contours[i][MaxNum[0]] + contours[i][MaxNum[1]] +contours[i][MaxNum[2]] +contours[i][MaxNum[3]]+
                              contours[i][MaxNum[4]] + contours[i][MaxNum[5]] +contours[i][MaxNum[6]] +contours[i][MaxNum[7]])/8; //求与其距离最远的三个点取平均，解决抖动
                  }
              }
          }

          returnNextPoint();
        }
         else if(count!=1)
        {
             _isfund = false;
        }
        imshow("读取视频", _srcImg);

}


void Windmill::Refresh()
{
    vector_centerPoint.clear();
}

void Windmill::doCaculate()
{
    if (vector_centerPoint.size() > 8)
    {
        _center = calculateWindmillInfo(vector_centerPoint[vector_centerPoint.size() - 9],
                vector_centerPoint[vector_centerPoint.size() - 5],
                vector_centerPoint[vector_centerPoint.size() - 1]);
        //Look for circumscribed circles between two images to improve accuracy
         circle(_srcImg, _center, 5, Scalar(0, 0, 255));
    }
}

Point2f Windmill::calculateWindmillInfo(Point2f pt1, Point2f pt2, Point2f pt3)
{
    float A1, A2, B1, B2, C1, C2, temp;
    A1 = pt1.x - pt2.x;
    B1 = pt1.y - pt2.y;
    C1 = (pow(pt1.x, 2) - pow(pt2.x, 2) + pow(pt1.y, 2) - pow(pt2.y, 2)) / 2;
    A2 = pt3.x - pt2.x;
    B2 = pt3.y - pt2.y;
    C2 = (pow(pt3.x, 2) - pow(pt2.x, 2) + pow(pt3.y, 2) - pow(pt2.y, 2)) / 2;
    //为了方便编写程序，令temp = A1*B2 - A2*B1
    temp = A1 * B2 - A2 * B1;
    //定义一个圆的数据的结构体对象CD
    Point2f CD;
    //判断三点是否共线
    if (temp == 0)
    {
        //共线则将第一个点pt1作为圆心
        CD.x = pt1.x;
        CD.y = pt1.y;
    }
    else
    {
        //不共线则求出圆心：
        CD.x = (C1*B2 - C2 * B1) / temp;
        CD.y = (C2*A1 - C1 * A2) / temp;
    }
    cout << "圆心中心为" << CD << endl;
    _center = CD;
    double ans = (pt2.x - pt1.x)*(pt3.y - pt1.y) - (pt2.y - pt1.y)*(pt3.x - pt1.x);//表示向量AB与AC的叉积的结果
    //    if (ans < 0)
    //    {
    //        _isclockwise = 1;
    //        cout << "逆时针" << endl;
    //    }
    //    if (ans > 0)
    //    {
    //        _isclockwise = 0;
    //        cout << "顺时针" << endl;
    //    }
    //    if (ans == 0)

    //        cout << "共线" << endl;

    return _center;
}
 Point2f Windmill:: forecastNextPoint(Point2f pt1, Point2f pt2)
 {
    Point2f fanCenter=pt1;
    Point2f center=pt2;
    //if(isAntiClockWise==1) ->nishizhen
     //if(isAntiClockWise==0) ->shunshizhen
    longth=sqrt(pow(fanCenter.x-center.x,2)+pow(fanCenter.y-center.y,2));

    direction.x=fanCenter.x-center.x;
    direction.y=fanCenter.y-center.y;
    //float tan=direction.y/direction.x;
    float sin=direction.y/longth;
    float cos=direction.x/longth;
    float processy=Offset*fanwidth*sin;
    float processx=Offset*fanwidth*cos;
    Point2f forecastPoint;
    forecastPoint.x=fanCenter.x-processx;
    forecastPoint.y=fanCenter.y-processy;
    Point2f forecastPointPlus;
    Point2f forecastDirection;
    if(_isclockwise==1)             //nishizhen anticlockwise
    {
      forecastDirection.x=(forecastPoint.x-fanCenter.x)*0-(forecastPoint.y-fanCenter.y)*1+fanCenter.x;
      forecastDirection.y=(forecastPoint.x-fanCenter.x)*1-(forecastPoint.y-fanCenter.y)*0+fanCenter.y;
      float longth1=sqrt(pow(forecastDirection.x-fanCenter.x,2)+pow(forecastDirection.y-fanCenter.y,2));
      float processy1=(forecastDirection.y-fanCenter.y)*Lead/longth1;
      float processx1=(forecastDirection.x-fanCenter.x)*Lead/longth1;
//      float processy1=lead*sin1;
//      float processx1=lead*cos1;
      forecastPointPlus.x=forecastPoint.x+processx1;
      forecastPointPlus.y=forecastPoint.y+processy1;

    }
    if(_isclockwise==0)
    {
        forecastDirection.x=(forecastPoint.x-fanCenter.x)*0-(forecastPoint.y-fanCenter.y)*(-1)+fanCenter.x;
        forecastDirection.y=(forecastPoint.x-fanCenter.x)*(-1)-(forecastPoint.y-fanCenter.y)*0+fanCenter.y;
        float longth1=sqrt(pow(forecastDirection.x-fanCenter.x,2)+pow(forecastDirection.y-fanCenter.y,2));
        float processy1=(forecastDirection.y-fanCenter.y)*Lead/longth1;
        float processx1=(forecastDirection.x-fanCenter.x)*Lead/longth1;
//        float processy1=lead*sin1;
//        float processx1=lead*cos1;
        forecastPointPlus.x=forecastPoint.x+processx1;
        forecastPointPlus.y=forecastPoint.y+processy1;

    }
    return forecastPointPlus;
 }

 Point2f Windmill::returnNextPoint()
 {
     if(_isfund) //如果找到装甲板中心点，便开始预测
     {


        _forecastCenter=forecastNextPoint(_fanCenter,newDirection);
        circle(_srcImg, newDirection, 5, Scalar(255, 255,0));
        // _forecastCenter=secondWayToCalculateForecastPoint(30);
         circle(_srcImg, _forecastCenter, 5, Scalar(0, 255,0));
        cout<<"the center of forecast is"<<_forecastCenter<<endl;
        return _forecastCenter;  //如果可以预测便返回这个Point2f类型的预测点
     }

 }


// Point2f Windmill:: secondWayToCalculateForecastPoint(float input)
// {
//     longth=sqrt(pow(_fanCenter.x-_center.x,2)+pow(_fanCenter.y-_center.y,2));
//     Point2f forecastDirection;
//     Point2f forecastPointPlus;
//     Point2f forecastPointOnCircle;
//     if(_isclockwise==1)             //nishizhen anticlockwise
//     {
//       forecastDirection.x=(_center.x-_fanCenter.x)*0-(_center.y-_fanCenter.y)*1+_fanCenter.x;
//       forecastDirection.y=(_center.x-_fanCenter.x)*1-(_center.y-_fanCenter.y)*0+_fanCenter.y;
//       float longth1=sqrt(pow(_center.x-_fanCenter.x,2)+pow(_center.y-_fanCenter.y,2));
//       float processy1=(forecastDirection.y-_fanCenter.y)*input/longth1;
//       float processx1=(forecastDirection.x-_fanCenter.x)*input/longth1;
//       forecastPointPlus.x=_fanCenter.x+processx1;
//       forecastPointPlus.y=_fanCenter.y+processy1;
//       float longth2=sqrt(pow(_center.x-forecastPointPlus.x,2)+pow(_center.y-forecastPointPlus.y,2));
//       forecastPointOnCircle.x=(longth1/longth2)*(forecastPointPlus.x-_center.x)+_center.x;
//       forecastPointOnCircle.y=(longth1/longth2)*(forecastPointPlus.y-_center.y)+_center.y;

//     }
//     if(_isclockwise==0)
//     {
//         forecastDirection.x=(_center.x-_fanCenter.x)*0-(_center.y-_fanCenter.y)*(-1)+_fanCenter.x;
//         forecastDirection.y=(_center.x-_fanCenter.x)*(-1)-(_center.y-_fanCenter.y)*0+_fanCenter.y;
//         float longth1=sqrt(pow(_center.x-_fanCenter.x,2)+pow(_center.y-_fanCenter.y,2));
//         float processy1=(forecastDirection.y-_fanCenter.y)*input/longth1;
//         float processx1=(forecastDirection.x-_fanCenter.x)*input/longth1;
//         float longth2=sqrt(pow(_center.x-forecastPointPlus.x,2)+pow(_center.y-forecastPointPlus.y,2));
//         forecastPointPlus.x=_fanCenter.x+processx1;
//         forecastPointPlus.y=_fanCenter.y+processy1;
//         forecastPointOnCircle.x=(longth1/longth2)*(forecastPointPlus.x-_center.x)+forecastPointPlus.x;
//         forecastPointOnCircle.y=(longth1/longth2)*(forecastPointPlus.y-_center.y)+forecastPointPlus.y;

//     }
//     return forecastPointPlus;

// }



// bool Windmill::isForeCast()
//{
//    float x = _srcImg.cols;
//    float y = _srcImg.rows;
//    Point2f t_frameCenter = Point2f(x,y);//声明画面中心
//    if(vector_centerPoint.size() < 3) //存取识别点数过少，无法预测
//        return false;
//    if(distance(vector_centerPoint[vector_centerPoint.size() - 1],t_frameCenter) < _DIFFER_DISTANCE
//            &&distance(vector_centerPoint[vector_centerPoint.size() - 2],t_frameCenter) < _DIFFER_DISTANCE
//            &&distance(vector_centerPoint[vector_centerPoint.size() - 3],t_frameCenter) < _DIFFER_DISTANCE) //捕捉点在视频中心附近，证明可跟随
//        return true;
//}


// void Windmill::close_circleCenter()
// {
//     bool adsdad = Windmill::isForeCast();
//     float x = _srcImg.cols;
//     float y = _srcImg.rows;
//     Point2f t_frameCenter = Point2f(x,y);//声明画面中心

//      Point2f c_final;
//    if(adsdad)
//    {
////        cout<<"正确在==================================================";
//        Point2f p0 = 3*t_frameCenter - vector_centerPoint[vector_centerPoint.size() - 3] - vector_centerPoint[vector_centerPoint.size() - 2];//在本帧图片找到提前两帧的识别点
//        Point2f p1 = 2*t_frameCenter - vector_centerPoint[vector_centerPoint.size() - 3] ;//在本帧图片中找到前一帧的识别点
//        Point2f p2 = t_frameCenter ;
//        Point2f p3 = vector_centerPoint[vector_centerPoint.size() - 1];
//        Point2f c1 = calculateWindmillInfo(p0,p1,p3);
//        Point2f c2 = calculateWindmillInfo(p1,p2,p3);
//        c_final = (c1+c2)/100; //用两组圆上的圆心大概确定朝向圆心


//    }
//    _center = c_final;
//    circle(_srcImg, _center, 6, Scalar(0, 0, 255));
////    return (c_final - t_frameCenter);
// }

 bool Windmill::getIsfund()
 {
     return _isfund;
 }



















