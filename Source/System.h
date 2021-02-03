#ifndef SYSTEM_H
#define SYSTEM_H
//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SF04 Sample Code (V1.0)
// File      :  System.h
// Author    :  MST
// Controller:  NEC V850/SG3 (uPD70F3740)
// Compiler  :  IAR compiler for V850 (3.50A)
// Brief     :  System functions, global definitions
//==============================================================================

//---------- Includes ----------------------------------------------------------
#include <io70f3740.h>             // controller register definitions
#include <assert.h>                // assert functions
#include <intrinsics.h>            // low level microcontroller commands
#include "typedefs.h"              // type definitions

//---------- Enumerations ------------------------------------------------------
// Error codes
typedef enum{
  ACK_ERROR                = 0x01,
  TIME_OUT_ERROR           = 0x02,
  CHECKSUM_ERROR           = 0x04,
  UNIT_ERROR               = 0x08
}etError;

//==============================================================================
void Init_HW (void);
//==============================================================================
// Initializes the used hardware

//==============================================================================
void DelayMicroSeconds (u32t nbrOfUs);
//==============================================================================
// wait function for small delays
// input:  nbrOfUs   wait x times approx. one micro second (fcpu = 4MHz)
// return: -
// note: smallest delay is approx. 30us due to function call

#endif