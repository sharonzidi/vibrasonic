/*
Christmas Light Controller & Real Time Frequency Analyzer
based on FHT code by Open Music Labs at openmusiclabs.com
 */
 


/////////////////////////////////////////////////////////////////////
// Easy Customizations
/////////////////////////////////////////////////////////////////////

// Adjust the Treshold - what volume should make it light up?
#define THRESHOLD 50
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
int r2 = 4;
int r3 = 5;
int r4 = 6;
int r5 = 7;
int r6 = 8;
int r7 = 9;
int r8 = 10;


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
  digitalWrite(r5, HIGH);
  pinMode     (r5, OUTPUT);
  digitalWrite(r6, HIGH);
  pinMode     (r6, OUTPUT);
  digitalWrite(r7, HIGH);
  pinMode     (r7, OUTPUT);
  digitalWrite(r8, HIGH);
  pinMode     (r8, OUTPUT);
 
  
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
      frequencyGraph(fht_oct_out[0] - oct_bias[0], fht_oct_out[1] - oct_bias[1], fht_oct_out[2] - oct_bias[2] - 10, fht_oct_out[3] - oct_bias[3], fht_oct_out[4] - oct_bias[4], fht_oct_out[5] - oct_bias[5], fht_oct_out[6] - oct_bias[6], fht_oct_out[7] - oct_bias[7]);
    }
    
    ++q;
  }
  
  
  
}

/**********************************************************************************

  Relay driver code - ACTIVE LOW relays.  You need to change this if you have an active high board.
  
**********************************************************************************/

void frequencyGraph(int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8) {
  
  
    if (x1 < 0) x1 = 0;
    if (x2 < 0) x2 = 0;
    if (x3 < 0) x3 = 0;
    if (x4 < 0) x4 = 0;
    if (x5 < 0) x5 = 0;
    if (x6 < 0) x6 = 0;
    if (x7 < 0) x7 = 0;
    if (x8 < 0) x8 = 0;
  
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



  // X5 Section of the Spectrum
  if (x5 > THRESHOLD && digitalRead(r5)) {  
    digitalWrite(r5, LOW);
  } 
  else if (x5 < THRESHOLD && !digitalRead(r5) ) {
    digitalWrite(r5, HIGH);
  }

  if (x6 > THRESHOLD && digitalRead(r6)) {   
    digitalWrite(r6, LOW);
  } 
  else if (x6 < THRESHOLD && !digitalRead(r6) ) {
    digitalWrite(r6, HIGH);
  }

  if (x7 > THRESHOLD && digitalRead(r7)) {
    digitalWrite(r7, LOW);
  } 
  else if (x7 < THRESHOLD && !digitalRead(r7)){
    digitalWrite(r7, HIGH);
  }

int top_threshold = THRESHOLD;
if (TOP_OCTAVE_DIVIDE) top_threshold /= 2;
  if (x8 > top_threshold && digitalRead(r8)) {
    digitalWrite(r8, LOW);
  } 
  else if (x8 < top_threshold && !digitalRead(r8) ) {
    digitalWrite(r8, HIGH);
  }
  



}

