#!/bin/bash

gpio mode 1 pwm
gpio pwm-ms
gpio pwmc 1920
gpio pwmr 200     # 0.1 ms per unit
gpio pwm 1 90
