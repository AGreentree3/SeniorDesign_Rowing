/******************************************************************************

 @file       row_Tracker.c

 @brief This file contains the Simple Peripheral sample application for use
        with the CC2650 Bluetooth Low Energy Protocol Stack.

 Group: CMCU, SCS
 Target Device: CC2640R2

 ******************************************************************************
 
 Copyright (c) 2013-2018, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: simplelink_cc2640r2_sdk_02_30_00_28
 Release Date: 2018-10-15 18:14:03
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <string.h>
#include <math.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/drivers/PIN.h>
#include <ti/display/Display.h>
#include <aon_ioc.h>

#if defined( USE_FPGA ) || defined( DEBUG_SW_TRACE )
#include <driverlib/ioc.h>
#endif // USE_FPGA | DEBUG_SW_TRACE

#include <icall.h>
#include "util.h"
#include "att_rsp.h"

// Include Drivers
#include <ti/drivers/I2C.h>
#include <ti/drivers/pin/PINCC26xx.h>
#include <SparkFun_BNO080_Library.h>

/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include "devinfoservice.h"
#include "simple_gatt_profile.h"
#include "ll_common.h"

#include "peripheral.h"



#ifdef USE_RCOSC
#include "rcosc_calibration.h"
#endif //USE_RCOSC

#include "board_key.h"

#include "board.h"

#include <row_tracker.h>

/*********************************************************************
 * CONSTANTS
 */

// Advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL          160

// General discoverable mode: advertise indefinitely
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL

// Minimum connection interval (units of 1.25ms, 80=100ms) for automatic
// parameter update request
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     80

// Maximum connection interval (units of 1.25ms, 800=1000ms) for automatic
// parameter update request
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     800

// Slave latency to use for automatic parameter update request
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms, 1000=10s) for automatic parameter
// update request
#define DEFAULT_DESIRED_CONN_TIMEOUT          1000

// After the connection is formed, the peripheral waits until the central
// device asks for its preferred connection parameters
#define DEFAULT_ENABLE_UPDATE_REQUEST         GAPROLE_LINK_PARAM_UPDATE_WAIT_REMOTE_PARAMS

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         6

// How often to perform periodic event (in msec)
#define SBP_PERIODIC_EVT_PERIOD               1

// How often to perform state machine periodic event (in msec)
#define STATE_MACHINE_EVT_PERIOD              8

// Application specific event ID for HCI Connection Event End Events
#define SBP_HCI_CONN_EVT_END_EVT              0x0001

// Type of Display to open
#if !defined(Display_DISABLE_ALL)
  #if defined(BOARD_DISPLAY_USE_LCD) && (BOARD_DISPLAY_USE_LCD!=0)
    #define SBP_DISPLAY_TYPE Display_Type_LCD
  #elif defined (BOARD_DISPLAY_USE_UART) && (BOARD_DISPLAY_USE_UART!=0)
    #define SBP_DISPLAY_TYPE Display_Type_UART
  #else // !BOARD_DISPLAY_USE_LCD && !BOARD_DISPLAY_USE_UART
    #define SBP_DISPLAY_TYPE 0 // Option not supported
  #endif // BOARD_DISPLAY_USE_LCD && BOARD_DISPLAY_USE_UART
#else // BOARD_DISPLAY_USE_LCD && BOARD_DISPLAY_USE_UART
  #define SBP_DISPLAY_TYPE 0 // No Display
#endif // !Display_DISABLE_ALL

// Task configuration
#define SBP_TASK_PRIORITY                     1

#ifndef SBP_TASK_STACK_SIZE
#define SBP_TASK_STACK_SIZE                   644
#endif

// Application events
#define SBP_STATE_CHANGE_EVT                  0x0001
#define SBP_CHAR_CHANGE_EVT                   0x0002
#define SBP_PAIRING_STATE_EVT                 0x0004
#define SBP_PASSCODE_NEEDED_EVT               0x0008
#define SBP_CONN_EVT                          0x0010

// Internal Events for RTOS application
#define SBP_ICALL_EVT                         ICALL_MSG_EVENT_ID // Event_Id_31
#define SBP_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // Event_Id_30
#define SBP_PERIODIC_EVT                      Event_Id_00
#define STATE_MACHINE_EVT                     Event_Id_01

// Bitwise OR of all events to pend on
#define SBP_ALL_EVENTS                        (SBP_ICALL_EVT        | \
                                               SBP_QUEUE_EVT        | \
                                               SBP_PERIODIC_EVT     | \
                                               STATE_MACHINE_EVT)


// Set the register cause to the registration bit-mask
#define CONNECTION_EVENT_REGISTER_BIT_SET(RegisterCause) (connectionEventRegisterCauseBitMap |= RegisterCause )
// Remove the register cause from the registration bit-mask
#define CONNECTION_EVENT_REGISTER_BIT_REMOVE(RegisterCause) (connectionEventRegisterCauseBitMap &= (~RegisterCause) )
// Gets whether the current App is registered to the receive connection events
#define CONNECTION_EVENT_IS_REGISTERED (connectionEventRegisterCauseBitMap > 0)
// Gets whether the RegisterCause was registered to receive connection event
#define CONNECTION_EVENT_REGISTRATION_CAUSE(RegisterCause) (connectionEventRegisterCauseBitMap & RegisterCause )



/*********************************************************************
 * TYPEDEFS
 */

// App event passed from profiles.
typedef struct
{
  appEvtHdr_t hdr;  // event header.
  uint8_t *pData;  // event data
} sbpEvt_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Display Interface
Display_Handle dispHandle = NULL;

I2C_Handle i2c;
I2C_Params i2cParams;


//State Machine Variables
#define READS_PER_MSEC 0.125//(1/STATE_MACHINE_EVT_PERIOD)
#define FEATHER 0
#define SQUARE 1
uint8_t feather_or_square_blades;
uint8_t ready_for_new_catch;
int time_since_last_catch;
uint8_t feathering_state;
int time_since_feathering_occurred;

uint8_t catch_occurred;
int msecs_per_stroke;
int reads_per_stroke_count;
float previous_gyro_z[4];
float previous_accel_y[3];
int sharp_dip_gyro_z;

float accel_x;
float accel_y;
float accel_z;
float gyro_x;
float gyro_y;
float gyro_z;

/* Pin driver handles */
static PIN_Handle ledPinHandle;

/* Global memory storage for a PIN_Config table */
static PIN_State ledPinState;

/*
 * Initial LED pin configuration table
 *   - LEDs Board_LED0 & Board_LED1 are off.
 */
PIN_Config ledPinTable[] = {
  //CC2640R2_LAUNCHXL_PIN_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  //CC2640R2_LAUNCHXL_PIN_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  ROW_CUSTOM_LED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  PIN_TERMINATE
};

/*********************************************************************
 * LOCAL VARIABLES
 */


// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Event globally used to post local events and pend on system and
// local events.
static ICall_SyncHandle syncEvent;

// Clock instances for internal periodic events.
static Clock_Struct periodicClock;
static Clock_Struct stateMachineClock;

// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

// Task configuration
Task_Struct sbpTask;
Char sbpTaskStack[SBP_TASK_STACK_SIZE];

// Scan response data (max size = 31 bytes)
static uint8_t scanRspData[] =
{
  // complete name
  0x0B,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  'A',
  'o',
  'w',
  'T',
  'r',
  'a',
  'c',
  'k',
  'e',
  'r',

  // connection interval range
  0x05,   // length of this data
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
  LO_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),   // 100ms
  HI_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),
  LO_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),   // 1s
  HI_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
};

// Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertising)
static uint8_t advertData[] =
{
  // Flags: this field sets the device to use general discoverable
  // mode (advertises indefinitely) instead of general
  // discoverable mode (advertise for 30 seconds at a time)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

  // service UUID, to notify central devices what services are included
  // in this peripheral
  0x03,   // length of this data
  GAP_ADTYPE_16BIT_MORE,      // some of the UUID's, but not all
  LO_UINT16(SIMPLEPROFILE_SERV_UUID),
  HI_UINT16(SIMPLEPROFILE_SERV_UUID)
};

// GAP GATT Attributes
static uint8_t attDeviceName[GAP_DEVICE_NAME_LEN] = "Cow Tracker";


/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void RowTracker_init( void );
static void RowTracker_taskFxn(UArg a0, UArg a1);

static uint8_t RowTracker_processStackMsg(ICall_Hdr *pMsg);
static uint8_t RowTracker_processGATTMsg(gattMsgEvent_t *pMsg);
static void RowTracker_processAppMsg(sbpEvt_t *pMsg);
static void RowTracker_processStateChangeEvt(gaprole_States_t newState);
static void RowTracker_processCharValueChangeEvt(uint8_t paramID);
static void RowTracker_performPeriodicTask(void);
static void RowTracker_performStateMachineUpdate(void);
static void RowTracker_clockHandler(UArg arg);
static void RowTracker_stateClockHandler(UArg arg);

static void RowTracker_passcodeCB(uint8_t *deviceAddr, uint16_t connHandle,
                                           uint8_t uiInputs, uint8_t uiOutputs);
static void RowTracker_pairStateCB(uint16_t connHandle, uint8_t state,
                                         uint8_t status);
static void RowTracker_processPairState(uint8_t state, uint8_t status);
static void RowTracker_processPasscode(uint8_t uiOutputs);

static void RowTracker_stateChangeCB(gaprole_States_t newState);
static void RowTracker_charValueChangeCB(uint8_t paramID);
static uint8_t RowTracker_enqueueMsg(uint8_t event, uint8_t state,
                                              uint8_t *pData);

static void RowTracker_connEvtCB(Gap_ConnEventRpt_t *pReport);
static void RowTracker_processConnEvt(Gap_ConnEventRpt_t *pReport);


/*********************************************************************
 * EXTERN FUNCTIONS
 */
extern void AssertHandler(uint8 assertCause, uint8 assertSubcause);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// Peripheral GAPRole Callbacks
static gapRolesCBs_t RowTracker_gapRoleCBs =
{
  RowTracker_stateChangeCB     // GAPRole State Change Callbacks
};

// GAP Bond Manager Callbacks
// These are set to NULL since they are not needed. The application
// is set up to only perform justworks pairing.
static gapBondCBs_t RowTracker_BondMgrCBs =
{
  (pfnPasscodeCB_t) RowTracker_passcodeCB, // Passcode callback
  RowTracker_pairStateCB                   // Pairing / Bonding state Callback
};

// Simple GATT Profile Callbacks
static simpleProfileCBs_t RowTracker_simpleProfileCBs =
{
  RowTracker_charValueChangeCB // Simple GATT Characteristic value change callback
};


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * The following typedef and global handle the registration to connection event
 */
typedef enum
{
   NOT_REGISTER       = 0,
   FOR_AOA_SCAN       = 1,
   FOR_ATT_RSP        = 2,
   FOR_AOA_SEND       = 4,
   FOR_TOF_SEND       = 8
}connectionEventRegisterCause_u;

// Handle the registration and un-registration for the connection event, since only one can be registered.
uint32_t       connectionEventRegisterCauseBitMap = NOT_REGISTER; //see connectionEventRegisterCause_u

/*********************************************************************
 * @fn      RowTracker_RegistertToAllConnectionEvent()
 *
 * @brief   register to receive connection events for all the connection
 *
 * @param connectionEventRegisterCause represents the reason for registration
 *
 * @return @ref SUCCESS
 *
 */
bStatus_t RowTracker_RegistertToAllConnectionEvent (connectionEventRegisterCause_u connectionEventRegisterCause)
{
  bStatus_t status = SUCCESS;

  // in case  there is no registration for the connection event, make the registration
  if (!CONNECTION_EVENT_IS_REGISTERED)
  {
    status = GAP_RegisterConnEventCb(RowTracker_connEvtCB, GAP_CB_REGISTER, LINKDB_CONNHANDLE_ALL);
  }
  if(status == SUCCESS)
  {
    //add the reason bit to the bitamap.
    CONNECTION_EVENT_REGISTER_BIT_SET(connectionEventRegisterCause);
  }

  return(status);
}

/*********************************************************************
 * @fn      RowTracker_UnRegistertToAllConnectionEvent()
 *
 * @brief   Unregister connection events
 *
 * @param connectionEventRegisterCause represents the reason for registration
 *
 * @return @ref SUCCESS
 *
 */
bStatus_t RowTracker_UnRegistertToAllConnectionEvent (connectionEventRegisterCause_u connectionEventRegisterCause)
{
  bStatus_t status = SUCCESS;

  CONNECTION_EVENT_REGISTER_BIT_REMOVE(connectionEventRegisterCause);
  // in case  there is no more registration for the connection event than unregister
  if (!CONNECTION_EVENT_IS_REGISTERED)
  {
    GAP_RegisterConnEventCb(RowTracker_connEvtCB, GAP_CB_UNREGISTER, LINKDB_CONNHANDLE_ALL);
  }

  return(status);
}

 /*********************************************************************
 * @fn      RowTracker_createTask
 *
 * @brief   Task creation function for the Simple Peripheral.
 *
 * @param   None.
 *
 * @return  None.
 */
void RowTracker_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = sbpTaskStack;
  taskParams.stackSize = SBP_TASK_STACK_SIZE;
  taskParams.priority = SBP_TASK_PRIORITY;

  Task_construct(&sbpTask, RowTracker_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      RowTracker_init
 *
 * @brief   Called during initialization and contains application
 *          specific initialization (ie. hardware initialization/setup,
 *          table initialization, power up notification, etc), and
 *          profile initialization/setup.
 *
 * @param   None.
 *
 * @return  None.
 */
static void RowTracker_init(void)
{
  // ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntity, &syncEvent);

#ifdef USE_RCOSC
  RCOSC_enableCalibration();
#endif // USE_RCOSC

#if defined( USE_FPGA )
  // configure RF Core SMI Data Link
  IOCPortConfigureSet(IOID_12, IOC_PORT_RFC_GPO0, IOC_STD_OUTPUT);
  IOCPortConfigureSet(IOID_11, IOC_PORT_RFC_GPI0, IOC_STD_INPUT);

  // configure RF Core SMI Command Link
  IOCPortConfigureSet(IOID_10, IOC_IOCFG0_PORT_ID_RFC_SMI_CL_OUT, IOC_STD_OUTPUT);
  IOCPortConfigureSet(IOID_9, IOC_IOCFG0_PORT_ID_RFC_SMI_CL_IN, IOC_STD_INPUT);

  // configure RF Core tracer IO
  IOCPortConfigureSet(IOID_8, IOC_PORT_RFC_TRC, IOC_STD_OUTPUT);
#else // !USE_FPGA
  #if defined( DEBUG_SW_TRACE )
    // configure RF Core tracer IO
    IOCPortConfigureSet(IOID_8, IOC_PORT_RFC_TRC, IOC_STD_OUTPUT | IOC_CURRENT_4MA | IOC_SLEW_ENABLE);
  #endif // DEBUG_SW_TRACE
#endif // USE_FPGA

  // LF Clock Output
  IOCPortConfigureSet(IOID_8, IOC_PORT_AON_CLK32K, IOC_STD_OUTPUT);
  AONIOC32kHzOutputEnable();

  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueue = Util_constructQueue(&appMsg);

  // Create one-shot clocks for internal periodic events.
  Util_constructClock(&periodicClock, RowTracker_clockHandler,
                      SBP_PERIODIC_EVT_PERIOD, 0, false, SBP_PERIODIC_EVT);
  Util_constructClock(&stateMachineClock, RowTracker_stateClockHandler,
                      STATE_MACHINE_EVT_PERIOD, 0, false, STATE_MACHINE_EVT);

  //dispHandle = Display_open(SBP_DISPLAY_TYPE, NULL);

  // Open led pin structure for use
  ledPinHandle = PIN_open(&ledPinState, ledPinTable);
  //PIN_setOutputValue(ledPinHandle, ROW_CUSTOM_LED, ROW_CUSTOM_LED_ON);

  // Set GAP Parameters: After a connection was established, delay in seconds
  // before sending when GAPRole_SetParameter(GAPROLE_PARAM_UPDATE_ENABLE,...)
  // uses GAPROLE_LINK_PARAM_UPDATE_INITIATE_BOTH_PARAMS or
  // GAPROLE_LINK_PARAM_UPDATE_INITIATE_APP_PARAMS
  // For current defaults, this has no effect.
  GAP_SetParamValue(TGAP_CONN_PAUSE_PERIPHERAL, DEFAULT_CONN_PAUSE_PERIPHERAL);

  // Setup the Peripheral GAPRole Profile. For more information see the User's
  // Guide:
  // http://software-dl.ti.com/lprf/sdg-latest/html/
  {
    // Device starts advertising upon initialization of GAP
    uint8_t initialAdvertEnable = TRUE;

    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until re-enabled by the application
    uint16_t advertOffTime = 0;

    uint8_t enableUpdateRequest = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint16_t desiredMinInterval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16_t desiredMaxInterval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16_t desiredSlaveLatency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16_t desiredConnTimeout = DEFAULT_DESIRED_CONN_TIMEOUT;

    // Set the Peripheral GAPRole Parameters
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                         &initialAdvertEnable);
    GAPRole_SetParameter(GAPROLE_ADVERT_OFF_TIME, sizeof(uint16_t),
                         &advertOffTime);

    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData),
                         scanRspData);
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);

    GAPRole_SetParameter(GAPROLE_PARAM_UPDATE_ENABLE, sizeof(uint8_t),
                         &enableUpdateRequest);
    GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(uint16_t),
                         &desiredMinInterval);
    GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(uint16_t),
                         &desiredMaxInterval);
    GAPRole_SetParameter(GAPROLE_SLAVE_LATENCY, sizeof(uint16_t),
                         &desiredSlaveLatency);
    GAPRole_SetParameter(GAPROLE_TIMEOUT_MULTIPLIER, sizeof(uint16_t),
                         &desiredConnTimeout);
  }

  // Set the Device Name characteristic in the GAP GATT Service
  // For more information, see the section in the User's Guide:
  // http://software-dl.ti.com/lprf/sdg-latest/html
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName);

  // Set GAP Parameters to set the advertising interval
  // For more information, see the GAP section of the User's Guide:
  // http://software-dl.ti.com/lprf/sdg-latest/html
  {
    // Use the same interval for general and limited advertising.
    // Note that only general advertising will occur based on the above configuration
    uint16_t advInt = DEFAULT_ADVERTISING_INTERVAL;

    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, advInt);
  }

  // Setup the GAP Bond Manager. For more information see the section in the
  // User's Guide:
  // http://software-dl.ti.com/lprf/sdg-latest/html/
  {
    // Don't send a pairing request after connecting; the peer device must
    // initiate pairing
    uint8_t pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
    // Use authenticated pairing: require passcode.
    uint8_t mitm = TRUE;
    // This device only has display capabilities. Therefore, it will display the
    // passcode during pairing. However, since the default passcode is being
    // used, there is no need to display anything.
    uint8_t ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
    // Request bonding (storing long-term keys for re-encryption upon subsequent
    // connections without repairing)
    uint8_t bonding = TRUE;
    // Whether to replace the least recently used entry when bond list is full,
    // and a new device is bonded.
    // Alternative is pairing succeeds but bonding fails, unless application has
    // manually erased at least one bond.
    uint8_t replaceBonds = FALSE;
    
    GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t), &pairMode);
    GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t), &mitm);
    GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
    GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t), &bonding);
    GAPBondMgr_SetParameter(GAPBOND_LRU_BOND_REPLACEMENT, sizeof(uint8_t), &replaceBonds);
  }

  // Initialize GATT attributes
  GGS_AddService(GATT_ALL_SERVICES);           // GAP GATT Service
  GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT Service
  DevInfo_AddService();                        // Device Information Service
  SimpleProfile_AddService(GATT_ALL_SERVICES); // Simple GATT Profile


  // Setup the SimpleProfile Characteristic Values
  // For more information, see the sections in the User's Guide:
  // http://software-dl.ti.com/lprf/sdg-latest/html/
  {
    uint8_t charValue1 = 1;
    uint8_t charValue2 = 2;
    uint8_t charValue3 = 3;
    uint8_t charValue4 = 4;
    uint8_t charValue5[SIMPLEPROFILE_CHAR5_LEN] = { 1, 2, 3, 4, 5 };

    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR1, sizeof(uint8_t),
                               &charValue1);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR2, sizeof(uint8_t),
                               &charValue2);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR3, sizeof(uint8_t),
                               &charValue3);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR4, sizeof(uint8_t),
                               &charValue4);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR5, SIMPLEPROFILE_CHAR5_LEN,
                               charValue5);
  }
  // Register callback with SimpleGATTprofile
  SimpleProfile_RegisterAppCBs(&RowTracker_simpleProfileCBs);

  // Start the Device
  VOID GAPRole_StartDevice(&RowTracker_gapRoleCBs);

  // Start Bond Manager and register callback
  VOID GAPBondMgr_Register(&RowTracker_BondMgrCBs);

  // Register with GAP for HCI/Host messages. This is needed to receive HCI
  // events. For more information, see the section in the User's Guide:
  // http://software-dl.ti.com/lprf/sdg-latest/html
  GAP_RegisterForMsgs(selfEntity);

  // Register for GATT local events and ATT Responses pending for transmission
  GATT_RegisterForMsgs(selfEntity);

  //Set default values for Data Length Extension
  {
    //Set initial values to maximum, RX is set to max. by default(251 octets, 2120us)
    #define APP_SUGGESTED_PDU_SIZE 251 //default is 27 octets(TX)
    #define APP_SUGGESTED_TX_TIME 2120 //default is 328us(TX)

    //This API is documented in hci.h
    //See the LE Data Length Extension section in the BLE-Stack User's Guide for information on using this command:
    //http://software-dl.ti.com/lprf/sdg-latest/html/cc2640/index.html
    //HCI_LE_WriteSuggestedDefaultDataLenCmd(APP_SUGGESTED_PDU_SIZE, APP_SUGGESTED_TX_TIME);
  }

#if !defined (USE_LL_CONN_PARAM_UPDATE)
  // Get the currently set local supported LE features
  // The HCI will generate an HCI event that will get received in the main
  // loop
  HCI_LE_ReadLocalSupportedFeaturesCmd();
#endif // !defined (USE_LL_CONN_PARAM_UPDATE)

  //Display_print0(dispHandle, 0, 0, "Row Tracker");




  // initialize i2c
    I2C_init();
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(ROW_CUSTOM_I2C0, &i2cParams);
    if (i2c == NULL)
    {
        //Display_print0(dispHandle, 1, 0, "Error Initializing I2C");


    }
    else
    {
        //Display_print0(dispHandle, 1, 0, "I2C Initialized");
    }


    // Initialize IMU

    if (IMU_begin(i2c))
    {
        //Display_print0(dispHandle, 2, 0, "IMU Initialized");
    }
    else
    {
        //Display_print0(dispHandle, 2, 0, "Error Initializing IMU");

    }
    //PIN_setOutputValue(ledPinHandle, CC2640R2_LAUNCHXL_PIN_RLED, 0);
    // Initialize state machine variables
    feather_or_square_blades = 0;
    ready_for_new_catch = 0;
    time_since_last_catch = 0;
    feathering_state = 0;
    time_since_feathering_occurred = 0;

    catch_occurred = 0;
    msecs_per_stroke = 3000;
    reads_per_stroke_count = 0;
    previous_gyro_z[0] = 0;
    previous_gyro_z[1] = 0;
    previous_gyro_z[2] = 0;
    previous_gyro_z[3] = 0;
    previous_accel_y[0] = 0;
    previous_accel_y[1] = 0;
    previous_accel_y[2] = 0;

    sharp_dip_gyro_z = 0;

    accel_x = 0;
    accel_y = 0;
    accel_z = 0;
    gyro_x = 0;
    gyro_y = 0;
    gyro_z = 0;


    //PIN_setOutputValue(ledPinHandle, CC2640R2_LAUNCHXL_PIN_RLED, 0);
    PIN_setOutputValue(ledPinHandle, ROW_CUSTOM_LED, ROW_CUSTOM_LED_ON);
}

/*********************************************************************
 * @fn      RowTracker_taskFxn
 *
 * @brief   Application task entry point for the Simple Peripheral.
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
static void RowTracker_taskFxn(UArg a0, UArg a1)
{
  // Initialize application
  RowTracker_init();

  // Application main loop
  for (;;)
  {
    uint32_t events;

    // Waits for an event to be posted associated with the calling thread.
    // Note that an event associated with a thread is posted when a
    // message is queued to the message receive queue of the thread
    events = Event_pend(syncEvent, Event_Id_NONE, SBP_ALL_EVENTS,
                        ICALL_TIMEOUT_FOREVER);

    /*uint8_t charValueCurrent;
    SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR1, &charValueCurrent);
    uint8_t charValueNew = charValueCurrent + 1;

    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR1, sizeof(uint8_t), &charValueNew);
    */
    if (events)
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      // Fetch any available messages that might have been sent from the stack
      if (ICall_fetchServiceMsg(&src, &dest,
                                (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
      {
        uint8 safeToDealloc = TRUE;

        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
        {
          ICall_Stack_Event *pEvt = (ICall_Stack_Event *)pMsg;

          if (pEvt->signature != 0xffff)
          {
            // Process inter-task message
            safeToDealloc = RowTracker_processStackMsg((ICall_Hdr *)pMsg);
          }
        }

        if (pMsg && safeToDealloc)
        {
          ICall_freeMsg(pMsg);
        }
      }

      // If RTOS queue is not empty, process app message.
      if (events & SBP_QUEUE_EVT)
      {
        while (!Queue_empty(appMsgQueue))
        {
          sbpEvt_t *pMsg = (sbpEvt_t *)Util_dequeueMsg(appMsgQueue);
          if (pMsg)
          {
            // Process message.
            RowTracker_processAppMsg(pMsg);

            // Free the space from the message.
            ICall_free(pMsg);
          }
        }
      }

      if (events & SBP_PERIODIC_EVT)
      {
        Util_startClock(&periodicClock);

        // Perform periodic application task
        RowTracker_performPeriodicTask();
      }

      if (events & STATE_MACHINE_EVT)
      {
        Util_startClock(&stateMachineClock);

        // Perform periodic state machine update
        RowTracker_performStateMachineUpdate();
      }
    }
  }
}

/*********************************************************************
 * @fn      RowTracker_processStackMsg
 *
 * @brief   Process an incoming stack message.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t RowTracker_processStackMsg(ICall_Hdr *pMsg)
{
  uint8_t safeToDealloc = TRUE;

  switch (pMsg->event)
  {
    case GATT_MSG_EVENT:
      // Process GATT message
      safeToDealloc = RowTracker_processGATTMsg((gattMsgEvent_t *)pMsg);
      break;

    case HCI_GAP_EVENT_EVENT:
      {

        // Process HCI message
        switch(pMsg->status)
        {
          case HCI_COMMAND_COMPLETE_EVENT_CODE:
            // Process HCI Command Complete Event
            {

#if !defined (USE_LL_CONN_PARAM_UPDATE)
              // This code will disable the use of the LL_CONNECTION_PARAM_REQ
              // control procedure (for connection parameter updates, the
              // L2CAP Connection Parameter Update procedure will be used
              // instead). To re-enable the LL_CONNECTION_PARAM_REQ control
              // procedures, define the symbol USE_LL_CONN_PARAM_UPDATE
              // The L2CAP Connection Parameter Update procedure is used to
              // support a delta between the minimum and maximum connection
              // intervals required by some iOS devices.

              // Parse Command Complete Event for opcode and status
              hciEvt_CmdComplete_t* command_complete = (hciEvt_CmdComplete_t*) pMsg;
              uint8_t   pktStatus = command_complete->pReturnParam[0];

              //find which command this command complete is for
              switch (command_complete->cmdOpcode)
              {
                case HCI_LE_READ_LOCAL_SUPPORTED_FEATURES:
                  {
                    if (pktStatus == SUCCESS)
                    {
                      uint8_t featSet[8];

                      // Get current feature set from received event (bits 1-9
                      // of the returned data
                      memcpy( featSet, &command_complete->pReturnParam[1], 8 );

                      // Clear bit 1 of byte 0 of feature set to disable LL
                      // Connection Parameter Updates
                      CLR_FEATURE_FLAG( featSet[0], LL_FEATURE_CONN_PARAMS_REQ );

                      // Update controller with modified features
                      HCI_EXT_SetLocalSupportedFeaturesCmd( featSet );
                    }
                  }
                  break;

                default:
                  //do nothing
                  break;
              }
#endif // !defined (USE_LL_CONN_PARAM_UPDATE)

            }
            break;

          case HCI_BLE_HARDWARE_ERROR_EVENT_CODE:
            AssertHandler(HAL_ASSERT_CAUSE_HARDWARE_ERROR,0);
            break;

          default:
            break;
        }
      }
      break;

      default:
        // do nothing
        break;

    }

  return (safeToDealloc);
}

/*********************************************************************
 * @fn      RowTracker_processGATTMsg
 *
 * @brief   Process GATT messages and events.
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t RowTracker_processGATTMsg(gattMsgEvent_t *pMsg)
{
  // See if GATT server was unable to transmit an ATT response
  if (attRsp_isAttRsp(pMsg))
  {
    // No HCI buffer was available. Let's try to retransmit the response
    // on the next connection event.
    if( RowTracker_RegistertToAllConnectionEvent(FOR_ATT_RSP) == SUCCESS)
    {
      // Don't free the response message yet
      return (FALSE);
    }
  }
  else if (pMsg->method == ATT_FLOW_CTRL_VIOLATED_EVENT)
  {
    // ATT request-response or indication-confirmation flow control is
    // violated. All subsequent ATT requests or indications will be dropped.
    // The app is informed in case it wants to drop the connection.

    // Display the opcode of the message that caused the violation.
    //Display_print1(dispHandle, 5, 0, "FC Violated: %d", pMsg->msg.flowCtrlEvt.opcode);
  }
  else if (pMsg->method == ATT_MTU_UPDATED_EVENT)
  {
    // MTU size updated
    //Display_print1(dispHandle, 5, 0, "MTU Size: %d", pMsg->msg.mtuEvt.MTU);
  }

  // Free message payload. Needed only for ATT Protocol messages
  GATT_bm_free(&pMsg->msg, pMsg->method);

  // It's safe to free the incoming message
  return (TRUE);
}

/*********************************************************************
 * @fn      RowTracker_processConnEvt
 *
 * @brief   Process connection event.
 *
 * @param pReport pointer to connection event report
 */
static void RowTracker_processConnEvt(Gap_ConnEventRpt_t *pReport)
{

  if( CONNECTION_EVENT_REGISTRATION_CAUSE(FOR_ATT_RSP))
  {
    // The GATT server might have returned a blePending as it was trying
    // to process an ATT Response. Now that we finished with this
    // connection event, let's try sending any remaining ATT Responses
    // on the next connection event.
    // Try to retransmit pending ATT Response (if any)
    if (attRsp_sendAttRsp() == SUCCESS)
    {

        // Disable connection event end notice
        RowTracker_UnRegistertToAllConnectionEvent (FOR_ATT_RSP);
    }
  }

}

/*********************************************************************
 * @fn      RowTracker_processAppMsg
 *
 * @brief   Process an incoming callback from a profile.
 *
 * @param   pMsg - message to process
 *
 * @return  None.
 */
static void RowTracker_processAppMsg(sbpEvt_t *pMsg)
{
  switch (pMsg->hdr.event)
  {
    case SBP_STATE_CHANGE_EVT:
      {
        RowTracker_processStateChangeEvt((gaprole_States_t)pMsg->
                                                hdr.state);
      }
      break;

    case SBP_CHAR_CHANGE_EVT:
      {
        RowTracker_processCharValueChangeEvt(pMsg->hdr.state);
      }
      break;

    // Pairing event
    case SBP_PAIRING_STATE_EVT:
      {
        RowTracker_processPairState(pMsg->hdr.state, *pMsg->pData);

        ICall_free(pMsg->pData);
        break;
      }

    // Passcode event
    case SBP_PASSCODE_NEEDED_EVT:
      {
        RowTracker_processPasscode(*pMsg->pData);

        ICall_free(pMsg->pData);
        break;
      }

	case SBP_CONN_EVT:
      {
        RowTracker_processConnEvt((Gap_ConnEventRpt_t *)(pMsg->pData));

        ICall_free(pMsg->pData);
        break;
	  }

    default:
      // Do nothing.
      break;
  }
}

/*********************************************************************
 * @fn      RowTracker_stateChangeCB
 *
 * @brief   Callback from GAP Role indicating a role state change.
 *
 * @param   newState - new state
 *
 * @return  None.
 */
static void RowTracker_stateChangeCB(gaprole_States_t newState)
{
  RowTracker_enqueueMsg(SBP_STATE_CHANGE_EVT, newState, NULL);
}

/*********************************************************************
 * @fn      RowTracker_processStateChangeEvt
 *
 * @brief   Process a pending GAP Role state change event.
 *
 * @param   newState - new state
 *
 * @return  None.
 */
static void RowTracker_processStateChangeEvt(gaprole_States_t newState)
{
#ifdef PLUS_BROADCASTER
  static bool firstConnFlag = false;
#endif // PLUS_BROADCASTER

  switch ( newState )
  {
    case GAPROLE_STARTED:
      {
        uint8_t ownAddress[B_ADDR_LEN];
        uint8_t systemId[DEVINFO_SYSTEM_ID_LEN];

        GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddress);

        // use 6 bytes of device address for 8 bytes of system ID value
        systemId[0] = ownAddress[0];
        systemId[1] = ownAddress[1];
        systemId[2] = ownAddress[2];

        // set middle bytes to zero
        systemId[4] = 0x00;
        systemId[3] = 0x00;

        // shift three bytes up
        systemId[7] = ownAddress[5];
        systemId[6] = ownAddress[4];
        systemId[5] = ownAddress[3];

        DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);

        // Display device address
        //Display_print0(dispHandle, 3, 0, Util_convertBdAddr2Str(ownAddress));
        //Display_print0(dispHandle, 4, 0, "Initialized");
      }
      break;

    case GAPROLE_ADVERTISING:
      //Display_print0(dispHandle, 4, 0, "Advertising");
      break;

#ifdef PLUS_BROADCASTER
    // After a connection is dropped, a device in PLUS_BROADCASTER will continue
    // sending non-connectable advertisements and shall send this change of
    // state to the application.  These are then disabled here so that sending
    // connectable advertisements can resume.
    case GAPROLE_ADVERTISING_NONCONN:
      {
        uint8_t advertEnabled = FALSE;

        // Disable non-connectable advertising.
        GAPRole_SetParameter(GAPROLE_ADV_NONCONN_ENABLED, sizeof(uint8_t),
                           &advertEnabled);

        advertEnabled = TRUE;

        // Enabled connectable advertising.
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                             &advertEnabled);

        // Reset flag for next connection.
        firstConnFlag = false;

        attRsp_freeAttRsp(bleNotConnected);
      }
      break;
#endif //PLUS_BROADCASTER

    case GAPROLE_CONNECTED:
      {
        linkDBInfo_t linkInfo;
        uint8_t numActive = 0;

        Util_startClock(&periodicClock);
        Util_startClock(&stateMachineClock);

        // Start IMU measurements
        // IMU_enableRotationVector(i2c, 1); // send data updates every 1 ms
        IMU_enableGyro(i2c, 1);
        IMU_enableAccelerometer(i2c, 1);

        numActive = linkDB_NumActive();

        // Use numActive to determine the connection handle of the last
        // connection
        if ( linkDB_GetInfo( numActive - 1, &linkInfo ) == SUCCESS )
        {
          //Display_print1(dispHandle, 2, 0, "Num Conns: %d", (uint16_t)numActive);
          //Display_print0(dispHandle, 3, 0, Util_convertBdAddr2Str(linkInfo.addr));
        }
        else
        {
          uint8_t peerAddress[B_ADDR_LEN];

          GAPRole_GetParameter(GAPROLE_CONN_BD_ADDR, peerAddress);

          //Display_print0(dispHandle, 2, 0, "Connected");
          //Display_print0(dispHandle, 3, 0, Util_convertBdAddr2Str(peerAddress));
        }

        #ifdef PLUS_BROADCASTER
          // Only turn advertising on for this state when we first connect
          // otherwise, when we go from connected_advertising back to this state
          // we will be turning advertising back on.
          if (firstConnFlag == false)
          {
            uint8_t advertEnabled = FALSE; // Turn on Advertising

            // Disable connectable advertising.
            GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                                 &advertEnabled);

            // Set to true for non-connectable advertising.
            advertEnabled = TRUE;

            // Enable non-connectable advertising.
            GAPRole_SetParameter(GAPROLE_ADV_NONCONN_ENABLED, sizeof(uint8_t),
                                 &advertEnabled);
            firstConnFlag = true;
          }
        #endif // PLUS_BROADCASTER
      }
      break;

    case GAPROLE_CONNECTED_ADV:
      //Display_print0(dispHandle, 2, 0, "Connected Advertising");
      break;

    case GAPROLE_WAITING:
      Util_stopClock(&periodicClock);
      Util_stopClock(&stateMachineClock);

      attRsp_freeAttRsp(bleNotConnected);

      //Display_print0(dispHandle, 2, 0, "Disconnected");

      // Reset the IMU to save power
      //IMU_softReset();

      // Clear remaining lines
      Display_clearLines(dispHandle, 3, 5);
      break;

    case GAPROLE_WAITING_AFTER_TIMEOUT:
      attRsp_freeAttRsp(bleNotConnected);

      //Display_print0(dispHandle, 2, 0, "Timed Out");

      // Clear remaining lines
      //Display_clearLines(dispHandle, 3, 5);

      #ifdef PLUS_BROADCASTER
        // Reset flag for next connection.
        firstConnFlag = false;
      #endif // PLUS_BROADCASTER
      break;

    case GAPROLE_ERROR:
      //Display_print0(dispHandle, 2, 0, "Error");
      break;

    default:
      //Display_clearLine(dispHandle, 2);
      break;
  }

}

/*********************************************************************
 * @fn      RowTracker_charValueChangeCB
 *
 * @brief   Callback from Simple Profile indicating a characteristic
 *          value change.
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  None.
 */
static void RowTracker_charValueChangeCB(uint8_t paramID)
{
  RowTracker_enqueueMsg(SBP_CHAR_CHANGE_EVT, paramID, 0);
}

/*********************************************************************
 * @fn      RowTracker_processCharValueChangeEvt
 *
 * @brief   Process a pending Simple Profile characteristic value change
 *          event.
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  None.
 */
static void RowTracker_processCharValueChangeEvt(uint8_t paramID)
{
  uint8_t newValue;

  switch(paramID)
  {
    case SIMPLEPROFILE_CHAR1:
      SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR1, &newValue);

      //Display_print1(dispHandle, 4, 0, "Char 1: %d", (uint16_t)newValue);
      break;

    case SIMPLEPROFILE_CHAR3:
      SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR3, &newValue);

      //Display_print1(dispHandle, 4, 0, "Char 3: %d", (uint16_t)newValue);
      break;

    default:
      // should not reach here!
      break;
  }
}

/*********************************************************************
 * @fn      RowTracker_performPeriodicTask
 *
 * @brief   Perform a periodic application task. This function gets called
 *          every five seconds (SBP_PERIODIC_EVT_PERIOD). In this example,
 *          the value of the third characteristic in the SimpleGATTProfile
 *          service is retrieved from the profile, and then copied into the
 *          value of the the fourth characteristic.
 *
 * @param   None.
 *
 * @return  None.
 *
 */
static void RowTracker_performPeriodicTask(void)
{

    // Check IMU

    if (IMU_dataAvailable(i2c))
    {
        // update measurements
        accel_x = IMU_getAccelX();
        accel_y = IMU_getAccelY();
        accel_z = IMU_getAccelZ();
        gyro_x = IMU_getGyroX();
        gyro_y = IMU_getGyroY();
        gyro_z = IMU_getGyroZ();


        /* UPDATE BLE CHARACTERISTICS */

        uint8_t z_ble = (uint8_t)accel_z;
        SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR1, sizeof(uint8_t), &z_ble);

    }


}

/*********************************************************************
 * @fn      RowTracker_performStateMachineUpdate
 *
 * @brief   Perform a periodic application task (state machine update). This function gets called
 *          every 2.5 msec (STATE_MACHINE_EVT_PERIOD). In this example,
 *          the value of the third characteristic in the SimpleGATTProfile
 *          service is retrieved from the profile, and then copied into the
 *          value of the the fourth characteristic.
 *
 * @param   None.
 *
 * @return  None.
 *
 */
static void RowTracker_performStateMachineUpdate(void)
{

    /* STATE MACHINE */
    //PIN_setOutputValue(ledPinHandle, CC2640R2_LAUNCHXL_PIN_RLED, 0);
    // Blade State - feathered or squared blades

    if ((gyro_x < -9) && (accel_y < 10) && (feather_or_square_blades == SQUARE))
    {
        // were in squared state, change to feathered state now
        feather_or_square_blades = FEATHER;

        // turn off LED
        //PIN_setOutputValue(ledPinHandle, Board_PIN_RLED, 0);

        // since we observed a change, we know that feathering is occurring
        feathering_state = 1;

        // restart timer since feathering detected
        time_since_feathering_occurred = 0;
    }
    else if ((gyro_x > 2) && (accel_y > 6) && (gyro_z > 0.8) && (feather_or_square_blades == FEATHER))
    {
        // were in feathered state, change to squared state
        feather_or_square_blades = SQUARE;
        // turn on LED
        //PIN_setOutputValue(ledPinHandle, Board_PIN_RLED, 1);
        // feathering is occurring since we observed a change
        feathering_state = 1;
        // assume that going from feathered to square indicates preparing for a catch

    }


    // Feathering State - whether the rower is rowing with feathered or square blades
    if (feathering_state == 1)
    {
        time_since_feathering_occurred += 1;
        if (time_since_feathering_occurred > (READS_PER_MSEC * msecs_per_stroke * 1.4)) // if 1.4 strokes have passed without feathering detected
        {
            feathering_state = 0; // no longer in progress
            time_since_feathering_occurred = 0;
            feather_or_square_blades = SQUARE; // must be in square blades
        }
    }

    // Catch State

    if (ready_for_new_catch == 0)
    {
        // not enough time has passed since the last catch was detected
        time_since_last_catch += 1;
        sharp_dip_gyro_z = 0;
        if ((time_since_last_catch > (READS_PER_MSEC * msecs_per_stroke * 0.5)) || (time_since_last_catch > (READS_PER_MSEC * 3000))) // 0.6 strokes have passed
        {
            ready_for_new_catch = 1;
            //PIN_setOutputValue(ledPinHandle, CC2640R2_LAUNCHXL_PIN_RLED, 0);
            time_since_last_catch = 0; // reset time

            // turn off LED
            //PIN_setOutputValue(ledPinHandle, Board_PIN_GLED, 0);
        }
        if (time_since_last_catch > (READS_PER_MSEC * 1000))//(time_since_last_catch > (READS_PER_MSEC * msecs_per_stroke * 0.3)) || (time_since_last_catch > (READS_PER_MSEC * 1000)))
        {
            catch_occurred = 0;// leave state like this for a portion of the stroke so app can read it
            //PIN_setOutputValue(ledPinHandle, CC2640R2_LAUNCHXL_PIN_GLED, 0);
        }
    }

    else
    {

        // identify if we have a global minimum
        if ((gyro_z < -0.35) && ((gyro_z - previous_gyro_z[2]) < -0.08))//((gyro_z < -0.4) && ((gyro_z - previous_gyro_z[2]) < -0.1))
        {
            sharp_dip_gyro_z = 1;
            //PIN_setOutputValue(ledPinHandle, CC2640R2_LAUNCHXL_PIN_RLED, 1);
        }

        // we are ready for a new catch
        // need to check different factors depending on whether or not we are feathering
        if (feathering_state == 1) // feathering
        {
            // need to check for the rotation associated with the catch and make sure that the blade has been squared
            if ((previous_gyro_z[3] < -0.4) && (gyro_z > previous_gyro_z[0]) && (previous_gyro_z[0] >= previous_gyro_z[1]) && (previous_gyro_z[1] >= previous_gyro_z[2]) && (previous_gyro_z[2] >= previous_gyro_z[3]) && (sharp_dip_gyro_z == 1))//((previous_gyro_z[3] < -0.45) && (gyro_z > previous_gyro_z[0]) && (previous_gyro_z[0] >= previous_gyro_z[1]) && (previous_gyro_z[1] >= previous_gyro_z[2]) && (previous_gyro_z[2] >= previous_gyro_z[3]) && (sharp_dip_gyro_z == 1))
            {
                ready_for_new_catch = 0;

                catch_occurred = 1;
                //PIN_setOutputValue(ledPinHandle, CC2640R2_LAUNCHXL_PIN_GLED, 1);
                // timing
                msecs_per_stroke = reads_per_stroke_count/READS_PER_MSEC;
                //if (msecs_per_stroke > 3000) msecs_per_stroke = 3000;
                reads_per_stroke_count = 0;
                // turn on LED
                //PIN_setOutputValue(ledPinHandle, Board_PIN_GLED, 1);
            }
        }
        else // not feathering
        {
            if ((previous_gyro_z[3] < -0.4) && (gyro_z > previous_gyro_z[0]) && (previous_gyro_z[0] >= previous_gyro_z[1]) && (previous_gyro_z[1] >= previous_gyro_z[2]) && (previous_gyro_z[2] >= previous_gyro_z[3]) && (sharp_dip_gyro_z == 1) && (abs(previous_accel_y[1] - previous_accel_y[2] + abs(previous_accel_y[0] - previous_accel_y[1])) > 1.0))//((previous_gyro_z[3] < -0.45) && (gyro_z > previous_gyro_z[0]) && (previous_gyro_z[0] >= previous_gyro_z[1]) && (previous_gyro_z[1] >= previous_gyro_z[2]) && (previous_gyro_z[2] >= previous_gyro_z[3]) && (sharp_dip_gyro_z == 1) && (abs(previous_accel_y[1] - previous_accel_y[2] + abs(previous_accel_y[0] - previous_accel_y[1])) > 1.0))
            {
                ready_for_new_catch = 0;
                catch_occurred = 1;
                //PIN_setOutputValue(ledPinHandle, CC2640R2_LAUNCHXL_PIN_GLED, 1);
                // timing
                msecs_per_stroke = reads_per_stroke_count/READS_PER_MSEC;
                //if (msecs_per_stroke > 3000) msecs_per_stroke = 3000;
                reads_per_stroke_count = 0;
                // turn on LED
                //PIN_setOutputValue(ledPinHandle, Board_PIN_GLED, 1);
            }
        }
    }

    // update stroke timing
    reads_per_stroke_count++;

    // store previous gyro_z reading
    previous_gyro_z[3] = previous_gyro_z[2];
    previous_gyro_z[2] = previous_gyro_z[1];
    previous_gyro_z[1] = previous_gyro_z[0];
    previous_gyro_z[0] = gyro_z;

    // store previous accel_y reading
    previous_accel_y[2] = previous_accel_y[1];
    previous_accel_y[1] = previous_accel_y[0];
    previous_accel_y[0] = accel_y;

    uint8_t temp_gyro = (uint8_t)gyro_x;

    /* UPDATE BLE CHARACTERISTICS */

    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR4, sizeof(uint8_t), &catch_occurred);


    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR3, sizeof(uint8_t), &feathering_state);

    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR2, sizeof(uint8_t), &catch_occurred);

}

/*********************************************************************
 * @fn      RowTracker_pairStateCB
 *
 * @brief   Pairing state callback.
 *
 * @return  none
 */
static void RowTracker_pairStateCB(uint16_t connHandle, uint8_t state,
                                            uint8_t status)
{
  uint8_t *pData;

  // Allocate space for the event data.
  if ((pData = ICall_malloc(sizeof(uint8_t))))
  {
    *pData = status;

    // Queue the event.
    RowTracker_enqueueMsg(SBP_PAIRING_STATE_EVT, state, pData);
  }
}

/*********************************************************************
 * @fn      RowTracker_processPairState
 *
 * @brief   Process the new paring state.
 *
 * @return  none
 */
static void RowTracker_processPairState(uint8_t state, uint8_t status)
{
  if (state == GAPBOND_PAIRING_STATE_STARTED)
  {
    //Display_print0(dispHandle, 2, 0, "Pairing started");
  }
  else if (state == GAPBOND_PAIRING_STATE_COMPLETE)
  {
    if (status == SUCCESS)
    {
      //Display_print0(dispHandle, 2, 0, "Pairing success");
    }
    else
    {
      //Display_print1(dispHandle, 2, 0, "Pairing fail: %d", status);
    }
  }
  else if (state == GAPBOND_PAIRING_STATE_BONDED)
  {
    if (status == SUCCESS)
    {
      //Display_print0(dispHandle, 2, 0, "Bonding success");
    }
  }
  else if (state == GAPBOND_PAIRING_STATE_BOND_SAVED)
  {
    if (status == SUCCESS)
    {
      //Display_print0(dispHandle, 2, 0, "Bond save success");
    }
    else
    {
      //Display_print1(dispHandle, 2, 0, "Bond save failed: %d", status);
    }
  }
}

/*********************************************************************
 * @fn      RowTracker_passcodeCB
 *
 * @brief   Passcode callback.
 *
 * @return  none
 */
static void RowTracker_passcodeCB(uint8_t *deviceAddr, uint16_t connHandle,
                                           uint8_t uiInputs, uint8_t uiOutputs)
{
  uint8_t *pData;

  // Allocate space for the passcode event.
  if ((pData = ICall_malloc(sizeof(uint8_t))))
  {
    *pData = uiOutputs;

    // Enqueue the event.
    RowTracker_enqueueMsg(SBP_PASSCODE_NEEDED_EVT, 0, pData);
  }
}

/*********************************************************************
 * @fn      RowTracker_processPasscode
 *
 * @brief   Process the Passcode request.
 *
 * @return  none
 */
static void RowTracker_processPasscode(uint8_t uiOutputs)
{
  // This app uses a default passcode. A real-life scenario would handle all
  // pairing scenarios and likely generate this randomly.
  uint32_t passcode = B_APP_DEFAULT_PASSCODE;

  // Display passcode to user
  if (uiOutputs != 0)
  {
    //Display_print1(dispHandle, 4, 0, "Passcode: %d", passcode);
  }

  uint16_t connectionHandle;
  GAPRole_GetParameter(GAPROLE_CONNHANDLE, &connectionHandle);

  // Send passcode response
  GAPBondMgr_PasscodeRsp(connectionHandle, SUCCESS, passcode);
}

/*********************************************************************
 * @fn      RowTracker_clockHandler
 *
 * @brief   Handler function for clock timeouts.
 *
 * @param   arg - event type
 *
 * @return  None.
 */
static void RowTracker_clockHandler(UArg arg)
{
  // Wake up the application.
  Event_post(syncEvent, arg);
}

/*********************************************************************
 * @fn      RowTracker_stateClockHandler
 *
 * @brief   Handler function for clock timeouts.
 *
 * @param   arg - event type
 *
 * @return  None.
 */
static void RowTracker_stateClockHandler(UArg arg)
{
  // Wake up the application.
  Event_post(syncEvent, arg);
}

/*********************************************************************
 * @fn      RowTracker_connEvtCB
 *
 * @brief   Connection event callback.
 *
 * @param pReport pointer to connection event report
 */
static void RowTracker_connEvtCB(Gap_ConnEventRpt_t *pReport)
{
  // Enqueue the event for processing in the app context.
  if( RowTracker_enqueueMsg(SBP_CONN_EVT, 0 ,(uint8_t *) pReport) == FALSE)
  {
    ICall_free(pReport);
  }

}

/*********************************************************************
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 * @param   pData - message data pointer.
 *
 * @return  TRUE or FALSE
 */
static uint8_t RowTracker_enqueueMsg(uint8_t event, uint8_t state,
                                           uint8_t *pData)
{
  sbpEvt_t *pMsg = ICall_malloc(sizeof(sbpEvt_t));

  // Create dynamic pointer to message.
  if (pMsg)
  {
    pMsg->hdr.event = event;
    pMsg->hdr.state = state;
    pMsg->pData = pData;

    // Enqueue the message.
    return Util_enqueueMsg(appMsgQueue, syncEvent, (uint8_t *)pMsg);
  }

  return FALSE;
}
/*********************************************************************
*********************************************************************/
