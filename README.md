## FDU2017_Unique_Robotics_Car

######项目中，目标检测、ROS上的发布/订阅器服务的项目工程太大，我们仅提供了主要修改和增加的代码文件。均在目录“Catkin(ROS)目标检测与发布订阅器服务”下面。

######Ardruno控制代码部分
包含了Android智能小车语音控制App及对应的anrdoid项目，后台服务器的代码【python实现】。其中的IP地址需要修改，android项目工程目录中的Config.java文件配置了webserver部署的IP地址和端口号，可自行查看。

######Ardruino智能小车控制代码部分
这部分代码是烧到Ardruino开发板中的代码，用于接收特定的Topic信号(默认订阅的服务为"car_cmd")，并进行相应的控制，通过控制小车的轮子转速控制小车的速度和转动方向。
