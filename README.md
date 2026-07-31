# FlexiStick -- Flexible joystick/hid device module for DUECA

## Introduction

This is a project that uses [DUECA middleware](https://github.com/dueca/dueca). It provides a general interface to Joystick compatible hid devices, through the "flexi-stick" module.

## Application

Joystick interface, using `dueca-gproject`, borrow the following:

FlexiStick/flexi-stick

Configure your script file to include the `flexi-stick` module,
specify which SDL2 joysticks it should read, and how the data should be sent to the rest of the simulation.
The python configuration files contain a piece of code to find out from SDL2 which joysticks
are available, and select the one you want.

The `dueca_mod.py` script in `run/solo/solo` provides an example of reading one or more "real" sticks.

The `dueca_mod.py` script in `run/solo/virtual` shows how to create a GUI-based virtual stick.

The `dueca_mod.py` script in `run/solo/customgui` and the example files in `run/run-data/gtk*` show
you how to create a GUI-based stick with background images under the gui elements.

This module reads user input, and can be configured to record and replay that input.
See the [DUECA example on record and replay](https://dueca.tudelft.nl/doc/example2.html)

## Author(s)

René van Paassen

## LICENSE

EUPL-1.2
