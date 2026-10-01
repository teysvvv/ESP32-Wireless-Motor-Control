# ESP32-Wireless-Motor-Control
# Project Overview
This project involves developing a wireless control and power supply system for a manual slope-climbing robot designed to travel from one edge of an inclined surface to the other. 

The system uses two **ESP32-DevKitC-32UE**. The first ESP32 is connected to a Funduino joystick shield, which collects user inputs and transmits them wirelessly through ESP-NOW. The second ESP32, mounted on the robot, receives these commands and communicates with the motor controllers through CAN using an MCP2515 module.

The robot is powered by a **4S battery**. To provide a regulated power supply for the ESP32, I designed a custom PCB incorporating two voltage regulation stages:

- **LM2596S-5:** Steps down the battery voltage to a regulated 5 V.
- **AMS1117-3.3:** Further regulates the 5 V supply to a stable 3.3 V for the ESP32.

## System Architecture

1. **Transmitter:** The first ESP32 reads joystick and button inputs through the Funduino joystick shield.
2. **Wireless Communication:** ESP-NOW transmits control data between the two ESP32 boards.
3. **Receiver:** The second ESP32 receives the commands and communicates with the motor controllers through the MCP2515 CAN module.
4. **Motor Control:**
   - Motor 1: Speed control with encoder feedback and position holding.
   - Motor 2: Steering position control.

## Software and Tools

- C++ / Arduino IDE
- ESP-NOW
- CAN communication
- KiCad

## Project Status

Under development and hardware testing.
