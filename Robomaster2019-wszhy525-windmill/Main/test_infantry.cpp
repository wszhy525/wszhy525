
#include <unistd.h>

#include"iostream"
#include<opencv2/opencv.hpp>
#include<thread>
#include<time.h>
#include<fstream>
#include"ImgProdCons.h"
#include"./Pose/AngleSolver.hpp"
#include"./Armor/ArmorDetector.h"
#include"./Rune/Rune.h"
#include"./Pose/Predictor.hpp"
#define DEBUG
#define CAMERA_NUMBER 10
using namespace std;
using namespace cv;

namespace rm
{

void ImgProdCons::init()
{
    //prevent usb port from being blocked
    init_signals();

    //Initialize camera
    _videoCapturePtr->open(0,1);

    _videoCapturePtr->setExposureTime(8);
    _videoCapturePtr->setVideoFormat(1280, 720, true);
    _videoCapturePtr->setBalanceRatio(1.6, 1.35, 2.1, false);
    _videoCapturePtr->setGain(1);
    _videoCapturePtr->setGamma(1);


    _videoCapturePtr->startStream();
    _videoCapturePtr->closeStream();
    _videoCapturePtr->startStream();
    //_videoCapturePtr->getAcquisitionFrameRate(framerateins);
    //cout<<framerateins<<endl;
    _videoCapturePtr->setFPS(50.0);
    _videoCapturePtr->info();

    //Initilize serial
    _serialPtr->openPort();
    _serialPtr->setDebug(false);
    int self_color;
    while(_serialPtr->setup(self_color) != Serial::OJBK)
    {
        sleep(1);
    }

  //  self_color = rm::BLUE;
    cout << "I am " << (self_color == rm::BLUE ? "blue" : "red") << "." << endl;
//    int self_color = rm::BLUE;

    //Initialize angle solver
    AngleSolverParam angleParam;
    angleParam.readFile(CAMERA_NUMBER);//choose camera
    _solverPtr->init(angleParam);
    _solverPtr->setResolution(_videoCapturePtr->getResolution());

    //Initialize armor detector
    ArmorParam armorParam;
    _armorDetectorPtr->init(armorParam);
    _armorDetectorPtr->setEnemyColor(self_color == rm::BLUE ? rm::RED : rm::BLUE);

    //Initialize rune detector
    //_runeDetectorPtr->init();
    //Initialize sentry strategies
    _sentryStrategiesPtr->initPara();

    _task = Serial::NO_TASK;
}

void ImgProdCons::sense()
{
    /* Loop for sensing */
    for(;;)
    {
        FeedBackData feedBackData;
        /* TODO: Handel exceptions when socket suddenly being plugged out. */
        if(_serialPtr->tryFeedBack(feedBackData, chrono::milliseconds(20)) == Serial::OJBK)
        {
            _task = /*Serial::BIG_BUFF;*//*feedBackData.task_mode;*/Serial::AUTO_SHOOT;

            this_thread::sleep_for(chrono::milliseconds(80));
        }
        else
        {

            _task = Serial::AUTO_SHOOT;
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    }

}

void ImgProdCons::consume()
{

    /*
     * Variables for serials
     */
    ControlData controlData;

    /*
     * Variables for recording camera
     */
    VideoWriter writer;
    bool isRecording = false;
    bool readyTOrecord = false;
    /*
     *  Variables for angle solve module
     */
    int angleFlag;
    Vec2f targetAngle;

    /*
     * Variables for rune detector
     *
     */
    std::array<int, 9> runeNumbers;
    std::vector<int> ledNumbers;
//    int runeFlag;
//    int shootFlag;

    /* Variables for armor detector module */
    int armorFlag;
    int armorType;
    std::vector<cv::Point2f> armorVertex;
    /*
     *  The main loop for task
     */
    Frame frame;
    for(;;)
    {
#ifndef _MSC_VER
        if (_quit_flag)
            return;
#endif
        if(_serialPtr->getErrorCode() == Serial::SYSTEM_ERROR || !_videoCapturePtr->isOpened())
        {
            this_thread::sleep_for(chrono::seconds(3));
        }
//        switch(_task)
//        {
//        case Serial::NO_TASK:
//            cout<< "manual" <<endl;
//            break;
//        case Serial::SMALL_BUFF:
//            cout<<"small buff"<<endl;
//            break;
//        case Serial::BIG_BUFF:
//            cout<<"big buff"<<endl;
//            break;
//        case Serial::AUTO_SHOOT:
//            cout<<"auto shoot"<<endl;
//            break;
//        default:
//            cout<<"unknown mode"<<endl;
//            break;
//        }

        if(!_buffer.getLatest(frame)) continue;


//        cv::imshow("emm",frame.img);
//        cv::waitKey(5);


        if(_task ==Serial::AUTO_SHOOT)//BIG_BUFF)
        {
      //            _videoCapturePtr->setVideoFormat(1280, 720, true);
//            _videoCapturePtr->setExposureTime(100);
//            auto t1 = chrono::high_resolution_clock::now();
            isRecording = false;
            readyTOrecord = true;

                  // _solverPtr->setResolution(Size(1280, 720));

//            _armorDetectorPtr->loadImg(frame.img);
//            armorFlag = _armorDetectorPtr->detect();
            _windmillPtr->loadImg(frame.img);
            _windmillPtr->setAntiClockwise();
            _windmillPtr->setClockwise();
            _windmillPtr->Detect();
             Point2f windmillArmor;


            if(_windmillPtr->getIsfund())//armorFlag == ArmorDetector::ARMOR_LOCAL || armorFlag == ArmorDetector::ARMOR_GLOBAL)
            {
//                armorVertex = _armorDetectorPtr->getArmorVertex();
//                armorType = _armorDetectorPtr->getArmorType();
                windmillArmor =  _windmillPtr->returnNextPoint(); //指向预测点
                _solverPtr->setTarget(windmillArmor, armorType);//利用单点结算来指向
                angleFlag = _solverPtr->solve();
                if(angleFlag != rm::AngleSolver::ANGLE_ERROR)
                {

                    targetAngle = _solverPtr->getAngle();
                    //_sentryStrategiesPtr->pushBackAngle(targetAngle);
                   // if(_sentryStrategiesPtr->ifHitTarget())
                    //    controlData.shoot_mode  = Serial::BURST_FIRE | Serial::LOW_SPEED;
                  //  else
                   //     controlData.shoot_mode  = Serial::NO_FIRE | Serial::LOW_SPEED;
//                    cout<<hex<<controlData.shoot_mode<<dec<<endl;

                    controlData.frame_seq   = frame.seq;
                    //controlData.shoot_mode  = Serial::BURST_FIRE | Serial::LOW_SPEED;
                    controlData.pitch_dev   = targetAngle[1];
                    controlData.yaw_dev     = targetAngle[0];

//                    if(CAMERA_NUMBER == 6)
//                    {
//                        controlData.pitch_dev = targetAngle[1];
//                        //controlData.yaw_dev     = targetAngle[0] + 2;
//                    }
//                    if(CAMERA_NUMBER == 7)
//                    {
//                        controlData.pitch_dev   = targetAngle[1] - 1.7;
//                        controlData.yaw_dev     = targetAngle[0] - 0.9;
//                    }

                    if(CAMERA_NUMBER == 10)
                    {
                        controlData.pitch_dev   = targetAngle[1] ;
                        controlData.yaw_dev     = -targetAngle[0];
                    }

                    //                controlData.speed_on_rail = 0;

#ifdef SENTRY
                    controlData.gimbal_mode = Serial::SERVO_MODE;
#endif
//                    cout<<_serialPtr->tryControl(controlData, chrono::milliseconds(3))<<endl;
                    if(_serialPtr->tryControl(controlData, chrono::milliseconds(3)) != Serial::OJBK)
                    {
                        cout<<"not sent"<<endl;
                    }
                    cout << "windmillDeviation: " << "["<<controlData.pitch_dev << " " << controlData.yaw_dev<<"]" << endl;
                   // cout<<"distance="<<_solverPtr->getDistance()/6<<endl;


                }
            }
            else
            {

                cout<<"??????"<<endl;
                _sentryStrategiesPtr->popFrontAngle();
            }


#ifdef SENTRY
            else
            {
                controlData.gimbal_mode = Serial::PATROL_AROUND;
                if(_serialPtr->tryControl(controlData, chrono::milliseconds(3)) != Serial::OJBK)
                {
                    cout<<"not sent"<<endl;
                }

            }
#endif

        }
        else if(0)//_task==Serial::AUTO_SHOOT)
        {
      //            _videoCapturePtr->setVideoFormat(1280, 720, true);
//            _videoCapturePtr->setExposureTime(100);
//            auto t1 = chrono::high_resolution_clock::now();
            isRecording = false;
            readyTOrecord = true;

                  // _solverPtr->setResolution(Size(1280, 720));
            //_armorDetectorPtr->loadImg(frame.img);
            //armorFlag = _armorDetectorPtr->detect();
            _windmillPtr->loadImg(frame.img);
            _windmillPtr->Detect();
            Point2f windmillArmor =  _windmillPtr->getCenter();
//            if(armorFlag == ArmorDetector::ARMOR_LOCAL || armorFlag == ArmorDetector::ARMOR_GLOBAL)
//            {
                //armorVertex = _armorDetectorPtr->getArmorVertex();
                //armorType = _armorDetectorPtr->getArmorType();
                _solverPtr->setTarget(windmillArmor, 1);
                cout<<windmillArmor.x<<"    "<<windmillArmor.y<<endl;
                angleFlag = _solverPtr->solve();
                if(angleFlag != rm::AngleSolver::ANGLE_ERROR)
                {
                    targetAngle = _solverPtr->getAngle();
//                    _sentryStrategiesPtr->pushBackAngle(targetAngle);
//                    if(_sentryStrategiesPtr->ifHitTarget())
                        controlData.shoot_mode  = Serial::BURST_FIRE | Serial::LOW_SPEED;
//                    else
//                        controlData.shoot_mode  = Serial::NO_FIRE | Serial::LOW_SPEED;
//                    cout<<hex<<controlData.shoot_mode<<dec<<endl;

                    controlData.frame_seq   = frame.seq;
                    //controlData.shoot_mode  = Serial::BURST_FIRE | Serial::LOW_SPEED;
                    controlData.pitch_dev   = targetAngle[1];
                    controlData.yaw_dev     = targetAngle[0];

//                    if(CAMERA_NUMBER == 6)
//                    {
//                        controlData.pitch_dev = targetAngle[1];
//                        //controlData.yaw_dev     = targetAngle[0] + 2;
//                    }
//                    if(CAMERA_NUMBER == 7)
//                    {
//                        controlData.pitch_dev   = targetAngle[1] - 1.7;
//                        controlData.yaw_dev     = targetAngle[0] - 0.9;
//                    }

                    if(CAMERA_NUMBER == 10)
                    {
                        controlData.pitch_dev   = targetAngle[1] ;
                        controlData.yaw_dev     = -targetAngle[0];
                    }

                    //                controlData.speed_on_rail = 0;

#ifdef SENTRY
                    controlData.gimbal_mode = Serial::SERVO_MODE;
#endif
//                    cout<<_serialPtr->tryControl(controlData, chrono::milliseconds(3))<<endl;
                    if(_serialPtr->tryControl(controlData, chrono::milliseconds(3)) != Serial::OJBK)
                    {
                        cout<<"not sent"<<endl;
                    }
                    cout << "Deviation: " << "["<<controlData.pitch_dev << " " << controlData.yaw_dev<<"]" << endl;



                }
//            }
//            else
//            {

////                cout<<"??????"<<endl;
//                _sentryStrategiesPtr->popFrontAngle();
//            }


#ifdef SENTRY
            else
            {
                controlData.gimbal_mode = Serial::PATROL_AROUND;
                if(_serialPtr->tryControl(controlData, chrono::milliseconds(3)) != Serial::OJBK)
                {
                    cout<<"not sent"<<endl;
                }

            }
#endif

        }
        else
        {
            //            if(readyTOrecord)
            //            {
            //                _videoCapturePtr->setVideoFormat(640, 480, true);
            //                _videoCapturePtr->setExposureTime(200);

            //                if(isRecording)
            //                {
            //                    writer << frame.img;
            //                }
            //                else
            //                {
            //                    time_t t;
            //                    time(&t);
            //                    const string fileName = "/home/nvidia/Robomaster/Robomaster2018/" + to_string(t) + ".avi";
            //                    writer.open(fileName, CV_FOURCC('M', 'J', 'P', 'G'), 25, Size(frame.img.size().width, frame.img.size().height));
            //                    if(!writer.isOpened())
            //                    {
            //                        cout << "Capture failed." << endl;
            //                        continue;
            //                    }
            //                    isRecording = true;
            //                    cout << "Start capture. " + fileName +" created." << endl;
            //                }

            //            }
        }
        //auto t1 = chrono::high_resolution_clock::now();


        // auto t2 = chrono::high_resolution_clock::now();

#ifdef DEBUG
        if(waitKey(1) == 'q')
        {
            return;
        }
        //   cout << "Detection duration: " << (static_cast<chrono::duration<double, std::milli>>(t2 - t1)).count() << " ms" << endl;
#endif

    }
}
}
