/*
 * pwm_generation.c
 *
 * Created: 4/8/2024 4:46:44 PM
 * Author : dcta
 */ 

#include <avr/io.h>

void set_up_environment(){
	//data direction register
	DDRD &= ~(1<<2);  //setting port d pin 2 as input
	DDRB  |= (1<<0); //setting port b pin 0 as output
	
	PORTB |= (1<<0); //putting port B as high initially
	PORTD |= (1<<2); //enabling internal pull up resistor
	
}


//1khz pwm generation
void Pwm_generation (){
	//TCNT0 = 0x00;
	DDRD |= (1<<6); 
	OCR0A = 100; //50 percent duty cycle
	TCCR0A = 0x83; //setting fast pwm on non inverting mode
	TCCR0B = 0x03; //setting 64 prescaler
	
}



int main(void)
{
	set_up_environment();
	Pwm_generation();
	while (1)
	{
		//PORTB ^= (1<<0);
		//set_up_timer0();
		
	}
}
