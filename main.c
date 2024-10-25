#include "stm32f0xx.h"
#include <stdio.h>
#include <stdint.h>

void enable_ports();
void write_display();

//Write display from lab 3
enable_ports();
write_display();



void enable_ports(){
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    //Setting the matrix pins to output
    //SORRY I KNOW I'M DOING THIS THE LONG WAY I JUST WANTED TO HAVE IT LAID OUT
    //THIS WAY FOR NOW SO WE CAN EASILY CHANGE INDIVDUAL PINS AS NEEDED, I PROMISE I'LL SIMPLIFY LATER!!!
    //B0
    GPIOB->MODER |= GPIO_MODER_MODER0_0;
    GPIOB->MODER &= ~GPIO_MODER_MODER0_1;
    //C0
    GPIOC->MODER |= GPIO_MODER_MODER0_0;
    GPIOC->MODER &= ~GPIO_MODER_MODER0_1;
    //C1
    GPIOC->MODER |= GPIO_MODER_MODER1_0;
    GPIOC->MODER &= ~GPIO_MODER_MODER1_1;
    //C2
    GPIOC->MODER |= GPIO_MODER_MODER2_0;
    GPIOC->MODER &= ~GPIO_MODER_MODER2_1;
    //C3
    GPIOC->MODER |= GPIO_MODER_MODER3_0;
    GPIOC->MODER &= ~GPIO_MODER_MODER3_1;
    //C4
    GPIOC->MODER |= GPIO_MODER_MODER4_0;
    GPIOC->MODER &= ~GPIO_MODER_MODER4_1;
    //C5
    GPIOC->MODER |= GPIO_MODER_MODER5_0;
    GPIOC->MODER &= ~GPIO_MODER_MODER5_1;
    //C6
    GPIOC->MODER |= GPIO_MODER_MODER6_0;
    GPIOC->MODER &= ~GPIO_MODER_MODER6_1;
    //C7
    GPIOC->MODER |= GPIO_MODER_MODER7_0;
    GPIOC->MODER &= ~GPIO_MODER_MODER7_1;
    //C8
    GPIOC->MODER |= GPIO_MODER_MODER8_0;
    GPIOC->MODER &= ~GPIO_MODER_MODER8_1;
    //C9
    GPIOC->MODER |= GPIO_MODER_MODER9_0;
    GPIOC->MODER &= ~GPIO_MODER_MODER9_1;
    //C10
    GPIOC->MODER |= GPIO_MODER_MODER10_0;
    GPIOC->MODER &= ~GPIO_MODER_MODER10_1;
    //C11
    GPIOC->MODER |= GPIO_MODER_MODER11_0;
    GPIOC->MODER &= ~GPIO_MODER_MODER11_1;

    //Buttons (may need to do different pins, I forget which ones we shouldn't do for A)
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    //A0
    GPIOA->MODER &= ~GPIO_MODER_MODER0;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_1;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR0_0;
    //A1
    GPIOA->MODER &= ~GPIO_MODER_MODER1;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR1_1;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR1_0;
    //A2
    GPIOA->MODER &= ~GPIO_MODER_MODER2;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR2_1;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR2_0;
    //A3
    GPIOA->MODER &= ~GPIO_MODER_MODER3;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR3_1;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR3_0;
}

void write_display() {
  if(mode == 'C'){
      snprintf(disp, 9, "Crashed");
  }
  else if(mode == 'L'){
      snprintf(disp, 9, "Landed ");
  }
  else if(mode == 'A'){
     snprintf(disp, 9, "ALt%5d", alt);
  }
  else if(mode == 'B'){
      snprintf(disp, 9, "FUEL %3d", fuel);
  }
  else if(mode == 'D'){
      snprintf(disp, 9, "Spd %4d", velo);
  }
}