

/*****************************************************************************
  1 头文件包含
**************************************************************************** */
#include "OamSpecTaskDef.h"
#include "SCMProc.h"
#include "SCMSoftDecode.h"
#include "ombufmngr.h"
#include "OmHdlcInterface.h"
#include "OmAppRl.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* lint -e767  */
#define    THIS_FILE_ID        PS_FILE_ID_SCM_SOFT_DECODE_C
/* lint +e767  */
#if ((OSA_CPU_ACPU == VOS_OSA_CPU)|| (VOS_WIN32 == VOS_OS_VER))
/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
/* 自旋锁，用来作OM数据接收的临界资源保护 */
VOS_SPINLOCK             g_stScmSoftDecodeDataRcvSpinLock;

/* HDLC控制结构 */
OM_HDLC_STRU             g_stScmHdlcSoftDecodeEntity;

/* SCM数据接收数据缓冲区 */
VOS_CHAR                 g_aucSCMDataRcvBuffer[SCM_DATA_RCV_PKT_SIZE];

/* SCM数据接收任务控制结构 */
SCM_DATA_RCV_CTRL_STRU   g_stSCMDataRcvTaskCtrlInfo;

SCM_SOFTDECODE_INFO_STRU   g_stScmSoftDecodeInfo;


/*****************************************************************************
  3 外部引用声明
*****************************************************************************/


/*****************************************************************************
  4 函数实现
*****************************************************************************/

VOS_UINT32 SCM_SoftDecodeCfgDataRcv(VOS_UINT8 *pucBuffer, VOS_UINT32 ulLen)
{
    VOS_UINT32                          ulRstl;
    VOS_ULONG                           ulLockLevel;

    VOS_SpinLockIntLock(&g_stScmSoftDecodeDataRcvSpinLock, ulLockLevel);

    ulRstl = SCM_SoftDecodeDataRcv(pucBuffer, ulLen);

    VOS_SpinUnlockIntUnlock(&g_stScmSoftDecodeDataRcvSpinLock, ulLockLevel);

    return ulRstl;
}


VOS_UINT32 SCM_SoftDecodeDataRcv(VOS_UINT8 *pucBuffer, VOS_UINT32 ulLen)
{
    VOS_INT32                           sRet;

    if (ulLen > (VOS_UINT32)OM_RingBufferFreeBytes(g_stSCMDataRcvTaskCtrlInfo.rngOmRbufId))
    {
        g_stScmSoftDecodeInfo.stRbInfo.ulBufferNotEnough++;

        return VOS_ERR;
    }

    sRet = OM_RingBufferPut(g_stSCMDataRcvTaskCtrlInfo.rngOmRbufId,
                            (VOS_CHAR *)pucBuffer,
                            (VOS_INT)ulLen);



    if (ulLen == (VOS_UINT32)sRet)
    {
        if (VOS_OK == VOS_SmV(g_stSCMDataRcvTaskCtrlInfo.SmID))
        {
            g_stScmSoftDecodeInfo.stPutInfo.ulDataLen += sRet;
            g_stScmSoftDecodeInfo.stPutInfo.ulNum++;

            return VOS_OK;
        }
    }

    g_stScmSoftDecodeInfo.stRbInfo.ulRingBufferPutErr++;

    return VOS_ERR;
}


VOS_VOID SCM_RcvDataDispatch(
    OM_HDLC_STRU                       *pstHdlcCtrl,
    VOS_UINT8                           ucDataType)
{
    /* TL数据 */
    if (SCM_DATA_TYPE_TL == ucDataType)
    {
        if (VOS_NULL_PTR != g_astSCMDecoderCbFunc[SOCP_DECODER_DST_CB_TL_OM])
        {
            /* TL不需要DATATYPE字段，回调时删除 */
            g_astSCMDecoderCbFunc[SOCP_DECODER_DST_CB_TL_OM](SOCP_DECODER_DST_LOM,
                                                    pstHdlcCtrl->pucDecapBuff + sizeof(SOCP_DATA_TYPE_ENUM_UIN8),
                                                    pstHdlcCtrl->ulInfoLen - sizeof(SOCP_DATA_TYPE_ENUM_UIN8),
                                                    VOS_NULL_PTR,
                                                    VOS_NULL);

        }

        return;
    }
    /* GU OM数据 */
    if (VOS_NULL_PTR != g_astSCMDecoderCbFunc[SOCP_DECODER_DST_CB_GU_OM])
    {
        g_astSCMDecoderCbFunc[SOCP_DECODER_DST_CB_GU_OM](SOCP_DECODER_DST_GUOM,
                                                    pstHdlcCtrl->pucDecapBuff + sizeof(SOCP_DATA_TYPE_ENUM_UIN8),
                                                    pstHdlcCtrl->ulInfoLen - sizeof(SOCP_DATA_TYPE_ENUM_UIN8),
                                                    VOS_NULL_PTR,
                                                    VOS_NULL);
    }
    return;
}
VOS_UINT32 SCM_SoftDecodeAcpuRcvData(
    OM_HDLC_STRU                       *pstHdlcCtrl,
    VOS_UINT8                          *pucData,
    VOS_UINT32                          ulLen)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucGutlType;
    VOS_UINT8                           ucChar;

    ulResult = VOS_ERR;

    for( i = 0; i < ulLen; i++ )
    {
        ucChar = (VOS_UINT8)pucData[i];

        ulResult = Om_HdlcDecap(pstHdlcCtrl, ucChar);

        if ( HDLC_SUCC == ulResult )
        {
            g_stScmSoftDecodeInfo.stHdlcDecapData.ulDataLen += pstHdlcCtrl->ulInfoLen;
            g_stScmSoftDecodeInfo.stHdlcDecapData.ulNum++;

            ucGutlType = pstHdlcCtrl->pucDecapBuff[0];

            SCM_RcvDataDispatch(pstHdlcCtrl, ucGutlType);
        }
        else if (HDLC_NOT_HDLC_FRAME == ulResult)
        {
            /*不是完整分帧,继续HDLC解封装*/
        }
        else
        {
            g_stScmSoftDecodeInfo.ulFrameDecapErr++;
        }
    }

    return VOS_OK;
}
VOS_UINT32 SCM_SoftDecodeCfgHdlcInit(OM_HDLC_STRU *pstHdlc)
{
    /* 申请用于HDLC解封装的缓存 */
    pstHdlc->pucDecapBuff    = (VOS_UINT8 *)VOS_MemAlloc(ACPU_PID_OM, STATIC_MEM_PT, SCM_DATA_RCV_PKT_SIZE);

    if (VOS_NULL_PTR == pstHdlc->pucDecapBuff)
    {
        vos_printf("SCM_SoftDecodeCfgHdlcInit: Alloc Decapsulate buffer fail!");

        return VOS_ERR;
    }

    /* HDLC解封装缓存长度赋值 */
    pstHdlc->ulDecapBuffSize = SCM_DATA_RCV_PKT_SIZE;

    /* 初始化HDLC解封装控制上下文 */
    Om_HdlcInit(pstHdlc);

    return VOS_OK;
}
VOS_VOID SCM_SoftDecodeCfgRcvSelfTask(
    VOS_UINT32                          ulPara1,
    VOS_UINT32                          ulPara2,
    VOS_UINT32                          ulPara3,
    VOS_UINT32                          ulPara4)
{
    VOS_INT32                           sRet;
    VOS_INT32                           lLen;
    VOS_INT32                           lRemainlen;
    VOS_INT32                           lReadLen;
    VOS_UINT32                          ulPktNum;
    VOS_UINT32                          i;
    VOS_ULONG                           ulLockLevel;

    /* make PC lint happy */
    ulPara1 = ulPara1;
    ulPara2 = ulPara2;
    ulPara3 = ulPara3;
    ulPara4 = ulPara4;


    for (;;)
    {
        if (VOS_OK != VOS_SmP(g_stSCMDataRcvTaskCtrlInfo.SmID, 0))
        {
            continue;
        }

        lLen = OM_RingBufferNBytes(g_stSCMDataRcvTaskCtrlInfo.rngOmRbufId);

        if (lLen <= 0)
        {
            continue;
        }

        ulPktNum = (VOS_UINT32)((lLen + SCM_DATA_RCV_PKT_SIZE - 1) / SCM_DATA_RCV_PKT_SIZE);

        lRemainlen = lLen;

        for (i = 0; i < ulPktNum; i++)
        {
            if (SCM_DATA_RCV_PKT_SIZE < lRemainlen)
            {
                lReadLen = SCM_DATA_RCV_PKT_SIZE;

                sRet = OM_RingBufferGet(g_stSCMDataRcvTaskCtrlInfo.rngOmRbufId,
                                        g_stSCMDataRcvTaskCtrlInfo.pucBuffer,
                                        SCM_DATA_RCV_PKT_SIZE);
            }
            else
            {
                lReadLen = lRemainlen;

                sRet = OM_RingBufferGet(g_stSCMDataRcvTaskCtrlInfo.rngOmRbufId,
                                        g_stSCMDataRcvTaskCtrlInfo.pucBuffer,
                                        lRemainlen);
            }

            if (sRet != lReadLen)
            {
                VOS_SpinLockIntLock(&g_stScmSoftDecodeDataRcvSpinLock, ulLockLevel);

                OM_RingBufferFlush(g_stSCMDataRcvTaskCtrlInfo.rngOmRbufId);

                VOS_SpinUnlockIntUnlock(&g_stScmSoftDecodeDataRcvSpinLock, ulLockLevel);

                g_stScmSoftDecodeInfo.stRbInfo.ulRingBufferFlush++;

                continue;
            }

            lRemainlen -= lReadLen;

            g_stScmSoftDecodeInfo.stGetInfo.ulDataLen += lReadLen;

            /* 调用HDLC解封装函数 */
            if (VOS_OK != SCM_SoftDecodeAcpuRcvData(&g_stScmHdlcSoftDecodeEntity,
                                                    (VOS_UINT8 *)g_stSCMDataRcvTaskCtrlInfo.pucBuffer,
                                                    (VOS_UINT32)lReadLen))
            {
                vos_printf("SCM_SoftDecodeCfgRcvSelfTask: SCM_SoftDecodeAcpuRcvData Fail");
            }

        }

#if (defined(DMT))
        return;
#endif
    }
}


VOS_UINT32 SCM_SoftDecodeCfgRcvTaskInit(VOS_VOID)
{
    VOS_UINT32                              ulRslt;

    /* 注册OM配置数据接收自处理任务 */
    ulRslt = VOS_RegisterSelfTaskPrio(ACPU_FID_OM,
                                      (VOS_TASK_ENTRY_TYPE)SCM_SoftDecodeCfgRcvSelfTask,
                                      SCM_DATA_RCV_SELFTASK_PRIO,
                                      SCM_OM_CFG_TASK_STACK_SIZE);
    if ( VOS_NULL_BYTE == ulRslt )
    {
        return VOS_ERR;
    }

    VOS_MemSet(&g_stScmSoftDecodeInfo, 0, sizeof(g_stScmSoftDecodeInfo));

    if (VOS_OK != VOS_SmCCreate("OMCF", 0, VOS_SEMA4_FIFO, &(g_stSCMDataRcvTaskCtrlInfo.SmID)))
    {
        vos_printf("SCM_SoftDecodeCfgRcvTaskInit: Error, OMCFG semCCreate Fail");

        g_stScmSoftDecodeInfo.stRbInfo.ulSemCreatErr++;

        return VOS_ERR;
    }

    if (VOS_OK != SCM_SoftDecodeCfgHdlcInit(&g_stScmHdlcSoftDecodeEntity))
    {
        vos_printf("SCM_SoftDecodeCfgRcvTaskInit: Error, HDLC Init Fail");

        g_stScmSoftDecodeInfo.ulHdlcInitErr++;

        return VOS_ERR;
    }

    g_stSCMDataRcvTaskCtrlInfo.rngOmRbufId = OM_RingBufferCreate(SCM_DATA_RCV_BUFFER_SIZE);

    if (VOS_NULL_PTR == g_stSCMDataRcvTaskCtrlInfo.rngOmRbufId)
    {
        vos_printf("SCM_SoftDecodeCfgRcvTaskInit: Error, Creat OMCFG ringBuffer Fail");

        g_stScmSoftDecodeInfo.stRbInfo.ulRingBufferCreatErr++;

        VOS_MemFree(ACPU_PID_OM, g_stScmHdlcSoftDecodeEntity.pucDecapBuff);

        return VOS_ERR;
    }

    g_stSCMDataRcvTaskCtrlInfo.pucBuffer = &g_aucSCMDataRcvBuffer[0];

    VOS_SpinLockInit(&g_stScmSoftDecodeDataRcvSpinLock);

    return VOS_OK;
}

VOS_VOID SCM_SoftDecodeInfoShow(VOS_VOID)
{
    vos_printf("\r\nSCM_SoftDecodeInfoShow:\r\n");

    vos_printf("\r\nSem Creat Error %d:\r\n",                   g_stScmSoftDecodeInfo.stRbInfo.ulSemCreatErr);
    vos_printf("\r\nSem Give Error %d:\r\n",                    g_stScmSoftDecodeInfo.stRbInfo.ulSemGiveErr);
    vos_printf("\r\nRing Buffer Creat Error %d:\r\n",           g_stScmSoftDecodeInfo.stRbInfo.ulRingBufferCreatErr);
    vos_printf("\r\nTask Id Error %d:\r\n",                     g_stScmSoftDecodeInfo.stRbInfo.ulTaskIdErr);
    vos_printf("\r\nRing Buffer not Enough %d:\r\n",            g_stScmSoftDecodeInfo.stRbInfo.ulBufferNotEnough);
    vos_printf("\r\nRing Buffer Flush %d:\r\n",                 g_stScmSoftDecodeInfo.stRbInfo.ulRingBufferFlush);
    vos_printf("\r\nRing Buffer Put Error %d:\r\n",             g_stScmSoftDecodeInfo.stRbInfo.ulRingBufferPutErr);

    vos_printf("\r\nRing Buffer Put Success Times %d:\r\n",     g_stScmSoftDecodeInfo.stPutInfo.ulNum);
    vos_printf("\r\nRing Buffer Put Success Bytes %d:\r\n",     g_stScmSoftDecodeInfo.stPutInfo.ulDataLen);

    vos_printf("\r\nRing Buffer Get Success Times %d:\r\n",     g_stScmSoftDecodeInfo.stGetInfo.ulNum);
    vos_printf("\r\nRing Buffer Get Success Bytes %d:\r\n",     g_stScmSoftDecodeInfo.stGetInfo.ulDataLen);

    vos_printf("\r\nHDLC Decode Success Times %d:\r\n",         g_stScmSoftDecodeInfo.stHdlcDecapData.ulNum);
    vos_printf("\r\nHDLC Decode Success Bytes %d:\r\n",         g_stScmSoftDecodeInfo.stHdlcDecapData.ulDataLen);

    vos_printf("\r\nHDLC Decode Error Times %d:\r\n",           g_stScmSoftDecodeInfo.ulFrameDecapErr);

    vos_printf("\r\nHDLC Init Error Times %d:\r\n",             g_stScmSoftDecodeInfo.ulHdlcInitErr);

    vos_printf("\r\nHDLC Decode Data Type Error Times %d:\r\n", g_stScmSoftDecodeInfo.ulDataTypeErr);
}

#endif

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif




