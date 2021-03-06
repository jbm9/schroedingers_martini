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
  ServoLeftStart = 5,
  ServoLeftStop = 110,

  ServoRightStart = 175,
  ServoRightStop = 60,
};

#define N_SERVO_STEPS 15

int servo_steps[N_SERVO_STEPS] = { 0, 10, 20, 35, 50,
                                   58, 66, 74, 82, 90,
				   95, 100, 105, 108, 110, };


enum Delays {
  DELAY_READINGS = 5000, // 5s of blinky to get a reading
};




Servo servo_left;
Servo servo_right;


volatile int geiger_counter;

void geiger_isr() {
  geiger_counter++;
}


void pour(uint8_t which_servo) {
  uint8_t led_intensity = 255; // How bright to pound the LED for the showy bits below
  uint8_t brightness = 128;

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
    delay(100);
  }

  Serial.println("Returning cup to zero");
  delay(250);

  Serial.println("Returning cup to zero");
  for (int i = N_SERVO_STEPS-1; i >= 0; i--) {
    if (which_servo) {
      servo_right.write(ServoRightStart - servo_steps[i]);
      Serial.println(ServoRightStart - servo_steps[i]);
    } else {
      servo_left.write(ServoLeftStart + servo_steps[i]);
      Serial.println(ServoLeftStart + servo_steps[i]);
    }
    analogWrite(PIN_BUTTON_LED, brightness);
    brightness -= 255/(2*N_SERVO_STEPS);
    Serial.println(brightness);
    delay(100);
  }

  analogWrite(PIN_BUTTON_LED, led_intensity);
  delay(100);

}



void setup() {
  Serial.begin(9600);
  //  Serial.println("Schroedinger's Martini Bot v0.1");

  geiger_counter = 0;
  attachInterrupt(0, geiger_isr, RISING);


  pinMode(PIN_BUTTON, INPUT);
  digitalWrite(PIN_BUTTON, 1); // turn on pull-up

  pinMode(PIN_BUTTON_LED, OUTPUT);

  pinMode(PIN_DIAGNOSTIC, OUTPUT);

  servo_left.attach(9);
  servo_right.attach(10);

  servo_left.write(ServoLeftStart);
  servo_right.write(ServoRightStart);

  pour(0);
  pour(1);

}




void loop() {
  Serial.print("Loop! ");
  //  Serial.println(geiger_counter);
  geiger_counter = 0;

  /* ****************************** */
  // Breathing while waiting for button push

  uint8_t do_breathe = 1;
  uint8_t brightness = 0;

  //  Serial.println("Waiting");

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
  //  Serial.println(geiger_counter);
  uint8_t which_servo = geiger_counter & 1;

  /* ****************************** */
  // Commence the jigglin'!

  Serial.print("Pouring: ");
  //  Serial.println(which_servo ? "R" : "L");

  pour(which_servo);


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
