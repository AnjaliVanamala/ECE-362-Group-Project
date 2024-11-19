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
/*#pragma pack(push, 1) // Disable padding to ensure proper alignment for the struct
typedef struct {
    char riff[4];        // "RIFF"
    uint32_t chunkSize;  // Size of the file minus 8 bytes for the "RIFF" and "chunkSize"
    char wave[4];        // "WAVE"
    char fmt[4];         // "fmt "
    uint32_t fmtSize;    // Size of the fmt chunk
    uint16_t audioFormat; // Audio format (1 = PCM)
    uint16_t numChannels; // Number of channels (1 = mono, 2 = stereo)
    uint32_t sampleRate; // Sample rate (e.g., 44100)
    uint32_t byteRate;   // Byte rate (sampleRate * numChannels * bitsPerSample / 8)
    uint16_t blockAlign; // Block align (numChannels * bitsPerSample / 8)
    uint16_t bitsPerSample; // Bits per sample (usually 16)
    char data[4];        // "data"
    uint32_t dataSize;   // Size of the data chunk
} WavHeader;
#pragma pack(pop)*/


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

//Global Structures for SSD
char disp[9]         = "Hello...";
uint8_t mode         = 'A';
uint8_t score       = 0;

int arr1[32]; 
int arr2[32]; 
int arr3[32]; 
int arr4[32]; 


extern uint8_t mode;
extern char keymap;
extern char disp[9];

char* keymap_arr = &keymap;
extern uint8_t font[];

//===========================================================================
// Configure GPIOC
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
/*int read_wav(const char *filename, int16_t **out_samples, size_t *out_num_samples) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open WAV file");
        return -1;
    }

    // Read the WAV header
    WavHeader header;
    if (fread(&header, sizeof(WavHeader), 1, file) != 1) {
        perror("Failed to read WAV header");
        fclose(file);
        return -1;
    }

    // Check for valid WAV format
    if (memcmp(header.riff, "RIFF", 4) != 0 || memcmp(header.wave, "WAVE", 4) != 0 ||
        memcmp(header.fmt, "fmt ", 4) != 0 || memcmp(header.data, "data", 4) != 0) {
        fprintf(stderr, "Invalid WAV file format\n");
        fclose(file);
        return -1;
    }

    // Allocate memory for the audio samples
    size_t num_samples = header.dataSize / (header.bitsPerSample / 8);
    *out_samples = (int16_t *)malloc(header.dataSize);
    if (*out_samples == NULL) {
        perror("Failed to allocate memory for samples");
        fclose(file);
        return -1;
    }

    // Read the audio data into the array
    if (fread(*out_samples, 1, header.dataSize, file) != header.dataSize) {
        perror("Failed to read audio data");
        free(*out_samples);
        fclose(file);
        return -1;
    }

    // Return the number of samples
    *out_num_samples = num_samples;

    fclose(file);
    return 0;
}

void init_tim6(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = (48000000 / RATE) - 1;
    TIM6->ARR = 0;
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->CR2 |= TIM_CR2_MMS_1; // TRGO on Update event
    TIM6->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1 << TIM6_DAC_IRQn;
}

void TIM6_DAC_IRQHandler () {
    TIM6->SR &= ~TIM_SR_UIF;
    //DAC->DHR12R1 = samples[count];
    //count += 1;
}*/

void check_points() {
    if ((GPIOB->IDR & 1<<6)) {
        for (int i = 0; i < 32; i++) {
            if (arr1[i] == 33) {
                score += 1;
                if (i < 29) {
                    if((arr1[i+2] == 35) && (arr1[i-2] = 31)) {
                        score+=1;
                    }
                }
                for (int i = 0; i < 32; i++) {
                    if(arr1[i] > 29) {
                        arr1[i] = 0;
                    }
                }
            } 
        }
    }
    if ((GPIOB->IDR & 1<<7)) {
        for (int i = 0; i < 32; i++) {
            if (arr2[i] == 33) {
                score += 1;
                if (i < 29) {
                    if((arr2[i+2] == 35) && (arr2[i-2] = 31)) {
                        score+=1;
                    }
                }
                for (int i = 0; i < 32; i++) {
                    if(arr2[i] > 29) {
                        arr2[i] = 0;
                    }
                }
            } 
        }
    }
    if ((GPIOB->IDR & 1<<1)) {
        for (int i = 0; i < 32; i++) {
            if (arr3[i] == 33) {
                score += 1;
                if (i < 29) {
                    if((arr3[i+2] == 35) && (arr3[i-2] = 31)) {
                        score+=1;
                    }
                }
                for (int i = 0; i < 32; i++) {
                    if(arr3[i] > 29) {
                        arr3[i] = 0;
                    }
                }
            } 
        }
    }
    if ((GPIOB->IDR & 1<<0)) {
        for (int i = 0; i < 32; i++) {
            if (arr4[i] == 33) {
                score += 1;
                if (i < 29) {
                    if((arr4[i+2] == 35) && (arr4[i-2] = 31)) {
                        score+=1;
                    }
                }
                for (int i = 0; i < 32; i++) {
                    if(arr4[i] > 29) {
                        arr4[i] = 0;
                    }
                }
            } 
        }
    }
}

int main(void) {
    internal_clock();
    enable_ports();
    set_arrays();
    int music[18] = {1, 2, 3, 4, 3, 2, 1, 2, 4, 3, 2, 1, 4, 3, 1, 2, 3, 4}; // Make sure to change the l > music length - 1 value in line 271
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
    free(samples);*/
    init_wavetable();
    setup_dac();
    set_freq(0,440.0);
    init_tim6();
    // for(;;) {
    // }

    for(;;);

    while(1) {
        GPIOC->ODR |= 1<<7;
        set_arrays();
        while(!(GPIOB->IDR & 1<<6)) {

        }
        l = 0;
        while(l < 28) {
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
                nano_wait(500);
                j = 0;
                move_it();
                k++;
                if (k>4) {
                    k = 0;
                    if (l < 18) {
                        send_it(music[l]);
                    }
                    l++;
                }
            }
        }
        
    }
}


//===========================================================================
// Write Seven Segment Display Stuff - a lot of it's what we did for lab 3
//===========================================================================
/*
void show_char(int n, char c) {

  if((n>=0)&&(n<=7))
  {
    GPIOB->ODR &= ~0x07FF;
    GPIOB->ODR |= n << 8;
    
    GPIOB->ODR |= font[c];
  }
  else{
    return;
  }
}

void write_display() {
  if(mode == 'Start'){
      snprintf(disp, 9, "Start"); //for all of the stuff we're printing, we need to come up with diff displays to fit on ssd
  }
  else if(mode == 'Run'){
      snprintf(disp, 9, "Current Score: %4d", score);
  }
  else if(mode == 'End'){
     snprintf(disp, 9, "End Score: %4d", score);
  }
}
*/
//===========================================================================
// Update Score & Timer 14 Stuff
//===========================================================================
/*
void update_score() {
    score += points; //update score by number of points per note, class before each write_display in timer 14
}

void TIM14_IRQHandler(void) {
    // Acknowledge the interrupt
    TIM14 -> SR &= ~TIM_SR_UIF;
    // call update_score
    update_score();
    // call write_display
    write_display();
}
void setup_tim14() {
// And then implement the function setup_tim14 to initialize Timer 14 to invoke an update interrupt twice per second (2 Hz). 
// This will be used to update the game state and display 2 times a second
    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
    TIM14->ARR = 1000-1;
    TIM14->PSC = 24000-1; 
    TIM14->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM14_IRQn);
    TIM14->CR1 |= TIM_CR1_CEN;
}
*/
//===========================================================================
// Mode Stuff --- Depends on what buttons we're using
//===========================================================================
/*
void mode_select(char key) {
    // if key == 'A'/'B'/'C', set mode to key
    if((key == 'A')){
     mode = 'Start';
    }
    else if((key == 'B')){
     mode = 'Run';
    }
    else if((key == 'C')){
     mode = 'End';
    }
}
*/


/*
void drive_column(int);   // energize one of the column outputs
int  read_rows();         // read the four row inputs
void update_history(int col, int rows); // record the buttons of the driven column
char get_key_event(void); // wait for a button event (press or release)
char get_keypress(void);  // wait for only a button press event.
float getfloat(void);     // read a floating-point number from keypad
void show_keys(void);     // demonstrate get_key_event()
*/
//===========================================================================
// Bit Bang SPI LED Array
//===========================================================================
/*
int msg_index = 0;
uint16_t msg[8] = { 0x0000,0x0100,0x0200,0x0300,0x0400,0x0500,0x0600,0x0700 };
extern const char font[];

//===========================================================================
// Configure PB12 (CS), PB13 (SCK), and PB15 (SDI) for outputs
//===========================================================================
void setup_bb(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~((3 << 24) | (3 << 26) | (3 << 30));
    GPIOB->MODER |= ((1 << 24) | (1 << 26) | (1 << 30));
    GPIOB->ODR |= (1 << 12);
    GPIOB->ODR &= ~(1 << 13);
}

void small_delay(void) {
    nano_wait(50000);
}

//===========================================================================
// Set the MOSI bit, then set the clock high and low.
// Pause between doing these steps with small_delay().
//===========================================================================
void bb_write_bit(int val) {
    // CS (PB12)
    // SCK (PB13)
    // SDI (PB15)
    if (val) {
        GPIOB->ODR |= (1 << 15);
    }
    else {
        GPIOB->ODR &= ~(1 << 15);
    }
    small_delay();
    GPIOB->ODR |= (1 << 13);
    small_delay();
    GPIOB->ODR &= ~(1 << 13);
}

//===========================================================================
// Set CS (PB12) low,
// write 16 bits using bb_write_bit,
// then set CS high.
//===========================================================================
void bb_write_halfword(int halfword) {
    GPIOB->ODR &= ~(1 << 12);
    int temp;
    for(int i = 15; i >= 0; i--) {
        temp = halfword;
        bb_write_bit((temp >> i) & 1);
    }
    GPIOB->ODR |= (1 << 12);
}

//===========================================================================
// Continually bitbang the msg[] array.
//===========================================================================
void drive_bb(void) {
    for(;;)
        for(int d=0; d<8; d++) {
            bb_write_halfword(msg[d]);
            nano_wait(1000000); // wait 1 ms between digits
        }
}

//============================================================================
// Configure Timer 15 for an update rate of 1 kHz.
// Trigger the DMA channel on each update.
// Copy this from lab 4 or lab 5.
//============================================================================

void init_tim15(void) {
    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
    TIM15->PSC = 480-1;
    TIM15->ARR = 100-1;
    TIM15->DIER |= TIM_DIER_UDE;
    TIM15->CR1 |= TIM_CR1_CEN;
}

//===========================================================================
// Configure timer 7 to invoke the update interrupt at 1kHz
// Copy from lab 4 or 5.
//===========================================================================

void init_tim7(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 480-1;
    TIM7->ARR = 100-1;
    TIM7->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM7_IRQn);
    TIM7->CR1 |= TIM_CR1_CEN;
}

//===========================================================================
// Copy the Timer 7 ISR from lab 5
//===========================================================================
// TODO To be copied

void TIM7_IRQHandler(void) {
    TIM7->SR &= ~TIM_SR_UIF;
    int rows = read_rows();
    update_history(col, rows);
    col = (col + 1) & 3;
    drive_column(col);
}

//===========================================================================
// Initialize the SPI2 peripheral.
//===========================================================================
void init_spi2(void) {
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~((3 << 24) | (3 << 26) | (3 << 30));
    GPIOB->MODER |= (2 << 24) | (2 << 26) | (2 << 30);
    GPIOB->AFR[1] &= ~(0b1111 << 16 | 0b1111 << 20 | 0b1111 << 28);
    SPI2->CR1 &= ~SPI_CR1_SPE;
    SPI2->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;
    SPI2->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_DS_3;
    SPI2->CR1 |= SPI_CR1_MSTR;
    SPI2->CR2 |= SPI_CR2_SSOE | SPI_CR2_NSSP;
    SPI2->CR2 |= SPI_CR2_TXDMAEN;
    SPI2->CR1 |= SPI_CR1_SPE;
}

//===========================================================================
// Configure the SPI2 peripheral to trigger the DMA channel when the
// transmitter is empty.  Use the code from setup_dma from lab 5.
//===========================================================================
void spi2_setup_dma(void) {
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel5->CCR &= ~DMA_CCR_EN;
    DMA1_Channel5->CMAR = (uint32_t)&msg;
    DMA1_Channel5->CPAR = (uint32_t)&(SPI2->DR);
    DMA1_Channel5->CNDTR = 8; 
    DMA1_Channel5->CCR |= DMA_CCR_DIR;
    DMA1_Channel5->CCR |= DMA_CCR_MINC;
    DMA1_Channel5->CCR |= DMA_CCR_MSIZE_0;
    DMA1_Channel5->CCR |= DMA_CCR_PSIZE_0;
    DMA1_Channel5->CCR |= DMA_CCR_CIRC;
    SPI2->CR2 |= SPI_CR2_TXDMAEN;
}

//===========================================================================
// Enable the DMA channel.
//===========================================================================
void spi2_enable_dma(void) {
    DMA1_Channel5->CCR |= DMA_CCR_EN;
}

//===========================================================================
// 4.4 SPI OLED Display
//===========================================================================
void init_spi1() {
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER &= ~((3 << 30) | (3 << 10) | (3 << 14));
    GPIOA->MODER |= (2 << 30) | (2 << 10) | (2 << 14);
    GPIOA->AFR[0] &= ~((0b1111 << 20) | (0b1111 << 28));
    GPIOA->AFR[1] &= ~(0b1111 << 28);
    SPI1->CR1 &= ~SPI_CR1_SPE;
    SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;
    SPI1->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_3;
    SPI1->CR2 &= ~(SPI_CR2_DS_1 | SPI_CR2_DS_2);
    SPI1->CR1 |= SPI_CR1_MSTR;
    SPI1->CR2 |= SPI_CR2_SSOE | SPI_CR2_NSSP;
    SPI1->CR2 |= SPI_CR2_TXDMAEN;
    SPI1->CR1 |= SPI_CR1_SPE;
}
void spi_cmd(unsigned int data) {
    while(!(SPI1->SR & SPI_SR_TXE)) {

    }
    SPI1->DR = data;
}
void spi_data(unsigned int data) {
    spi_cmd(data | 0x200);
}
void spi1_init_oled() {
    nano_wait(1000000);
    spi_cmd(0x38);
    spi_cmd(0x08);
    spi_cmd(0x01);
    nano_wait(2000000);
    spi_cmd(0x06);
    spi_cmd(0x02);
    spi_cmd(0x0c);
}
void spi1_display1(const char *string) {
    spi_cmd(0x10);
    int i = 0;
    char temp = 'a';
    while (string[i] != '\0') {
        spi_data(string[i]);
        temp = string[i];
        i++;
    }
}
void spi1_display2(const char *string) {
    spi_cmd(0xc0);
    int i = 0;
    while (string[i] != '\0') {
        spi_data(string[i]);
        i++;
    }
}

//===========================================================================
// This is the 34-entry buffer to be copied into SPI1.
// Each element is a 16-bit value that is either character data or a command.
// Element 0 is the command to set the cursor to the first position of line 1.
// The next 16 elements are 16 characters.
// Element 17 is the command to set the cursor to the first position of line 2.
//===========================================================================
uint16_t display[34] = {
        0x002, // Command to set the cursor at the first position line 1
        0x200+'E', 0x200+'C', 0x200+'E', 0x200+'3', 0x200+'6', + 0x200+'2', 0x200+' ', 0x200+'i',
        0x200+'s', 0x200+' ', 0x200+'t', 0x200+'h', + 0x200+'e', 0x200+' ', 0x200+' ', 0x200+' ',
        0x0c0, // Command to set the cursor at the first position line 2
        0x200+'c', 0x200+'l', 0x200+'a', 0x200+'s', 0x200+'s', + 0x200+' ', 0x200+'f', 0x200+'o',
        0x200+'r', 0x200+' ', 0x200+'y', 0x200+'o', + 0x200+'u', 0x200+'!', 0x200+' ', 0x200+' ',
};

//===========================================================================
// Configure the proper DMA channel to be triggered by SPI1_TX.
// Set the SPI1 peripheral to trigger a DMA when the transmitter is empty.
//===========================================================================
void spi1_setup_dma(void) {
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel3->CCR &= ~DMA_CCR_EN;
    DMA1_Channel3->CMAR = (uint32_t)&display;
    DMA1_Channel3->CPAR = (uint32_t)&(SPI1->DR);
    DMA1_Channel3->CNDTR = 8; 
    DMA1_Channel3->CCR |= DMA_CCR_DIR;
    DMA1_Channel3->CCR |= DMA_CCR_MINC;
    DMA1_Channel3->CCR |= DMA_CCR_MSIZE_0;
    DMA1_Channel3->CCR |= DMA_CCR_PSIZE_0;
    DMA1_Channel3->CCR |= DMA_CCR_CIRC;
    SPI1->CR2 |= SPI_CR2_TXDMAEN;
}

//===========================================================================
// Enable the DMA channel triggered by SPI1_TX.
//===========================================================================
void spi1_enable_dma(void) {
    DMA1_Channel3->CCR |= DMA_CCR_EN;
}

//===========================================================================
// Main function
//===========================================================================
*/
//int main(void) {
    //internal_clock();
    /*
    msg[0] |= font['E'];
    msg[1] |= font['C'];
    msg[2] |= font['E'];
    msg[3] |= font[' '];
    msg[4] |= font['3'];
    msg[5] |= font['6'];
    msg[6] |= font['2'];
    msg[7] |= font[' '];
    */
    // GPIO enable
    //enable_ports();
    //init_tim7();
    
    // setup keyboard
    /*
    init_tim7();

    // LED array Bit Bang
//#define BIT_BANG
#if defined(BIT_BANG)
    setup_bb();
    drive_bb();
#endif

    // Direct SPI peripheral to drive LED display
//#define SPI_LEDS
#if defined(SPI_LEDS)
    init_spi2();
    spi2_setup_dma();
    spi2_enable_dma();
    init_tim15();
    show_keys();
#endif

    // LED array SPI
//#define SPI_LEDS_DMA
#if defined(SPI_LEDS_DMA)
    init_spi2();
    spi2_setup_dma();
    spi2_enable_dma();
    show_keys();
#endif

    // SPI OLED direct drive
//#define SPI_OLED
#if defined(SPI_OLED)
    init_spi1();
    spi1_init_oled();
    spi1_display1("Hello again,");
    spi1_display2(username);
#endif

    // SPI
//#define SPI_OLED_DMA
#if defined(SPI_OLED_DMA)
    init_spi1();
    spi1_init_oled();
    spi1_setup_dma();
    spi1_enable_dma();
#endif

    // Uncomment when you are ready to generate a code.
    autotest();

    // Game on!  The goal is to score 100 points.
    //game();
    */
//}
