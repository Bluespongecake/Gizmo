//Libraries to include
#include <Servo.h>
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

//Stepper motor pins for RAMPS 1.4
//X is for the top left motor
#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38
#define X_MIN_PIN           3
#define X_MAX_PIN           2

//Y is for the top right motor
#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56
#define Y_MIN_PIN          14
#define Y_MAX_PIN          15

//Z is for the wiper
#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62
#define Z_MIN_PIN          18
#define Z_MAX_PIN          19

//Store current co-ordinates 
int current_x;
int current_y;

//Store steps to be made 
int x_change; 
int y_change; 
int x_steps_new;
int y_steps_new;
int * steps_new;

//Pin declerations


//Initialising objects
Servo penServo;
hd44780_I2Cexp lcd;

void setup() {
  Serial.begin(9600);
  
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(X_ENABLE_PIN, OUTPUT);
  digitalWrite(X_ENABLE_PIN, LOW);

  pinMode(Y_STEP_PIN, OUTPUT);
  pinMode(Y_DIR_PIN, OUTPUT);
  pinMode(Y_ENABLE_PIN, OUTPUT);
  digitalWrite(Y_ENABLE_PIN, LOW);

  pinMode(Z_STEP_PIN, OUTPUT);
  pinMode(Z_DIR_PIN, OUTPUT);
  pinMode(Z_ENABLE_PIN, OUTPUT);
  digitalWrite(Z_ENABLE_PIN, LOW);

  penServo.attach(11);

  lcd.begin(16, 2);

  //Dispaly start message
  lcd_start_message();

  //Setting home co-ordinates
  //Start "home" at (320,145);
  current_x = 220;
  current_y = 156;

  //SINGLE TEST CODE GOES HERE
  move_pen(0, -200);
  Serial.println(String(current_x) + ", " + String(current_y));
  move_pen(-50, 0);
  Serial.println(String(current_x) + ", " + String(current_y));
  draw(250, 0); 
  Serial.println(String(current_x) + ", " + String(current_y));
  move_pen(-250,0);
  Serial.println(String(current_x) + ", " + String(current_y));
}

void loop() {

}

void pen_up() {
  delay(200);
  penServo.write(0);
}

void pen_down() {
  delay(200);
  penServo.write(90);
}

void draw(int change_x, int change_y){
  pen_down();
  steps_new = turns_to_dist(current_x, current_y, change_x, change_y, 40, 625, 3200);
  x_steps_new = steps_new[1];
  y_steps_new = steps_new[0];
  stepper_speed_ratio(x_steps_new, y_steps_new, 35);
  current_x = current_x + change_x;
  current_y = current_y + change_y;
  pen_up();
}

void move_pen(int change_x, int change_y){
  pen_up(); 
  steps_new = turns_to_dist(current_x, current_y, change_x, change_y, 40, 625, 3200);
  x_steps_new = steps_new[1];
  y_steps_new = steps_new[0];
  stepper_speed_ratio(x_steps_new, y_steps_new, 35);
  current_x = current_x + change_x;
  current_y = current_y + change_y;
}

void draw_TTC_grid() {
  move_pen(0, -200);
  move_pen(-30, 0);
  draw(150, 0);
  move_pen(0,-100);
}

void line_down_center() {
  digitalWrite(X_DIR_PIN, HIGH);
  digitalWrite(Y_DIR_PIN, HIGH);
  for (int n = 0; n < 3200; n++) {
    digitalWrite(X_STEP_PIN, HIGH);
    digitalWrite(Y_STEP_PIN, HIGH);
    delayMicroseconds(300);
    digitalWrite(X_STEP_PIN, LOW);
    digitalWrite(Y_STEP_PIN, LOW);
    delayMicroseconds(300);
  }
}

void single_step_x(){
  if(x_steps_new < 0){
    digitalWrite(X_DIR_PIN, HIGH);
  } else {
    digitalWrite(X_DIR_PIN, LOW);
  };
    digitalWrite(X_STEP_PIN, HIGH);
    delayMicroseconds(300);
    digitalWrite(X_STEP_PIN, LOW);
    delayMicroseconds(300);
}

void single_step_y(){
  if(y_steps_new < 0){
    digitalWrite(Y_DIR_PIN, LOW);
  } else {
    digitalWrite(Y_DIR_PIN, HIGH);
  };
    digitalWrite(Y_STEP_PIN, HIGH);
    delayMicroseconds(300);
    digitalWrite(Y_STEP_PIN, LOW);
    delayMicroseconds(300);
}

void single_step_z_cw(){
    digitalWrite(Y_DIR_PIN, LOW);
    
    digitalWrite(Z_STEP_PIN, HIGH);
    delayMicroseconds(400);
    digitalWrite(Z_STEP_PIN, LOW);
    delayMicroseconds(400);
}

void single_wipe() {
  digitalWrite(X_DIR_PIN, HIGH);
  for (int i = 0; i < 6; i++) {
    for (int n = 0; n < 3200; n++) {
      single_step_z_cw();
    }
  }
}

void lcd_start_message_scrolling() {
  int count = 0;
  lcd.clear();
  lcd.setCursor(6,0);
  lcd.print("PL.AI");
  lcd.setCursor(16,0);
  lcd.print("PL.AI");
  while (true){
    lcd.setCursor(1,1);
    lcd.print("Interactive Tic-Tac-Toe");
    lcd.scrollDisplayLeft();
    delay(1000);
  };
}

void lcd_start_message() {
  int count = 0;
  lcd.clear();
  lcd.setCursor(6,0);
  lcd.print("PL.AI");
  lcd.setCursor(2,1);
  lcd.print("Tic-Tac-Toe");
}

void lcd_display_winner() {

}

void led_strip_countdown() {

}

//converts xy distance change to change in motor angles
//for changes in motor distances the two motors should move at different speeds but complete the movement in the same period of time
//returns number of steps required for each motor
// positive for left (X) == clockwise
// positive for right (Y) = anticlockwise
int * turns_to_dist(int start_x, int start_y, int x_dist_delta, int y_dist_delta, int pulley_diameter, int motor_gap, int steps_per_rot){
    static int rotations[2];
    //left motor
    double start_belt_left = sqrt(pow(start_x, 2) + pow(start_y, 2));
    double start_belt_right = sqrt(pow(motor_gap - start_x, 2) + pow(start_y, 2));
    double delta_belt_dist_left = sqrt(pow(x_dist_delta + start_x, 2) + pow(y_dist_delta + start_y, 2)) - start_belt_left;

    double delta_belt_dist_right = sqrt(pow(motor_gap - (x_dist_delta + start_x), 2) + pow(y_dist_delta + start_y, 2)) - start_belt_right;

    int rot_left = steps_per_rot * (delta_belt_dist_left) / pulley_diameter;
    //right motor
    int rot_right = steps_per_rot * (delta_belt_dist_right) / pulley_diameter;
    rotations[0] = rot_left;
    rotations[1] = rot_right;
    return rotations;
}

//accepts a number of steps for each motor and smooths the pen movement by running both steppers concurrently. Only accepts positive numbers of steps for each motor
void stepper_speed_ratio(int x_steps, int y_steps, int steps_per) {
    x_steps = abs(x_steps);
    y_steps = abs(y_steps);
    int larger = (x_steps > y_steps) ? x_steps : y_steps;
    bool x_is_larger = x_steps == larger;
    double ratio = (x_is_larger) ? (double(y_steps) / larger) : (double(x_steps) / larger);

    int int_percent_ratio = ratio * steps_per;
    int x_completed = 0;
    int y_completed = 0;

    // printf("ratio percent: %i\n", int_percent_ratio);
    // printf("larger: %d\n", larger);
    // printf("x_is_larger: %d\n", x_is_larger);

    int x_increment = (x_is_larger) ? steps_per : int_percent_ratio;
    int y_increment = (x_is_larger) ? int_percent_ratio : steps_per;
    for(int i = 0; i < larger - steps_per; i += steps_per) {
        //increment x
        for(int j = 0; j < x_increment; j++) {
            x_completed++;
            single_step_x();
        }
        //increment y
        for(int j = 0; j < y_increment; j++) {
            y_completed++;
            single_step_y();
        }

    }
    int x_adj = x_steps - x_completed;
    int y_adj = y_steps - y_completed;

    //adjustment to x and y for any that have not been completed
    for(int i = 0; i < x_adj; i++) {
        x_completed++;
        single_step_x();
    }

    for(int i = 0; i < y_adj; i++) {
        y_completed++;
        single_step_y();
    }
} 
