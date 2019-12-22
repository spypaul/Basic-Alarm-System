/******************************************************************************************
* EE344 Lab 4
*   This lab create a alarm wave generator with a key pad and lcd interface. There are two 
*states for the state machine: alarm on and alarm off. The display will have check sum value
*printed at the begining, and the alarm state indication on the lcd all the time.
*Pressing 'A' will generate a alarm and 'D' to disalarm. THere is a kernel(scheduler) to 
*accomplish multi-tasking. On eof the task, ControlDisplayTask is in the module. 
*
* Shao-Peng Yang, 10/24/2019
******************************************************************************************/
#include "MCUType.h"                                                                         //Include project header file
#include "BasicIO.h"
#include "K65TWR_ClkCfg.h"
#include "Key.h"
#include "LCD.h"
#include "AlarmWave.h"
#include "syLab4Project.h"
#include "K65TWR_GPIO.h"
#include "SysTickDelay.h"

#define COL 1U
#define ROW 2U
#define SEC 1000U


//typedef enum {AON,AOFF} UISTATE_T;                                                    //create states for the state machine
UISTATE_T AlarmState = AOFF;                                                            //define and initialize the current state for the state machin for the public reource

static INT16U CalcChkSum(INT8U *startaddr,  INT8U *endaddr);
static void ControlDisplayTask(void);


static const INT8C sylab4CS[] = "CS: 0x";
static const INT8C sylab4ALOFF[] = "ALARM OFF";
static const INT8C sylab4ALON[] = "ALARM ON";
static INT8U sylab4Reset = 1;                                                           //global private variable for indicating entering from reset

void main(void){
    /*Check sum pointers and variable*/
    INT8U *high_addr = (INT8U *)0x001FFFFF;
    INT8U *low_addr = (INT8U *)0x00000000;
    INT16U check_sum = 0;
    INT16U temp = 0;


    K65TWR_BootClock();

    KeyInit();
    LcdInit();
    AlarmWaveInit();
    GpioDBugBitsInit();
    /*Checking Memory*/
    check_sum = CalcChkSum(low_addr, high_addr);
    LcdClrDisp();
    LcdMoveCursor(ROW,COL);
    LcdDispStrg(sylab4CS);
    temp = check_sum >> 8;
    LcdDispByte((INT8U)temp);
    LcdDispByte((INT8U)check_sum);
    (void)SysTickDlyInit();
    SysTickDelay(SEC);
    /*create time scheduler for the kernel*/
    while(1){
        SysTickWaitEvent(10);
        ControlDisplayTask();
        AlarmWaveControlTask();
        KeyTask();
    }
}
/***********************************************************************
*CalcChkSum() - Sums up the bytes stored in the memory from
*               the start addr to the end addr, this a private function
*Return Value: the 2bytes unsigned result of the summation
*Arguments: *startaddr is a pointer pointing to the data in the
*            start of the memory specified by users
*            *endaddr is a pointer pointing to the data in the
*            end of the memory specified by users
***********************************************************************/
static INT16U CalcChkSum(INT8U *startaddr, INT8U *endaddr){
    INT16U sum = 0;
    INT8U data =0;
    INT8U over_flow = 0;
    while(startaddr <= endaddr && over_flow == 0)
    {
        data = *startaddr;
        sum += (INT16U)data;
        /*check overflow*/
        if(startaddr == (INT8U *)0xFFFFFFFFU){
            over_flow = 1;
        }
        else{
            startaddr++;
        }
    }
    return sum;
}

/******************************************************************************
*ControlDisplayTask - this is the task that check the key every 50ms, and
*                   it changes the display message and states only when 
*                   there is a change in keys we got('A' TO 'D' or 'D' TO 'A')
*Return Value: none
*Arguments: none
******************************************************************************/
static void ControlDisplayTask(void){
    
    /*time slice variable to make the period of the task be 50ms*/
    INT32U curr_slice = 0;
    static INT32U last_slice = 0;

    /*variables to check whether there is a key change to decide whether changing states and display the message*/
    INT8C cur_key =0;
    static INT8C last_key = 0;

    DB2_TURN_ON();
    curr_slice = SysTickGetSliceCount();                                                            //get the slice # of the current time slice
    if(curr_slice - last_slice >= 5){                                                               //check for 5 slices difference

        last_slice = curr_slice;
        cur_key = KeyGet();
        if(((cur_key != last_key) && (cur_key == DC1 || cur_key == DC4)) || sylab4Reset == 1){      //checking whether there is a key chnge or the system is out of reset to decide the state
            last_key = cur_key;
            if(sylab4Reset){
                AlarmState = AOFF;
                sylab4Reset = 0;
            }
            else if(cur_key == DC1){
                AlarmState = AON;
            }
            else if(cur_key == DC4){
                AlarmState = AOFF;
            }
            else{
            }
            switch(AlarmState){
                case(AON):
                    LcdClrDisp();
                    LcdDispStrg(sylab4ALON);
                    break;
                case(AOFF):
                    LcdClrDisp();
                    LcdDispStrg(sylab4ALOFF);
                    break;
                default:
                    break;
            }
        }
        else{
        }
    }
    else{
    }
    DB2_TURN_OFF();
}

