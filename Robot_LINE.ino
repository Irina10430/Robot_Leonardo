#define ROTATION_LEFT 4 //направление вращения левого колеса
#define VOLTAGE_LEFT 5 //управляющее напряжение левого колеса
#define VOLTAGE_RIGHT 6 //управляющее напряжение правого колеса
#define ROTATION_RIGHT 7 //направление вращения правого колеса

float left_cnt0 = 0; float left_cnt; float dcnt_left; float right_cnt0 = 0; float right_cnt; float dcnt_right;
long int time; long int old_time;
long int time_left0 = 0; long int time_left; long int dt_left; long int time_right0 = 0; long int time_right; long int dt_right;
long int voltage_left = 150; long int voltage_right = 150; //управляющее напряжение
float KI = 0.25; //коэффициент интегрального регулятора
float KP = 700; //коэффициент пропорционального регулятора
float KD = 1; //коэффициент дифференциального регулятора
float speed = 0.06; //требуемая скорость (см/с)

void left_wheel(void)
{
  time_left = micros();
  dt_left = time_left - time_left0;
  time_left0 = time_left;

  if (dt_left > 100)
    left_cnt++;
}

void right_wheel(void)
{
  time_right = micros();
  dt_right = time_right - time_right0;
  time_right0 = time_right;

  if (dt_right > 100)
    right_cnt++;
}

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ROTATION_LEFT, OUTPUT);
  pinMode(VOLTAGE_LEFT, OUTPUT);
  pinMode(VOLTAGE_RIGHT, OUTPUT);
  pinMode(ROTATION_RIGHT, OUTPUT);
  
  digitalWrite(ROTATION_LEFT, HIGH);
  digitalWrite(VOLTAGE_LEFT, LOW);
  digitalWrite(VOLTAGE_RIGHT, LOW);
  digitalWrite(ROTATION_RIGHT, HIGH);
  attachInterrupt(digitalPinToInterrupt(2),left_wheel,CHANGE);
  attachInterrupt(digitalPinToInterrupt(3),right_wheel,CHANGE);
  Serial.begin(9600);
  old_time = millis();
}

int state = 0;

void loop() 
{
  time = micros();
  long dtime = time - old_time;

  analogWrite(VOLTAGE_LEFT, voltage_right);
  analogWrite(VOLTAGE_RIGHT, voltage_left);

  if(dtime>10000)
  {
    old_time = time;

      dcnt_left = left_cnt - left_cnt0;
      left_cnt0 = left_cnt;

      dcnt_right = right_cnt - right_cnt0;
      right_cnt0 = right_cnt;

      float S_left = 6*3.14*(dcnt_left/40); 
      float S_right = 6*3.14*(dcnt_right/40);

      float V_left = S_left/dtime*1000*0.99;
      float V_right = S_right/dtime*1000;
  
      float error_S = left_cnt - right_cnt;

      float error_V_left = V_left - speed;
      float error_V_right = V_right - speed;

      voltage_left -= (error_V_left*KP+error_S*KI);
      voltage_right -= (error_V_right*KP-error_S*KI);
      if (voltage_left > 255) 
        voltage_left = 255;
      if (voltage_right > 255) 
        voltage_right = 255;
      if (voltage_left < 0) 
        voltage_left = 0;
      if (voltage_right < 0) 
        voltage_right = 0;
      
      Serial.print("\n");

      Serial.print("left_cnt: ");
      Serial.print(left_cnt);
      Serial.print(", right_cnt: ");
      Serial.println(right_cnt);

      Serial.print("error_V_left: ");
      Serial.print(error_V_left*100);
      Serial.print(", error_V_right: ");
      Serial.println(error_V_right*100);

      Serial.print("error_S: ");
      Serial.println(error_S);

      Serial.print("voltage_left: ");
      Serial.print(voltage_left);
      Serial.print(", voltage_right: ");
      Serial.println(voltage_right);

      Serial.print("V_left: ");
      Serial.print(V_left*100);
      Serial.print(", V_right: ");
      Serial.println(V_right*100);
    
      //Serial.print("\n");
      //Serial.println(dtime);
  }
}
