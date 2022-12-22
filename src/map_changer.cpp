#include "ros/ros.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "math.h"
#include <cmath>
#include <ros/ros.h>                        
#include <std_srvs/SetBool.h>               
#include <std_msgs/Bool.h>
#include <std_msgs/String.h>
#include <nav_msgs/LoadMap.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int8.h>
// #include <vector>
#include <yaml-cpp/yaml.h>

class map_changer
{
public:
    map_changer();
    ros::NodeHandle nh_;
    ros::NodeHandle pnh_;
    bool read_yaml();
    void initial_pose_set(float pose_x,float pose_y,float ori_z,float ori_w);
    void call_map();
    bool change_map_callback(std_srvs::SetBool::Request &request, 
                             std_srvs::SetBool::Response &response);
    
private:
std::string filename_;
std::string pass_;
ros::Publisher initial_pose_pub;
ros::ServiceServer change_call;
ros::ServiceClient change_map;

bool ch_flag=false;
int map_id,count=0;
std::string map_name;
double pose_x,pose_y,ori_z,ori_w;
// std_srvs::SetBoolResponse map_response;
// std_srvs::SetBoolRequest map_request;

};

map_changer::map_changer() :
nh_(),
pnh_("~")
{
pnh_.param("filename", filename_, filename_);
change_call=nh_.advertiseService("change_call", 
                                  &map_changer::change_map_callback,this);

change_map=nh_.serviceClient<nav_msgs::LoadMap>("change_map");
initial_pose_pub = nh_.advertise<geometry_msgs::PoseWithCovarianceStamped>("initialpose", 10);//initial_pose publish
}

bool map_changer::change_map_callback(std_srvs::SetBool::Request &request,
                                      std_srvs::SetBool::Response &response)//from waypoint 
    {
    ch_flag=true;
    count=request.data;
    //call_map();
    return true;
    }

bool map_changer::read_yaml()//read_yaml result
    {
    YAML::Node yaml_pass= YAML::LoadFile(filename_);
    YAML::Node map_yaml;
    // YAML::Node map_yaml=YAML::LoadFile(filename_);
    map_yaml= yaml_pass["map_list"];
    int i=filename_.size();
    ROS_INFO("size :%d",&i);
    if (count>=i)
        {
            ROS_INFO("End map!!");
            exit(0);
        }
    else
        {
        map_id = map_yaml[count]["map"]["id"].as<int>();
        map_name= map_yaml[count]["map"]["map_name"].as<std::string>();
        pose_x=map_yaml[count]["map"]["pose_x"].as<double>();
        pose_y=map_yaml[count]["map"]["pose_y"].as<double>();
        ori_z=map_yaml[count]["map"]["ori_z"].as<double>();
        ori_w=map_yaml[count]["map"]["ori_w"].as<double>();
        std::cout << map_id << ", " << map_name <<"\n";
        }
    return true;
    }

void map_changer::initial_pose_set(float x,float y,float o_z,float o_w)//initial_pose set function
    
    {
    geometry_msgs::PoseWithCovarianceStamped pose_msg;
    pose_msg.header.stamp = ros::Time::now();
    pose_msg.header.frame_id = "map";
    pose_msg.pose.pose.position.x = x;
    pose_msg.pose.pose.position.y = y;
    pose_msg.pose.covariance[0] = 0.25;
    pose_msg.pose.covariance[6 * 1 + 1] = 0.25;
    pose_msg.pose.covariance[6 * 5 + 5] = 0.06853891945200942;
    pose_msg.pose.pose.orientation.z = o_z;
    pose_msg.pose.pose.orientation.w = o_w;
    initial_pose_pub.publish(pose_msg);
    }
    
void map_changer::call_map()//mainloop
    {
    //ros::Rate loop_rate(1);
        nav_msgs::LoadMap::Request map_req;
        nav_msgs::LoadMap::Response map_res;
        // map_req.map_url="/home/haru/map/1007a.yaml";
        map_req.map_url=map_name;
        if (ch_flag)
        {
            read_yaml();
            ROS_INFO("map_id: %d",map_id);
            ROS_INFO("map_url: %s",map_name);
            change_map.call(map_req,map_res);
            initial_pose_set(pose_x,pose_y,ori_z,ori_w);
            ch_flag=false;
            count++;
            ROS_INFO("Call Change Map!:count: %d" ,count);
        }
        else
            ROS_INFO("No");
    }                                                        

int main(int argc, char** argv){
  ros::init(argc, argv, "map_changer");
 // ros::Rate loop_rate(1);
  map_changer map_ch;
  ros::Rate loop_rate(1);
  bool read_result = map_ch.read_yaml();
    if(!read_result){
        ROS_ERROR("Map changer system is shutting down");
        return 1;
    }
    else{
     while (ros::ok())
        {
            map_ch.call_map();
            ros::spinOnce();                                         
            loop_rate.sleep();
       }
    }        
        
    return 0;
    }
