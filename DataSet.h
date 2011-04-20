/**
 * @file   DataSet.h
 * @author feng <huf07@mails.tsinghua.edu.cn>
 * @date   Tue Sep  9 10:58:12 2008
 * 
 * @brief  header file of DataSet.cpp
 * 
 * 
 */

#ifndef _dataset_h_
#define _dataset_h_

#include <vector>
#include "Features.h"

class Classifier;

class DataSet{
 public:
  DataSet();
  ~DataSet();

  void Init();
  void Clear();
  void NormalizeWeights();
  void InitializeWeights();

  void ReadFromFile(std::ifstream& f1, std::ifstream& f2, std::ifstream& f3);
	
  void SetExtractor(Features* features);

//  bool BoostingInputFiles(Classifier* classifier);
//  void TestForBoosting(char* filename, int& pointer, Classifier* classifier);
  float CalcTrainTruePositiveRate(Classifier* classifier);
  float CalcTrainFalsePositiveRate(Classifier* classifier);
  float CalcTestTruePositiveRate(Classifier* classifier);
  float CalcTestFalsePositiveRate(Classifier*classifier);

  float* FetchSampleFeature(int,int);

  float* FetchSampleFeatures(int i);

  void RandomSelect();

  int RandomNegativeSamples(Classifier* classifier);

  void SetRandomFeatureSelectionRatio(float r){m_selected_feature_ratio = r;}

  //  void ValidateDetector(Classifier* classifier, float& d, float& f);

 // bool m_randomselect_flag;


  float** m_samples;
  char (*m_samples_img)[100];
  char (*m_samples_desc)[100];
  char (*m_bootstrap_filename)[100];

  int* m_labels;

  int* m_test_labels;
  int m_test_num, m_test_dy, m_test_dx;
  char (*m_test_filename)[100];

  float*  m_weights;
  int m_sample_num;
  int m_feature_num;
  int m_selected_feature_num;
  int m_feature_size;
  int m_positive_num;
  int m_negative_num;

	
//  int m_bootstrap_maxlevel;
  int* m_bootstrap_fileused;
  int m_bootstrap_filenum;
//  int m_bootstrap_level;
	
//  float m_bootstrap_resizeratio[5];
//  int m_bootstrap_increment[5];

  float* m_features_tmp;

  Features* m_extractor;

  int* m_selected_feature_idx;

  float m_selected_feature_ratio;
};

#endif
