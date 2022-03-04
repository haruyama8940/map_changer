#include "ros/ros.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "math.h"
#include <cmath>
#include <ros/ros.h>                        // ROSヘッダファイル
#include <std_srvs/SetBool.h>                // サービスヘッダファイル
#include <std_srvs/Trigger.h>                // サービスヘッダファイル
#include <std_msgs/Bool.h>
#include <std_msgs/String.h>
#include <nav_msgs/LoadMap.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int8.h>
// #include <gazebo_msgs/LinkStates.h>
// #include <gazebo_msgs/ModelStates.h>
// #include <gazebo_msgs/SetModelState.h>
// #include <vector>
#include <yaml-cpp/yaml.h>

class map_changer
{
public:
    map_changer();
    ros::NodeHandle nh_;
    ros::NodeHandle pnh_;
    bool read_yaml();
    void call_map();
    bool change_map_callback(std_srvs::Trigger::Request &request, 
                             std_srvs::Trigger::Response &response);
private:
std::string filename_;
std::string pass_;
ros::ServiceServer change_call;
ros::ServiceClient change_map;
// std_srvs::SetBoolResponse map_response;
// std_srvs::SetBoolRequest map_request;

};

map_changer::map_changer() :
nh_(),
pnh_("~")
{
pnh_.param("filename", filename_, filename_);
change_call=nh_.advertiseService("change_map", 
                                  &map_changer::change_map_callback,this);

change_map=nh_.serviceClient<nav_msgs::LoadMap>("change_map");
}

bool map_changer::change_map_callback(std_srvs::Trigger::Request &request,
                                      std_srvs::Trigger::Response &response)//from waypoint 
    {
    return true;
    }

bool map_changer::read_yaml()//read_yaml result
    {
    return true;
    }

void map_changer::call_map()//mainloop
    {
    nav_msgs::LoadMap::Request map_req;
    nav_msgs::LoadMap::Response map_res;
    map_req.map_url="/home/haru/map/1007a.yaml";
    change_map.call(map_req,map_res);
    }



int main(int argc, char** argv){
  ros::init(argc, argv, "map_changer");
  map_changer map_ch;
  ros::Rate rate(1);
  bool read_result = map_ch.read_yaml();
    if(!read_result){
        ROS_ERROR("Waypoint Navigatioin system is shutting down");
        return 1;
    }
    else{
        map_ch.call_map();
    return 0;
    }
}