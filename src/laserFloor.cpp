#include "laserFloor.h"
#include <stdio.h>
//#define MARK_LINE printf("*** FILE: %s - LINE: %d\n", __FILE__, __LINE__)
#define MARK_LINE

void init_limits(HSB_limits_t *limits) {
  limits->h_min = 0;
  limits->s_min = 5;
  limits->b_min = 180;
  limits->h_max = 15;
  limits->s_max = 70;
  limits->b_max = 240;
}

int open_camera(raspicam::RaspiCam_Cv *camera) {
  MARK_LINE;
  camera->set(CV_CAP_PROP_FRAME_WIDTH, 640);
  MARK_LINE;
  camera->set(CV_CAP_PROP_FRAME_HEIGHT, 480);
  MARK_LINE;
  // capture.set(CV_CAP_PROP_BRIGHTNESS, 50);
  // capture.set(CV_CAP_PROP_SATURATION, 80);
  // capture.set(CV_CAP_PROP_FPS, 30);
  if (!camera->open()) {
    return -1;
  }
  MARK_LINE;
  return 0;
}

void get_laser_position(raspicam::RaspiCam_Cv *camera,
                        const HSB_limits_t *limits, int *x, int *y) {
  cv::Mat frame;
  cv::Scalar min(limits->h_min, limits->s_min, limits->b_min);
  cv::Scalar max(limits->h_max, limits->s_max, limits->b_max);
  AcquireFrame(camera, frame);
  DetectLaser(frame, min, max, x, y);
}

int AcquireFrame(raspicam::RaspiCam_Cv *capture, cv::Mat &frame) {
  capture->grab();
  capture->retrieve(frame);
  return 0;
}

int DetectLaser(cv::Mat &frame, const cv::Scalar min, const cv::Scalar max,
                int *x, int *y) {
  cv::Mat framehsv;
  cv::Mat output;
  unsigned int c = 0;
  int rows = 0, cols = 0;
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
