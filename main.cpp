/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "Semaphore.h"
#include "Thread.h"
#include "UnbufferedSerial.h"
#include "cmsis_os2.h"
#include "mbed.h"

static UnbufferedSerial bt(PA_11, PA_12);
//UnbufferedSerial pc(USBTX,USBRX);
DigitalOut led(LED1);

Mutex bt_mutex;
Semaphore btrc_smp;

char hola[] = "Hola mundo\r\n";
char men[30];

// Blinking rate in milliseconds
#define BLINKING_RATE     500ms
Thread hilo_enviar (osPriorityNormal, OS_STACK_SIZE, nullptr, nullptr);
Thread hilo_recibir(osPriorityNormal1, OS_STACK_SIZE, nullptr, nullptr);

void enviar (void);
void btrc_isr(void);
void recibir(void);

int main()
{
    bt.baud(9600);
    sprintf(men, "Arranque del bluetooth\n\r");
    bt.write(men, sizeof(men));
    
    hilo_enviar.start(enviar);
    hilo_recibir.start(recibir);
    bt.attach(&btrc_isr, SerialBase::RxIrq);

    while (true) {
       
        ThisThread::sleep_for(BLINKING_RATE);
    }
}

void enviar(void)
{
    int conta=0;
    while(true)
    {
        bt_mutex.lock();
        sprintf(men, "#Conteo: %03u &\n\r", conta++);
        bt.write(men, 15);
        bt_mutex.unlock();    
        ThisThread::sleep_for(2s);
    }
}

void btrc_isr(void)
{
   //char c;
    btrc_smp.release();
    //bt.read(&c, 1);
}


void recibir(void)
{
    char c;
   
    while(true)
    {
        btrc_smp.acquire();
        bt_mutex.lock();
        bt.read(&c, 1);
        bt_mutex.unlock();
        if (c=='a')  led=1;
        else if(c=='b') led=0;    

    }


}

