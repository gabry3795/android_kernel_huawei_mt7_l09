

#ifndef __FR_INTERFACE_H__
#define __FR_INTERFACE_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "codec_com_codec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define FR_CODED_SERIAL_LENGTH      (76)                                        /*FR编码器编码结果,76个参数共152个Byte*/
#define FR_CODED_BITS_LENGTH        (260)                                       /*FR编码器编码比特提取结果,76个参数提取共260比特*/
#define FR_HOMING_WHOLE_FRAME       (76)                                        /*FR 下行全帧HOMING检测参数个数*/
#define FR_HOMING_FIRST_SUBFRAME    (25)                                        /*FR 下行第一子帧HOMING检测参数个数*/
#define FR_SIGNAL_RAND_RANGE_15     (15)                                        /* 小信号随机数幅度 */


#define FR_GetEncStatePtr()         (&g_stFrEncodeState)                        /*获取全局变量FR编码主结构体指针*/

#define FR_GetDecStatePtr()         (&g_stFrDecodeState)                        /*获取全局变量FR解码主结构体指针*/

#define FR_GetParsNumPtr()          (g_ashwFrBitNo)                             /*获取全局变量FR各参数比特个数表指针*/

#define FR_GetOldResetPtr()         (&g_uhwFrOldResetFlag)                      /*获取全局变量FR上一帧reset标志指针*/



/*****************************************************************************
  3 枚举定义
*****************************************************************************/



/*****************************************************************************
 枚 举 名  : FR_VAD_STATUS_ENUM
 枚举说明  : VAD检测标志
 *****************************************************************************/
enum FR_VAD_STATUS_ENUM
{
    FR_VAD_STATUS_NOT                   = 0,                                    /*VAD检测为0*/
    FR_VAD_STATUS_YES                   = 1,                                    /*VAD检测为1*/
    FR_VAD_STATUS_BUTT
};
typedef VOS_UINT16 FR_VAD_STATUS_ENUM_UINT16;


/*****************************************************************************
 枚 举 名  : FR_SP_STATUS_ENUM
 枚举说明  : SP判别标志
 *****************************************************************************/
enum FR_SP_STATUS_ENUM
{
    FR_SP_STATUS_NOT                    = 0,                                    /*SP判别为0*/
    FR_SP_STATUS_YES                    = 1,                                    /*SP判别为1*/
    FR_SP_STATUS_BUTT
};
typedef VOS_UINT16 FR_SP_STATUS_ENUM_UINT16;


/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : FR_ENCODED_SERIAL_STRU
 功能描述  : FR声码器编码结果序列结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           ashwLarc[8];                            /*Larc系数,共8个参数*/
    VOS_INT16                           ashwFirstSubFrm[17];                    /*第一子帧系数*/
    VOS_INT16                           ashwSecondSubFrm[17];                   /*第二子帧系数*/
    VOS_INT16                           ashwThirdSubFrm[17];                    /*第三子帧系数*/
    VOS_INT16                           ashwFourthSubFrm[17];                   /*第四子帧系数*/
}FR_ENCODED_SERIAL_STRU;

/*****************************************************************************
 实体名称  : FR_DECODE_FRAME_STRU
 功能描述  : FR送入解码器前码流格式结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           ashwSerial[FR_CODED_SERIAL_LENGTH];     /* FR解码参数流 */
    VOS_UINT16                          enSidFlag;                              /* SID标志 */
    VOS_UINT16                          enTafFlag;                              /* TAF标志 */
    VOS_UINT16                          enBfiFlag;                              /* BFI标志 */
}FR_DECODE_FRAME_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID FR_DlFrameConvert(VOS_INT16 *pshwFrFrame, VOS_INT16 *pshwEncodedSerial);
extern VOS_VOID FR_UlFrameConvert(VOS_INT16 *pshwEncodedSerial, VOS_INT16 *pshwFrFrame);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
