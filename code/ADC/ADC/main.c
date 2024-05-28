


#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h> 

// Buffer to store received characters
#define RX_BUFFER_SIZE 16
volatile char rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t rx_index = 0;
volatile uint8_t rx_data_ready = 0;


void set_up_environment(){
	//Enabling port B pin 0 as output (for indicating the sleep mode/wakeup mode)
	DDRB  |= (1<<0); //setting port b pin 0 as output
	PORTB |= (1<<0); //putting port B as high initially
	
	//enabling port D pin 2 as input (for external interrupt to enable sleep/wake up mode)
	DDRD &= ~(1<<2);  //setting port d pin 2 as input
	PORTD |= (1<<2); //enabling internal pull up resistor
	
	//enabling port B pin 3 (timer 1) as output (will drive the sounder)
	DDRB |= (1<<1);
	PORTB |= (1<<1); //initial value 1
	
	//Initialization of ADC
	DDRC &= ~(1<<0); //setting port c pin 0 as input 
	ADCSRA |= (1<<7); //Enabling ADC
	ADCSRA |= (1<<2) | (1<<1) | (1<<0); //setting adc prescaler to 128
	ADMUX =  0x60; //seting reference voltage to avc and selecting channel 0 and left justified mode
	
	
	//uart initialization	
	UCSR0B |= (1<<TXEN0) |(1<<RXEN0) ;//enabling transmission and receive
	UCSR0C |= (3<<UCSZ00);//setting 8 bit character size, no parity bit and 1 stop bit
	UBRR0 |= 103; //selecting baud rate for 9600
	
}