# FlexiStick -- Flexible joystick/hid device module for DUECA

## Introduction

This is a project that uses [DUECA middleware](https://github.com/dueca/dueca). It provides a general interface to Joystick compatible hid devices, through the "flexi-stick" module.

## Application

Joystick interface, using `dueca-gproject`, borrow the following:

FlexiStick/flexi-stick

Configure your script file to include the `flexi-stick` module,
specify which SDL2 joysticks it should read, and how the data should be sent to the rest of the simulation.

The `dueca_mod.py` script in `run/solo/solo` provides an example of reading one or more "real" sticks.

The `dueca_mod.py` script in `run/solo/virtual` shows how to create a GUI-based virtual stick.

## Author(s)

René van Paassen

## LICENSE

EUPL-1.2
