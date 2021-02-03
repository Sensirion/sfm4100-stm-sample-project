//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SF04 Sample Code (V1.0)
// File      :  SF04.cpp
// Author    :  MST
// Controller:  NEC V850/SG3 (uPD70F3740)
// Compiler  :  IAR compiler for V850 (3.50A)
// Brief     :  Sensor layer. Definitions of commands, registers, eeprom adr.
//              functions for sensor access
//==============================================================================

//---------- Includes ----------------------------------------------------------
#include "SF04.h"
#include <string.h>

//==============================================================================
u8t SF04_CheckCrc(u8t data[], u8t nbrOfBytes, u8t checksum)
//==============================================================================
{
  u8t crc = 0;	
  u8t byteCtr;
  //calculates 8-Bit checksum with given polynomial
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
  { crc ^= (data[byteCtr]);
    for (u8t bit = 8; bit > 0; --bit)
    { if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
      else crc = (crc << 1);
    }
  }
  if (crc != checksum) return CHECKSUM_ERROR;
  else return 0;
}

//===========================================================================
u8t SF04_ReadRegister(etSF04Register eSF04Register, nt16 *pRegisterValue)
//===========================================================================
{
  u8t checksum;   //variable for checksum byte
  u8t data[2];    //data array for checksum verification
  u8t error=0;    //variable for error code

  I2c_StartCondition();
  error |= I2c_WriteByte (I2C_ADR<<1 & ~I2C_RW_MASK | I2C_WRITE);
  error |= I2c_WriteByte (eSF04Register);
  I2c_StartCondition();
  error |= I2c_WriteByte (I2C_ADR<<1 & ~I2C_RW_MASK | I2C_READ);
  pRegisterValue->s16.u8H = data[0] = I2c_ReadByte(ACK);
  pRegisterValue->s16.u8L = data[1] = I2c_ReadByte(ACK);
  checksum=I2c_ReadByte(NO_ACK);
  error |= SF04_CheckCrc (data,2,checksum);
  I2c_StopCondition();
  return error;
}

//===========================================================================
u8t SF04_WriteRegister(etSF04Register eSF04Register, nt16 *pRegisterValue)
//===========================================================================
{
  u8t error=0;   //variable for error code

  //-- check if selected register is writable --
  assert(!(eSF04Register == READ_ONLY_REG1 || eSF04Register == READ_ONLY_REG2));
  //-- write register to sensor --
  I2c_StartCondition();
  error |= I2c_WriteByte (I2C_ADR<<1 & ~I2C_RW_MASK | I2C_WRITE);
  error |= I2c_WriteByte (eSF04Register & ~I2C_RW_MASK | I2C_WRITE);
  error |= I2c_WriteByte (pRegisterValue->s16.u8H);
  error |= I2c_WriteByte (pRegisterValue->s16.u8L);
  I2c_StopCondition();
  return error;
}

//===========================================================================
u8t SF04_ReadEeprom( u16t eepromStartAdr, u16t size, nt16 eepromData[])
//===========================================================================
{
  u8t  checksum;   //checksum
  u8t  data[2];    //data array for checksum verification
  u8t  error=0;    //error variable
  u16t i;          //counting variable

  nt16 eepromStartAdrTmp;  //variable for eeprom adr. as nt16
  eepromStartAdrTmp.u16=eepromStartAdr;

  //-- write I2C sensor address and command --
  I2c_StartCondition();
  error |= I2c_WriteByte (I2C_ADR<<1 & ~I2C_RW_MASK | I2C_WRITE);
  error |= I2c_WriteByte (EEPROM_R);
  //-- write 12-bit eeprom address left aligned --
  eepromStartAdrTmp.u16=(eepromStartAdrTmp.u16<<4);   // align eeprom adr left
  error |= I2c_WriteByte (eepromStartAdrTmp.s16.u8H);
  error |= I2c_WriteByte (eepromStartAdrTmp.s16.u8L);
  //-- write I2C sensor address for read --
  I2c_StartCondition();
  error |= I2c_WriteByte (I2C_ADR<<1 & ~I2C_RW_MASK | I2C_READ);
  //-- read eeprom data and verify checksum --
  for(i=0; i<size; i++)
  { eepromData[i].s16.u8H = data[0] = I2c_ReadByte(ACK);
    eepromData[i].s16.u8L = data[1] = I2c_ReadByte(ACK);
    checksum=I2c_ReadByte( (i < size-1) ? ACK : NO_ACK ); //NACK for last byte
    error |= SF04_CheckCrc (data,2,checksum);
  }
  I2c_StopCondition();
  return error;
}

//===========================================================================
u8t SF04_Measure(etSF04MeasureType eSF04MeasureType, nt16 *pMeasurand)
//===========================================================================
{
  u8t  checksum;   //checksum
  u8t  data[2];    //data array for checksum verification
  u8t  error=0;    //error variable
  u16t i;          //counting variable

  I2c_StartCondition();
  //-- write I2C sensor address + write bit --
  error |= I2c_WriteByte (I2C_ADR<<1 & ~I2C_RW_MASK | I2C_WRITE);
  //-- write measurement command --
  switch(eSF04MeasureType)
  { case FLOW :  error |= I2c_WriteByte (TRIGGER_FLOW_MEASUREMENT); break;
    case TEMP :  error |= I2c_WriteByte (TRIGGER_TEMP_MEASUREMENT); break;
    case VDD  :  error |= I2c_WriteByte (TRIGGER_VDD_MEASUREMENT) ; break;
    default: assert(0);
  }
  I2c_StartCondition();
  //-- write I2C sensor address + read bit--
  error |= I2c_WriteByte (I2C_ADR<<1 & ~I2C_RW_MASK | I2C_READ);
  //-- wait until hold master is released --
  SCL=HIGH;                     // set SCL I/O port as input
  for(i=0; i<1000; i++)         // wait until master hold is released or
  { DelayMicroSeconds(1000);    // a timeout (~1s) is reached
    if (SCL_CONF==1) break;
  }
  //-- check for timeout --
  if(SCL_CONF==0) error |= TIME_OUT_ERROR;
  //-- read two data bytes and one checksum byte --
  pMeasurand->s16.u8H = data[0] = I2c_ReadByte(ACK);
  pMeasurand->s16.u8L = data[1] = I2c_ReadByte(ACK);
  checksum=I2c_ReadByte(NO_ACK);
  //-- verify checksum --
  error |= SF04_CheckCrc (data,2,checksum);
  I2c_StopCondition();
  return error;
}

//===========================================================================
u8t SF04_ReadSerialNumber( nt32 *serialNumber )
//===========================================================================
{
  nt16 registerValue;  //register value for register
  u16t eepromBaseAdr;  //eeprom base address of active calibration field
  u16t eepromAdr;      //eeprom address of SF04's scale factor
  nt16 eepromData[2];  //serial number
  u8t  error=0;        //error variable

  //-- read "Read-Only Register 2" to find out the active configuration field --
  error |= SF04_ReadRegister(READ_ONLY_REG2,&registerValue);
  //-- calculate eeprom address of product serial number --
  eepromBaseAdr=(registerValue.u16 & 0x0007)*0x0300; //RO_REG2 bit<2:0>*0x0300
  eepromAdr= eepromBaseAdr + EE_ADR_SN_PRODUCT;
  //-- read product serial number from SF04's eeprom--
  error |= SF04_ReadEeprom( eepromAdr, 2, eepromData);
  serialNumber->s32.u16H=eepromData[0].u16;
  serialNumber->s32.u16L=eepromData[1].u16;
  return error;
}

//===========================================================================
u8t SF04_ReadScaleFactor(nt16 *scaleFactor)
//===========================================================================
{
  nt16 registerValue;     //register value for user register
  u16t eepromBaseAdr;     //eeprom base address of active calibration field
  u16t eepromAdr;         //eeprom address of SF04's scale factor
  u8t  error=0;           //error variable

  //-- read "User Register " to find out the active calibration field --
  error |= SF04_ReadRegister(USER_REG ,&registerValue);
  //-- calculate eeprom address of scale factor --
  eepromBaseAdr=((registerValue.u16 & 0x0070)>>4)*0x0300; //UserReg bit<6:4>*0x0300
  eepromAdr= eepromBaseAdr + EE_ADR_SCALE_FACTOR;
  //-- read scale factor from SF04's eeprom--
  error |= SF04_ReadEeprom( eepromAdr, 1, scaleFactor);

  return error;
}

//===========================================================================
u8t SF04_ReadFlowUnit(char *flowUnitStr)
//===========================================================================
{
  //-- table for unit dimension, unit time, unit volume (x=not defined) --
  const char *unitDimension[]={"x","x","x","n","u","m","c","d","","-","h","k",
                               "M","G","x","x"};
  const char *unitTimeBase[] ={"","us","ms","s","min","h","day","x","x","x","x",
                               "x","x","x","x","x"};
  const char *unitVolume[]   ={"ln","sl","x","x","x","x","x","x","l","g","x",
                               "x","x","x","x","x","Pa","bar","mH2O","inH2O",
                               "x","x","x","x","x","x","x","x","x","x","x","x"};
  //-- local variables --
  nt16 registerValue;     //register value for  user register
  u16t eepromBaseAdr;     //eeprom base address of active calibration field
  u16t eepromAdr;         //eeprom address of SF04's flow unit word
  nt16 flowUnit;          //content of SF04's flow unit word
  u8t  tableIndex;        //index of one of the unit arrays
  u8t  error=0;

  //-- read "User Register" to find out the active calibration field --
  error |= SF04_ReadRegister(USER_REG ,&registerValue);
  //-- calculate eeprom address of flow unit--
  eepromBaseAdr=((registerValue.u16 & 0x0070)>>4)*0x0300; //UserReg bit<6:4>*0x0300
  eepromAdr= eepromBaseAdr + EE_ADR_FLOW_UNIT;
  //-- read flow unit from SF04's eeprom--
  error |= SF04_ReadEeprom( eepromAdr, 1, &flowUnit);
  //-- get index of corresponding table and copy it to unit string --
  tableIndex=(flowUnit.u16 & 0x000F)>>0;         //flowUnit bit <3:0>
  strcpy(flowUnitStr, unitDimension[tableIndex]);
  tableIndex=(flowUnit.u16 & 0x1F00)>>8;         //flowUnit bit <8:12>
  strcat(flowUnitStr, unitVolume[tableIndex]);
  tableIndex=(flowUnit.u16 & 0x00F0)>>4;         //flowUnit bit <4:7>
  if(unitTimeBase[tableIndex] != "")             //check if time base is defined
  { strcat(flowUnitStr, "/");
    strcat(flowUnitStr, unitTimeBase[tableIndex]);
  }
  //-- check if unit string is feasible --
  if(strchr(flowUnitStr,'x') != NULL ) error |= UNIT_ERROR;

  return error;
}