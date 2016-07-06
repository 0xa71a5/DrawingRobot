#define Points_
#ifdef Points_
#include <iostream>
#include<opencv/cv.h>
#include<opencv/cvaux.h>
#include<opencv/cxcore.h>
#include<opencv/highgui.h>
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
int	index=0;

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

		corners[index].x=x;
		corners[index].y=y;
		printf("corner%d (%d,%d)\n",index,x,y);
		if(index==3)
		{
			notClose=false;
		}
		index=(index+1)%4;
    break;

	case CV_EVENT_RBUTTONUP:
		index=0;
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
	Mat rawImg=imread("G:\\2016raw.jpg");
	
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
		waitKey(30);
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
	
	
	vector<vector<Point>> countours;
	vector<vector<Point>> countours2;
	vector<vector<Point>> wall;
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
	vector<vector<Point>> pp(3);
	vector<vector<Point>> pt(1);
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
	vector<Point> poi;
	float start,end,step;
	if(decideK(sampleMat1))
	{
		start=a;
		end=-a;
		step=-1;
		k=a/b;
	}
	else
	{
		start=0;
		end=a+a;
		step=-1;
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
				line(imgToMap,Point(sumX+wall[matIndex][0].x,sumY+wall[matIndex][0].y),poi[poi.size()-1]+Point(wall[matIndex][0].x,wall[matIndex][0].y),Scalar(0,0,255));
			}
			
			//sumX+=1;
			poi.push_back(Point(sumX,sumY));
			//imgToMap.at<Vec3b>(sumY+wall[matIndex][0].y,sumX+wall[matIndex][0].x)=Vec3b(0,0,255);
			
			
			sampleMat1.at<uchar>(sumY,sumX)=255;
			sumX=sumX+wall[matIndex][0].x;
			sumY=sumY+wall[matIndex][0].y;
			sumX/=5;
			sumY/=5;
			sumY=100-sumY;
			cout<<Point(sumX,sumY)<<endl;
			imshow("demoMap",imgToMap);
			imshow("demoCanny",cannyImg);
			//imshow("demoSampleMat1",sampleMat1);
			waitKey(30);
		}
	}
	}
	
	//imshow("demoSampleMat1",sampleMat1);
	imshow("demoCanny",cannyImg);
	imshow("demoMap",imgToMap);
	cout<<"Done!"<<endl;
	waitKey(0);
	return 0;
}










#endif