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
# - The potentiometer calibration values do not reflect the range that the rotary 
#   potentiometer moves while connected to the linkage. They are known angles 
#   with reference to the bike so that voltages read while logging can be 
#   converted to a fork travel distance. The real angles of the pot will fall 
#   somewhere within the reference angles. 0 degrees is defined as an axis 
#   starting at the crown linkage mounting point and going downwards parallel to 
#   the fork travel direction. The potentiometer is assumed to provide a linear 
#   relationship between angle and voltage read. 

# Linkage dimensions 
linkage_1_len = 114.3     # Length of linkage piece 1 
linkage_2_len = 114.3     # Length of linkage piece 2 
linkage_offset_x = 25.4   # Distance between crown and arch linkage mounting 
                          # points perpendicular to fork travel direction. 
linkage_offset_y = 25.4   # Distance between crown and arch linkage mounting 
                          # points parrallel to for travel direction (excluding 
                          # travel) 

# Fork data 
fork_travel = 163         # Total distance the fork can travel 

# Pot calibration data (see notes above) 
pot_min_angle = 0.0       # Minimum potentiometer reference angle 
pot_max_angle = 180.0     # Maximum potentiometer reference angle 
pot_angle_delta = pot_max_angle - pot_min_angle
pot_min_voltage = 1.0     # Potentiometer voltage at its minimum angle 
pot_max_voltage = 3.0     # Potentiometer voltage at its maximum angle 
pot_voltage_delta = pot_max_voltage - pot_min_voltage 

#================================================================================


#================================================================================
# Test data 

# This data is used for testing the calculations. They're not used during the 
# data log analysis. 

# Fork travel & potentiometer angle data points 
fork_travel_data = range(0, fork_travel + 1) 
pot_voltage_data = [None] * (fork_travel + 1)

#================================================================================


#================================================================================
# Calculation functions 

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
# brief: Potentiometer angle to voltage conversion 
##
def pot_theta_to_volt(theta): 
    return theta * pot_voltage_delta / pot_angle_delta + pot_min_voltage 


##
# brief: Potentiometer voltage to angle conversion 
##
def pot_volt_to_theta(voltage): 
    return voltage * pot_angle_delta / pot_voltage_delta + pot_min_angle 

#================================================================================


#================================================================================
# Test functions - For internal use 

##
# brief: Converts a fork travel distance to a potentiometer angle 
##
def pot_angle_calc(travel): 
    ab = ab_distance(fork_travel + linkage_offset_y - travel) 
    theta = ab_right_angle(ab) + a_cosine_angle(ab) 
    return pot_theta_to_volt(theta) 

#================================================================================


#================================================================================
# User functions - Include these in other scripts as needed 

##
# brief: Converts a potentiometer voltage to a fork travel distance 
##
def fork_travel_calc(voltage): 
    print(1) 

#================================================================================


#================================================================================
# Test Calculation 

# Populate the potentiometer angle buffer 
for y in fork_travel_data: 
    pot_voltage_data[y] = pot_angle_calc(y) 

# Plot the potentiometer angle against the fork travel 
fig, ax = plt.subplots() 
ax.plot(fork_travel_data, pot_voltage_data) 
ax.set_xlabel("Fork Travel (mm)") 
ax.set_ylabel("Fork Pot Angle (degrees)") 
ax.set_title("Fork Travel to Potentiometer Angle Conversion")
plt.show() 

#================================================================================
