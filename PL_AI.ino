//Libraries to include
#include <Servo.h>
#include <Wire.h>                          // I2C libraries
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

//Ultrasonic (HC-SR04) pin declerations
#define echoPin1 16
#define trigPin1 17
#define echoPin2 23
#define trigPin2 25
#define echoPin3 27
#define trigPin3 29

//Store current co-ordinates as global variables
int current_x;
int current_y;

//Duration of sound wave travel 
//1 -> Top 
//3 -> Bottom 

//Store steps to be made 
int x_steps_to_be_made;
int y_steps_to_be_made;
int * steps_to_be_made;


//Initialising objects
Servo penServo;
hd44780_I2Cexp lcd;

void setup() {
  Serial.begin(9600);

  // set pinmodes for steppers
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
  //Start "home" at (265,125);
  current_x = 180;
  current_y = 150;

  //TEST CODE - SINGLE RUN
  draw_TTC_grid(); 

  //wipe_board();
  
 
}

void loop() {
  // put your main code here, to run repeatedly:

}

//method to lift pen from board
void pen_up() {
  delay(200);
  penServo.write(0);
}

// method to contact pen with board
void pen_down() {
  delay(200);
  penServo.write(90);
}

// method to draw a line between two cartesian coordinates
void draw(int change_x, int change_y){
  lcd_drawing_message();
  pen_down();
  steps_to_be_made = turns_to_dist(current_x, current_y, change_x, change_y, 40, 625, 3200);
  x_steps_to_be_made = steps_to_be_made[1];
  y_steps_to_be_made = steps_to_be_made[0];
  stepper_speed_ratio(x_steps_to_be_made, y_steps_to_be_made, 40);
  current_x = current_x + change_x;
  current_y = current_y + change_y;
  pen_up();
  lcd_start_message();
  
  Serial.print("moving to pos: (");
  Serial.print(current_x);
  Serial.print(" ");
  Serial.println(current_y);
}

// method to move pen from one cartesian coordinate to another
void move_pen(int change_x, int change_y){
  lcd_drawing_message();
  pen_up(); 
  steps_to_be_made = turns_to_dist(current_x, current_y, change_x, change_y, 40, 625, 3200);
  x_steps_to_be_made = steps_to_be_made[1];
  y_steps_to_be_made = steps_to_be_made[0];
  stepper_speed_ratio(x_steps_to_be_made, y_steps_to_be_made, 40);
  current_x = current_x + change_x;
  current_y = current_y + change_y;
  lcd_start_message(); 

  Serial.print("moving to pos: (");
  Serial.print(current_x);
  Serial.print(" ");
  Serial.println(current_y);
}

void draw_TTC_grid() {
  move_pen(60,280);
  draw(0,-200); 
  move_pen(85,0);
  draw(0,200); 
  move_pen(75, -70);
  draw(-220,0);
  move_pen(0, -75);
  draw(220,0);
  return_home(); 
}

// test method
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

//run L motor 1 step
void single_step_x(){
    digitalWrite(X_STEP_PIN, HIGH);
    delayMicroseconds(200);
    digitalWrite(X_STEP_PIN, LOW);
    delayMicroseconds(200);
}

// run R motor 1 step
void single_step_y(){

    digitalWrite(Y_STEP_PIN, HIGH);
    delayMicroseconds(200);
    digitalWrite(Y_STEP_PIN, LOW);
    delayMicroseconds(200);
}

//wiper motor
void single_step_z_cw(){
    digitalWrite(Z_STEP_PIN, HIGH);
    delayMicroseconds(150);
    digitalWrite(Z_STEP_PIN, LOW);
    delayMicroseconds(150);
}

void single_wipe() {
  lcd_wiping_message();
  digitalWrite(Z_DIR_PIN, HIGH);
  for (int i = 0; i < 6; i++) {
    for (int n = 0; n < 3200; n++) {
      single_step_z_cw();
    }
  }
  lcd_start_message();
}

//Run single_wipe five times 
void wipe_board(){
  for (int i = 0; i < 5; i++){
    single_wipe();
  };
}

void return_home(){
  move_pen(180-current_x,150-current_y);
}

// display scrolling message on LCD
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

// display "PL.AI - Tic-Tac-Toe" on LCD
void lcd_start_message() {
  lcd.clear();
  lcd.setCursor(6,0);
  lcd.print("PL.AI");
  lcd.setCursor(3  ,1);
  lcd.print("Tic-Tac-Toe");
}

// display "DRAWING" on LCD
void lcd_drawing_message() {
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("DRAWING");
}

// display "WIPING" on LCD
void lcd_wiping_message() {
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("WIPING");
}

void lcd_user_turn() {
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("YOUR TURN");
}

// self explanatory
void lcd_display_winner() {

}

// start the game
void gesture_start (){
  
}

//end the game
void gesture_end (){
  
}

// converts xy distance change to change in motor angles
// for changes in motor distances the two motors should move at different speeds but complete the movement in the same period of time
// returns number of steps required for each motor
// positive (extension) for left (X) == clockwise
// positive (extension) for right (Y) = anticlockwise
int * turns_to_dist(int start_x, int start_y, int x_dist_delta, int y_dist_delta, int pulley_diameter, int motor_gap, int steps_per_rot){
    static int rotations[2];
    // starting lengths for belts
    double start_belt_left = sqrt(pow(start_x, 2) + pow(start_y, 2));
    double start_belt_right = sqrt(pow(motor_gap - start_x, 2) + pow(start_y, 2));

    // changes to belt lengths to be computed
    double delta_belt_dist_left = sqrt(pow(x_dist_delta + start_x, 2) + pow(y_dist_delta + start_y, 2)) - start_belt_left;
    double delta_belt_dist_right = sqrt(pow(motor_gap - (x_dist_delta + start_x), 2) + pow(y_dist_delta + start_y, 2)) - start_belt_right;

    // steps to rotate each motor (scalar quantities)
    int rot_left = steps_per_rot * (delta_belt_dist_left) / pulley_diameter;
    int rot_right = steps_per_rot * (delta_belt_dist_right) / pulley_diameter;
    
    rotations[1] = rot_left;
    Serial.print("L: ");
    Serial.println(rot_left);
    rotations[0] = rot_right;
    Serial.print("R: ");
    Serial.println(rot_right);

    // determine whether to rotate each motor cw or ccw
    if (rot_left < 0){
          digitalWrite(X_DIR_PIN, LOW);
          Serial.println("left: ccw");
    } else {
          digitalWrite(X_DIR_PIN, HIGH);
          Serial.println("left: cw");

    };
    if (rot_right < 0){
          Serial.println("right: cw");
          digitalWrite(Y_DIR_PIN, HIGH);
    } else {
          digitalWrite(Y_DIR_PIN, LOW);
          Serial.println("right: ccw");

    };
    return rotations;
}

//accepts a number of steps for each motor and smooths the pen movement by running both steppers concurrently. Only accepts positive numbers of steps for each motor
void stepper_speed_ratio(int x_steps, int y_steps, int steps_per) {
    
    //ensure both values are positive
    x_steps = abs(x_steps);
    y_steps = abs(y_steps);

    //determine ratio between steps for each motor
    int larger = (x_steps > y_steps) ? x_steps : y_steps;
    bool x_is_larger = x_steps == larger;
    double ratio = (x_is_larger) ? (double(y_steps) / larger) : (double(x_steps) / larger);
    int int_percent_ratio = ratio * steps_per;

    //store number of steps completed for each motor
    int x_completed = 0;
    int y_completed = 0;

    // printf("ratio percent: %i\n", int_percent_ratio);
    // printf("larger: %d\n", larger);
    // printf("x_is_larger: %d\n", x_is_larger);

    // determine how many steps to increment each motor per cycle
    int x_increment = (x_is_larger) ? steps_per : int_percent_ratio;
    int y_increment = (x_is_larger) ? int_percent_ratio : steps_per;

    //run cycle
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

    //adjustment to x and y for any that have not been completed
    int x_adj = x_steps - x_completed;
    int y_adj = y_steps - y_completed;

    for(int i = 0; i < x_adj; i++) {
        x_completed++;
        single_step_x();
    }
    for(int i = 0; i < y_adj; i++) {
        y_completed++;
        single_step_y();
    }
    Serial.print("X completed: ");
    Serial.println(x_completed);
    Serial.print("Y completed: ");
    Serial.println(y_completed);
} 