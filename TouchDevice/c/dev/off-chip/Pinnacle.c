
/*! @file Pinnacle.c
    @brief Cirque Pinnacle ASIC low-level device driver
*/

#include <string.h>
#include "Common.h"
#include "Pinnacle.h" 
#include "Spi.h"
#include "System.h"
#include "Util.h"

#define PINNACLE_CS_ASSERT()       (P3OUT_bit.P3OUT_0 = 0)
#define PINNACLE_CS_DEASSERT()     (P3OUT_bit.P3OUT_0 = 1)

/* PINNACLE_REG_STATUS1 register mask bits */
#define PINNACLE_SW_CC             (0x08)
#define PINNACLE_SW_DR             (0x04)

/* SPI filler bytes */
#define PINNACLE_FILLER            (0xFB)
#define PINNACLE_AUTO_INC          (0xFC)

/* PINNACLE_REG_FEED_CONFIG1 register mask bits */
#define PINNACLE_FEED_ENABLE       (0x01)
#define PINNACLE_DATA_MODE         (0x02)

#define PINNACLE_NUM_PACKET_BYTES  (5)

/* Pinnacle AG register addresses */
typedef enum
{
  PINNACLE_REG_FW_ID,
  PINNACLE_REG_FW_VERSION,
  PINNACLE_REG_STATUS1,
  PINNACLE_REG_SYS_CONFIG1,
  PINNACLE_REG_FEED_CONFIG1,
  PINNACLE_REG_FEED_CONFIG2,
  PINNACLE_REG_RESERVED1,
  PINNACLE_REG_CAL_CONFIG1,
  PINNACLE_REG_PS2_AUX_CTRL,
  PINNACLE_REG_SAMPLE_RATE,
  PINNACLE_REG_Z_IDLE,
  PINNACLE_REG_Z_SCALER,
  PINNACLE_REG_SLEEP_INTERVAL,
  PINNACLE_REG_SLEEP_TIMER,
  PINNACLE_REG_RESERVED2,
  PINNACLE_REG_RESERVED3,
  PINNACLE_REG_PACKET_BYTE_0,
  PINNACLE_REG_PACKET_BYTE_1,
  PINNACLE_REG_PACKET_BYTE_2,
  PINNACLE_REG_PACKET_BYTE_3,
  PINNACLE_REG_PACKET_BYTE_4,
  PINNACLE_REG_RESERVED4,
  PINNACLE_REG_RESERVED5,
  PINNACLE_REG_RESERVED6,
  PINNACLE_REG_RESERVED7,
  PINNACLE_REG_RESERVED8,
  PINNACLE_REG_RESERVED9,
  PINNACLE_REG_RESERVED10,
  PINNACLE_REG_RESERVED11,
  PINNACLE_REG_RESERVED12,
  PINNACLE_REG_RESERVED13,
  PINNACLE_REG_RESERVED14,
  
  PINNACLE_REG_MAX
} PINNACLE_REG_ADDR_t;

static u8_t packetBytes[PINNACLE_NUM_PACKET_BYTES];
u8_t fwId;
u8_t fwVersion;

static u8_t PinnacleRegReadMulti(PINNACLE_REG_ADDR_t regAddr, 
                                 u8_t numRegs, u8_t *dataBuf);
static u8_t PinnacleRegReadSingle(PINNACLE_REG_ADDR_t regAddr);
static u8_t PinnacleRegWriteSingle(PINNACLE_REG_ADDR_t regAddr, 
                                   u8_t dataByte);

static PinnacleCallbackFnc_t callbackFncPtr;

/*!
 * @brief Pinnacle ASIC device driver initialization function
 * @retval RET_SUCCESS initialization successful
 */
u8_t PinnacleInit(void)
{
  u8_t status;
  
  /* reset */
  PinnacleRegWriteSingle(PINNACLE_REG_SYS_CONFIG1, 0x01);
  
  /* wait until reset complete */
  do
  {
    status = PinnacleRegReadSingle(PINNACLE_REG_STATUS1);
  } while ((status & PINNACLE_SW_CC) == 0);
  
  UtilDelayMsec(500);
  
  /* read firmware ID */
  fwId = PinnacleRegReadSingle(PINNACLE_REG_FW_ID);

  /* read firmware version */
  fwVersion = PinnacleRegReadSingle(PINNACLE_REG_FW_ID);

  /* set Z IDLE to report back only a single zero packet */
  PinnacleRegWriteSingle(PINNACLE_REG_Z_IDLE, 0x01);
  
  /* clear SW_CC and SW_DR */
  PinnacleRegWriteSingle(PINNACLE_REG_STATUS1, 0);

  /* enable reporting and set to absolute data mode */
  PinnacleRegWriteSingle(PINNACLE_REG_FEED_CONFIG1, 
                         (PINNACLE_FEED_ENABLE | PINNACLE_DATA_MODE));
  
  return RET_SUCCESS;
}

/*!
 * @brief 
 * @retval 
 */
static u8_t PinnacleRegReadMulti(PINNACLE_REG_ADDR_t regAddr, 
                                 u8_t numRegs, u8_t *dataBuf)
{
  PINNACLE_CS_ASSERT();
  SpiWriteByte(0xA0 | regAddr);
  
  SpiWriteByte(PINNACLE_AUTO_INC);
  SpiWriteByte(PINNACLE_AUTO_INC);
  
  while (numRegs--)
  {
    if (numRegs == 0)
    {
      *dataBuf++ = SpiWriteByte(PINNACLE_FILLER);
    }
    else
    {
      *dataBuf++ = SpiWriteByte(PINNACLE_AUTO_INC);
    }
  }

  PINNACLE_CS_DEASSERT();
  
  return RET_SUCCESS;
}

/*!
 * @brief 
 * @retval 
 */
static u8_t PinnacleRegReadSingle(PINNACLE_REG_ADDR_t regAddr)
{
  u8_t retVal;
  
  PINNACLE_CS_ASSERT();
  SpiWriteByte(0xA0 | regAddr);
  SpiWriteByte(PINNACLE_FILLER);
  SpiWriteByte(PINNACLE_FILLER);
  retVal = SpiWriteByte(PINNACLE_FILLER);
  PINNACLE_CS_DEASSERT();
  
  return retVal;
}

/*!
 * @brief 
 * @retval 
 */
static u8_t PinnacleRegWriteSingle(PINNACLE_REG_ADDR_t regAddr, u8_t dataByte)
{  
  PINNACLE_CS_ASSERT();
  SpiWriteByte(0x80 | regAddr);
  SpiWriteByte(dataByte);
  PINNACLE_CS_DEASSERT();
  
  return RET_SUCCESS;
}

/*!
 * @brief 
 * @retval 
 */
#pragma vector = PORT2_VECTOR
static __interrupt void PinnacleIntrHandler(void)
{    
  /* read packet bytes */
  PinnacleRegReadMulti(PINNACLE_REG_PACKET_BYTE_0, 
                       sizeof(packetBytes), packetBytes);
  
  /* provide callback service */
  if (callbackFncPtr)
  {
    callbackFncPtr(packetBytes, sizeof(packetBytes));
  }
  
  P2IFG_bit.P2IFG_0 = 0;
  
  /* clear SW_CC and SW_DR */
  PinnacleRegWriteSingle(PINNACLE_REG_STATUS1, 0);
  
  SystemWakeup();
}

/*!
 * @brief 
 */
u8_t PinnacleRegisterCallbackFnc(PinnacleCallbackFnc_t fncPtr)
{  
  callbackFncPtr = fncPtr;
  
  return RET_SUCCESS;
}

