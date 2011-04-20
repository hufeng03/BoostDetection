/**
 * @file   SimpleClassifier.h
 * @author feng <huf07@mails.tsinghua.edu.cn>
 * @date   Tue Sep  9 10:58:12 2008
 * 
 * @brief  header file of SimpleClassifier.cpp
 * 
 * 
 */

#ifndef _simpleclassifier_h_
#define _simpleclassifier_h_

#include "WeakClassifier.h"
#include "DataSet.h"

class SimpleClassifier : public WeakClassifier {
 public:
	 SimpleClassifier();
  SimpleClassifier(Features& F);
  ~SimpleClassifier();
  void Train(DataSet* trainset);
  int Test(float* fv);
  int Test(Features*,int y,int x);
  int Test_fast(float*);
  float GetValue_fast(float*);
  void WriteToFile(std::ofstream& f) const;
  void ReadFromFile(std::ifstream& f);
  void Clear();
  SimpleClassifier& operator = (const SimpleClassifier &);


  float GetValue(float* fv);

  

 private:

  float m_thresh;
  int m_parity;
};

#endif
