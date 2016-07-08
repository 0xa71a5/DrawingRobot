/*
 * 32.8steps=1cm
 * 1step=0.0305cm
 */
#include <FrequencyTimer2.h>
#include <TimerOne.h>
#define DEBUG
//#define UNLOCK
#define waitMotor1()    while(ok1==false)delay(1);
#define waitMotor2()    while(ok2==false)delay(1);
#define waitMotor12()    while(ok1==false||ok2==false)delay(1);

#define dirMotor1(v)       digitalWrite(dirPin1,(v));
#define dirMotor2(v)       digitalWrite(dirPin2,!(v));

#define dirPin1 3
#define stepperPin1 4
#define sleepPin1 2

#define up  0
#define down 1

#define MAXPERIOD 1800
#define MINPERIOD 600

//#define MAXPERIOD 3000
//#define MINPERIOD 2000

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
double currentX,currentY;

const double circleX[37]={60.0000,59.6962,58.7939,57.3205,55.3209,52.8558,
50.0000,46.8404,43.4730,40.0000,36.5270,33.1596,
30.0000,27.1442,24.6791 ,22.6795,21.2061,20.3038,
20.0000,20.3038,21.2061,22.6795,24.6791,27.1442,
30.0000,33.1596,36.5270,40.0000,43.4730,46.8404,
50.0000,52.8558,55.3209,57.3205,58.7939,59.6962,60.0000};

const double circleY[37]={50.0000,53.4730,56.8404,60.0000,62.8558,65.3209,67.3205,
68.7939,69.6962,70.0000,69.6962,68.7939,67.3205,65.3209,
62.8558,60.0000,56.8404,53.4730,50.0000,46.5270,43.1596,
40.0000,37.1442,34.6791,32.6795,31.2061,30.3038,30.0000,
30.3038,31.2061,32.6795,34.6791,37.1442,40.0000,43.1596,46.5270,50.0000};

const double circleX2[73]={60.0000,59.9239,59.6962,59.3185,58.7939,58.1262,57.3205,56.3830,55.3209,54.1421,52.8558,51.4715,50.0000,48.4524,46.8404,45.1764,43.4730,41.7431,40.0000,38.2569,36.5270,34.8236,33.1596,31.5476,30.0000,28.5285,27.1442,25.8579,24.6791,23.6170,22.6795,21.8738,21.2061,20.6815,20.3038,20.0761,20.0000,20.0761,20.3038,20.6815,21.2061,21.8738,22.6795,23.6170,24.6791,25.8579,27.1442,28.5285,30.0000,31.5476,33.1596,34.8236,36.5270,38.2569,40.0000,41.7431,43.4730,45.1764,46.8404,48.4524,50.0000,51.4715,52.8558,54.1421,55.3209,56.3830,57.3205,58.1262,58.7939,59.3185,59.6962,59.9239,60.0000};
const double circleY2[73]={50.0000,51.7431,53.4730,55.1764,56.8404,58.4524,60.0000,61.4715,62.8558,64.1421,65.3209,66.3830,67.3205,68.1262,68.7939,69.3185,69.6962,69.9239,70.0000,69.9239,69.6962,69.3185,68.7939,68.1262,67.3205,66.3830,65.3209,64.1421,62.8558,61.4715,60.0000,58.4524,56.8404,55.1764,53.4730,51.7431,50.0000,48.2569,46.5270,44.8236,43.1596,41.5476,40.0000,38.5285,37.1442,35.8579,34.6791,33.6170,32.6795,31.8738,31.2061,30.6815,30.3038,30.0761,30.0000,30.0761,30.3038,30.6815,31.2061,31.8738,32.6795,33.6170,34.6791,35.8579,37.1442,38.5285,40.0000,41.5476,43.1596,44.8236,46.5270,48.2569,50.0000};

const double circleX3[121]={60.0000,59.9726,59.8904,59.7538,59.5630,59.3185,59.0211,58.6716,58.2709,57.8201,57.3205,56.7734,56.1803,55.5429,54.8629,54.1421,53.3826,52.5864,51.7557,50.8928,50.0000,49.0798,48.1347,47.1674,46.1803,45.1764,44.1582,43.1287,42.0906,41.0467,40.0000,38.9533,37.9094,36.8713,35.8418,34.8236,33.8197,32.8326,31.8653,30.9202,30.0000,29.1072,28.2443,27.4136,26.6174,25.8579,25.1371,24.4571,23.8197,23.2266,22.6795,22.1799,21.7291,21.3284,20.9789,20.6815,20.4370,20.2462,20.1096,20.0274,20.0000,20.0274,20.1096,20.2462,20.4370,20.6815,20.9789,21.3284,21.7291,22.1799,22.6795,23.2266,23.8197,24.4571,25.1371,25.8579,26.6174,27.4136,28.2443,29.1072,30.0000,30.9202,31.8653,32.8326,33.8197,34.8236,35.8418,36.8713,37.9094,38.9533,40.0000,41.0467,42.0906,43.1287,44.1582,45.1764,46.1803,47.1674,48.1347,49.0798,50.0000,50.8928,51.7557,52.5864,53.3826,54.1421,54.8629,55.5429,56.1803,56.7734,57.3205,57.8201,58.2709,58.6716,59.0211,59.3185,59.5630,59.7538,59.8904,59.9726,60};
const double circleY3[121]={50.0000,51.0467,52.0906,53.1287,54.1582,55.1764,56.1803,57.1674,58.1347,59.0798,60.0000,60.8928,61.7557,62.5864,63.3826,64.1421,64.8629,65.5429,66.1803,66.7734,67.3205,67.8201,68.2709,68.6716,69.0211,69.3185,69.5630,69.7538,69.8904,69.9726,70.0000,69.9726,69.8904,69.7538,69.5630,69.3185,69.0211,68.6716,68.2709,67.8201,67.3205,66.7734,66.1803,65.5429,64.8629,64.1421,63.3826,62.5864,61.7557,60.8928,60.0000,59.0798,58.1347,57.1674,56.1803,55.1764,54.1582,53.1287,52.0906,51.0467,50.0000,48.9533,47.9094,46.8713,45.8418,44.8236,43.8197,42.8326,41.8653,40.9202,40.0000,39.1072,38.2443,37.4136,36.6174,35.8579,35.1371,34.4571,33.8197,33.2266,32.6795,32.1799,31.7291,31.3284,30.9789,30.6815,30.4370,30.2462,30.1096,30.0274,30.0000,30.0274,30.1096,30.2462,30.4370,30.6815,30.9789,31.3284,31.7291,32.1799,32.6795,33.2266,33.8197,34.4571,35.1371,35.8579,36.6174,37.4136,38.2443,39.1072,40.0000,40.9202,41.8653,42.8326,43.8197,44.8236,45.8418,46.8713,47.9094,48.9533,50};

//const int splineX[41]={10,12,12,13,13,14,14,22,23,24,25,25,26,26,27,27,28,28,28,28,29,29,30,30,30,31,31,33,33,34,34,35,35,36,36,37,37,38,38,39,39};
//const int splineY[41]={39,40,41,41,42,42,43,42,42,42,42,43,44,44,44,45,46,47,48,49,49,49,54,55,56,57,58,60,61,63,64,64,64,66,66,67,68,68,69,69,70};

#ifdef DEBUG
#define debugPrint(v) Serial.print((v))
#define debugPrintln(v) Serial.println((v))
#else
#define debugPrint((v)) 
#define debugPrintln((v))
#endif

#define uu(x)

void calc(double _x1,double _y1,double _x2,double _y2)
{
  _x1=_x1+a;
  _x2=_x2+a;
  
 debugPrint("upY1:");
 debugPrint(n-_y1);
 debugPrint(" upY2:");
 debugPrintln(n-_y2);
 
 double L1_new;
 double L1_old;
 
 L1_new=sqrt(double(_x2*_x2+(n-_y2)*(n-_y2)));
 L1_old= sqrt(double(_x1*_x1+(n-_y1)*(n-_y1)));
 double L2_new;
 double L2_old;
 
 L2_new=sqrt(double((m-_x2)*(m-_x2)+(n-_y2)*(n-_y2)));
 L2_old=sqrt(double((m-_x1)*(m-_x1)+(n-_y1)*(n-_y1)));
 
debugPrint("L1_old:");
debugPrint(L1_old);
debugPrint(", L2_old:");
debugPrint(L2_old);
debugPrint(" , L1_new:");
debugPrint(L1_new);
debugPrint(" , L2_new:");
debugPrintln(L2_new);

  deltL1=L1_new-L1_old;
  deltL2=L2_new-L2_old;
}

//16us/execute one time
void step1()
{
 if(count1>0)
 {
  digitalWrite(stepperPin1,digitalRead(stepperPin1)^1);
 // L1+=signL1*0.0305;//mm
  count1--;
 }
 else
 {
  ok1=true;
  #ifdef UNLOCK
  digitalWrite(sleepPin1,0);
  #endif
 }
}
//16us/execute one time
void step2()
{
 if(count2>0)
 {
  digitalWrite(stepperPin2,digitalRead(stepperPin2)^1); 
 // L2+=signL2*0.0305;//mm
  count2--;
 }
 else
 {
  ok2=true;
   #ifdef UNLOCK
  digitalWrite(sleepPin2,0);
  #endif
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
  L1=sqrt(x*x+(n-y)*(n-y));
  L2=sqrt((m-x)*(m-x)+(n-y)*(n-y));
}
void calcXY(double L1_,double L2_)
{
  currentX=(L1_*L1_-L2_*L2_+m*m)/(2*m);
  currentY=n-sqrt(L1_*L1_-currentX*currentX);
}
void calcXY()
{
  currentX=(L1*L1-L2*L2+m*m)/(2*m);
  currentY=n-sqrt(L1*L1-currentX*currentX);
}
void setXY(double x_,double y_)
{
  currentX=x_;
  currentY=y_;
  Serial.print("setXY");
  Serial.print(currentX);
  Serial.print(",");
  Serial.println(currentY);
}

void drawSpline(String &str)
{
  uint8_t *coordX;
  uint8_t *coordY;
  int index=0;
  int num=0;
  if(str[0]!='<'||str[1]!='L')
  return;
  index=2;
  while(str[index]!='>')
  {
    num*=10;
    num+=str[index]-'0';
    index++;
  }
  if(num<2)
  return;
  int lengths=num;
  Serial.print("Length:");
  Serial.println(lengths);
  coordX=new uint8_t [lengths];
  coordY=new uint8_t [lengths];
  index++;
  int coordCount=0;
  while(index<str.length())
  {
    if(str[index]=='<')
    {
      index++;
      char flag=str[index];
      index++;
      num=0;
      while(str[index]!='>')
      {
        num*=10;
        num+=str[index]-'0';
        index++;
      }
     
      if(flag=='X')
      {
        if(num<0)num=0;
        if(num>80)num=80;
        coordX[coordCount]=num;
      }
      else if(flag=='Y')
      {
        if(num<0)num=0;
        if(num>100)num=100;
        coordY[coordCount]=num;
        coordCount++;
      }
      index++;
    }
  }
  
  for(int i=0;i<lengths;i++)
  {
    Serial.print("coord");
    Serial.print(i);
    Serial.print(":");
    Serial.print(coordX[i]);
    Serial.print(",");
    Serial.println(coordY[i]);
  }
  moveToPoint(coordX[0],coordY[0]);
  delay(1000);
   for(int i=1;i<lengths;i++)
  {
    moveToPoint(coordX[i],coordY[i]);
    delay(500);
  }
}


void moveP2P(double x1_,double y1_,double x2_,double y2_)
{
 calc(x1_,y1_,x2_,y2_);

debugPrint("deltL1:");
debugPrint(deltL1);
debugPrint(", deltL2:");
debugPrintln(deltL2);

double step1ToRun=deltL1*56.657;
double step2ToRun=deltL2*56.657;

debugPrint("stepL1:");
debugPrint(step1ToRun);
debugPrint(" , stepL2:");
debugPrint(step2ToRun);

//确定了两个电机转向还有转步之后确定转速
//尽量让两者转动的总时间相同
double k;
if(step2ToRun!=0)
k=abs(step1ToRun/step2ToRun);//两者要走的路程的比值
else
k=0.3;

debugPrint(" , K:");
debugPrintln(k);

//用来计算两者的电机转速（周期反比）
//转速周期范围800us - 2300us
//MINPERIOD-MAXPERIOD
  if(k>1)//k>1时   MINPERIOD<p1<(MAXPERIOD/k)
  {
    if(k>(double)MAXPERIOD/MINPERIOD)//超量程比值
    {
      periodL1=MINPERIOD;
      periodL2=MAXPERIOD;
    }
    else//k在正常量程之下
    {
      periodL1=0.5*(MINPERIOD+MAXPERIOD/k);
      periodL2=k*periodL1;
    }
  }
  else//k<1时    (800/k)<p1<2300
  {
    if(k<(double)MINPERIOD/MAXPERIOD)//超量程比例
    {
      periodL1=MAXPERIOD;
      periodL2=MINPERIOD;
    }
    else//k在正常量程之下
    {
      periodL1=0.5*(MINPERIOD/k+MAXPERIOD);
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
  debugPrintln("");
}
int serial_putc( char c, struct __file * )
{
  Serial.write( c );
  return c;
}
void printf_begin(void)
{
  fdevopen( &serial_putc, 0 );
}
void moveToPoint(double x_,double y_)
{
  printf("(%d,%d)=>(%d,%d)\n",(int)currentX,(int)currentY,(int)x_,(int)y_);
  moveP2P(currentX,currentY,x_,y_);
  currentX=x_;
  currentY=y_;
}

void handle(String s)
{
  unsigned int x_=0,y_=0;
  double h=12.43;
  int count=0;
  if(s[0]=='c')
  {
    drawCircles();
    return;
  }
 
  if(s[0]=='d')
  {
    count=1;
        while(s[count]!=',')
      {
        x_*=10;
        x_+=s[count]-'0';
        count++;
        if(count==s.length())break;
      }
      count++;
      while(count<s.length())
      {
        y_*=10;
        y_+=s[count]-'0';
        count++;
      }
      if(x_<0)x_=0;
      if(x_>70)x_=70;
      if(y_<0)y_=0;
      if(y_>100)y_=100;
      printf("Circle center (%d,%d)\n",(int)x_,(int)y_);
      drawCirclesCalc(x_,y_);
      return;
  }
  if(s[0]=='u')
  {
    printf("Unlock motor!");
    digitalWrite(sleepPin1,0);
    digitalWrite(sleepPin2,0);
    return;
  }
  if(s[0]=='s')
  {
    count=1;
        while(s[count]!=',')
      {
        x_*=10;
        x_+=s[count]-'0';
        count++;
        if(count==s.length())break;
      }
      count++;
      while(count<s.length())
      {
        y_*=10;
        y_+=s[count]-'0';
        count++;
      }
      if(x_<0)x_=0;
      if(x_>70)x_=70;
      if(y_<0)y_=0;
      if(y_>100)y_=100;
      currentX=x_;
      currentY=y_;
      printf("Position set to (%d,%d)\n",(int)currentX,(int)currentY);
      return;
  }
  while(s[count]!=',')
  {
    x_*=10;
    x_+=s[count]-'0';
    count++;
    if(count==s.length())break;
  }
  count++;
  while(count<s.length())
  {
    y_*=10;
    y_+=s[count]-'0';
    count++;
  }
  if(x_<0)x_=0;
  if(x_>70)x_=70;
  if(y_<0)y_=0;
  if(y_>100)y_=100;
  printf("(%d,%d)==>(%d,%d)",(int)currentX,(int)currentY,x_,y_);
  moveToPoint(x_,y_);
}
void drawCirclesCalc(double X0,double Y0)
{
  double cX,cY;
  double r=15;
  double stepV=((double)3/180)*PI;
  Serial.println("BeginToCircle");
  moveToPoint(X0+r,Y0);
  delay(1000);
  for(double i=0;i<=2*PI;i+=stepV)
  {
    cX=r*cos(i)+X0;
    cY=r*sin(i)+Y0; 
    printf("(%d,%d)==>(%d,%d)\n",(int)currentX,(int)currentY,(int)cX,(int)cY);
    moveToPoint(cX,cY);
  }
    moveToPoint(X0+r,Y0);
    Serial.println("Finish Circle!");
}
void drawCircles()
{//37  73  121
  Serial.println("BeginToCircle");
  for(int i=0;i<121;i++)
  {
    printf("(%d,%d)==>(%d,%d)\n",(int)currentX,(int)currentY,(int)circleX[i],(int)circleY[i]);
    moveToPoint(circleX3[i],circleY3[i]);
  }
  Serial.println("Finish Circle!");
}

void setup() {
 Serial.begin(115200);
 Serial3.begin(19200);
 printf_begin();
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
 #define DRAW_SPLINE 1
 #define STAND_BY 0
 uint8_t mode=255;
 double dx=0,dy=0;
 String str="";
 int section=20;
 currentX=40;
 currentY=50; 
 while(1)
 {
  while(Serial.available())
  {
    char data=Serial.read();
    delay(1);
    str+=data;
  }
  if(Serial3.available()!=0)
  {
    mode=STAND_BY;
    bool notBreak=true;
    String longData="";
    while(notBreak)
    {
      while(Serial3.available()!=0)
      {
        char data=Serial3.read();
        delayMicroseconds(500);
        if(data=='@')
        {
          notBreak=false;
          break;
        }
        longData+=data;
      }
    }
    Serial.println("Receive from rasp:");
    Serial.println(longData);
    drawSpline(longData);
  }
  if(str!="")
  {
    handle(str);
    str="";
  }
 }
}


