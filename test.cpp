#include "DataSet.h"
#include "Classifier.h"
#include "Features.h"
#include "EdgeletFeatures.h"
#include "HaarFeatures.h"
#include "HogFeatures.h"
#include "CartClassifier.h"
#include "SvmClassifier.h"
#include "BoostClassifier.h"
#include "CascadeClassifier.h"
#include "Detector.h"
#include <iostream>
#include <time.h>

int verbose = 1;
char dir_prefix[100] = "./Dataset/data_face";


void Read_Configure_File(char* filename, Detector* pDetector)
{
     char buf[256], item[256], val[100];
     std::clog << "Reading configuration file ..."<<std::endl;
     std::ifstream ifile;

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
	       else if(!strcmp(item, "-work_directory"))
	       {
		    strcpy(dir_prefix,val);
		    char file_name[100];
		    std::ifstream f;
		    sprintf(file_name,"%s/Detector.txt",dir_prefix);
		    f.open(file_name,std::ios::in);
		    if(f.is_open())
			 pDetector->ReadFromFile(f);
		    f.close();
	       }
	  }
     }
}

int main(int argc, char** argv)
{
     if(argc!=2)
     {
	  std::cerr<<"Configure file is needed to call test!"<<std::endl;
	  return 0;
     }
     char filename[100],imagename[100];
     IplImage* img;
     std::ifstream ifile;
     Detector dt;

     Read_Configure_File(argv[1], &dt);

     std::vector<CvRect> results;
     std::ifstream f1;
     int num,i;

     //general test
     sprintf(filename,"%s/detect_samples.txt",dir_prefix);
     f1.open(filename);
     f1>>num;
     f1.ignore(256,'\n');
     for(i=0;i<num;i++)
     {
	  //read image
	  f1>>imagename;
	  img = cvLoadImage(imagename,0);
	  std::vector<CvRect> results;
	  std::clog<<"Checking the "<<i<<" st image: "<<imagename<<std::endl;
	  long start = clock();
	  dt.Check(img,results);
	  long end = clock();
	  std::clog<<"use time "<<(double)(end-start)/CLOCKS_PER_SEC<<" s"<<std::endl;
	  cvReleaseImage(&img);
	  img = cvLoadImage(imagename);
	  dt.DrawResults(img,results);
	  sprintf(filename,"%s/results/result_%d.bmp",dir_prefix,i);
	  cvSaveImage(filename,img);
	  cvReleaseImage(&img);
     }
     f1.close();

     //test for tyrue positive rate
     sprintf(filename, "%s/test_true_positive.txt",dir_prefix);
     f1.open(filename);
     f1>>num;
     f1.ignore(256,'\n');
     int hit = 0 ;
     for(i=0;i<num;i++)
     {
	  f1>>imagename;
	  img = cvLoadImage(imagename,0);
	  std::vector<CvRect> results;
	  if(dt.Test(img,0,0)==1)
	  {
	       hit++;
	  }
	  cvReleaseImage(&img);
     } 
     std::clog<<"True positive rate is "<<float(hit)/num<<" ("<<hit<<"/"<<num<<") "<<std::endl;
     f1.close();

     //test for false positive rate
     sprintf(filename,"%s/test_false_positive.txt",dir_prefix);
     f1.open(filename);
     int falsehit=0, scanwindow=0;
     f1>>num;
     f1.ignore(256,'\n');
     for(i=0;i<num;i++)
     {
	  //read image
	  f1>>imagename;
	  img = cvLoadImage(imagename,0);
	  std::vector<CvRect> results;
	  dt.Check(img,results);
	  cvReleaseImage(&img);
	  img = cvLoadImage(imagename);
	  dt.DrawResults(img,results);
	  sprintf(filename,"%s/tmp/result_%d.bmp",dir_prefix,i);
	  cvSaveImage(filename,img);
	  falsehit += results.size();
	  int w = img->width, h = img->height;
	  int rect_width = dt.GetExtractor()->GetWidth();
	  int rect_height = dt.GetExtractor()->GetHeight();
	  while(w>=rect_width && h>=rect_height)
	  {
	       for(int j=0;j<=h-rect_height;j=j+2)
		    for(int k=0;k<=w-rect_width;k=k+2)
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
