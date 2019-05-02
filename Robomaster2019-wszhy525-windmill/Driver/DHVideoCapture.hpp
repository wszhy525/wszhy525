#pragma once

#ifndef __unix__
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "DHCamera/GenICam/StreamSource.h"
#include "DHCamera/GenICam/System.h"
#include "DHCamera/GenICam/Camera.h"
#include "DHCamera/GenICam/StreamSource.h"
#include "DHCamera/GenICam/GigE/GigECamera.h"
#include "DHCamera/GenICam/GigE/GigEInterface.h"
#include "DHCamera/Infra/PrintLog.h"
//#include "StreamRetrieve.h"
#include "DHCamera/Memory/SharedPtr.h"
#include "DHCamera/Infra/CString.h"
#include<opencv2/opencv.hpp>

using namespace cv;
using namespace Dahua::GenICam;
using namespace Dahua::Infra;

class DHVideoCapture
{
public:
    DHVideoCapture();
//    DHVideoCapture(const int id, int buffer_size = 2);
    ~DHVideoCapture();

    bool open(const int id, int size_buffer = 2);
    bool isOpened();

//    void setBufferSize(size_t bsize);
    bool setVideoFormat(size_t width, size_t height, bool mjpg = 1);
    bool setExposureTime(int t = 0);
    bool changeVideoFormat(int width, int height, bool mjpg = 1);
    bool setFPS(double fps);
    bool setBalanceRatio(double red, double green, double blue, bool autoBalance = false);
    bool setGain(double gain = 1.0);
    bool setGamma(double gamma = 1.0);
    int  getAcquisitionFrameRate(double &dFrameRate);
    bool startStream();
    bool closeStream();
    bool restartCapture();

    //RMVideoCapture& operator >> (cv::Mat & image);

    //bool read(cv::Mat& image);

    /*
     * @Brief:  Grabs the next frame from video file or capturing device.
     * @Return: 'true' if success
     */
    bool grab(CFrame &frame);

    /*
     * @Brief:  Decodes and returns the grabbed video frame.
     * @Output: img:    the video frame is returned here. If no frames has been grabbed
     *                  the image remains unchanged.
     */
    bool retrieve(cv::Mat& image, CFrame &frame);

    bool getBalanceRatio();
    cv::Size getResolution();

    void info();

private:
    ICameraPtr _cameraSptr;

    CSystem &_systemObj = CSystem::getInstance();

    IStreamSourcePtr _streamPtr;

    TVector<ICameraPtr> _vCameraPtrList;

    int _exposure_time;
    double _fps;
    int *_resolution;
    double *_balance_ratio;
    float _gamma;
    float _gain;

    int _camera_id;

    bool _grab_done = false;
};
