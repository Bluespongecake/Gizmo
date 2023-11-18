# Gizmo
### DE2 Gizmo

*By Max McCormack and Navyansh Malhotra*


**Code for a machine that plays tic-tac-toe against the user on a whiteboard.**

Submitted as our DE2 Physical Computing final project submission.



3 scripts are used in the set-up and running of PL.AI. 
1. The Arduino script `PL.AI.ino` controls all the hardware: kinematics, inputs and outputs. 
2. The Python calibration script `regions.py` finds the co-ordinates of the top-left element of the 3*3 grid, based on the camera angle and board size. 
3. The Python TTC script `PL_AI_TTC.py` uses OpenCV to recognise circles and dictates the computer’s moves. Based on the gameboard’s state, it sends an integer to the serial port, which instructs the Arduino to execute certain functions and commands to draw the new state of the game boards/display the winner and update *“emotion”* through the led array.


