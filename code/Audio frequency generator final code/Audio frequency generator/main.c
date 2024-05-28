/*
 * Audio frequency generator.c
 *
 * Created: 4/16/2024 10:08:14 PM
 * Author: Md Soyabbir Abu Hanif
 */ 

#define F_CPU 16000000UL // Define CPU frequency
#include <avr/io.h>
#include <avr/interrupt.h>

#include <string.h>
#include <stdlib.h>

#define  BUFFER_SIZE 3 //size of the  rx buffer
 unsigned char rx_buffer[BUFFER_SIZE];
volatile uint8_t rx_buffer_head = 0;

volatile uint8_t timer2_ms = 0;
volatile uint16_t current_frequency = 500 ;  //setting an initial value
uint8_t adcVal ; 

 char freq  [3];
 
 volatile int new_data_received = 0; 

void set_up_environment (){
	//Enabling port B pin 1 as output for Buzzer using timer 1
	DDRB |= (1<<1);
	PORTB |= (1<<1); //setting initial value as 1
	
	
	//enabling port D as input
	DDRD &= ~(1<<2);  //setting port d pin 2 as input	
	PORTD |= (1<<2); //enabling internal pull up resistor
	
	
	//Initializing pwm with timer 0 interrupt
	DDRD |= (1<<6);
	TCCR0A = 0x83; //setting fast pwm on non inverting mode
	TCCR0B = 0x03; //setting 64 prescaler
	TIMSK0 |= (1 << OCIE0A);	
	
	//enabling adc interrupt
	DDRC &= ~(1<<0); //setting port c pin 0 as input
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADATE); // Enable ADC, enable ADC interrupt, set ADC prescaler to 128, enable auto-triggering
	ADMUX =  0x60; //seting reference voltage to avc and selecting channel 0 and left justified mode
	ADCSRA |= (1<<6); //starting conversation 
	
	
	
	//USART initialization and receiver interrupt
	UCSR0B |= (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0) ; //enabling receiver, transmitter, receiver interrupt
	UCSR0C |= (3<<UCSZ00);//setting 8 bit character size 
	UBRR0 |= 103; //setting baud rate 9600 for 16 MHz clock frequency
}

//interrupt service routine for USART receive and storing into the ring buffer
ISR (USART_RX_vect){
	unsigned char received_data = UDR0; 
	//checking for valid input. if null character, rx bufffer will not be loaded
	if ( received_data != '\0' )
	{
		rx_buffer [rx_buffer_head] = received_data; //storing the byte into ring buffer
		rx_buffer_head = (rx_buffer_head +1) % BUFFER_SIZE;
		new_data_received = 1; 
	}
	
}

//reading frequency from rx buffer and converting to integer
uint16_t frequency_read (){
	//convert buffer data to integer
	return atoi((char*)rx_buffer);
	
}

void usart_send_data (unsigned char data ){
	while (!(UCSR0A &(1<<UDRE0))); //checking if UDRE register is 1 or not. if 1, it will
	//indicate the udre buffer is empty. so ready to be written
	//_delay_ms (100);
	UDR0 = data;
	
}


//initializing timer 2 for timer interrupt to update the frequency in every 2 seconds

void timer2_int (){
	TCCR2A |= (1<<WGM21); //configure timer 2 for ctc mode 
	OCR2A = 255; //set compare value for maximum time. It will generate a 0.016 s delay
	TIMSK2 |= (1<<OCIE2A); //enable timer compare interrupt
	TCCR2B |= (1<<CS22) | (1<<CS21) | (1<<CS20); //set prescaler to 1024
}

//timer2 interrupt service routine. I will update the frequency value in every 2 seconds
ISR (TIMER2_COMPA_vect){
	timer2_ms +=1; 
	if (timer2_ms == 124)  //here, 0.016s *124 = 2s
	{
		//if new data received from usart, frequency will be changed from usard data
		if (new_data_received)
		{
			current_frequency = frequency_read();
			new_data_received = 0; 
		}
		
		//otherwise, frequency will follow the adc value
		else {
			current_frequency = 50+ ( (3.72) *adcVal ) ; // here, max adc value is 255 and frequency limit is 
			// 50 to 1000. so, frequency increasing factor = 950/255 = 3.72.  
		}
		
		 
		//converting frequency to character so that frequency can be shown to the terminal using tx of usart
		itoa(current_frequency, freq, 10);
		for ( int i = 0; freq[i]!= '\0'; ++i )
		{
			usart_send_data( freq [i]);;
		}
		
		timer2_ms = 0;
	}
}


//adc interrupt service routine
ISR(ADC_vect) {
	adcVal = ADCH; // Read ADC value
	ADCSRA |= (1<<6); //starting conversation 
}


//pwm interrupt service routine
ISR(TIMER0_COMPA_vect) {
	OCR0A = adcVal; // Update duty cycle
}



//frequency generation for sounder using timer 1 in normal mode
void frequency_generation (uint16_t frequency){
	float on_time = 1.0 / (frequency * 2);  // time period divided by 2 of desired frequency
	uint16_t ticks = (uint16_t)((on_time * F_CPU) / 64); // Calculate number of ticks
	
	TCNT1 = 0xFFFF - ticks; // Set initial value for TCNT1
	TCCR1A = 0x00;
	TCCR1B |= (1<<CS11) | (1<<CS10); // configuring for 64 clock prescaling
	while ((TIFR1 &(1<<0)) ==0); //checking for timer 1 overflow flag to be 1
	//reseting the registers
	TCCR1A = 0x00;
	TCCR1B = 0x00; 
	TIFR1 = 0x01; 
	
	
}


int main(void)
{
	
	timer2_int();  //initializing timer 2 interrupt
    set_up_environment(); 
	sei(); //initializing global interrrupt
	
    while (1) 
    {
		
		
		PORTB ^= (1<<1);  //toggling the sounder output
		frequency_generation( current_frequency); //delay according to frequency
		
    }
}

