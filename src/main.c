// ***********************  main.c  ***********************
// Brief desicription of program:
// - Initializes an ESP8266 module to act as a WiFi client
//   and fetch weather data from openweathermap.org
//
// *********************************************************
/* to make this work you must
 Step 1) Set parameters of your AP in lines 40-41 of esp8266.c
 Step 2) Change line 39 with directions in lines 40-42
 Step 3) Run a terminal emulator like Putty
         115200 bits/sec, 8 bit, 1 stop, no flow control
 Step 4) Set line 50 to match baud rate of your ESP8266 (9600 or 115200)
 Step 5) Some ESP8266 respond with "ok", others with "ready"
         esp8266.c ESP8266_Reset function, try different strings like "ready" and "ok"
 Step 6) Some ESP8266 respond with "ok", others with "no change"
         esp8266.c ESP8266_SetWifiMode function, try different strings like "no change" and "ok"
 Example
 AT+GMR version 0018000902 uses "ready" and "no change"
 AT+GMR version:0.60.0.0 uses "ready" and "ok"
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "pll.h"
#include "UART.h"
#include "esp8266.h"
#include "LED.h"

// prototypes for functions defined in startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

char Fetch[] = "GET /data/2.5/weather?q=Knoxville,us&APPID=eef65fc89dee5a931d8b97d87b7841a9 HTTP/1.1\r\nHost:api.openweathermap.org\r\n\r\n";
// 1) go to http://openweathermap.org/appid#use
// 2) Register on the Sign up page
// 3) get an API key (APPID) replace the 1234567890abcdef1234567890abcdef with your APPID

int main(void){
  DisableInterrupts();
  PLL_Init(Bus80MHz);
  LED_Init();
  Output_Init();       // UART0 only used for debugging
  printf("\n\r-----------\n\rSystem starting...\n\r");
  ESP8266_Init(115200);      // connect to access point, set up as client
  ESP8266_GetVersionNumber();
  while(1){
    ESP8266_GetStatus();
    if(ESP8266_MakeTCPConnection("api.openweathermap.org")){ // open socket in server
      LED_GreenOn();
      ESP8266_SendTCP(Fetch);
    }
    ESP8266_CloseTCPConnection();
    while(Board_Input()==0){// wait for touch
    };
    LED_GreenOff();
    LED_RedToggle();
  }
}

// transparent mode for testing -  Do not change this code
void ESP8266SendCommand(char *);
int main2(void){  char data;
  DisableInterrupts();
  PLL_Init(Bus80MHz);
  LED_Init();
  Output_Init();       // UART0 as a terminal
  printf("\n\r-----------\n\rSystem starting at 9600 baud...\n\r");
//  ESP8266_Init(38400);
  ESP8266_InitUART(9600,true);
  ESP8266_EnableRXInterrupt();
  EnableInterrupts();
  ESP8266SendCommand("AT+RST\r\n");
  data = UART_InChar();
//  ESP8266SendCommand("AT+UART=115200,8,1,0,3\r\n");
//  data = UART_InChar();
//  ESP8266_InitUART(115200,true);
//  data = UART_InChar();

  while(1){
// echo data back and forth
    data = UART_InCharNonBlock();
    if(data){
      ESP8266_PrintChar(data);
    }
  }
}



