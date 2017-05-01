//----------------------------------------------
// Basic-PPM-Code by Gleb Devyatkin
//
// Modified and cleanup
// by Florian Voelker http://iflowproduction.de
//----------------------------------------------

int inPin = 2;		// pushbutton connected to digital pin 7
int buttonVal = 0;     	// variable to store the read value

int outPinPPM = 10; // Output-Pin for CPPM-Signal

int pitchAnalog=0;
uint16_t pitch_uS=0;
int rollAnalog=0;
uint16_t roll_uS=0;
int buttonButton=0;
uint16_t button_uS=0;

// wait-times. Shouldn't be changed
uint16_t FIXED_uS_LOW = 300;     // PPM frame fixed LOW phase
uint16_t PULSE_uS_MIN = 650;     // pulse minimum width minus start in uS
uint16_t PULSE_uS_MAX = 1700;

ISR(TIMER1_COMPA_vect) {
	ppmoutput();		// Jump to ppmoutput subroutine
}

void setup()
{
	Serial.begin(9600);
	pinMode(inPin, INPUT);
	pinMode(A0,INPUT);
	pinMode(A1,INPUT);

	pinMode(outPinPPM, OUTPUT);		// sets the digital pin as output

	// Setup timer - Don't change it
	TCCR1A = B00110001;		// Compare register B used in mode '3'
	TCCR1B = B00010010;		// WGM13 and CS11 set to 1
	TCCR1C = B00000000;		// All set to 0
	TIMSK1 = B00000010;		// Interrupt on compare B
	TIFR1  = B00000010;		// Interrupt on compare B
	OCR1A = 22000; 			// 22mS PPM output refresh
	OCR1B = 1000;
}

void loop()
{
	buttonVal = digitalRead(inPin);	// read the input pin
  
	if (buttonVal==HIGH) {
		button_uS=1600;
	} else {
		button_uS=800;
	}

	pitchAnalog = analogRead(A0);
	rollAnalog = analogRead(A1);

	pitch_uS = (map(pitchAnalog, 90, 950, 0,1023) * 1) + PULSE_uS_MIN;
	roll_uS = (map(rollAnalog, 90, 950, 0,1023) * 1) + PULSE_uS_MIN;

	//catch max and min values 
  pitch_uS = constrain(pitch_uS, PULSE_uS_MIN, PULSE_uS_MAX);
  roll_uS = constrain(roll_uS, PULSE_uS_MIN, PULSE_uS_MAX);
  button_uS = constrain(button_uS, PULSE_uS_MIN, PULSE_uS_MAX);
}

void ppmoutput() { // PPM output sub
	// Channel 1 - Aeleron
	digitalWrite(outPinPPM, LOW);
	delayMicroseconds(FIXED_uS_LOW);	// Hold
	digitalWrite(outPinPPM, HIGH);
	delayMicroseconds(roll_uS);		// Hold for Aeleron_uS microseconds      

	// Channel 2 - Elevator 
	digitalWrite(outPinPPM, LOW);
	delayMicroseconds(FIXED_uS_LOW);	// Hold
	digitalWrite(outPinPPM, HIGH);
	delayMicroseconds(roll_uS);		// Hold for Elevator_uS microseconds      

	// Channel 3 - Throttle
	digitalWrite(outPinPPM, LOW);
	delayMicroseconds(FIXED_uS_LOW);	// Hold
	digitalWrite(outPinPPM, HIGH);
	delayMicroseconds(pitch_uS);	// Hold for Throttle_uS microseconds      

	// Channel 4 - Rudder
	digitalWrite(outPinPPM, LOW);
	delayMicroseconds(FIXED_uS_LOW);	// Hold
	digitalWrite(outPinPPM, HIGH);
	delayMicroseconds(pitch_uS);	// Hold for Rudder_uS microseconds

	// Channel 5
	digitalWrite(outPinPPM, LOW);
	delayMicroseconds(FIXED_uS_LOW);	// Hold
	digitalWrite(outPinPPM, HIGH);
	delayMicroseconds(pitch_uS);

	// Channel 6 - Button
	digitalWrite(outPinPPM, LOW);
	delayMicroseconds(FIXED_uS_LOW);	// Hold
	digitalWrite(outPinPPM, HIGH);
	delayMicroseconds(button_uS);

	// Channel 7
	digitalWrite(outPinPPM, LOW);
	delayMicroseconds(FIXED_uS_LOW);	// Hold
	digitalWrite(outPinPPM, HIGH);
	delayMicroseconds(roll_uS);

	// Channel 8
	digitalWrite(outPinPPM, LOW);
	delayMicroseconds(FIXED_uS_LOW);	// Hold
	digitalWrite(outPinPPM, HIGH);
	delayMicroseconds(pitch_uS);

	// Synchro pulse
	digitalWrite(outPinPPM, LOW);
	delayMicroseconds(FIXED_uS_LOW);	// Hold
	digitalWrite(outPinPPM, HIGH);	// Start Synchro pulse
}
