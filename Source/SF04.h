#ifndef SF04_H
#define SF04_H
//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SF04 Sample Code (V1.0)
// File      :  SF04.h
// Author    :  MST
// Controller:  NEC V850/SG3 (uPD70F3740)
// Compiler  :  IAR compiler for V850 (3.50A)
// Brief     :  Sensor layer. Definitions of commands, registers, eeprom adr.
//              functions for sensor access
//==============================================================================

//---------- Includes ----------------------------------------------------------
#include "I2C_HAL.h"
#include "system.h"
//---------- Defines -----------------------------------------------------------
//  CRC
#define POLYNOMIAL 0x131  //P(x)=x^8+x^5+x^4+1 = 100110001

//  SF04 eeprom map
#define EE_ADR_SN_CHIP       0x02E4
#define EE_ADR_SN_PRODUCT    0x02F8
#define EE_ADR_SCALE_FACTOR  0x02B6
#define EE_ADR_FLOW_UNIT     0x02B7

// sensor command
typedef enum{
  USER_REG_W               = 0xE2, // command writing user register
  USER_REG_R               = 0xE3, // command reading user register
  ADV_USER_REG_W           = 0xE4, // command writing advanced user register
  ADV_USER_REG_R           = 0xE5, // command reading advanced user register
  READ_ONLY_REG1_R         = 0xE7, // command reading read-only register 1
  READ_ONLY_REG2_R         = 0xE9, // command reading read-only register 2
  TRIGGER_FLOW_MEASUREMENT = 0xF1, // command trig. a flow measurement
  TRIGGER_TEMP_MEASUREMENT = 0xF3, // command trig. a temperature measurement
  TRIGGER_VDD_MEASUREMENT  = 0xF5, // command trig. a supply voltage measurement
  EEPROM_W                 = 0xFA, // command writing eeprom
  EEPROM_R                 = 0xFA, // command reading eeprom
  SOFT_RESET               = 0xFE  // command soft reset
}etCommand;

// sensor register
typedef enum{
  USER_REG                 = USER_REG_R,
  ADV_USER_REG             = ADV_USER_REG_R,
  READ_ONLY_REG1           = READ_ONLY_REG1_R,
  READ_ONLY_REG2           = READ_ONLY_REG2_R
}etSF04Register;

// measurement signal selection
typedef enum{
  FLOW                     = TRIGGER_FLOW_MEASUREMENT,
  TEMP                     = TRIGGER_TEMP_MEASUREMENT,
  VDD                      = TRIGGER_VDD_MEASUREMENT,
}etSF04MeasureType;

// This enum lists all available flow resolution (Advanced User Register [11:9])
typedef enum {
  eSF04_RES_9BIT                  = ( 0<<9  ),
  eSF04_RES_10BIT                 = ( 1<<9  ),
  eSF04_RES_11BIT                 = ( 2<<9  ),
  eSF04_RES_12BIT                 = ( 3<<9  ),
  eSF04_RES_13BIT                 = ( 4<<9  ),
  eSF04_RES_14BIT                 = ( 5<<9  ),
  eSF04_RES_15BIT                 = ( 6<<9  ),
  eSF04_RES_16BIT                 = ( 7<<9  ),
  eSF04_RES_MASK                  = ( 7<<9  )
} etSF04Resolution;

//==============================================================================
u8t SF04_CheckCrc(u8t data[], u8t nbrOfBytes, u8t checksum);
//==============================================================================
// calculates checksum for n bytes of data and compares it with expected
// checksum
// input:  data[]       checksum is built based on this data
//         nbrOfBytes   checksum is built for n bytes of data
//         checksum     expected checksum
// return: error:       CHECKSUM_ERROR = checksum does not match
//                      0              = checksum matches

//==============================================================================
u8t SF04_ReadRegister(etSF04Register eSF04Register, nt16 *pRegisterValue);
//==============================================================================
// reads the selected SF04 register (16bit)
// input : eSF04Register register selection
// output: *pRegisterValue
// return: error

//==============================================================================
u8t SF04_WriteRegister(etSF04Register eSF04Register, nt16 *pRegisterValue);
//==============================================================================
// writes the selected SF04 register (16bit)
// input:  eSF04Register register selection
//         *pRegisterValue
// output: -
// return: error

//==============================================================================
u8t SF04_ReadEeprom( u16t eepromStartAdr, u16t size, nt16 eepromData[]);
//==============================================================================
// reads data from the SF04's eeprom
// input : eepromStartAdr : Eeprom address of first word to read
//         size           : number of words(16bit) to read
// output: eepromData[]   : pointer to a u16t array
// return: error

//==============================================================================
u8t SF04_Measure(etSF04MeasureType eSF04MeasureType, nt16 *pMeasurand);
//==============================================================================
// measures flow, temperature, VDD
// input:  eSF04MeasureType
// output: *pMeasurand
// return: error
// note:   timing for timeout may be changed

//==============================================================================
u8t SF04_ReadSerialNumber( nt32 *serialNumber );
//==============================================================================
// reads the product's serial number
// input:  -
// output: *serialNumber: the product's serial number
// return: error

//==============================================================================
u8t SF04_ReadScaleFactor(nt16 *scaleFactor);
//==============================================================================
// reads the scale factor of the active calibration field
// input:  -
// output: *scaleFactor: scale facor of active calibration field
// return: error

//==============================================================================
u8t SF04_ReadFlowUnit(char *flowUnitStr);
//==============================================================================
// reads the flow unit of the active calibration field
// input:  -
// output: *flowUnitStr: pointer to unit string
// return: error
// note: flowUnitStr may be up to 11 characters (incl. string termination 0x00)

#endif
