#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "homework4.h"

int main(void)
{
    char rChar;
    char *response = "\n\n\r2534 is the best course in the curriculum!\r\n\n";

    // TODO: Declare the variables that main uses to interact with your state machine.
    bool finished = false;
    int i;

    // Stops the Watchdog timer.
    initBoard();
    // TODO: Declare a UART config struct as defined in uart.h.
    //       To begin, configure the UART for 9600 baud, 8-bit payload (LSB first), no parity, 1 stop bit.
     eUSCI_UART_ConfigV1 uartConfig =
    {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                      //ClockSource = 3MHz
        19,                                                  //Clock PreScaler = 19 (Quotient)
        8,                                                   //First Mod Reg = 8 (Remainder)
        0x55,                                                //UCBRSx = 0x55, N = 0.4378
        EUSCI_A_UART_NO_PARITY,                              //No Parity
        EUSCI_A_UART_LSB_FIRST,                              //LSB first
        EUSCI_A_UART_ONE_STOP_BIT,                           //1 Stop bit
        EUSCI_A_UART_MODE,                                   //UART Mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,       //Oversampling
        EUSCI_A_UART_8_BIT_LEN,                              //8-bit payload (Data length)
    };

    // TODO: Make sure Tx AND Rx pins of EUSCI_A0 work for UART and not as regular GPIO pins.
     GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
     GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    // TODO: Initialize EUSCI_A0
     UART_initModule(EUSCI_A0_BASE, &uartConfig);

    // TODO: Enable EUSCI_A0
     UART_enableModule(EUSCI_A0_BASE);

    while(1)
    {
        // TODO: Check the receive interrupt flag to see if a received character is available.
        //       Return 0xFF if no character is available.
        if (UART_getInterruptStatus (EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
                    rChar = UART_receiveData(EUSCI_A0_BASE);
        else
            rChar = 0xFF;

        // TODO: If an actual character was received, echo the character to the terminal AND use it to update the FSM.
        //       Check the transmit interrupt flag prior to transmitting the character.
        if (rChar != 0xFF)
          if (UART_getInterruptStatus (EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
         {
            finished = charFSM(rChar);
            if(!finished)
            {
                UART_transmitData(EUSCI_A0_BASE, rChar);
            }

        // TODO: If the FSM indicates a successful string entry, transmit the response string.
        //       Check the transmit interrupt flag prior to transmitting each character and moving on to the next one.
        //       Make sure to reset the success variable after transmission.
        if (finished) //charFSM(rChar) == false)
            if (UART_getInterruptStatus (EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
            {
                for(i=0; i < 49 ;i++)
                {
                   UART_transmitData(EUSCI_A0_BASE, response[i]);
                }
         // finished = false;
            }
         }
    }
}

void initBoard()
{
    WDT_A_hold(WDT_A_BASE);
}

// TODO: FSM for detecting character sequence.
bool charFSM(char rChar)
{
   typedef enum {nullstate, state2, state25, state253, state2534} numbers;

   static numbers currentstate = nullstate;
   bool finished = false;

   switch (currentstate){
       case nullstate:
           if (rChar == '2')
               currentstate =  state2;
           break;
       case state2:
           if (rChar == '5')
               currentstate = state25;
           else
               currentstate = nullstate;
           break;
       case state25:
           if(rChar == '3')
               currentstate = state253;
           else
               currentstate = nullstate;
           break;
       case state253:
           if(rChar == '4')
               currentstate = state2534;
               else
                   currentstate = nullstate;
           break;
       case state2534:
                finished = true;
                currentstate = nullstate;
                break;
   }
    return finished;
}
