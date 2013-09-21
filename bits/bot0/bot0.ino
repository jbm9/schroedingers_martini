/* Local Variables:  */
/* mode: c++         */
/* comment-column: 0 */
/* End:              */


#include <Servo.h>

enum SMPins {
  PIN_GEIGER = 2, // This must be 2 or 3 for interrupts, see attachInterrupt()

  PIN_BUTTON = 4,    // Input for button, any digital line
  PIN_BUTTON_LED = 5, // PWM'able output for button LED

  // Servo is going to eat 9/10 as PWM outputs anyway, so we use those
  PIN_SERVO_L = 9,  // Output for left servo
  PIN_SERVO_R = 10, // Output for right servo

  PIN_DIAGNOSTIC = 13, // "diagnostic" LED
};

enum ServoJunk {
  ServoLeftStart = 0,
  ServoLeftStop = 80,

  ServoRightStart = 170,
  ServoRightStop = 90,
};

#define N_SERVO_STEPS 10

int servo_steps[N_SERVO_STEPS] = { 12, 24, 36, 46,  54,
				   62, 68, 74, 78,  80, };


enum Delays {
  DELAY_READINGS = 5000, // 5s of blinky to get a reading
};




Servo servo_left;
Servo servo_right;


volatile int geiger_counter;

void geiger_isr() {
  geiger_counter++;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Schroedinger's Martini Bot v0.1");

  geiger_counter = 0;
  attachInterrupt(0, geiger_isr, RISING);


  pinMode(PIN_BUTTON, INPUT);
  digitalWrite(PIN_BUTTON, 1); // turn on pull-up

  pinMode(PIN_BUTTON_LED, OUTPUT);

  pinMode(PIN_SERVO_L, OUTPUT);
  pinMode(PIN_SERVO_R, OUTPUT);

  pinMode(PIN_DIAGNOSTIC, OUTPUT);


  servo_left.attach(PIN_SERVO_L);
  servo_right.attach(PIN_SERVO_R);

  servo_left.write(ServoLeftStop);
  servo_right.write(ServoRightStop);

  delay(1000);
  servo_left.write(ServoLeftStart);
  servo_right.write(ServoRightStart);

}


void loop() {
  Serial.print("Loop! ");
  Serial.println(geiger_counter);
  geiger_counter = 0;

  /* ****************************** */
  // Breathing while waiting for button push

  uint8_t do_breathe = 1;
  uint8_t brightness = 0;

  Serial.println("Waiting");

  while(do_breathe) { // TODO Make LED "breathe" while waiting for input: http://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/

    for (brightness = 0; do_breathe && brightness < 245; brightness += 10) {
      analogWrite(PIN_BUTTON_LED, brightness);
      do_breathe = digitalRead(PIN_BUTTON);
      delay(100);
    }

    for (; do_breathe && brightness >= 10; brightness -= 10) {
      analogWrite(PIN_BUTTON_LED, brightness);
      do_breathe = digitalRead(PIN_BUTTON);
      delay(80);
    }
  }


  /* ****************************** */
  // Silly showmanship while reading the geiger counter data

  Serial.print("Reading: ");

  uint8_t led_intensity = 255; // How bright to pound the LED for the showy bits below

  geiger_counter = 0;

  for (int delay_counter = 0; delay_counter < DELAY_READINGS/200; delay_counter++) {
    analogWrite(PIN_BUTTON_LED, led_intensity);
    delay(100);
    analogWrite(PIN_BUTTON_LED, 0);
    delay(100);
  }

  /* ****************************** */
  // Decide which servo to pour from
  Serial.println(geiger_counter);
  uint8_t which_servo = geiger_counter & 1;

  /* ****************************** */
  // Commence the jigglin'!

  Serial.print("Pouring: ");
  Serial.println(which_servo ? "R" : "L");

  for (int i = 0; i < N_SERVO_STEPS; i++) {
    if (which_servo) {
      servo_right.write(ServoRightStart - servo_steps[i]);
      Serial.println(ServoRightStart - servo_steps[i]);
    } else {
      servo_left.write(ServoLeftStart + servo_steps[i]);
      Serial.println(ServoLeftStart + servo_steps[i]);
    }
    analogWrite(PIN_BUTTON_LED, brightness);
    brightness -= 255/(2*N_SERVO_STEPS);
    delay(25);
  }

  delay(250);

  Serial.println("Returning cup to zero");
  for (int i = N_SERVO_STEPS; i >= 0; i--) {
    if (which_servo) {
      servo_right.write(ServoRightStart - servo_steps[i]);
      Serial.println(ServoRightStart - servo_steps[i]);
    } else {
      servo_left.write(ServoLeftStart + servo_steps[i]);
      Serial.println(ServoLeftStart + servo_steps[i]);
    }
    analogWrite(PIN_BUTTON_LED, brightness);
    brightness -= 255/(2*N_SERVO_STEPS);
    delay(25);
  }

  analogWrite(PIN_BUTTON_LED, led_intensity);
  delay(100);


  digitalWrite(PIN_DIAGNOSTIC, 1);
  delay(20);
  digitalWrite(PIN_DIAGNOSTIC, 0);
  delay(20);

  if (which_servo) {
    digitalWrite(PIN_DIAGNOSTIC, 1);
    delay(20);
    digitalWrite(PIN_DIAGNOSTIC, 0);
    delay(20);
  }

}
