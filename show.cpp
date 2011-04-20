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

int verbose = 1;
char dir_prefix[100] = "./Dataset/data_human";

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
     if(argc!=4)
     {
	  std::cerr<<"./show Configure_file Image_file number"<<std::endl;
	  return 0;
     }
     char filename[100];
     IplImage* img;
     Detector dt;

     Read_Configure_File(argv[1], &dt);
     
     sprintf(filename, "%s/%s",dir_prefix,argv[2]);
     img = cvLoadImage(filename);

     dt.DrawFeatures(img,atoi(argv[3])); 

     sprintf(filename, "%s/%s_show.bmp",dir_prefix,argv[2]);
     cvSaveImage(filename,img);
}
