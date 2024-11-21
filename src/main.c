/**
  ******************************************************************************
  * @file    main.c
  * @author  Weili An, Niraj Menon
  * @date    Feb 3, 2024
  * @brief   ECE 362 Lab 6 Student template
  ******************************************************************************
*/

/*******************************************************************************/

// Fill out your username, otherwise your completion code will have the 
// wrong username!
const char* username = "avanamal";

/*******************************************************************************/ 

#include "stm32f0xx.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>


void nano_wait(unsigned int);
void internal_clock();

void enable_ports();
void set_color(int, int, int, int, int, int);
void full_clock(int);
void set_row(int);
void pulse_clock();
void pulse_latch();
void write_display();
void update_score();
void setup_tim14();

//SSD Functions
void show_char(int n, char c);
void setup_tim7();
void write_display();
void drive_column(int c);
void scr_2_dsp(void);

//Global Structures for SSD
uint8_t score       = 0;
char disp[9]         = "     Pts";
uint8_t col          = 0;

int arr1[32]; 
int arr2[32]; 
int arr3[32]; 
int arr4[32]; 

//int freq[14] = {261.63, 261.63, 392.00, 392.00, 440.00, 440.00, 392.00, 349.23, 349.23, 329.23, 329.23, 293.66, 293.66, 261.63};
int freq[24] = {392.00, 440.00, 392.00, 349.23, 329.63, 349.23, 392.00, 293.66, 329.63, 349.23, 329.63, 349.23, 392.00, 392.00, 440.00, 392.00, 349.23, 329.63, 349.23, 392.00, 293.66, 392.00, 329.63, 261.63};

int freqindex = 0;

// extern uint8_t mode;
// extern char keymap;
// extern char disp[9];

// char* keymap_arr = &keymap;
// extern uint8_t font[];

//===========================================================================
// Configure GPIO Pins
//===========================================================================

void enable_ports(void) {
    // Only enable port C for the keypad
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER &= ~0xffffff;
    for(int i = 0; i<12; i++) {
        GPIOC->MODER |= (1 << i*2);
    }
    GPIOC->OTYPER &= ~0xfff;
    GPIOC->ODR |= 1<<7;
    GPIOC->ODR &= ~(1<<6 | 1<<11);
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~(3<<0 | 3<<2 | 3<<12 | 3<<14);
    GPIOB->PUPDR |= (2<<0 | 2<<2 | 2<<12 | 2<<14);

    //SDD ports - sets all pins used to outputs
    for(int i = 8; i<=10; i++) {
        GPIOB->MODER |= (1 << i*2);
    }
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    for(int i = 5; i<=12; i++) {
        GPIOA->MODER |= (1 << i*2);
    }
}

void set_arrays() {
    for(int i = 0; i < 32; i++) {
        arr1[i] = 0;
        arr2[i] = 0;
        arr3[i] = 0;
        arr4[i] = 0;
    }
}

void set_color(int r0, int g0, int b0, int r1, int g1, int b1) {
    GPIOC->ODR &= ~(1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<8 | 1<<9);
    GPIOC->ODR |= r0<<0;
    GPIOC->ODR |= b0<<1;
    GPIOC->ODR |= r1<<2;
    GPIOC->ODR |= b1<<3;
    GPIOC->ODR |= g0<<8;
    GPIOC->ODR |= g1<<9;
}

void pulse_clock(void) {
    nano_wait(5);
    GPIOC->ODR |= 1<<6;
    nano_wait(5);
    GPIOC->ODR &= ~(1<<6);
    nano_wait(5);
}

void set_row(int num) {
    GPIOC->ODR &= ~(1<<4 | 1<<10 | 1<<5);
    int A = num % 2;
    int C = num / 4;
    int B = (num - 4*C) / 2;
    GPIOC->ODR |= (A<<4);
    GPIOC->ODR |= (B<<10);
    GPIOC->ODR |= (C<<5);
}

void full_clock(int row) {
    for(int i = 0; i<32; i++){
        int r0 = 0;
        int g0 = 0;
        int b0 = 0;
        int r1 = 0;
        int b1 = 0;
        int g1 = 0;
        int j = 0;
        while (j < 32) {
            if (row >=0 && row <=2) {
                if (arr4[j] - 5 == i) {
                    r0 = 1;
                    g0 = 1;
                    b0 = 1;
                }
                if (arr2[j] - 5 == i) {
                    r1 = 1;
                    g1 = 1;
                    b1 = 1;

                }
            }
            if (row >=4 && row <=6) {
                if (arr3[j] - 5 == i) {
                    r0 = 1;
                    g0 = 1;
                    b0 = 1;
                }
                if (arr1[j] - 5 == i) {
                    r1 = 1;
                    g1 = 1;
                    b1 = 1;
                }
            }
            j++;
        }
        set_color(r0, g0, b0, r1, g1, b1);
        pulse_clock();
    }
    pulse_latch();
}

void pulse_latch(void) {
    nano_wait(5);
    GPIOC->ODR |= 1<<11;
    nano_wait(5);
    GPIOC->ODR &= ~(1<<11);
    nano_wait(5);
}

void send_it(int num) {
    int i = 0;
    if (num == 1) {
        while(i <= 27) {
            if (arr1[i] == 0 && arr1[i + 1] == 0 && arr1[i + 2] == 0 && arr1[i + 3] == 0 && arr1[i + 4] == 0) {
                arr1[i] = 1;
                arr1[i+1] = 2;
                arr1[i+2] = 3;
                arr1[i+3] = 4;
                arr1[i+4] = 5;
                break;
            }
            i++;
        }
    } else if (num == 2) {
        while(i <= 27) {
            if (arr2[i] == 0 && arr2[i + 1] == 0 && arr2[i + 2] == 0 && arr2[i + 3] == 0 && arr2[i + 4] == 0) {
                arr2[i] = 1;
                arr2[i+1] = 2;
                arr2[i+2] = 3;
                arr2[i+3] = 4;
                arr2[i+4] = 5;
                break;
            }
            i++;
        }
    } else if (num == 3) {
        while(i <= 27) {
            if (arr3[i] == 0 && arr3[i + 1] == 0 && arr3[i + 2] == 0 && arr3[i + 3] == 0 && arr3[i + 4] == 0) {
                arr3[i] = 1;
                arr3[i+1] = 2;
                arr3[i+2] = 3;
                arr3[i+3] = 4;
                arr3[i+4] = 5;
                break;
            }
            i++;
        }
    } else if (num == 4) {
        while(i <= 27) {
            if (arr4[i] == 0 && arr4[i + 1] == 0 && arr4[i + 2] == 0 && arr4[i + 3] == 0 && arr4[i + 4] == 0) {
                arr4[i] = 1;
                arr4[i+1] = 2;
                arr4[i+2] = 3;
                arr4[i+3] = 4;
                arr4[i+4] = 5;
                break;
            }
            i++;
        }
    } else {
        num = 0;
    }
}

void move_it() {
    
    for(int i = 0; i < 32; i++){
        if (arr1[i] > 0){
            arr1[i]++;
            if (arr1[i] > 36) {
                arr1[i] = 0;
            }
        }
        if (arr2[i] > 0){
            arr2[i]++;
            if (arr2[i] > 36) {
                arr2[i] = 0;
            }
        }
        if (arr3[i] > 0){
            arr3[i]++;
            if (arr3[i] > 36) {
                arr3[i] = 0;
            }
        }
        if (arr4[i] > 0){
            arr4[i]++;
            if (arr4[i] > 36) {
            arr4[i] = 0;
        }
        }
        
    }
}
#define N 1000
#define RATE 20000
short int wavetable[N];
int step0 = 0;
int offset0 = 0;
int step1 = 0;
int offset1 = 0;
uint32_t volume = 2048;

void init_wavetable(void) {
    for(int i=0; i < N; i++) {
        wavetable[i] = 32767 * sin(2 * M_PI * i / N);
    }
}

void set_freq(int chan, float f) {
    if (chan == 0) {
        if (f == 0.0) {
            step0 = 0;
            offset0 = 0;
        } else
            step0 = ((f * N) / RATE) * (1<<16);
    }
    if (chan == 1) {
        if (f == 0.0) {
            step1 = 0;
            offset1 = 0;
        } else
            step1 = (f * N / RATE) * (1<<16);
    }
}

void setup_dac(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODER4;
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    
    // Enable the trigger for the DAC.
    DAC->CR &= ~DAC_CR_TSEL1;
    DAC->CR |= DAC_CR_TEN1;
    
    // Enable the DAC.
    DAC->CR |= DAC_CR_EN1;
}

void TIM6_DAC_IRQHandler () {
    TIM6->SR &= ~TIM_SR_UIF;
    offset0 += step0;
    offset1 += step1;
    if (offset0 >= (N << 16)){
        offset0 -= (N << 16);
    }
    if (offset1 >= (N << 16)){
        offset1 -= (N << 16);
    }

    int samp = wavetable[offset0 >> 16] + wavetable[offset1 >> 16];
    samp = samp * volume;
    samp = samp >> 17;
    samp += 2048;
    DAC->DHR12R1 = samp;
}

void init_tim6(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = (1000000 / RATE) - 1;
    TIM6->ARR = 48-1;
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->CR2 |= TIM_CR2_MMS_1; // TRGO on Update event
    TIM6->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1 << TIM6_DAC_IRQn;
}

void check_points() {
    if ((GPIOB->IDR & 1<<6)) {
        for (int i = 0; i < 32; i++) {
            if (arr1[i] == 33) {
                score += 1;
                if (i < 29) {
                    if((arr1[i+2] == 33) && (arr1[i-2] = 29)) {
                        score+=1;
                    }
                }
                for (int i = 0; i < 32; i++) {
                    if(arr1[i] > 28) {
                        arr1[i] = 0;
                    }
                }
                break;
            } 
        }
    }
    if ((GPIOB->IDR & 1<<7)) {
        for (int i = 0; i < 32; i++) {
            if (arr2[i] == 33) {
                score += 1;
                if (i < 29) {
                    if((arr2[i+2] == 33) && (arr2[i-2] = 29)) {
                        score+=1;
                    }
                }
                for (int i = 0; i < 32; i++) {
                    if(arr2[i] > 28) {
                        arr2[i] = 0;
                    }
                }
                break;
            } 
        }
    }
    if ((GPIOB->IDR & 1<<1)) {
        for (int i = 0; i < 32; i++) {
            if (arr3[i] == 33) {
                score += 1;
                if (i < 29) {
                    if((arr3[i+2] == 33) && (arr3[i-2] = 29)) {
                        score+=1;
                    }
                }
                for (int i = 0; i < 32; i++) {
                    if(arr3[i] > 28) {
                        arr3[i] = 0;
                    }
                }
                break;
            } 
        }
    }
    if ((GPIOB->IDR & 1<<0)) {
        for (int i = 0; i < 32; i++) {
            if (arr4[i] == 33) {
                score += 1;
                if (i < 29) {
                    if((arr4[i+2] == 33) && (arr4[i-2] = 29)) {
                        score+=1;
                    }
                }
                for (int i = 0; i < 32; i++) {
                    if(arr4[i] > 28) {
                        arr4[i] = 0;
                    }
                }
                break;
            } 
        }
    }
}

void check_end() {
    for(int i = 0; i < 32; i++) {
        if (arr1[i] == 31 || arr2[i] == 31 || arr3[i] == 31 || arr4[i] == 31) {
            set_freq(0, freq[freqindex]);
            freqindex++;
            //nano_wait(500);
        }
    }
}

int main(void) {
    internal_clock();
    enable_ports();
    set_arrays();
    //int music[15] = {1, 1, 3, 3, 4, 4, 3, 0, 4, 4, 3, 3, 2, 2, 1}; //twinkle twinkle Make sure to change the l > music length - 1 value in line 271
    int music[29] = {3, 4, 3, 2, 1, 2, 3, 0, 1, 2, 3, 0, 2, 3, 4, 0, 3, 4, 3, 2, 1, 2, 3, 0, 1, 0, 4, 0, 3, 2};
    int i = 0;
    int arr[6] = {0, 1, 2, 4, 5, 6};
    int j = 0;
    int k = 0;
    int l = 0;
    
    /*const char *filename = "twinkle.wav";  // Replace with your .wav file path
    int16_t *samples = NULL;
    size_t num_samples = 0;
    read_wav(filename, &samples, &num_samples);
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= 3<<(2*4);
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    DAC->CR &= ~DAC_CR_EN1;
    DAC->CR &= ~DAC_CR_BOFF1;
    DAC->CR |= DAC_CR_TEN1;
    DAC->CR |= DAC_CR_TSEL1;
    DAC->CR |= DAC_CR_EN1;
    int x = 0;
    for (;;) {
        while((DAC->SWTRIGR & DAC_SWTRIGR_SWTRIG1) == DAC_SWTRIGR_SWTRIG1);
        DAC->DHR12R1 = samples[x];
        DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
        x = (x + 1) & 0xfff;
        nano_wait(100000);
    }
    
    free(samples);
    */
    init_wavetable();
    
    // for(;;) {
    // }
    
    //for(;;);
    
    while(1) {
        setup_dac();
        //set_freq(0,440.0);
        set_freq(0,0.0);
        init_tim6();
        GPIOC->ODR |= 1<<7;
        set_arrays();
        setup_tim7();
        while(!(GPIOB->IDR & 1<<6)) {
            
        }
        l = 0;
        freqindex = 0;
        score = 0;
        while(l < 36) {
            GPIOC->ODR |= 1<<7;
            set_row(arr[i]);
            full_clock(arr[i]);
            GPIOC->ODR &= ~(1<<7);
            i++;
            i %=6;
            nano_wait(50000);
            j++;
            
            if(j > 100){
                check_points();
                nano_wait(2000);
                j = 0;
                move_it();
                k++;
                if (k>5) {
                    k = 0;   
                    check_end();                 
                    if (l < 30) {
                        send_it(music[l]);
                    }
                    l++;
                }
            }
        }
        
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//SSD CODE BELOW THIS LINE (OTHER THAN IN ENABLE PORTS)
//////////////////////////////////////////////////////////////////////////////////////////////
//Font array_____________________________________________________________________________________
const char font[] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x00, // 32: space
    0x86, // 33: exclamation
    0x22, // 34: double quote
    0x76, // 35: octothorpe
    0x00, // dollar
    0x00, // percent
    0x00, // ampersand
    0x20, // 39: single quote
    0x39, // 40: open paren
    0x0f, // 41: close paren
    0x49, // 42: asterisk
    0x00, // plus
    0x10, // 44: comma
    0x40, // 45: minus
    0x80, // 46: period
    0x00, // slash
    // digits
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67,
    // seven unknown
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    // Uppercase
    0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x6f, 0x76, 0x30, 0x1e, 0x00, 0x38, 0x00,
    0x37, 0x3f, 0x73, 0x7b, 0x31, 0x6d, 0x78, 0x3e, 0x00, 0x00, 0x00, 0x6e, 0x00,
    0x39, // 91: open square bracket
    0x00, // backslash
    0x0f, // 93: close square bracket
    0x00, // circumflex
    0x08, // 95: underscore
    0x20, // 96: backquote
    // Lowercase
    0x5f, 0x7c, 0x58, 0x5e, 0x79, 0x71, 0x6f, 0x74, 0x10, 0x0e, 0x00, 0x30, 0x00,
    0x54, 0x5c, 0x73, 0x7b, 0x50, 0x6d, 0x78, 0x1c, 0x00, 0x00, 0x00, 0x6e, 0x00
};

//Show character function - may need to change GPIO stuff?___________________________________
void show_char(int n, char c) {

  if((n>=0)&&(n<=7))
  {
    // GPIOB->ODR &= ~0x07FF; //0000 0111 1111 1111 -> 0000 1111 1110 0000 = 0x0FE0, need to do for gpiob too for pb8-10 -> 0000 0111 0000 0000 = 0x0700
    GPIOA->ODR &= ~0x0FE0;
    GPIOB->ODR &= ~0x0700;
    GPIOB->ODR |= n << 8; 
    
    GPIOA->ODR |= font[c] << 5; 
  }
  else{
    return;
  }
}

// // Drive Column - may need to change GPIO stuff?______________________________________________
// void drive_column(int c) {
// //   c = least 2 bits of c
//     c &= (0x03);
// //   first clear the bits 4-7 of GPIOC 
//     GPIOC->BSRR |= (1<<(4+16));
//     GPIOC->BSRR |= (1<<(5+16));
//     GPIOC->BSRR |= (1<<(6+16));
//     GPIOC->BSRR |= (1<<(7+16));
// //   then set the bits corresponding to the column `c`
//     GPIOC->BSRR |= (1<<(c+4));
// }

//Score to Display Converter________________________________________________________________
void scr_2_dsp(void){
    snprintf(disp, 9, "%d", score);
}

//Timer 7 Stuff______________________________________________________________________________
void TIM7_IRQHandler(){
// acknowledge the interrupt first
TIM7 -> SR &= ~TIM_SR_UIF;

// Get character from the `disp` string at position `col`
// show the character at column `col`
scr_2_dsp();

//deals with weird bug
if((score < 10) && (col == 1)){
    show_char(2, ' ');
}
else if((score >= 10) && (col == 2))
{
    show_char(3, ' ');
}
else{
    show_char(col, disp[col]);
}

// increment col by 1, remember to wrap it around if it is > 7
col++;
if(col > 7){
    col = 0;
}
// drive the new column
// drive_column(col);
}

void setup_tim7() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->ARR = 10-1;
    TIM7->PSC = 480-1; 
    TIM7->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM7_IRQn);
    TIM7->CR1 |= TIM_CR1_CEN;
}