

#ifndef __QOS_FC_RAB_STATUS_H__
#define __QOS_FC_RAB_STATUS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "QosFcCommon.h"

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

/*****************************************************************************
  4 枚举定义
*****************************************************************************/
/*****************************************************************************
 结构名    : QOS_FC_RAB_STATE_ENUM
 协议表格  :
 ASN.1描述 :
 结构说明  : 定义无线承载的状态。该状态对GU和L代表含义不同:对GU来说，其代表RAB
             的状态；对LTE来说，其代表DRB的状态
*****************************************************************************/
typedef enum QOS_FC_RAB_STATE
{
    QOS_FC_RAB_STATE_NORMAL,           /* 正常数传状态 */
    QOS_FC_RAB_STATE_RANDOM_DISCARD,   /* 随机丢包状态 */
    QOS_FC_RAB_STATE_DISCARD,          /* 丢包状态 */
    QOS_FC_RAB_STATE_NOT_DISCARD,      /* 流控不丢包状态，STICK */
    QOS_FC_RAB_STATE_BUTT
} QOS_FC_RAB_STATE_ENUM;

typedef VOS_UINT32 QOS_FC_RAB_STATE_ENUM_UINT32;


/*****************************************************************************
   5 STRUCT定义
*****************************************************************************/
/*****************************************************************************
 结构名    : QOS_FC_RAB_ENTITY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 定义CDS任务实体结构
*****************************************************************************/
typedef struct QOS_FC_RAB_ENTITY
{
    VOS_UINT8               ucRabId;                      /* 承载标识  */
    QCI_TYPE_ENUM_UINT8     enQci;
    VOS_UINT8               ucPriority;                   /* 承载优先级 */
    VOS_UINT8               ucLinkRabId;                  /* 主承载标识  */
    QOS_FC_RAB_STATE_ENUM_UINT32       enRabState;
} QOS_FC_RAB_ENTITY_STRU;



/*****************************************************************************
  6 UNION定义
*****************************************************************************/


/*****************************************************************************
  7 全局变量声明
*****************************************************************************/


/*****************************************************************************
  8 函数声明
*****************************************************************************/
extern VOS_VOID QosFcRabInit(VOS_VOID);
extern VOS_UINT32 QosFc_RestoreHighPriRab(VOS_VOID);
extern VOS_UINT32 QosFc_DiscardAllDataFlow(VOS_VOID);
extern VOS_UINT32 QosFc_RestoreDataFlow(VOS_VOID);
extern VOS_UINT32 QosFc_DiscardDataFlow(VOS_VOID);
extern VOS_UINT32 QosFc_RandomDiscardDataFlow(VOS_VOID);

extern VOS_VOID QosFc_RabCreate(CONST MsgBlock  *pstMsg);
extern VOS_VOID QosFc_RabRelease(CONST MsgBlock  *pstMsg);

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif

