/******************************************************************************

   Copyright(C)2013,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : ImsaProcCdsMsg.c
  Description     : 该C文件实现CDS消息处理和CDS消息发送
  History           :
     1.sunbing 49683      2013-06-19  Draft Enact

******************************************************************************/

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "ImsaProcCdsMsg.h"
#include "ImsaEntity.h"
#include "ImsaPublic.h"

/*lint -e767*/
#define    THIS_FILE_ID   PS_FILE_ID_IMSAPROCCDSMSG_C
/*lint +e767*/

/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#if (FEATURE_ON == FEATURE_IMS)
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/


/*****************************************************************************
  3 Function
*****************************************************************************/

VOS_VOID IMSA_CONN_SndCdsSetImsBearerReq( VOS_VOID )
{
    IMSA_CDS_SET_IMS_BEARER_REQ_STRU   *pstSetImsBearerReq  = VOS_NULL_PTR;
    IMSA_NORMAL_CONN_STRU              *pstNormalConn       = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn          = VOS_NULL_PTR;
    VOS_UINT32                          i                   = IMSA_NULL;
    IMSA_CDS_IMS_BEARER_STRU           *pstCdsImsBearer     = VOS_NULL_PTR;
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_SndCdsSetImsBearerReq is entered!");

    /*分配空间并检验分配是否成功*/
    pstSetImsBearerReq = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_CDS_SET_IMS_BEARER_REQ_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstSetImsBearerReq)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_CONN_SndCdsSetImsBearerReqS:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET( IMSA_GET_MSG_ENTITY(pstSetImsBearerReq), 0, IMSA_GET_MSG_LENGTH(pstSetImsBearerReq));

    /*填写消息头*/
    /*lint -e960*/
    IMSA_WRITE_CDS_MSG_HEAD(pstSetImsBearerReq, ID_IMSA_CDS_SET_IMS_BEARER_REQ);
    /*lint +e960*/

    /*填写响应结果*/
    pstEmcConn                          = IMSA_CONN_GetEmcConnAddr();
    if (IMSA_PDP_STATE_ACTIVE           == pstEmcConn->stSipSignalPdp.enPdpState)
    {
        pstCdsImsBearer                 = &pstSetImsBearerReq->astImsBearerArray[pstSetImsBearerReq->ulImsBearerNum];
        pstCdsImsBearer->ucEpsbId       = pstEmcConn->stSipSignalPdp.ucPdpId;
        pstCdsImsBearer->enBearerType   = IMSA_CDS_IMS_BEARER_TYPE_SIGNAL;

        pstSetImsBearerReq->ulImsBearerNum++;
    }

    for (i = 0; i < pstEmcConn->ulSipMediaPdpNum; i++)
    {
       if (IMSA_PDP_STATE_ACTIVE           == pstEmcConn->astSipMediaPdpArray[i].enPdpState)
        {
            pstCdsImsBearer                 = &pstSetImsBearerReq->astImsBearerArray[pstSetImsBearerReq->ulImsBearerNum];
            pstCdsImsBearer->ucEpsbId       = pstEmcConn->astSipMediaPdpArray[i].ucPdpId;
            pstCdsImsBearer->enBearerType   = IMSA_CDS_IMS_BEARER_TYPE_MEDIA;

            pstSetImsBearerReq->ulImsBearerNum++;
        }
    }

    pstNormalConn                       = IMSA_CONN_GetNormalConnAddr();

    for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i++)
    {
       if (IMSA_PDP_STATE_ACTIVE           == pstNormalConn->astSipSignalPdpArray[i].enPdpState)
        {
            pstCdsImsBearer                 = &pstSetImsBearerReq->astImsBearerArray[pstSetImsBearerReq->ulImsBearerNum];
            pstCdsImsBearer->ucEpsbId       = pstNormalConn->astSipSignalPdpArray[i].ucPdpId;
            pstCdsImsBearer->enBearerType   = IMSA_CDS_IMS_BEARER_TYPE_SIGNAL;

            pstSetImsBearerReq->ulImsBearerNum++;
        }
    }

    /* 视频包走A核，所以保存的视频承载不需要配置给CDS */
    for (i = 0; i < pstNormalConn->ulSipMediaPdpNum; i++)
    {
        if (IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipMediaPdpArray[i].enPdpState)
        {
            /* if (IMSA_TRUE == IMSA_CONN_IsVoiceMediaPdpType(&pstNormalConn->astSipMediaPdpArray[i].stTft,
                                                            pstNormalConn->astSipMediaPdpArray[i].stEpsQos.ucQCI))
            {
                pstCdsImsBearer                 = &pstSetImsBearerReq->astImsBearerArray[pstSetImsBearerReq->ulImsBearerNum];
                pstCdsImsBearer->ucEpsbId       = pstNormalConn->astSipMediaPdpArray[i].ucPdpId;
                pstCdsImsBearer->enBearerType   = IMSA_CDS_IMS_BEARER_TYPE_MEDIA;

                pstSetImsBearerReq->ulImsBearerNum++;
            } */

            pstCdsImsBearer                 = &pstSetImsBearerReq->astImsBearerArray[pstSetImsBearerReq->ulImsBearerNum];
            pstCdsImsBearer->ucEpsbId       = pstNormalConn->astSipMediaPdpArray[i].ucPdpId;
            pstCdsImsBearer->enBearerType   = IMSA_CDS_IMS_BEARER_TYPE_MEDIA;

            pstSetImsBearerReq->ulImsBearerNum++;
        }
    }

    /* 当IMS承载数大于0，则携带IMS端口号范围信息 */
    pstControlManager = IMSA_GetControlManagerAddress();
    if (pstSetImsBearerReq->ulImsBearerNum > 0)
    {
        pstSetImsBearerReq->stImsPortInfo.usMinImsPort
                = pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMinPort;
        pstSetImsBearerReq->stImsPortInfo.usMaxImsPort
                = pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMaxPort;
    }

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstSetImsBearerReq);
}

/*****************************************************************************
 Function Name  : IMSA_CdsMsgDistr()
 Description    : CDS消息分发函数
 Input          : VOS_VOID *pRcvMsg
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-20  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_CdsMsgDistr(const VOS_VOID *pRcvMsg )
{
    /* 定义消息头指针*/
    PS_MSG_HEADER_STRU          *pHeader = VOS_NULL_PTR;

    /* 获取消息头指针*/
    pHeader = (PS_MSG_HEADER_STRU *) pRcvMsg;

    switch(pHeader->ulMsgName)
    {
        /*读文件结果*/
        case ID_IMSA_CDS_SET_IMS_BEARER_CNF:
            IMSA_INFO_LOG("IMSA_CdsMsgDistr: ID_IMSA_CDS_SET_IMS_BEARER_CNF!");

            break;

        default:
            IMSA_ERR_LOG("IMSA_CdsMsgDistr: Msg id is err.");
            break;

    }
}

#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of ImsaProcCdsMsg.c */



