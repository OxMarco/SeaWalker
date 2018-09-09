# Automatic Boat Pilot system
Developed at Åland University of Applied Sciences, some of their code has been readapted. At the moment only the basic hardware classes have been created

## Idea ##

The idea is to build a small vessel capable of advanced autonomous navigation (on a given path), path optimisation and obstacle avoidance, being at the same time completely independent from fuel thanks to renewable energy sources, only relying on solar panels and thermoelectric generators.
The displacement of the boat is around 2m, with a bulb at the bottom containing LiFePo batteries.

On the electronics side, tasks are split into several units, made of microcontrollers running a RTOS for rapid and time-constrained reply and a Single Board Computer with Arch Linux.
Up to now the units identified are:

 1. *High level navigation unit*
 2. *Low level navigation unit*
 3. *Power unit*
 4. *Engine unit*

The **High level navigation unit** performs the following tasks:

 - Path planning: global route calculation, considering shorelines, high-traffic routes and sea bathymetry to avoid groundings;
 - Obstacle Avoidance: using a forward-looking sonar for underwater obstacles, AIS for boats, a thermal imaging camera and maybe a small radar for other types of objects;
 - Local navigation: optimise the path taking into account the general predefined path (from the path planner), use Inertial Navigation and dead reckoning in case of GPS failure.
 - Local System Management: put sensors in sleep mode when they're not needed or in case of power shortages.

The **Low level navigation unit** is responsible for:

 - Local navigation
 - Local System Management
 - Communication: transmit and receive messages via radio link or satellite;

The **Power unit** contains a MPPT solar charger and a battery monitor and is responsible for:
 - Charging and powering the units;
 - Sensing the battery and informing other units on the charge status;
 - Disabling units to go in low power mode (one after the other).

The last unit is simply an interface to the engines, basically it's an ESC connected to the other units.

The backbone of the boat is the well-established CAN Bus running NMEA2000 since almost all marine sensors use this protocol, and both the SBC and the uC would share messages via CanBus.
Locally, the SBC will have a CanBus interface and a MessageBus where threads responsible for the aforementioned tasks will stream messages (inter-thread) and eventually publish frames on the Can Bus to interface with sensors (say put the Sonar in sleep mode) or the other units.
Ideally in case of failure the boat should be able to navigate with just one of the two navigation units, obviously losing some functionalities.

### Continuous integration ###
Travis-Ci Russian roulette says:
[![Build Status](https://travis-ci.org/grcasanova/SeaWalker.svg?branch=master)](https://travis-ci.org/grcasanova/SeaWalker)

Coverity scan result
<a href="https://scan.coverity.com/projects/grcasanova-seawalker">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/16664/badge.svg"/>
</a>

### Dependencies
The code uses the following libraries
- MAVlink
- cpp-httplib
- json (nlohmann)
- rang
- wiringPi

### License
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

### Foot note ###
Fun fact: this design should be COLREG compliant due to the several anti-collision features and some small details such as the navigation light or the manoeuvre normaliser (to make the boat steer in a human-understandable way).
