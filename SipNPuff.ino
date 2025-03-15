#include <Mouse.h>
#include <HX711.h>

const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
HX711 scale;

bool pressure = false;
bool lastPressure = false;
bool vacuum = false;
bool lastVacuum = false;

const int buttonPin = 12;
bool button = false;
bool lastButton = false;

int xPin = A0;
int yPin = A1;

int xVal = 0;
int yVal = 0;

// Timing control
unsigned long lastScaleRead = 0;
unsigned long lastSerialPrint = 0;
unsigned long lastMouseMove = 0;

float weight = 0;

void setup() {
  Serial.begin(9600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.tare();
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  unsigned long currentMillis = millis();

  // Read load cell every 35 - 50 ms
  if (currentMillis - lastScaleRead >= 35) {
    weight = scale.get_units(1); // Reduced to 1 unit reading for speed
    lastScaleRead = currentMillis;

    // Update pressure/vacuum status
    pressure = (weight >= 50000);
    vacuum = (weight <= -50000);
  }

  // Read button and joystick
  int buttonState = digitalRead(buttonPin);
  button = (buttonState == LOW);
  xVal = analogRead(xPin);
  yVal = analogRead(yPin);

  // Handle mouse clicks
  if (pressure && !lastPressure) {
    Mouse.press(MOUSE_LEFT);
    lastPressure = true;
  } else if (!pressure && lastPressure) {
    Mouse.release(MOUSE_LEFT);
    lastPressure = false;
  }

  if (vacuum && !lastVacuum) {
    Mouse.click(MOUSE_RIGHT);
    lastVacuum = true;
  } else if (!vacuum && lastVacuum) {
    Mouse.release(MOUSE_RIGHT);
    lastVacuum = false;
  }

  if (button && !lastButton) {
    Mouse.press(MOUSE_MIDDLE);
    lastButton = true;
  } else if (!button && lastButton) {
    Mouse.release(MOUSE_MIDDLE);
    lastButton = false;
  }

  // Handle mouse movement every 5 - 15 ms for smoothness
  if (currentMillis - lastMouseMove >= 2) {
    int xMove = 0;
    int yMove = 0;

    if (xVal > 504) xMove = (1 + 0.05 * (xVal - 504));
    if (xVal < 502) xMove = (-0.05 * (502 - xVal) - 1);
    if (yVal > 500) yMove = (1 + 0.05 * (yVal - 500));
    if (yVal < 498) yMove = (-0.05 * (498 - yVal) - 1);

    Mouse.move(xMove, yMove, 0);
    lastMouseMove = currentMillis;
  }

  // Optional: Serial print every 500 ms for debugging
  //if (currentMillis - lastSerialPrint >= 500) {
    //Serial.print("xValue: "); Serial.println(xVal);
    //Serial.print("yValue: "); Serial.println(yVal);
    //Serial.print("Button: "); Serial.println(button);
    //Serial.print("Pressure: "); Serial.println(pressure);
    //Serial.print("Vacuum: "); Serial.println(vacuum);
    //Serial.println();
    //lastSerialPrint = currentMillis;
  //}
}