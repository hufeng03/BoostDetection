/**
 * @file   Classifier.h
 * @author feng <huf07@mails.tsinghua.edu.cn>
 * @date   Tue Sep  9 10:58:12 2008
 * 
 * @brief  header file of Classifier.cpp
 * 
 * 
 */

#ifndef _classifier_h_
#define _classifier_h_

#include <fstream>

class DataSet;
class Features;

class Classifier {
 public:
  Classifier();
//  Classifier(Features& F);
  virtual ~Classifier();
  virtual int Test(float* fv) = 0;
  virtual int Test(Features*,int y,int x) = 0;
  virtual void Train(DataSet* trainset) = 0;
  virtual void WriteToFile(std::ofstream& f) const = 0;
  virtual void ReadFromFile(std::ifstream& f) = 0;
//  virtual void SetFeatureSize(int size){m_feature_size = size;};
  virtual void Clear() = 0;
  void QuickSort(float* values,int* labels,const int lo,const int hi);

  void SetTruePositiveRate(float fl){m_true_positive = fl;};
  void SetFalsePositiveRate(float fl){m_false_positive = fl;};
  float GetTurePositiveRate(){return m_true_positive;};
  float GetFalsePositiveRate(){return m_false_positive;};

protected:
  float m_true_positive;
  float m_false_positive;
};

#endif
