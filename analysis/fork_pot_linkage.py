#================================================================================
# File: fork_pot_linkage.py 
# 
# Author: Sam Donnelly (samueldonnelly11@gmail.com)
# 
# Description: Position calculation for the potentiometer that indicates fork 
#              travel. The rotary potentiometer is attached to the crown of the 
#              bike fork and it gets rotated by a linkage connected to the fork 
#              arch. Using the dimensions of this mechanical design, we can take 
#              fork travel and determine the rotation of the potentiometer at 
#              any given distance. This relationship can then be used to convert 
#              potentiometer voltage to a fork travel distance in data logging 
#              post processing / analysis. 
# 
# Date: September 20, 2024 
#================================================================================


#================================================================================
# Includes 

import math 
import matplotlib.pyplot as plt 

#================================================================================


#================================================================================
# Constants 

# Notes: 
# - Distances are in millimeters (mm) 
# - These constants should be updated to reflect the bike geometry and mechanical 
#   design. 

linkage_1_len = 114.3     # Length of linkage piece 1 
linkage_2_len = 114.3     # Length of linkage piece 2 
linkage_offset_x = 25.4   # Distance between crown and arch linkage mounting 
                          # points perpendicular to fork travel direction. 
linkage_offset_y = 25.4   # Distance between crown and arch linkage mounting 
                          # points parrallel to for travel direction (excluding 
                          # travel) 
fork_travel = 163         # Total distance the fork can travel 
pot_0deg_voltage = 1.0    # Potentiometer voltage at 0 degree linkage angle 
pot_180deg_voltage = 3.0  # Potentiometer voltage at 180 degree linkage angle 

#================================================================================


#================================================================================
# Test data 

# This data is used for testing the calculations. They're not used during the 
# data log analysis. 

# Fork travel & potentiometer angle data points 
fork_travel_data = range(0, fork_travel + 1) 
pot_angle_data = [None] * (fork_travel + 1)

#================================================================================


#================================================================================
# Functions 

##
# brief: Distance between linkage mounting points. 
##
def ab_distance(travel): 
    return math.sqrt(pow(linkage_offset_x, 2) + pow(travel, 2)) 


##
# brief: Angle between linkage mounting points if the points were on a right 
#        angle triangle. 
##
def ab_right_angle(travel): 
    return math.degrees(math.atan(linkage_offset_x / travel)) 


##
# brief: Angle at the linkage crown mounting point formed between the three 
#        pivot points of the linkage (cosine law). 
##
def a_cosine_angle(ab): 
    numerator = pow(linkage_1_len, 2) + pow(ab, 2) - pow(linkage_2_len, 2) 
    denominator = 2 * linkage_1_len * ab 
    return math.degrees(math.acos(numerator / denominator)) 


##
# brief: Converts a fork travel distance to a potentiometer angle 
##
def pot_angle_calc(travel): 
    ab = ab_distance(fork_travel + linkage_offset_y - travel) 
    return ab_right_angle(ab) + a_cosine_angle(ab) 


##
# brief: Converts a potentiometer voltage to a fork travel distance 
##
def fork_travel_calc(voltage): 
    print(1) 

#================================================================================


#================================================================================
# Calculation 

# Populate the potentiometer angle buffer 
for y in fork_travel_data: 
    pot_angle_data[y] = pot_angle_calc(y) 

# Plot the potentiometer angle against the fork travel 
fig, ax = plt.subplots() 
ax.plot(fork_travel_data, pot_angle_data) 
ax.set_xlabel("Fork Travel (mm)") 
ax.set_ylabel("Fork Pot Angle (degrees)") 
ax.set_title("Fork Travel to Potentiometer Angle Conversion")
plt.show() 

#================================================================================
