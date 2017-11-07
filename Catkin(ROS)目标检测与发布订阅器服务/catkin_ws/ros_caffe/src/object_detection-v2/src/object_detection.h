/*
Copyright (c) 2016 HPC Department of SenseTime Group Limited.

*************************************************************************************
商汤集团（SenseTime Group）是一家科技创新公司，致力于引领人工智能核心“深度学习”
技术突破，构建人工智能、大数据分析行业解决方案。我们成功聚集了当下华人中最出色、
最有影响力的深度学习、计算机视觉科学家，以及来自于谷歌、百度、微软、联想等一批
产业界的领军人物。在人工智能产业兴起的大背景下，商汤集团凭借在技术、人才、专利
上超过十年的积累，迅速成为了人工智能行业领军企业。

公司的核心团队是由两部分精英组成：一部分是在世界各所顶尖大学，如麻省理工学院、
斯坦福大学、清华大学、香港大学、香港中文大学、香港科技大学取得博士学位的优秀科学家；
另一部分是来自谷歌、微软、联想、百度等产业界的资深员工。学界业界精英的融合，
成就了SenseTime人才储备上的优势，为公司的长久发展打下了坚实基础。
加入我们，一起做中国的原创技术：

rogy.liu@sensetime.com
*************************************************************************************
*/
#ifndef __body_detection_h__
#define __body_detection_h__

#include <boost/foreach.hpp>
#include <caffe/caffe.hpp>
#include <caffe/util/bbox_util.hpp>
//#include "bbox_util.hpp"

#include <ros/ros.h>
#include <ros/package.h>
#include <sensor_msgs/image_encodings.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cuda.h>
#include <cuda_runtime.h>

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <cassert>

//using namespace std;
//using namespace caffe;

typedef struct _config
{
	// model
	std::string prototxt;
	std::string caffemodel;

	// output
	int resized_width;
	int resized_height;	
	std::string location_output_layer_name;
	std::string confidence_output_layer_name; 
	std::string priorbox_output_layer_name;
	bool share_location;
	int num_classes;
	int background_label_id;
	std::string label_map_file;
	caffe::CodeType code_type;
	float confidence_threshold;

	// nms
	float nms_threshold;
	int top_k;
	int keep_top_k;

	// visual
	bool visualize;
	float visualize_threshold;
	int show_image_width;
	int show_image_height;

	// save
	bool need_save;
	std::string save_format;	// VOC, COCO, ILSVRC 
	std::string save_directory;

} config; 

const int mean_b = 104;
const int mean_g = 117;
const int mean_r = 123;

//const int resized_width = 300;
//const int resized_height = 300;

const int resized_width = 224;
const int resized_height = 224;

const std::string location_output_layer_name = "mbox_loc";				// blob name
const std::string confidence_output_layer_name = "mbox_conf_flatten";	// blob name
const std::string priorbox_output_layer_name = "mbox_priorbox";			// blob name

class SSD
{
public:
	SSD(const std::string& configfile);

	std::string detect(const cv::Mat& img);
         
        template <typename Dtype>
        std::string getResult(const std::vector<cv::Mat>& images, const caffe::Blob<Dtype>* detections,
                   const float threshold, const std::vector<cv::Scalar>& colors,
                   const std::map<int, std::string>& label_to_display_name);

private:
	bool init(const std::string configfile);

	bool readConfigFile(const std::string configfile);
	
	/* Wrap the input layer of the network in separate cv::Mat objects
	 * (one per channel). This way we save one memcpy operation and we
	 * don't need to rely on cudaMemcpy2D. The last preprocessing
	 * operation will write the separate channels directly to the input
	 * layer. */
	void wrapInputLayer(std::vector<cv::Mat>& input_channels);

	void preprocess(const cv::Mat& img);

#if CPU_ONLY
	void getResultsCPU(const std::vector<caffe::Blob<float>*>& top);
#else	
	void getResultsGPU(const std::vector<caffe::Blob<float>*>& top);
#endif
	inline int getBlobIdxByName(std::string& query_name)
	{
		std::vector<std::string> const& blob_names = net_->blob_names();
		for (unsigned int i = 0; i != blob_names.size(); ++i)
		{
			if (query_name == blob_names[i])
			{
				return i;
			}
		}
		
		LOG(FATAL) << "Unknown blob name: " << query_name;
		return -1;
	}

private:
	caffe::shared_ptr<caffe::Net<float> > net_;
	config config_;
	int loc_blob_idx_;
	int conf_blob_idx_;
	int prior_blob_idx_;
	std::map<int, std::string> label_to_display_name_;
};


#endif // __body_detection_h__
