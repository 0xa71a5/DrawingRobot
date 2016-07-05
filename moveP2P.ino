#include <FrequencyTimer2.h>
#include <TimerOne.h>
#define DEBUG
#define waitMotor1()    while(ok1==false)delay(1);
#define waitMotor2()    while(ok2==false)delay(1);
#define waitMotor12()    while(ok1==false||ok2==false)delay(1);

#define dirMotor1(x)       digitalWrite(dirPin1,x);
#define dirMotor2(x)       digitalWrite(dirPin2,!x);

#define dirPin1 3
#define stepperPin1 4
#define sleepPin1 2

#define up  0
#define down 1

#define dirPin2 6
#define stepperPin2 7
#define sleepPin2 5
double a=13.5;
bool ok1=true,ok2=true;
bool en1=false,en2=false;
//volatile long count1 = 0;
//volatile long count2 = 0;
long int count1=0;
long int count2=0;
int useless=0;
double L1=0;
double L2=0;
double deltL1=0;
double deltL2=0;
double n=116,m=106;
double x1,y1,x2,y2;
int signL1=1;
int signL2=1;
bool dirL1=up;
bool dirL2=up;
unsigned int periodL1=1000;
unsigned int periodL2=2000;
#ifdef DEBUG
#define debugPrint(x) Serial.print(x)
#define debugPrintln(x) Serial.println(x)
#else
#define debugPrint(x) Serial2.print(x)
#define debugPrintln(x) Serial2.println(x)
#endif

void calc(double _x1,double _y1,double _x2,double _y2)
{
  _x1=_x1+a;
  _x2=_x2+a;
  
 debugPrint("upY1:");
 debugPrint(n-_y1);
 debugPrint(" upY2:");
 debugPrintln(n-_y2);
 
 double L1_new;
 L1_new=pow(double(_x2*_x2+(n-_y2)*(n-_y2)),0.5);
 L1= pow(double(_x1*_x1+(n-_y1)*(n-_y1)),0.5);
 double L2_new;
 L2_new=pow(double((m-_x2)*(m-_x2)+(n-_y2)*(n-_y2)),0.5);
 L2=pow(double((m-_x1)*(m-_x1)+(n-_y1)*(n-_y1)),0.5);
 
debugPrint("L1:");
debugPrint(L1);
debugPrint(", L2:");
debugPrint(L2);
debugPrint(" , L1_new:");
debugPrint(L1_new);
debugPrint(" , L2_new:");
debugPrintln(L2_new);

  deltL1=L1_new-L1;
  deltL2=L2_new-L2;
}

//16us/execute one time
void step1()
{
 if(count1>0)
 {
  digitalWrite(stepperPin1,digitalRead(stepperPin1)^1);
  L1+=signL1*0.0305;//mm
  count1--;
 }
 else
 {
  ok1=true;
  digitalWrite(sleepPin1,0);
 }
}
//16us/execute one time
void step2()
{
 if(count2>0)
 {
  digitalWrite(stepperPin2,digitalRead(stepperPin2)^1); 
  L2+=signL2*0.0305;//mm
  count2--;
 }
 else
 {
  ok2=true;
  digitalWrite(sleepPin2,0);
 }
}
void setMotor1(int speedNum)
{
  Timer1.initialize(speedNum);
}
void setMotor1(double steps,int speedNum)
{
  if(steps<0)
  {
    signL1=-1;
    dirMotor1(up);
    steps=-steps;
  }
  else
  {
    signL1=1;
    dirMotor1(down);
  }
  en1=true;
  count1=(long int)steps;
  digitalWrite(sleepPin1,1);
  Timer1.initialize(speedNum);
  ok1=false;
}
void setMotor2(int speedNum)
{
   FrequencyTimer2::setPeriod(speedNum*2);
   FrequencyTimer2::enable();
}
void setMotor2(double steps,int speedNum)
{
  if(steps<0)
  {
    signL2=-1;
    dirMotor2(up);
    steps=-steps;
  }
   else
  {
    signL2=1;
    dirMotor2(down);
  }
  en2=true;
  count2=(long int)steps;
  digitalWrite(sleepPin2,1);
  FrequencyTimer2::setPeriod(speedNum*2);
  FrequencyTimer2::enable();
  ok2=false;
}

void waitForComplete()
{
  while(ok1==false||ok2==false)
  {
    if(ok1)
    digitalWrite(sleepPin1,0);
    if(ok2)
    digitalWrite(sleepPin2,0);

    #ifdef DEBUG
    Serial.println(L1);
    delay(10);
    #endif
  }
}
void setL12(double x,double y)
{
  L1=pow(x*x+(n-y)*(n-y),0.5);
  L2=pow((m-x)*(m-x)+(n-y)*(n-y),0.5);
}
void getCurrentX()
{
  
}
void getCurrentY()
{
  
}

void moveP2P(double x1_,double y1_,double x2_,double y2_)
{
 calc(x1_,y1_,x2_,y2_);

debugPrint("deltL1:");
debugPrint(deltL1);
debugPrint(", deltL2:");
debugPrintln(deltL2);

double step1ToRun=deltL1*32.8;
double step2ToRun=deltL2*32.8;

debugPrint("stepL1:");
debugPrint(step1ToRun);
debugPrint(" , stepL2:");
debugPrint(step2ToRun);

//确定了两个电机转向还有转步之后确定转速
//尽量让两者转动的总时间相同
double k=abs(step1ToRun/step2ToRun);//两者要走的路程的比值

debugPrint(" , K:");
debugPrintln(k);

//用来计算两者的电机转速（周期反比）
//转速周期范围800us - 2300us
  if(k>1)//k>1时   800<p1<(2300/k)
  {
    if(k>2.875)//超量程比值
    {
      periodL1=800;
      periodL2=2300;
    }
    else//k在正常量程之下
    {
      periodL1=0.5*(800+2300/k);
      periodL2=k*periodL1;
    }
  }
  else//k<1时    (800/k)<p1<2300
  {
    if(k<0.34)//超量程比例
    {
      periodL1=2300;
      periodL2=800;
    }
    else//k在正常量程之下
    {
      periodL1=0.5*(800/k+2300);
      periodL2=k*periodL1;
    }  
  }

  debugPrint("PeriodL1:");
  debugPrint(periodL1);
  debugPrint(" , PeriodL2:");
  debugPrintln(periodL2);
  
  setMotor1(step1ToRun,periodL1);
  setMotor2(step2ToRun,periodL2);
  waitMotor12();
  debugPrintln("Done!");
}
void setup() {
 Serial.begin(115200);
 
 pinMode(dirPin1, OUTPUT);
 pinMode(stepperPin1, OUTPUT);
 pinMode(sleepPin1,OUTPUT);
 
 pinMode(dirPin2, OUTPUT);
 pinMode(stepperPin2, OUTPUT);
 pinMode(sleepPin2,OUTPUT);

 pinMode(13,OUTPUT);
 digitalWrite(sleepPin1,0);//1 means run
 digitalWrite(sleepPin2,0);
 
 Timer1.initialize(1000);
 Timer1.attachInterrupt(step1);
 FrequencyTimer2::setPeriod(2000);
 FrequencyTimer2::setOnOverflow(step2);
 FrequencyTimer2::enable();
 delay(1000);
 digitalWrite(13,1);
}

void loop() {
  //(15,75)  (60,55);
 while(Serial.available()==0);
x1=60;y1=55;
x2=10;y2=35;
moveP2P(10,35,20,75);
delay(500);
moveP2P(20,75,40,35);
delay(500);
moveP2P(40,35,60,75);
delay(500);
moveP2P(60,75,70,35);
delay(3000);
moveP2P(70,35,10,35);
while(1);
}

