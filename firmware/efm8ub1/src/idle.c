/**************************************************************************//**
 * Copyright (c) 2015 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/
/////////////////////////////////////////////////////////////////////////////
// idle.c
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include "idle.h"

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

static idleTimer_TypeDef idle;

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

/**************************************************************************//**
 * Start the idle timer
 *****************************************************************************/
void idleTimerStart(void)
{
  // idle duration between 4 ms and 1020 ms
  if (idle.duration > 0)
  {
    idle.timer = (uint16_t)idle.duration * 4;
  }
  // idle duration is infinite
  else
  {
    idle.timer = 0;
  }
}

/**************************************************************************//**
 * Returns true if the idle duration timer has expired
 *****************************************************************************/
bool isIdleTimerIndefinite(void)
{
  // Infinite duration
  if (idle.duration == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**************************************************************************//**
 * Returns true if the idle duration timer has expired
 * @return true - idle timer has expired, restart the timer
 *****************************************************************************/
bool isIdleTimerExpired(void)
{
  if (!isIdleTimerIndefinite() && idle.timer == 0)
  {
    idleTimerStart();
    return true;
  }

  return false;
}

/**************************************************************************//**
 * Set the HID idle duration for all report IDs
 * @param duration idle duration in units of 4 ms
 * (0 = infinite; 4 ms to 1020 ms)
 *****************************************************************************/
void idleSetDuration(uint8_t duration)
{
  // If the desired idle duration is less than the polling rate,
  // then set the idle duration equal to the polling rate
  if ((duration != 0) && (duration < POLL_RATE_MS / 4))
  {
    idle.duration = POLL_RATE_MS / 4;
  }
  else
  {
    idle.duration = duration;
  }

  idleTimerStart();
}

/**************************************************************************//**
 * Tick the idle timer by 1 ms tick (sync with SOF)
 *****************************************************************************/
void idleTimerTick(void)
{
  if (idle.timer > 0)
  {
    idle.timer--;
  }
}

/**************************************************************************//**
 * Get the HID idle duration for all report IDs
 * @return idle duration in units of 4 ms (0 = infinite; 4 ms to 1020 ms)
 *****************************************************************************/
uint8_t idleGetDuration(void)
{
  return idle.duration;
}
