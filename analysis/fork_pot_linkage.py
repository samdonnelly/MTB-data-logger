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
import numpy as np 

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
# - The fork travel data resolution controls the number of data points used to 
#   generate the relationship between fork travel distance and potentiometer 
#   voltage. A higher resolution (smaller number) will produce more accurate 
#   conversions. This value should be in the range of: 0 < res <= 1. 

# Linkage dimensions 
linkage_1_len = 114.3     # Length of linkage piece 1 
linkage_2_len = 114.3     # Length of linkage piece 2 
linkage_offset_x = 25.4   # Distance between crown and arch linkage mounting 
                          # points perpendicular to fork travel direction. 
linkage_offset_y = 25.4   # Distance between crown and arch linkage mounting 
                          # points parrallel to fork travel direction (excluding 
                          # fork travel) 

# Fork data 
fork_travel = 163         # Total distance the fork can travel 
fork_travel_res = 0.25    # Fork travel data resolution 
travel_points = int(fork_travel / fork_travel_res + 1) 

# Pot calibration data (see notes above) 
pot_min_angle = 0.0       # Minimum potentiometer reference angle 
pot_max_angle = 180.0     # Maximum potentiometer reference angle 
pot_angle_delta = pot_max_angle - pot_min_angle
pot_min_voltage = 1.0     # Potentiometer voltage at its minimum ref angle 
pot_max_voltage = 3.0     # Potentiometer voltage at its maximum ref angle 
pot_voltage_delta = pot_max_voltage - pot_min_voltage 

# ADC/DAC conversion info 
adc_resolution = 10       # Potentiometer ADC resolution (bits) 
adc_max_volt = 3.3        # Maximum voltage for ADC 
adc_max_volt_dig = 0      # Maximum digital voltage for ADC (set during init) 

#================================================================================


#================================================================================
# Test data - for internal use only 

# This data is used for testing the calculations. They're not used during the 
# data log analysis. 

# Fork travel & potentiometer angle data points 
fork_travel_data = np.linspace(0, fork_travel, travel_points) 
pot_voltage_data = [None] * travel_points 

#================================================================================


#================================================================================
# Calculation functions - for internal use only 

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


##
# brief: Converts a fork travel distance to a potentiometer voltage reading 
##
def pot_voltage_calc(travel): 
    ab = ab_distance(fork_travel + linkage_offset_y - travel) 
    theta = ab_right_angle(ab) + a_cosine_angle(ab) 
    return pot_theta_to_volt(theta) 


##
# brief: Converts a digital voltage integer to analog float 
##
def voltage_dac(voltage_digital): 
    return voltage_digital * adc_max_volt / adc_max_volt_dig 


##
# brief: Converts an analog voltage float to a digital integer 
##
def voltage_adc(voltage_analog): 
    return voltage_analog * adc_max_volt_dig / adc_max_volt 

#================================================================================


#================================================================================
# Test functions - for internal use only 

##
# brief: User interface to test the calculations 
##
def user_pot_calc_test(): 
    while (True): 
        user_input = input("Travel: ") 

        # Make sure the input is a number. If not then exit the user interface. 
        try: 
            test_travel = float(user_input) 
        except: 
            break 
        
        # Bound the input 
        if (test_travel > fork_travel): 
            test_travel = fork_travel 
        elif (test_travel < 0): 
            test_travel = 0 

        # Find the closest pre-defined fork travel distance based on the input. 
        for y in range(travel_points): 
            if (fork_travel_data[y] >= test_travel): 
                index = y 
                break 
        
        if (index != 0): 
            diff_low = test_travel - fork_travel_data[index - 1] 
            diff_high = fork_travel_data[index] - test_travel 

            if (diff_low < diff_high): 
                index = index - 1 
        
        test_voltage = pot_voltage_data[index] 
        calc_travel = fork_travel_calc(voltage_adc(test_voltage)) 

        # Show the user the results 
        print("Test travel: " + str(fork_travel_data[index])) 
        print("Test voltage: " + str(test_voltage)) 
        print("Calc travel: " + str(calc_travel) + "\r\n") 

#================================================================================


#================================================================================
# User functions - Include these in other scripts as needed 

##
# brief: Initialize data so conversions can be done properly 
# 
# description: Must be called before the 'fork_travel_calc' function can be used 
#              properly. 
##
def fork_pot_init(): 
    # Generate a relationship between the fork travel and the potentiometer 
    # voltage. This is used to convert voltages to travel distances. 
    for y in range(travel_points): 
        pot_voltage_data[y] = pot_voltage_calc(fork_travel_data[y]) 

    # Creates the upper limit of the digital voltage value based on the ADC 
    # resolution set. This is needed to convert the digital value from log files 
    # to floating values used for travel calculations. 
    global adc_max_volt_dig
    for i in range(adc_resolution): 
        adc_max_volt_dig += (1 << i) 


##
# brief: Converts a potentiometer voltage to a fork travel distance 
# 
# description: A binary search is performed on the 'pot_voltage_data' buffer 
#              using a potentiometer voltage supplied to the function to find 
#              the fork travel distance that most closely represents the 
#              provided voltage. 
#              
#              Increasing the fork travel data resolution in the constants 
#              defined at the top of the script will give a more accurate 
#              answer. Interpolation is not done for voltages that lie between 
#              data points simply because the resolution can be changed instead. 
# 
# param : voltage : digital (integer) voltage reading from data log file 
##
def fork_travel_calc(voltage): 
    # Convert the digital voltage value (integer) to a floating "analog" value. 
    # Data log files save a 0-3.3V reading as a 6, 8, 10 or 12-bit integer 
    # depending on the resolution of the conversion. Make sure the ADC 
    # resolution defined in this script matches what is put in the log files. 
    voltage = voltage_dac(voltage) 

    # Set the initial bounds of the data to check the voltage against. 
    travel_max = travel_points 
    travel_min = 0 

    # Bound the voltage. There will likely be noise in the voltage reading that 
    # exceeds the voltage limits for the given linkage geometry. If that is the 
    # case then return the bound of the data. 
    if (voltage <= pot_voltage_data[travel_min]): 
        return fork_travel_data[travel_min] 
    elif (voltage >= pot_voltage_data[travel_max - 1]): 
        return fork_travel_data[travel_max - 1] 

    # Binary search for the closest result to the provided voltage. 
    while (True): 
        travel_guess = int((travel_max - travel_min) / 2 + travel_min) 

        # Convergance on a solution is checked after 'travel_guess' is updated 
        # so that the correct value it returned. 
        if (travel_min >= travel_max): 
            break 

        voltage_guess = pot_voltage_data[travel_guess] 

        if (voltage < voltage_guess): 
            travel_max = travel_guess - 1 
        elif (voltage > voltage_guess): 
            travel_min = travel_guess + 1 
        else: 
            break 

    return fork_travel_data[travel_guess] 

#================================================================================


#================================================================================
# Test Calculation 

# Initialize data so conversions can be done 
fork_pot_init() 

# User interface to test the calculations 
user_pot_calc_test() 

# Plot the potentiometer voltage against the fork travel 
fig, ax = plt.subplots() 
ax.plot(fork_travel_data, pot_voltage_data) 
ax.set_xlabel("Fork Travel (mm)") 
ax.set_ylabel("Fork Pot Voltage (V)") 
ax.set_title("Fork Travel <--> Potentiometer Voltage")
plt.show() 

#================================================================================
