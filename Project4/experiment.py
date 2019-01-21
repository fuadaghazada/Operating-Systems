#
#   Simple test program for Disk Scheduling Simulation (CS342 - Project 4)
#
#   @author: Fuad Aghazada
#   @date: 27/12/2018

import os
import random
import math

NUM_EXPERIMENT = 100
OUT_FILENAME = 'result.txt'

fcfs = []
sstf = []
scan = []
c_scan = []
look = []
c_look = []

# Runs the experiment
def run_exp():
    head_pos = random.randint(0, 5000)
    os.system('./diskschedule ' + str(head_pos))

    file = open(OUT_FILENAME, 'r')

    line = file.readline().replace(' ', '')
    fcfs.append(int(line.split(':')[1]))

    line = file.readline().replace(' ', '')
    sstf.append(int(line.split(':')[1]))

    line = file.readline().replace(' ', '')
    scan.append(int(line.split(':')[1]))

    line = file.readline().replace(' ', '')
    c_scan.append(int(line.split(':')[1]))

    line = file.readline().replace(' ', '')
    look.append(int(line.split(':')[1]))

    line = file.readline().replace(' ', '')
    c_look.append(int(line.split(':')[1]))

    file.close()

# Reporting the statistical data from experiment
def stats():
    file = open('report.txt', 'w')

    file.write("--FCFS--\n")
    file.write("Mean: " + str(calc_mean(fcfs)) + "\n")
    file.write("SD: " + str(calc_sd(fcfs)) + "\n\n")

    file.write("--SSTF--\n")
    file.write("Mean: " + str(calc_mean(sstf)) + "\n")
    file.write("SD: " + str(calc_sd(sstf)) + "\n\n")

    file.write("--SCAN--\n")
    file.write("Mean: " + str(calc_mean(scan)) + "\n")
    file.write("SD: " + str(calc_sd(scan)) + "\n\n")

    file.write("--C-SCAN--\n")
    file.write("Mean: " + str(calc_mean(c_scan)) + "\n")
    file.write("SD: " + str(calc_sd(c_scan)) + "\n\n")

    file.write("--LOOK--\n")
    file.write("Mean: " + str(calc_mean(look)) + "\n")
    file.write("SD: " + str(calc_sd(look)) + "\n\n")

    file.write("--C-LOOK--\n")
    file.write("Mean: " + str(calc_mean(c_look)) + "\n")
    file.write("SD: " + str(calc_sd(c_look)) + "\n\n")

    file.close()

# Calculates Mean (Average) of given numbers
def calc_mean(nums):
    return round(float(sum(nums)) / len(nums), 2)

# Calculates Standart Deviation of given numbers
def calc_sd(nums):

    sd = 0.0
    mean = calc_mean(nums)

    for num in nums:
        sd += ((num - mean)**2)

    return round(math.sqrt(float(sd) / len(nums)), 2)

# ------------------------

# Test
os.system("make clean")
os.system("make")

for i in range(0, NUM_EXPERIMENT):
    run_exp()

# Writing stats
stats()

os.system("make clean")
