#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// 四个舵机通道
#define BASE    0   // 底座：左右旋转
#define SHOULDER 2   // 大臂：前后倾斜
#define ELBOW   4   // 小臂：弯曲伸展
#define GRIPPER 6   // 夹子：开合

// 脉冲范围（可根据实际舵机微调）
#define PMIN    150   // 0 度
#define PMAX    600   // 180 度
#define CENTER  375   // 90 度（原点）

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);
  delay(10);

  Serial.println(F("=================================="));
  Serial.println(F("  4-DOF 机器臂 全舵机测试"));
  Serial.println(F("=================================="));

  // 全部归位原点
  allToCenter();
  delay(1500);
}

void loop() {
  // ========================================
  //  第1步：依次单独测试每个舵机
  //  帮助你确认哪个是底座、大臂、小臂、夹子
  // ========================================

  // --- 底座 (通道0)：左右旋转 ---
  Serial.println(F("\n[底座] 左右旋转测试"));
  Serial.println(F("舵机转到 0 度..."));  pwm.setPWM(BASE, 0, PMIN);    delay(1000);
  Serial.println(F("舵机转回 90 度...")); pwm.setPWM(BASE, 0, CENTER);  delay(1000);
  Serial.println(F("舵机转到 180 度..."));pwm.setPWM(BASE, 0, PMAX);    delay(1000);
  Serial.println(F("舵机转回 90 度...")); pwm.setPWM(BASE, 0, CENTER);  delay(1500);

  // --- 大臂 (通道2)：前后倾斜 ---
  Serial.println(F("\n[大臂] 前后倾斜测试"));
  Serial.println(F("大臂转到 60 度...")); pwm.setPWM(SHOULDER, 0, pmap(60));  delay(1000);
  Serial.println(F("大臂转回 90 度...")); pwm.setPWM(SHOULDER, 0, CENTER);    delay(1000);
  Serial.println(F("大臂转到 120 度..."));pwm.setPWM(SHOULDER, 0, pmap(120)); delay(1000);
  Serial.println(F("大臂转回 90 度...")); pwm.setPWM(SHOULDER, 0, CENTER);    delay(1500);

  // --- 小臂 (通道4)：弯曲伸展 ---
  Serial.println(F("\n[小臂] 弯曲伸展测试"));
  Serial.println(F("小臂转到 60 度...")); pwm.setPWM(ELBOW, 0, pmap(60));  delay(1000);
  Serial.println(F("小臂转回 90 度...")); pwm.setPWM(ELBOW, 0, CENTER);    delay(1000);
  Serial.println(F("小臂转到 120 度..."));pwm.setPWM(ELBOW, 0, pmap(120)); delay(1000);
  Serial.println(F("小臂转回 90 度...")); pwm.setPWM(ELBOW, 0, CENTER);    delay(1500);

  // --- 夹子 (通道6)：开合 ---
  Serial.println(F("\n[夹子] 开合测试"));
  Serial.println(F("夹子打开 (60度)...")); pwm.setPWM(GRIPPER, 0, pmap(60));  delay(1000);
  Serial.println(F("夹子闭合 (120度)..."));pwm.setPWM(GRIPPER, 0, pmap(120)); delay(1000);
  Serial.println(F("夹子打开 (60度)...")); pwm.setPWM(GRIPPER, 0, pmap(60));  delay(1000);
  Serial.println(F("夹子回到 90 度..."));  pwm.setPWM(GRIPPER, 0, CENTER);    delay(1500);

  // ========================================
  //  第2步：联动演示（模拟抓取动作）
  // ========================================
  Serial.println(F("\n=================================="));
  Serial.println(F("  联动演示：模拟抓取动作"));
  Serial.println(F("=================================="));

  // 1. 张开夹子
  Serial.println(F("1. 张开夹子..."));
  pwm.setPWM(GRIPPER, 0, pmap(60));
  delay(500);

  // 2. 底座右转 + 大臂下放 + 小臂伸展（接近目标）
  Serial.println(F("2. 手臂伸向目标..."));
  smoothMove(BASE, CENTER, pmap(135), 10);      // 底座右转 45 度
  smoothMove(SHOULDER, CENTER, pmap(120), 10);   // 大臂前倾 30 度
  smoothMove(ELBOW, CENTER, pmap(60), 10);       // 小臂弯曲 30 度
  delay(500);

  // 3. 夹子闭合（抓起）
  Serial.println(F("3. 夹子闭合..."));
  pwm.setPWM(GRIPPER, 0, pmap(120));
  delay(1000);

  // 4. 抬起手臂
  Serial.println(F("4. 抬起手臂..."));
  smoothMove(SHOULDER, pmap(120), CENTER, 10);
  smoothMove(ELBOW, pmap(60), CENTER, 10);
  delay(500);

  // 5. 底座转回中间
  Serial.println(F("5. 底座转回中间..."));
  smoothMove(BASE, pmap(135), CENTER, 10);
  delay(500);

  // 6. 张开夹子（放下）
  Serial.println(F("6. 张开夹子..."));
  pwm.setPWM(GRIPPER, 0, pmap(60));
  delay(500);

  // 7. 全部归位
  Serial.println(F("7. 全部归位..."));
  allToCenter();

  Serial.println(F("\n=================================="));
  Serial.println(F("  一轮结束，5秒后重新开始"));
  Serial.println(F("=================================="));
  delay(5000);
}

// ---------- 工具函数 ----------

// 角度 → 脉冲值
int pmap(int angle) {
  angle = constrain(angle, 0, 180);
  return map(angle, 0, 180, PMIN, PMAX);
}

// 所有舵机归位 90 度
void allToCenter() {
  pwm.setPWM(BASE,     0, CENTER);
  pwm.setPWM(SHOULDER,  0, CENTER);
  pwm.setPWM(ELBOW,    0, CENTER);
  pwm.setPWM(GRIPPER,  0, CENTER);
  Serial.println(F("[全部归位 90 度]"));
}

// 平滑移动（阻塞式，适合测试用）
void smoothMove(uint8_t ch, int from, int to, int spd) {
  if (from < to) {
    for (int p = from; p <= to; p++) {
      pwm.setPWM(ch, 0, p);
      delay(spd);
    }
  } else {
    for (int p = from; p >= to; p--) {
      pwm.setPWM(ch, 0, p);
      delay(spd);
    }
  }
}
