
#ifdef __cplusplus

#include <stdlib.h>
#define _WHITE 255
#define _SCALE 255

class RaspicamLaser {
public:
  RaspicamLaser(int width = 640, int height = 480);
  ~RaspicamLaser();
  bool openCamera();
  void closeCamera();
  bool acquireFrame(int slp);
  bool position(int *x, int*y, int slp = 0);
  void setFrameSize(int width, int height);
  unsigned int red_threshold() {return _red_thr;};
  void set_red_threshold(unsigned int v) {_red_thr = v;};
  bool saveFrame(std::string &name);
  bool available() {return _available && _camera->isOpened();};
private:
  unsigned int _red_thr;
  raspicam::RaspiCam_Cv *_camera;
  cv::Mat _lastFrame;
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
CRaspicamLaser newCRaspicamLaser(int width, int height);
void           delCRaspicamLaser(CRaspicamLaser rl);

//
// Each public method. Takes an opaque reference to the object
// that was returned from the above constructor plus the methods parameters.
int CRaspicamLaserOpenCamera(CRaspicamLaser rl);
void CRaspicamLaserCloseCamera(CRaspicamLaser rl);

int CRaspicamLaserPosition(CRaspicamLaser rl, int *x, int*y);

void CRaspicamLaserSetFrameSize(CRaspicamLaser rl, int width, int height);

unsigned int CRaspicamLaserRedThreshold(CRaspicamLaser rl);
void CRaspicamLaserSetRedThreshold(CRaspicamLaser rl, unsigned int thr);

int CRaspicamLaserAvailable(CRaspicamLaser rl);

int CRaspicamLaserSaveFrame(CRaspicamLaser rl, const char *cname, int slp);

#ifdef __cplusplus
} //extern "C" {
#endif

