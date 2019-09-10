#include "ros/ros.h"
#include "std_msgs/String.h"
#include "can_msgs/Frame.h"
#include <sstream>
#include <swsocketcan_bridge/swsocketcan_bridge.h>



//Con data
Swsocketcan_bridge Con;

void dataMsgsCallback(const can_msgs::Frame::ConstPtr& msg)
{
	Con.fromROSMsg(Con.can_to_base,msg);
	write(Con.socket_fd_,&(Con.can_to_base),sizeof(Con.can_to_base));
}
int main(int argc, char *argv[])
{
    ros::init(argc, argv, "Swsocketcan_bridge");
	ros::NodeHandle nh;
  	ros::NodeHandle n("~");

	std::string host;
	int port;
    n.param<std::string>("host", host, "192.168.1.150");
	n.param<int>("port", port, 6020);

    ros::Publisher can_frame_pub = nh.advertise<can_msgs::Frame>("received_messages", 10);
    ros::Subscriber can_frame_sub = nh.subscribe("sent_messages",10,dataMsgsCallback);
	
	ros::Rate loop_rate(20);
	int count = 0;
	int err_read;
	while (ros::ok())
	{
		ROS_INFO_STREAM("Connecting to TCP/IP_to_CAN at " << host);

        Con.connect(host, port);

		if (!Con.isConnected())
		{
			ROS_WARN("Unable to connect, retrying.");
			ros::Duration(1).sleep();
			continue;
		}
		
		ROS_INFO("Connected to Con.");
		while(ros::ok())
		{
			err_read = Con.read_from_can(Con.socket_fd_, &(Con.can_from_base), sizeof(Con.can_from_base));
			if(err_read == -2) break;//-2 表示没有数据
			if(err_read < 0) continue;

			Con.toROSMsg(Con.can_from_base,Con.can_msgs_pub);
			can_frame_pub.publish(Con.can_msgs_pub);
			Con.clearROSMsg(Con.can_msgs_pub);
			memset(&(Con.can_from_base),0,sizeof(Con.can_from_base));
			ros::spinOnce();
			loop_rate.sleep();
		}
		
		Con.disconnect();
	}

	return 0;
}