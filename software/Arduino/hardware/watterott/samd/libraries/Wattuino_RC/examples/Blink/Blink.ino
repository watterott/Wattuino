/*
 * Blink Example
 *
 * Turns on an LED for one second, then off for one second, repeatedly.
 */

// setup function, runs once when you press reset or power the board
void setup()
{
  // J1...J7 have LEDs
  pinMode(RCJ1, OUTPUT);     // initialize pin as an output
}

// loop function, runs over and over again forever
void loop()
{
  digitalWrite(RCJ1, HIGH);  // turn LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(RCJ1, LOW);   // turn LED off by making the voltage LOW
  delay(1000);              // wait for a second
}
