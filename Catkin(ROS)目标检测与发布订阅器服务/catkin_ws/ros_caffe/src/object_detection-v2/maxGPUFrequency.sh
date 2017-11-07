#!/bin/bash
sudo cat /sys/kernel/debug/clock/gbus/possible_rates
echo 852000000 | sudo tee /sys/kernel/debug/clock/override.gbus/rate
echo 1 | sudo tee /sys/kernel/debug/clock/override.gbus/state
sudo cat /sys/kernel/debug/clock/gbus/rate
