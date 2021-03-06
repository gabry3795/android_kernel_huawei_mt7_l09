

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "NasCcInclude.h"

#include "NasUsimmApi.h"

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/*****************************************************************************
  2 常量定义
*****************************************************************************/
/* CC中同时运行的定时器的最大数目 */
#define NAS_CC_MAX_TIMER_NUM   16
#define THIS_FILE_ID PS_FILE_ID_NASCC_TIMER_C

/*****************************************************************************
  3 类型定义
*****************************************************************************/
/* 定时器句柄结构，用于管理定时器资源 */
typedef struct
{
    HTIMER                              hTimer;
    NAS_CC_TIMER_ID_ENUM                enTimerId;
    NAS_CC_ENTITY_ID_T                  entityId;
} NAS_CC_TIMER_HANDLE_STRU;

/* 超时处理函数的类型定义 */
typedef VOS_VOID (* NAS_CC_TIMEOUT_PROC_FUNC)(NAS_CC_ENTITY_ID_T entityId, VOS_UINT32 ulParam);

/* 定时器信息结构 */
typedef struct
{
    VOS_UINT32                          ulTimeout;
    NAS_CC_TIMEOUT_PROC_FUNC            pfnTimeoutProc;
} NAS_CC_TIMER_INFO_STRU;


/*****************************************************************************
  4 函数声明
*****************************************************************************/
/* 超时处理函数声明 */
LOCAL VOS_VOID  NAS_CC_T303Timeout(NAS_CC_ENTITY_ID_T entityId, VOS_UINT32 ulParam);
LOCAL VOS_VOID  NAS_CC_T305Timeout(NAS_CC_ENTITY_ID_T entityId, VOS_UINT32 ulParam);
LOCAL VOS_VOID  NAS_CC_T308Timeout(NAS_CC_ENTITY_ID_T entityId, VOS_UINT32 ulParam);
LOCAL VOS_VOID  NAS_CC_T310Timeout(NAS_CC_ENTITY_ID_T entityId, VOS_UINT32 ulParam);
LOCAL VOS_VOID  NAS_CC_T313Timeout(NAS_CC_ENTITY_ID_T entityId, VOS_UINT32 ulParam);
LOCAL VOS_VOID  NAS_CC_T323Timeout(NAS_CC_ENTITY_ID_T entityId, VOS_UINT32 ulParam);
LOCAL VOS_VOID  NAS_CC_T332Timeout(NAS_CC_ENTITY_ID_T entityId, VOS_UINT32 ulParam);
LOCAL VOS_VOID  NAS_CC_T335Timeout(NAS_CC_ENTITY_ID_T entityId, VOS_UINT32 ulParam);
LOCAL VOS_VOID  NAS_CC_T336Timeout(NAS_CC_ENTITY_ID_T entityId, VOS_UINT32 ulParam);
LOCAL VOS_VOID  NAS_CC_T337Timeout(NAS_CC_ENTITY_ID_T entityId, VOS_UINT32 ulParam);
LOCAL VOS_VOID  NAS_CC_HoldTimeout(NAS_CC_ENTITY_ID_T entityId, VOS_UINT32 ulParam);
LOCAL VOS_VOID  NAS_CC_MptyTimeout(NAS_CC_ENTITY_ID_T entityId, VOS_UINT32 ulParam);
LOCAL VOS_VOID  NAS_CC_ECTTimeout(NAS_CC_ENTITY_ID_T entityId, VOS_UINT32 ulParam);
LOCAL VOS_VOID  NAS_CC_UserConnTimeout(NAS_CC_ENTITY_ID_T entityId, VOS_UINT32 ulParam);
LOCAL VOS_VOID  NAS_CC_RabInactProtectTimeout(NAS_CC_ENTITY_ID_T entityId, VOS_UINT32 ulParam);


/*****************************************************************************
  5 变量定义
*****************************************************************************/
/* 所有可用的定时器资源 */
LOCAL NAS_CC_TIMER_HANDLE_STRU  f_astCcTimerHandles[NAS_CC_MAX_TIMER_NUM];

LOCAL VOS_UINT32  f_astCcTimerLenFromNvim[] = {
    30000,
    30000,
    30000,
    30000,
    30000,
    30000,
    30000,
    30000,
    10000,
    10000,
    10000,
    10000,
    10000,
    30000,
    30000,
};

/* 定时器信息表，该表中记录了每种定时器的超时时间和超时处理函数 */

LOCAL NAS_CC_TIMER_INFO_STRU  f_astCcTimerInfoTbl[] = {
    {30000, NAS_CC_T303Timeout},
    {30000, NAS_CC_T305Timeout},
    {30000, NAS_CC_T308Timeout},
    {30000, NAS_CC_T310Timeout},
    {30000, NAS_CC_T313Timeout},
    {30000, NAS_CC_T323Timeout},
    {30000, NAS_CC_T332Timeout},
    {30000, NAS_CC_T335Timeout},
    {10000, NAS_CC_T336Timeout},
    {10000, NAS_CC_T337Timeout},
    {10000, NAS_CC_HoldTimeout},
    {10000, NAS_CC_MptyTimeout},
    {10000, NAS_CC_ECTTimeout},
    {30000, NAS_CC_UserConnTimeout},
    {30000, NAS_CC_RabInactProtectTimeout}
};


/*****************************************************************************
  6 函数定义
*****************************************************************************/

VOS_UINT8 NAS_CC_GetNvTimerLen(
    NAS_CC_TIMER_ID_ENUM                enTimerId,
    VOS_UINT32                         *ulTimerLen
)
{
    if (enTimerId >= (sizeof(f_astCcTimerLenFromNvim)/sizeof(f_astCcTimerLenFromNvim[0])))
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_GetTimerLen: enTimerId wrong.");
        return VOS_FALSE;
    }

    *ulTimerLen = f_astCcTimerLenFromNvim[enTimerId];

    return VOS_TRUE;
}


VOS_VOID NAS_CC_SetNvTimerLen(
    NAS_CC_TIMER_ID_ENUM                enTimerId,
    VOS_UINT32                          ulTimerLen
)
{
    if (enTimerId >= (sizeof(f_astCcTimerLenFromNvim)/sizeof(f_astCcTimerLenFromNvim[0])))
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_SetTimerLen: enTimerId wrong.");
        return;
    }

    f_astCcTimerLenFromNvim[enTimerId] = ulTimerLen;
}

VOS_VOID  NAS_CC_StopAllRunningTimer(VOS_VOID)
{
    VOS_UINT32                          i;

    for (i=0; i<NAS_CC_MAX_TIMER_NUM; i++)
    {
        if (( VOS_NULL_PTR != f_astCcTimerHandles[i].hTimer )
          &&( TI_NAS_CC_MAX != f_astCcTimerHandles[i].enTimerId))
        {
             /* 停止VOS定时器 */
            if (VOS_OK == NAS_StopRelTimer(WUEPS_PID_CC,
                                 f_astCcTimerHandles[i].enTimerId,
                                 &f_astCcTimerHandles[i].hTimer))
            {
                f_astCcTimerHandles[i].hTimer = VOS_NULL_PTR;
                f_astCcTimerHandles[i].enTimerId = TI_NAS_CC_MAX;
            }
            else
            {
                NAS_CC_ERR_LOG("NAS_CC_StopAllRunningTimer: NAS_StopRelTimer failed.");
            }
        }
    }
}

/*****************************************************************************
 函 数 名  : NAS_CC_InitAllTimers
 功能描述  : 初始化所有定时器，应在CC初始化及Reset时被调用
 输入参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年1月17日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID  NAS_CC_InitAllTimers(VOS_VOID)
{
    VOS_UINT32                          i;

    for (i=0; i<NAS_CC_MAX_TIMER_NUM; i++)
    {
        f_astCcTimerHandles[i].hTimer = VOS_NULL_PTR;
        f_astCcTimerHandles[i].enTimerId = TI_NAS_CC_MAX;
    }
}



VOS_VOID  NAS_CC_StartTimer(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_TIMER_ID_ENUM                enTimerId,
    VOS_UINT32                          ulParam
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulMsgName;
    VOS_UINT32                          ulTimerLen;

    ulTimerLen = 0;

    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_StartTimer: entityId wrong.");
        return;
    }

    if (enTimerId >= (sizeof(f_astCcTimerInfoTbl)/sizeof(NAS_CC_TIMER_INFO_STRU)))
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_StartTimer: enTimerId wrong.");
        return;
    }

    /* 寻找空闲的定时器句柄 */
    for (i=0; i<NAS_CC_MAX_TIMER_NUM; i++)
    {
        if (TI_NAS_CC_MAX == f_astCcTimerHandles[i].enTimerId)
        {
            break;
        }
    }

    if (i < NAS_CC_MAX_TIMER_NUM)
    {
        /* 此处做判断，如果是T305/T308，判断一下是否是测试卡模式，如果不是测试卡模式，则读取en_NV_Item_CC_TimerLen里写入的定时器时长 */
        ulTimerLen = f_astCcTimerInfoTbl[enTimerId].ulTimeout;

        if ((TI_NAS_CC_T305 == enTimerId) || (TI_NAS_CC_T308 == enTimerId))
        {
            if (VOS_FALSE == NAS_USIMMAPI_IsTestCard())
            {
                NAS_CC_GetNvTimerLen(enTimerId, &ulTimerLen);
            }
        }

        /* 启动VOS定时器 */
        /* 需要将TimerId输入 */
        ulMsgName = (enTimerId ) | (entityId << 16) ;

        if (VOS_OK == NAS_StartRelTimer(&f_astCcTimerHandles[i].hTimer,
                                        WUEPS_PID_CC,
                                        ulTimerLen,
                                        ulMsgName,
                                        ulParam,
                                        VOS_RELTIMER_NOLOOP))
        {
            /* 记录entity ID和超时处理函数 */
            f_astCcTimerHandles[i].entityId = entityId;
            f_astCcTimerHandles[i].enTimerId = enTimerId;
        }
        else
        {
            NAS_CC_ERR_LOG("NAS_CC_StartTimer: VOS_StartRelTimer failed.");
        }
    }
    else
    {
        NAS_CC_ERR_LOG("NAS_CC_StartTimer: Can not find free timer handle.");
    }
}
VOS_VOID  NAS_CC_StopTimer(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_TIMER_ID_ENUM                enTimerId
)
{
    VOS_UINT32                          i;

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);
    NAS_CC_ASSERT(enTimerId < TI_NAS_CC_MAX);

    /* 寻找entityId和enTimerId与输入匹配的句柄 */
    for (i=0; i<NAS_CC_MAX_TIMER_NUM; i++)
    {
        if ((enTimerId == f_astCcTimerHandles[i].enTimerId) &&
            (entityId == f_astCcTimerHandles[i].entityId))
        {
            break;
        }
    }

    if (i < NAS_CC_MAX_TIMER_NUM)
    {
        /* 停止VOS定时器 */
        if (NAS_StopRelTimer(WUEPS_PID_CC,
                             f_astCcTimerHandles[i].enTimerId,
                             &f_astCcTimerHandles[i].hTimer) == VOS_OK)

        {
            f_astCcTimerHandles[i].hTimer = VOS_NULL_PTR;
            f_astCcTimerHandles[i].enTimerId = TI_NAS_CC_MAX;
        }
        else
        {
            NAS_CC_ERR_LOG("NAS_CC_StopTimer: NAS_StopRelTimer failed.");
        }
    }
    else
    {
        NAS_CC_NORM_LOG1("NAS_CC_StopTimer: Timer is not running.", enTimerId);
    }
}



VOS_VOID  NAS_CC_StopAllTimer(
    NAS_CC_ENTITY_ID_T                  entityId
)
{
    VOS_UINT32                          i;

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    /* 寻找指定实体正在运行的所有定时器 */
    for (i=0; i<NAS_CC_MAX_TIMER_NUM; i++)
    {
        if ((f_astCcTimerHandles[i].entityId == entityId)
         && (f_astCcTimerHandles[i].hTimer != VOS_NULL_PTR))
        {
            /* 停止VOS定时器 */
            if (NAS_StopRelTimer(WUEPS_PID_CC,
                                 f_astCcTimerHandles[i].enTimerId,
                                 &f_astCcTimerHandles[i].hTimer) == VOS_OK)
            {
                f_astCcTimerHandles[i].hTimer = VOS_NULL_PTR;
                f_astCcTimerHandles[i].enTimerId = TI_NAS_CC_MAX;
            }
            else
            {
                NAS_CC_ERR_LOG("NAS_CC_StopAllTimer: NAS_StopRelTimer failed.");
            }
        }
        else if ((f_astCcTimerHandles[i].entityId == entityId)
              && (f_astCcTimerHandles[i].enTimerId < TI_NAS_CC_MAX))
        {
            /*定时器超时后需要清除f_astCcTimerHandles中相应记录*/
            f_astCcTimerHandles[i].enTimerId = TI_NAS_CC_MAX;
        }
        else
        {
            /*for pc lint*/
        }
    }
}


/*****************************************************************************
 函 数 名  : NAS_CC_ClearCallOnTimeout
 功能描述  : 由于超时原因结束呼叫
 输入参数  : entityId  - 超时的CC实体
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年1月17日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID  NAS_CC_ClearCallOnTimeout(
    NAS_CC_ENTITY_ID_T                  entityId
)
{
    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

/* if 声码器已开启，则关闭声码器 (TBD) */

    NAS_CC_StopAllTimer(entityId);
    NAS_CC_SendDisconnect(NAS_CC_GetEntityTi(entityId), NAS_CC_CAUSE_102);
    NAS_CC_StartTimer(entityId, TI_NAS_CC_T305, NAS_CC_CAUSE_102);
    NAS_CC_ChangeCallState(entityId, NAS_CC_CALL_STATE_U11);
}
VOS_VOID  NAS_CC_ProcTimeoutMsg(
    VOS_VOID                            *pMsg
)
{
    REL_TIMER_MSG           *pTmrMsg = (REL_TIMER_MSG *)pMsg;
    NAS_CC_TIMER_ID_ENUM            enTid;
    NAS_CC_ENTITY_ID_T              enEntityId;

    enTid =  (NAS_CC_TIMER_ID_ENUM)((pTmrMsg->ulName) & 0xff);
    if ( enTid >=  TI_NAS_CC_MAX )
    {
        NAS_CC_WARN_LOG1("CC Timer expired,timer id is overflow.", enTid);
        return;
    }
    enEntityId = ((pTmrMsg->ulName) >> 16);

    NAS_TIMER_EventReport(enTid, WUEPS_PID_CC, NAS_OM_EVENT_TIMER_OPERATION_EXPIRED);

    f_astCcTimerInfoTbl[enTid].pfnTimeoutProc(enEntityId, pTmrMsg->ulPara);
}




LOCAL VOS_VOID  NAS_CC_T303Timeout(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT32                          ulParam
)
{
    VOS_UINT8 ucTi = NAS_CC_GetEntityTi(entityId);
    NAS_CC_CALL_STATE_ENUM_U8  enCurrState = NAS_CC_GetCallState(entityId);

    NAS_CC_CAUSE_VALUE_ENUM_U32         enCcCause;

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    NAS_CC_INFO_LOG("NAS_CC_T303Timeout");

    NAS_CC_StopTimer(entityId, TI_NAS_CC_T303);

    if (NAS_CC_CALL_STATE_U1 == enCurrState)
    {
        NAS_CC_ClearCallOnTimeout(entityId);
    }
    else if(NAS_CC_CALL_STATE_U0_1 == enCurrState)
    {
        NAS_CC_StopAllTimer(entityId);
        NAS_CC_SendMmccAbortReq(ucTi);

        enCcCause = NAS_CC_CAUSE_CC_INTER_ERR_T303_TIME_OUT;
        NAS_CC_SendMnccMsg(entityId, MNCC_REJ_IND, &enCcCause, sizeof(enCcCause));

        NAS_CC_INFO_LOG1("NAS_CC_T303Timeout: ChangeCallState to U0, current state:", enCurrState);
        NAS_CC_ChangeCallState(entityId, NAS_CC_CALL_STATE_U0);
    }
    else
    {
        NAS_CC_ERR_LOG1("NAS_CC_T303Timeout: Bad state.", enCurrState);
    }
}



LOCAL VOS_VOID  NAS_CC_T305Timeout(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT32                          ulParam
)
{
    NAS_CC_CALL_STATE_ENUM_U8  enCurrState = NAS_CC_GetCallState(entityId);

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    NAS_CC_INFO_LOG("NAS_CC_T305Timeout");

    NAS_CC_StopTimer(entityId, TI_NAS_CC_T305);

    if (NAS_CC_CALL_STATE_U11 == enCurrState)
    {
        /*
        [5.4.3.5 Abnormal cases]
        send a RELEASE message to the network with the cause number originally
        contained in the DISCONNECT message and optionally, a second cause
        information element with cause #102
        */
        NAS_CC_SendRelease(NAS_CC_GetEntityTi(entityId),
                           VOS_TRUE,
                           (NAS_CC_CAUSE_VALUE_ENUM_U32)ulParam,
                           VOS_TRUE,
                           NAS_CC_CAUSE_102);

        NAS_CC_StartTimer(entityId, TI_NAS_CC_T308, NAS_CC_T308_FIRST);

        NAS_CC_INFO_LOG("NAS_CC_T305Timeout ChangeCallState to U19");

        NAS_CC_ChangeCallState(entityId, NAS_CC_CALL_STATE_U19);
    }
    else
    {
        NAS_CC_ERR_LOG1("NAS_CC_T305Timeout: Bad state.", enCurrState);
    }
}



LOCAL VOS_VOID  NAS_CC_T308Timeout(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT32                          ulParam
)
{
    NAS_CC_CALL_STATE_ENUM_U8  enCurrState = NAS_CC_GetCallState(entityId);
    NAS_CC_MSG_RELEASE_COMPLETE_MT_STRU     stRelComp;

    VOS_UINT8  ucTi = NAS_CC_GetEntityTi(entityId);

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    NAS_CC_StopTimer(entityId, TI_NAS_CC_T308);

    if (enCurrState != NAS_CC_CALL_STATE_U19)
    {
        NAS_CC_ERR_LOG1("NAS_CC_T308Timeout: Bad state.", enCurrState);
        return;
    }

    NAS_CC_INFO_LOG("NAS_CC_T308Timeout");

    if (NAS_CC_T308_FIRST == ulParam)
    {
        /* 如果是第一次超时，再次发送Release消息 */
        NAS_CC_SendRelease(ucTi, VOS_TRUE, NAS_CC_CAUSE_102, VOS_FALSE, 0);
        NAS_CC_StartTimer(entityId, TI_NAS_CC_T308, NAS_CC_T308_SECOND);
    }
    else
    {
        /* 释放MM连接 */
        NAS_CC_SendMmccRelReq(ucTi, MMCC_RELEASE_SPECIFIC);

        /* CHR优化项目，T308第二次超时后，携带原因值102 */
        PS_MEM_SET(&stRelComp, 0, sizeof(NAS_CC_MSG_RELEASE_COMPLETE_MT_STRU));

        NAS_CC_FillCauseIe(NAS_CC_CAUSE_102, &stRelComp.stCause);

        /* 上报MNCC_REL_CNF原语 */
        NAS_CC_SendMnccMsg(entityId, MNCC_REL_CNF, &stRelComp, sizeof(NAS_CC_MSG_RELEASE_COMPLETE_MT_STRU));

        /* 进入null状态 */
        NAS_CC_INFO_LOG("NAS_CC_T308Timeout ChangeCallState to U0");
        NAS_CC_ChangeCallState(entityId, NAS_CC_CALL_STATE_U0);
    }
}
LOCAL VOS_VOID  NAS_CC_T310Timeout(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT32                          ulParam
)
{
    NAS_CC_CALL_STATE_ENUM_U8  enCurrState = NAS_CC_GetCallState(entityId);

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    NAS_CC_INFO_LOG("NAS_CC_T310Timeout");

    NAS_CC_StopTimer(entityId, TI_NAS_CC_T310);

    if (NAS_CC_CALL_STATE_U3 == enCurrState)
    {
        NAS_CC_ClearCallOnTimeout(entityId);
    }
    else
    {
        NAS_CC_ERR_LOG1("NAS_CC_T310Timeout: Bad state.", enCurrState);
    }
}



LOCAL VOS_VOID  NAS_CC_T313Timeout(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT32                          ulParam
)
{
    NAS_CC_CALL_STATE_ENUM_U8  enCurrState = NAS_CC_GetCallState(entityId);

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    NAS_CC_INFO_LOG("NAS_CC_T313Timeout");

    NAS_CC_StopTimer(entityId, TI_NAS_CC_T313);

    if (NAS_CC_CALL_STATE_U8 == enCurrState)
    {
        NAS_CC_ClearCallOnTimeout(entityId);
    }
    else
    {
        NAS_CC_ERR_LOG1("NAS_CC_T313Timeout: Bad state.", enCurrState);
    }
}



LOCAL VOS_VOID  NAS_CC_T323Timeout(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT32                          ulParam
)
{
    NAS_CC_CALL_STATE_ENUM_U8  enCurrState = NAS_CC_GetCallState(entityId);

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    NAS_CC_INFO_LOG("NAS_CC_T323Timeout");

    NAS_CC_StopTimer(entityId, TI_NAS_CC_T323);

    if (NAS_CC_CALL_STATE_U26 == enCurrState)
    {
        NAS_CC_ClearCallOnTimeout(entityId);
    }
    else
    {
        NAS_CC_ERR_LOG1("NAS_CC_T323Timeout: Bad state.", enCurrState);
    }
}



LOCAL VOS_VOID  NAS_CC_T332Timeout(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT32                          ulParam
)
{
    NAS_CC_CALL_STATE_ENUM_U8  enCurrState = NAS_CC_GetCallState(entityId);
    VOS_UINT8        ucTi = NAS_CC_GetEntityTi(entityId);

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    NAS_CC_INFO_LOG("NAS_CC_T332Timeout");

    NAS_CC_StopTimer(entityId, TI_NAS_CC_T332);

    if (NAS_CC_CALL_STATE_U0_3 == enCurrState)
    {
        NAS_CC_SendReleaseComplete(ucTi, VOS_TRUE, NAS_CC_CAUSE_102);

        NAS_CC_SendMmccRelReq(ucTi, MMCC_RELEASE_SPECIFIC);

        NAS_CC_INFO_LOG("NAS_CC_T332Timeout ChangeCallState to U0");
        NAS_CC_ChangeCallState(entityId, NAS_CC_CALL_STATE_U0);
    }
    else
    {
        NAS_CC_ERR_LOG1("NAS_CC_T332Timeout: Bad state.", enCurrState);
    }
}



LOCAL VOS_VOID  NAS_CC_T335Timeout(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT32                          ulParam
)
{
    NAS_CC_CALL_STATE_ENUM_U8   enCurrState = NAS_CC_GetCallState(entityId);
    VOS_UINT8                   ucTi = NAS_CC_GetEntityTi(entityId);

    NAS_CC_CAUSE_VALUE_ENUM_U32         enCcCause;

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    NAS_CC_StopTimer(entityId, TI_NAS_CC_T335);

    if (NAS_CC_CALL_STATE_U0_5 == enCurrState)
    {
        NAS_CC_SendReleaseComplete(ucTi, VOS_TRUE, NAS_CC_CAUSE_102);

        NAS_CC_SendMmccRelReq(ucTi, MMCC_RELEASE_SPECIFIC);

        enCcCause = NAS_CC_CAUSE_CC_INTER_ERR_T335_TIME_OUT;
        NAS_CC_SendMnccMsg(entityId, MNCC_REJ_IND, &enCcCause, sizeof(enCcCause));

        NAS_CC_INFO_LOG("NAS_CC_T335Timeout ChangeCallState to U0");
        NAS_CC_ChangeCallState(entityId, NAS_CC_CALL_STATE_U0);
    }
    else
    {
        NAS_CC_ERR_LOG1("NAS_CC_T335Timeout: Bad state.", enCurrState);
    }
}



LOCAL VOS_VOID  NAS_CC_T336Timeout(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT32                          ulParam
)
{
    NAS_CC_CAUSE_VALUE_ENUM_U32         cause;

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    NAS_CC_INFO_LOG("NAS_CC_T336Timeout");

    NAS_CC_StopTimer(entityId, TI_NAS_CC_T336);

    if (NAS_CC_GetDtmfState(entityId) != NAS_CC_DTMF_S_START_REQ)
    {
        NAS_CC_ERR_LOG("NAS_CC_T336Timeout: Not in start_req state.");
        return;
    }

    /* 发送MNCC_START_DTMF_REJ原语 */
    cause = NAS_CC_CAUSE_102;
    NAS_CC_SendMnccMsg(entityId, MNCC_START_DTMF_REJ, &cause, sizeof(cause));

    NAS_CC_ChangeDtmfState(entityId, NAS_CC_DTMF_S_START_REJ);

    /* 发送缓存的dtmf请求 */
    NAS_CC_SendBufferedDtmfReq(entityId);
}



LOCAL VOS_VOID  NAS_CC_T337Timeout(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT32                          ulParam
)
{
    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    NAS_CC_INFO_LOG("NAS_CC_T337Timeout");

    NAS_CC_StopTimer(entityId, TI_NAS_CC_T337);

    if (NAS_CC_GetDtmfState(entityId) != NAS_CC_DTMF_S_STOP_REQ)
    {
        NAS_CC_ERR_LOG("NAS_CC_T337Timeout: Not in stop_req state.");
        return;
    }

    /* stop dtmf操作即使超时也认为操作成功了，仅输出一个告警 */
    NAS_CC_SendMnccMsg(entityId, MNCC_STOP_DTMF_CNF, VOS_NULL_PTR, 0);
    NAS_CC_WARN_LOG("Timeout when stop dtmf.");

    NAS_CC_INFO_LOG("NAS_CC_T337Timeout: ChangeDtmfState IDLE");
    NAS_CC_ChangeDtmfState(entityId, NAS_CC_DTMF_S_IDLE);

    /* 发送缓存的dtmf请求 */
    NAS_CC_SendBufferedDtmfReq(entityId);
}



LOCAL VOS_VOID  NAS_CC_HoldTimeout(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT32                          ulParam
)
{
    NAS_CC_HOLD_AUX_STATE_ENUM_U8   holdState = NAS_CC_GetHoldAuxState(entityId);
    NAS_CC_CAUSE_VALUE_ENUM_U32     cause = NAS_CC_CAUSE_102;

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    NAS_CC_StopTimer(entityId, TI_NAS_CC_HOLD);

    if (NAS_CC_HOLD_AUX_S_HOLD_REQ == holdState)
    {
        /* 记录补充业务切换状态，调用统一补充业务切换状态处理函数 */
        NAS_CC_SetSsSwitchHoldInfo(entityId, NAS_CC_SS_SWITCH_TIME_OUT, cause);

        NAS_CC_ProcSsSwitchMain();
    }
    else if (NAS_CC_HOLD_AUX_S_RETRIEVE_REQ == holdState)
    {
        /* 记录补充业务切换状态，调用统一补充业务切换状态处理函数 */
        NAS_CC_SetSsSwitchRetrieveInfo(entityId, NAS_CC_SS_SWITCH_TIME_OUT, cause);

        NAS_CC_ProcSsSwitchMain();
    }
    else
    {
        NAS_CC_ERR_LOG1("NAS_CC_HoldTimeout: Bad hold state.", holdState);
    }
}



LOCAL VOS_VOID  NAS_CC_MptyTimeout(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT32                          ulParam
)
{
    VOS_UINT8                           ucInvokeId = (VOS_UINT8)ulParam;
    NAS_SS_OPERATION_ENUM_U8            enOperation;
    NAS_CC_ENTITY_ID_T                  ulHoldEntityID;
    NAS_CC_ENTITY_ID_T                  ulRetrieveEntityID;

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    NAS_CC_INFO_LOG("NAS_CC_MptyTimeout");

    NAS_CC_StopTimer(entityId, TI_NAS_CC_MPTY);

    if (NAS_CC_RestoreSsOperation(entityId, ucInvokeId, &enOperation) != VOS_OK)
    {
        NAS_CC_ERR_LOG1("NAS_CC_RestoreSsOperation: Restore SsOperation failure.", (VOS_INT32)entityId);
        return;
    }

    NAS_CC_HandleMptyEvent(entityId, NAS_CC_MPTY_EVT_FAIL, enOperation);

    /* 如果entityId记录到了补充业务切换信息中，则缓存此消息，否则发出 */
    ulHoldEntityID      = NAS_CC_GetSsSwitchHoldEntityID();
    ulRetrieveEntityID  = NAS_CC_GetSsSwitchRetrieveEntityID();

    if (entityId == ulHoldEntityID)
    {
        /* 缓存InvokeId */
        NAS_CC_SetSsSwitchInvokeId(VOS_TRUE, ucInvokeId);

        NAS_CC_SetSsSwitchHoldInfo(entityId, NAS_CC_SS_SWITCH_TIME_OUT, NAS_CC_CAUSE_102);

        NAS_CC_ProcSsSwitchMain();
    }
    else if (entityId == ulRetrieveEntityID)
    {
        /* 缓存InvokeId */
        NAS_CC_SetSsSwitchInvokeId(VOS_TRUE, ucInvokeId);

        NAS_CC_SetSsSwitchRetrieveInfo(entityId, NAS_CC_SS_SWITCH_TIME_OUT, NAS_CC_CAUSE_102);

        NAS_CC_ProcSsSwitchMain();
    }
    else
    {
        NAS_CC_SendMnccMsg(entityId,
                           MNCC_FACILITY_LOCAL_REJ,
                           &ucInvokeId,
                           sizeof(ucInvokeId));
    }

    return;
}
LOCAL VOS_VOID  NAS_CC_ECTTimeout(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT32                          ulParam
)
{
    VOS_UINT8                   ucInvokeId = (VOS_UINT8)ulParam;
    NAS_SS_OPERATION_ENUM_U8    enOperation;

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    NAS_CC_INFO_LOG("NAS_CC_ECTTimeout");

    NAS_CC_StopTimer(entityId, TI_NAS_CC_ECT);

    if (NAS_CC_RestoreSsOperation(entityId, ucInvokeId, &enOperation) != VOS_OK)
    {
        NAS_CC_ERR_LOG1("NAS_CC_RestoreSsOperation: Restore SsOperation failure.", (VOS_INT32)entityId);
        return;
    }

    NAS_CC_SendMnccMsg(entityId,
                       MNCC_FACILITY_LOCAL_REJ,
                       &ucInvokeId,
                       sizeof(ucInvokeId));
}




LOCAL VOS_VOID  NAS_CC_UserConnTimeout(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT32                          ulParam
)
{
/*
    If:
    - the user connection has to be attached but no appropriate channel is available
    for a contiguous time of 30 seconds; or if

    - the codec or interworking function is de-activated for a contiguous time of 30
    seconds;

    then the mobile station may initiate call clearing.
*/
    NAS_CC_StopTimer(entityId, TI_NAS_CC_USER_CONN);

    NAS_CC_INFO_LOG("NAS_CC_UserConnTimeout");

}



LOCAL VOS_VOID  NAS_CC_RabInactProtectTimeout(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT32                          ulParam
)
{
    VOS_UINT8                           ucTi;

    NAS_CC_CAUSE_VALUE_ENUM_U32         enCcCause;


    ucTi = NAS_CC_GetEntityTi(entityId);

    NAS_CC_StopTimer(entityId, TI_NAS_CC_RABMINACT_PROTECT);

    /* 如果对应实体在active状态，挂断电话 */
    if (NAS_CC_CALL_STATE_U10 == NAS_CC_GetCallState(entityId))
    {
        NAS_CC_SendReleaseComplete(ucTi, VOS_TRUE, NAS_CC_CAUSE_111);

        NAS_CC_SendMmccRelReq(ucTi, MMCC_RELEASE_SPECIFIC);

        enCcCause = NAS_CC_CAUSE_CC_INTER_ERR_WAIT_RAB_TIME_OUT;
        NAS_CC_SendMnccMsg(entityId, MNCC_REJ_IND, &enCcCause, sizeof(enCcCause));

        NAS_CC_INFO_LOG("NAS_CC_RabInactProtectTimeout ChangeCallState to U0");
        NAS_CC_ChangeCallState(entityId, NAS_CC_CALL_STATE_U0);
    }

}
VOS_VOID  NAS_CC_StopRabProtectTimer(VOS_VOID)
{
    VOS_UINT32                          i;

    /*停止定时器*/
    for (i = 0; i < NAS_CC_MAX_ENTITY_NUM; i++)
    {
        if (NAS_CC_CALL_STATE_U10 == NAS_CC_GetCallState(i))
        {
            NAS_CC_StopTimer(i, TI_NAS_CC_RABMINACT_PROTECT);
        }
    }
}
VOS_VOID  NAS_CC_StartRabProtectTimer(VOS_VOID)
{
    VOS_UINT32                          i;

    /*启动定时器*/
    for (i = 0; i < NAS_CC_MAX_ENTITY_NUM; i++)
    {
        if (NAS_CC_CALL_STATE_U10 == NAS_CC_GetCallState(i))
        {
            NAS_CC_StartTimer(i, TI_NAS_CC_RABMINACT_PROTECT, 0);
        }
    }
}



#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

