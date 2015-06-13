CC = gcc # C compiler
CPP = g++ # C compiler
CFLAGS = -fPIC -Wall -g # C flags
DEFINES =  -D RASPICAM
LDFLAGS = -shared # linking flags
RM = rm -f  # rm command
TARGET_LIB = libLaserCam.so # target lib
SONAME = $(TARGET_LIB)

INCLUDES = -I/usr/local/opencv/include 
LIB_PATHS = -L/usr/local/opencv/lib
LIBS = -lraspicam -lraspicam_cv -lopencv_features2d -lopencv_calib3d -lopencv_ml -lopencv_video -lopencv_legacy -lopencv_objdetect -lopencv_photo -lopencv_imgproc -lopencv_core

SRCS = laserCam.cpp # source files
OBJS = $(SRCS:.cpp=.o)
 
.PHONY: all
all: ${TARGET_LIB}
 
$(SRCS:.cpp):%.cpp
	$(CPP) $(DEFINES) $(INCLUDES) $(CFLAGS) -MM $<
 
$(SRCS:.c):%.c
	$(CC) $(DEFINES) $(INCLUDES) $(CFLAGS) -MM $<
 
$(TARGET_LIB): $(OBJS)
	$(CC)  ${LDFLAGS} $(LIB_PATHS) $(LIBS) -o $@ $^
  
.PHONY: clean
clean:
	-${RM} ${OBJS}
	
.PHONY: clobber
clobber:
	-${RM} ${OBJS} ${TARGET_LIB}
	
.PHONY: install
install: ${TARGET_LIB}
	install -m 0755 ${TARGET_LIB} /usr/local/lib
	ldconfig
	ldconfig -p | grep Laser