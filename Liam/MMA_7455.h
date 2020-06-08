
/**
     Freescale 3-Axis Accelerometer MMA7455 Library designed for Arduino
     Copyright (C) 2015  Alexandre Boni

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/**
    Name:      MMA_7455
    Author:    Alexandre Boni
    Created:   2015/09/16
    Modified:  2015/09/26
    Version:   0.1
    IDE:       Arduino 1.6.5-r2
    License:   GPLv2

    Release:
      0.1
            Creation of this code from
            Moritz Kemper's MMA7455 library
            released in 03/04/2012 under
            Creative Commons License.
            (moritz.kemper@zhdk.ch)



*/
/**
    Name:      MMA_7455
    Author:    Alexandre Boni
    Created:   2015/09/16
    Modified:  2017/06/11
    By:        Ola Palm
    Version:   0.1
    IDE:       Arduino 1.6.5-r2
    License:   GPLv2
*/

#ifndef __MMA_7455_H__
#define __MMA_7455_H__

#include "MotionSensor.h"
#include "Wire.h"

/* I2C address */
#define MMA7455_ADDRESS (0x1D)

/* 10bits Output X LBS */
#define XOUTL_OFF (0x00)
#define XOUTL_MASK (0xFF)

/* 10bits Output X MBS */
#define XOUTH_OFF (0x01)
#define XOUTH_MASK (0x03)

/* 10bits Output Y LBS */
#define YOUTL_OFF (0x02)
#define YOUTL_MASK (0xFF)

/* 10bits Output Y MBS */
#define YOUTH_OFF (0x03)
#define YOUTH_MASK (0x03)

/* 10bits Output Z LBS */
#define ZOUTL_OFF (0x04)
#define ZOUTL_MASK (0xFF)

/* 10bits Output Z MBS */
#define ZOUTH_OFF (0x05)
#define ZOUTH_MASK (0x03)
//
/* 8bits Output X */
#define XOUT8_OFF (0x06)
#define XOUT8_MASK (0xFF)

/* 8bits Output Y */
#define YOUT8_OFF (0x07)
#define YOUT8_MASK (0xFF)

/* 8bits Output Z */
#define ZOUT8_OFF (0x08)
#define ZOUT8_MASK (0xFF)
//
/* Status Register */
#define STATUS_OFF (0x09)
#define STATUS_DRDY (0x01 << 0)
#define STATUS_DOVR (0x01 << 1)
#define STATUS_PERR (0x01 << 2)

/* Detection Source */
#define DETSRC_OFF (0x0A)
#define DETSRC_INT1 (0x01 << 0)
#define DETSRC_INT2 (0x01 << 1)
#define DETSRC_PDZ (0x01 << 2)
#define DETSRC_PDY (0x01 << 3)
#define DETSRC_PDX (0x01 << 4)
#define DETSRC_LDZ (0x01 << 5)
#define DETSRC_LDY (0x01 << 6)
#define DETSRC_LDX (0x01 << 7)

/* Temperature Value */
#define TOUT_OFF (0x0B)
#define TOUT_MASK (0xFF)

/* I2C Device Address */
#define I2CAD_OFF (0x0D)
#define I2CAD_DAD (0x01 << 0)
#define I2CAD_DAD_MASK (0x7F << 0)
#define I2CAD_I2CDIS (0x01 << 7)

/* User Info */
#define USRINF_OFF (0x0E)

/* Who Am I Value */
#define WHOAMI_OFF (0x0F)

/* Offset Drift X LSB */
#define XOFFL_OFF (0x10)
#define XOFFL_MASK (0xFF)

/* Offset Drift X MSB */
#define XOFFH_OFF (0x11)
#define XOFFH_MASK (0x07)

/* Offset Drift Y LSB */
#define YOFFL_OFF (0x12)
#define YOFFL_MASK (0xFF)

/* Offset Drift Y MSB */
#define YOFFH_OFF (0x13)
#define YOFFH_MASK (0x07)

/* Offset Drift Z LSB */
#define ZOFFL_OFF (0x14)
#define ZOFFL_MASK (0xFF)

/* Offset Drift Z MSB */
#define ZOFFH_OFF (0x15)
#define ZOFFH_MASK (0x07)
//
///* Mode Control Register */
#define MCTL_OFF (0x16)
#define MCTL_MOD_MASK (0x03 << 0)
#define MCTL_MOD_STBY (0x00 << 0)
#define MCTL_MOD_MSMT (0x01 << 0)
#define MCTL_MOD_LVL (0x02 << 0)
#define MCTL_MOD_PLS (0x03 << 0)
#define MCTL_GLVL_MASK (0x03 << 2)
#define MCTL_GLVL_2G (0x01 << 2)
#define MCTL_GLVL_4G (0x02 << 2)
#define MCTL_GLVL_8G (0x00 << 2)
#define MCTL_STON (0x01 << 4)
#define MCTL_SPI3W (0x01 << 5)
#define MCTL_DRPD (0x01 << 6)
//
///* Interrupt Latch Reset */
//#define INTRST_OFF              (0x17)
//#define INTRST_CLRINT1          (0x01 << 0)
//#define INTRST_CLRINT2          (0x01 << 1)
//
///* Control 1 */
//#define CTL1_OFF                (0x18)
//#define CTL1_XDA_DIS            (0x01 << 3)
//#define CTL1_YDA_DIS            (0x01 << 4)
//#define CTL1_ZDA_DIS            (0x01 << 5)
//#define CTL1_THOPT              (0x01 << 6)
//#define CTL1_INTRG_MASK         (0x03 << 1)
//#define CTL1_INTRG_LVL_PSL      (0x00 << 1)
//#define CTL1_INTRG_PSL_LVL      (0x01 << 1)
//#define CTL1_INTRG_PSL_PSL      (0x02 << 1)
//#define CTL1_INTPIN             (0x01 << 0)
//
///* Control 2 */
//#define CTL2_OFF                (0x19)
//#define CTL2_LDPL               (0x01 << 0)
//#define CTL2_PDPL               (0x01 << 1)
//#define CTL2_DRVO               (0x01 << 2)
//
///* Level Detection */
//#define LDTH_OFF                (0x1A)
//#define LDTH_MASK               (0xFF)
//#define LDTH_SIGN               (0x01 << 7)
//
///* Pulse Detection */
//#define PDTH_OFF                (0x1B)
//#define PDTH_MASK               (0xFF)
//
///* Pulse Duration */
//#define PW_OFF                  (0x1C)
//#define PW_MASK                 (0xFF)
//
///* Latency Time */
//#define LT_OFF                  (0x1D)
//#define LT_MASK                 (0xFF)
//
///* Time Windows 2nd Pulse  */
//#define TW_OFF                  (0x1E)
//#define TW_MASK                 (0xFF)

/* Accelerometer mode */
typedef enum _MODE
{
  standby = 0,
  measure = 1,
  level = 2,
  pulse = 3,
  none
} MODE;

///* Level mode */
//typedef enum _LEVEL_MODE
//{
//  lvl_positive = 0,
//  lvl_freefall = 1
//} LEVEL_MODE;
//
///* Threshold mode */
//typedef enum _TH_MODE
//{
//  th_absolute = 0,
//  th_signed = 1
//} TH_MODE;
//
///* Pulse mode */
//typedef enum _PULSE_MODE
//{
//  pls_positive = 0,
//  pls_negative = 1
//} PULSE_MODE;
//
///* Interrupt mode */
//typedef enum _ISR_MODE
//{
//  level_pulse = CTL1_INTRG_LVL_PSL,
//  pulse_level = CTL1_INTRG_PSL_LVL,
//  pulse_pulse = CTL1_INTRG_PSL_PSL
//} ISR_MODE;

class MMA_7455 : public MOTIONSENSOR
{
public:
  MMA_7455();
  /* MOTIONSENSOR */

  boolean initialize();
  void updateHeading();
  int getHeading();
  void setNewTargetHeading();
  int headingVsTarget();
  int getTiltAngle();
  int getZAngle();
  int getYAngle();
  int getXAngle();
  /* MOTIONSENSOR */

  void autoupdate(void);
  void reset(void);
  void setSensitivity(int sensitivity);
  int getSensitivity(void);

  void setMode(MODE mode);
  MODE getMode(void);

  //  void    setSelfTest(bool enable);

  // void    enableDetectionXYZ(bool x, bool y, bool z);
  // void    setThresholdMode(TH_MODE mode);
  // void    setThresholdMode(unsigned int mode);

  // void    setLevelPolarity(LEVEL_MODE mode);
  // void    setLevelPolarity(unsigned int mode);
  //  void    setLevelThresholdLimit(int8_t limit);

  // void    setPulsePolarity(unsigned int mode);
  //    void    setPulsePolarity(PULSE_MODE mode);
  // void    setPulseThresholdLimit(uint8_t limit);
  // void    setPulseDuration(uint8_t time);  /* 1 = 0.5 ms */
  // void    setPulseLatency(uint8_t time);   /* 1 = 1 ms */
  // void    setPulseDuration2(uint8_t time); /* 1 = 1 ms */

  void setAxisOffset(int16_t x, int16_t y, int16_t z);
  void getAxisOffset(int16_t *x, int16_t *y, int16_t *z);

  //    void    setInterruptMode(ISR_MODE mode);
  // void    getLevelDetection(bool* x, bool* y, bool* z);
  // void    getPulseDetection(bool* x, bool* y, bool* z);
  // void    getInterrupt(bool* int1, bool* int2);
  //void    clearInterrupt(void);
  //    void    enableInterruptPins(bool enable);

  //  int8_t  readAxis8(char axis);
  // void    readAxis8(int8_t* x, int8_t* y, int8_t* z);
  // float   readAxis8g(char axis);
  // void    readAxis8g(float* x, float* y, float* z);
  int16_t readAxis10(char axis);
  void readAxis10(int16_t *x, int16_t *y, int16_t *z);
  float readAxis10g(char axis);
  void readAxis10g(float *x, float *y, float *z);

  uint8_t readReg(uint8_t reg);
  void writeReg(uint8_t reg, uint8_t val);
  void writeReg(uint8_t reg);
};

#endif /* __MMA_7455_H__ */
