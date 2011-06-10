// -*- mode: c++ -*-

// Arduino-Compatible Multi-Threading Library (mthread)

// Copyright (C) 2010, 2011 Jonathan Lamothe <jonathan@jlamothe.net>

// This program is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see: http://www.gnu.org/licenses/

// See license.txt for more details.

#include <mthread.h>
#include <newdel.h>

#define COUNT 5
#define WAIT_TIME 1000

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
  if(count < COUNT)
    {
      Serial.println("Event not triggered.");
      count++;
      delay(WAIT_TIME);
      return false;
    }
  Serial.println("Event triggered.");
  delay(WAIT_TIME);
  return true;
}

bool MyEvent::on_event()
{
  if(count > 0)
    {
      Serial.println("Handler running.");
      count--;
      delay(WAIT_TIME);
      return true;
    }
  Serial.println("Handler completed.");
  delay(WAIT_TIME);
  return false;
}

void setup()
{
  Serial.begin(9600);
  main_thread_list->add_thread(new MyEvent);
  delay(1000);
}

// jl
