// 1 S SPEED
// 0 1 2 3 4 5
// 
//
//
//
int E1 = 5;       //定义M1 使能端 
int E2 = 6;       //定义M2 使能端 
int M1 = 4;      //定义M1 控制端 
int M2 = 7;      //定义M2 控制端 

volatile static unsigned long leftState = 0;
volatile static unsigned long rightState = 0;
 int leftAngle;
 int rightAngle;

void leftBlink()
{
       if(leftState > 2147483647)
       {
          leftState = 0;
       }
       else
       {
          leftState++;
       }
}

void rightBlink()
{
       if(rightState > 2147483647)
      {
         rightState = 0;
      }
      else
      {
         rightState++;
      }
}
void setup()
{
          int i;
          for(i = 4;i<=7; ++i)
          {
             pinMode(i, OUTPUT);
          }
          Serial.begin(9600);
          attachInterrupt(0, rightBlink, RISING);
          attachInterrupt(1, leftBlink, RISING);
}

void loop(void)
{
         int motorSpeed = 0;
         int standby = Serial.read();
         int actionType = Serial.read();
         int speed0 = Serial.read();
         int speed1 = Serial.read();
         int speed2 = Serial.read();
         int speed3 = Serial.read();

        motorSpeed = (speed0 - '0') * 1000 + (speed1 - '0') * 100 + (speed2 - '0') * 10 + (speed3 - '0') ;
if('1'==standby){
        switch (actionType)
		{
		  case '3'://11  l1r0
                         leftState = 0;
                         analogWrite (E2,motorSpeed);           //PWM 调速
                         digitalWrite(M2,HIGH);
             		 rightState = 0;
                         analogWrite (E1,motorSpeed);           //PWM 调速
                         digitalWrite(M1,HIGH);
                         Serial.println (31); 
                         break; 
                 case '1'://01  l0r0
                         leftState = 0;
                         analogWrite (E2,motorSpeed); 
                         digitalWrite(M2,LOW); 
			 rightState = 0;
                         analogWrite (E1,motorSpeed);           //PWM 调速
                         digitalWrite(M1,HIGH);
                         Serial.println (11); 
                         break;
                 case '2'://10  l1r1
                         leftState = 0;
                         analogWrite (E2,motorSpeed);           //PWM 调速
                         digitalWrite(M2,HIGH);
                         rightState = 0;
                         analogWrite (E1,motorSpeed); 
                         digitalWrite(M1,LOW); 
                         Serial.println (21); 
                 case '0'://00   l0r1
                         leftState = 0;
                         analogWrite (E2,motorSpeed); 
                         digitalWrite(M2,LOW); 
                         rightState = 0;
                         analogWrite (E1,motorSpeed); 
                         digitalWrite(M1,LOW); 
                         Serial.println (01); 
                         break;
	         case '4'://停止
                         motorSpeed = 0;
                         analogWrite (E1,motorSpeed); 
                         analogWrite (E2,motorSpeed); 
                         digitalWrite(E2,LOW); 
			 digitalWrite(E1,LOW); 
                         Serial.println (41); 
                         break;
                 case '5'://转动角度 
                         //rightAngle = 360 * rightState / 2096;
                         Serial.println('L');
                         Serial.println (leftState);
			 Serial.println('R'); 
                         Serial.println (rightState);
                         break; 
                 default :
                         Serial.println (00);
		}
}
        delay(40);
 }

