/**
 * @file   main.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:12:06 2010
 * 
 * @brief  main 
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


extern int car_test();
extern int car_train();
extern int face_test();
extern int face_train();
extern int human_test();
extern int human_train();

//global varable definition
std::vector<WeakClassifier*> BoostClassifier::m_wc_pool;
bool* BoostClassifier::m_feature_selected = NULL;

parameter* SvmClassifier::m_param = NULL;
problem* SvmClassifier::m_prob = NULL;
char dir_prefix[] = "./Dataset/data_face";

int main ()
{
	//重定向程序记录输出
		char filename[100];
// 	 	sprintf(filename,"%s/log.txt",dir_prefix);
// 	 	std::ofstream logfile;
// 	 	logfile.open(filename);
// 	 	std::clog.rdbuf(logfile.rdbuf());

// 
// 		sprintf(filename,"%s/error.txt",dir_prefix);
// 		std::ofstream errfile;
// 		errfile.open(filename);
// 		std::cerr.rdbuf(errfile.rdbuf());



//	car_train();
//	human_train();
//	face_train();
	face_test();
	//	human_train();
	return 1;
}
