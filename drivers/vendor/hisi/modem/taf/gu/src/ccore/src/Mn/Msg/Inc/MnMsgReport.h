/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : MnMsgReport.h
  版 本 号   : 初稿
  作    者   : 周君 40661
  生成日期   : 2008年2月19日
  最近修改   :
  功能描述   :
  函数列表   :
  修改历史   :
  1.日    期   : 2008年2月19日
    作    者   : 周君 40661
    修改内容   : 创建文件
  2.日    期   : 2008年8月14日
    作    者   : 傅映君 62575
    修改内容   : 问题单号 AT2D04609,CMMS命令的增加；
  3.日    期   : 2008年8月29日
    作    者   : fuyingjun
    修改内容   : 问题单号:AT2D05344, 短消息模块初始化过程结束后进行SDT工具与单板反复进行连接和断开操作,协议栈会重复上报初始化完成事件;

******************************************************************************/
#ifndef __MNMSGREPORT_H__
#define __MNMSGREPORT_H__


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include  "vos.h"
#include  "MnMsgApi.h"
#include  "TafCbsInterface.h"
#include "NasSms.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 接口函数声明
*****************************************************************************/

VOS_VOID MN_MSG_ReportResult(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    VOS_UINT32                          ulEventType,
    VOS_UINT32                          ulErrorCode
);


VOS_VOID MN_MSG_ReportRcvMsgEvent(
    MN_OPERATION_ID_T                   opId,
    const MN_MSG_DELIVER_EVT_INFO_STRU  *pstDeliverEvt
);


VOS_VOID MN_MSG_ReportSmChanged(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    VOS_BOOL                            bBroadCast,
    MN_MSG_MEM_STORE_ENUM_U8            enMemStore
);


VOS_VOID MN_MSG_ReportMemExceed(
    MN_OPERATION_ID_T                   opId,
    MN_MSG_MEM_STORE_ENUM_U8            enMemStore
);


VOS_VOID MN_MSG_ReportSrvParmChanged(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    MN_MSG_MEM_STORE_ENUM_U8            enMemStore
);


VOS_VOID MN_MSG_ReportDeliverErr(TAF_MSG_ERROR_ENUM_UINT32 enErrorCode);


VOS_VOID MN_MSG_ReportLinkCtrl(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    VOS_UINT32                          ulEventType,
    VOS_UINT32                          ulErrorCode,
    MN_MSG_LINK_CTRL_U8                 enLinkCtrl
);

/*****************************************************************************
 函 数 名  : MN_MSG_ReportSentEvent
 功能描述  : 上报消息发送或缓存的结果
 输入参数  : clientId  : 发起该请求的Client的ID
             opId      : Operation ID, 标识本次操作
             pstSendMsgEvt:消息发送或缓存的结果
             ulEventType:操作类型
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月19日
    作    者   : 周君 40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID MN_MSG_ReportSentEvent(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    MN_MSG_SENDMSG_EVT_INFO_STRU        *pstSendMsgEvt,
    VOS_UINT32                          ulEventType
);

/*****************************************************************************
 函 数 名  : MN_MSG_ReportSubmitRptEvent
 功能描述  : 上报消息发送或缓存的结果
 输入参数  : clientId  : 发起该请求的Client的ID
             opId      : Operation ID, 标识本次操作
             pstSubmitRptEvt:接收发送消息的报告
             ulEventType:操作类型
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月19日
    作    者   : 周君 40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID MN_MSG_ReportSubmitRptEvent(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    MN_MSG_SUBMIT_RPT_EVT_INFO_STRU     *pstSubmitRptEvt,
    VOS_UINT32                          ulEventType
);

/*****************************************************************************
 函 数 名  : MN_MSG_ReportRcvPathEvent
 功能描述  : 上报设置或获取当前配置路径的结果
 输入参数  : clientId  : 发起该请求的Client的ID
             opId      : Operation ID, 标识本次操作
             bBroadCast:是否需要广播上报
             pRcvPathEvt:当前配置路径的结果
             ulEventType:操作类型
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月19日
    作    者   : 周君 40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID MN_MSG_ReportRcvPathEvent(
    MN_CLIENT_ID_T                         clientId,
    MN_OPERATION_ID_T                      opId,
    VOS_BOOL                               bBroadCast,
    const MN_MSG_RCVMSG_PATH_EVT_INFO_STRU *pRcvPathEvt,
    VOS_UINT32                             ulEventType
);


VOS_VOID MN_MSG_ReportDeleteEvent(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    const MN_MSG_DELETE_EVT_INFO_STRU   *pstDeleteEvt,
    VOS_UINT32                          ulEventType
);


VOS_VOID MN_MSG_ReportDeleteTestEvent(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    const MN_MSG_DELETE_TEST_EVT_INFO_STRU *pstDeleteTestEvt,
    VOS_UINT32                          ulEventType
);


VOS_VOID MN_MSG_ReportWriteEvent(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    const MN_MSG_WRITE_EVT_INFO_STRU    *pstWriteEvt,
    VOS_UINT32                          ulEventType
);


VOS_VOID MN_MSG_ReportReadEvent(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    const MN_MSG_READ_EVT_INFO_STRU     *pReadEvt,
    VOS_UINT32                          ulEventType
);

/*****************************************************************************
 函 数 名  : MN_MSG_ReportMemStatustEvent
 功能描述  : 上报设置或获取当前APP内存使用的结果
 输入参数  : clientId  : 发起该请求的Client的ID
             opId      : Operation ID, 标识本次操作
             bBroadCast:是否是广播上报
             pstMemStatusEvt:当前APP内存情况
             ulEventType:操作类型
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月19日
    作    者   : 周君 40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID MN_MSG_ReportMemStatustEvent(
    MN_CLIENT_ID_T                       clientId,
    MN_OPERATION_ID_T                    opId,
    VOS_BOOL                             bBroadCast,
    const MN_MSG_MEMSTATUS_EVT_INFO_STRU *pstMemStatusEvt,
    VOS_UINT32                           ulEventType
);


VOS_VOID MN_MSG_ReportListEvent(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    const MN_MSG_LIST_EVT_INFO_STRU     *pListEvt,
    VOS_UINT32                          ulEventType
);



VOS_VOID MN_MSG_ReportModifyEvent(
    MN_CLIENT_ID_T                           clientId,
    MN_OPERATION_ID_T                        opId,
    const MN_MSG_MODIFY_STATUS_EVT_INFO_STRU *pModifyStatusEvt,
    VOS_UINT32                               ulEventType
);


VOS_VOID MN_MSG_ReportSrvParmEvent(
    MN_CLIENT_ID_T                       clientId,
    MN_OPERATION_ID_T                    opId,
    const MN_MSG_SRV_PARM_EVT_INFO_STRU *pstSrvParmEvt,
    VOS_UINT32                           ulEventType
);


VOS_VOID MN_MSG_SmInitFinish(
    MN_CLIENT_ID_T                      clientId
);


#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* __MNMSGREPORT_H__*/

