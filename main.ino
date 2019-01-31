///////////////////////////
const int startup = 200;
const int warmIdle = 100;
const int hotIdle = 200;
const int inop = 0;
//////////////////////////

const int max = 10;

const int DIR_A1 = 5;
const int DIR_B1 = 4;
const int DIR_A2 = 7;
const int DIR_B2 = 8;
const int PWM1 = 6;
const int PWM2 = 3;
const int tempPin = 0;

void setup()
{
  pinMode(DIR_A1, OUTPUT);
  pinMode(DIR_B1, OUTPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(DIR_A2, OUTPUT);
  pinMode(DIR_B2, OUTPUT);
  pinMode(PWM2, OUTPUT);

  Serial.begin(9600);
}

int getTempC()
{
  int tempC = 0;
  int tempVal = 0;
  float volts = 0.0;

  tempVal = analogRead(tempPin);
  volts = tempVal * 5;
  volts /= 1023.0;
  tempC = (volts - 0.5) * 100;

  return tempC;
}

int checkTempC()
{
  int test[max];
  int average = 0;

  for(int i = 0; i < max; i ++)
  {
    test[i] = getTempC();
    average = average + test[i];
  }

  return average / max;
}

void PWMWrite(int value1, int value2)
{
  digitalWrite(DIR_A1, HIGH);
  digitalWrite(DIR_B1, LOW);
  digitalWrite(DIR_A2, LOW);
  digitalWrite(DIR_B2, HIGH);
  analogWrite(PWM1, value1);
  analogWrite(PWM2, value2);
}

void loop()
{
  int tempC = 0;
  int idleCurve = 0;

  unsigned long currentMillis = 0;
  unsigned long passingMillis = 0;
  unsigned long interval1 = 10000;
  unsigned long interval2 = 30000 + interval1;

  tempC = checkTempC();

  currentMillis = millis();

  if(tempC < 45)
  {
    ////////////////////////////////////
    ///////////PHASE 1//////////////////
    ////////////////////////////////////

    Serial.println("Cold Start");
    Serial.println(tempC);

    while(currentMillis < interval1)
    {
      PWMWrite(startup, inop);

      currentMillis = millis();
    }

    tempC = checkTempC();
  
    ////////////////////////////////////
    ///////////PHASE 2//////////////////
    ////////////////////////////////////

    Serial.println("30 sec High Airflow");
    Serial.println(tempC);

    currentMillis = millis();

    while (currentMillis < interval2)
    {
      tempC = checkTempC();

      idleCurve = map (currentMillis, interval1, interval2, startup, warmIdle);

      PWMWrite(idleCurve, inop);
      currentMillis = millis();
    }
  }

  ////////////////////////////////////
  ///////////PHASE 3//////////////////
  ////////////////////////////////////

  Serial.println("Warm Idle");
  Serial.println(tempC);

  while (tempC < 60)
  {
    tempC = checkTempC();

    PWMWrite(inop, warmIdle);
  }

  ////////////////////////////////////
  ///////////PHASE 4//////////////////
  ////////////////////////////////////

  Serial.println("Hot idle");
  Serial.println(tempC);
  if(tempC >= 60)
  {
    while (true)
    {
      PWMWrite(inop, hotIdle);
    }
  }
}
