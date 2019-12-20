#include <Servo.h>

bool firstRunSuccessful = false;

int ch1raw;
int ch1correct;

int ch2raw;
int ch2correct;

int ch3raw;
int ch3correct;

int ch4raw; 
int ch4correct;

Servo esc9; // SILVER_B
Servo esc10; // BLACK_B
Servo esc11; // BLACK_A
Servo esc12; // SILVER_A

int esc9Val;
int esc10Val;
int esc11Val;
int esc12Val;

void setup() {
  // put your setup code here, to run once:

  pinMode(4, INPUT); // Roll
  pinMode(5, INPUT); // Throttle
  pinMode(6, INPUT); // Pitch
  pinMode(7, INPUT); // Yaw

  esc9.attach(9);
  esc10.attach(10);
  esc11.attach(11);
  esc12.attach(12);
  
  esc9.writeMicroseconds(1000);
  esc10.writeMicroseconds(1000);
  esc11.writeMicroseconds(1000);
  esc12.writeMicroseconds(1000);

  delay(1000);

  esc9.detach();
  esc10.detach();

  delay(1000);

  esc9.attach(10);
  esc10.attach(9);

  esc9.writeMicroseconds(1000);
  esc10.writeMicroseconds(1000);

  Serial.begin(9600);
  Serial.println("setup complete");
  
}

void loop() {
  // put your main code here, to run repeatedly:

  delay(100);
  
  noInterrupts();
  ch1raw = pulseIn(4, HIGH); // Roll
  ch2raw = pulseIn(5, HIGH); // throttle
  ch3raw = pulseIn(6, HIGH); // Pitch
  ch4raw = pulseIn(7, HIGH); // Yaw
  interrupts();
  
  //ch1correct = map(ch1raw, 1, 1, 1, 1);
  ch2correct = map(ch2raw, 1083, 1985, 2000, 1000);
  //ch3correct = map(ch3raw, 1, 1, 1, 1);
  //ch4correct = map(ch4raw, 1, 1, 1, 1);

  masterControl(ch1raw, ch2correct, ch3raw, ch4raw);

}

void masterControl(int roll, int throttle, int pitch, int yaw) {

  int minThrottle = 1180;

  if ((roll == 0) && (throttle == 3200) && (pitch == 0) && (yaw == 0)) {
    Serial.println("Turn on remote");
    return;
  }
  if ((roll < 1500) && (roll > 1496) && (throttle > 1538) && (throttle < 1544) && (pitch < 874)
      && (pitch > 870) && (yaw < 1500) && (yaw > 1496)) {
    
    if (firstRunSuccessful) {
      emergencyLanding();
    } else {
      Serial.println("Turn on remote");
    }
    return;      
  }

  if (throttle < minThrottle) {
    esc9Val = 1000;
    esc10Val = 1000;
    esc11Val = 1000;
    esc12Val = 1000;
    esc9.writeMicroseconds(esc9Val);
    esc10.writeMicroseconds(esc10Val);
    esc11.writeMicroseconds(esc11Val);
    esc12.writeMicroseconds(esc12Val);
    return; // drone must be flying in order to operate others.
  }

  setMotorSpeed(roll, throttle, pitch, yaw);

  firstRunSuccessful = true;
  ///*
  Serial.print(roll);
  Serial.print(", ");
  Serial.print(throttle);
  Serial.print(", ");
  Serial.print(pitch);
  Serial.print(", ");
  Serial.print(yaw);
  Serial.print(" | ");
  Serial.print(esc9Val); // Silver_B
  Serial.print(", ");
  Serial.print(esc10Val); // Black_B
  Serial.print(", ");
  Serial.print(esc11Val); // Black_A
  Serial.print(", ");
  Serial.println(esc12Val); // Silver_A
  //*/
  
  esc9.writeMicroseconds(esc9Val);
  esc10.writeMicroseconds(esc10Val);
  esc11.writeMicroseconds(esc11Val);
  esc12.writeMicroseconds(esc12Val);
  
}

void setMotorSpeed(int roll, int throttle, int pitch, int yaw) {
  
  int maxMotorSpeed = 2000;

  // if everything is at rest, set values to the throttle (hover)
  if ((pitch > 1465) && (pitch < 1475) && (roll > 1505) && (roll < 1515)) {
    esc9Val = throttle;
    esc10Val = throttle;
    esc11Val = throttle;
    esc12Val = throttle;
    return;
  }

  // Assign each motor value to its physical motor position. Done for readability.
  int s_A = esc12Val;
  int b_A = esc11Val;
  int s_B = esc9Val;
  int b_B = esc10Val;

  // get 4 motor speeds based on throttle and pitch
  int pitchDifference;
  int frontMotorSpeed;
  int backMotorSpeed;
  if (pitch < 1465) {
    pitchDifference = map(pitch, 1000, 1469, 500, 0);

    frontMotorSpeed = throttle + pitchDifference;
    backMotorSpeed = throttle;
    if (frontMotorSpeed > maxMotorSpeed) {
      int subtractionTotal = frontMotorSpeed - maxMotorSpeed;

      frontMotorSpeed = 2000;
      backMotorSpeed = backMotorSpeed - subtractionTotal;
    }
    
  } else if (pitch > 1475) {
    pitchDifference = map(pitch, 1470, 2000, 0, 500);
    // int forward = map(pitch, 1470, 2000, 0, 500);

    backMotorSpeed = throttle + pitchDifference;
    frontMotorSpeed = throttle;
    if (backMotorSpeed > maxMotorSpeed) {
      int subtractionTotal = backMotorSpeed - maxMotorSpeed;

      backMotorSpeed = 2000;
      frontMotorSpeed = frontMotorSpeed - subtractionTotal;
    }
    
  }

  s_A = frontMotorSpeed;
  b_A = frontMotorSpeed;
  s_B = backMotorSpeed;
  b_B = backMotorSpeed;

  // update 4 motor speeds to account for roll
  int rollDifference;
  int leftMotorSpeed;
  int rightMotorSpeed;
  if (roll < 1505) { // Left
    rollDifference = map(roll, 1000, 1509, 500, 0);

    //rightMotorSpeed is high
    rightMotorSpeed = throttle + rollDifference;
    leftMotorSpeed = throttle;
    if (rightMotorSpeed > maxMotorSpeed) {
      int subtractionTotal = rightMotorSpeed - maxMotorSpeed;

      rightMotorSpeed = 2000;
      leftMotorSpeed = leftMotorSpeed - subtractionTotal;
    }
    
  } else if (roll > 1515) { // Right
    rollDifference = map(roll, 1510, 2000, 0, 500);
    
    //leftMotorSpeed is high
    leftMotorSpeed = throttle + rollDifference;
    rightMotorSpeed = throttle;
    if (leftMotorSpeed > maxMotorSpeed) {
      int subtractionTotal = leftMotorSpeed - maxMotorSpeed;

      leftMotorSpeed = 2000;
      rightMotorSpeed = rightMotorSpeed - subtractionTotal;
    }
    
  }

  // XXX
  // Consider frontMotorSpeed, backMotorSpeed, leftMotorSpeed, rightMotorSpeed, to assign values.
  //s_A | frontMotorSpeed, leftMotorSpeed
  //b_A | frontMotorSpeed, rightMotorSpeed
  //s_B | backMotorSpeed, rightMotorSpeed
  //b_B | backMotorSPeed, leftMotorSpeed
  s_A = (frontMotorSpeed + leftMotorSpeed) / 2;
  b_A = (frontMotorSpeed + rightMotorSpeed) / 2;
  s_B = (backMotorSpeed + rightMotorSpeed) / 2;
  b_B = (backMotorSpeed + leftMotorSpeed) / 2;

  // Assign each motor speed to its respective esc.
  esc9Val = s_B;
  esc10Val = b_B;
  esc11Val = b_A;
  esc12Val = s_A;
  
}

int roll(int rollInput) {
  return 0;
}

int throttle(int throttleInput) {
  return 0;
}

int pitch(int pitchInput) {
  return 0;
}

int yaw(int yawInput) {
  return 0;
}

// XXX
void emergencyLanding() {
  Serial.println("Emergency Landing");
  
  int landingSpeedList[] = {
    2000,
    1870,
    1755,
    1652,
    1572,
    1510,
    1480,
    1460,
    1420,
    1380,
    1340,
    1300,
    1280,
    1260,
    1245,
    1180,
  };
  int emergencyMotorSpeed = 2000;
  esc9.writeMicroseconds(emergencyMotorSpeed);
  esc10.writeMicroseconds(emergencyMotorSpeed);
  esc11.writeMicroseconds(emergencyMotorSpeed);
  esc12.writeMicroseconds(emergencyMotorSpeed);

  int i = 0;
  while (i < 16) {
    delay(1000);
    Serial.println(landingSpeedList[i]);
    
    esc9.writeMicroseconds(landingSpeedList[i]);
    esc10.writeMicroseconds(landingSpeedList[i]);
    esc11.writeMicroseconds(landingSpeedList[i]);
    esc12.writeMicroseconds(landingSpeedList[i]);
    
    i++;
  }

  delay(5000);

  esc9.writeMicroseconds(1000);
  esc10.writeMicroseconds(1000);
  esc11.writeMicroseconds(1000);
  esc12.writeMicroseconds(1000);

  Serial.println("Emergency Landing Successful");
  while (true) {
  }
  
}
