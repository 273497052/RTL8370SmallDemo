#include "RTL8370MB.h"
#include "port.h"
#include "Timer.h"
#include "rtk_switch.h"
#include "stm32f4xx_rcc.h"
#include "interrupt.h"
#include "led.h"
#include "rtl8367c_asicdrv.h"
#include "rtk_cpu.h"

__align(4) ETH_DMADESCTypeDef DMARxDscrTab[ETH_RXBUFNB];	//��̫��DMA�������������ݽṹ��ָ��
__align(4) ETH_DMADESCTypeDef DMATxDscrTab[ETH_TXBUFNB];	//��̫��DMA�������������ݽṹ��ָ�� 
__align(4) uint8_t Rx_Buff[ETH_RX_BUF_SIZE*ETH_RXBUFNB]; 					//��̫���ײ���������buffersָ�� 
__align(4) uint8_t Tx_Buff[ETH_TX_BUF_SIZE*ETH_TXBUFNB]; 					//��̫���ײ���������buffersָ��

static void ETHERNET_NVICConfiguration(void);

u8 RTL8370MB_SWITCH_Init(void)
{
	//Boot up time for RTL8370MB
	BaseTimer::Instance()->delay_ms(1000);
	
	rtk_api_ret_t ret;
	
	
	ret = rtk_switch_init();
	if(ret != RT_ERR_OK)
		return 1;
	
	rtk_port_mac_ability_t pAbility;
	rtk_mode_ext_t pMode;
	u8 rval;
	
	rval=rtk_port_macForceLinkExt_get(EXT_PORT1,&pMode,&pAbility);	
	if(rval != RT_ERR_OK)
		return 2;
	pAbility.forcemode = 1;//MAC_FORCE
	pAbility.speed = 1;//PORT_SPEED_100M;
	pAbility.duplex = 1;//PORT_FULL_DUPLEX;
	pAbility.link = 1;//PORT_LINKUP;
	pAbility.nway = 0;//DISABLED;
	pAbility.txpause = 1;//ENABLED;
	pAbility.rxpause = 1;//ENABLED;
	rval=rtk_port_macForceLinkExt_set(EXT_PORT1,MODE_EXT_RMII_PHY,&pAbility);	
	rval=rtk_port_macForceLinkExt_get(EXT_PORT1,&pMode,&pAbility);	
	if(rval != RT_ERR_OK)
		return 3;
	rval=rtk_port_macForceLinkExt_get(EXT_PORT1,&pMode,&pAbility);	
	rval=rtk_port_macForceLinkExt_get(EXT_PORT1,&pMode,&pAbility);	
	if(rval != RT_ERR_OK)
		return 4;
	
		//LED test
	{
		/*
		rtk_portmask_t portmask;
		RTK_PORTMASK_PORT_SET(portmask,UTP_PORT0);
		RTK_PORTMASK_PORT_SET(portmask,UTP_PORT1);
		RTK_PORTMASK_PORT_SET(portmask,UTP_PORT2);
		RTK_PORTMASK_PORT_SET(portmask,UTP_PORT3);
		RTK_PORTMASK_PORT_SET(portmask,UTP_PORT4);
		RTK_PORTMASK_PORT_SET(portmask,UTP_PORT5);
		RTK_PORTMASK_PORT_SET(portmask,UTP_PORT6);
		RTK_PORTMASK_PORT_SET(portmask,UTP_PORT7);
		rtk_led_serialModePortmask_set(SERIAL_LED_0_1,&portmask);
		
		rtk_led_operation_set(LED_OP_SERIAL);
		rtk_led_serialMode_set(LED_ACTIVE_HIGH);
		rtk_led_OutputEnable_set(ENABLED);
		
		rtk_led_groupConfig_set(LED_GROUP_0,LED_CONFIG_ACT);
		rtk_led_groupConfig_set(LED_GROUP_1,LED_CONFIG_LINK_ACT);
		rtk_led_groupConfig_set(LED_GROUP_2,LED_CONFIG_LEDOFF);
		rval = rtk_led_enable_set(LED_GROUP_0,&portmask);
		rval = rtk_led_enable_set(LED_GROUP_1,&portmask);
		rval = rtk_led_enable_set(LED_GROUP_2,&portmask);
		*/
		
		/*
		rtk_portmask_t portmask;
		RTK_PORTMASK_PORT_SET(portmask,UTP_PORT0);
		RTK_PORTMASK_PORT_SET(portmask,UTP_PORT1);
		RTK_PORTMASK_PORT_SET(portmask,UTP_PORT2);
		RTK_PORTMASK_PORT_SET(portmask,UTP_PORT3);
		RTK_PORTMASK_PORT_SET(portmask,UTP_PORT4);
		RTK_PORTMASK_PORT_SET(portmask,UTP_PORT5);
		//RTK_PORTMASK_PORT_SET(portmask,UTP_PORT6);
		//RTK_PORTMASK_PORT_SET(portmask,UTP_PORT7);
		rtk_led_force_mode_t mode;
		rtk_led_operation_set(LED_OP_PARALLEL);
		//rval = rtk_led_modeForce_set(UTP_PORT2,LED_GROUP_0,LED_FORCE_ON);
		//rval = rtk_led_modeForce_set(UTP_PORT2,LED_GROUP_1,LED_FORCE_ON);
		//rval = rtk_led_modeForce_get(UTP_PORT1,LED_GROUP_0,&mode);
		//rval = rtk_led_blinkRate_set(LED_BLINKRATE_1024MS);
		//rval = rtk_led_modeForce_set(UTP_POT1,LED_GROUP_0,LED_FORCE_BLINK);
		//rval = rtk_led_modeForce_set(UTP_PORT1,LED_GROUP_1,LED_FORCE_BLINK);
		//rval = rtk_led_modeForce_set(UTP_PORT2,LED_GROUP_0,LED_FORCE_OFF);
		//rval = rtk_led_modeForce_set(UTP_PORT2,LED_GROUP_1,LED_FORCE_OFF);
		rtk_led_groupConfig_set(LED_GROUP_0,LED_CONFIG_ACT);
		rtk_led_groupConfig_set(LED_GROUP_1,LED_CONFIG_LINK_ACT);
		rval = rtk_led_enable_set(LED_GROUP_0,&portmask);
		rval = rtk_led_enable_set(LED_GROUP_1,&portmask);
		*/
	}
		
	/*
	rtk_uint32 value;
	
	rval= rtl8367c_setAsicReg(0x13c3, 0x08);
	rval= rtl8367c_setAsicReg(0x13c4, 0x1075);
	rval= rtl8367c_getAsicReg(0x13c3, &value);
	rval= rtl8367c_getAsicReg(0x13c4, &value);
	rval=rtk_port_macForceLinkExt_get(EXT_PORT0,&pMode,&pAbility);*/

	/*
	rtk_uint32 value;
	rval= rtl8367c_getAsicReg(0x1b00, &value);
	rval= rtl8367c_getAsicReg(0x1b03, &value);
	*/
	return 0;
}

u8 RTL8370MB_PHY_Init(void)
{
	u8 rval=0;
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOG , ENABLE);//ʹ��GPIOʱ�� RMII�ӿ�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);   //ʹ��SYSCFGʱ��
  
	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII); //MAC��PHY֮��ʹ��RMII�ӿ�

	/*������������ RMII�ӿ� 
	  ETH_MDIO -------------------------> PA2
	  ETH_MDC --------------------------> PC1
	  ETH_RMII_REF_CLK------------------> PA1
	  ETH_RMII_CRS_DV ------------------> PA7
	  ETH_RMII_RXD0 --------------------> PC4
	  ETH_RMII_RXD1 --------------------> PC5
	  ETH_RMII_TX_EN -------------------> PG11
	  ETH_RMII_TXD0 --------------------> PG13
	  ETH_RMII_TXD1 --------------------> PG14
	  ETH_RESET-------------------------> PD3*/
	
	  //����PA1 PA2 PA7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH); //���Ÿ��õ�����ӿ���
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

	//����PC1,PC4 and PC5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH); //���Ÿ��õ�����ӿ���
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);
                                
	//����PG11, PG14 and PG13 
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);
	
	//����PD3Ϊ�������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/*
	GPIO_ResetBits(RTL8370MB_RST_PORT,RTL8370MB_RST_PIN);					//Ӳ����λLAN8720
	BaseTimer::Instance()->delay_ms(50);
//	delay_ms(50);	
	GPIO_SetBits(RTL8370MB_RST_PORT,RTL8370MB_RST_PIN);				 	//��λ���� 
	*/
	ETHERNET_NVICConfiguration();
	rval=ETH_MACDMA_Config();
	return !rval;					//ETH�Ĺ���Ϊ:0,ʧ��;1,�ɹ�;����Ҫȡ��һ�� 
}

void ETHERNET_NVICConfiguration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;  //��̫���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0X00; //�жϼĴ�����2������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0X00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//�õ�8370���ٶ�ģʽ
//����ֵ:
//001:10M��˫��
//101:10Mȫ˫��
//010:100M��˫��
//110:100Mȫ˫��
//����:����.
u8 RTL8370MB_Get_Speed(void)
{
	u8 val = 0;
	val |= (1<<1);
	val |= (1<<2);
	return val;
	/*
	rtk_port_linkStatus_t linkStatus;
	rtk_port_speed_t speed;
	rtk_port_duplex_t duplex;
	rtk_api_ret_t ret = rtk_port_phyStatus_get(EXT_PORT1,&linkStatus,&speed,&duplex); //��RTL8370MB��port״̬��ȡĿǰ�ٶ�
	if(ret != RT_ERR_OK)
		return 1;
	
	u8 val = 0;
	
	if(linkStatus != PORT_LINKUP)
		return 2;
	
	switch(speed)
	{
		case PORT_SPEED_10M:
			val |= 1;
			break;
		case PORT_SPEED_100M:
			val |= (1<<1);
			break;
		default:
			return 3;
	}
	
	switch(duplex)
	{
		case PORT_HALF_DUPLEX:
			break;
		case PORT_FULL_DUPLEX:
			val |= (1<<2);
			break;
		default:
			return 4;
	}
	
	return val;
	*/
}

u8 ETH_MACDMA_Config(void)
{

	ETH_InitTypeDef ETH_InitStructure; 
	
	//ʹ����̫��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);
                        
	
	ETH_DeInit();  								//AHB����������̫��
	ETH_SoftwareReset();  						//�����������
	while (ETH_GetSoftwareResetStatus() == SET);//�ȴ��������������� 
	ETH_StructInit(&ETH_InitStructure); 	 	//��ʼ������ΪĬ��ֵ  

	///����MAC�������� 
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;   			//������������Ӧ����
	ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;					//�رշ���
	ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable; 		//�ر��ش�����
	ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable; 	//�ر��Զ�ȥ��PDA/CRC���� 
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;						//�رս������е�֡
	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;//����������й㲥֡
	ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;			//�رջ��ģʽ�ĵ�ַ����  
	ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;//�����鲥��ַʹ��������ַ����   
	ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;	//�Ե�����ַʹ��������ַ���� 
	ETH_InitStructure.ETH_InterFrameGap = ETH_InterFrameGap_40Bit;
#ifdef CHECKSUM_BY_HARDWARE
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable; 			//����ipv4��TCP/UDP/ICMP��֡У���ж��   
#endif
	//������ʹ��֡У���ж�ع��ܵ�ʱ��һ��Ҫʹ�ܴ洢ת��ģʽ,�洢ת��ģʽ��Ҫ��֤����֡�洢��FIFO��,
	//����MAC�ܲ���/ʶ���֡У��ֵ,����У����ȷ��ʱ��DMA�Ϳ��Դ���֡,����Ͷ�������֡
	ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; //��������TCP/IP����֡
	ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;     //�����������ݵĴ洢ת��ģʽ    
	ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;   //�����������ݵĴ洢ת��ģʽ  

	ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;     	//��ֹת������֡  
	ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;	//��ת����С�ĺ�֡ 
	ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;  		//�򿪴���ڶ�֡����
	ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;  	//����DMA����ĵ�ַ���빦��
	ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;            			//�����̶�ͻ������    
	ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;     		//DMA���͵����ͻ������Ϊ32������   
	ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;			//DMA���յ����ͻ������Ϊ32������
	ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
	
	//rval=ETH_RTL8370MB_Init(&ETH_InitStructure,RTL8370MB_PHY_ADDRESS);		//����ETH
	
	u8 rval=0;
	rval=ETH_RTL8370MB_Init(&ETH_InitStructure,RTL8370MB_PHY_ADDRESS);
	if(rval==ETH_SUCCESS)//���óɹ�
	{
		ETH_DMAITConfig(ETH_DMA_IT_NIS|ETH_DMA_IT_R,ENABLE);  	//ʹ����̫�������ж�	
	}
	
	return rval;
}

extern void lwip_pkt_handle(void);		//��lwip_comm.c���涨��
//��̫���жϷ�����
extern "C"{
void ETH_IRQHandler(void)
{
	while(ETH_GetRxPktSize(DMARxDescToGet)!=0) 	//����Ƿ��յ����ݰ�
	{ 
		lwip_pkt_handle();		
	}
	ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
}  
}

//����һ���������ݰ�
//����ֵ:�������ݰ�֡�ṹ��
FrameTypeDef ETH_Rx_Packet(void)
{ 
	u32 framelength=0;
	FrameTypeDef frame={0,0};   
	//��鵱ǰ������,�Ƿ�����ETHERNET DMA(���õ�ʱ��)/CPU(��λ��ʱ��)
	if((DMARxDescToGet->Status&ETH_DMARxDesc_OWN)!=(u32)RESET)
	{	
		frame.length=ETH_ERROR; 
		if ((ETH->DMASR&ETH_DMASR_RBUS)!=(u32)RESET)  
		{ 
			ETH->DMASR = ETH_DMASR_RBUS;//���ETH DMA��RBUSλ  
			ETH->DMARPDR=0;//�ָ�DMA����
		}
		return frame;//����,OWNλ��������
	}  
	if(((DMARxDescToGet->Status&ETH_DMARxDesc_ES)==(u32)RESET)&& 
	((DMARxDescToGet->Status & ETH_DMARxDesc_LS)!=(u32)RESET)&&  
	((DMARxDescToGet->Status & ETH_DMARxDesc_FS)!=(u32)RESET))  
	{       
		framelength=((DMARxDescToGet->Status&ETH_DMARxDesc_FL)>>ETH_DMARxDesc_FrameLengthShift)-4;//�õ����հ�֡����(������4�ֽ�CRC)
 		frame.buffer = DMARxDescToGet->Buffer1Addr;//�õ����������ڵ�λ��
	}else framelength=ETH_ERROR;//����  
	frame.length=framelength; 
	frame.descriptor=DMARxDescToGet;  
	//����ETH DMAȫ��Rx������Ϊ��һ��Rx������
	//Ϊ��һ��buffer��ȡ������һ��DMA Rx������
	DMARxDescToGet=(ETH_DMADESCTypeDef*)(DMARxDescToGet->Buffer2NextDescAddr);   
	return frame;  
}
//����һ���������ݰ�
//FrameLength:���ݰ�����
//����ֵ:ETH_ERROR,����ʧ��(0)
//		ETH_SUCCESS,���ͳɹ�(1)
u8 ETH_Tx_Packet(u16 FrameLength)
{   
	//��鵱ǰ������,�Ƿ�����ETHERNET DMA(���õ�ʱ��)/CPU(��λ��ʱ��)
	if((DMATxDescToSet->Status&ETH_DMATxDesc_OWN)!=(u32)RESET)return ETH_ERROR;//����,OWNλ�������� 
 	DMATxDescToSet->ControlBufferSize=(FrameLength&ETH_DMATxDesc_TBS1);//����֡����,bits[12:0]
	DMATxDescToSet->Status|=ETH_DMATxDesc_LS|ETH_DMATxDesc_FS;//�������һ���͵�һ��λ����λ(1������������һ֡)
  	DMATxDescToSet->Status|=ETH_DMATxDesc_OWN;//����Tx��������OWNλ,buffer�ع�ETH DMA
	if((ETH->DMASR&ETH_DMASR_TBUS)!=(u32)RESET)//��Tx Buffer������λ(TBUS)�����õ�ʱ��,������.�ָ�����
	{ 
		ETH->DMASR=ETH_DMASR_TBUS;//����ETH DMA TBUSλ 
		ETH->DMATPDR=0;//�ָ�DMA����
	} 
	//����ETH DMAȫ��Tx������Ϊ��һ��Tx������
	//Ϊ��һ��buffer����������һ��DMA Tx������ 
	DMATxDescToSet=(ETH_DMADESCTypeDef*)(DMATxDescToSet->Buffer2NextDescAddr);    
	return ETH_SUCCESS;   
}
//�õ���ǰ��������Tx buffer��ַ
//����ֵ:Tx buffer��ַ
u32 ETH_GetCurrentTxBuffer(void)
{  
  return DMATxDescToSet->Buffer1Addr;//����Tx buffer��ַ  
}

//ΪETH�ײ����������ڴ�
//����ֵ:0,����
//    ����,ʧ��
u8 ETH_Mem_Malloc(void)
{ 
//	DMARxDscrTab = (ETH_DMADESCTypeDef*)mymalloc(SRAMIN,ETH_RXBUFNB*sizeof(ETH_DMADESCTypeDef));//�����ڴ�
//	DMATxDscrTab = (ETH_DMADESCTypeDef*)mymalloc(SRAMIN,ETH_TXBUFNB*sizeof(ETH_DMADESCTypeDef));//�����ڴ�  
//	Rx_Buff = (uint8_t*)mymalloc(SRAMIN,ETH_RX_BUF_SIZE*ETH_RXBUFNB);	//�����ڴ�
//	Tx_Buff = (uint8_t*)mymalloc(SRAMIN,ETH_TX_BUF_SIZE*ETH_TXBUFNB);	//�����ڴ�
//	if(!DMARxDscrTab||!DMATxDscrTab||!Rx_Buff||!Tx_Buff)
//	{
//		ETH_Mem_Free();
//		return 1;	//����ʧ��
//	}	
	return 0;		//����ɹ�
}

//�ͷ�ETH �ײ�����������ڴ�
void ETH_Mem_Free(void)
{ 
//	myfree(SRAMIN,DMARxDscrTab);//�ͷ��ڴ�
//	myfree(SRAMIN,DMATxDscrTab);//�ͷ��ڴ�
//	myfree(SRAMIN,Rx_Buff);		//�ͷ��ڴ�
//	myfree(SRAMIN,Tx_Buff);		//�ͷ��ڴ�  
}

u8 RTL8370MB_SWITCH_CheatInit(void)
{
	/*
	//MCO2 init
	RCC_PLLI2SCmd(DISABLE);
	RCC_PLLI2SConfig((uint32_t)200, (uint32_t)4);
	RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);
	RCC_PLLI2SCmd(ENABLE);
	
	GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��PC

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//PC9
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù��ܿ���

  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz


  GPIO_Init(GPIOC, &GPIO_InitStructure);


	RCC_MCO2Config(RCC_MCO2Source_PLLI2SCLK,RCC_MCO2Div_1);
	*/
	return 0;
}

