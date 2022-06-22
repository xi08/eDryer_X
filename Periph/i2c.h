// code = utf-8
#ifndef __i2c_h
#define __i2c_h

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#define i2cSCLPin 12
#define i2cSDAPin 11

#define i2cAddrWR 0
#define i2cAddrRD 1

/**
 * @brief i2c工作状态枚举
 *
 */
typedef enum
{
    i2cIdleState = 0,
    i2cMasterState,
    i2cStartState = 2,
    i2cDataState,
    i2cACKState,
    i2cNACKState,
    i2cStopState,
} i2cState_enum;

/**
 * @brief i2c状态码枚举
 *
 */
typedef enum
{
    /* 通用状态 */
    i2cStatusCode_Success = 0, // 成功

    /* 应答响应 */
    i2cStatusCode_ACK = 0,  // 接收到应答
    i2cStatusCode_NACK = 1, // 接收到非应答

    /* 地址检测响应 */
    i2cStatusCode_AddrOK = 0,    // 地址存在
    i2cStatusCode_AddrError = 1, // 地址不存在

    /* 模式错误 */
    i2cStatusCode_ModeErr = 2, // 工作模式错误

    /* 主机错误 */
    i2cStatusCode_LostBusCtrl = 3, // 总线仲裁错误
    i2cStatusCode_Timeout = 4,     // 超时错误

    /* 特殊错误 */
    i2cStatusCode_UnknownErr = 0xff // 未知错误

} i2cStatusCode_enum;

void i2cInit(void);
void i2cSlaveModeResp(void);

i2cStatusCode_enum i2cMasterModeEnable(void);
i2cStatusCode_enum i2cMasterModeDisable(void);
i2cStatusCode_enum i2cSTART(void);
i2cStatusCode_enum i2cSTOP(void);
i2cStatusCode_enum i2cACK(void);
i2cStatusCode_enum i2cNACK(void);
i2cStatusCode_enum i2cSend(uint8_t sData);
i2cStatusCode_enum i2cReset(void);
i2cStatusCode_enum i2cRead(uint8_t *rData);
i2cStatusCode_enum i2cReadACK(void);
i2cStatusCode_enum i2cAddrCheck(uint8_t addr);

#endif
