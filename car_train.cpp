/**
 * @file   car_train.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:17:18 2010
 * 
 * @brief  train car detector.
 * 
 * 
 */
#include "DataSet.h"
#include "Classifier.h"
#include "Features.h"
#include "EdgeletFeatures.h"
#include "CartClassifier.h"
#include "SvmClassifier.h"
#include "BoostClassifier.h"
#include "CascadeClassifier.h"
#include "Detector.h"
#include <iostream>

#define WIDTH 50
#define HEIGHT 20

char dir_prefix[] = "./Dataset/data_car";
int verbose = 1;

int main()
{
	//define test_image, file_reader, feature_extrator, classifier and detector
	IplImage* img;
	std::ifstream ifile;
	EdgeletFeatures ef(WIDTH,HEIGHT);
	CascadeClassifier cc;
	Detector dt;
	DataSet ds;

	//set the training dataset
	ds.SetExtractor(&ef);
	std::ifstream f1,f2,f3;
	char filename[100];
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

	if(ifile.is_open())
	{
		float	d = ds.CalcTrainTruePositiveRate(&cc);
		float   f = ds.CalcTrainFalsePositiveRate(&cc);
		cc.SetDetectionRate(d);
		cc.SetFalsePositiveRate(f);
		std::clog<<"Continue Training with detection rate = "<<d<<", false positve rate = "<<f<<"."<<std::endl;
	}

	BoostClassifier::InitWeakClassifierPool(ef,"CartClassifier",ds.m_selected_feature_ratio);

	dt.SetClassifier(&cc);
	dt.SetExtractor(&ef);

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
