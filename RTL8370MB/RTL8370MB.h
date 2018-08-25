#ifndef _RTL_8370_MB_H_
#define _RTL_8370_MB_H_


#include "stm32f4x7_rtl8370mb_eth.h"
#include "rtk_types.h"
#include "rtk_error.h"

#define RTL8370MB_PHY_ADDRESS  	0x00				//PHYоƬ��ַ.
#define RTL8370MB_RST_PORT 		  GPIOD 			//��λ����
#define RTL8370MB_RST_PIN				GPIO_Pin_3

#ifdef __cplusplus
extern "C" {
#endif
 
extern __align(4)  ETH_DMADESCTypeDef DMARxDscrTab[ETH_RXBUFNB];			//��̫��DMA�������������ݽṹ��ָ��
extern __align(4)  ETH_DMADESCTypeDef DMATxDscrTab[ETH_TXBUFNB];			//��̫��DMA�������������ݽṹ��ָ�� 
extern __align(4)  uint8_t Rx_Buff[ETH_RX_BUF_SIZE*ETH_RXBUFNB]; 							//��̫���ײ���������buffersָ�� 
extern __align(4)  uint8_t Tx_Buff[ETH_TX_BUF_SIZE*ETH_TXBUFNB]; 							//��̫���ײ���������buffersָ��
extern ETH_DMADESCTypeDef  *DMATxDescToSet;			//DMA����������׷��ָ��
extern ETH_DMADESCTypeDef  *DMARxDescToGet; 		//DMA����������׷��ָ�� 
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;	//DMA�����յ���֡��Ϣָ��	
	
u8 RTL8370MB_SWITCH_Init(void);
u8 RTL8370MB_PHY_Init(void);
u8 RTL8370MB_SWITCH_CheatInit(void);
u8 RTL8370MB_Get_Speed(void);
u8 ETH_MACDMA_Config(void);
FrameTypeDef ETH_Rx_Packet(void);
u8 ETH_Tx_Packet(u16 FrameLength);
u32 ETH_GetCurrentTxBuffer(void);
u8 ETH_Mem_Malloc(void);
void ETH_Mem_Free(void);	
	
#ifdef __cplusplus
}
#endif


#endif // _RTL_8370_MB_H_
