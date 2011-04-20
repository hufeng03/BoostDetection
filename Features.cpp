/**
 * @file   Features.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:03:56 2010
 * 
 * @brief  implementation file of Feature class.
 * 
 * 
 */

#include "Features.h"

Features::~Features()
{
  ;
}

void Features::LoadFromFile(char *filename, float* fv)
{
	std::ifstream file;
	file.open(filename);
//	for(int i=0;i<m_feature_num;i++)
//		for(int j=0;j<m_feature_size;j++)
//			file>>fv[i*m_feature_size+j];
	file.read(reinterpret_cast<char*>(fv),sizeof(float)*m_feature_num*m_feature_size);
	file.close();
}

void Features::SaveToFile(char* filename, float* fv)
{
//	FILE* fp = fopen(filename,'rb');
//	char mybigbuff[m_feature_size*m_feature_num*20];
//	while(fread(mybigbuff, sizeof(char), m_feature_size*m_Feature_num*20, fp));
	
	
	std::ofstream file;
	file.open(filename);
	file.write(reinterpret_cast<char*>(fv),sizeof(float)*m_feature_num*m_feature_size);
//	for(int i=0;i<m_feature_num;i++)
//		for(int j=0;j<m_feature_size;j++)
//			file<<fv[i*m_feature_size+j]<<" ";			
	file.close();
}	
