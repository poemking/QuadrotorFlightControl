/*=====================================================================================================*/
/*=====================================================================================================*/
#include "QuadCopterConfig.h"
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LED_Config
**功能 : LED 設定 & 配置
**輸入 : None
**輸出 : None
**使用 : LED_Config();
**=====================================================================================================*/
/*=====================================================================================================*/
void LED_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOC, ENABLE);

	/* LED_R PC13 */  /* LED_G PC14 */  /* LED_B PC15 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStruct);



	LED_G = 1;
	LED_R = 1;
	LED_B = 1;
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : KEY_Config
**功能 : KEY 設定 & 配置
**輸入 : None
**輸出 : None
**使用 : KEY_Config();
**=====================================================================================================*/
/*=====================================================================================================*/
void KEY_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* KEY PB2 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}
/*=====================================================================================================*/
/*=====================================================================================================*/
