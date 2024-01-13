/*
 * main.cpp

 *
 *  Created on: 2018/01/17
 *      Author: yoneken
 */
#include <mainpp.h>
#include <main.h>
#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Int16.h>
#include "std_msgs/MultiArrayLayout.h"
#include "geometry_msgs/Twist.h"
#include <nbt.h>

static nbt_t ros_nbt;

ros::NodeHandle nh;

// True = Forward; False = Reverse
bool Direction_left = true;
bool Direction_right = true;

// Minumum and maximum values for 16-bit integers
const int encoder_minimum = -32768;
const int encoder_maximum = 32767;

std_msgs::String str_msg;
ros::Publisher chatter("chatter", &str_msg);
char hello[] = "Hello world!";

// Keep track of the number of wheel ticks
std_msgs::Int16 right_wheel_tick_count;
ros::Publisher rightPub("right_ticks", &right_wheel_tick_count);

std_msgs::Int16 angleCount;
std_msgs::Int16 left_wheel_tick_count;
ros::Publisher leftPub("left_ticks", &left_wheel_tick_count);
ros::Publisher anglePub("angle_pub", &angleCount);

void steering(const geometry_msgs::Twist& cmd_msg);

ros::Subscriber<geometry_msgs::Twist> sub("car/cmd_vel", steering);

void steering(const geometry_msgs::Twist& cmd_msg) {

}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
  nh.getHardware()->flush();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  nh.getHardware()->reset_rbuf();
}

void setup(void)
{
  nh.initNode();
  nh.advertise(chatter);
  nh.advertise(rightPub);
  nh.advertise(leftPub);
  nh.advertise(anglePub);

  NBT_init(&ros_nbt, 100);
}

void loop(void)
{


  if(NBT_handler(&ros_nbt)){
		#ifdef STM32F1xx
	  	  HAL_GPIO_TogglePin(LED_BUILTIN_GPIO_Port, LED_BUILTIN_Pin);
		#endif
	  str_msg.data = hello;
	  chatter.publish(&str_msg);
	  rightPub.publish( &right_wheel_tick_count );
	  leftPub.publish( &left_wheel_tick_count );
	  anglePub.publish( &angleCount);
	  nh.spinOnce();
  }

//  HAL_Delay(100);
}

void left_wheel_tick() {
	bool val = HAL_GPIO_ReadPin(ENC_IN_LEFT_B_GPIO_Port, ENC_IN_LEFT_B_Pin);

	if (val == false) {
	    Direction_right = false;  // Reverse
	  } else {
	    Direction_right = true;  // Forward
	  }

	if (Direction_left) {
	    if (left_wheel_tick_count.data == encoder_maximum) {
	      left_wheel_tick_count.data = encoder_minimum;
	    } else {
	      left_wheel_tick_count.data++;
	    }
	  } else {
	    if (left_wheel_tick_count.data == encoder_minimum) {
	      left_wheel_tick_count.data = encoder_maximum;
	    } else {
	      left_wheel_tick_count.data--;
	    }
	  }

}

void right_wheel_tick() {

	bool val = HAL_GPIO_ReadPin(ENC_IN_RIGHT_B_GPIO_Port, ENC_IN_RIGHT_B_Pin);

	  if (val == false) {
	      Direction_left = true;  // Reverse
	    } else {
	      Direction_left = false;  // Forward
	    }

	    if (Direction_left) {
	      if (left_wheel_tick_count.data == encoder_maximum) {
	        left_wheel_tick_count.data = encoder_minimum;
	      } else {
	        left_wheel_tick_count.data++;
	      }
	    } else {
	      if (left_wheel_tick_count.data == encoder_minimum) {
	        left_wheel_tick_count.data = encoder_maximum;
	      } else {
	        left_wheel_tick_count.data--;
	      }
	    }

}

