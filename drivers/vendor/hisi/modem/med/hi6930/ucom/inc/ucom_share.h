/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : ucom_share.h
  �� �� ��   : ����
  ��    ��   : C00137131
  ��������   : 2012��3��12��
  ����޸�   :
  ��������   : ucom_share.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��3��12��
    ��    ��   : C00137131
    �޸�����   : �����ļ�

******************************************************************************/

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include  "med_drv_mb_hifi.h"
#include  "vos.h"


#ifndef __UCOM_SHARE_H__
#define __UCOM_SHARE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 �궨��
*****************************************************************************/

#define UCOM_GetCarmShareAddrStru()     (&g_stHifiShareAddr)

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
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/
extern CARM_HIFI_DYN_ADDR_SHARE_STRU                g_stHifiShareAddr;


/*****************************************************************************
  10 ��������
*****************************************************************************/

extern VOS_VOID UCOM_SHARE_Init(VOS_VOID);











#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of ucom_share.h */