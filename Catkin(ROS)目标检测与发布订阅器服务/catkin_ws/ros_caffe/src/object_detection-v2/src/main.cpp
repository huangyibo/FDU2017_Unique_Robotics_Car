#include "object_detection.h"
#include <ros/ros.h>
#include <ros/package.h>
#include "std_msgs/String.h"
#include <sensor_msgs/image_encodings.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "time_helper.h"
#include <map>
#include <string.h>

typedef std::pair<std::string, int> PAIR;

struct CmpByValue 
{
  bool operator()(const PAIR& lhs, const PAIR& rhs) {  
    return lhs.second < rhs.second;  
  } 
};

int judge_object(std::string *objects, std::string object_val)
{
  std_msgs::String msg_tmp1;  
  std_msgs::String msg_tmp2;
  msg_tmp1.data = object_val;
  ROS_INFO("judge_object [%s] ", msg_tmp1.data.c_str());
  for(int i=0; i != 6; ++i)
  {
     //if( std::strcmp(object_val, objects[i]) == 0)
     msg_tmp2.data = objects[i];
     ROS_INFO("judge_object objects [%s] ", msg_tmp2.data.c_str());
     if(object_val == objects[i])
     {
        msg_tmp1.data = object_val;
        msg_tmp2.data = objects[i];
        ROS_INFO("in judge_object [%s == %s] ", msg_tmp1.data.c_str(), msg_tmp2.data.c_str());
        return 1;
     }
  }
  return 0;
}

void map_put(std::string key, int value, std::map<std::string, int>& map_result)
{
  std::map<std::string, int>::iterator iter;
  if(iter == map_result.end())
  {
    map_result[key] = value;
  }
  else
  {
    map_result[key] = map_result[key] + value;
  }

}

int main(int argc, char **argv)
{	
        ros::init(argc,argv,"ros_caffe_test");
        ros::NodeHandle nh;
        ros::Publisher chatter_pub = nh.advertise<std_msgs::String>("chatter",1);
        ros::Rate loop_rate(10);
        const std::string ROOT_SAMPLE = ros::package::getPath("ros_caffe");
        std::string configfile=ROOT_SAMPLE+"/src/object_detection-v2/config/config.xml";
	SSD ssd(configfile);
  
        std::string result;
	std::string presult;
        cv::VideoCapture vcap(0);
        
    if (false == vcap.isOpened())
    {
        ROS_ERROR_STREAM( "camera cannot open!");
        ros::shutdown;
    }
    
    cv_bridge::CvImagePtr frame;
    frame = boost::make_shared<cv_bridge::CvImage>();
    frame->encoding=sensor_msgs::image_encodings::BGR8;

    cv::Mat image;
    std_msgs::String msg;
    std_msgs::String msg_tmp;
	int count = 0;

    int running_count = 0;
    std::map<std::string, int> map_result;
    std::string objects[6] = {"cat", "bicycle", "sofa", "dog", "train", "car"};
    


    while (true)
    {
		{
			//__TIC__();
			 vcap >> image;
                     // imshow("",image);
                  // std::cout<<"4..."<<std::endl;

			//printf("frame w = %d\th = %d\n", image.cols, image.rows);
			//printf("sg.xu: camera capture elapsed time: ");
			//__TOC__();
		}

		__TIC__();
                result=ssd.detect(image);
		if(result == "")
 		count++;
		if(count == 20)
		{
			msg.data = "";
			count = 0;
		}
               presult = msg.data;
               if(running_count == 5)
               {
                  std::vector<PAIR> name_score_vec(map_result.begin(), map_result.end());                        
                  std::sort(name_score_vec.begin(), name_score_vec.end(), CmpByValue());
                  int vec_len = name_score_vec.size() - 1;
                  std::pair<std::string, int> object_result = name_score_vec[vec_len];
                  msg.data = object_result.first;
                  ROS_INFO("running_count == 5 ...... %s ", msg.data.c_str());
                  if(presult != object_result.first)
                  {
                     ROS_INFO("chatter_pub.publish ...... %s", msg.data.c_str());
                     chatter_pub.publish(msg);
                     presult = object_result.first;
                     map_result.clear();
                  }
                  //chatter_pub.publish(msg);
                  map_result.clear();
                  running_count = 0;
               }
               
               int tmp = judge_object(objects, result);
               std::stringstream ss;
               std::string str;
               ss<<tmp;
               ss>>str;
               msg_tmp.data = str;
               ROS_INFO("judge_object: %s", msg_tmp.data.c_str());               
               if(result != "" && judge_object(objects, result)==1)
               {
                 ++running_count;
                 //msg.data = result;
                 map_put(result, 1, map_result);
                 msg_tmp.data = result;
                 ROS_INFO("++running_count...... %s", msg_tmp.data.c_str());
               }          
            
		
		//2017.04.12  注释以下两段
		//printf("sg.xu: total elapsed time: ");
		__TOC__();
		loop_rate.sleep();
		int key = cv::waitKey(1);
        	if (27 == key )
        	{
            		break;
        	}        
    }
    return 0;
}
