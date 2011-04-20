/**
 * @file   face_train.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:10:09 2010
 * 
 * @brief  train face detector.
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

//#define CONTINUE_TRAIN

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
	DataSet ds;
	
	//load defult features
	char filename[100];
	//	sprintf(filename,"%s/haar_features.txt",dir_prefix);
	//ifile.open(filename);
	//hf.ReadFromFile(ifile);
	//ifile.close();

	//set the training dataset
	ds.SetExtractor(&hf);
	std::ifstream f1,f2,f3;
	sprintf(filename,"%s/train_samples.txt", dir_prefix);
	f1.open(filename);
	sprintf(filename,"%s/bootstrap_samples.txt", dir_prefix);
	f2.open(filename);
	sprintf(filename,"%s/test_samples.txt", dir_prefix);
	f3.open(filename);

	if(f1.is_open() && f2.is_open() && f3.is_open())
	{
	  ds.ReadFromFile(f1,f2,f3);	
	}

	BoostClassifier::InitWeakClassifierPool(hf,"CartClassifier",ds.m_selected_feature_ratio);

#ifdef CONTINUE_TRAIN
	//load already-calculated classifier
	sprintf(filename,"%s/cascadeclassifier.txt",dir_prefix);
	ifile.open(filename);
	if(ifile.is_open())
	{
		cc.ReadFromFile(ifile);
	}
#endif

	dt.SetClassifier(&cc);
	dt.SetExtractor(&hf);
	
	dt.train(&ds);

	std::ofstream f4;
	sprintf(filename,"%s/result.txt",dir_prefix);
	f3.open(filename,std::ios::out);
	cc.WriteToFile(f4);
	f1.close();
	f2.close();
	f3.close();
	f4.close();

	return 1;
}
