#include "DataSet.h"
#include "Classifier.h"
#include "Features.h"
#include "EdgeletFeatures.h"
#include "HaarFeatures.h"
#include "SimpleClassifier.h"
#include "BoostClassifier.h"
#include "CascadeClassifier.h"
#include "Detector.h"

#define WIDTH 50
#define HEIGHT 20

SimpleClassifier* BoostClassifier::m_sc_pool = NULL;
int* BoostClassifier::m_feature_selected = NULL;
int main(int argc, char* argv[])
{
  if(argc!=2)
    {
      printf("show image_name.image_format");
      return 0;
    }
  //define test_image, file_reader, feature_extrator, classifier and detector
  int i;
  IplImage* img;
  std::ifstream ifile;
  EdgeletFeatures hf(WIDTH,HEIGHT);
  CascadeClassifier cc;

  BoostClassifier::InitFeatureSelected(&hf);

  //read image
  img = cvLoadImage(argv[1],1);

  //load default classifier
  ifile.open("./data/cascadeclassifier.txt");
  cc.ReadFromFile(ifile);


  cvNamedWindow("features",1);

  for(i=0;i<hf.GetSize();i++)
  {
    if(BoostClassifier::m_feature_selected[i] == 1)
    {
      hf.ShowFeature(img, i);
      cvShowImage("features",img);
      cvWaitKey(0);
    }
  }
  cvDestroyWindow("features");

  return 1;
}
