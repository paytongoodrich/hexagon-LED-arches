//  Hexagon Arches
//  Payton Goodrich
//  August 2022

#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include <Arduino.h>
FASTLED_USING_NAMESPACE

// These are the three data pins 
// Pin 1 for the first archway one will walk through, and Pin 3 as the last.
#define DATA_PIN_1            13
#define DATA_PIN_2            12
#define DATA_PIN_3            11

#define NUM_LEDS              175       // Number of LEDs per strip.  Assumes same number for all three arches
#define LED_TYPE              WS2812B 
#define COLOR_ORDER           GRB

#define BRIGHTNESS            128       // Scale 0-255.  Higher brightness may cause greater color shift.
#define MAX_POWER_MILLIAMPS   500       // Do not change for safety in the circuit.
#define FRAMES_PER_SECOND  120          //Frame rate at which the LEDs are updated.

// Add LEDs to fastLED library
CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];

TBlendType                    currentBlending;
uint8_t                       colorIndex; // Integer representing what color from the pallete to pick
uint8_t                       cyclerIndex; // Counts the number of times this program has been run

  // Set color palletes  

  CRGB FR_Colors[] = { 
      CRGB::BlueViolet,
      CRGB::Cyan, 
      CRGB::Chartreuse, 
      CRGB::Yellow, 
      CRGB::DarkOrange};

  DEFINE_GRADIENT_PALETTE( example_gp ) {
    0,    138, 43, 226,
    80,    0, 255, 255,
    130,    127, 255, 0, 
    205,    255, 255, 0,
    238,    255, 140, 0,            // I'm making this my last color choice
    255,    255, 140, 0 };          // And then just duplicating it here

  CRGBPalette16 myPal = example_gp;

  CRGB FR_Plus[] = { 
      CRGB::BlueViolet,
      CRGB::Cyan, 
      CRGB::Chartreuse, 
      CRGB::Yellow, 
      CRGB::DarkOrange,
      CRGB::Black,
      CRGB::Black};


//---------------------------------------------------------------------------//

void setup() {

  // Recovery
  delay(3000);

  // Boot LEDs 
  FastLED.addLeds<LED_TYPE,DATA_PIN_1,COLOR_ORDER>(leds1, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE,DATA_PIN_2,COLOR_ORDER>(leds2, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE,DATA_PIN_3,COLOR_ORDER>(leds3, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  FastLED.setMaxPowerInVoltsAndMilliamps( 5, MAX_POWER_MILLIAMPS);
  currentBlending = LINEARBLEND;
  
  // Initialize counters
  
  colorIndex = 0;
  cyclerIndex = 0; 

  // Set all LEDs to 'OFF'
  for (int i = 0; i < NUM_LEDS; i++) { //for all LEDS in leds1
    leds1[i] = CRGB::Black;
    leds2[i] = CRGB::Black;
    leds3[i] = CRGB::Black;
    }
  FastLED.show();

}

// List of patterns to cycle through.  Each pattern is defined as a separate function further down in the code.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {majorCyclonic, cyclonic, cyclonePulses, chaoticFlashes, confetti, sinelon, bpm, juggle};
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns



//---------------------------------------------------------------------------//

void loop() {

    // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 15 ) { nextPattern(); } // change patterns periodically

}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}


//---------------------------------------------------------------------------//


void majorCyclonic() {
  
  // spirals RGB colors on through all three strips in a single pulse

  // Set all LEDs to 'OFF'
  for (int i = 0; i < NUM_LEDS; i++) { //for all LEDS in leds1
    leds1[i] = CRGB::Black;
    leds2[i] = CRGB::Black;
    leds3[i] = CRGB::Black;
    }
  FastLED.show();

static uint8_t hue = 0;
  
  // First slide the led in one direction
  for(int i = 0; i < 3 * NUM_LEDS; i++) {
    
    hue++;
    
    // Set the i'th led
    if (i < NUM_LEDS){ leds1[i] = CHSV(hue, 255, 255);}// First Strand
    else if (i < 2 * NUM_LEDS){leds2[i-NUM_LEDS] = CHSV(hue, 255, 255);} // Second Strand
    else {leds3[i-2 * NUM_LEDS] = CHSV(hue, 255, 255);} // Third Strand  
    
    // Show the leds
    FastLED.show(); 
    
    // now that we've shown the leds, make them all fade a little bit.
    fadeall();
  }
  
  // Now go in the other direction.  
  for(int i = (3 * NUM_LEDS)-1; i >= 0; i--) {

    hue++; 

    // Set the i'th led
    if (i < NUM_LEDS){ leds1[i] = CHSV(hue, 255, 255);}// First Strand
    else if (i < 2 * NUM_LEDS){leds2[i-NUM_LEDS] = CHSV(hue, 255, 255);} // Second Strand
    else {leds3[i-2 * NUM_LEDS] = CHSV(hue, 255, 255);} // Third Strand  
    
    // Show the leds
    FastLED.show();
    
    // now that we've shown the leds, make them all fade a little bit.
    fadeall();
  }
  
}

//-----------------------------------------------------------------------//

void cyclonePulses(){
  
  // spirals random colors from an array through all three strips, one at a time, such that one-third of all LEDs are lit at a given time.
  
  for (int i = 0; i < NUM_LEDS; i++) { //for all LEDS in leds1
    colorIndex = random8( sizeof(FR_Colors) / sizeof(FR_Colors[0]) );
    leds1[i] = FR_Colors[colorIndex];
    leds3[i] = CRGB::Black;
    FastLED.show();
    }

  for (int j = 0; j < NUM_LEDS; j++) { //for all LEDS in leds2
    colorIndex = random8( sizeof(FR_Colors) / sizeof(FR_Colors[0]) );
    leds1[j] = CRGB::Black;
    leds2[j] = FR_Colors[colorIndex];
    FastLED.show();
    }

  for (int k = 0; k < NUM_LEDS; k++) { //for all LEDS in leds1
    colorIndex = random8( sizeof(FR_Colors) / sizeof(FR_Colors[0]) );
    leds2[k] = CRGB::Black;
    leds3[k] = FR_Colors[colorIndex];
    FastLED.show();
    }

}

//---------------------------------------------------------------------------//


void cyclonic() {
  
// spirals colors on all three strips simultaneously

static uint8_t hue = 0;
  
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i++) {
    
    // Set the i'th led to red 
    leds1[i] = CHSV(hue++, 255, 255);
    leds2[i] = CHSV(hue++, 255, 255);
    leds3[i] = CHSV(hue++, 255, 255);
    
    // Show the leds
    FastLED.show(); 
    
    // now that we've shown the leds, make them all fade a little bit.
    fadeall();
    
    // Wait a little bit before we loop around and do it again
    delay(10);
  }

  // Now go in the other direction.  
  for(int i = (NUM_LEDS)-1; i >= 0; i--) {
    
    // Set the i'th led to red 
    leds1[i] = CHSV(hue++, 255, 255);
    leds2[i] = CHSV(hue, 255, 255);
    leds3[i] = CHSV(hue, 255, 255);
    
    // Show the leds
    FastLED.show();
    
    // now that we've shown the leds, make them all fade a little bit.
    fadeall();

    // Wait a little bit before we loop around and do it again
    delay(10);
  }
  
}

//---------------------------------------------------------------------------//

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { 
  
  // Used in cyclonic programs to fade out all LEDs
  
  leds1[i].nscale8(250); 
  leds2[i].nscale8(250); 
  leds3[i].nscale8(250); 
  } 
} 

//---------------------------------------------------------------------------//

void chaoticFlashes(){
  
  // randomly assigns colors from an array to all LEDs and then flashes them for a fraction of a second, repeating

  for (int i = 0; i < NUM_LEDS; i++) { //for all LEDS in leds1
    
    colorIndex = random8( sizeof(FR_Plus) / sizeof(FR_Plus[0]) );
    leds1[i] = FR_Plus[colorIndex];
  }

    for (int j = 0; j < NUM_LEDS; j++) { //for all LEDS in leds1
    
    colorIndex = random8( sizeof(FR_Plus) / sizeof(FR_Plus[0]) );
    leds2[j] = FR_Plus[colorIndex];
  }
  
    for (int k = 0; k < NUM_LEDS; k++) { //for all LEDS in leds1
    
    colorIndex = random8( sizeof(FR_Plus) / sizeof(FR_Plus[0]) );
    leds3[k] =  FR_Plus[colorIndex];
  }

  FastLED.show();
  delay(500);
  
}


//-----------------------------------------------------------------------//

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fadeToBlackBy( leds2, NUM_LEDS, 20);
  fadeToBlackBy( leds3, NUM_LEDS, 20);
  int pos = random16(NUM_LEDS);
  leds1[pos] += CHSV( gHue + random8(64), 200, 255);
  leds2[pos] += CHSV( gHue + random8(64), 200, 255);
  leds3[pos] += CHSV( gHue + random8(64), 200, 255);
}

//-----------------------------------------------------------------------//

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fadeToBlackBy( leds2, NUM_LEDS, 20);
  fadeToBlackBy( leds3, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds1[pos] += CHSV( gHue, 255, 192);
  leds2[pos] += CHSV( gHue, 255, 192);
  leds3[pos] += CHSV( gHue, 255, 192);
}

//-----------------------------------------------------------------------//


void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds1[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds2[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds3[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

//-----------------------------------------------------------------------//


void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fadeToBlackBy( leds2, NUM_LEDS, 20);
  fadeToBlackBy( leds3, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds1[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    leds2[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    leds3[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

//-----------------------------------------------------------------------//


void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
  uint8_t brightness = 255;
  for ( int i = 0; i < NUM_LEDS; i++) {
    leds1[i] = 
//      leds1[i] = ColorFromPalette( FR_Color_Pallete, colorIndex, brightness, currentBlending);
      leds2[i] = leds1[i];
      leds3[i] = leds1[i];
      colorIndex += 3;
  }
}

//-----------------------------------------------------------------------//
void gradientDisplay(){

  for (uint8_t i=0; i<NUM_LEDS; i++) {
    uint8_t paletteIndex = map(i,0,NUM_LEDS-1,0,240);  //index is now based on pixel number
    leds1[i] = ColorFromPalette( myPal, paletteIndex, BRIGHTNESS, LINEARBLEND);
  }

    for (uint8_t j=0; j<NUM_LEDS; j++) {
    uint8_t paletteIndex = map(j,0,NUM_LEDS-1,0,240);  //index is now based on pixel number
    leds2[j] = ColorFromPalette( myPal, paletteIndex, BRIGHTNESS, LINEARBLEND);
  }

    for (uint8_t k=0; k<NUM_LEDS; k++) {
    uint8_t paletteIndex = map(k,0,NUM_LEDS-1,0,240);  //index is now based on pixel number
    leds3[k] = ColorFromPalette( myPal, paletteIndex, BRIGHTNESS, LINEARBLEND);
  }
  
  FastLED.show();
  FastLED.delay(5000);
  
}

//-----------------------------------------------------------------------//


void fadeOut() { 

  int brightness = BRIGHTNESS;
  
  while(brightness>0){

    for (uint8_t i=0; i<NUM_LEDS; i++) {
    uint8_t paletteIndex = map(i,0,NUM_LEDS-1,0,240);  //index is now based on pixel number
    leds1[i] = ColorFromPalette( myPal, paletteIndex, brightness, LINEARBLEND);
  }

    for (uint8_t j=0; j<NUM_LEDS; j++)  {
    uint8_t paletteIndex = map(j,0,NUM_LEDS-1,0,240);  //index is now based on pixel number
    leds2[j] = ColorFromPalette( myPal, paletteIndex, brightness, LINEARBLEND);
  }

    for (uint8_t k=0; k<NUM_LEDS; k++) {
    uint8_t paletteIndex = map(k,0,NUM_LEDS-1,0,240);  //index is now based on pixel number
    leds3[k] = ColorFromPalette( myPal, paletteIndex, brightness, LINEARBLEND);
  }

  FastLED.show();
//  FastLED.delay(5000/BRIGHTNESS);
  brightness = brightness - 1;
  
  }

  brightness = BRIGHTNESS;
  
//  delay(5000);
  
}
