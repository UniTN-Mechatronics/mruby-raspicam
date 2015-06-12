#include "laserCam.h"
#include <stdio.h>

//#define MARK_LINE printf("*** FILE: %s - LINE: %d\n", __FILE__, __LINE__)
#define MARK_LINE


RaspicamLaser::RaspicamLaser() {
  camera = new raspicam::RaspiCam_Cv();
  camera->set(CV_CAP_PROP_FRAME_WIDTH, 640);
  camera->set(CV_CAP_PROP_FRAME_HEIGHT, 480);
  // capture.set(CV_CAP_PROP_BRIGHTNESS, 50);
  // capture.set(CV_CAP_PROP_SATURATION, 80);
  // capture.set(CV_CAP_PROP_FPS, 30);
  camera->open();
}

int RaspicamLaser::acquireFrame(cv::Mat &frame) {
  camera->grab();
  camera->retrieve(frame);
  return 0;
}

int RaspicamLaser::position(int *x, int*y) {
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


CRaspicamLaser newCRaspicamLaser() {
  return reinterpret_cast<void*>(new RaspicamLaser());
}

void delCRaspicamLaser(CRaspicamLaser laser) {
  delete reinterpret_cast<RaspicamLaser*>(laser);
}

//
// Each public method. Takes an opaque reference to the object
// that was returned from the above constructor plus the methods parameters.
int CRaspicamLaserPosition(CRaspicamLaser rl, int *x, int*y) {
  return reinterpret_cast<RaspicamLaser*>(rl)->position(x, y);
}

