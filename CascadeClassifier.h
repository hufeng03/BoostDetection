/**
 * @file   CascadeClassifier.h
 * @author feng <huf07@mails.tsinghua.edu.cn>
 * @date   Tue Sep  9 10:58:12 2008
 * 
 * @brief  header file of CascadeClassifier.cpp
 * 
 * 
 */

#ifndef _cascadeclassifier_h_
#define _cascadeclassifier_h_

#include "Classifier.h"
#include "BoostClassifier.h"
#include "Features.h"

class CascadeClassifier : public Classifier{
 public:
	 CascadeClassifier();
  ~CascadeClassifier();
  void Train(DataSet* trainset);
  int Test(float* fv);
  int Test(Features*,int y,int x);
  void WriteToFile(std::ofstream& f) const;
  void ReadFromFile(std::ifstream& f);
  void Clear();

  void AddOneBoostClassifier(int i, DataSet* trainset);

  void DrawFeatures(IplImage*, Features*, int);
  void DrawFeatures(IplImage*, Features*);

 private:
  std::vector<BoostClassifier*> m_acs;
  int m_count;
  //for training
  float m_falsepositive_goal;
  float m_ac_falsepositive_goal;
  float m_ac_detection_goal;
};

#endif
