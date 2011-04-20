/**
 * @file   face_test.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:09:19 2010
 * 
 * @brief  test face detection
 * 
 * 
 */
#include "DataSet.h"
#include "Classifier.h"
#include "Features.h"
#include "EdgeletFeatures.h"
#include "HaarFeatures.h"
#include "CartClassifier.h"
#include "SvmClassifier.h"
#include "BoostClassifier.h"
#include "CascadeClassifier.h"
#include "Detector.h"
#include <iostream>

#define WIDTH 24
#define HEIGHT 24

//global varable definition
char dir_prefix[] = "./Dataset/data_face";
int verbose = 1;

int main()
{
  //define test_image, file_reader, feature_extrator, classifier and detector
  IplImage* img;
  std::ifstream ifile;
  HaarFeatures hf(WIDTH,HEIGHT);
  CascadeClassifier cc;
  Detector dt;

  //load defult features
  char filename[100],imagename[100];
	
  //load default classifier
  sprintf(filename,"%s/cascadeclassifier.txt",dir_prefix);
  ifile.open(filename);
  cc.ReadFromFile(ifile);

  dt.SetClassifier(&cc);
  dt.SetExtractor(&hf);

  std::vector<CvRect> results;
  std::ifstream f1;
  sprintf(filename,"%s/detect_samples.txt",dir_prefix);
  f1.open(filename);
  
  int num,i;
  f1>>num;
  f1.ignore(256,'\n');
  for(i=0;i<num;i++)
  {
    f1>>imagename;
    img = cvLoadImage(imagename);
    std::vector<CvRect> results;
    std::clog<<"Checking the "<<i<<"st image: "<<imagename<<std::endl;
    dt.Check(img,results);
    dt.DrawResults(img,results);
    sprintf(filename,"%s/result_%d.bmp",dir_prefix,i);
    cvSaveImage(filename,img);
    cvReleaseImage(&img);
  }
  f1.close();

  return 1;
}
