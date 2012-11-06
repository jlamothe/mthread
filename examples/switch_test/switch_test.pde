// -*- mode: c++ -*-

// Arduino-Compatible Multi-Threading Library (mthread)

// Copyright (C) 2010-2012 Jonathan Lamothe <jonathan@jlamothe.net>

// This program is free software: you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this program.  If not, see:
// http://www.gnu.org/licenses/

// See license.txt for more details.

// * * *

// This program demonstrates the SwitchInput class.  It assumes three
// switches are hooked up to pins 2, 3 and 4.  The switch on pin 2
// should be a dry contact to ground (the internal pull-up is used on
// this switch).  The switch on pin 3 should be a dry contact to
// ground with an external pull-up resistor.  The switch on pin 4
// should connect to the device's power output, with a pull-down
// resistor to ground.  There shold also be an LED connected to pin 5
// (with an appropriate resistor).

// When any of these switches close, the LED will illuminate.  When
// any of the switches open, the LED will go out.

#include <mthread.h>

#define OUTPUT_PIN 5
#define NUM_SWITCHES 3

int switch_pin[] = { 2, 3, 4 };

SwitchInput::Type switch_type[] = {
    SwitchInput::pull_up_internal,
    SwitchInput::pull_up,
    SwitchInput::pull_down
};

class MySwitch : public SwitchInput
{
public:
    MySwitch(int pin, Type type);
    void on_close();
    void on_open();
};

MySwitch::MySwitch(int pin, Type type) :
    SwitchInput(pin, DEFAULT_DEBOUNCE, type)
{}

void MySwitch::on_close()
{
    digitalWrite(OUTPUT_PIN, HIGH);
}

void MySwitch::on_open()
{
    digitalWrite(OUTPUT_PIN, LOW);
}

void setup()
{
    pinMode(OUTPUT_PIN, OUTPUT);
    digitalWrite(OUTPUT_PIN, LOW);
    MySwitch *sw[NUM_SWITCHES];
    int i;
    for(i = 0; i < NUM_SWITCHES; i++)
    {
        sw[i] = new MySwitch(switch_pin[i], switch_type[i]);
        main_thread_list->add_thread(sw[i]);
    }
}

// jl
