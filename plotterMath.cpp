#include <math.h>       /* pow */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>


/* returns the lengths of the two lines from input of x y coords in mm */
/*accurate to the nearest mm*/
/* coords (0,0) are top left of board*/
int * bipolarToCartesian(int motor_gap, int x_coord, int y_coord) {
    static int lens[2];
    /* length of left string*/
    int len_l = sqrt(pow(x_coord, 2) + pow(y_coord, 2));

    /* length of right string*/
    int len_r = sqrt(pow(motor_gap - x_coord, 2) + pow(y_coord, 2));

    return lens;
}

/* TODO*/
/* returns x and y coords for given belt lengths*/
int * CartesianToBipolar(int points_width, int l_len, int r_len) {

    /* length of left string*/
    int x = 1;

    /* length of right string*/
    int y = 1;

    return 0;
}

/* only accepts horizontal or vertical lines rn*/
int * drawLine(int start_point, int end_point) {
    return 0;
}

//converts xy distance change to change in motor angles
//for changes in motor distances the two motors should move at different speeds but complete the movement in the same period of time
//returns number of steps required for each motor
// positive for l == clockwise
// positive for r = anticlockwise
int * turns_to_dist(int start_x, int start_y, int x_dist_delta, int y_dist_delta, int pulley_diameter, int motor_gap, int steps_per_rot){
    static int rotations[2];
    /*left motor*/
    double start_belt_left = sqrt(pow(start_x, 2) + pow(start_y, 2));
    double start_belt_right = sqrt(pow(motor_gap - start_x, 2) + pow(start_y, 2));
    double delta_belt_dist_left = sqrt(pow(x_dist_delta + start_x, 2) + pow(y_dist_delta + start_y, 2)) - start_belt_left;

    double delta_belt_dist_right = sqrt(pow(motor_gap - (x_dist_delta + start_x), 2) + pow(y_dist_delta + start_y, 2)) - start_belt_right;

    int rot_left = steps_per_rot * (delta_belt_dist_left) / pulley_diameter;
    /*right motor*/
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
            // single_step_x();
        }
        //increment y
        for(int j = 0; j < y_increment; j++) {
            y_completed++;
            // single_step_y();
        }

    }
    int x_adj = x_steps - x_completed;
    int y_adj = y_steps - y_completed;

    //adjustment to x and y for any that have not been completed
    for(int i = 0; i < x_adj; i++) {
        x_completed++;
        // single_step_x();
    }

    for(int i = 0; i < y_adj; i++) {
        y_completed++;
        // single_step_y();
    }

}

int main(){
    int *rots;
    rots = turns_to_dist(265, 125, 0, 400, 40, 625, 3200);
    printf("rot_left   = %d\n", rots[0]);
    printf("rot_right   = %d\n", rots[1]);

    stepper_speed_ratio(400, 400, 10);

    return 0;
}

