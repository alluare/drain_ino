#include <DFRobot_TFmini.h>
#include <SoftwaeSerial.h>
#define DEEP 15


for TFmini plus LiDar 
SoftwareSerial mySerial(8, 7); // RX, TX (LiDar)


DFRobot_TFmini  TFmini;
uint16_t distance_center, strength;


int trig_1 = 2 , echo_1 =3; //ultrasonic_1 pin 
int trig_2 = 10, echo_2 = 11; //ultrasonic_2 pin 

long distance_1, distance_2 = 0;
int count, print_count, distance_tot, distance_avg = 0; //ultrasonic mean value


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  TFmini.begin(mySerial);

  pinMode(trig_1, OUTPUT);
  pinMode(echo_1, INPUT);
  pinMode(trig_2, OUTPUT);
  pinMode(echo_2, INPUT);
}


long distance(int trig, int echo){ // 초음파센서를 통해 거리를 측정하는 함수
  digitalWrite(trig, HIGH);
  delay(1);
  digitalWrite(trig, LOW);

  return (pulseIn(echo, HIGH) * 0.034 / 2); // return distance
}


double error(long measured, long real){ // 두 정수의 오차를 계산하는 함수
  if (real == 0){
    return 1;
  }

  else{
    return abs((measured - real) / real);
  }
}


void loop() {
  distance_1 = distance(trig_1, echo_1);
  distance_2 = distance(trig_2, echo_2);
  delay(1);

  if (TFmini.measure()){
    distance_center = TFmini.getDistance();       //Get distance data
  }

  
  if (count != 20){ // count==20일 때까지 count++ 하면서 거리 저장하기
    double error_1 = error(distance_1, distance_center);
    double error_2 = error(distance_2, distance_center);

    if ( (error_1 < 0.5) && (error_2 < 0.5) ){ // 둘다 오차가 작을 때
      count++;
      distance_tot += (distance_center * 0.6) + (distance_1 * 0.2) + (distance_2 * 0.2);
    }
    
    else if ( (error_1 >= 0.5) && (error_2 < 0.5) ){ // 센서 1의 오차가 클 때
      count++;
      distance_tot += (distance_center * 0.6) + (distance_2 * 0.4);       
    }

    else if ( (error_1 < 0.5) && (error_2 >= 0.5) ){ // 센서 2의 오차가 클 때
      count++;
      distance_tot += (distance_center * 0.6) + (distance_1 * 0.4);
    }

    // else: 초음파센서 둘다 라이다센서랑 오차 클때 -> 라이다센서에 문제있다고 생각

  }
    
  else { // count == 20
    distance_tot /= 20; // 평균값 내기
    count = 0; // count 초기화
    int print_count = 0;

    if (distance_tot > DEEP){
      delay(10000); // 우선 10초동안 거리 측정 쉬기
      
      long d;

      while(1){ // 얕은 길이가 여러번 측정되면 무한반복문 탈출
        distance_1 = distance(trig_1, echo_1);
        distance_2 = distance(trig_2, echo_2);
        delay(1);
        if (TFmini.measure()){
          distance_center = TFmini.getDistance();       //Get distance data
        }

        d = (distance_1 * 0.2) + (distance_2 * 0.2) + (distance_center * 0.6);

        if (d < DEEP){
          print_count++;
        }

        if (print_count > 10){
          break;
        }
      }
    }

    else{
      Serial.print(count);
      Serial.print("      ");


      Serial.print(distance_center);

      Serial.print("      ");
      Serial.print(distance_1);

      Serial.print("      ");
      Serial.print(distance_2);
      Serial.print("      ");

      Serial.println(distance_tot);
    }
  }

  delay(100);  
}