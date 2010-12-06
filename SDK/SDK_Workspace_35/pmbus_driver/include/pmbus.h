#ifndef PMBUS_LIB
#define PMBUS_LIB


#define READBYTE      0
#define READWORD      2
#define WRITEBYTE     4
#define SENDBYTE      6
#define WRITEWORD     8

#define PEC_PASS           1
#define PEC_FAIL           0
#define CRC8_POLY	   0x07
#define CRC8_INIT_REM      0x0

//GROUP#0
// read byte

#define CAPABILITY                          1
#define STATUS_BYTE                         2
#define STATUS_VOUT                         3
#define STATUS_IOUT                         4
#define STATUS_INPUT                        5
#define STATUS_TEMEPERATURE                 6
#define STATUS_CML                          7
#define STATUS_OTHER                        8
#define STATUS_MFR_SPECIFIC                 9
#define STATUS_FANS_1_2                     10
#define STATUS_FANS_3_4                     11
#define PMBUS_REVISION                      12

// GROUP #2 
// read word

#define STATUS_WORD                         13
#define READ_VIN                            14
#define READ_IIN                            15
#define READ_VCAP                           16
#define READ_VOUT                           17
#define READ_IOUT                           18
#define READ_TEMPERATURE_1                  19
#define READ_TEMPERATURE_2                  20
#define READ_TEMPERATURE_3                  21
#define READ_FAN_SPEED_1                    22
#define READ_FAN_SPEED_2                    23
#define READ_FAN_SPEED_3                    24
#define READ_FAN_SPEED_4                    25
#define READ_DUTY_CYCLE                     26
#define READ_FREQUENCY                      27
#define READ_POUT                           28
#define READ_PIN                            29
#define MFR_VIN_MIN                         30
#define MFR_VIN_MAX                         31
#define MFR_IIN_MAX                         32
#define MFR_PIN_MAX                         33
#define MFR_VOUT_MIN                        34
#define MFR_VOUT_MAX                        35
#define MFR_IOUT_MAX                        36
#define MFR_POUT_MAX                        37
#define MFR_TAMBIENT_MAX                    38
#define MFR_TAMBIENT_MIN                    39

// GROUP #4
// write byte

#define STORE_DEFAULT_CODE                  40
#define RESTORE_DEFAULT_CODE                41
#define STORE_USER_CODE                     42
#define RESTORE_USER_CODE                   43

// GROUP#6
// send byte

#define CLEAR_FAULTS                        44
#define STORE_DEFAULT_ALL                   45
#define RESTORE_DEFAULT_ALL                 46
#define STORE_USER_ALL                      47
#define RESTORE_USER_ALL                    48

//GROUP#0/4
// read/write byte
#define PAGE_GENERAL_CALL                  49
#define OPERATION                           50
#define ON_OFF_CONFIG                       51
#define PHASE                               52
#define WRITE_PROTECT                       53
#define VOUT_MODE                           54
#define FAN_CONFIG_1_2                      55
#define FAN_CONFIG_3_4                      56
#define VOUT_OV_FAULT_RESPONSE              57
#define VOUT_UV_FAULT_RESPONSE              58
#define IOUT_OC_FAULT_RESPONSE              59
#define IOUT_0C_LV_FAULT_RESPONSE           60
#define IOUT_UC_FAULT_RESPONSE              61
#define OT_FAULT_RESPONSE                   62
#define UT_FAULT_RESPONSE                   63
#define VIN_OV_FAULT_RESPONSE               64
#define VIN_UV_FAULT_RESPONSE               65
#define IIN_OC_FAULT_RESPONSE               66
#define TON_MAX_FAULT_RESPONSE              67
#define POUT_OP_FAULT_RESPONSE              68

//GROUP#2/8
// read/write word

#define VOUT_COMMAND                        69
#define VOUT_TRIM                           70
#define VOUT_CAL_OFFSET                     71
#define VOUT_MAX                            72
#define VOUT_MARGIN_HIGH                    73
#define VOUT_MARGIN_LOW                     74
#define VOUT_TRANSITION_RATE                75
#define VOUT_DROOP                          76
#define VOUT_SCALE_LOOP                     77
#define VOUT_SCALE_MONITOR                  78
#define POUT_MAX                            79
#define MAX_DUTY                            80
#define FREQUENCY_SWITCH                    81
#define VIN_ON                              82
#define VIN_OFF                             83
#define INTERLEAVE                          84
#define IOUT_CAL_GAIN                       85
#define IOUT_CAL_OFFSET                     86
#define FAN_COMMAND_1                       87
#define FAN_COMMAND_2                       88
#define FAN_COMMAND_3                       89
#define FAN_COMMAND_4                       90
#define VOUT_OV_FAULT_LIMIT                 91
#define VOUT_OV_WARN_LIMIT                  92
#define VOUT_UV_WARN_LIMIT                  93
#define VOUT_UV_FAULT_LIMIT                 94
#define IOUT_OC_FAULT_LIMIT                 95
#define IOUT_OC_LV_FAULT_LIMIT              96
#define IOUT_OC_WARN_LIMIT                  97
#define IOUT_UC_FAULT_LIMIT                 98
#define OT_FAULT_LIMIT                      99
#define OT_WARN_LIMIT                       100
#define UT_WARN_LIMIT                       101
#define UT_FAULT_LIMIT                      102
#define VIN_OV_FAULT_LIMIT                  103
#define VIN_OV_WARN_LIMIT                   104
#define VIN_UV_WARN_LIMIT                   105
#define VIN_UV_FAULT_LIMIT                  106
#define IIN_OC_FAULT_LIMIT                  107
#define IIN_OC_WARN_LIMIT                   108
#define POWER_GOOD_ON                       109
#define POWER_GOOD_OFF                      110
#define TON_DELAY                           111
#define TON_RISE                            112
#define TON_MAX_FAULT_LIMIT                 113
#define TOFF_DELAY                          114
#define TOFF_FALL                           115
#define TOFF_MAX_WARN_LIMIT                 116
#define POUT_OP_FAULT_LIMIT                 117
#define POUT_OP_WARN_LIMIT                  118
#define PIN_OP_WARN_LIMIT                   119 



/* Added by Mohamed Bamakhrama */
#include <xparameters.h>
#include <xiic.h>
#include <xbasic_types.h>
#include <stdio.h>

#define V_SCALE	(float)(1.0f/4096.0f)

/* Hardware Aliases */
#define PMBUS_BASE_ADDRESS    XPAR_PMBUS_IIC_BASEADDR  	/* Base Address of the IIC controller connected to the board PMBus */
#define PMBUS_MAIN_SPLY_ADDR  (52)						/* The IIC address of the PMBus controller for the main supplies */ 
#define PMBUS_MGT_SPLY_ADDR   (53)						/* The IIC address of the PMBus controller for the MGT supplies */  
#define VCCINT_RAIL           (0)						/* The Output of the PMBus controller connected to vccint (i.e. page) */
#define VCCAUX_RAIL           (2)						/* The Output of the PMBus controller connected to vccint */

/* PMBus Commands */  
#define PMBC_PAGE             (0x0)						/* command allows the output rail to be selected */
#define PMBC_OPERATION        (0x1)						/* command allows for supply marining, etc */
#define PMBC_REVISION         (0x98)					/* this command returns the PMBus Revision */

/* PMBus Command BitMasks */
#define PMBB_MARGIN_NONE	  (0x80)					/* Disables Margining */
#define PMBB_MARGIN_LO        (0x94)					/* Margins the supply low (Ignore Faults) */
#define PMBB_MARGIN_HI        (0xA4)					/* Margins the supply high (Ignore Faults) */

#define PMBUS_NOT_BUSY		  (XIIC_SR_RX_FIFO_EMPTY_MASK | XIIC_SR_TX_FIFO_EMPTY_MASK)


#ifdef PMBUS_DEBUG
#define pmbus_debug(format, arg...) xil_printf(__FILE__ ":"  "%d: " format "\n" ,__LINE__, ## arg)
#else
#define pmbus_debug(...) ;
#endif


/**
 * Functions
 */

int PMBusInit(void);

void i2c_wait_until_ready(void);

u8 PMBus_PEC(u8 Command_Byte, u8 RWFlag,unsigned int Message,u8 *Received_Value);

u8 PMBus(u8 Command_Byte, u8 RWFlag, unsigned int Message, u8 *Received_Value);





#endif
