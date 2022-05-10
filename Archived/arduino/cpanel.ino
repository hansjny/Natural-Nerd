#include <FastLED.h>

#define DEBUG 0

#define NUM_PWR_IO 3
#define NUM_LEDS 34
#define LED_PIN 11
#define FLUCT_THRESH 10

// Color scheme 
#define ACTIVE 20, 200, 255
#define PASSIVE 255, 140, 20

// Timing
#define PULSE_CYCLE 4000
#define RUN_DELAY 50
#define IDLE_SECONDS 5

int handle_rgb_light(struct pwr_io_ctrl ctrls[NUM_PWR_IO]);
int handle_light_adjustment(struct pwr_io_ctrl ctrls[NUM_PWR_IO]);
int update_value(struct pwr_io_ctrl *ctrl_obj);
int set_led_color(int color1, int color2, int color3);

// pwr_io_ctrl is used for storing all of the values pertaining to a specific input/output. 
struct pwr_io_ctrl{
  int out_pin;                  // Digital output pin
  int analog_in_pin;            // Analog input pin
  int current_value;            // Current brightness value
  unsigned long last_touched;   // Time when knob was last turned
  int base_color[3];            
  /** 3 value array describing RGB color (0-255). This base_color value is used whenever the 
   *  brightness is being adjusted. For example, if the user turns first knob, the lights 
   *  inside the control box will turn cyan and a number of lights will turn on. The number
   *  of lights are used to indicate the brightness setting of the first output.
   */
};

// Pulse is used for pulsing a color regularly over a base color
struct pulse {
  int calls_per_cycle;          // The number of times that the pulse will need to be called.
  float current_color[3];       // 3 value array describing current RGB color (0-255)
  int pulse_color[3];           // 3 value array describing RGB color to pulse (0-255)
  int retracting;               // Whether the pulsed color is being activated or deactivated
};

//Initialize Input pin & Output Pin pairs 
struct pwr_io_ctrl io1 = {3, 1, 0, 0, {0, 255, 255}}; 
  // Output 3 is controlled with analog 1, and starts with value 0, never touched, with base color cyan
struct pwr_io_ctrl io2 = {9, 6, 0, 0, {255, 255, 0}}; 
  // Output 6 is controlled with analog 9, and starts with value 0, never touched, with base color yellow
struct pwr_io_ctrl io3 = {5, 4, 0, 0, {120, 0, 255}};
  // Output 4 is controlled with analog 5, and starts with value 0, never touched, with base color purple
//struct pwr_io_ctrl io4 = {6, 2, 0, 0, {80, 255, 0}};

struct pwr_io_ctrl ctrls[NUM_PWR_IO] = {io1, io2, io3}; // Group the controls together

struct pulse passive_pulse  = {PULSE_CYCLE / RUN_DELAY, {0.0, 0.0, 0.0}, {PASSIVE}, 0}; 
struct pulse active_pulse   = {PULSE_CYCLE / RUN_DELAY, {0.0, 0.0, 0.0}, {ACTIVE}, 0};

//Define RGB LED array
CRGB leds[NUM_LEDS];

void setup() {
  pinMode(io1.out_pin, OUTPUT);
  pinMode(io1.analog_in_pin, INPUT);
  
  pinMode(io2.out_pin, OUTPUT);
  pinMode(io2.analog_in_pin, INPUT);
  
  pinMode(io3.out_pin, OUTPUT);
  pinMode(io3.analog_in_pin, INPUT);
  
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  set_led_color(0, 255, 70);
  Serial.begin(9600);      // open the serial port at 9600 bps:
}

// the loop routine runs over and over again forever:
void loop() {

  handle_light_adjustment(ctrls);
  handle_rgb_light(ctrls);
  
  /** Debug outputs **/
  if (DEBUG) {
    Serial.print("PIN: ");
    Serial.print(io1.analog_in_pin);
    Serial.print(", val: ");
    Serial.print(analogRead(io1.analog_in_pin));
    Serial.println();
    
    Serial.print("PIN: ");
    Serial.print(io2.analog_in_pin);
    Serial.print(", val: ");
    Serial.print(analogRead(io2.analog_in_pin));
    Serial.println();
    
    Serial.print("PIN: ");
    Serial.print(io3.analog_in_pin);
    Serial.print(", val: ");
    Serial.print(analogRead(io3.analog_in_pin));
    Serial.println();
  }
  delay(RUN_DELAY);
}


int light_pulse(struct pulse *p) {
  float steps[3]  =  {(float)p->pulse_color[0] / p->calls_per_cycle, 
                      (float)p->pulse_color[1] / p->calls_per_cycle,
                      (float)p->pulse_color[2] / p->calls_per_cycle};
  
  if (p->retracting) {
    p->current_color[0] -= steps[0];
    p->current_color[1] -= steps[1];
    p->current_color[2] -= steps[2]; 
    
    if (p->current_color[0] <= 0 ||
        p->current_color[0] <= 0 ||
        p->current_color[0] <= 0) {
      p->current_color[0] = 0.0; 
      p->current_color[1] = 0.0;
      p->current_color[2] = 0.0;     
      p->retracting = 0; 
    }
  }
  else {
    p->current_color[0] += steps[0];
    p->current_color[1] += steps[1];
    p->current_color[2] += steps[2]; 
     
    if (p->current_color[0] >= p->pulse_color[0] ||
        p->current_color[1] >= p->pulse_color[1] ||
        p->current_color[2] >= p->pulse_color[2]) {

      p->current_color[0] = (float)p->pulse_color[0]; 
      p->current_color[1] = (float)p->pulse_color[1]; 
      p->current_color[2] = (float)p->pulse_color[2];      
      p->retracting = 1;    
    }
  }
  
  set_led_color((int)p->current_color[0], 
                (int)p->current_color[1],
                (int)p->current_color[2]);
}


/** Set the circle of RGB leds to correspond to
 *  the value fetched by the potentiometer, as
 *  a visual cue to how many % of the light
 *  strength that is being shown/adjusted
 */
int set_rgb_circle(struct pwr_io_ctrl *ctrl) {
    int i, val = map(ctrl->current_value, 0, 255, 0, NUM_LEDS);
    for (i = 0; i < NUM_LEDS; i++) {
      if (i < val) 
        leds[i] = CRGB{ctrl->base_color[0], ctrl->base_color[1], ctrl->base_color[2]};
      else
        leds[i] = CRGB{0, 0, 0};
    }
  FastLED.show(); 
}

/** Check if there are any potentiometers / lights
 *  whose value has been changed within the frames
 *  of the idle time. If it has, we can update RGB
 *  circle
 */
int handle_rgb_light(struct pwr_io_ctrl ctrls[NUM_PWR_IO]) {
  int i;
  int off = 1;
  unsigned long curtime = millis(), lowest_diff = curtime;
  struct pwr_io_ctrl *lowest_ctrl = NULL;

  for (i = 0; i < NUM_PWR_IO; i++) {
    unsigned long measure_diff = curtime - ctrls[i].last_touched; 
    if (measure_diff < 1000*IDLE_SECONDS && measure_diff < lowest_diff) {
      lowest_ctrl = &ctrls[i];
      lowest_diff = measure_diff;
    }
    
    if (ctrls[i].current_value != 0)
      off = 0;
  }

  if (lowest_ctrl != NULL) 
    set_rgb_circle(lowest_ctrl);
  else {
   if (off)
     light_pulse(&passive_pulse);
   else
     light_pulse(&active_pulse);
  }
    
  return 0;
}


int handle_light_adjustment(struct pwr_io_ctrl ctrls[NUM_PWR_IO]) {
  int i;
  for (i = 0; i < NUM_PWR_IO; i++)
    update_value(&ctrls[i]);
  return 0;
}


int update_value(struct pwr_io_ctrl *ctrl_obj) {
  int val = map(analogRead(ctrl_obj->analog_in_pin), 0, 1023, 0, 255);
  
  //Avoid fluctuation flickering on low & high by not changing unless major
  if (ctrl_obj->current_value + FLUCT_THRESH  > val
      && ctrl_obj->current_value - FLUCT_THRESH  < val) 
    return 0;
  
  if (val <= FLUCT_THRESH)
    val = 0;
  else if (val >= 255 - FLUCT_THRESH)
    val = 255;
  
  analogWrite(ctrl_obj->out_pin, val);

  ctrl_obj->current_value = val;
  ctrl_obj->last_touched = millis();
  return 0;
}

// Basic method to set all the leds to the given color value.
int set_led_color(int color1, int color2, int color3) {
  int i;
  for (i = 0; i < NUM_LEDS; i++)
    leds[i] = CRGB{color1, color2, color3};

 FastLED.show(); 
 return 0;
}
