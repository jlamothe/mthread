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

// This program demonstrates the EventHandler class by creating an
// event which will only evaluate true periodically and then run for a
// while before waiting for the condition to evaluate true again.  The
// program reports what's going on over the serial console for the
// demonstration.

#include <mthread.h>

#define COUNT 5
#define SLEEP_TIME 1

class MyEvent : public EventHandler
{
public:
    MyEvent();
protected:
    bool condition();
    bool on_event();
private:
    int count;
};

MyEvent::MyEvent()
{
    count = 0;
}

bool MyEvent::condition()
{
    sleep(SLEEP_TIME);
    if(count < COUNT)
    {
        Serial.println("Event not triggered.");
        count++;
        return false;
    }
    Serial.println("Event triggered.");
    return true;
}

bool MyEvent::on_event()
{
    sleep(SLEEP_TIME);
    if(count > 0)
    {
        Serial.println("Handler running.");
        count--;
        return true;
    }
    Serial.println("Handler completed.");
    return false;
}

void setup()
{
    Serial.begin(9600);
    main_thread_list->add_thread(new MyEvent);
    delay(1000);
}

// jl
