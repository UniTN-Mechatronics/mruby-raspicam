#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>
#include <stdlib.h>
#define _WHITE 255
#define _SCALE 255

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  unsigned int h_min, h_max;
  unsigned int s_min, s_max;
  unsigned int b_min, b_max;
} HSB_limits_t;

void init_limits(HSB_limits_t *limits);
int open_camera(raspicam::RaspiCam_Cv *capture);
void get_laser_position(raspicam::RaspiCam_Cv *capture,
                        const HSB_limits_t *limits, int *x, int *y);

#ifdef __cplusplus
} /* extern "C" */
#endif

int AcquireFrame(raspicam::RaspiCam_Cv *capture, cv::Mat &frame);

int DetectLaser(cv::Mat &frame, const cv::Scalar min, const cv::Scalar max,
                int *x, int *y);
