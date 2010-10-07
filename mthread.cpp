// Arduino Multi-Threading Library (mthread)
// Copyright (C) 2010  Jonathan Lamothe
// <jonathan@jlamothe.net>

// This program is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see
// <http://www.gnu.org/licenses/>.

#include "mthread.h"

Thread::Thread()
{

  // Initialize values:
  mode = run_mode;
  kill_flag = false;

}

Thread::~Thread()
{}

Thread::Mode Thread::get_mode() const
{
  return mode;
}

bool Thread::kill(bool force)
{

  // If it's a forced kill, don't ask questions - just do it:
  if(force)
    {
      mode = kill_mode;
      return true;
    }

  // Otherwise, fail if we're already in a forced kill mode:
  if(mode == kill_mode)
    return false;

  // If the thread is paused, wake it up:
  if(mode == pause_mode)
    mode = run_mode;

  // Request that the thread be killed:
  kill_flag = true;

}

bool Thread::pause()
{

  // Fail unless the thread is running or already paused:
  if(mode != run_mode && mode != pause_mode)
    return false;

  // Pause the Thread:
  mode = pause_mode;
  return true;

}

bool Thread::resume()
{

  // Fail unless the thread is already running or paused:
  if(mode != run_mode && mode != pause_mode)
    return false;

  // Resume the Thread:
  mode = run_mode;
  return true;

}

bool Thread::sleep(unsigned t)
{

  // Fail unless the thread is currently running:
  if(mode != run_mode)
    return false;

  // Set the sleep timeout:
  mode = sleep_mode;
  resume_time = millis() + ((unsigned long)(t)) * 1000ul;
  return true;

}

bool Thread::sleep_micro(unsigned long t)
{

  // Fail unless the thread is currently running:
  if(mode != run_mode)
    return false;

  // Set the sleep timeout:
  mode = sleep_micro_mode;
  resume_time = micros() + t;
  return true;

}

bool Thread::sleep_milli(unsigned long t)
{

  // Fail unless the thread is currently running:
  if(mode != run_mode)
    return false;

  // Set the sleep timeout:
  mode = sleep_milli_mode;
  resume_time = millis() + t;
  return true;

}

bool Thread::loop()
{

  // Do nothing:
  return false;

}

ThreadList::ThreadList(bool keep)
{

  // Initialize values:
  thread = NULL;
  thread_count = 0;
  thread_index = 0;
  keep_flag = keep;

}

bool Thread::call()
{
  unsigned long t;

  // Determine the Thread's mode and act accordingly:
  switch(mode)
    {

    case run_mode:

      // If the main loop completes destroy the Thread:
      if(!loop())
	{
	  delete this;
	  return false;
	}
      return true;

    case pause_mode:

      // Thread is paused - do nothing:
      return true;

    case sleep_mode:
    case sleep_milli_mode:

      // Check if the sleep timeout has completed:
      if(millis() >= resume_time)
	{

	  // Return to normal operation:
	  mode = run_mode;
	  if(!loop())
	    {
	      delete this;
	      return false;
	    }

	}
      return true;

    case sleep_micro_mode:

      // Check if the sleep timeout has completed:
      if(micros() >= resume_time)
	{

	  // Return to normal operation:
	  mode = run_mode;
	  if(!loop())
	    {
	      delete this;
	      return false;
	    }

	}
      return true;

    }

  // The thread is either in kill mode or something really bad has
  // happened - terminate with extreme prejudice:
  delete this;
  return false;

}

ThreadList::~ThreadList()
{

  // Destroy any threads left in the list - there shouldn't be any,
  // but better safe than sorry:
  for(unsigned i = 0; i < thread_count; i++)
    delete thread[i];

  // Free the list itself:
  free(thread);

}

bool ThreadList::add_thread(Thread *t)
{

  // Sanity check:
  if(t == NULL)
    return false;

  // Allocate space in the list for the new thread:
  Thread **new_thread = (Thread **)realloc(thread,
					   sizeof(Thread *) *
					   (thread_count + 1));
  if(new_thread == NULL)
    return false;
  thread = new_thread;

  // Append the new pointer to the list and update the thread count:
  thread[thread_count] = t;
  thread_count++;
  return true;

}

bool ThreadList::loop()
{

  // Die if requested:
  if(kill_flag)
    return false;

  // If nothing remains in the list, do nothing:
  if(thread_count == 0)
    return keep_flag;

  // Call the next Thread in the list:
  if(thread[thread_index]->call())
    {

      // The Thread needs to be called again - move on to the next
      // thread:
      thread_index++;
      if(thread_index >= thread_count)
	thread_index = 0;
      return true;

    }

  // The Thread doesn't need to be called again - remove it from the list:
  thread_count--;
  for(unsigned i = thread_index; i < thread_count; i++)
    thread[i] = thread[i + 1];
  if(thread_index >= thread_count)
    thread_index = 0;

  // If nothing remains in the list, free it and exit:
  if(thread_count == 0)
    {
      free(thread);
      thread = NULL;
      return keep_flag;
    }

  // Re-allocate the list to reflect the missing thread:
  thread = (Thread **)realloc(thread,
			      sizeof(Thread *) * thread_count);
  return true;

}

void loop()
{

  // Make sure the main ThreadList exists:
  if(main_thread_list == NULL)
    return;

  // Call it:
  if(!main_thread_list->call())
    main_thread_list = NULL;

}

// Initialize the main ThreadList:
ThreadList *main_thread_list = new ThreadList;

// jl
