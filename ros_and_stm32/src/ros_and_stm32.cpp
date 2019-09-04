#include "ros/ros.h"
#include "std_msgs/String.h"
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/Twist.h>
#include "serial/serial.h"
#include "../include/user_protcol.h"
#include <sstream>

#include <cstdlib>


serialData data;
//创建一个serial类
serial::Serial ser;
bool param_use_debug_vel;

void serial_recv(uint8_t *recvBuffer, size_t recvSize);
void cmd_vel_callback(const geometry_msgs::Twist& cmd_vel);

int main(int argc, char *argv[])
{

    system("sudo chmod 777 /dev/ttyUSB0");
    //创建timeout
    serial::Timeout to = serial::Timeout::simpleTimeout(100);
    //设置要打开的串口名称
    ser.setPort("/dev/ttyUSB0");
    //设置串口通信的波特率
    ser.setBaudrate(115200);
    //串口设置timeout
    ser.setTimeout(to);
 
    try
    {
        //打开串口
        ser.open();
    }
    catch(serial::IOException& e)
    {
        ROS_ERROR_STREAM("Unable to open port.");
        return -1;
    }
    
    //判断串口是否打开成功
    if(ser.isOpen())
    {
        ROS_INFO_STREAM("/dev/ttyUSB0 is opened.");
    }
    else
    {
        return -1;
    }

	ros::init(argc, argv, "ros_and_stm32");
	ros::NodeHandle n;

	ros::Publisher pub_raw_pose = n.advertise<geometry_msgs::TwistStamped>("raw_vel", 1000);

    ros::Subscriber write_sub = n.subscribe("cmd_vel",1000,cmd_vel_callback); //订阅键盘指令
    ros::NodeHandle nh("~");
    nh.param<bool>("debug_vel",param_use_debug_vel, true);

    /*msg type*/
    geometry_msgs::TwistStamped pub_msg_pose;

    int i=0;
	ros::Rate loop_rate(20);
    while(ros::ok())
    {
        ros::spinOnce();
        serial_recv((uint8_t*)&data,sizeof(serialData));  //接受STM32发送数据
        //ROS_INFO("type[%d]",data.type);
        if(data.type == VAL_POSE)
        {
            pub_msg_pose.header.stamp = ros::Time::now();
            /*测试，注释了以下4行代码,完整使用时解除*/
            pub_msg_pose.twist.linear.x = data.dat.vel.linear[0];
            //pub_msg_pose.twist.linear.y = data.dat.vel.linear[1];
            pub_msg_pose.twist.angular.z = data.dat.vel.linear[1];
            pub_raw_pose.publish(pub_msg_pose);  //将数据发送给odom节点
            //ROS_INFO("type[%d]  V_x[%f]",data.type, data.dat.vel.linear[0]);
            if(param_use_debug_vel)
            {
                //ROS_INFO("type[%d]  V_x[%f]   current_throttle[%.2f]",data.type,data.dat.vel.linear[0],data.dat.vel.linear[1]);
                ROS_INFO("v:[%f]   pwm:[%f]",data.dat.vel.linear[0], data.dat.vel.linear[1]);
                //ROS_INFO("pwm: [%f]  n: [%f]   v: [%.2f]", data.dat.vel.linear[1], data.dat.vel.linear[0], data.dat.vel.linear[0]*0.630/60);
            }
        }
        loop_rate.sleep();

    }
    //关闭串口
    ser.close();

	return 0;
}

void serial_recv(uint8_t *recvBuffer, size_t recvSize)  //接收数据函数
{
    int count=0;
    do{
         if(ser.available()){
             //获取数据头
             do{
                ser.read(recvBuffer,1);
                if(recvBuffer[0] != _SERIAL_SYN_CODE_START)
                {
                    ROS_WARN("serial_rx _PROTOCL_SYN_START %d",count++);
                    continue;
                }
                //ROS_INFO("serial_rx _PROTOCL_SYN_START IS OK!!!");
                break;
             }while(1);

             ser.read(&recvBuffer[1],sizeof(serialData)-1);
             //判断数据尾
             if(recvBuffer[sizeof(serialData) - 2] != _SERIAL_SYN_CODE_CR || recvBuffer[sizeof(serialData) - 1] != _SERIAL_SYN_CODE_LF)
             {
                 ROS_ERROR("serial_rx DATA_ERROR");
                 continue;
             }
         }
         return;
        }while(1);
}


void cmd_vel_callback(const geometry_msgs::Twist& cmd_vel)  //收到键盘话题后，向下发送给STM32
{
	//ROS_INFO("I heard linear velocity: x-[%f],y-[%f],",cmd_vel.linear.x,cmd_vel.linear.y);
	//ROS_INFO("I heard angular velocity: [%f]",cmd_vel.angular.z);
    //ROS_INFO("linear velocity: x-[%f], angular velocity: [%f]",cmd_vel.linear.x,cmd_vel.angular.z);
	//std::cout << "Twist Received" << std::endl;	
	serialData S_data;//send data
    memset(&S_data,0,sizeof(S_data));
    S_data.syn = _SERIAL_SYN_CODE_START;
    S_data.type = VAL_VEL;
    S_data.dat.vel.linear[0] = cmd_vel.linear.x;
    S_data.dat.vel.linear[1] = cmd_vel.angular.z;
    S_data.sys_CR = _SERIAL_SYN_CODE_CR;
    S_data.syn_LF = _SERIAL_SYN_CODE_LF;
	ser.write((uint8_t*)&S_data,sizeof(serialData));
    //ROS_INFO("sizeof(serialData) : %d",sizeof(serialData));
}