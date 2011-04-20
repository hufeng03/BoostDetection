
/**
 * @file   human_test.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:10:58 2010
 * 
 * @brief  test human detector.
 * 
 * 
 */
#include "DataSet.h"
#include "Classifier.h"
#include "Features.h"
#include "EdgeletFeatures.h"
#include "HaarFeatures.h"
#include "HogFeatures.h"
#include "SimpleClassifier.h"
#include "SvmClassifier.h"
#include "BoostClassifier.h"
#include "CascadeClassifier.h"
#include "Detector.h"
#include <iostream>


#define WIDTH 64
#define HEIGHT 128

char dir_prefix[] = "./Dataset/data_human";
int verbose = 0;

int main()
{
  char filename[100],imagename[100];
//   sprintf(filename,"%s/log.txt",dir_prefix);
//   std::ofstream logfile;
//   logfile.open(filename);
//   std::clog.rdbuf(logfile.rdbuf());
// 
//  
//   sprintf(filename,"%s/error.txt",dir_prefix);
//   std::ofstream errfile;
//   errfile.open(filename);
//   std::cerr.rdbuf(errfile.rdbuf());

  //define test_image, file_reader, feature_extrator, classifier and detector
  IplImage* img;
  std::ifstream ifile;
  HogFeatures hgf(WIDTH,HEIGHT);
  CascadeClassifier cc;
  Detector dt;

  SvmClassifier::InitSvm(&hgf);

  //load default classifier
  sprintf(filename,"%s/cascadeclassifier.txt",dir_prefix);
  ifile.open(filename);
  cc.ReadFromFile(ifile);

  dt.SetClassifier(&cc);
  dt.SetExtractor(&hgf);


  std::vector<CvRect> results;
  std::ifstream f1;
  int num,i;

//general test
/*  sprintf(filename,"%s/test_samples.txt",dir_prefix);
  f1.open(filename);
  int num,i;
  f1>>num;
  f1.ignore(256,'\n');
  for(i=0;i<num;i++)
  {
	  //read image
	  f1>>imagename;
//	  sprintf(filename,"%s%s",dir_prefix,imagename);
	  img = cvLoadImage(imagename);
	  std::vector<CvRect> results;
	  std::clog<<"Checking the "<<i<<"st image: "<<imagename<<std::endl;
	  dt.Check(img,results);
	  dt.DrawResults(img,results);
	  sprintf(filename,"%s/result_%d.bmp",dir_prefix,i);
	  cvSaveImage(filename,img);
	  cvReleaseImage(&img);
  }
  f1.close();*/
//test for detection rate
  sprintf(filename, "%s/test_detection_rate.txt",dir_prefix);
  f1.open(filename);
  f1>>num;
  f1.ignore(256,'\n');
  int hit = 0 ;
  for(i=0;i<num;i++)
  {
	f1>>imagename;
	img = cvLoadImage(imagename,0);
	//std::vector<CvRect> results;
	//std::clog<<"Checking the "<<i<<"st image:: "<<imagename<<std::endl;
	
	//dt.Check(img,results);
	hgf.Init(img);
        if(cc.Test(&hgf,3,3)==1)
	{
	  hit++;
	}
	sprintf(filename,"%s/result_%d_xx.bmp",dir_prefix,i);
	cvSaveImage(filename,img);
	cvReleaseImage(&img);
  } 
  std::clog<<"Detection rate is "<<float(hit)/num<<" ("<<hit<<"/"<<num<<") "<<std::endl;
  f1.close();

//test for false positive rate
  sprintf(filename,"%s/test_falsepositive_rate.txt",dir_prefix);
  f1.open(filename);
  long falsehit=0, scanwindow=0;
  f1>>num;
  f1.ignore(256,'\n');
  for(i=0;i<num;i++)
  {
	  //read image
	  f1>>imagename;
//	  sprintf(filename,"%s%s",dir_prefix,imagename);
	  img = cvLoadImage(imagename,0);
	  std::vector<CvRect> results;
//	  std::clog<<"Checking the "<<i<<"st image: "<<imagename<<std::endl;
	  dt.Check(img,results);
	  dt.DrawResults(img,results);
	  sprintf(filename,"%s/result_%d.bmp",dir_prefix,i);
	  cvSaveImage(filename,img);
//	  cvReleaseImage(&img);
	  falsehit += results.size();
	  int w = img->width, h = img->height;
	  while(w>=WIDTH && h>=HEIGHT)
	  {
	    for(int j=0;j<=h-HEIGHT;j=j+2)
	      for(int k=0;k<=w-WIDTH;k=k+2)
		scanwindow++;
	    w = w*0.8;
	    h = h*0.8;
	  }
	cvReleaseImage(&img);
  }
  std::clog<<"False positive rate is "<<float(falsehit)/scanwindow<<" ("<<falsehit<<"/"<<scanwindow<<") "<<std::endl;
  f1.close();

  return 1;
}
