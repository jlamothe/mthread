// -*- mode: c++ -*-

/// @file mthread.h
/// @author Jonathan Lamothe

// Arduino Multi-Threading Library (mthread)

// Copyright (C) 2010 Jonathan Lamothe <jonathan@jlamothe.net>

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

#ifndef MTHREAD_H
#define MTHREAD_H

#ifndef DEFAULT_DEBOUNCE
#define DEFAULT_DEBOUNCE 50	///< @brief Default switch debounce
				///time.
#endif

#include "../newdel/newdel.h"

class ThreadList;
void loop();

/// @brief Provides thread functionality.
class Thread
{
public:

  /// @brief Various running modes for a Thread.
  enum Mode
    {
      run_mode,			///< @brief Thread is running.
      pause_mode,		///< @brief Thread is paused.
      sleep_mode,		///< @brief Thread is sleeping (for
				///seconds).
      sleep_milli_mode,		///< @brief Thread is sleeping (for
				///milliseconds).
      sleep_micro_mode,		///< @brief Thread is sleeping (for
				///microseconds).
      kill_mode			///< @brief Thread is to be killed on
				///next call.
    };

  /// @brief Constructor.
  Thread();

  /// @brief Destructor.
  virtual ~Thread();

  /// @brief Returns the running mode for the Thread.

  /// @return The running mode.
  Mode get_mode() const;

  /// @brief Kills a Thread.

  /// @param force If true, the Thread will be killed immediately on
  /// the next call without running any more loops, if false, the
  /// Thread will have to opportunity to terminate cleanly.

  /// @note If the force parameter is set to false, the Thread could
  /// possibly ignore or cancel the request, however this is still the
  /// preferred way of calling the kill() function.

  /// @return true on success, false otherwise.
  bool kill(bool force = false);

  /// @brief Pauses a Thread.

  /// This function will cause a Thread to pause until its resume()
  /// function is called.

  /// @return true on success, false otherwise.
  bool pause();

  /// @brief Resumes a paused Thread.

  /// @return true on success, false otherwise.
  bool resume();

  /// @brief Puts the Thread to sleep for a certain number of seconds.

  /// If the loop() function is already running, it will be allowed to
  /// finish but will not be called again until the timeout has
  /// expired.

  /// @param t The number of seconds for which the Thread is to sleep.

  /// @return true on success, false otherwise.
  bool sleep(unsigned long t);

  /// @brief Puts the Thread to sleep for a certain number of
  /// microseconds.

  /// If the loop() function is already running, it will be allowed to
  /// finish will not be called again until the timeout has
  /// expired.

  /// @param t The number of microseconds for which the Thread is to
  /// sleep.

  /// @return true on success, false otherwise.
  bool sleep_micro(unsigned long t);

  /// @brief Puts the Thread to sleep for a certain number of
  /// milliseconds.

  /// If the loop() function is already running, it will be allowed to
  /// finish will not be called again until the timeout has
  /// expired.

  /// @param t The number of milliseconds for which the Thread is to
  /// sleep.

  /// @return true on success, false otherwise.
  bool sleep_milli(unsigned long t);

protected:

  /// @brief The Thread's main loop.

  /// This function is to be overridden.  It takes the place of the
  /// loop function found in most Arduino programs.  A single loop()
  /// should run as quickly as possible, as it will hold up other
  /// Thread objects while it is executing.

  /// @see kill_flag.

  /// @return true if the loop needs to be called again, false if the
  /// Thread has completed executing (at which point it will be
  /// destroyed).
  virtual bool loop();

  /// @brief Kill flag.

  /// This variable should be checked at the beginning of every loop()
  /// function.  If it is true, the Thread has been requested to be
  /// killed, and the loop() function should behave accordingly.  The
  /// request can be cancelled by resetting it to false.
  bool kill_flag;

private:

  /// @brief Determines if the function is active and runs through a
  /// loop if appropriate.

  /// This function is called automatically by a ThreadList.

  /// @return true if the Thread needs to be called again, false if
  /// the Thread has completed execution.

  /// @note This function is called automatically by a ThreadList
  /// object.  It is important to note that once a Thread has
  /// completed its execution (this function returns false) it will
  /// automatically destroy itself and MUST NOT be used again.  A new
  /// instance must first be created.
  bool call();

  unsigned long stop_time,	///< @brief The time at which the
				///thread was stopped.
    wait_time;			///< @brief The amount of time for
				///which the thread is to wait.
  Mode mode;			///< @brief The Thread's running mode
				///(can be read externally through the
				///get_mode() function).

  friend class ThreadList;
  friend void loop();
};

/// @brief A class for running several Thread objects simultaneously.

/// A ThreadList object is a Thread in and of itself.  This allows the
/// creation of tiered ThreadList objects by placing a lower-priority
/// ThreadList inside of a higher-priority ThreadList.

/// @note DO NOT place a Thread in more than one ThreadList or more
/// than once in a single ThreadList. DO NOT place a ThreadList inside
/// of itself or one of its children.  Also, DO NOT place the @link
/// mthread_globals::main_thread_list main_thread_list @endlink in
/// another ThreadList.  These WILL cause memory corruption (and are
/// silly things to do in the first place).
class ThreadList : public Thread
{
public:

  /// @brief Constructor.

  /// @param keep If true, the ThreadList will continue to run even
  /// after it's empty, otherwise it will automatically destroy itself
  /// once all of its Thread objects have finished.
  ThreadList(bool keep = false);

  /// @brief Destructor.
  ~ThreadList();

  /// @brief Adds a Thread to the ThreadList.

  /// @param t A pointer to the Thread to be added.

  /// @return true on success, false otherwise.
  bool add_thread(Thread *t);

protected:

  /// @brief The main loop.

  /// @see Thread::loop().
  bool loop();

private:

  Thread **thread;		///< @brief An array of pointers to
				///the Thread objects in the list.
  unsigned thread_count,	///< @brief The number of Thread
				///objects in the list.
    thread_index;		///< @brief The index number of the
				///active Thread.
  bool keep_flag;		///< @brief Indicates whether or not
				///to continue running the ThreadList
				///after it has run out of Thread
				///objects.

};

/// @brief Handler for a switch input.
class SwitchInput : public Thread
{
public:

  /// @brief Types of switches.
  enum Type {
    pull_up_internal,		///< @brief Switch equipped with an
				///internal pull-up resistor.
    pull_up,			///< @brief Switch equipped with an
				///external pull-up resistor.
    pull_down			///< @brief Switch equipped with a
				///pull-down resistor.
  };

  /// @brief Constructor.

  /// @param pin The number of the switch to which the pin is
  /// connected.

  /// @param debounce The debounce time (in milliseconds).

  /// @param type The type of switch connected.
  SwitchInput(int pin,
	      unsigned long debounce = DEFAULT_DEBOUNCE,
	      Type type = pull_up_internal);

  /// @brief Destructor.
  virtual ~SwitchInput();

  /// @brief Checks to see if the switch is closed.

  /// @return true if the switch is closed, false otherwise.
  bool is_closed();

  /// @brief Checks to see if the switch is open.

  /// @return true if the switch is open, false otherwise.
  bool is_open();

  /// @brief Called when the switch closes.
  virtual void on_close();

  /// @brief Called when the switch opens.
  virtual void on_open();

protected:

  /// @brief Main loop.

  /// @see Thread::loop().
  bool loop();

private:

  unsigned long debounce,	///< @brief The debounce time (in
				///milliseconds).
    last_change;		///< @brief The time of the last
				///change (in milliseconds).
  int current_value,		///< @brief The switch's current
				///(reported) value.
    last_value,			///< @brief The switch's value on the
				///last read.
    pin;			///< @brief The pin to which the
				///switch is connected.
  Type type;			///< @brief The type of switch
				///connected.

};

/// @brief A pointer to the main ThreadList.

/// This object will be run in place of the loop function expected in
/// most Arduino programs.
extern ThreadList *main_thread_list;

#endif	// MTHREAD_H

// jl
