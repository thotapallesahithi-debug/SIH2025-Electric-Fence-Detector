/* ----------------------------------------------------------
   DUAL FENCE DETECTION – LEFT & RIGHT
   Two identical inductive-coil sensors,
   both connected to a single Arduino UNO.
-----------------------------------------------------------*/

// ANALOG INPUTS
const int LEFT_SENSOR  = A0;
const int RIGHT_SENSOR = A1;

// LED OUTPUTS
const int LEFT_LED  = 9;
const int RIGHT_LED = 8;

// Adaptive baseline filters
float baselineL = 0;
float baselineR = 0;

// Sensitivity threshold
const int SENSITIVITY = 30;

// Timing controls
const unsigned long DEBOUNCE_TIME    = 350;
const unsigned long NO_PULSE_TIMEOUT = 3000;

unsigned long lastPulseL = 0;
unsigned long lastPulseR = 0;

bool risingL = false;
bool risingR = false;

void setup() {
  Serial.begin(9600);

  pinMode(LEFT_LED, OUTPUT);
  pinMode(RIGHT_LED, OUTPUT);

  digitalWrite(LEFT_LED, LOW);
  digitalWrite(RIGHT_LED, LOW);

  delay(300);
  Serial.println("Dual Fence Detector Active...");
}

// --------------------------------------------------------------
// MAIN LOOP
// --------------------------------------------------------------
void loop() {

  unsigned long now = millis();

  // Read analog values
  int rawL = analogRead(LEFT_SENSOR);
  int rawR = analogRead(RIGHT_SENSOR);

  // Adaptive baseline update
  baselineL = (0.995 * baselineL) + (0.005 * rawL);
  baselineR = (0.995 * baselineR) + (0.005 * rawR);

  int diffL = rawL - baselineL;
  int diffR = rawR - baselineR;

  // =====================================================
  // LEFT SENSOR LOGIC
  // =====================================================
  if (diffL > SENSITIVITY && !risingL) {

    unsigned long dt = now - lastPulseL;

    if (dt > DEBOUNCE_TIME) {
      risingL = true;

      if (lastPulseL > 0) {

        Serial.print("LEFT PULSE INTERVAL = ");
        Serial.println(dt);

        if (dt < 1000) {
          Serial.println("UNAUTHORIZED FENCE DETECTED (LEFT)");
          digitalWrite(LEFT_LED, HIGH);
          digitalWrite(RIGHT_LED, LOW);
        } else {
          digitalWrite(LEFT_LED, LOW);
        }

        Serial.println("--------------------------------");
      }

      lastPulseL = now;
    }
  }

  if (diffL < (SENSITIVITY / 2)) {
    risingL = false;
  }

  if (lastPulseL != 0 && (now - lastPulseL > NO_PULSE_TIMEOUT)) {
    digitalWrite(LEFT_LED, LOW);
    lastPulseL = 0;
  }


  // =====================================================
  // RIGHT SENSOR LOGIC
  // =====================================================
  if (diffR > SENSITIVITY && !risingR) {

    unsigned long dt = now - lastPulseR;

    if (dt > DEBOUNCE_TIME) {
      risingR = true;

      if (lastPulseR > 0) {

        Serial.print("RIGHT PULSE INTERVAL = ");
        Serial.println(dt);

        if (dt < 1000) {
          Serial.println("UNAUTHORIZED FENCE DETECTED (RIGHT)");
          digitalWrite(RIGHT_LED, HIGH);
          digitalWrite(LEFT_LED, LOW);
        } else {
          digitalWrite(RIGHT_LED, LOW);
        }

        Serial.println("--------------------------------");
      }

      lastPulseR = now;
    }
  }

  if (diffR < (SENSITIVITY / 2)) {
    risingR = false;
  }

  if (lastPulseR != 0 && (now - lastPulseR > NO_PULSE_TIMEOUT)) {
    digitalWrite(RIGHT_LED, LOW);
    lastPulseR = 0;
  }

  delay(4);
}
