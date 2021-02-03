//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SF04 Sample Code (V1.0)
// File      :  main.h
// Author    :  MST
// Controller:  NEC V850/SG3 (uPD70F3740)
// Compiler  :  IAR compiler for V850 (3.50A)
// Brief     :  This code shows how to implement the basic commands for a
//              flow or differential pressure senor based on SF04 sensor chip.
//              Due to compatibility reasons the I2C interface is implemented
//              as "bit-banging" on normal I/O's. This code is written for an
//              easy understanding and is neither optimized for speed nor code
//              size.
//
// Porting to a different microcontroller (uC):
//   - define the byte-order for your uC (e.g. little endian) in typedefs.h
//   - the definitions of basic types may have to be changed  in typedefs.h
//   - change the port functions / definitions for your uC    in I2C_HAL.h/.c
//   - adapt the timing of the delay function for your uC     in system.c
//   - adapt the HW_Init()                                    in system.c
//   - change the uC register definition file <io70f3740.h>   in system.h
//---------- Includes ----------------------------------------------------------
#include "SF04.h"
#include "System.h"
#include <stdio.h>
//==============================================================================
int main()
//==============================================================================
{  // variables
  u8t error = 0;         //variable for error code. For error codes see system.h
  nt16 registerValue;    //variable for a sensor's register value
  nt16 measurand;        //variable for a sensor measurement
  nt32 snProduct;        //variable for the sensor's product serial number
  nt16 scaleFactor;      //variable for the sensor's scale factor
  ft flow;               //variable for measured flow as a float value
  char flowUnitStr[15];  //string for the flow unit
  char output[40];       //output string for measured value and unit

  Init_HW();                  //Initializes the Hardware (osc, watchdog,...)
  I2c_Init();                 //Initializes the uC-ports for I2C
  DelayMicroSeconds(20000);   //wait for sensor initialization t_INIT (20ms)
  //-- get product serial number and convert it to a string--
  error |= SF04_ReadSerialNumber(&snProduct);
  sprintf(output, "Product Serial Number: %ld \n",snProduct.u32);
  //-- get scale factor flow --
  error |= SF04_ReadScaleFactor(&scaleFactor);
  //-- get flow unit --
  error |= SF04_ReadFlowUnit(flowUnitStr);
  //-- set measurement resolution to 14 bit --
  error |= SF04_ReadRegister(ADV_USER_REG,&registerValue);
  registerValue.u16 = (registerValue.u16 & ~eSF04_RES_MASK) | eSF04_RES_14BIT;
  error |= SF04_WriteRegister(ADV_USER_REG,&registerValue);
  while(1)
  { //-- measure flow and convert to a string --
    error |= SF04_Measure(FLOW, &measurand);
    //-- calculate flow --
    flow = (float)measurand.i16 / scaleFactor.u16;    //for bidirectional flow
    sprintf(output, "%f %s \n",flow,flowUnitStr);
    DelayMicroSeconds(10000);
  }
}