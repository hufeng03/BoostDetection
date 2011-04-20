/**
 * @file   SvmClassifier.h
 * @author feng <huf07@mails.tsinghua.edu.cn>
 * @date   Tue Sep  9 10:58:12 2008
 * 
 * @brief  header file of SvmClassifier.cpp
 * 
 * 
 */

#ifndef _svmclassifier_h_
#define _svmclassifier_h_

#include "WeakClassifier.h"
#include "DataSet.h"

extern "C" {
#include "./liblinear-weights-1.51/linear.h"
}

class SvmClassifier : public WeakClassifier {
 public:
  SvmClassifier();
  SvmClassifier(Features* F);
  ~SvmClassifier();
  void Train(DataSet* trainset);
  int Test(float* fv);
  float GetValue(float* fv);
  int Test(Features*,int y,int x);
  int Test_fast(float*);
  float GetValue_fast(float*);
  void WriteToFile(std::ofstream& f) const;
  void ReadFromFile(std::ifstream& f);
  void Clear();
  SvmClassifier& operator=(const SvmClassifier&);

  void GetBestPenaltyByCrossValidation(DataSet* trainset);

  static int* m_target;
  static parameter* m_param;
  static problem* m_prob;
  static float* m_f;
  static feature_node *m_x;
  static float* m_v;
  static void InitSvm(DataSet* trainset);

private:
  struct model *m_model;
  float m_thresh;
  float m_a;
  float m_b;
   // int m_parity;
};

#endif
