#include <ros.h>
#include <geometry_msgs/Twist.h>

ros::NodeHandle nh;
void car_cmd (const geometry_msgs::Twist& toggle_mag);
ros::Subscriber<geometry_msgs::Twist> sub_car("car_cmd", &car_cmd);

#include "DualMC33926MotorShield.h"
DualMC33926MotorShield md;

#include <SR04.h>
#define TRIG_PIN1 30
#define ECHO_PIN1 31
#define TRIG_PIN2 32
#define ECHO_PIN2 33
SR04 sensorRight = SR04(ECHO_PIN1, TRIG_PIN1);
SR04 sensorLeft  = SR04(ECHO_PIN2, TRIG_PIN2);

#define stopDistance 20
#define loopDelay    80

void setup() {
  Serial.begin(115200);
  md.init();
  nh.initNode();
  nh.subscribe(sub_car);
}

boolean stop_cmd = false;
boolean isRunningPic = false;

unsigned long start_time = 0;
unsigned long duration = 0;
int nextM1Speed = 0;
int nextM2Speed = 0;
unsigned long nextduration = 0;

void loop() {
  nh.spinOnce();
  readConsole();
  stopIfFault();
  delay(loopDelay);
}

boolean stopIfFault() {
  if (isFault() == true) {
    stop_cmd = false;
    duration = 0;
    nextduration = 0;
    isRunningPic = false;
    run_car(0, 0, 0);
  }
}

boolean isFault() {
  if (md.getFault()) {
    Serial.println("fault");
    return true;
  }
  if (stop_cmd == true) {
    Serial.println("Stop by command");
    return true;
  }
  if (duration != 0 && millis() - start_time >= duration) {
    if (nextduration != 0) {
      Serial.println("Finish 1st run, running 2st");
      run_car(nextM1Speed, nextM2Speed, nextduration);
      nextduration = 0;
    } else {
      Serial.println("Finish run");
      return true;
    }
  }
  long distanceRight = sensorRight.Distance();
  long distanceLeft  = sensorLeft.Distance();
  if (distanceRight > 0 && distanceRight <= stopDistance &&
      distanceLeft > 0 && distanceLeft <= stopDistance) {
    Serial.println("stop by obstacle");
    return true;
  }
  return false;
}

void car_cmd (const geometry_msgs::Twist& toggle_mag) {
  int cmd_type = toggle_mag.linear.x;
  int cmd_data = toggle_mag.linear.y;
  if (cmd_type == 1) { // pic cmd
    if (isRunningPic == true) return;
    isRunningPic = true;
    switch (cmd_data) {
      case 1: //forward cat
        run_car(120, -120, 2000);
        break;
      case 2: //back bicycle
        run_car(-120, 120, 2000);
        break;
      case 3: //right sofa
        run_car(-120, -120, 7000);
        break;
      case 4: //left dog
        run_car(120, 120, 7000);
        break;
      case 5: //right forward train
        run_car2(-120, -120, 1600, 120, -120, 2000);
        break;
      case 6: //left forward car
        run_car2(120, 120, 1600, 120, -120, 2000);
        break;
    }
  } else { // car cmd
    isRunningPic = false;
    switch (cmd_data) {
      case 1: //forward
        run_car(120, -120, 2000);
        break;
      case 2: //left
        run_car(120, 120, 1600);
        break;
      case 3: //right
        run_car(-120, -120, 1600);
        break;
      case 4: //backword
        run_car(-120, 120, 2000);
        break;
      case 5: //stop
        stop_cmd = true;
        break;
    }
  }
}

void run_car(int M1Speed, int M2Speed, int _duration) {
  md.setSpeeds(M1Speed, M2Speed);
  start_time = millis();
  duration = _duration;
  nextduration = 0;
}

void run_car2(int M1Speed, int M2Speed, int _duration, int _nextM1Speed, int _nextM2Speed, int _nextduration) {
  run_car(M1Speed, M2Speed, _duration);
  nextM1Speed = _nextM1Speed;
  nextM2Speed = _nextM2Speed;
  nextduration = _nextduration;
}

int msg = 1;
void readConsole() {
  String comdata = "";
  while (Serial.available() > 0) {
    comdata += char(Serial.read());
    delay(10);
  }
  if (comdata.length() > 0) {
    Serial.println("console");
    switch (msg++) {
      case 1:
        run_car2(-120, -120, 1400, 120, -120, 2000);
        break;
      case 2:
        run_car(120, -120, 5000);
        break;
      case 3:
        stop_cmd = true;
        break;
      case 4:
        run_car2(-120, -120, 1400, 120, -120, 2000);
        break;
      case 5:
        run_car2(-120, -120, 1400, 120, -120, 2000);
        break;
    }
  }
}
