// main code file for lil light meter Arduino with an appState StateFlow machine.

//#include <stdio.h>
//#include <string.h>

//#define DEBUG_STATE
//#define DEBUG_EVENTS

// includes needed for the 7 segment display
#include <Wire.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>
#include "view.h"

// timer utility
unsigned long timer = 0;
void set_timer(unsigned long timer_milli) {
  timer = millis() + timer_milli;
}


// application vars and methods

#include "app_state.h"
#include "signal_queue.h"

#include "input.h"

int offset_index = 0;
byte small_fS = 8;
byte large_fS = 1; 
#define ASA_ARRAY_SIZE 7
int asa_arr[ASA_ARRAY_SIZE] = {50, 100, 200, 400, 800, 1600, 3200};
byte asa_index = 3;
#define SPEED_ARRAY_SIZE 12
int speed_arr[SPEED_ARRAY_SIZE] = {1000, 500, 250, 125, 60, 30, 15, 8, 4, 2, 1, 0};
byte speed_index = 6;
#define FSTOP_ARRAY_SIZE 13
float fstop_arr[FSTOP_ARRAY_SIZE] = {1.0, 1.4, 2.0, 2.8, 4.0, 5.6, 8.0, 11.0, 16.0, 22.0, 32.0, 44.0, 64.0};
byte fstop_index = 3;

int getLumIndex(int s) {
  return((float)s / 50.0) + offset_index; // adjusted form /100 to /50
}

// config functions
void welcome() {
  display(97, 'A', false);
}
void display_offset() {
  display(offset_index, '_', false);
}
void inc_offset() {
  if (offset_index < 5) {offset_index++;}
  send_signal(done);
}
void dec_offset() {
  if (offset_index > -5) {offset_index--;}
  send_signal(done);
}
void display_small_fS() {
  display_float(fstop_arr[small_fS], 'o', false);
}
void inc_small_fS() {
  if (small_fS < FSTOP_ARRAY_SIZE-1) {small_fS++;}
  send_signal(done);
}
void dec_small_fS() {
  if (small_fS > 0) {small_fS--;}
  send_signal(done);
}
void display_large_fS() {
  display_float(fstop_arr[large_fS], 'O', false);
}
void inc_large_fS() {
  if (large_fS < FSTOP_ARRAY_SIZE-3) {large_fS++;}
  send_signal(done);
}
void dec_large_fS() {
  if (large_fS > 0) {large_fS--;}
  send_signal(done);
}


// isPossibleFStop(s) determines if the the current light_sample, shutter speed and asa
// will map to a "possible" f-Stop. otherwise false indicates that we must suggest a different 
// shutter speed (see toFStop(s))
boolean isPossibleFStop(int s) {
  int i = getLumIndex(s) + speed_index - asa_index;
  //float asa = (float)asa_arr[asa_index];
  //float spd = (float)speed_arr[speed_index];
  //float fst = lum * (asa / spd);
  if (i < 0) {return false;}
  if (i >= FSTOP_ARRAY_SIZE) {return false;}
  return true;
}

float toFStop(int s) {
  int lum_index = getLumIndex(s);
  int i = lum_index + speed_index - asa_index;
  byte recall_speed_index = speed_index;
  while (i < 0 && speed_index < SPEED_ARRAY_SIZE) {
    speed_index++;
    i = lum_index + speed_index - asa_index;
  }
  while (i >= SPEED_ARRAY_SIZE && speed_index < SPEED_ARRAY_SIZE) {
    speed_index++;
    i = lum_index + speed_index - asa_index;
  }
  if (i < 0 || i >= SPEED_ARRAY_SIZE) {
    // this whole business of changing the shutter speed has failed to help
    speed_index = recall_speed_index;
    return (0.0);
  }
  fstop_index = i;
  return (fstop_arr[i]);
}

// application methods
void sample_light() {
  int s =  analogRead(sensorPin);
  delay(5);
  light_reading = (analogRead(sensorPin) + s)/2 ;
  send_signal(done);
}
void disp_f_stop() {
  float fs;
  if (!isPossibleFStop(light_reading)) {
    set_timer(1000);
  }
  fs = toFStop(light_reading);
  display_float(fs, 'F', false);
}
void reset_sleep_timeout() {}
void display_shutter_speed() {
  int speed_value = speed_arr[speed_index];
  display(speed_value, 's', false);
}
void display_asa() {
  int asa_value = asa_arr[asa_index];
  display(asa_value, 0, false);
} 
void start_config_timer() {}
void display_black() {
  display(111, 0, false);
}
void slow_pulse() {}
void decrement_asa() {
  if (asa_index > 0) {asa_index--;}
  send_signal(done);
}
void increment_asa() {
  if (asa_index < ASA_ARRAY_SIZE-1) {asa_index++;}
  send_signal(done);
}
void decrement_speed() {
  if (speed_index > 0) {speed_index--;}
  send_signal(done);
}
void increment_speed() {
  if (speed_index < SPEED_ARRAY_SIZE-1) {speed_index++;}
  send_signal(done);
}

void send_signal(int sig) {
    enqueue(sig, -1, false, false);
}

void check_timer() {
  if (timer != 0 && timer < millis()) {
    struct Signal_item s;
    s.sig = times_up;
    s.data = 0;
    if (enqueue(times_up, 0, true, false)) {
      timer = 0;
    }
  }
}


void setup() {

  // initialize inputs
  setup_inputs();
  setup_view();
  // initialize the state machine
  setup_pre_init_state();
}

// main arduino loop
void loop () {
  listen_inputs();
  check_timer();

  // onTick function
  onTick_processor();

  while(queue_size() > 0) {
    struct Signal_item this_event = dequeue();

    State from_state = state;

    // see if this signal causes a transitions in the state machine
    state = (State)state_trans_processor(state, this_event.sig, this_event.data);

    // did the signal cause a state change?
    if (from_state != state) {
      // since this 'state' is a new state --> run any onEnterState functionality.
      onEnterState_processor(this_event.sig, this_event.data);
    }
    else { // nope, no state change occurred
      if (this_event.consume) {
        // signals marked "consume" that do not cause a transition are put back in the queue.
        // note that consume is set to false so that this re queuing will not loop
        enqueue(this_event.sig, this_event.data, false, false);
      }
    }

  }
  delay(100);

}

