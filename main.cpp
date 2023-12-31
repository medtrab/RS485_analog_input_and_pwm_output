
#include "platform/mbed_thread.h"
#include "mbed.h"
#include <RS485.h>

RS485 RS485(PC_10,PC_11,PB_3); // Tx, Rx , !RE and DE MAX485 pin connector

DigitalOut ho(PB_3); // this pin should be connected to !RE and DE, ready to send data and cleat to send
typedef uint8_t byte;

byte regvalue[9];
byte data[9] = {0x01,0x04,0x00,0x48,0x00,0x02,0xf1,0xdd};// data to send
//               **  **   **   **   **   Vin  Vin   PWM

// Initialize a pins to perform analog input and PWM output functions
AnalogIn   AnalogInput(PA_0);//A1 should be changed ny pin name written on your Board 
PwmOut PwmOutput(PA_2);

//variables to store voltage from Analog input
float Vin =0;
unsigned short HexValue_AnlogIn;
int voltage_to_print_first = 0;
int voltage_to_print_second = 0;
int PWM_value_duty = 0;

int main(void)
{
    PwmOutput.period_ms(1);//frequency of sampling eqial 1/priod(s) f=1000hz
    while (1) {
        // mesure voltage from pin A0 
        Vin = AnalogInput.read()*3.3;
        HexValue_AnlogIn = AnalogInput.read_u16();
        PwmOutput = AnalogInput.read();
        PWM_value_duty = int(PwmOutput.read()*100);
        voltage_to_print_first = Vin; // example if Vin = 3.2 voltage_to_print_first = 3
        voltage_to_print_second = (Vin*100); // example if Vin = 3.2 voltage_to_print_second = 2
        printf("Vin = %d,%d \n",voltage_to_print_first,voltage_to_print_second);
        printf("PWM duty  = %d % \n",PWM_value_duty);
        // prapre to send hex value of 16 bit on two 8 bit
        uint8_t hex_One  = (uint8_t) (HexValue_AnlogIn & 0x00FF);
        uint8_t hex_two  = (uint8_t) (HexValue_AnlogIn & 0xFF00);
        //Prepare data now
        data[8]= PWM_value_duty;
        data[7]= hex_two;
        data[6]= hex_One;
        ThisThread::sleep_for(200);
        ho = 1;                  // Enable sending on MAX485
        RS485.sendMsg(data,sizeof(data));
        ThisThread::sleep_for(600);            // Must wait for all the data to be sent   
        ho = 0;                  // Enable receiving on MAX485
        printf("Getting data\n");

        // for reading data
       if(RS485.readable() >0){
           memset(regvalue,0,sizeof(regvalue));
           ThisThread::sleep_for(200);
           RS485.recvMsg(regvalue,sizeof(data),500);
           ThisThread::sleep_for(200);
           for (int count = 0; count < 9; count++) {
               printf("%X - ", regvalue[count]);
           }
       }else printf("No Data\n");
       printf("Done\n");
       ThisThread::sleep_for(200);
    }
}
