
#ifdef __cplusplus

#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>
#include <stdlib.h>
#define _WHITE 255
#define _SCALE 255

class RaspicamLaser {
public:
  RaspicamLaser();
  int acquireFrame(cv::Mat &frame);
  int position(int *x, int*y);
private:
  raspicam::RaspiCam_Cv *camera;
};

#endif

//
// C Interface.
typedef void* CRaspicamLaser;

#ifdef __cplusplus
extern "C" {
#endif
//
// Need an explicit constructor and destructor.
CRaspicamLaser newCRaspicamLaser();
void           delCRaspicamLaser(CRaspicamLaser rl);

//
// Each public method. Takes an opaque reference to the object
// that was returned from the above constructor plus the methods parameters.
int CRaspicamLaserPosition(CRaspicamLaser rl, int *x, int*y);

#ifdef __cplusplus
} //extern "C" {
#endif

