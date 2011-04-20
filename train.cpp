/**
 * @file   train.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:11:26 2010
 * 
 * @brief  train a detector.
 * 
 * 
 */
#include "DataSet.h"
#include "Classifier.h"
#include "Features.h"
#include "HogFeatures.h"
#include "EdgeletFeatures.h"
#include "HaarFeatures.h"
#include "CartClassifier.h"
#include "SvmClassifier.h"
#include "BoostClassifier.h"
#include "CascadeClassifier.h"
#include "Detector.h"
#include <iostream>

//global varable definition
int verbose;
char dir_prefix[100];

void Read_Configure_File(char* filename, Detector* pDetector, DataSet* pDataSet)
{
     int width, height;
     float selected_feature_ratio;
     bool bcontinue = false;
     char train_images[100], bootstrap_images[100],test_images[100];
     Classifier* pClassifier;
     Features* pFeature;

     char		buf[256], item[256], val[100];
     std::clog << "Reading configuration file..."<<std::endl;
     std::ifstream	ifile;

     ifile.open(filename);
     if(!ifile.is_open())
     {
	  std::cerr<<"Input file \""<<filename<<"\" does not exist!"<<std::endl;
     }

     while (!ifile.eof())
     {
	  ifile.getline(buf, 256);	// skip the header, number of pictures

	  if (buf[0] == '-')
	  {
	       char	*st_pos = strchr (buf, ' ');
	       char	*ed_pos = strrchr (buf, ' ');
	       strncpy (item, buf, st_pos-buf);
	       item[st_pos-buf] = '\0';
	       strcpy (val, ed_pos+1);				

	       if(!strcmp(item, "-verbose"))
	       {
		    verbose = atoi(val);
	       } 
	       else if (!strcmp(item, "-width"))  //width of boudingbox of object
	       {
			 width = atoi(val);
	       }
	       else if (!strcmp(item, "-height")) //height of boudingbox of object
	       {
			 height = atoi(val);
	       }
	       else if (!strcmp (item, "-feature_type")) //initialize feature extractor
	       {
		    if(strcmp(val,"HoG")==0)
			 pFeature = new HogFeatures(width, height);
		    else if(strcmp(val,"Haar")==0)
			 pFeature = new HaarFeatures(width, height);
		    else if(strcmp(val,"Edgelet")==0)
			 pFeature = new EdgeletFeatures(width, height);
		    else
			 ;
		    pDetector->SetExtractor(pFeature);
		    pDataSet->SetExtractor(pFeature);
	       }
	       else if(!strcmp (item, "-classifier_type"))       //initialize classifier of detector
	       {
		    if(strcmp(val,"Cascade")==0)
			 pClassifier = new CascadeClassifier;
		    else
			 ;
		    pDetector->SetClassifier(pClassifier);
	       }
	       else if(!strcmp(item, "-selected_feature_ratio")) //set ratio of random feature selection
	       {
		    selected_feature_ratio = atof(val);
		    pDataSet->SetRandomFeatureSelectionRatio(selected_feature_ratio);
	       }
	       else if (!strcmp(item, "-work_directory"))
	       {
		    strcpy (dir_prefix, val);
		    char file_name[100];
		    std::ifstream	f1,f2,f3;
		    sprintf(file_name,"%s/train_samples.txt",dir_prefix);
		    f1.open(file_name);
		    sprintf(file_name,"%s/bootstrap_samples.txt",dir_prefix);
		    f2.open(file_name);
		    sprintf(file_name,"%s/test_samples.txt",dir_prefix);
		    f3.open(file_name);
		    if(f1.is_open() && f2.is_open() && f3.is_open())  
		    {
			 pDataSet->ReadFromFile(f1,f2,f3);	
		    }		    
	       }
	       else if(!strcmp(item, "-boost_type"))            //set boosting type
	       {
		    if(strcmp(val,"DISCRETE")==0)
			 BoostClassifier::m_boost_type = DISCRETE;
		    else if(strcmp(val,"REAL")==0)
			 BoostClassifier::m_boost_type = REAL;
		    else if(strcmp(val,"LOGIT")==0)
			 BoostClassifier::m_boost_type = LOGIT;
		    else if(strcmp(val,"GENTLE")==0)
			 BoostClassifier::m_boost_type = GENTLE;
		    else
			 ;
	       }
	       else if(!strcmp(item, "-weakclassifier_type"))   //Initialize weakclassifier pool
	       {
		    if(!strcmp(val,"CART"))
		    {
	 		BoostClassifier::InitWeakClassifierPool(pFeature, "CartClassifier", selected_feature_ratio);
			BoostClassifier::m_weak_type = CART;
		    }
		    else if(!strcmp(val,"SVM"))
		    {
			 BoostClassifier::InitWeakClassifierPool(pFeature, "SvmClassifier", selected_feature_ratio);
			 BoostClassifier::m_weak_type = SVM;
			 SvmClassifier::InitSvm(pDataSet);
		    }
	       }
	  }
     }
}


int main(int argc, char** argv)
{
     if(argc!=2)
     {
	  std::cerr<<"Configure file is needed to call train!"<<std::endl;
	  return 0;
     }
     char		filename[100];
     sprintf(filename,"%s/error.txt",dir_prefix);
     std::ofstream	errfile;
     errfile.open(filename);
     std::cerr.rdbuf(errfile.rdbuf());

     //define test_image, file_reader, feature_extrator, classifier and detector
     IplImage*		img;
     std::ifstream	ifile;
     Detector		dt;
     DataSet		ds;
     
     Read_Configure_File(argv[1], &dt, &ds);

     dt.Train(&ds);

     std::ofstream	f;
     sprintf(filename,"%s/Detector.txt",dir_prefix);
     f.open(filename,std::ios::out);
	
     dt.WriteToFile(f);

     return 1;
}
