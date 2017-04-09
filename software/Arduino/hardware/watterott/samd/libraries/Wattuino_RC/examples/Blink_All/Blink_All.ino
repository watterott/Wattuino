/*
 * Blink Example
 *
 * Turns on all LEDs for one second, then off for one second, repeatedly.
 */

// setup function, runs once when you press reset or power the board
void setup()
{
  // initialize pins as outputs
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RCJ1, OUTPUT);
  pinMode(RCJ2, OUTPUT);
  pinMode(RCJ3, OUTPUT);
  pinMode(RCJ4, OUTPUT);
  pinMode(RCJ5, OUTPUT);
  pinMode(RCJ6, OUTPUT);
  pinMode(RCJ7, OUTPUT);
  pinMode(RCS1, OUTPUT);
}

// loop function, runs over and over again forever
void loop()
{
  // turn LEDs on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(RCJ1, HIGH);
  digitalWrite(RCJ2, HIGH);
  digitalWrite(RCJ3, HIGH);
  digitalWrite(RCJ4, HIGH);
  digitalWrite(RCJ5, HIGH);
  digitalWrite(RCJ6, HIGH);
  digitalWrite(RCJ7, HIGH);
  digitalWrite(RCS1, HIGH);
  
  delay(1000); // wait for a second

  // turn LEDs off by making the voltage LOW
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(RCJ1, LOW);
  digitalWrite(RCJ2, LOW);
  digitalWrite(RCJ3, LOW);
  digitalWrite(RCJ4, LOW);
  digitalWrite(RCJ5, LOW);
  digitalWrite(RCJ6, LOW);
  digitalWrite(RCJ7, LOW);
  digitalWrite(RCS1, LOW);

  delay(1000); // wait for a second
}
