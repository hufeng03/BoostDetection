/**
 * @file   DataSet.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:08:19 2010
 * 
 * @brief  implementatio of DataSet
 * 
 * 
 */
#include "DataSet.h"
#include "highgui.h"
#include "Classifier.h"
#include "BoostClassifier.h"
#include <iostream>
#include<ctime>


extern char dir_prefix[];
extern int verbose;

DataSet::DataSet()
{
//	m_selected_feature_tmp = NULL;
	m_selected_feature_ratio = 0.05;
//	m_bootstrap_level = 0;
	//m_randomselect_flag = false;
}

DataSet::~DataSet()
{
     ;
}

void DataSet::Init()
{
	int i;
	try{
  m_weights = new float[m_sample_num];
  m_labels = new int[m_sample_num];

  m_test_labels = new int[m_test_num];
  m_test_filename = new char[m_test_num][100];

  m_bootstrap_filename = new char[m_bootstrap_filenum][100];
  m_bootstrap_fileused = new int[m_bootstrap_filenum];


//  m_selected_feature_tmp = new float[m_selected_feature_num];

  m_selected_feature_idx = new int[m_selected_feature_num];

  m_samples = new float*[m_sample_num];
  for(i=0;i<m_sample_num;i++)
  {
	  m_samples[i] = new float[m_selected_feature_num*m_feature_size];
  }

  m_samples_img = new char[m_sample_num][100];
  m_samples_desc = new char[m_sample_num][100];

  m_features_tmp = new float[m_feature_num*m_feature_size];

	}catch(std::bad_alloc &ba)
	{
	  std::cerr<<ba.what()<<"at the "<<i<<"th training sample!"<<std::endl;
	  exit(0);
	}
}

void DataSet::Clear()
{
  int i;
  for(i=0;i<m_sample_num;i++)
    {
      delete [](m_samples[i]);
    }
  delete []m_samples;


  delete []m_labels;
  delete []m_weights;
  delete []m_test_labels;
 
  
  delete []m_test_filename;
  delete []m_bootstrap_filename;
  delete []m_samples_img;
  delete []m_samples_desc;
  delete []m_features_tmp;
  delete []m_selected_feature_idx;
}

void DataSet::ReadFromFile(std::ifstream& f1, std::ifstream& f2, std::ifstream& f3)
{
	std::clog<<"Read Training Dataset from files ..."<<std::endl;
	IplImage* img;
  char filename[100];
  char imgname[100];
  char str[256];
  char seps[] = " ,\t\n";
  char* p = NULL;
  char params[3][10];
  int dx,dy;
  bool flag_only_positive_provided = true;
  int i,j,num;  

  f1.getline(str,256);
  p = strtok(str,seps);
  m_positive_num = atoi(p);
  m_negative_num = m_positive_num;
  num = m_positive_num;
  p = strtok(NULL,seps);
  while(p)
  {
    m_negative_num = atoi(p);
    flag_only_positive_provided = false;
    num += m_negative_num;
  }
  m_sample_num = m_positive_num + m_negative_num;
  f2>>m_bootstrap_filenum;
  f2.ignore(256,'\n');

  f3>>m_test_num>>m_test_dy>>m_test_dx;
  f3.ignore(256,'\n');
	
  m_feature_num = m_extractor->GetFeatureNum();
  m_feature_size = m_extractor->GetFeatureSize();
  m_selected_feature_num = m_selected_feature_ratio*m_feature_num;
  Init();

  for(i=0;i<m_bootstrap_filenum;i++)
    {
      f2>>m_bootstrap_filename[i];
      m_bootstrap_fileused[i] = 0;
    }

  for(i=0;i<m_test_num;i++)
    f3>>m_test_filename[i]>>m_test_labels[i];

  for(i=0;i<m_sample_num;i++)
  {
      sprintf(m_samples_desc[i],"%s/tmp/%d_desc.dat",dir_prefix,i);
      if(i<m_positive_num)
	   sprintf(m_samples_img[i],"%s/tmp/%d_pos.bmp",dir_prefix,i);
      else
	  sprintf(m_samples_img[i],"%s/tmp/%d_neg.bmp",dir_prefix,i);
  }	

	float tt=0.0f;
  for(i=0;i<num;i++)
    {
	if(float(i)/num>tt)
	{
		std::clog<<".";
		tt+=0.01;
	}
      f1.getline(str,256);
      p = strtok(str,seps);
      strcpy(imgname,p);
      j = 0;
      p = strtok(NULL,seps);
      while(p)
      {
	strcpy(params[j],p);
        j++;
      	p = strtok(NULL,seps);
      }
	
      if(j==1)
	m_labels[i] = atoi(params[0]);
      else if(j==3)
        m_labels[i] = atoi(params[2]);
      else
        m_labels[i] = 1;

      if(j==2 || j==3)
      {
        dy = atoi(params[0]);
	dx = atoi(params[1]);
      }
      else
      {
	dy = 0;
        dx = 0;
      }
	
        img= NULL;
	if(!(img = cvLoadImage(imgname,0)))
		std::clog<<"Cannot load "<<imgname<<std::endl;
	
	if(verbose!=0)
	{
	if(dx>0 || dy>0)
	{
	  cvSetImageROI(img,cvRect(dx,dy,m_extractor->GetWidth(),m_extractor->GetHeight()));
	}
	cvSaveImage(m_samples_img[i],img);

	cvResetImageROI(img);
	}

	m_extractor->Init(img);
	m_extractor->Extract(m_features_tmp,dy,dx);
	m_extractor->SaveToFile(m_samples_desc[i],m_features_tmp);
	
	cvReleaseImage(&img);
   }
  std::clog<<std::endl;

    if(flag_only_positive_provided)
    {
        for(i=m_positive_num;i<m_sample_num;i++)
		m_labels[i] = -1;
	RandomNegativeSamples(NULL);
    }
}

void DataSet::RandomSelect()
{
	std::clog<<"Random select "<<m_selected_feature_num<<"features ..."<<std::endl;
	//bool* selected = new bool[m_feature_num];
	CvRNG rng = cvRNG((unsigned)time(NULL));//±£Ö¤Ã¿ÂÖÉú³ÉµÄÎ±Ëæ»ú²»Ò»Ñù
	int i,j,k;

	i=0;
	while(i<m_selected_feature_num)
	{
		m_selected_feature_idx[i] = cvRandInt(&rng)%(m_feature_num);

		for(j=0;j<i;j++)
		{
			if(m_selected_feature_idx[j] == m_selected_feature_idx[i])
				break;
		}
		if(j==i)
		{
			//selected[m_selected_feature_idx[i]]=true;
//			std::clog<<m_selected_feature_idx[i]<<", ";
			i++;
		}
	}
//	std::clog<<std::endl;
//	float* fv = new float[m_feature_num*m_feature_size];
	float tt=0.0;
	for(i=0;i<m_sample_num;i++)
	{
		if(i>tt*m_sample_num)
		{
			std::clog<<".";
			tt+=0.01;
		}
//		IplImage* img;
//		img = cvLoadImage(m_samples_img[i],0);
//		m_extractor->Init(img);
//		m_extractor->Extract(m_samples[i],0,0,m_selected_feature_idx,m_selected_feature_num);;

		m_extractor->LoadFromFile(m_samples_desc[i],m_features_tmp);

		for(j=0;j<m_selected_feature_num;j++)
		{
			//for(k=0; k<m_feature_size;k++)
			//	m_samples[i][j*m_feature_size+k] = m_features_tmp[m_selected_feature_idx[j]*m_feature_size+k];
			memcpy(m_samples[i]+j*m_feature_size,m_features_tmp+m_selected_feature_idx[j]*m_feature_size,m_feature_size*sizeof(float));
		}

//		cvReleaseImage(&img);
	}
	std::clog<<std::endl;
//	delete []fv;
	//m_randomselect_flag = true;
}

void DataSet::InitializeWeights()
{
  int i;
  for(i=0; i<m_positive_num; i++)
    {
      m_weights[i] = 0.5/m_positive_num;
    }
  for(i=m_positive_num;i<m_sample_num;i++)
    {
      m_weights[i] = 0.5/m_negative_num;
    }
}

void DataSet::NormalizeWeights()
{
  float sum;
  int i;

  sum = 0.0;
  for(i=0;i<m_sample_num;i++) 
    {
      sum += m_weights[i];
    }
  for(i=0;i<m_sample_num;i++) 
    {
      m_weights[i] = m_weights[i]/sum;
    }
}

void DataSet::SetExtractor(Features* features)
{
  m_extractor = features;
}

float* DataSet::FetchSampleFeature(int i,int j)
{
 	return (m_samples[i]+j*m_feature_size);
}

float* DataSet::FetchSampleFeatures(int i)
{
	return m_samples[i];
}

float DataSet::CalcTrainTruePositiveRate(Classifier* classifier)
{
	int i;
	int detection_num = 0;
	for(i=0;i<m_positive_num;i++)
	{
	  m_extractor->LoadFromFile(m_samples_desc[i],m_features_tmp);
	  if(classifier->Test(m_features_tmp) == 1)
			detection_num++;
	}
	return (float)detection_num/m_positive_num;
}

float DataSet::CalcTestTruePositiveRate(Classifier* classifier)
{
  int i=0, hit=0, total=0;;
  IplImage* img;
  for(i=0;i<m_test_num;i++)
    {
      if(m_test_labels[i] == 1)
	{
	  img = cvLoadImage(m_test_filename[i],0);
	  m_extractor->Init(img);
	  cvReleaseImage(&img);
	  if(classifier->Test(m_extractor,m_test_dy,m_test_dx)==1)
	    {
	      hit++;
	    }
	  total++;
	}
    }
  return float(hit)/total;
}

float DataSet::CalcTestFalsePositiveRate(Classifier* classifier)
{
  int i=0,j,k, hit=0, total=0, w, h, rect_width, rect_height;
  IplImage* img;
  std::vector<CvRect> results;
  rect_width = m_extractor->GetWidth();
  rect_height = m_extractor->GetHeight();
  for(i=0;i<m_test_num;i++)
    {
      if(m_test_labels[i] == -1)
	{
	  img = cvLoadImage(m_test_filename[i],0);
	  m_extractor->Init(img);
	  w = img->width, h = img->height;
	  cvReleaseImage(&img);
	  if(w>rect_width && h>rect_height)
	  {
	    for(j=0;j<=h-rect_height;j+=5)
	      {
		for(k=0;k<=w-rect_width;k+=5)
		  {  
		    if(classifier->Test(m_extractor,j,k) == 1)
		      hit++;
		    total++;
		  }
	      }
	  }
	}
    }
  return float(hit)/total;  
}

float DataSet::CalcTrainFalsePositiveRate(Classifier* classifier)
{
	int i;
	int positive_num = 0;
	for(i=m_positive_num;i<m_sample_num;i++)
	{
	  m_extractor->LoadFromFile(m_samples_desc[i],m_features_tmp);
	  if(classifier->Test(m_features_tmp) == 1)
			positive_num++;
	}
	return (float)positive_num/m_negative_num;
}

//void DataSet::ValidateDetector(Classifier* classifier, float& d, float& f)
//{
//  int i, num1=0,num2=0;
//    for(i=0;i<m_sample_num;i++)
//    {
//		IplImage* img;
//		img = cvLoadImage(m_samples_img[i],0);
//		m_extractor->Init(img);
//		//m_extractor->Extract(m_features_tmp,0,0,BoostClassifier::m_feature_selected);
//		cvReleaseImage(&img);
//
//      if(classifier->Test(m_extractor,0,0) == 1)//false positive
//	  if(m_labels[i] == 1)
//	      num1++;
//	  else
//	    num2++;
//    }
//    d = float(num1)/m_positive_num;
//    f = float(num2)/m_negative_num;
//    //std::clog<<"Detection Rate"
//}

/*
void DataSet::TestForBoosting(char* filename, int& pointer, Classifier* classifier)
{
  IplImage* img = cvLoadImage(filename,0);
  IplImage* img_resized;
  int rect_width = m_extractor->GetWidth();
  int rect_height = m_extractor->GetHeight();
  if(img==NULL || img->width<rect_width || img->height<rect_height)
    {
      return;
    }
	
  int img_width = img->width;
  int img_height = img->height;

  img_resized = cvCreateImage(cvSize(img_width,img_height),IPL_DEPTH_8U,1);

  cvCopy(img,img_resized);

  int result;
  int size;
  float ratio = 1.0;
	

  while((img_height>rect_height+1) && (img_width>rect_width+1))
    {
      m_extractor->Init(img_resized);
      for(int i=0; i<img_height-rect_height;i+=m_bootstrap_increment[m_bootstrap_level])
	{
	  for(int j=0; j<img_width-rect_width; j+=m_bootstrap_increment[m_bootstrap_level])
	    {
		//  m_extractor->Extract(m_features_tmp, i,j,BoostClassifier::m_feature_selected);
	    //  result = classifier->Test(m_features_tmp);
			result = classifier->Test(m_extractor,i,j);
	      if(result == 1)//false positive
		  {
		    m_extractor->Extract(m_features_tmp,i,j);
		    m_extractor->SaveToFile(m_samples_desc[pointer],m_features_tmp);
		    
		  cvSetImageROI(img_resized,cvRect(j,i,rect_width,rect_height));
		  cvSaveImage(m_samples_img[pointer],img_resized);

		  //m_extractor->Extract(m_features_tmp,i,j);
		  //m_extractor->SaveToFile(m_samples_desc[pointer],m_features_tmp);

		  cvResetImageROI(img_resized);
		  pointer++;
		  if(pointer == m_sample_num)// all are set to false positives.
		  {
			  cvReleaseImage(&img);
			  cvReleaseImage(&img_resized);
			  return;
		  }
			}
	    }
	}
		
      ratio = ratio * m_bootstrap_resizeratio[m_bootstrap_level];
      img_height = cvFloor(img_height*ratio);
      img_width = cvFloor(img_width*ratio);
      cvReleaseImage(&img_resized);
      img_resized = cvCreateImage(cvSize(img_width,img_height),IPL_DEPTH_8U,1);
      cvResize(img,img_resized);
    }
  cvReleaseImage(&img);
  cvReleaseImage(&img_resized);
}
*/

int DataSet::RandomNegativeSamples(Classifier* classifier)
{
	bool flag;
	int i,j,width, height,x,y, ind;
	int total = 0;
	float resize_ratio;
	char filename[100];
  	int rect_width = m_extractor->GetWidth();
  	int rect_height = m_extractor->GetHeight();
	IplImage *img,*img_resized;
	CvRNG rng = cvRNG((unsigned)time(NULL));//±£Ö¤Ã¿ÂÖÉú³ÉµÄÎ±Ëæ»ú²»Ò»Ñù


	std::clog<<"Random Select Negative Samples from bootstrapping files";
	float tt=0.0f;
	for(i=m_positive_num;i<m_sample_num;i++)
	{
		if(float(i-m_positive_num)/m_negative_num>tt)
		{
			std::clog<<".";
			tt+=0.01;
		}
		flag = true;
		while(flag)
		{
			ind = cvRandInt(&rng)%m_bootstrap_filenum;	
			
			int e = cvRandInt(&rng)%31-20;
			resize_ratio = pow(float(1.05), float(e));
			img = NULL;
			
			if(!(img = cvLoadImage(m_bootstrap_filename[ind],0)))
			{
				continue;
			}
			width = resize_ratio*img->width;
			height = resize_ratio*img->height;
			if(width<rect_width || height<rect_height)
			{
				cvReleaseImage(&img);
				continue;
			}
			img_resized = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 1);
			cvResize(img,img_resized);	
			//std::clog<<"The "<<i<<"th sample is extracted from "<<resize_ratio<<" resized "<<ind<<"th image ("<<m_bootstrap_filename[ind]<<")'s ";
			m_extractor->Init(img_resized);
			j=0;
			while(j++<100)
			{
				if(width==rect_width)
					x=0;
				else
					x = cvRandInt(&rng)%(width-rect_width);
				if(height==rect_height)
					y=0;
				else 
					y = cvRandInt(&rng)%(height-rect_height); 
				total++;
				if(classifier==NULL || classifier->Test(m_extractor,y,x)==1)
				{
				        std::clog<<"The "<<i<<"th sample is extracted from "<<resize_ratio<<" resized "<<ind<<"th image ("<<m_bootstrap_filename[ind]<<")'s "<<y<<", "<<x<<" rectangle."<<std::endl;
		    			m_extractor->Extract(m_features_tmp,y,x);
		    			m_extractor->SaveToFile(m_samples_desc[i],m_features_tmp);
					if(verbose!=0)
					{		    
		  			cvSetImageROI(img_resized,cvRect(x,y,rect_width,rect_height));
		  			cvSaveImage(m_samples_img[i],img_resized);
					cvResetImageROI(img_resized);
					}
					flag = false;
					break;
				}
			}
			cvReleaseImage(&img_resized);
			cvReleaseImage(&img);
		}
	}
	std::clog<<std::endl;
	return total;
}

/*
bool DataSet::BoostingInputFiles(Classifier* classifier)
{
  int i, j, pointer, index;
  float val;
  char filename[100];

  pointer = m_positive_num;

//   for(i=m_positive_num;i<m_sample_num;i++)
//     {
// 		IplImage* img;
// 		img = cvLoadImage(m_samples_img[i],0);
// 		m_extractor->Init(img);
// 		//m_extractor->Extract(m_features_tmp,0,0,BoostClassifier::m_feature_selected);
// 		cvReleaseImage(&img);
// 
//       if(classifier->Test(m_extractor,0,0) == 1)//false positive
// 	{
// 	  if(pointer!=i)
// 	    {
// 		  strncpy(filename,m_samples_img[i],100);
// 		  strncpy(m_samples_img[i],m_samples_img[pointer],100);
// 		  strncpy(m_samples_img[pointer],filename,100);
// 
// 		  strncpy(filename,m_samples_desc[i],100);
// 		  strncpy(m_samples_desc[i],m_samples_desc[pointer],100);
// 		  strncpy(m_samples_desc[pointer],filename,100);
// 	    }
// 	  pointer++;
// 	  if(pointer==m_sample_num)
// 	    {
// 	      break;
// 	    }
// 	}
//     }

  std::clog<<"Find "<<pointer-m_positive_num<<" false positives in negative samples. We should replace samples (from"<<pointer<<"th to "<<m_sample_num-1<<"th) with false positives from bootstrapping images."<<std::endl;
  if(pointer == m_sample_num)//all are false positive
    {
		std::cerr<<"all negative samples are false positive!"<<std::endl;
      return true;
    }
  index = 0;
  while(pointer<m_sample_num)//not all are false positive
    {
      if(index == m_bootstrap_filenum)//used all bootstrapping image
	{
	  if(m_bootstrap_level == m_bootstrap_maxlevel-1)//used all bootstrapping level
	    {
	      //return false;
	      m_bootstrap_level = -1;
	    }
	  //else
	   // {
	      m_bootstrap_level++;//next level of bootstrapping
	      for(i=0;i<m_bootstrap_filenum;i++)
		{
		  m_bootstrap_fileused[i] = 0;//all bootstrapping images are set to unused
		}
	      index = 0;//start from the 0th bootstrapping image
	      pointer = m_positive_num;//replace all false positives in training samples
  		std::clog<<"go to the next bootstrap level: "<<m_bootstrap_level<<". We replace all negative samples (from"<<pointer<<"th to "<<m_sample_num-1<<"th) with false positives from bootstrapping images."<<std::endl;
	   // }
	}
      if(m_bootstrap_fileused[index]==1)
	{
	  index++;
	  continue;
	}
	  int tmp = pointer;
      TestForBoosting(m_bootstrap_filename[index], pointer, classifier);
      m_bootstrap_fileused[index]=1;
	  if(pointer-tmp!=0)
		  std::clog<<pointer-tmp<<" true negative samples("<<tmp<<"~"<<pointer-1<<") are replaced by false positives found in the "<<index<<"th bootstrapping images:"<<m_bootstrap_filename[index]<<std::endl;
      index++;
    }

  return true;
}
*/
