#include <ctime>
#include <fstream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>
#include <raspicam/raspicam.h>

#include "laserCam.h"

//#define MARK_LINE printf("*** FILE: %s - LINE: %d\n", __FILE__, __LINE__)
#define MARK_LINE

RaspicamLaser::RaspicamLaser(int width, int height) {
  _available = false;
  _camera = new raspicam::RaspiCam_Cv();
  setFrameSize(width, height);
  // capture.set(CV_CAP_PROP_BRIGHTNESS, 50);
  // capture.set(CV_CAP_PROP_SATURATION, 80);
  // capture.set(CV_CAP_PROP_FPS, 30);
  _available = _camera->open();
}

RaspicamLaser::~RaspicamLaser() {
  _camera->release();
  delete _camera;
}

void RaspicamLaser::setFrameSize(int width, int height) {
  _camera->set(CV_CAP_PROP_FRAME_WIDTH, width);
  _camera->set(CV_CAP_PROP_FRAME_HEIGHT, height);
}

void RaspicamLaser::saveFrame(std::string &name, int slp) {
  if (slp > 0)
    sleep(slp);
  cv::Mat frame;
  acquireFrame(frame);
  // save
  imwrite(name, frame);
}

int RaspicamLaser::acquireFrame(cv::Mat &frame) {
  if (!_available)
    return -1;
  _camera->grab();
  _camera->retrieve(frame);
  return 0;
}

int RaspicamLaser::position(int *x, int *y) {
  if (!_available)
    return -1;
  cv::Mat framehsv;
  cv::Mat output;
  unsigned int c = 0;
  int rows = 0, cols = 0;
  cv::Scalar min(0, 5, 180);
  cv::Scalar max(15, 70, 240);
  cv::Mat frame;

  acquireFrame(frame);
  cv::cvtColor(frame, framehsv, CV_BGR2HSV);
  rows = frame.rows, cols = frame.cols;
  cv::inRange(framehsv, min, max, output);
  for (register int i = 0; i < rows; i++) {
    for (register int k = 0; k < cols; k++) {
      c = static_cast<int>(output.at<uchar>(i, k));
      if (c == _WHITE) {
        *x = (k - cols / 2) * _SCALE / cols;
        *y = (rows / 2 - i) * _SCALE / rows;
      }
    }
  }
  return 0;
}

#define RASPICAM_CLASS(o) reinterpret_cast<RaspicamLaser *>(o)

CRaspicamLaser newCRaspicamLaser(int width, int height) {
  return reinterpret_cast<void *>(new RaspicamLaser(width, height));
}

void delCRaspicamLaser(CRaspicamLaser rl) {
  delete RASPICAM_CLASS(rl);
}

//
// Each public method. Takes an opaque reference to the object
// that was returned from the above constructor plus the methods parameters.
int CRaspicamLaserPosition(CRaspicamLaser rl, int *x, int *y) {
  return RASPICAM_CLASS(rl)->position(x, y);
}

int CRaspicamLaserAvailable(CRaspicamLaser rl) {
  return RASPICAM_CLASS(rl)->available();
}

void CRaspicamLaserSetFrameSize(CRaspicamLaser rl, int width, int height) {
  RASPICAM_CLASS(rl)->setFrameSize(width, height);
}

void CRaspicamLaserSaveFrame(CRaspicamLaser rl, const char *cname, int slp) {
  std::string name = cname;
  return RASPICAM_CLASS(rl)->saveFrame(name, slp);
}
