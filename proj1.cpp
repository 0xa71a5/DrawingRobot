#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;
using namespace cv;
int thresholdValue=150;
int cannyThreshold=30;
Mat grayImg;
Mat threImg;
Mat processImg;
Mat cannyImg;
Mat imgToMap(500,350,CV_8UC3);
bool notClose=true;
vector<Point2f> corners(4);
int cIndex=0;


int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
          B38400, B19200, B9600, B4800, B2400, B1200, B300, B115200 };
int name_arr[] = {38400,  19200,  9600,  4800,  2400,  1200,  300, 38400,  
          19200,  9600, 4800, 2400, 1200,  300, 115200 };

int fd;
int nread;
char buff[512];
char *dev  = "/dev/ttyUSB0"; //串口二




int set_Parity(int fd,int databits,int stopbits,int parity)
{ 
	struct termios options; 
	if  ( tcgetattr( fd,&options)  !=  0) { 
		perror("SetupSerial 1");     
		return(-1);  
	}
	options.c_cflag &= ~CSIZE; 
	switch (databits) /*设置数据位数*/
	{   
	case 7:		
		options.c_cflag |= CS7; 
		break;
	case 8:     
		options.c_cflag |= CS8;
		break;   
	default:    
		fprintf(stderr,"Unsupported data size\n"); return (-1);  
	}
switch (parity) 
{   
	case 'n':
	case 'N':    
		options.c_cflag &= ~PARENB;   /* Clear parity enable */
		options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
		break;  
	case 'o':   
	case 'O':     
		options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/  
		options.c_iflag |= INPCK;             /* Disnable parity checking */ 
		break;  
	case 'e':  
	case 'E':   
		options.c_cflag |= PARENB;     /* Enable parity */    
		options.c_cflag &= ~PARODD;   /* 转换为偶效验*/     
		options.c_iflag |= INPCK;       /* Disnable parity checking */
		break;
	case 'S': 
	case 's':  /*as no parity*/   
	    options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;break;  
	default:   
		fprintf(stderr,"Unsupported parity\n");    
		return (-1);  
	}  
/* 设置停止位*/  
switch (stopbits)
{   
	case 1:    
		options.c_cflag &= ~CSTOPB;  
		break;  
	case 2:    
		options.c_cflag |= CSTOPB;  
	   break;
	default:    
		 fprintf(stderr,"Unsupported stop bits\n");  
		 return (-1); 
} 
/* Set input parity option */ 
if (parity != 'n')   
	options.c_iflag |= INPCK; 
tcflush(fd,TCIFLUSH);
options.c_cc[VTIME] = 150; /* 设置超时15 seconds*/   
options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
if (tcsetattr(fd,TCSANOW,&options) != 0)   
{ 
	perror("SetupSerial 3");   
	return (-1);  
} 
return (0);  
}


void set_speed(int fd, int speed){
  int   i; 
  int   status; 
  struct termios   Opt;
  tcgetattr(fd, &Opt); 
  for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) { 
    if  (speed == name_arr[i]) {     
      tcflush(fd, TCIOFLUSH);     
      cfsetispeed(&Opt, speed_arr[i]);  
      cfsetospeed(&Opt, speed_arr[i]);   
      status = tcsetattr(fd, TCSANOW, &Opt);  
      if  (status != 0) {        
        perror("tcsetattr fd1");  
        return;     
      }    
      tcflush(fd,TCIOFLUSH);   
    }  
  }
}
int OpenDev(char *Dev)
{
	int	fd = open( Dev, O_RDWR );         //| O_NOCTTY | O_NDELAY	
	if (-1 == fd)	
	{ 			
		perror("Can't Open Serial Port");
		return -1;		
	}	
	else	
		return fd;
}
void Serial_begin(char devs[],int speed_num=115200)
{

	fd = OpenDev(devs);
	set_speed(fd,speed_num);
	if (set_Parity(fd,8,1,'N') == -1)  {
		printf("Set Parity Error\n");
		exit (0);
	}
}

void Serial_print(string data)
{
	//char *buffer=new char [data.length()];
	//char  buffer[1024]=" Call back Test! ";
	const char *buffer=data.c_str();
	int    Length=data.length();
	int    nByte;
	nByte = write(fd, buffer ,Length);
}


void trackbar(int input,void *u)  
{  
    thresholdValue=input;
}  
void trackCanny(int input,void *u)  
{  
    Canny(processImg,cannyImg,input,int(float(input)*1.5),3);
	printf("Thre1:%d Thre2:%d  \n",input,int(float(input)*1.5));

	imshow("demoCanny",cannyImg);
}
void cvMouseCallback(int mouseEvent,int x,int y,int flags,void* param)
{
  switch(mouseEvent)
  {
	case CV_EVENT_LBUTTONUP:

		corners[cIndex].x=x;
		corners[cIndex].y=y;
		printf("corner%d (%d,%d)\n",cIndex,x,y);
		if(cIndex==3)
		{
			notClose=false;
		}
		cIndex=(cIndex+1)%4;
    break;

	case CV_EVENT_RBUTTONUP:
		cIndex=0;
	break;
  }
}
void mouseCall2(int eventIndex,int x,int y,int flag,void *param)
{
	if(eventIndex==CV_EVENT_LBUTTONDOWN)
	{
		
		printf("(%d,%d)\n",x,y);
	}
}

void mouseCall1(int eventIndex,int x,int y,int flag,void *param)
{
	if(eventIndex==CV_EVENT_LBUTTONDOWN)
	{
		int value=cannyImg.at<uchar>(x,y);
		printf("%d\n",value);
	}
}
bool decideK(const Mat &A)
{
	//true => k>0
	//false => k<0
	int rows=A.rows;
	int cols=A.cols;
	unsigned long int counts[4];
	counts[0]=counts[1]=counts[2]=counts[3]=0;
	int i,j;
	for(i=0;i<cols/2;i++)
		for(j=0;j<rows/2;j++)
			if(A.at<uchar>(j,i)==255)counts[0]++;
	for(i=cols/2;i<cols;i++)
		for(j=0;j<rows/2;j++)
			if(A.at<uchar>(j,i)==255)counts[1]++;
	for(i=0;i<cols/2;i++)
		for(j=rows/2;j<rows;j++)
			if(A.at<uchar>(j,i)==255)counts[2]++;
	for(i=cols/2;i<cols;i++)
		for(j=rows/2;j<rows;j++)
			if(A.at<uchar>(j,i)==255)counts[3]++;

	if((counts[0]+counts[3])>(counts[1]+counts[2]))
			return false;
	return true;

}




int main()
{
	Serial_begin("/dev/ttyUSB0",19200);
	corners[0]=Point2f(72,51);
	corners[1]=Point2f(363,35);
	corners[2]=Point2f(366,457);
	corners[3]=Point2f(56,449);

	Mat rawImg=imread("2016raw.jpg");
	
	//waitKey(1000);
	//cvDestroyWindow("demoRaw");

	cvNamedWindow("demoMap",0);
	//cvNamedWindow("demoSampleMat1",0);
	//cvNamedWindow("demoThre",1);
	//cvNamedWindow("demoProc",1);
	cvNamedWindow("demoCanny",1);
	cvNamedWindow("demoRaw",1);
	//imshow("demoRaw",rawImg);
	createTrackbar( "Threshold", "demoThre", &thresholdValue, 255, trackbar);
	createTrackbar( "Threshold", "demoCanny", &cannyThreshold, 255, trackCanny);
	setMouseCallback("demoCanny",mouseCall1);
	setMouseCallback("demoThre",mouseCall2);
	setMouseCallback("demoMap",mouseCall2);
	setMouseCallback("demoRaw",cvMouseCallback);
	cvtColor(rawImg,grayImg,CV_BGR2GRAY,0);
	while(notClose)
	{
		imshow("demoRaw",rawImg);
		if(waitKey(30)==27)
			notClose=false;
	}
	cvDestroyWindow("demoRaw");


    int img_height = rawImg.rows;  
    int img_width = rawImg.cols;  
    vector<Point2f> corners_trans(4);  
    corners_trans[0] = Point2f(0,0);  
    corners_trans[1] = Point2f(imgToMap.cols-1,0);  
    corners_trans[2] = Point2f(imgToMap.cols-1,imgToMap.rows-1);  
    corners_trans[3] = Point2f(0,imgToMap.rows-1);  
  
    Mat transform = getPerspectiveTransform(corners,corners_trans);  
    cout<<transform<<endl;  
    warpPerspective(rawImg, imgToMap, transform, imgToMap.size());
	//imgToMap=imread("G:\\2016Map.jpg");
	imshow("demoMap",imgToMap);
	
	cvtColor(imgToMap,grayImg,CV_BGR2GRAY);
	threshold(grayImg,threImg,150,255,0);
	//imshow("demoThre",threImg);
	Mat element=getStructuringElement(MORPH_CROSS,Size(7,7));
	dilate(threImg,processImg,element);
	erode(processImg,processImg,element);
	//imshow("demoProc",processImg);
	Canny(processImg,cannyImg,50,80,3);
	
	
	vector<vector<Point> > countours;
	vector<vector<Point> > countours2;
	vector<vector<Point> > wall;
	vector<Vec4i> hierarchy;
	findContours(processImg,countours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE,Point(0,0));
	for(int i=0;i<countours.size();i++)
	{
		if(countours[i].size()>10)
			countours2.push_back(countours[i]);
	}
	countours=countours2;
	cout<<"Countours:"<<countours.size()<<endl;
	vector<Point> tempP2;
	for(int i=0;i<countours.size();i++)
	{
		
		cout<<i<<":"<<countours[i].size()<<endl;
		int up=1000,down=0,right=0,left=1000;

		for(int j=0;j<countours[i].size();j++)
		{
			if(countours[i][j].x<left)left=countours[i][j].x;
			if(countours[i][j].x>right)right=countours[i][j].x;
			if(countours[i][j].y<up)up=countours[i][j].y;
			if(countours[i][j].y>down)down=countours[i][j].y;
			imgToMap.at<Vec3b>(countours[i][j].y,countours[i][j].x)=Vec3b(0,255,0);
		}
		left--;right++;up--;down++;
		rectangle(imgToMap,Point(left,up),Point(right,down),Scalar(0,0,255));
		tempP2.clear();
		if((right-left)>300&&(down-up)>450)
			continue;
		tempP2.push_back(Point(left,up));
		tempP2.push_back(Point(right,down));
		wall.push_back(tempP2);		
	}
	vector<vector<Point> > pp(3);
	vector<vector<Point> > pt(1);
	pp[0]=wall[0];
	pp[1]=wall[1];
	pp[2]=wall[2];

	if(pp[0][1].y<pp[1][1].y)
	{
		pt[0]=pp[0];
		pp[0]=pp[1];
		pp[1]=pt[0];
	}
	else if(pp[2][1].y>pp[1][1].y&&pp[2][1].y<pp[0][1].y)
	{
		pt[0]=pp[1];
		pp[1]=pp[2];
		pp[2]=pt[0];
	}
	else if(pp[2][1].y>pp[0][1].y)
	{
		pt[0]=pp[2];
		pp[2]=pp[1];
		pp[1]=pp[0];
		pp[0]=pt[0];
	}
	wall[0]=pp[0];
	wall[1]=pp[1];
	wall[2]=pp[2];
	

	for(int i=0;i<wall.size();i++)
	{
		cout<<"Wall:"<<i<<"  ";
		for(int j=0;j<wall[i].size();j++)
		{
			cout<<wall[i][j]<<" ";
		}
		//rectangle(cannyImg,wall[i][0],wall[i][1],Scalar(255));
		cout<<endl;
	}
	Mat sampleMat1;
	vector<Point> poi,poi2;
	for(int matIndex=0;matIndex<3;matIndex++)
	{
	sampleMat1=Mat(cannyImg,Rect(wall[matIndex][0].x,wall[matIndex][0].y,wall[matIndex][1].x-wall[matIndex][0].x,wall[matIndex][1].y-wall[matIndex][0].y));
	float a=sampleMat1.rows;
	float b=sampleMat1.cols;
	float x,y;
	float k=a/b;
	float deltX=a;
	float sumX=0,sumY=0;
	float count=0;
	
	float start,end,step;
	if(decideK(sampleMat1))
	{
		start=a;
		end=-a;
		step=-3;
		k=a/b;
	}
	else
	{
		start=0;
		end=a+a;
		step=-3;
		k=-a/b;
	}
	for(deltX=start;deltX>end;deltX+=step)
	{
		sumX=0;
		sumY=0;
		count=0;
		for (x=0;x<b;x++)
		{
			y=k*x+deltX;
			if(sampleMat1.at<uchar>((int)y,(int)x)==255)
			{
				count++;
				sumX+=x;
				sumY+=y;
			}
		}
		//line(sampleMat1,Point(0,-deltX),Point(b,a+deltX),Scalar(255));
		if(count==2)
		{
			sumX=sumX/count;
			sumY=sumY/count;
			if(poi.size()>0)
			{
				if(poi[poi.size()-1]==Point(sumX,sumY))
				{
					continue;
				}
				//line(sampleMat1,Point(sumX,sumY),poi[poi.size()-1],Scalar(255));
				//line(imgToMap,Point(sumX+wall[matIndex][0].x,sumY+wall[matIndex][0].y),poi[poi.size()-1]+Point(wall[matIndex][0].x,wall[matIndex][0].y),Scalar(0,0,255));
			}
			
			//sumX+=1;
			
			imgToMap.at<Vec3b>(sumY+wall[matIndex][0].y,sumX+wall[matIndex][0].x)=Vec3b(0,0,255);
			
			poi.push_back(Point(sumX,sumY));

			sampleMat1.at<uchar>(sumY,sumX)=255;
			sumX=sumX+wall[matIndex][0].x;
			sumY=sumY+wall[matIndex][0].y;
			sumX/=5;
			sumY/=5;
			sumY=100-sumY;
			cout<<Point(sumX,sumY)<<endl;
			poi2.push_back(Point(sumX,sumY));
			imshow("demoMap",imgToMap);
			imshow("demoCanny",cannyImg);
			//imshow("demoSampleMat1",sampleMat1);
			waitKey(30);
		}
	}
	}
	String mesg="";
	
	String tempS="";
	//string mesg="";
	//string tempS="";
	stringstream ss;


	mesg="<L";
	ss.clear();
	ss<<(int)poi.size();
	ss>>tempS;
	mesg+=tempS;
	mesg+=">";
	for(int i=0;i<poi2.size();i++)
	{
		mesg+="<X";
		ss.clear();
		ss<<(int)poi2[i].x;
		ss>>tempS;
		mesg+=tempS;
		mesg+="><Y";
		ss.clear();
		ss<<(int)poi2[i].y;
		ss>>tempS;
		mesg+=tempS;
		mesg+=">";
		cout<<"("<<(int)poi2[i].x<<"*"<<(int)poi2[i].y<<")"<<endl;
	}
	cout<<"Message:"<<endl<<mesg<<endl;
	cout<<"Serial out>>\n";
	tempS="#";
	Serial_print(tempS);
	usleep(500);
	for(int i=0;i<mesg.length();i++)
	{
		tempS=mesg[i];
		Serial_print(tempS);
		usleep(500);
	}
	tempS="@";
	Serial_print(tempS);
	
	//imshow("demoSampleMat1",sampleMat1);
	imshow("demoCanny",cannyImg);
	imshow("demoMap",imgToMap);
	cout<<"Done!\n"<<endl;

	return 0;
}
