/***************************************************************************/
/*                                                                         */
/* raspicam.c - mruby testing                                              */
/* Copyright (C) 2015 Paolo Bosetti and Matteo Ragni,                      */
/* paolo[dot]bosetti[at]unitn.it and matteo[dot]ragni[at]unitn.it          */
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

#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>

#include "mruby.h"
#include "mruby/variable.h"
#include "mruby/string.h"
#include "mruby/data.h"
#include "mruby/class.h"
#include "mruby/value.h"
#include "mruby/array.h"
#include "mruby/numeric.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "memory.h"
#include "laserFloor.h"

// Struct holding data:
typedef struct {
  raspicam::RaspiCam_Cv *camera;
  HSB_limits_t *limits;
} raspicam_data_s;

// Garbage collector handler, for raspicam_data struct
// if raspicam_data contains other dynamic data, free it too!
// Check it with GC.start
static void raspicam_data_destructor(mrb_state *mrb, void *p_) {
  raspicam_data_s *pd = (raspicam_data_s *)p_;
  delete (pd->camera);
  delete (pd->limits);
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
static void mrb_raspicam_init(mrb_state *mrb, mrb_value self) {
  mrb_value data_value;    // this IV holds the data
  raspicam_data_s *p_data; // pointer to the C struct

  data_value = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@data"));

  // if @data already exists, free its content:
  if (!mrb_nil_p(data_value)) {
    Data_Get_Struct(mrb, data_value, &raspicam_data_type, p_data);
    free(p_data);
  }
  // Allocate and zero-out the data struct:
  p_data = (raspicam_data_s*)malloc(sizeof(raspicam_data_s));
  memset(p_data, 0, sizeof(raspicam_data_s));
  if (!p_data)
    mrb_raise(mrb, E_RUNTIME_ERROR, "Could not allocate @data");

  // Wrap struct into @data:
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "@data"), // set @data
             mrb_obj_value( // with value hold in struct
                 Data_Wrap_Struct(mrb, mrb->object_class, &raspicam_data_type,
                                  p_data)));

  // Now set values into struct:
  p_data->limits = new HSB_limits_t;
  p_data->camera = new raspicam::RaspiCam_Cv();
  init_limits(p_data->limits);
  if (!open_camera(p_data->camera))
    mrb_raise(mrb, E_RUNTIME_ERROR, "Could not open camera!");
}

static mrb_value mrb_raspicam_initialize(mrb_state *mrb, mrb_value self) {
  // Call strcut initializer:
  mrb_raspicam_init(mrb, self);
  return mrb_nil_value();
}

static mrb_value mrb_raspicam_pos(mrb_state *mrb, mrb_value self) {
  raspicam_data_s *p_data = NULL;
  mrb_value ary;
  int x = 0, y = 0;
  // call utility for unwrapping @data into p_data:
  mrb_raspicam_get_data(mrb, self, &p_data);

  // raspicam with p_data content:
  ary = mrb_ary_new_capa(mrb, 2);
  get_laser_position(p_data->camera, p_data->limits, &x, &y);
  mrb_ary_set(mrb, ary, 0, mrb_fixnum_value(x));
  mrb_ary_set(mrb, ary, 1, mrb_fixnum_value(y));
  return ary;
}

static mrb_value mrb_raspicam_set_HSB_min(mrb_state *mrb, mrb_value self) {
  mrb_value ary_in = mrb_nil_value();
  raspicam_data_s *p_data = NULL;
  mrb_int i;
  mrb_value elem;
  mrb_get_args(mrb, "A", &ary_in);

  // call utility for unwrapping @data into p_data:
  mrb_raspicam_get_data(mrb, self, &p_data);
  if (3 != RARRAY_LEN(ary_in))
    mrb_raise(mrb, E_RUNTIME_ERROR, "Need an array of 3 numbers");
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "@hsb_min"), ary_in);
  p_data->limits->h_min = mrb_fixnum_p(mrb_ary_entry(ary_in, 0));
  p_data->limits->s_min = mrb_fixnum_p(mrb_ary_entry(ary_in, 1));
  p_data->limits->b_min = mrb_fixnum_p(mrb_ary_entry(ary_in, 2));

  return mrb_true_value();
}

static mrb_value mrb_raspicam_set_HSB_max(mrb_state *mrb, mrb_value self) {
  mrb_value ary_in = mrb_nil_value();
  raspicam_data_s *p_data = NULL;
  mrb_int i;
  mrb_value elem;
  mrb_get_args(mrb, "A", &ary_in);

  // call utility for unwrapping @data into p_data:
  mrb_raspicam_get_data(mrb, self, &p_data);
  if (3 != RARRAY_LEN(ary_in))
    mrb_raise(mrb, E_RUNTIME_ERROR, "Need an array of 3 numbers");
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "@hsb_max"), ary_in);
  p_data->limits->h_max = mrb_fixnum_p(mrb_ary_entry(ary_in, 0));
  p_data->limits->s_max = mrb_fixnum_p(mrb_ary_entry(ary_in, 1));
  p_data->limits->b_max = mrb_fixnum_p(mrb_ary_entry(ary_in, 2));

  return mrb_true_value();
}

/* MEMORY INFO */
static mrb_value mrb_process_getCurrentRSS(mrb_state *mrb, mrb_value self) {
  return mrb_fixnum_value(getCurrentRSS());
}

static mrb_value mrb_process_getPeakRSS(mrb_state *mrb, mrb_value self) {
  return mrb_fixnum_value(getPeakRSS());
}

void mrb_mruby_raspicam_gem_init(mrb_state *mrb) {
  struct RClass *raspicam, *process;
  raspicam = mrb_define_class(mrb, "Raspicam", mrb->object_class);
  mrb_define_method(mrb, raspicam, "initialize", mrb_raspicam_initialize,
                    MRB_ARGS_NONE());
  mrb_define_method(mrb, raspicam, "position", mrb_raspicam_pos,
                    MRB_ARGS_NONE());
  mrb_define_method(mrb, raspicam, "hsb_min=", mrb_raspicam_set_HSB_min,
                    MRB_ARGS_REQ(1));
  mrb_define_method(mrb, raspicam, "hsb_max=", mrb_raspicam_set_HSB_max,
                    MRB_ARGS_REQ(1));

  process = mrb_define_module(mrb, "ProcessInfo");
  mrb_const_set(mrb, mrb_obj_value(process), mrb_intern_lit(mrb, "PID"),
                mrb_fixnum_value(getpid()));
  mrb_const_set(mrb, mrb_obj_value(process), mrb_intern_lit(mrb, "PPID"),
                mrb_fixnum_value(getppid()));

  mrb_define_class_method(mrb, process, "current_mem",
                          mrb_process_getCurrentRSS, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, process, "peak_mem", mrb_process_getPeakRSS,
                          MRB_ARGS_NONE());
}

void mrb_mruby_raspicam_gem_final(mrb_state *mrb) {}


#ifdef __cplusplus
} //extern "C" {
#endif
