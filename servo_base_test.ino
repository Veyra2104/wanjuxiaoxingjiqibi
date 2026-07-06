#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_CH   0
#define SERVOMIN   150
#define SERVOMAX   600
#define CENTER     375

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);
  delay(10);

  // 先归位到原点 90 度
  pwm.setPWM(SERVO_CH, 0, CENTER);
  delay(1000);
}

void loop() {
  // 循环 3 次
  for (int i = 1; i <= 3; i++) {
    Serial.print("Round ");
    Serial.println(i);

    // 90 度慢慢转到 0 度
    for (int pulse = CENTER; pulse >= SERVOMIN; pulse--) {
      pwm.setPWM(SERVO_CH, 0, pulse);
      delay(15);
    }

    // 0 度慢慢转到 180 度
    for (int pulse = SERVOMIN; pulse <= SERVOMAX; pulse++) {
      pwm.setPWM(SERVO_CH, 0, pulse);
      delay(15);
    }

    // 180 度慢慢转回 90 度原点
    for (int pulse = SERVOMAX; pulse >= CENTER; pulse--) {
      pwm.setPWM(SERVO_CH, 0, pulse);
      delay(15);
    }

    delay(500);
  }

  Serial.println("Done. Stopped at 90 degree.");
  // 停在原点，不再执行
  while (1);
}
