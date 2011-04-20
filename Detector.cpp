/**
 * @file   Detector.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:08:45 2010
 * 
 * @brief  implementation of Detector.
 * 
 * 
 */
#include <cv.h>
#include <highgui.h>
#include "Detector.h"
#include "HogFeatures.h"
#include "HaarFeatures.h"
#include "EdgeletFeatures.h"
#include "CascadeClassifier.h"
#include "BoostClassifier.h"
#include "SvmClassifier.h"
#include <typeinfo>

#include <iostream>

Detector::Detector()
{
     m_classifier = NULL;
     m_extractor = NULL;
}

Detector::~Detector()
{

}


CvRect Detector::IntersectRect(const CvRect& i, const CvRect& j)
{
     CvRect k;

     int itop, ibottom, ileft, iright, jtop, jbottom, jleft, jright, top, bottom ,left, right, width, height;
     ileft = i.x;
     itop = i.y;
     iright = ileft+i.width-1;
     ibottom = itop + i.height -1;

     jleft = j.x;
     jtop = j.y;
     jright = jleft+j.width-1;
     jbottom = jtop + j.height -1;

     left=ileft>jleft?ileft:jleft;
     top=itop>jtop?itop:jtop;
     bottom = ibottom<jbottom?ibottom:jbottom;
     right= iright<jright?iright:jright;
     width = right - left +1;
     height =bottom - top +1;

     if(width>0 && height>0)
     {
	  k.x = left;
	  k.y = top;
	  k.width = width;
	  k.height = height;
     }
     else
     {
	  k.x = 0;
	  k.y = 0;
	  k.width = 0;
	  k.height = 0;
     }
     return k;
}

CvRect Detector::UnionRect(const CvRect& i, const CvRect& j)
{
     CvRect k;
     k.x = (i.x < j.x ? i.x : j.x);
     k.y = (i.y < j.y ? i.y : j.y);
     k.width = ((i.x+i.width) > (j.x + j.width) ? (i.x+i.width) : (j.x + j.width)) - k.x ;
     k.height= ((i.y+i.height) > (j.y + j.height) ? (i.y+i.height) : (j.y + j.height)) - k.y ;
     return k;
}

inline int Detector::SizeOfRect(const CvRect& rect)
{
     return rect.height*rect.width;
}

void Detector::PostProcess(std::vector<CvRect>& results, int combine_min)
{
     std::vector<CvRect> res1;
     std::vector<CvRect> resmax;
     std::vector<int> res2;
     bool yet;
     CvRect rectInter;
		
     int i, j, size;
     for(i=0;i<results.size();i++)
     {
	  yet = false;
	  for(j=0;j<res1.size();j++)
	  {
	       rectInter = IntersectRect(results[i],resmax[j]);
	       if(rectInter.width>0 && rectInter.width>0)
	       {
		    size = SizeOfRect(rectInter);
		    if(
			 size>0.6*SizeOfRect(results[i])
			 && size>0.6*SizeOfRect(resmax[j])
			 )
		    {
			 resmax[j] = UnionRect(resmax[j],results[i]);
			 res1[j].x += results[i].x;
			 res1[j].y += results[i].y;
			 res1[j].width += (results[i].width-1);
			 res1[j].height += (results[i].height-1);
			 res2[j]++;
			 yet = true;
			 break;
		    }
	       }
	  }
	  if(yet==false)
	  {
	       res1.push_back(results[i]);
	       resmax.push_back(results[i]);
	       res2.push_back(1);			
	  }
     }

     for(i=0;i<res1.size();i++)
     {
	  int count = res2[i];
	  res1[i].x =res1[i].x/count;
	  res1[i].y =res1[i].y/count;
	  res1[i].width =res1[i].width/count;
	  res1[i].height =res1[i].height/count;
     }

     results.clear();
     for(i=0;i<res1.size();i++) 
     {
	  if(res2[i]>combine_min)
	  {
	       results.push_back(res1[i]);
	  }
     }
}

void Detector::Check(IplImage* img, std::vector<CvRect>& results)
{
     IplImage *image, *image_resized;
     int rect_width = m_extractor->GetWidth();
     int rect_height = m_extractor->GetHeight();
     if(img==NULL || img->width<rect_width || img->height<rect_height)
     {
	  return;
     }
     image = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
     if(img->nChannels == 3)
     {   
	  cvCvtColor(img, image, CV_BGR2GRAY);
     }
     else
     {
	  cvCopy(img,image,NULL);
     }
     int img_width = img->width;
     int img_height = img->height;

     int result;
     int size;
     float ratio = 1.0;
     CvRect rect;
	

     size = m_extractor->GetFeatureNum();

     float* features = (float*)malloc(m_extractor->GetFeatureNum()*m_extractor->GetFeatureSize()*sizeof(float));

     results.clear();

     image_resized = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
     cvCopy(image,image_resized);

     long start, end, duration=0;
     while((img_height>=rect_height) && (img_width>=rect_width))
     {
	  m_extractor->Init(image_resized);

	  for(int i=0; i<=img_height-rect_height;i=i+4)
	  {
	       for(int j=0; j<=img_width-rect_width; j=j+4)
	       {
		    result = m_classifier->Test(m_extractor,i,j);
		    if(result == 1)
		    {
			 rect.x = j/ratio;
			 rect.y = i/ratio;
			 rect.width = rect_width/ratio;
			 rect.height = rect_height/ratio;
			 results.push_back(rect);
		    }
	       }
	  }
		
	  ratio = ratio*0.6;
	  img_height = ratio*img->height;
	  img_width = ratio*img->width;
	  cvReleaseImage(&image_resized);
	  image_resized = cvCreateImage(cvSize(img_width,img_height),IPL_DEPTH_8U,1);
	  cvResize(image,image_resized);
     }
     std::clog<<"time for test features: "<<double(duration)/CLOCKS_PER_SEC<<std::endl;

     delete []features;
//  PostProcess(results, 2);
//  PostProcess(results, 0);
}

//this function can only be called when classifier is boost or cascade type
void Detector::DrawFeatures(IplImage* image, int n)
{
     dynamic_cast<CascadeClassifier*>(m_classifier)->DrawFeatures(image, m_extractor, n);
}

void Detector::DrawResults(IplImage* image,  const std::vector<CvRect>& results)
{
     int height;
     int width;

     CvPoint pt1, pt2;

     height = image->height;
     width = image->width;

     for(int k=0;k<results.size();k++)
     {
	  pt1.y = (results[k].y>=0)?results[k].y:0; 
	  pt1.y = (results[k].y<height)?results[k].y:(height-1);
	  pt2.y = ((results[k].y+results[k].height-1)>=0)?(results[k].y+results[k].height-1):0;
	  pt2.y = ((results[k].y+results[k].height-1)<height)?(results[k].y+results[k].height-1):(height-1);
	  pt1.x = (results[k].x>=0)?results[k].x:0;
	  pt1.x = (results[k].x<width)?results[k].x:(width-1);
	  pt2.x = ((results[k].x+results[k].width-1)>=0)?(results[k].x+results[k].width-1):0;
	  pt2.x = ((results[k].x+results[k].width-1)<width)?(results[k].x+results[k].width-1):(width-1);	
	  cvDrawRect(image,pt1,pt2,CV_RGB(255,0,0));
     }
     return;
}

void Detector::ShowResults(IplImage* image,  const std::vector<CvRect>& results)
{

     DrawResults(image,results);

     cvNamedWindow("results",1);

     cvShowImage("results",image);
     cvWaitKey(0);
     cvDestroyWindow("results");
}

void Detector::Train(DataSet* trainset)
{
     m_classifier->Train(trainset);
}

void Detector::ReadFromFile(std::ifstream& ifile)
{
     char type[100];
     int height,width;
     ifile>>type>>width>>height;
     if(m_extractor == NULL)
     {
	  if(!strcmp(type, "HogFeatures"))
	       m_extractor = new HogFeatures(width,height);
	  else if(!strcmp(type, "EdgeletFeatures"))
	       m_extractor = new EdgeletFeatures(width,height);
	  else if(!strcmp(type, "HaarFeatures"))
	       m_extractor = new HaarFeatures(width,height);
	  else
	       ;
     }
     ifile>>type;
     if(m_classifier==NULL)
     {
	  if(!strcmp(type, "CascadeClassifier"))
	  {
	       m_classifier = new CascadeClassifier;
/*	       ifile>>type;
	       if(!strcmp(type, "DISCRETE"))
	       {
		    BoostClassifier::m_boost_type = DISCRETE;
	       }
	       else if(!strcmp(type, "REAL"))
	       {
		    BoostClassifier::m_boost_type = REAL;
	       }
	       else if(!strcmp(type, "LOGIT"))
	       {
		    BoostClassifier::m_boost_type = LOGIT;
	       }
	       else if(!strcmp(type,"GENTLE"))
	       {
		    BoostClassifier::m_boost_type = GENTLE;
	       }
	       else
	       {
		    ;
	       }
	       ifile>>type;
	       if(!strcmp(type,"SVM"))
	       {
		    BoostClassifier::m_weak_type = SVM;
	       }
	       else if(!strcmp(type, "CART"))
	       {
		    BoostClassifier::m_weak_type = CART;
	       }
	       else
	       {
		    ;
	       }
*/
	  }
	  else if(!strcmp(type, "SvmClassifier"))
	       m_classifier = new SvmClassifier;
	  else
	       ;
     }

     m_classifier->ReadFromFile(ifile);  
}

int Detector::Test(IplImage* img, int y, int x)
{
     m_extractor->Init(img);
     if(m_classifier->Test(m_extractor,y,x)==1)
	  return 1;
     else
	  return -1;
}

void Detector::WriteToFile(std::ofstream& ofile)
{
     if(typeid(*m_extractor) == typeid(HogFeatures))
	  ofile<<"HogFeatures";
     else if(typeid(*m_extractor) == typeid(EdgeletFeatures))
	  ofile<<"EdgeletFeatures";
     else if (typeid(*m_extractor) == typeid(HaarFeatures))
	  ofile<<"HaarFeatures";
     else
	  ;
     ofile<<" "<<m_extractor->GetWidth()<<" "<<m_extractor->GetHeight()<<std::endl;

     if(typeid(*m_classifier) == typeid(CascadeClassifier))
     {
	  ofile<<"CascadeClassifier";
/*	  switch(BoostClassifier::m_boost_type)
	  {
	  case DISCRETE:
	       ofile<<" DISCRETE"; break;
	  case REAL:
	       ofile<<" REAL"; break;
	  case LOGIT:
	       ofile<<" LOGIT"; break;
	  case GENTLE:
	       ofile<<" GENTLE"; break;
	  }
	  switch(BoostClassifier::m_weak_type)
	  {
	  case SVM:
	       ofile<<" SVM"; break;
	  case CART:
	       ofile<<" CART"; break;
	  }
*/
	  ofile<<std::endl;
     }
     else if(typeid(*m_classifier) == typeid(SvmClassifier))
	  ofile<<"SvmClassifier"<<std::endl;
     else
	  ;
     
     m_classifier->WriteToFile(ofile);
}
