#include "stm32f0xx.h"
#include <stdio.h>
#include <stdint.h>


//Write display from lab 3

void write_display();

write_display();

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