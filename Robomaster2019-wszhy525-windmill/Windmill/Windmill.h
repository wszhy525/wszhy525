#pragma once
#include<opencv2/opencv.hpp>
#include<queue>

using namespace cv;
using namespace std;

class Windmill
{
public:

    /*
    * @brief constructor and destructor
    * @para it should include all of your parameters which will be changed in the match.
    */
    Windmill();
    ~Windmill();
    /*
    * @brief load image
    * @para frame_: the input frame
    */
    Mat loadImg(Mat frame_);

    /*
    * @brief detect the frame and return the center of the target.
    * @para
    */

    void Detect();

    /*
    * @brief only the pixel point with the color inside the threshold will be detected
    * @para color: the thresholds of red bule and green
    */

    bool getDistance(const Vec3b& color);

    /*
    * @brief when the detect fanCenter is too far(the distance between two points is above 75)
    * clear the vector and start detecting again to aviod disturbs and the error of the movements of camera
    * @para
    */

    void Refresh();

    /*
    * @brief set the attribute of direction which is clockwise or anticlockwise
    * @para
    */

    void setDirection(bool isclockwise_) { _isclockwise = isclockwise_; }

    /*
    * @brief set the attribute of direction which is clockwise or anticlockwise
    * @para
    */

    bool getDirection() { return _isclockwise; }

    /*
    * @brief if the parameter is suitable do the caculation
    * @para
    */
    void doCaculate();

    /*
    * @brief set the center of the whole windmill
    * @para center_ the center of the whole windmill
    */
    void setCenter(Point2f center_) { _center = center_; }

    /*
    * @brief caculate the distance between two points which were detected
    * @para Pointa onr point
    * @para Pointb the other point
    */

    float distance(Point2f Pointa, Point2f Pointb);

    /*
    * @brief formal parameter
    * @para
    */


    /*
    * @brief use to caculate and save the direction of retation and the center of windmill
    * @para pt1,pt2,pt3: these points can be given through Detect function
    */

    Point2f calculateWindmillInfo(Point2f pt1, Point2f pt2, Point2f pt3);
    /*
    * @brief calculate the max munber of the three numbers
    * @para a,b,c: three numbers of the double classification
    */
    double Max(double a, double b, double c);
    /*
    * @brief calculate the min munber of the three numbers
    * @para a,b,c: three numbers of the double classification
    */
    double Min(double a, double b, double c);
    /*
    * @brief calculate the result of HSV value
    * @para a,b,c are the BRG value of each pics
    */
    void calculateTheHSV(double a, double b, double c);

    /*

    * @brief ��Ϊ�������256ɫ��ͼƬ��ͨ���������������ɫ�����������Ժ�����
    * @para inputImg_: image input
    * @para outputImg_: image output
    * @para div_:
    */
    void colorReduce(Mat & inputImg_, Mat &outputImg_, int div_);

    /*
    * @brief get�������������Ƿ��ҵ����Ŀ��
    * @para _isfound:��־�Ƿ��ҵ��˴����Ŀ��
    *
    */


        Point2f getCenter()
        {   _noFound.x=0.0;
            _noFound.y=0.0;
            if(q.size())
                return q.back();
            else
                return _noFound;

        }

        /*
        * @brief ���ڸ��������Ƿ���
        * @para _srcImg:��ҪԭʼͼƬ�ߴ�����
        *       point:��������Ԥ���
        */






        /*
         * @brief ���õ�ǰ���Լ���������������Ԥ����һ������
         * @para pt1:��ǰ������
         *       pt2��ָ��Բ�ĵķ��������
        */

        Point2f forecastNextPoint(Point2f pt1, Point2f pt2);

        /*
        * @brief ����Ԥ���
        * @para _isfund ��־�Ƿ��ҵ����ĵ㣬�Ƿ����Ԥ��
        */
        Point2f returnNextPoint();




        /*
         * @brief �����ж��Ƿ��ҵ���
         * @ps:������_isfund��ֵ
        */

        bool getIsfund();//��־�Ƿ��ҵ��˴����Ŀ��
        /*
        * @brief �����ж��Ƿ��Ԥ��
        */
        bool isForeCast();


        /*
        * @brief �ջ�ģʽ�¼���Բ��
        * @para vector_centerPoint ��Ҫʶ������Ĵ����Ŀ��
        *
        */

        void close_circleCenter();


        /*
        * @brief �ڶ���Ԥ��Բ�ĵķ�ʽ
        * @para float input :
        */
        Point2f secondWayToCalculateForecastPoint(float input);
        /*
        * @brief Is the direction of roration is clockwise or anticlockwise
        * @para bool direction
        */
        void setClockwise(bool isclockwise);
        /*
        * @brief Is the direction of roration is clockwise or anticlockwise
        * @para bool direction
        */
        void setAntiClockwise(bool isclockwise);

private:
    bool _isclockwise;				//use to judge the direction of retation
    Point2f _center;				//the center of windmill
    Point2f _fanCenter;             //the center of the fan of the windmill


    vector<Point2f> vector_centerPoint;  //the temporary storage of the fancenter
    Mat _srcImg;					//the original imgage
    Mat _processedImg;				//image in the processing
    int MAXRANGE;  //�������߿��ⷶΧ
    int MINRANGE; //������С�߿��ⷶΧ
    int MAXMAXRANGE;
    int _DIFFER_DISTANCE; //
    Point2f _forecastCenter;     //the center of the fan of the windmill in the forecast mod
    double H;
    double S;
    double V;
    Point2f _noFound;//the point when can't find target
    bool _isfund; //��־�Ƿ��ҵ��˴����Ŀ��

//    RotatedRect _detectedRect;      //the fan centers which were detected before
//    vector<RotatedRect> vector_detectedRect;  //the fan centers which were detected before will be store up in this vector
//    vector<cv::Point2f> vertex;
//    vector<Rect> vector_Rect;
//    Rect _roi;
//    Mat _roiImg;
    queue <Point2f> q;
    float fanwidth;
    float longth;
    float Lead;                         //tiqianliang the lead of the target
    Point2f direction;
    float Offset;
    vector<vector<Point> > contours;
    Point2f newDirection;  //����zhy�����ҵ����·���

};
