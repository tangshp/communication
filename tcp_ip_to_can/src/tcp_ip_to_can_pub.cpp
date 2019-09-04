#include "ros/ros.h"
#include "std_msgs/String.h"
#include "tcp_ip_to_can/Cmd_vel.h"
#include "tcp_ip_to_can/Controller.h"
#include <tcp_ip_to_can/Tcp_ip_to_can.h>


#include <sstream>

/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */
int main(int argc, char *argv[])
{
	//Con data
	Tcp_ip_to_can Con;
	
	// parameters
	std::string host;
	int port;

	ros::init(argc, argv, "tcp_ip_to_can");
	ros::NodeHandle nh;
  	ros::NodeHandle n("~");

	ros::Publisher chatter_pub = nh.advertise<std_msgs::String>("chatter", 1000);
	n.param<std::string>("host", host, "192.168.1.150");
	n.param<int>("port", port, 6020);

	ros::Rate loop_rate(20);

	/**
	 * A count of how many messages we have sent. This is used to create
	 * a unique string for each message.
	 */
	int count = 0;
	
	//prepare data
	tcp_ip_to_can::Cmd_vel cmd_velocity;
	tcp_ip_to_can::Controller data_from_controller2;
	memset(&cmd_velocity,0,sizeof(cmd_velocity));

	//发报文数据部分
	CMD_VEL cmd_vel_to_can;//结构体;linear,angular
	memset(&cmd_vel_to_can,0,sizeof(cmd_vel_to_can));

	//收报文数据部分
	CMD_VEL data_from_can;
	memset(&data_from_can,0,sizeof(data_from_can));
	//填充报文
	NETWORK_CANFRAME Date_to_controller2;
	Date_to_controller2.candevindex = 0x00;
	Date_to_controller2.id = 0x6EA;
	Date_to_controller2.len = 8;
	memcpy(Date_to_controller2.byte ,&cmd_vel_to_can ,sizeof(cmd_vel_to_can));
	Date_to_controller2.ext = 0x00;
	Date_to_controller2.rtr = 0x00;
	Date_to_controller2.param1 = 0x00000000;

	NETWORK_CANFRAME Date_from_controller2;
	memset(&Date_from_controller2, 0, sizeof(Date_from_controller2));
	ROS_INFO("sizeof(data_from_can.byte):[%d]",sizeof(Date_from_controller2.byte));
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

		 while (ros::ok())
		{
			//ros::Time start = ros::Time::now();
			write(Con.socket_fd_,&Date_to_controller2,sizeof(Date_to_controller2));
			// int s=read(Con.socket_fd_,&Date_from_controller2, sizeof(Date_from_controller2));
			err_read = Con.read_from_can(Con.socket_fd_, &Date_from_controller2, sizeof(Date_from_controller2));
			if(err_read == -2) break;
			if(err_read < 0) continue;
			memcpy(&data_from_can,Date_from_controller2.byte,sizeof(Date_from_controller2.byte));
			ROS_INFO("can_id_from_controller2:[%x] linear:[%d] angular:[%d]",Date_from_controller2.id,data_from_can.linear,data_from_can.angular);
			memset(&Date_from_controller2, 0, sizeof(Date_from_controller2));
			cmd_vel_to_can.linear += 1;
			cmd_vel_to_can.angular += 1;
			memcpy(Date_to_controller2.byte ,&cmd_vel_to_can ,sizeof(cmd_vel_to_can));
			//ros::spinOnce();
			loop_rate.sleep();
		}
		std_msgs::String msg;

		std::stringstream ss;
		ss << "hello world " << count;
		msg.data = ss.str();

		ROS_INFO("%s", msg.data.c_str());

		/**
		 * The publish() function is how you send messages. The parameter
		 * is the message object. The type of this object must agree with the type
		 * given as a template parameter to the advertise<>() call, as was done
		 * in the constructor above.
		 */
		chatter_pub.publish(msg);

		loop_rate.sleep();
		++count;
	}

	return 0;
}