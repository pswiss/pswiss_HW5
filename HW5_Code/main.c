#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include"I2C2_Commands.h"   // I2C2 Functions

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

////////////////////////////////////////////////////////////////////////////////

#define pinExAddress  0b0100000
#define extendedButtonPin  7
#define extendedLEDPin 0
#define inputMask 0b10000000

void initExpander()
{
    // Initializes the Pin Expander
    i2c_master_setup();
    
    i2c_master_start();
    unsigned char sendbyte = (pinExAddress << 1)|(0b00000000);// Writing    
    i2c_master_send(sendbyte); // Send address
    i2c_master_send(0x05); // IOCON
    i2c_master_send(0b00110110);
    i2c_master_restart();
    
    
    
    
    i2c_master_send(0x00); // Configuring IO direction
    i2c_master_send(inputMask);// Send configure gp0 as output, gp7 as input    
    i2c_master_stop();
    
}

void setExpander(char pin, char level)
{
    // Send a message to the pin expander
    i2c_master_start();
    unsigned char sendbyte = (pinExAddress << 1)|(0b00000000);// Writing
    i2c_master_send(sendbyte); // Send address
    i2c_master_send(0x09); // GPIO
    i2c_master_send(0b11111111); // all on
    /*i2c_master_restart();
    
    sendbyte = (pinExAddress << 1)|(0b00000001);// Reading
    i2c_master_send(sendbyte); // Send address
    unsigned char pinState = i2c_master_recv(); // Receive current state of GPIO
    i2c_master_ack(1);
    i2c_master_restart();
    
    sendbyte = (pinExAddress << 1)|(0b00000000);// Writing
    i2c_master_send(sendbyte); // Send address
    
    unsigned char outMask = 0b00000001;
    int i;
    for(i=0; i< pin; i++) // shift the outMask according to pin number
    {
        outMask << 1; // puts a 1 at desired pin
    }
    
    if(level==0)
    {
        outMask = outMask ^ 0b11111111;// Flips the bits
        i2c_master_send(pinState&outMask); // desired pin will = 0
    }
    else
    {
        i2c_master_send(pinState|outMask); // desired pin will = 1
    }
     */
    
    
    
    i2c_master_stop();
    
}

char getExpander(char pin)
{
    // Get the level of the pin
    
    // Read current state of the expander
    i2c_master_start();
    unsigned char sendbyte = (pinExAddress << 1)|(0b00000000);// Writing
    i2c_master_send(sendbyte); // Send address
    i2c_master_send(0x09); // GPIO
    i2c_master_restart();
    sendbyte = (pinExAddress << 1)|(0b00000001);// Reading
    i2c_master_send(sendbyte); // Send address
    unsigned char pinState = i2c_master_recv(); // Receive current state of GPIO
    i2c_master_ack(1);

    i2c_master_stop();
    i2c_master_restart();
    
    unsigned char inMask = 0b00000001;
    int i;
    for(i=0; i< pin; i++) // shift the inMask according to pin number
    {
        inMask << 1; // puts a 1 at desired pin
    }
    
    if(inMask&pinState == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    ANSELBbits.ANSB2 = 0;
    __builtin_enable_interrupts();
        
    // Configure I2C
    unsigned char buttonStatus = 0;
    initExpander();
    
    
    
    while(1) 
    {
        setExpander(0, 1);
	    /*
         * // Read GP7
        buttonStatus = getExpander(extendedButtonPin);
        if(buttonStatus == 0)
        {
            setExpander(extendedLEDPin, 0);
        }
        else
        {
            setExpander(extendedLEDPin, 1);
        }
         **/
        
    }
}