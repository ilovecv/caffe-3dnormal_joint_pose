/*
 * norm_layer.cpp
 *
 *  Created on: Sep 13, 2014
 *      Author: dragon123
 */


#include <algorithm>
#include <vector>
#include <cmath>

#include "caffe/layer.hpp"
#include "caffe/vision_layers.hpp"
#include "caffe/util/math_functions.hpp"

using std::max;

namespace caffe {

template <typename Dtype>
void NormLayer<Dtype>::SetUp(const vector<Blob<Dtype>*>& bottom,
      vector<Blob<Dtype>*>* top) {
  CHECK_EQ(bottom.size(), 1) << "NormLayer Layer takes a single blob as input.";
  CHECK_EQ(top->size(), 1) << "NormLayer Layer takes a single blob as output.";
  (*top)[0]->Reshape(bottom[0]->num(), bottom[0]->channels(),
      bottom[0]->height(), bottom[0]->width());
}

template <typename Dtype>
Dtype NormLayer<Dtype>::Forward_cpu(const vector<Blob<Dtype>*>& bottom,
    vector<Blob<Dtype>*>* top) {
  const Dtype* bottom_data = bottom[0]->cpu_data();
  Dtype* top_data = (*top)[0]->mutable_cpu_data();

  int channel = bottom[0]->channels();
  int height = bottom[0]->height();
  int width = bottom[0]->width();

  int num = bottom[0]->num();
  int dim = bottom[0]->count() / bottom[0]->num();
  int dimScale = height * width;

  memcpy(top_data, bottom_data, sizeof(Dtype) * bottom[0]->count());

  for(int i = 0; i < num; i ++)
  {
	  for(int h = 0; h < height; h ++)
		  for(int w = 0; w < width; w ++)
		  {
			  float sumdata = 0;
			  for(int c = 0; c < channel; c ++)
			  {
				  float tdata = bottom_data[i * dim + c * dimScale + h * width + w];
				  sumdata += tdata * tdata;
			  }
			  sumdata = sqrt(sumdata) + 1e-6;
			  for(int c = 0; c < channel; c ++)
			  {
				  top_data[i * dim + c * dimScale + h * width + w] /= sumdata;
			  }
		  }
  }

  return Dtype(0);
}

template <typename Dtype>
void NormLayer<Dtype>::Backward_cpu(const vector<Blob<Dtype>*>& top,
    const bool propagate_down,
    vector<Blob<Dtype>*>* bottom) {
  const Dtype* top_diff = top[0]->cpu_diff();
  const Dtype* top_data = top[0]->cpu_data();
  const Dtype* bottom_data = (*bottom)[0]->cpu_data();
  Dtype* bottom_diff = (*bottom)[0]->mutable_cpu_diff();

  int channel = (*bottom)[0]->channels();
  int height = (*bottom)[0]->height();
  int width = (*bottom)[0]->width();
  int num = (*bottom)[0]->num();
  int dim = (*bottom)[0]->count() / (*bottom)[0]->num();
  int dimScale = height * width;

  for(int i = 0; i < num; i ++)
  {
  	  for(int h = 0; h < height; h ++)
  		  for(int w = 0; w < width; w ++)
  		  {
  			  float sumdata = 0;

  			  for(int c = 0; c < channel; c ++)
  			  {
  				  float tdata = bottom_data[i * dim + c * dimScale + h * width + w];
  				  sumdata += tdata * tdata;
  			  }
  			  float sumdata23 = sumdata * sqrt(sumdata) + 1e-6;
  			  for(int c1 = 0; c1 < channel; c1 ++)
  			  {
  				  float tdiff = 0;
  				  for(int c2 = 0; c2 < channel; c2 ++)
  				  {
  					  float tnum = 0;
  					  float zc1 = bottom_data[i * dim + c1 * dimScale + h * width + w];
  					  float zc2 = bottom_data[i * dim + c2 * dimScale + h * width + w];
  					  if(c1 == c2) tnum = (sumdata - zc1 * zc1) / sumdata23;
  					  else tnum = - (zc1 * zc2) / sumdata23;

  					  tnum = tnum * top_diff[i * dim + c2 * dimScale + h * width + w];
  					  tdiff += tnum;
  				  }
  	  			  bottom_diff[i * dim + c1 * dimScale + h * width + w] = tdiff;
  			  }
  		  }
  }

}


INSTANTIATE_CLASS(NormLayer);


}  // namespace caffe
