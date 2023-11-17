//https://playground.arduino.cc/Main/MPU-6050/

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 imu;

const int flexPin1 = A0;
const int flexPin2 = A1;
float VCC = 5;
float flexResistor = 10000; // 10k Ohms

float flexMinResistance = 40000;
float flexMaxResistance = 300000;
float baseFlexVal = 16;
float flexInterval = 10;

int mapMultiplier = 10;

void setup() {
  Serial.begin(115200);
  pinMode(flexPin1, INPUT);
  pinMode(flexPin2, INPUT);

   if (!imu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
    Serial.println("MPU6050 Found!");
  
  // set accelerometer range to +-8G
  imu.setAccelerometerRange(MPU6050_RANGE_8_G);

  // set gyro range to +- 500 deg/s
  imu.setGyroRange(MPU6050_RANGE_500_DEG);

  // set filter bandwidth of digital lowpass filter to 21 Hz. For smoothing of signal & removal of high freq noise
  imu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  delay(10); 

}

int flexSensor(const int flexPin) {
  int ADCRaw = analogRead(flexPin);
  float ADCVoltage = (ADCRaw * VCC) / 1023; // get the voltage e.g (512 * 5) / 1023 = 2.5V

  float Resistance = flexResistor * (VCC / ADCVoltage - 1);
  int flexValue = map(Resistance, flexMinResistance, flexMaxResistance, 0, 1023); 
  flexValue = constrain(flexValue, 0, 1023); 
  return flexValue;
  //Serial.println(constrainVal);
}

int mapAcc(float accValue) {
  accValue = accValue * 10;
  int map_accValue = map(accValue, -78.4, 78.4, 0, 1023);
  map_accValue = constrain(accValue, 0, 1023); 

  return map_accValue;
}

int mapGyro(float gyroValue) {
  gyroValue = gyroValue * 100;
  int map_gyroValue = map(gyroValue, -500, 500, 0, 1023);
  map_gyroValue = constrain(gyroValue, 0, 1023);

  return map_gyroValue; 
}

void printIMU() {
  sensors_event_t a, g, temp;
  imu.getEvent(&a, &g, &temp);

  float acc_X = a.acceleration.x;
  float acc_Y = a.acceleration.y;
  float acc_Z = a.acceleration.z;

  float gyro_X = g.gyro.x;
  float gyro_Y = g.gyro.y;
  float gyro_Z = g.gyro.z;

  int map_acc_X = mapAcc(acc_X);
  int map_acc_Y = mapAcc(acc_Y);
  int map_acc_Z = mapAcc(acc_Z);

  int map_gyro_X = mapGyro(gyro_X);
  int map_gyro_Y = mapGyro(gyro_Y);
  int map_gyro_Z = mapGyro(gyro_Z);

  Serial.print("XAcc:");
  Serial.print(map_acc_X);
  Serial.print(",");
  Serial.print("YAcc:");
  Serial.print(map_acc_Y);
  Serial.print(",");
  Serial.print("ZAcc:");
  Serial.print(map_acc_Z);
  Serial.print(",");

  Serial.print("XGyro:");
  Serial.print(map_gyro_X);
  Serial.print(",");
  Serial.print("YGyro:");
  Serial.print(map_gyro_Y);
  Serial.print(",");
  Serial.print("ZGyro:");
  Serial.print(map_gyro_Z);
  Serial.println();

  delay(15);

}

void loop() {
  
  // Values are printed to show readings on Arduino Serial Plotter
  for(int i = 0; ; i++) {
  Serial.print(i);
  Serial.print(",");
  int flexVal1 = flexSensor(flexPin1);
  Serial.print("flexPin1:");
  Serial.println(flexVal1);

  int flexVal2 = flexSensor(flexPin2);
  Serial.print("flexPin2:");
  Serial.println(flexVal2);

  printIMU();
  

  // sampling rate of 20Hz
  delay(50);
  }
  
}
