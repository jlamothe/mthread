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
// License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.

// * * *

// This program creates 5 seperate threads which report over serial
// when they're called.  It can be tested by running the program and
// watching the serial console.

#include <mthread.h>

// Our custom Thread:
class FooThread : public Thread
{
public:
    FooThread(int id);
protected:
    bool loop();
private:
    int id;
};

FooThread::FooThread(int id)
{
    this->id = id;
}

bool FooThread::loop()
{

    // Die if requested:
    if(kill_flag)
        return false;

    // Print the status message:
    Serial.print("FooThread ");
    Serial.print(id);
    Serial.println(" called.");

    // Sleep for one second:
    sleep(1);
    return true;

}

void setup()
{

    // Create five threads and add them to the main ThreadList:
    for(int i = 1; i <= 5; i++)
        main_thread_list->add_thread(new FooThread(i));

    // Initialize the serial connection:
    Serial.begin(9600);
    delay(1000);

}

// jl
