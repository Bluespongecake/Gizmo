# Gizmo
DE2 Gizmo


3 scripts are used in the set-up and running of PL.AI. 
The Arduino script controls all the hardware: kinematics, inputs and outputs. 
The Python calibration script find the co-ordinates of the top-left element of the 3*3 grid, based on the camera angle and board size. 
The Python TTC script uses OpenCV to recognise circles and dictates the computer’s moves. Based on the gameboard’s state, it sends an integer to the serial port, which instructs the Arduino to execute certain functions and commands to draw the new state of the game boards/display the winner and update “emotion” through the led array 

See PL_AI.ino for arduino script

See PL_AI_TTC.py for tic tac toe API and computer vision
