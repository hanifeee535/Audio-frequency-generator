/*
 * delay_function_using_timer.c
 *
 * Created: 4/8/2024 2:23:29 PM
 * Author : Md soyabbir Abu Hanif
 */ 

#include <avr/io.h>

void set_up_environment(){
	//data direction register
	DDRD &= ~(1<<2);  //setting port d pin 2 as input
	DDRB  |= (1<<0); //setting port b pin 0 as output
	
	PORTB |= (1<<0); //putting port B as high initially
	PORTD |= (1<<2); //enabling internal pull up resistor
	
}

//delay 10 ms
void set_up_timer0 (){
	TCNT0 = 0 ; //setting initial value of tcnt0 register
	TCCR0A = 0x00; // normal waveform generation with OC0B disconnected
	TCCR0B = 0x05; // setting 1024 prescaler
	while (((TIFR0 &(1<<0))) ==0); // checking if t0v0 flag is one or not
	TCCR0B = 0; //reseting tcc0b for next operation
	TCCR0A = 0; //reseting tccr0a for next operation
	TIFR0 =0x01; //reseting tifr0 
	
}



int main(void)
{
    set_up_environment();
    while (1) 
    {
	  PORTB ^= (1<<0);
	  set_up_timer0();
    }
}

