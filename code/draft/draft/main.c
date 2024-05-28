#include <avr/interrupt.h>
#include <avr/sleep.h>

void set_up_environment() {
	// LED pin (PB0)
	DDRB |= (1 << PB0);        // Define Pin 0 of port B as output
	PORTB &= ~(1 << PB0);      // Set pin B0 low

	// Wake-up pin (PD2)
	DDRD &= ~(1 << PD2);       // Define pin 2 of port D as input
	PORTD |= (1 << PD2);       // Enable internal pull-up resistor

	// Configure the interrupt for falling edge
	EICRA |= (1 << ISC01);     // Trigger on falling edge
	EIMSK |= (1 << INT0);      // Enable interrupt INT0
	sei();                     // Enable global interrupt
}

void go_to_sleep() {
	PORTB &= ~(1 << PB0);      // Turn off the LED
	sleep_enable();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_mode();
	sleep_disable();
}

int main(void) {
	set_up_environment();

	while (1) {
		if (!(PIND & (1 << PD3))) { // Check if the sleep pin (PD3) is low
			go_to_sleep();
			} else {
			PORTB |= (1 << PB0);  // Turn on the LED
		}
	}

	return 0; // This line is not necessary but added for clarity
}
