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

#ifndef SYLAB4PROJECT_H_
#define SYLAB4PROJECT_H_

/*Public Resource AlarmState*/
typedef enum {AON,AOFF} UISTATE_T;
extern UISTATE_T AlarmState;

#endif /* SYLAB4PROJECT_H_ */
