/**
 * @file   CascadeClassifier.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:07:03 2010
 * 
 * @brief  implementation of CascadeClassifier
 * 
 * 
 */
#include "CascadeClassifier.h"
#include "Features.h"
#include <highgui.h>
#include <iostream>

//#define CONTINUE_TRAIN

extern char dir_prefix[];


CascadeClassifier::CascadeClassifier()
{
	m_count = 0;
	m_falsepositive_goal = 0.000001;  //training goal
	m_ac_falsepositive_goal = 0.7;
	m_ac_detection_goal = 0.9975;
}

CascadeClassifier::~CascadeClassifier()
{
  ;
}

int CascadeClassifier::Test(float* fv)
{
	int i;
  for(i=0; i<m_count; i++)
    {
      if(m_acs[i]->Test(fv)==-1)
	return -1;
    }
  return 1;
}

int CascadeClassifier::Test(Features* extractor,int y,int x)
{
	int i;
	for(i=0; i<m_count; i++)
	{
		if(m_acs[i]->Test(extractor,y,x)==-1)
			return -1;
	}
	return 1;
}

void CascadeClassifier::Clear()
{
  if(m_count != 0)
    {
      for(int i=0;i<m_count;i++)
	{
	  m_acs[i]->Clear();
	}
//       delete[] m_acs;
      //       m_acs = NULL;
    }
  m_count = 0;
}

void CascadeClassifier::ReadFromFile(std::ifstream& f)
{
	int i;
	BoostClassifier* ac;
  Clear();
  f>>m_count;
  f.ignore(256,'\n');
  m_acs.clear();
  try{
  for(i=0;i<m_count;i++)
  {
	  ac = new BoostClassifier;
	  m_acs.push_back(ac);
  }
  }catch(std::bad_alloc &ba)
  {
	  std::cerr<<ba.what()<<"at the "<<i<<" CascadeClassifier ReadFromFile!"<<std::endl;
	  exit(0);
  }
  for(i=0;i<m_count;i++)
    {
      m_acs[i]->ReadFromFile(f);
	  m_false_positive *= m_ac_falsepositive_goal;
	  m_true_positive *= m_ac_detection_goal;
    }
}

void CascadeClassifier::WriteToFile(std::ofstream& f) const
{
  f<<m_count<<std::endl;
  for(int i=0;i<m_count;i++)
    {
      m_acs[i]->WriteToFile(f);
      f<<std::endl;
    }
}

void CascadeClassifier::Train(DataSet* trainset)
{
  int i = 0;
  bool result;
  char filename[100];
  sprintf(filename,"%s/CascadeClassifier.txt",dir_prefix);

#ifdef CONTINUE_TRAIN
  std::clog<<"Read existing cascade classifier and then continue train."<<std::endl;
  std::ifstream ifile;
  ifile.open(filename,std::ios::in);
  ReadFromFile(ifile);
  ifile.close();
#endif
  
  std::clog<<"Start to Train Cascade..."<<std::endl;

  while(1)
  {
	if(m_count!=0)
	{  //bootstrapping
	  std::clog<<"Start to bootstrapping..."<<std::endl;
	  int tmp = trainset->RandomNegativeSamples(this);
	  std::clog<<"End with test false positive rate:"<<float(trainset->m_negative_num)/tmp<<std::endl;
	}
	  //for log
	  std::clog<<"Start to Train the "<<m_count<<"th Boost Classifier..."<<std::endl;
          AddOneBoostClassifier(m_count, trainset);

	  std::clog<<"End with: ";
	  m_false_positive *= m_acs[m_count-1]->GetTrainResult();
	  m_true_positive *= m_ac_detection_goal;
	  std::clog<<"Train detection rate = "<<m_true_positive<<"("<<trainset->CalcTrainTruePositiveRate(this)<<"), false positive rate = "<<m_false_positive<<std::endl;

	  std::clog<<"Test detection rate = "<<trainset->CalcTestTruePositiveRate(this)<<", false positive rate = "<<trainset->CalcTestFalsePositiveRate(this)<<std::endl;

	  //save the temporal result
	  std::ofstream f;
      	  f.open(filename,std::ios::out);
          WriteToFile(f);
      	  f.close();
	
	if(m_false_positive<m_falsepositive_goal)
		break;
	
   }
   std::clog<<"Congratulation. The training goal is reached!"<<std::endl;
}




void CascadeClassifier::AddOneBoostClassifier(int i, DataSet* trainset)
{
	BoostClassifier* ac;
	try{
		ac = new BoostClassifier;
		m_acs.push_back(ac);
	}
	catch(std::bad_alloc &ba)
	{
		std::cerr<<ba.what()<<"at the CascadeClassifier AddOneBoostClassifier!"<<std::endl;
		exit(0);
	}
  m_acs[i]->SetTrainGoal(m_ac_falsepositive_goal,m_ac_detection_goal);

  m_acs[i]->Train(trainset);

  m_count++;

}

void CascadeClassifier::DrawFeatures(IplImage* img, Features* extractor)
{
     for(int i=0;i<m_count;i++)
     {
	  m_acs[i]->DrawFeatures(img, extractor);
     }
}

void CascadeClassifier::DrawFeatures(IplImage* img, Features* extractor, int n)
{
     int num = 0, i = 0, j;
     while( num < n && i < m_count)
     {
	  j = n-num > m_acs[i]->GetWeakClassifierNumber()?m_acs[i]->GetWeakClassifierNumber():n-num;
	  m_acs[i]->DrawFeatures(img, extractor, j);
	  num += j;
	  i++;
     }
}

