#ifndef _harrfeatures_h_
#define _harrfeatures_h_

#include "Features.h"
#include <fstream>
#include <math.h>
#include <cv.h>
#include <highgui.h>
#include <vector>

typedef struct Haar
{
  int type;
  int y1,y2,y3,y4,x1,x2,x3,x4;
}Haar;

/*inline int icvGet2D(CvMat* mat, int row, int col)
{
     return CV_MAT_ELEM(*mat, double, row, col);
     }

inline int icvGetInt2D(CvMat* mat, int row, int col)
{
     return (int*)CV_MAT_ELEM_PTR_FAST(*mat,row,col,sizeof(int));
}*/

/*
#define sum_elem_ptr(sum,row,col)  \
    ((int*)CV_MAT_ELEM_PTR_FAST((sum),(row),(col),sizeof(int)))

#define sqsum_elem_ptr(sqsum,row,col)  \
    ((double*)CV_MAT_ELEM_PTR_FAST((sqsum),(row),(col),sizeof(double)))

#define calc_sum(rect,offset) \
    ((rect).p0[offset] - (rect).p1[offset] - (rect).p2[offset] + (rect).p3[offset])
*/

class HaarFeatures : public Features {

 public:
  HaarFeatures(int width = 24, int height = 24);

  ~HaarFeatures();

  void Init(IplImage*);

  /** 
   * This function calculate the feature vector.
   * The result was written to m_features, and each element equals the reponse of the correspondent Harr.
   */
 // bool Extract(float* , int , int );
  bool Extract(float* , int , int, bool* selected = NULL);
  bool Extract(float* , int , int, int* selected,int num);
  
  void Clear();

  void DrawFeature(IplImage*, int, CvScalar);

  void ReadFromFile(std::ifstream& f);
  void WriteToFile(std::ofstream& f);

 private:
 
  inline double GetOneFeatureValue(int type, int y1, int y2, int y3, int y4, int x1, int x2, int x3, int x4);
  void CalcIntegral(IplImage* img);

  CvMat* m_integral;
  CvMat* m_square;
  std::vector<Haar> m_haars;
  double m_var;
  float m_paddedsize;
  int m_sum_width;
  int m_sum_height;
};

#endif
