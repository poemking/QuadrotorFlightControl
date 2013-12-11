/*=====================================================================================================*/
/*=====================================================================================================*/
#include "stm32f4_system.h"
#include "QCopterFC.h"
#include "QCopterFC_ctrl.h"
#include "QCopterFC_ahrs.h"
#include "QCopterFC_transport.h"
#include "module_board.h"
#include "module_rs232.h"
#include "module_motor.h"
#include "module_sensor.h"
#include "module_nrf24l01.h"
/*=====================================================================================================*/

extern __IO uint16_t PWM1_InputCaptureValue ;
extern __IO uint16_t PWM2_InputCaptureValue ;
extern __IO uint16_t PWM3_InputCaptureValue ;
extern __IO uint16_t PWM4_InputCaptureValue ;
extern __IO uint16_t PWM5_InputCaptureValue ;
extern __IO int global_rc_thr;
extern __IO int global_rc_roll;
extern __IO int global_rc_pitch;
extern __IO int global_rc_yaw;

/*=====================================================================================================*/
void System_Init(void)
{
	SystemInit();

	LED_Config();
	KEY_Config();
	RS232_Config();
	Motor_Config();
	PWM_Capture_Config();
	Sensor_Config();
	nRF24L01_Config();

	PID_Init(&PID_Yaw);
	PID_Init(&PID_Roll);
	PID_Init(&PID_Pitch);

	PID_Pitch.Kp = +1.5f;
	PID_Pitch.Ki = +0.002f;
	PID_Pitch.Kd = +1.0f;

	PID_Roll.Kp  = +1.5f;
	PID_Roll.Ki  = +0.002f;
	PID_Roll.Kd  = +1.0f;

	PID_Yaw.Kp   = +0.0f;
	PID_Yaw.Ki   = +0.0f;
	PID_Yaw.Kd   = +0.0f;

	Delay_10ms(2);
}
/*=====================================================================================================*/
/*=====================================================================================================*/
int main(void)
{
	u8 Sta = ERROR;
	FSM_Mode FSM_State = FSM_Rx;

	/* System Init */
	System_Init();
	test_printf();

	/* Throttle Config */
	if (KEY == 1) {
		LED_B = 0;
		Motor_Control(PWM_MOTOR_MAX, PWM_MOTOR_MAX, PWM_MOTOR_MAX, PWM_MOTOR_MAX);
	}

	while (KEY == 1);

	LED_B = 1;
	Motor_Control(PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN);

	/* nRF Check */
	while (Sta == ERROR)
		Sta = nRF_Check();

	/* Sensor Init */
	if (Sensor_Init() == SUCCESS)
		LED_G = 0;

	Delay_10ms(10);

	/* Systick Config */
	if (SysTick_Config(SystemCoreClock / SampleRateFreg)) { // SampleRateFreg = 500 Hz
		while (1);
	}

	/* Wait Correction */
	while (SensorMode != Mode_Algorithm);

	/* Lock */
	LED_R = 1;
	LED_G = 1;
	LED_B = 1;

	while (!KEY) {
		LED_B = ~LED_B;
		Delay_10ms(1);
		Transport_Send(TxBuf[0]);
		printf("Roll%d,Pitch%d,Yaw%d,CH1 %u(%d),CH2 %u(%d),CH3 %u(%d),CH4 %u(%d),CH5 %u()\r\n",
		       (int)AngE.Roll, (int)AngE.Pitch, (int)AngE.Yaw,
		       PWM1_InputCaptureValue, global_rc_roll,
		       PWM2_InputCaptureValue, global_rc_pitch,
		       PWM3_InputCaptureValue, global_rc_thr,
		       PWM4_InputCaptureValue, global_rc_yaw,
		       PWM5_InputCaptureValue);
	}

	LED_B = 1;

	/* Final State Machine */
	while (1) {
		LED_G = ~LED_G;

		switch (FSM_State) {

		/************************** FSM Tx ****************************************/
		case FSM_Tx:
			// FSM_Tx
			nRF_TX_Mode();

			do {
				Sta = nRF_Tx_Data(TxBuf[0]);
			} while (Sta == MAX_RT);

			// FSM_Tx End
			FSM_State = FSM_Rx;
			break;

		/************************** FSM Rx ****************************************/
		case FSM_Rx:
			// FSM_Rx
			nRF_RX_Mode();
			Sta = nRF_Rx_Data(RxBuf[0]);

			if (Sta == RX_DR) {
				Transport_Recv(RxBuf[0]);
			}

			// FSM_Rx End
			FSM_State = FSM_CTRL;
			break;

		/************************** FSM CTRL **************************************/
		case FSM_CTRL:
			// FSM_CTRL
			CTRL_FlightControl();
			// FSM_CTRL End
			FSM_State = FSM_UART;
			break;

		/************************** FSM UART ***************************************/
		case FSM_UART:
			// FSM_USART
			RS232_VisualScope(USART3, TxBuf[0] + 20, 8);
			// FSM_USART End
			FSM_State = FSM_DATA;
			break;

		/************************** FSM DATA **************************************/
		case FSM_DATA:
			// FSM_DATA
			Transport_Send(TxBuf[0]);
			// FSM_DATA End
			FSM_State = FSM_Tx;
			break;
		}
	}
}
/*=====================================================================================================*/
/*=====================================================================================================*/
