/**
 * @file   EdgeletFeatures.cpp
 * @author feng <huf07@mails.tsinghua.edu.cn>
 * @date   Tue Sep  9 13:47:01 2008
 * 
 * @brief  cpp file of EdgeletFeatures.h
 * 
 * 
 */

#include "EdgeletFeatures.h"
#include <iostream>

#define DIST(A,B) ((A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y))



EdgeletFeatures::EdgeletFeatures(int width, int height) : Features(width,height)
{
  int length = 0;
  CvPoint p1,p2,st,ed;
  int number;
  Edgelet E;
  Pixel P;
  m_edgelets.clear();

  int i, j, m, n, k;

  int num=0;

  std::clog<<"Calculate edgelet templates."<<std::endl;
  for(i=0; i<m_rect_height; i++)
    {
      for(j=0; j<m_rect_width; j++)    
	{
	  st=cvPoint(j,i);
	  //line
	  for(m=0; m<m_rect_height; m++)
	    {
	      for(n=0; n<m_rect_width; n++)
		{
		  ed = cvPoint(m,n);
		  if(n<j)
		    {
		      continue;
		    }
		  if(n==j && m<=i)
		    {
		      continue;
		    }
		  E = RenderLine(st, ed);
		  length = E.size();
		  if(length>3 && length<13)
		    {
		      m_edgelets.push_back(E);
		      num++;
		    }
		}	  
	    }
	  //arc
	  for(m=0; m<m_rect_height; m++)
	    {
	      for(n=0; n<m_rect_width; n++)
		{
		  ed = cvPoint(n,m);
		  if(n<j)
		    {
		      continue;
		    }
		  if(n==j && m<=i)
		    {
		      continue;
		    }

		  float dist = sqrt(float(DIST(st, ed)));

		  if(dist>12 || dist<2)
		    {
		      continue;
		    }
		  for(k=1;k<=3;k++)
		    {
		      E = RenderArc(st, ed, pow(2.0,k));
		      length =E.size();
		      if(length>3 && length<13)
			{
			  m_edgelets.push_back(E);
			  num++;
			}
		    }
		}
	    }
	}
    }
  std::clog<<"Calculate sysmetric pair of edgelet"<<std::endl;
  //sysmetric pair
  number = m_edgelets.size();
  for(i=0;i<number;i++)
    {
      E = m_edgelets[i];
      length = E.size(); 
      P = E[0];
      if(P[0]<m_rect_width/2)
	{
	  for(k=0; k<length; k++)
	    {
	      P=E[k];
	      P[0] = m_rect_width-1-P[0];
	      P[2] = 5-P[2];
	      E.push_back(P);
	    }
	  m_edgelets.push_back(E);
	}
      E = m_edgelets[i];
      P = E[0];
      if(P[1]<m_rect_height/2)
	{
	  for(k=0; k<length; k++)
	    {
	      P=E[k];
	      P[1] = m_rect_height-1-P[1];
	      P[2] = 5-P[2];
	      E.push_back(P);
	    }
	  m_edgelets.push_back(E);		
	}
    }

  m_feature_num = m_edgelets.size();
  m_feature_size = 1;
  m_edge_intensity = NULL;
  m_edge_norm = NULL;

  //edge orientation similarity measurement look up table
  m_l[0] = 1;
  m_l[1] = 0.8;
  m_l[2] = 0.5;
  m_l[3] = 0;
  m_l[4] = 0.5;
  m_l[5] = 0.8;

  return;
}

EdgeletFeatures::~EdgeletFeatures()
{
  m_edgelets.clear();
  if(m_edge_intensity!=NULL)
    {
      cvReleaseMat(&m_edge_intensity);
    }
  if(m_edge_norm!=NULL)
    {
      cvReleaseMat(&m_edge_norm);
    }
  return;
}

void EdgeletFeatures::Init(IplImage* image)
{
  int width = image->width;
  int height = image->height;
  IplImage* sobel_x = cvCreateImage(cvSize(width, height), IPL_DEPTH_16S, 1);
  IplImage* sobel_y = cvCreateImage(cvSize(width, height), IPL_DEPTH_16S, 1);

  if(m_edge_intensity == NULL || m_edge_intensity->width!=width || m_edge_intensity->height!=height)
    {
      Clear();
      m_edge_intensity = cvCreateMat(height, width, CV_32FC1);
      m_edge_norm = cvCreateMat(height, width, CV_8UC1);
    }

  cvSobel(image, sobel_x, 1, 0, 3);
  cvSobel(image, sobel_y, 0, 1, 3);
  
  for(int i=0; i<height; i++)
    {
      for(int j=0;j<width; j++)
	{
	  float gx=cvGetReal2D(sobel_x,i,j);
	  float gy=cvGetReal2D(sobel_y,i,j);
	  float theta;
	  int n;
	  cvSetReal2D(m_edge_intensity, i, j, sqrt(gx*gx+gy*gy));
	  theta = atan2(gy,gx);
	  if( theta < 0 ) theta += CV_PI;
	  if( theta >= CV_PI) theta = 0.0f;
	  n = cvFloor(6.0*theta/CV_PI);
	  cvSetReal2D(m_edge_norm, i, j, n);
	}
    }
  cvReleaseImage(&sobel_x);
  cvReleaseImage(&sobel_y);
  return;
}

void EdgeletFeatures::Clear()
{
  if(m_edge_intensity != NULL)
    {
      cvReleaseMat(&m_edge_intensity);
    }
  if(m_edge_norm != NULL)
    {
      cvReleaseMat(&m_edge_norm);
    }
  

}

Edgelet EdgeletFeatures::RenderLine(CvPoint st, CvPoint ed)
{
  int y1 = st.y;
  int x1 = st.x;
  int y2 = ed.y;
  int x2 = ed.x;
  int flag = 1;
  int length = 0;
  int x,y,p,a,b,f;
  float m;
  int n;
  Edgelet E;
  Pixel P;

  if(x2!=x1)
    {
      m=float(y2-y1)/float(x2-x1);
    }
  else if(y2==y1)
    {
      flag=0; //error
    }
  else
    {
      m=-1000;
    }
  if(flag==0)
    {
      ;
    }
  else if(m>=0 && m<1)
    {
      if(x2>x1)
	{
	  flag=1;
	}
      else
	{
	  flag=2;
	}
    }
  else if(m>=1)
    {
      if(y2>y1)
	{
	  flag=3;
	}
      else
	{
	  flag=4;
	}
    }
  else if(m<0 && m>=-1)
    {	
      if(x2>x1)
	{
	  flag=5;
	}
      else
	{
	  flag=6;;
	}
    }
  else if(m<-1)
    {
      if(y2>y1)
	{
	  flag=7;
	}
      else
	{
	  flag=8;
	}
    }

  if(m>=0 && m<0.577)
    {
      n = 2;
    }
  else if(m>=0.577 && m<1.732)
    {
      n = 1;
    }
  else if(m>=1.732)
    {
      n = 0;
    }
  else if(m<-1.732)
    {
      n = 5;
    }
  else if(m>=-1.732 && m<-0.577)
    {
      n = 4;
    }
  else if(m>=-0.577 && m<0)
    {
      n = 3;
    }
  P[2]=n;
  
  x = x1;
  y = y1;
  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
    {
      P[0] = x;
      P[1] = y;
      E.push_back(P);
      length = 1;
    }
  else
    {
      E.clear();
      return E;
    }

  if(flag==0)
    {
      ; //error
    }
  else if(flag==1)
    {
      x=x1;y=y1;p=2*(y2-y1)-(x2-x1);
      a=2*(y2-y1);
      b=2*(y2-y1-x2+x1);
      while(x!=x2)
	{
	  x++;
	  if(p<0)
	    {
	      p=p+a;
	    }
	  else
	    {
	      y=y++;
	      p=p+b;
	    }
	  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
	    {
	      P[0] = x;
	      P[1] = y;
	      E.push_back(P);
	      length++;
	    }
	  else
	    {
	      E.clear();
	      return E;
	    }
	}
    }
  else if(flag==2)
    {
      x=x1;y=y1;p=2*(y2-y1)+(x2-x1);
      a=-2*(y2-y1);
      b=-2*(y2-y1-x2+x1);
      while(x!=x2)
	{
	  x=x--;
	  if(p<0)
	    {
	      p=p+a;
	    }
	  else
	    {
	      y=y--;
	      p=p+b;
	    }
	  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
	    {
	      P[0] = x;
	      P[1] = y;
	      E.push_back(P);
	      length++;
	    }
	  else
	    {
	      E.clear();
	      return E;
	    }
	}
    }
  else if(flag==3)
    {
      x=x1;y=y1;p=2*(x2-x1)-(y2-y1);
      a=2*(x2-x1);
      b=2*(x2-x1-y2+y1);
      while(y!=y2)
	{
	  y++;
	  if(p<0)
	    {
	      p=p+a;
	    }
	  else
	    {
	      x=x++;
	      p=p+b;
	    }
	  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
	    {
	      P[0] = x;
	      P[1] = y;
	      E.push_back(P);
	      length++;
	    }
	  else
	    {
	      E.clear();
	      return E;
	    }
	}
    }
  else if(flag==4)
    {
      x=x1;y=y1;p=2*(x2-x1)+(y2-y1);
      a=-2*(x2-x1);
      b=-2*(x2-x1-y2+y1);
      while(y!=y2)
	{
	  y=y--;
	  if(p<0)
	    {
	      p=p+a;
	    }
	  else
	    {
	      x=x--;
	      p=p+b;
	    }
	  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
	    {
	      P[0] = x;
	      P[1] = y; 
	      E.push_back(P);
	      length++;
	    }
	  else
	    {
	      E.clear();
	      return E;
	    }
	}
    }
  else if(flag==5)
    {
      x=x1;y=y1;p=2*(y2-y1)-(x2-x1);
      a=2*(-y2+y1);
      b=2*(-y2+y1-x2+x1);
      while(x!=x2)
	{
	  x++;
	  if(p<0)
	    {
	      p=p+a;
	    }
	  else
	    {
	      y=y--;
	      p=p+b;
	    }
	  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
	    {
	      P[0] = x;
	      P[1] = y;
	      E.push_back(P);
	      length++;
	    }
	  else
	    {
	      E.clear();
	      return E;
	    }
	}
    }
  else if(flag==6)
    {
      x=x1;y=y1;p=2*(y2-y1)+(x2-x1);
      a=2*(y2-y1);
      b=2*(y2-y1+x2-x1);
      while(x!=x2)
	{
	  x--;
	  if(p<0)
	    {
	      p=p+a;
	    }
	  else
	    {
	      y=y++;
	      p=p+b;
	    }
	  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
	    {
	      P[0] = x;
	      P[1] = y;
	      E.push_back(P);
	      length++;
	    }
	  else
	    {
	      E.clear();
	      return E;
	    }
	}
    }
  else if(flag==7)
    {
      x=x1;y=y1;p=-2*(x2-x1)-(y2-y1);
      a=-2*(x2-x1);
      b=-2*(x2-x1+y2-y1);
      while(y!=y2)
	{
	  y++;
	  if(p<0)
	    {
	      p=p+a;
	    }
	  else
	    {
	      x=x--;
	      p=p+b;
	    }
	  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
	    {
	      P[0] = x;
	      P[1] = y;
	      E.push_back(P);
	      length++;
	    }
	  else
	    {
	      E.clear();
	      return E;
	    }
	}
    }
  else
    {
      x=x1;y=y1;p=2*(x2-x1)+(y2-y1);
      a=2*(x2-x1);
      b=2*(x2-x1+y2-y1);
      while(y!=y2)
	{
	  y--;
	  if(p<0)
	    {
	      p=p+a;
	    }
	  else
	    {
	      x=x++;
	      p=p+b;
	    }
	  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
	    {
	      P[0] = x;
	      P[1] = y;
	      E.push_back(P);
	      length++;
	    }
	  else
	    {
	      E.clear();
	      return E;
	    }
	}
    }
  return E;
}

Edgelet EdgeletFeatures::RenderArc(CvPoint st, CvPoint ed, int k)
{
  CvPoint2D32f center;
  float dist;
  float radius;
  int y1 = st.y;
  int x1 = st.x;
  int y2 = ed.y;
  int x2 = ed.x;
  int flag = 0;
  int length = 0;
  int y,x;
  float r2;
  float m = 0.0;
  CvPoint pt1,pt2;
  Edgelet E;
  Pixel P;
  int n;

  dist = sqrt(float(DIST(st, ed)));
  
  if(k == 8)
    {
      radius = 0.5*dist/sin(22.5*CV_PI/180);
      center.x = (st.x+ed.x)/2.0-(ed.y-st.y)/(2.0*tan(22.5*CV_PI/180)); 
      center.y = (st.y+ed.y)/2.0+(ed.x-st.x)/(2.0*tan(22.5*CV_PI/180));
    }
  else if(k == 4)
    {
      radius = dist/sqrt(2.0);
      center.x = (st.x+ed.x)/2.0-(ed.y-st.y)/2.0;
      center.y = (st.y+ed.y)/2.0+(ed.x-st.x)/2.0;
    }
  else if(k==2)
    {
      radius = dist/2.0;
      center.x = (st.x+ed.x)/2.0;
      center.y = (st.y+ed.y)/2.0;
    }
  else
    {
      return E;
    }
  
  r2 = radius*radius;
 
  flag=1;
  
  if(x1!=center.x)
    {
      m=(y1-center.y)/(x1-center.x);
    }
  else if(y1==center.y)
    {
      flag = 0;
    }
  else
    {
      m=-1000;
    }
  if(flag==0)
    {
      ;
    }
  else if(m>=0 && m<1)
    {
      if(x1>center.x)
	{
	  flag=1;
	}
      else
	{
	  flag=2;
	}
    }
  else if(m>=1)
    {
      if(y1>center.y)
	{
	  flag=3;
	}
      else
	{
	  flag=4;
	}
    }
  else if(m<0 && m>=-1)
    {	
      if(x1>center.x)
	{
	  flag=5;
	}
      else
	{
	  flag=6;;
	}
    }
  else if(m<-1)
    {
      if(y1>center.y)
	{
	  flag=7;
	}
      else
	{
	  flag=8;
	}
    }

  x = x1;
  y = y1;  

  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
    {
      m=atan2(y-center.y, x-center.x);
      n=cvFloor(abs(m*6.0/CV_PI));
      P[0] = x;
      P[1] = y;
      P[2] = n;
      E.push_back(P);
      length = 1;
    }
  else
    {
      E.clear();
      return E;
    }

 HERE:
  if(flag==0)
    {
      ;
    }
  else if(flag==1)
    {
      while(y-center.y<x-center.x)
	{
	  y++;
	  pt1 = cvPoint(x,y);
	  pt2 = cvPoint(x-1,y);
	  if(abs(DIST(pt1,center)-r2)>abs(DIST(pt2,center)-r2))
	    {
	      x=x-1;
	    }
	  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
	    {
	      m=atan2(y-center.y, x-center.x);
	      n=cvFloor(abs(m*6.0/CV_PI));
	      P[0] = x;
	      P[1] = y;
	      P[2] = n;
	      E.push_back(P);
	      length++;
	    }
	  else
	    {
	      E.clear();
	      return E;
	    }
	  if(x==x2 && y==y2)
	    {
	      flag = 0;
	      break;
	    }
	}
      if(flag!=0)
	{
	  flag = 3;
	  goto HERE;
	}    
    } 
  else if(flag == 2)
    {    
      while(y-center.y>x-center.x)
	{
	  y--;
	  pt1 = cvPoint(x,y);
	  pt2 = cvPoint(x+1,y);
	  if(abs(DIST(pt1,center)-r2)>abs(DIST(pt2,center)-r2))
	    {
	      x=x+1;
	    }
	  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
	    {
	      m=atan2(y-center.y, x-center.x);
	      n=cvFloor(abs(m*6.0/CV_PI));
	      P[0] = x;
	      P[1] = y;
	      P[2] = n;
	      E.push_back(P);
	      length++;
	    }
	  else
	    {
	      E.clear();
	      return E;
	    }
	  if(x==x2 && y==y2)
	    {
	      flag = 0;
	      break;
	    }
	}
      if(flag!=0)
	{
	  flag = 4;
	  goto HERE;
	}    
    }
  else if(flag == 3)
    {
      while(x-center.x>0)
	{
	  x--;
	  pt1 = cvPoint(x,y);
	  pt2 = cvPoint(x,y+1);
	  if(abs(DIST(pt1,center)-r2)>abs(DIST(pt2,center)-r2))
	    {
	      y=y+1;
	    }
	  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
	    {
	      m=atan2(y-center.y, x-center.x);
	      n=cvFloor(abs(m*6.0/CV_PI));
	      P[0] = x;
	      P[1] = y;
	      P[2] = n;
	      E.push_back(P);
	      length++;
	    }
	  else
	    {
	      E.clear();
	      return E;
	    }
	  if(x==x2 && y==y2)
	    {
	      flag = 0;
	      break;
	    }
	}
      if(flag!=0)
	{
	  flag = 7;
	  goto HERE;
	}       
    }
  else if(flag == 4)
    {
      while(x-center.x<0)
	{
	  x++;
	  pt1 = cvPoint(x,y);
	  pt2 = cvPoint(x,y-1);
	  if(abs(DIST(pt1,center)-r2)>abs(DIST(pt2,center)-r2))
	    {
	      y=y-1;
	    }
	  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
	    {
	      m=atan2(y-center.y, x-center.x);
	      n=cvFloor(abs(m*6.0/CV_PI));
	      P[0] = x;
	      P[1] = y;
	      P[2] = n;
	      E.push_back(P);
	      length++;
	    }
	  else
	    {
	      E.clear();
	      return E;
	    }
	  if(x==x2 && y==y2)
	    {
	      flag = 0;
	      break;
	    }
	}
      if(flag!=0)
	{
	  flag = 8;
	  goto HERE;
	}      
    }
  else if(flag==5)
    {
      while(y-center.y<0)
	{
	  y++;
	  pt1 = cvPoint(x,y);
	  pt2 = cvPoint(x+1,y);
	  if(abs(DIST(pt1,center)-r2)>abs(DIST(pt2,center)-r2))
	    {
	      x=x+1;
	    }
	  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
	    {
	      m=atan2(y-center.y, x-center.x);
	      n=cvFloor(abs(m*6.0/CV_PI));
	      P[0] = x;
	      P[1] = y;
	      P[2] = n;
	      E.push_back(P);
	      length++;
	    }
	  else
	    {
	      E.clear();
	      return E;
	    }
	  if(x==x2 && y==y2)
	    {
	      flag = 0;
	      break;
	    }
	}
      if(flag!=0)
	{
	  flag = 1;
	  goto HERE;
	}    
    }
  else if(flag==6)
    {
      while(y-center.y>0)
	{
	  y--;
	  pt1 = cvPoint(x,y);
	  pt2 = cvPoint(x-1,y);
	  if(abs(DIST(pt1,center)-r2)>abs(DIST(pt2,center)-r2))
	    {
	      x=x-1;
	    }
	  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
	    {
	      m=atan2(y-center.y, x-center.x);
	      n=cvFloor(abs(m*6.0/CV_PI));
	      P[0] = x;
	      P[1] = y;
	      P[2] = n;
	      E.push_back(P);
	      length++;
	    }
	  else
	    {
	      E.clear();
	      return E;
	    }
	  if(x==x2 && y==y2)
	    {
	      flag = 0;
	      break;
	    }
	}
      if(flag!=0)
	{
	  flag = 2;
	  goto HERE;
	}    
    }
  else if(flag==7)
    {
      while(y-center.y+x-center.x>0)
	{
	  x--;
	  pt1 = cvPoint(x,y);
	  pt2 = cvPoint(x,y-1);
	  if(abs(DIST(pt1,center)-r2)>abs(DIST(pt2,center)-r2))
	    {
	      y=y-1;
	    }
	  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
	    {
	      m=atan2(y-center.y, x-center.x);
	      n=cvFloor(abs(m*6.0/CV_PI));
	      P[0] = x;
	      P[1] = y;
	      P[2] = n;
	      E.push_back(P);
	      length++;
	    }
	  else
	    {
	      E.clear();
	      return E;
	    }
	  if(x==x2 && y==y2)
	    {
	      flag = 0;
	      break;
	    }
	}
      if(flag!=0)
	{
	  flag = 6;
	  goto HERE;
	} 
    }   
  else
    {
      while(y-center.y+x-center.x<0)
	{
	  x++;
	  pt1 = cvPoint(x,y);
	  pt2 = cvPoint(x,y+1);
	  if(abs(DIST(pt1,center)-r2)>abs(DIST(pt2,center)-r2))
	    {
	      y=y+1;
	    }
	  if(x>=0 && x<m_rect_width && y>=0 && y<m_rect_height)
	    {
	      m=atan2(y-center.y, x-center.x);
	     n=cvFloor(abs(m*6.0/CV_PI));
	      P[0] = x;
	      P[1] = y;
	      P[2] = n;
	      E.push_back(P);
	      length++;
	    }
	  else
	    {
	      E.clear();
	      return E;
	    }
	  if(x==x2 && y==y2)
	    {
	      flag = 0;
	      break;
	    }
	}
      if(flag!=0)
	{
	  flag = 5;
	  goto HERE;
	}    
    }
  return E;
}



// bool EdgeletFeatures::Extract(float* fv,int y, int x)
// {
//   float r;
//   int length,m,n,i,j;
//   float v;
//   int d;
//   for(i=0;i<m_feature_num;i++)
//     {
//       length = m_edgelets[i].size();
// 	  r=0.0;
//       for(j=0; j<length; j++)
// 	{
// 	  m = x+m_edgelets[i][j][0];
// 	  n = y+m_edgelets[i][j][1];
// 	  v = ((float*)(m_edge_intensity->data.ptr + m_edge_intensity->step*n))[m];
// 	  d = ((char*)(m_edge_norm->data.ptr + m_edge_norm->step*n))[m];
// 	  r+=v*m_l[abs(d-m_edgelets[i][j][2])];
// 	}
//       r=r/length;
//       fv[i] = r;
//     }
//   return true;
// }

bool EdgeletFeatures::Extract(float* fv,int y, int x, bool* selected)
{
  float r;
  int length,m,n;
  Pixel P;
  for(int i=0;i<m_feature_num;i++)
    {
      if(selected!=NULL && !selected[i])
	{
	  continue;
	}
      length = m_edgelets[i].size();
      r = 0.0f;
      for(int j=0; j<length; j++)
	{
	  P = m_edgelets[i][j];
	  m = x+P[0];
	  n = y+P[1];
	  r+=cvGetReal2D(m_edge_intensity, n, m)*m_l[int(abs(cvGetReal2D(m_edge_norm, n, m) - P[2]))];
	}
      r=r/length;
      fv[i] = r;
    }
  return true;
}

bool EdgeletFeatures::Extract(float* fv, int y, int x, int* selected,int num)
{
	float r;
	int length,m,n,i,j,k;
	Pixel P;
	for(i=0;i<num;i++)
	{
		j = selected[i];
		length = m_edgelets[j].size();
		r = 0.0f;
		for(k=0; k<length; k++)
		{
			P = m_edgelets[j][k];
			m = x+P[0];
			n = y+P[1];
			r+=cvGetReal2D(m_edge_intensity, n, m)*m_l[int(abs(cvGetReal2D(m_edge_norm, n, m) - P[2]))];
		}
		r=r/length;
		fv[i] = r;
	}
	return true;
}

void EdgeletFeatures::DrawFeature(IplImage* img, int n, CvScalar rgb)
{
  for(int i=0;i<m_edgelets[n].size();i++)
  {
    cvSet2D(img, m_edgelets[n][i][1], m_edgelets[n][i][0], rgb);
  }
  return;
}

void EdgeletFeatures::ReadFromFile(std::ifstream& f)
{

}

void EdgeletFeatures::WriteToFile(std::ofstream& f)
{

}
