#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define BASE      0    // 底座 (90原点, 大=右, 小=左)
#define SHOULDER  10   // 大臂 (90原点)
#define ELBOW     8    // 小臂 (90原点)
#define GRIPPER   6    // 夹子 (90原点, 大=开, 小=闭)

#define PMIN    150
#define PMAX    600
#define CENTER  375

int pmap(int angle) {
  return map(constrain(angle, 0, 180), 0, 180, PMIN, PMAX);
}

void quadMove(int fromB, int toB, int fromS, int toS,
              int fromE, int toE, int fromG, int toG, int spd) {
  int steps = max(max(abs(toB - fromB), abs(toS - fromS)),
                  max(abs(toE - fromE), abs(toG - fromG)));
  if (steps == 0) steps = 1;

  for (int i = 0; i <= steps; i++) {
    int pB = fromB + (toB - fromB) * i / steps;
    int pS = fromS + (toS - fromS) * i / steps;
    int pE = fromE + (toE - fromE) * i / steps;
    int pG = fromG + (toG - fromG) * i / steps;
    pwm.setPWM(BASE, 0, pmap(pB));
    pwm.setPWM(SHOULDER, 0, pmap(pS));
    pwm.setPWM(ELBOW, 0, pmap(pE));
    pwm.setPWM(GRIPPER, 0, pmap(pG));
    delay(spd);
  }
}

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);
  delay(10);

  Serial.println(F("归位：底座90 大臂90 小臂90 夹子90"));
  pwm.setPWM(BASE, 0, CENTER);
  pwm.setPWM(SHOULDER, 0, CENTER);
  pwm.setPWM(ELBOW, 0, CENTER);
  pwm.setPWM(GRIPPER, 0, CENTER);
  delay(2000);
}

void loop() {
  // [1] 向下前伸
  Serial.println(F("[1] 向下前伸"));
  quadMove(90, 90, 90, 180, 90, 82, 90, 90, 30);
  delay(5000);

  // [2] 闭合夹子
  Serial.println(F("[2] 闭合夹子"));
  quadMove(90, 90, 180, 180, 82, 82, 90, 60, 25);
  delay(500);

  // [3] 回到平常姿态
  Serial.println(F("[3] 回到平常姿态（夹子闭合）"));
  quadMove(90, 90, 180, 125, 82, 95, 60, 60, 30);
  delay(2000);

  // [4] 向左转动
  Serial.println(F("[4] 向左转动"));
  quadMove(90, 45, 125, 125, 95, 95, 60, 60, 30);
  delay(1000);

  // [5] 向下前伸
  Serial.println(F("[5] 向下前伸"));
  quadMove(45, 45, 125, 180, 95, 82, 60, 60, 30);
  delay(500);

  // [6] 打开夹子
  Serial.println(F("[6] 打开夹子"));
  quadMove(45, 45, 180, 180, 82, 82, 60, 112, 25);
  delay(2000);

  // [7] 全部归位
  Serial.println(F("[7] 全部归位"));
  quadMove(45, 90, 180, 90, 82, 90, 112, 90, 30);

  Serial.println(F("\n完成，停在原点。"));
  while (1);
}
