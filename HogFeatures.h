/**
 * @file   HogFeaturess.h
 * @author feng <huf07@mails.tsinghua.edu.cn>
 * @date   Tue Sep  9 10:58:12 2008
 * 
 * @brief  header file of HogFeatures.cpp
 * 
 * 
 */

#ifndef _hogfeatures_h_
#define _hogfeatures_h_

#include "Features.h"
#include <fstream>
#include <math.h>
#include <cv.h>
#include <highgui.h>
#include <vector>

typedef struct Hog
{
  int x,y,w,h;
}Hog;

class HogFeatures : public Features 
{
 public:
  HogFeatures(int width = 64, int height = 128);

  ~HogFeatures();

  void Init(IplImage*);

  /** 
   * This function calculate the feature vector.
   * The result was written to m_features, and each element equals the reponse of the correspondent edgelet.
   */
  //bool Extract(float* fv, int y, int x);
  bool Extract(float* , int , int, bool* selected = NULL);
  bool Extract(float* , int , int, int* selected,int num);
  void Clear();
  void DrawFeature(IplImage* img, int n, CvScalar);

  void ReadFromFile(std::ifstream& f);
  void WriteToFile(std::ofstream& f);

 private:
 
  CvMat* m_gradient_amplitude;
  CvMat* m_gradient_angle;
  CvMat* m_intergral_gradient[10];
  int* m_gradient_x;
  int* m_gradient_y;
  double* m_gradient_k[9];
  
  std::vector<Hog> m_hogs; /**< Array of edgelets contained in the m_width*m_height rectangle */
};


#endif
