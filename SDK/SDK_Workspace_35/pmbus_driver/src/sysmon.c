#include <stdio.h>
#include <xparameters.h>

#include "../include/sysmon.h"

/**
 * Copied from a Xilinx demo project
 *
 */

void init_system_monitor(XSysMon_Config *ConfigPtr, XSysMon SysMonInst) {
	int channels;

	// Setup SysMon Using the SysMon Driver Functions
	ConfigPtr = XSysMon_LookupConfig(SYSMON_DEVICE_ID); // Initialize the driver
	XSysMon_CfgInitialize(&SysMonInst, ConfigPtr, ConfigPtr->BaseAddress);
	channels = XSM_SEQ_CH_CALIB | XSM_SEQ_CH_TEMP | // Use the Cal, Temp, Vccint, Vccaux in the sequence, plus vpvn, and 2 aux channels
			XSM_SEQ_CH_VCCAUX | XSM_SEQ_CH_VCCINT | XSM_SEQ_CH_VPVN
			| XSM_SEQ_CH_AUX12 | XSM_SEQ_CH_AUX13;

	XSysMon_SetAdcClkDivisor(&SysMonInst, 25); // Set sysmon clock to approx 5MHz  (Bus (125MHz)/25 = 5)
	XSysMon_SetSequencerMode(&SysMonInst, XSM_SEQ_MODE_SINGCHAN); // Switch SysMon out of sequencer mode (before configuring registers)
	XSysMon_SetSeqChEnables(&SysMonInst, channels); // Enable these channels in the sequence
	XSysMon_SetSeqAvgEnables(&SysMonInst, channels); // Enable averaging on these channels
	XSysMon_SetSeqInputMode(&SysMonInst, XSM_SEQ_CH_VPVN); // Turn on Bipolar Mode for vp/vn current sensing (improves accuracy)
	XSysMon_SetAvg(&SysMonInst, XSM_AVG_16_SAMPLES); // Averaging on 16 samples
	XSysMon_SetAlarmEnables(&SysMonInst, 0xF); // Turn on all Alarm checking
	SysMon_Set(XSM_CFR1_OFFSET, SysMon_Get(XSM_CFR1_OFFSET) | 0xF0 ); // Turn on all calibration Note: uses low-level driver functions
	XSysMon_SetSequencerMode(&SysMonInst, XSM_SEQ_MODE_CONTINPASS); // Switch on Sequencer to begin

	//Configure the SysMon Alarm Thresholds
	SysMon_Set(XSM_ATR_TEMP_UPPER_OFFSET, TEMP_ITX(TEMP_ALARM_ON)); // Temperature upper alarm
	SysMon_Set(XSM_ATR_TEMP_LOWER_OFFSET, TEMP_ITX(TEMP_ALARM_OFF)); // Temperature lower alarm
	SysMon_Set(XSM_ATR_VCCINT_UPPER_OFFSET, SPLY_ITX(1.05f)); // Vccint upper alarm
	SysMon_Set(XSM_ATR_VCCINT_LOWER_OFFSET, SPLY_ITX(0.95f)); // Vccint lower alarm
	SysMon_Set(XSM_ATR_VCCAUX_UPPER_OFFSET, SPLY_ITX(2.625f)); // Vccaux upper alarm
	SysMon_Set(XSM_ATR_VCCAUX_LOWER_OFFSET, SPLY_ITX(2.375f)); // Vccaux lower alarm
}

// This calculates and shows the data that is to be displayed
int show_sysmon_data(void) {
	int temperature, vccint, vccaux, vshunt, vboard, vboard_shunt, alm = 0;
	float temp_f, vccint_f, vccaux_f, vshunt_mv_f, vboard_f, iboard_f,
			iccint_f, pint_f;

	// Read the sensors
	temperature = SysMon_Get(XSM_TEMP_OFFSET);
	vccint = SysMon_Get(XSM_VCCINT_OFFSET);
	vccaux = SysMon_Get(XSM_VCCAUX_OFFSET);
	vshunt = SysMon_Get(R_SHUNT_CHAN);
	vboard = SysMon_Get(V_BOARD_CHAN);
	vboard_shunt = SysMon_Get(I_BOARD_CHAN);

	// Convert the sensor data into human readable format, using the sensor transfer functions
	temp_f = TEMP_TX(temperature);
	vccint_f = SPLY_TX(vccint);
	vccaux_f = SPLY_TX(vccaux);
	vshunt_mv_f = ((float) vshunt * 1000.0f) / 65536.0f;
	vboard_f = ((float) vboard * V_BOARD_FS) / 65536.0f;

	// Calculate the current, and hence the power
	iccint_f = (vshunt_mv_f * R_SHUNT_FACTOR) / 1000.0f; // I = V / R
	pint_f = iccint_f * vccint_f; // P = V * I

	iboard_f = ((float) vboard_shunt / (I_BOARD_GAIN * I_BOARD_RSHUNT))
			/ 65536.0f; // I = (V / gain) / R

	// Generate the text for the normal sensors	
	xil_printf("====== SysMon Internal Sensors ======\r\n");
	xil_printf("Temperature: %0d.%01d C\r\n", I_PART(temp_f),
			F_PART(temp_f,10.0f));
	xil_printf("Vccint     : %0d.%03d V\r\n", I_PART(vccint_f),
			F_PART(vccint_f,1000.0f));
	//xil_printf("Vccaux     : %0d.%03d V\r\n", I_PART(vccaux_f), F_PART(vccaux_f,1000.0f) );
	//xil_printf(" \r\n" );	
	//xil_printf("====== Board Sensors ======\r\n" );	
	//xil_printf("12V Supply:  %0d.%03d V\r\n", I_PART(vboard_f), F_PART(vboard_f,1000.0f) );	
	//xil_printf("12V Current: %0d.%03d A\r\n", I_PART(iboard_f), F_PART(iboard_f,1000.0f) );	
	//xil_printf(" \r\n" );
	xil_printf("===== Vccint Power Measurement =====\r\n");
	xil_printf("Vshunt     : %3d.%01d mV\r\n", I_PART(vshunt_mv_f),
			F_PART(vshunt_mv_f, 10.0f));
	xil_printf("Iccint = Vshunt/5mOhm : %0d.%03d A\r\n", I_PART(iccint_f),
			F_PART(iccint_f, 1000.0f));
	xil_printf("Pint = Iccint * Vccint : %0d.%03d W\r\n", I_PART(pint_f),
			F_PART(pint_f, 1000.0f));

	// Read the alarms and output them to the LEDs
	alm = SysMon_Get(XSM_AOR_OFFSET); // Reads the alarm status.

	xil_printf("%s\r\n",
			(alm & XSM_AOR_TEMP_MASK) ? ((alm & (XSM_AOR_VCCINT_MASK
					| XSM_AOR_VCCAUX_MASK)) ? "** Voltage,Temp **\r\n"
					: "** Temp **\r\n") : (alm & (XSM_AOR_VCCINT_MASK
					| XSM_AOR_VCCAUX_MASK)) ? "** Voltage **\r\n" : "OK\r\n");

	return 0;
}

