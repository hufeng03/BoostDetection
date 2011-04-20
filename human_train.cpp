/**
 * @file   human_train.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:11:26 2010
 * 
 * @brief  train human detector.
 * 
 * 
 */
#include "DataSet.h"
#include "Classifier.h"
#include "Features.h"
#include "HogFeatures.h"
#include "SimpleClassifier.h"
#include "SvmClassifier.h"
#include "BoostClassifier.h"
#include "CascadeClassifier.h"
#include "Detector.h"
#include <iostream>

#define WIDTH 64
#define HEIGHT 128

//#define CONTINUE_TRAIN

//global varable definition

char dir_prefix[] = "./Dataset/data_human";
int verbose = 0;


int main()
{
  char filename[100];
//   sprintf(filename,"%s/log.txt",dir_prefix);
//   std::ofstream logfile;
//   logfile.open(filename);
//   std::clog.rdbuf(logfile.rdbuf());

  sprintf(filename,"%s/error.txt",dir_prefix);
  std::ofstream errfile;
  errfile.open(filename);
  std::cerr.rdbuf(errfile.rdbuf());

	//define test_image, file_reader, feature_extrator, classifier and detector
	IplImage* img;
	std::ifstream ifile;
	HogFeatures hgf(WIDTH,HEIGHT);
	CascadeClassifier cc;
	Detector dt;
	DataSet ds;

	//set the training dataset
	ds.SetExtractor(&hgf);
	std::ifstream f1,f2,f3;
	sprintf(filename,"%s/train_samples.txt",dir_prefix);
	f1.open(filename);
	sprintf(filename,"%s/bootstrap_samples.txt",dir_prefix);
	f2.open(filename);
	sprintf(filename,"%s/test_samples.txt",dir_prefix);
	f3.open(filename);	
	
	//load already-calculated classifier
	sprintf(filename,"%s/cascadeclassifier.txt",dir_prefix);
	ifile.open(filename);
	if(ifile.is_open())
	{
		std::clog<<"Read already existed cascade classifier"<<std::endl;
		cc.ReadFromFile(ifile);	
	}

	
	if(f1.is_open() && f2.is_open() && f3.is_open())  
	{
	  ds.ReadFromFile(f1,f2,f3);	
	}

	//initialize adaboost
	SvmClassifier::InitSvm(&ds);
	BoostClassifier::InitWeakClassifierPool(hgf,"SvmClassifier",ds.m_selected_feature_ratio);

	if(ifile.is_open())
	{
		float	d = ds.CalcTrainTruePositiveRate(&cc);
		float   f = ds.CalcTrainFalsePositiveRate(&cc);
		cc.SetDetectionRate(d);
		cc.SetFalsePositiveRate(f);
		std::clog<<"Continue Training with detection rate = "<<d<<", false positve rate = "<<f<<"."<<std::endl;
	}


	dt.SetClassifier(&cc);
	dt.SetExtractor(&hgf);

	//	dt.SetClassifier(&cc);
	//dt.SetExtractor(&hgf);

	dt.train(&ds);

	std::ofstream f4;
	sprintf(filename,"%s/result.txt",dir_prefix);
	f4.open(filename,std::ios::out);
	cc.WriteToFile(f4);
	f1.close();
	f2.close();
	f3.close();
	f4.close();

	return 1;
}
