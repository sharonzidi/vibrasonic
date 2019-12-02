////////////////////////////////////////////////////////////////////
// Easy Customizations
/////////////////////////////////////////////////////////////////////

// Adjust the Treshold - what volume should make it light up?
#define THRESHOLD 10
// Attempt to 'zero out' noise when line in is 'quiet'.  You can change this to make some segments more sensitive.
int  oct_bias[] = { 100, 81, 54, 47, 56, 58, 60, 67  };
// Divide Threshold by 2 for top octave? 1 - yes 2 - no.  Makes highest frequency blink more.
#define TOP_OCTAVE_DIVIDE true 



/////////////////////////////////////////////////////////////////////
// Hard Customizations - know what you are doing, please.
/////////////////////////////////////////////////////////////////////
// FHT defaults - don't change without reading the Open Music Labs documentation at openmusiclabs.com
#define LOG_OUT 1 // use the log output function
#define FHT_N 256 // set to 256 point fht
#define OCTAVE 1
#define OCT_NORM 0

// Delay - defines how many cycles before the lights will update.  OML's algorithm at 256 samples (needed for our 8 octaves) takes
// 3.18 ms per cycle, so we essentially throw out 14 cycles (I used mechanical relays, you can lower this for solid state relays).
// 15 cycles = 47.7 ms update rate.  Be careful here and don't change it too quickly!  I warned you!
#define DELAY 15

// Relay pins - Don't change unless you aren't following the instructions on dqydj.net
int r1 = 3;
int r2 = 5;
int r3 = 6;
int r4 = 9;

#include <FHT.h> // include the library


void setup() {
    
  // Initialize Relay
  digitalWrite(r1, HIGH);
  pinMode     (r1, OUTPUT);
  digitalWrite(r2, HIGH);
  pinMode     (r2, OUTPUT);
  digitalWrite(r3, HIGH);
  pinMode     (r3, OUTPUT);
  digitalWrite(r4, HIGH);
  pinMode     (r4, OUTPUT);
 
  
  Serial.begin(9600); // use the serial port
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0

}

/**********************************************************************************

  Loop - includes initialization function and the full loop
  
**********************************************************************************/

void loop() {
  
  // True full loop
  int q = 0;
  
  while(1) { // reduces jitter
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into bins
    }
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_octave(); // take the output of the fht
    sei();
    if (q % DELAY == 0) {
      for (int i = 0; i < 8; ++i) {
        Serial.print(fht_oct_out[i] - oct_bias[i]);
        Serial.print(" ");
      }
      Serial.println("");
      // You can comment this line out if you just want to see debugging info on your computer
      frequencyGraph(fht_oct_out[0] - oct_bias[0], fht_oct_out[2] - oct_bias[2], fht_oct_out[4] - oct_bias[4] , fht_oct_out[6] - oct_bias[6]);
    }
    
    ++q;
  }
  
  
  
}

/**********************************************************************************

  Relay driver code - ACTIVE LOW relays.  You need to change this if you have an active high board.
  
**********************************************************************************/

void frequencyGraph(int x1, int x2, int x3, int x4) {
  
  
    if (x1 < 0) x1 = 0;
    if (x2 < 0) x2 = 0;
    if (x3 < 0) x3 = 0;
    if (x4 < 0) x4 = 0;
  
  // X1 Section of the Spectrum
  if (x1 > THRESHOLD && digitalRead(r1)) {
    digitalWrite(r1, LOW);   
  } 
  else if (x1 < THRESHOLD && !digitalRead(r1)) {  
    digitalWrite(r1, HIGH);     
  }



  // X2 Section of the Spectrum
  if (x2 > THRESHOLD && digitalRead(r2)) {
    digitalWrite(r2, LOW);
  } 
  else if (x1 < THRESHOLD && !digitalRead(r2)) {
    digitalWrite(r2, HIGH);
  }


  // X3 Section of the Spectrum
  if (x3 > THRESHOLD && digitalRead(r3)) {
    digitalWrite(r3, LOW);
  } 
  else if (x3 < THRESHOLD && !digitalRead(r3)) {
    digitalWrite(r3, HIGH);
  }

  // X4 Section of the Spectrum
  if (x4 > THRESHOLD && digitalRead(r4)) {
    digitalWrite(r4, LOW);
  } 
  else if (x4 < THRESHOLD && !digitalRead(r4)) {
    digitalWrite(r4, HIGH);
  }
  



}

