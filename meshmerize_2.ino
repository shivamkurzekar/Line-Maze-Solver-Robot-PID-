// Shivam Kurzekar
int value = 0;
String binary = "00000000";
const int l1 = 4, l2 = 5, pl = 6, r1 = 8, r2 = 7, pr = 9, Junction_Pulse = 3, UART_EN = 2;
float x, y, error_pwm = 0, error_sum = 0, previous_error = 0;
const int left_sensor = 10, right_sensor = 11, lsa_2 = A5, lsa_4 = A4, lsa_5 = A3, lsa_7 = A2;
int Junction = 0;
float p_error = 0, i_error = 0, d_error = 0, total_error = 0;
float average_pwm = 80, left_pwm = 0, right_pwm = 0, rotational_pwm = 100;
boolean lsv = 0, rsv = 0, lsa_2v = 0, lsa_4v = 0, lsa_5v = 0, lsa_7v = 0;        //left_sensor_value(lsv)

float kp = 1.4;
float ki = 0.008;
float kd = 20;
int i = 0, array_size = 20;
int sum[20];


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(l1, OUTPUT);
  pinMode(l2, OUTPUT);
  pinMode(pl, OUTPUT);
  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(pr, OUTPUT);
  pinMode(Junction_Pulse, INPUT);
  pinMode(UART_EN, OUTPUT);
  pinMode(left_sensor, INPUT);
  pinMode(right_sensor, INPUT);
  pinMode(lsa_2, INPUT);
  pinMode(lsa_4, INPUT);
  pinMode(lsa_5, INPUT);
  pinMode(lsa_7, INPUT);
  for (int i = 0; i < array_size; i++)
  {
    sum[i] = 0;
  }
  digitalWrite(UART_EN, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:
  go_straight();

  if (digitalRead(Junction_Pulse) == HIGH || analogRead(A0) > 700)
  {
    Stop();
    delay(50);
    read_sensor_values();
    Junction = detect_Junction();
    if (Junction == 1 || Junction == 3 || Junction == 4)
    {
      Slight_left();
    }

    else if (Junction == 6 || Junction == 5)
    {
      Slight_left();
    }
    else if (analogRead(A0) > 690)
    {
      Sharp_right();
    }
  }
}



void Stop()
{
  Serial.println("STOP");
  digitalWrite(l1, LOW);
  digitalWrite(l2, LOW);
  digitalWrite(r1, LOW);
  digitalWrite(r2, LOW);
  delay(300);
}

int detect_Junction()
{
  Serial.println("identifying Junction");
  int i = 0;
  if (lsa_2v && lsa_4v && lsa_5v && lsa_7v)      //T junction
  {
    i = 1;
  }
  else if (!lsa_2v  && lsa_5v && lsa_7v && !lsv)        // Right
  {
    i = 2;
  }
  else if (lsa_2v && lsa_4v  && !lsa_7v && !rsv)        // Left
  {
    i = 3;
  }
  else if (lsa_2v && lsa_4v  && !lsa_7v && rsv )          // left and 135
  {
    i = 4;
  }
  else if (!lsa_2v  && lsa_5v && lsa_7v && lsv)         //right and 135
  {
    i = 5;
  }
  else if ((lsv && rsv) || ((digitalRead(Junction_Pulse) == HIGH) && (digitalRead(lsa_2) == HIGH || digitalRead(lsa_7) == HIGH))) //Y JUNCTION
  {
    i = 6;
  }
  Serial.println("Junction is :-  ");
  Serial.println(i);
  return i;
}

int Read_LSA()
{
  Serial.print("Reading LSA    ");
  x = 0;
  for (int k = 0; k <= 5; k++)
  {
    x = analogRead(A0) + x;
  }
  x = x / 6;
  y = map(x, 20, 670, 0, 70);
  Serial.println(y - 35);
  return (y - 35);
}
void read_sensor_values()
{
  Serial.println("Reading Sensors");
  lsa_2v = digitalRead(lsa_2);             //LSA
  lsa_4v = digitalRead(lsa_4);
  lsa_5v = digitalRead(lsa_5);
  lsa_7v = digitalRead(lsa_7);
  lsv = digitalRead(left_sensor);          //read IR
  rsv = digitalRead(right_sensor);
}


void go_straight()
{
  Serial.println("Going Straight");
  error_pwm = Read_LSA();
  //Serial.println(error_pwm);
  sum[i] = error_pwm;
  error_sum = 0;
  for (int n = 0; n < array_size; n++)
  {
    error_sum = sum[n] + error_sum;
  }

  p_error = kp * error_pwm;
  i_error = ki * error_sum;
  d_error = kd * (error_pwm - previous_error);
  total_error = p_error + i_error + d_error;
  previous_error = error_pwm;
  left_pwm = (average_pwm + total_error);
  right_pwm = (average_pwm - total_error);
  left_pwm = constrain(left_pwm, 0, 255);
  right_pwm = constrain(right_pwm, 0, 255);
  analogWrite(pl, left_pwm);
  analogWrite(pr, right_pwm);

  digitalWrite(l1, HIGH);
  digitalWrite(l2, LOW);
  digitalWrite(r1, HIGH);
  digitalWrite(r2, LOW);
  i++;
  if (i > array_size)
  {
    i = 0;
  }
}

void Slight_left()
{
  Serial.println("Taking Slight LEFT");
  int x = Read_LSA();
  analogWrite(pl, rotational_pwm);
    analogWrite(pr, rotational_pwm);
    digitalWrite(l1, LOW);
    digitalWrite(l2, LOW);
    digitalWrite(r1, HIGH);
    digitalWrite(r2, LOW);
    delay(500);
  while (1)
  {
    analogWrite(pl, rotational_pwm);
    analogWrite(pr, rotational_pwm);
    digitalWrite(l1, LOW);
    digitalWrite(l2, LOW);
    digitalWrite(r1, HIGH);
    digitalWrite(r2, LOW);
    x = Read_LSA();
    if ((digitalRead(Junction_Pulse) == LOW) && (x > -10) && (x < 25))
    {
      break;
    }
  }
}

void Sharp_right()
{
  Serial.println("Taking Sharp RIGHT");
  analogWrite(pl, rotational_pwm);
  analogWrite(pr, rotational_pwm);
  digitalWrite(l1, HIGH);
  digitalWrite(l2, LOW);
  digitalWrite(r1, LOW);
  digitalWrite(r2, HIGH);
  delay(500);
  int x = Read_LSA();
  while (1)
  {
    analogWrite(pl, rotational_pwm);
    analogWrite(pr, rotational_pwm);
    digitalWrite(l1, HIGH);
    digitalWrite(l2, LOW);
    digitalWrite(r1, LOW);
    digitalWrite(r2, HIGH);
    x = Read_LSA();
    if ((digitalRead(Junction_Pulse) == LOW) && (x > -10) && (x < 10))
    {
      break;
    }
  }
}
