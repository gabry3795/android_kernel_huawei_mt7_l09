/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : om_msghook.h
  �� �� ��   : ����
  ��    ��   : ��ׯ�� 59026
  ��������   : 2011��5��31��
  ����޸�   :
  ��������   : om_msghook.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2011��5��31��
    ��    ��   : ��ׯ�� 59026
    �޸�����   : �����ļ�

******************************************************************************/

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "vos.h"
#include "om_comm.h"


#ifndef __OM_MSGHOOK_H__
#define __OM_MSGHOOK_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define OM_MSGHOOK_TENTH_SECOND_PER20MS                     (200)               /* ÿ20ms��0.1ms�ĸ��� */

/* ��ȡ�����Ϣ��ȡʹ�ܱ�־ */
#define OM_MSGHOOK_GetMsgHookEnable()                       \
                    (g_stOmMsghook.uhwVosMsgHookEnable)

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/


/*****************************************************************************
  4 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
  5 ��Ϣ����
*****************************************************************************/

/*****************************************************************************
  6 STRUCT����
*****************************************************************************/
/*****************************************************************************
 ʵ������  : OM_MSGHOOK_STRU
 ��������  : ��ά�ɲ�ģ��������ƽṹ
 *****************************************************************************/
typedef struct
{
    OM_SWITCH_ENUM_UINT16               uhwVosMsgHookEnable;
    VOS_UINT16                          uhwReserved;
}OM_MSGHOOK_STRU;

/*****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/

extern VOS_UINT32 OM_MSGHOOK_DefaultCfg(CODEC_MSG_HOOK_CONFIG_STRU *pstMsgHookCfg);
extern VOS_VOID OM_MSGHOOK_Init(VOS_VOID);
extern VOS_VOID OM_MSGHOOK_SetMsgHookEnable(VOS_UINT16 uhwEnable);
extern VOS_UINT32 OM_MSGHOOK_MsgCfgMsgHookReq(VOS_VOID *pvOsaMsg);
extern VOS_VOID OM_MSGHOOK_VosMsgHook(VOS_VOID *pvOsaMsg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of om_msghook.h */