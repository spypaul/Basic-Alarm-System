/**************************************************************************
 * AlarmWave.c - this contains two functions and a ISR for the alarm wave 
 *              generation. 
 *
 *  Created on: Nov 1, 2019
 *      Author: Shao-Peng Yang
 **************************************************************************/

#include "MCUType.h"
#include "K65TWR_GPIO.h"
#include "AlarmWave.h"
#include "syLab4Project.h"
#include "SysTickDelay.h"

/*The lookup table for the data of the 64 samples of the sine wave*/
static const INT16U awLookUpDac[64] = {2047, 2248, 2447, 2642, 2831, 3012, 3185, 3346, 3495, 3630,
                                     3750, 3853, 3939, 4007, 4056, 4085, 4095, 4085, 4056, 4007,
                                     3939, 3853, 3750, 3630,3495, 3346, 3185, 3012, 2831, 2642,
                                     2447, 2248, 2047, 1846, 1647, 1452, 1263, 1082, 909, 748,
                                     599, 464, 344, 241, 155, 87, 38, 9, 0,  9, 38, 87, 155,
                                     241, 344, 464, 599, 748, 909, 1082, 1263, 1452, 1647, 1846};

void PIT0_IRQHandler(void);


/******************************************************************************
*AlarmWaveInit - this initialize the DAC and PIT counter
*Return Value: none
*Arguments: none
******************************************************************************/
void AlarmWaveInit(void){

    SIM->SCGC6 |= SIM_SCGC6_PIT(1);
    SIM->SCGC2 |= SIM_SCGC2_DAC0(1);
    PIT->MCR = PIT_MCR_MDIS(0); 
    PIT->CHANNEL[0].LDVAL = 3124;                                                           //60M/19200-1 to find the count
    PIT->CHANNEL[0].TCTRL = (PIT_TCTRL_TIE(1))|(PIT_TCTRL_TEN(1));

    DAC0->C0 |= (DAC_C0_DACEN(1)|DAC_C0_DACRFS(1)|DAC_C0_DACTRGSEL(1)|DAC_C0_DACSWTRG(1));


}

/******************************************************************************
*AlarmWaveControlTask - this task decide whether to turn on the PIT NVIC
*                       according to the alarm state so that the alarm will be 
8                       generated in the PIT ISR
*Return Value: none
*Arguments: none
******************************************************************************/
void AlarmWaveControlTask(void){
    INT32U cur_slice = 0;
    static INT32U last_slice = 0;
    cur_slice = SysTickGetSliceCount();
    if(cur_slice - last_slice >= 10){                                       //update every ten time slice, 100ms
        last_slice = cur_slice;
        DB3_TURN_ON();
        switch(AlarmState){
            case AON:
                PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1);
                NVIC_ClearPendingIRQ(PIT0_IRQn);
                NVIC_EnableIRQ(PIT0_IRQn);
                break;
            case AOFF:
                NVIC_DisableIRQ(PIT0_IRQn);
                DAC0->DAT[0].DATL = 0x00;                                   //make sure the alarm is off
                DAC0->DAT[0].DATH = 0x00;
                break;
            default:
                break;
        }
        DB3_TURN_OFF();
    }
    else{
    }
}



void PIT0_IRQHandler(void){
    static INT8U table_count =0;                                        //track the index for the table
    static INT32U counter = 0;                                          // track for one second
    static INT8U on_off = 0x01U;                                        //bit to decide turn on or turn off the wave to generate "beep" for the alarm
    DB4_TURN_ON();
    PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1);
    if(counter < 19200 && on_off ==1){                                  //write samples to the DAC for a second
        counter++;
        DAC0->DAT[0].DATL = (INT8U)(awLookUpDac[table_count]);
        DAC0->DAT[0].DATH = (INT8U)(awLookUpDac[table_count]>>8);
    }
    else if (counter >= 19200){                                         //roll over ince reach 1 second
        counter = 0;
        on_off ^= 0X01U;
    }
    else{
        counter++;                  
        DAC0->DAT[0].DATL = 0x00;                                       //turn on for half scale for the DAC
        DAC0->DAT[0].DATH = 0X08;
    }

    if(table_count < 63){
        table_count++;
    }
    else{
        table_count = 0;
    }

    DB4_TURN_OFF();
}


