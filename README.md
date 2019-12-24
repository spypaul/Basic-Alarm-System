# Basic-Alarm-System
## Description
The project focus on creating cooperative kernel and the usage of interrupt for PIT(programmable interval timer), DAC(Digital to Analog converter), keypad, and LCD. The period of the time slice is 10ms. The UI is designed to read the input from the Key Pad: when the key 'A' is pressed, the state of the state machine will change to ALARM state, and an alarm signal will be sent to the DAC. To disalarm, the key 'D' must be pressed. The PIT handler is used to send the sine wave samples to the DAC. The timming for the PIT is programed to match the multiplication of the Frequency and the sampling frequency, so the samples can be sent with the proper timing to create the sine wave through the DAC. 
