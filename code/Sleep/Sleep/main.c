/*
 * sleep.c
 *
 * Created: 4/4/2024 12:21:27 AM
 * Author : md soyabbir abu hanif
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

volatile uint8_t led_flag = 0x01;

//setting the gpio pins
void set_up_environment (){
		DDRB = 0b00000001; //Defining pin 0 of port B as output
		PORTB = 0b00000001; //setting pin 0 of port B as high
		
		DDRD &= 0b11111011; //defining pin 2 of port D as input
		//PORTD = 0b00000100; //enabling internal pull up resistor
	
}

//setting the interrupt for falling edge 
void setting_interrupt (){
	EICRA |= (1<<ISC01); // Trigger into falling edge
	EIMSK |= (1<<INT0); //enabling interrupt INT0 ;
	sei(); //enabling global interrupt
}

//LED indicator
void toggle_led (){
	
	if (led_flag){
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_enable();
		sleep_cpu(); 
		PORTB |= (1<<0);
		_delay_ms(200);
	}
	else {
		sleep_disable();  
		PORTB &= ~(1<<0);
		}

}


//Interrupt handler
ISR (INT0_vect){
	led_flag ^= (1<<0); //toggling the interrupt flag
	//PORTB ^= (1<<0); //toggling the LED in every interrupt happen
	TIFR0 = 0x00; //clearing the interrupt flag
	
}



int main (void){
	
	set_up_environment();
	setting_interrupt();
	
	while(1){
		toggle_led();
	}
}