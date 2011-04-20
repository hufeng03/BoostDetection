/**
 * @file   car_test.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:17:42 2010
 * 
 * @brief  test car detector.
 * 
 * 
 */
#include "DataSet.h"
#include "Classifier.h"
#include "Features.h"
#include "EdgeletFeatures.h"
#include "HaarFeatures.h"
#include "SimpleClassifier.h"
#include "SvmClassifier.h"
#include "BoostClassifier.h"
#include "CascadeClassifier.h"
#include "Detector.h"
#include <iostream>

#define WIDTH 50
#define HEIGHT 20

char dir_prefix[] = "./Dataset/data_car";
int verbose = 0;

int main()
{
  char filename[100], imagename[100];
  //define test_image, file_reader, feature_extrator, classifier and detector
  IplImage* img;
  std::ifstream ifile;
  EdgeletFeatures hf(WIDTH,HEIGHT);
  CascadeClassifier cc;
  Detector dt;

  //load defult features
  // ifile.open("./data/haar_features.txt");
  //hf.ReadFromFile(ifile);
  //ifile.close();

  //load default classifier
  sprintf(filename,"%s/cascadeclassifier.txt",dir_prefix);
  ifile.open(filename);
  cc.ReadFromFile(ifile);

  dt.SetClassifier(&cc);
  dt.SetExtractor(&hf);

  std::vector<CvRect> results;
  std::ifstream f1;
  int num,i;

  //general test
  /**/
  sprintf(filename,"%s/detect_samples.txt",dir_prefix);
  f1.open(filename);
  f1>>num;
  f1.ignore(256,'\n');
  for(i=0;i<num;i++)
  {
    //read image
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
