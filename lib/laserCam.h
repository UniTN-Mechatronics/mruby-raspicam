
#ifdef __cplusplus

#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>
#include <stdlib.h>
#define _WHITE 255
#define _SCALE 255

class RaspicamLaser {
public:
  RaspicamLaser();
  ~RaspicamLaser();
  int acquireFrame(cv::Mat &frame);
  int position(int *x, int*y);
  int available() {return _available ? 1 : 0;};
private:
  raspicam::RaspiCam_Cv *_camera;
  bool _available;
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

int CRaspicamLaserAvailable(CRaspicamLaser laser);

#ifdef __cplusplus
} //extern "C" {
#endif

