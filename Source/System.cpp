//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SF04 Sample Code (V1.0)
// File      :  System.cpp
// Author    :  MST
// Controller:  NEC V850/SG3 (uPD70F3740)
// Compiler  :  IAR compiler for V850 (3.50A)
// Brief     :  System functions
//==============================================================================

//---------- Includes ----------------------------------------------------------
#include "system.h"

//==============================================================================
void Init_HW (void)
//==============================================================================
{
  //-- initialize system clock of V850 (fcpu = fosc, no PLL) --
  PRCMD = 0x00;   // unlock PCC register
  PCC = 0x00;     // perform settings in PCC register
  RCM = 0x01;     // disable ring oscillator
  //-- watchdog --
  WDTM2 = 0x0f;   // stop watchdog
  //-- interrupts --
  __EI();         // enable interrupts for debugging with minicube

  //Settings for debugging with Sensirion EKH4 and minicube2, power up sensor
  //Not needed for normal use
  PMDLL = 0xF0;
  PDLL  = 0x04;
}

//==============================================================================
#pragma optimize = s none
void DelayMicroSeconds (u32t nbrOfUs)
//==============================================================================
{
  for(u32t i=0; i<nbrOfUs; i++)
  {  //__asm("nop");  //nop's may be added for timing adjustment
  }
}