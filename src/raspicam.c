/***************************************************************************/
/*                                                                         */
/* raspicam.c - mruby testing                                              */
/* Copyright (C) 2015 Paolo Bosetti,                                       */
/* paolo[dot]bosetti[at]unitn.it                                           */
/* Department of Industrial Engineering, University of Trento              */
/*                                                                         */
/* This library is free software.  You can redistribute it and/or          */
/* modify it under the terms of the GNU GENERAL PUBLIC LICENSE 2.0.        */
/*                                                                         */
/* This library is distributed in the hope that it will be useful,         */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           */
/* Artistic License 2.0 for more details.                                  */
/*                                                                         */
/* See the file LICENSE                                                    */
/*                                                                         */
/***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

#include "mruby.h"
#include "mruby/variable.h"
#include "mruby/string.h"
#include "mruby/data.h"
#include "mruby/class.h"
#include "mruby/value.h"
#include "mruby/array.h"
#include "mruby/numeric.h"

#include "laserCam.h"

//#define MARK_LINE printf("*** FILE: %s - LINE: %d\n", __FILE__, __LINE__)
#define MARK_LINE
#define E_RASPICAM_ERROR (mrb_class_get(mrb, "RaspicamError"))

// Struct holding data:
typedef struct { CRaspicamLaser camera; } raspicam_data_s;

// Garbage collector handler, for raspicam_data struct
// if raspicam_data contains other dynamic data, free it too!
// Check it with GC.start
static void raspicam_data_destructor(mrb_state *mrb, void *p_) {
  raspicam_data_s *pd = (raspicam_data_s *)p_;
  delCRaspicamLaser(pd->camera);
  free(pd);
  // or simply:
  // mrb_free(mrb, pd);
};

// Creating data type and reference for GC, in a const struct
const struct mrb_data_type raspicam_data_type = {"raspicam_data",
                                                 raspicam_data_destructor};

// Utility function for getting the struct out of the wrapping IV @data
static void mrb_raspicam_get_data(mrb_state *mrb, mrb_value self,
                                  raspicam_data_s **data) {
  mrb_value data_value;
  data_value = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@data"));

  // Loading data from data_value into p_data:
  Data_Get_Struct(mrb, data_value, &raspicam_data_type, *data);
  if (!*data)
    mrb_raise(mrb, E_RUNTIME_ERROR, "Could not access @data");
}

// Data Initializer C function (not exposed!)
static void mrb_raspicam_init(mrb_state *mrb, mrb_value self, mrb_int w,
                              mrb_int h) {
  mrb_value data_value;    // this IV holds the data
  raspicam_data_s *p_data; // pointer to the C struct
  data_value = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@data"));

  // if @data already exists, free its content:
  if (!mrb_nil_p(data_value)) {
    Data_Get_Struct(mrb, data_value, &raspicam_data_type, p_data);
    free(p_data);
  }
  // Allocate and zero-out the data struct:
  p_data = (raspicam_data_s *)malloc(sizeof(raspicam_data_s));

  // memset(p_data, 0, sizeof(raspicam_data_s));
  if (!p_data)
    mrb_raise(mrb, E_RUNTIME_ERROR, "Could not allocate @data");

  // Wrap struct into @data:
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "@data"), // set @data
             mrb_obj_value( // with value hold in struct
                 Data_Wrap_Struct(mrb, mrb->object_class, &raspicam_data_type,
                                  p_data)));

  // Now set values into struct:
  p_data->camera = newCRaspicamLaser(w, h);
}

static mrb_value mrb_raspicam_initialize(mrb_state *mrb, mrb_value self) {
  mrb_int width, height;
  mrb_int nargs = mrb_get_args(mrb, "|ii", &width, &height);
  if (nargs == 0) {
    width = 640;
    height = 480;
  } else if (nargs == 1) {
    height = (mrb_int)((width / 640.0) * 480.0);
  }
  // Call strcut initializer:
  mrb_raspicam_init(mrb, self, width, height);
  return mrb_nil_value();
}

static mrb_value mrb_raspicam_open(mrb_state *mrb, mrb_value self) {
  raspicam_data_s *p_data = NULL;
  // call utility for unwrapping @data into p_data:
  mrb_raspicam_get_data(mrb, self, &p_data);
  if (0 != CRaspicamLaserOpenCamera(p_data->camera))
    mrb_raise(mrb, E_RUNTIME_ERROR, "Could not open camera!");
  return self;
}

static mrb_value mrb_raspicam_close(mrb_state *mrb, mrb_value self) {
  raspicam_data_s *p_data = NULL;
  // call utility for unwrapping @data into p_data:
  mrb_raspicam_get_data(mrb, self, &p_data);
  CRaspicamLaserCloseCamera(p_data->camera);
  return mrb_nil_value();
}

static mrb_value mrb_raspicam_red_thr(mrb_state *mrb, mrb_value self) {
  unsigned int thr;
  raspicam_data_s *p_data = NULL;
  mrb_raspicam_get_data(mrb, self, &p_data);

  // raspicam with p_data content:
  thr = CRaspicamLaserRedThreshold(p_data->camera);
  return mrb_fixnum_value((mrb_int)thr);
}

static mrb_value mrb_raspicam_set_red_thr(mrb_state *mrb, mrb_value self) {
  raspicam_data_s *p_data = NULL;
  mrb_int val;
  mrb_get_args(mrb, "i", &val);
  // call utility for unwrapping @data into p_data:
  mrb_raspicam_get_data(mrb, self, &p_data);

  // raspicam with p_data content:
  CRaspicamLaserSetRedThreshold(p_data->camera, val);
  return mrb_fixnum_value(val);
}


static mrb_value mrb_raspicam_pos(mrb_state *mrb, mrb_value self) {
  raspicam_data_s *p_data = NULL;
  mrb_value ary;
  int x = 0, y = 0, res;
  // call utility for unwrapping @data into p_data:
  mrb_raspicam_get_data(mrb, self, &p_data);

  // raspicam with p_data content:
  ary = mrb_ary_new_capa(mrb, 2);
  res = CRaspicamLaserPosition(p_data->camera, &x, &y);
  if (0 != res)
    mrb_raise(mrb, E_RASPICAM_ERROR, "Could not get position (device closed?)");
  mrb_ary_set(mrb, ary, 0, mrb_fixnum_value(x));
  mrb_ary_set(mrb, ary, 1, mrb_fixnum_value(y));
  return ary;
}

static mrb_value mrb_raspicam_save(mrb_state *mrb, mrb_value self) {
  raspicam_data_s *p_data = NULL;
  mrb_value name;
  mrb_int slp = 0, nargs;
  nargs = mrb_get_args(mrb, "S|i", &name, &slp);
  if (nargs == 1)
    slp = 0;
  // call utility for unwrapping @data into p_data:
  mrb_raspicam_get_data(mrb, self, &p_data);

  // raspicam with p_data content:
  if (0 != CRaspicamLaserSaveFrame(p_data->camera,
                                   mrb_string_value_cstr(mrb, &name), slp))
    mrb_raise(mrb, E_RASPICAM_ERROR, "Could not save image (device closed?");
  return name;
}

void mrb_mruby_raspicam_gem_init(mrb_state *mrb) {
  struct RClass *raspicam;
  mrb_define_class(mrb, "RaspicamError", mrb_class_get(mrb, "Exception"));
  raspicam = mrb_define_class(mrb, "RaspiCam", mrb->object_class);
  
  mrb_define_method(mrb, raspicam, "initialize", mrb_raspicam_initialize,
                    MRB_ARGS_OPT(2));
  mrb_define_method(mrb, raspicam, "open", mrb_raspicam_open, MRB_ARGS_NONE());
  mrb_define_method(mrb, raspicam, "close", mrb_raspicam_close,
                    MRB_ARGS_NONE());
  mrb_define_method(mrb, raspicam, "red_threshold", mrb_raspicam_red_thr,
                    MRB_ARGS_NONE());
  mrb_define_method(mrb, raspicam, "red_threshold=", mrb_raspicam_set_red_thr,
                    MRB_ARGS_REQ(1));
  mrb_define_method(mrb, raspicam, "position", mrb_raspicam_pos,
                    MRB_ARGS_NONE());
  mrb_define_method(mrb, raspicam, "save_image", mrb_raspicam_save,
                    MRB_ARGS_REQ(1) & MRB_ARGS_OPT(1));
}

void mrb_mruby_raspicam_gem_final(mrb_state *mrb) {}
