/*
  * create_by : Thomas Gandarias
  * date : 06/10/2017
*/

//start code

//inclu la librairie de ros
#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "rosgraph_msgs/Log.h"
#include "turtlesim/Pose.h"


ros::Publisher velocity_publisher;
ros::Subscriber pose_subscriber;
ros::Subscriber rosout_subsciber;
turtlesim::Pose turtlesim_pose;

//variables d'environement
const double x_min = 0.0;
const double y_min = 0.0;
const double x_max = 11.0;
const double y_max = 11.0;
bool haveTouchSmtgs = false;
bool randomDirection = true;


const double PI = 3.14159265359;

/**
  * method to move the robot straight
  * speed : vitesse du robot_cleaner
  * distance : distance a parcourir
  * isForward : si le robot avance ou recule
*/
void move(double speed, double distance, bool isForward);
/**
  * method to rotate the turtle
  * angular_speed : vitesse angulaire
  * angle: valeur de l'angle en radian
  * clockwise : sens horloge de la rotation
*/
void rotate(double angular_speed, double angle,bool clockwise);

/*
  * method de convertion d'angle

*/
double degrees2radians(double angle_in_degree);

double setDesiredOrientation(double desired_angle_radians);

void poseCallback(const turtlesim::Pose::ConstPtr & pose_message);

void rosoutCallBack(const rosgraph_msgs::Log & log_message);

double getDistance(double x1, double y1, double x2, double y2);

void moveGoal(turtlesim::Pose goal_pose, double distance_tolerance);

int main(int argc, char *argv[])
{
  //initiate new ros node, the third argument is the name of the node
  ros::init(argc, argv, "robot_cleaner");

  // Create the main access point to communications with the ROS system.
  // The first NodeHandle constructed will fully initialize this node, and the last
  // NodeHandle destructed will close down the node.
  ros::NodeHandle n;

  //declaration des variables
  double speed, distance, angular_speed, angle, randomAngle;
  bool isForward, clockwise;

  velocity_publisher = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
  pose_subscriber = n.subscribe("/turtle1/pose", 10, poseCallback);
  rosout_subsciber = n.subscribe("/rosout", 10, rosoutCallBack);
  // std::cout << "enter speed" << '\n';
  // std::cin >> speed;
  // std::cout << "enter distance" << '\n';
  // std::cin >> distance;
  // std::cout << "forward ? " << '\n';
  // std::cin >> isForward;
  //
  // move(speed, distance, isForward);
  //
  // std::cout << "enter angular velocity (degree/sec)" << '\n';
  // std::cin >> angular_speed;
  // std::cout << "enter desired angle (degree)" << '\n';
  // std::cin >> angle;
  // std::cout << "clockwise ? " << '\n';
  // std::cin >> clockwise;
  //
  // rotate(degrees2radians(angular_speed), degrees2radians(angle), clockwise);


  // setDesiredOrientation(degrees2radians(120));
	ros::Rate loop_rate(1000);
	// loop_rate.sleep();
	// setDesiredOrientation(degrees2radians(-60));
	// loop_rate.sleep();
  // setDesiredOrientation(degrees2radians(0));

  turtlesim::Pose goal_pose;
  while(ros::ok())
  {
    if(haveTouchSmtgs == true)
    {
      move(10,0.6,false);
      randomDirection = (rand() % 2 == 1);
      randomAngle = rand()%360;
      // std::cout << "randomDirection : "<< randomDirection << '\n';
      // std::cout << "randomAngle : "<< randomAngle << '\n';
      rotate(degrees2radians(50),degrees2radians(randomAngle),randomDirection);
      haveTouchSmtgs = false;
    }
    else{
      move(10, 0.2, true);
    }
    loop_rate.sleep();
  }

  ros::spin();
  return 0;
}

void rosoutCallBack(const rosgraph_msgs::Log & log_message)
{
  if(log_message.level == log_message.WARN && log_message.name == "/turtlesim")
  {
      haveTouchSmtgs = true;
  }
}
/**
  * method to move the robot straight
  * speed : vitesse du robot_cleaner
  * distance : distance a parcourir
  * isForward : si le robot avance ou recule
*/
void move(double speed, double distance, bool isForward) {
  geometry_msgs::Twist vel_msg;

  //set a random linear velocity in the x axis
  if(isForward)
    vel_msg.linear.x = abs(speed);
  else
    vel_msg.linear.x = -abs(speed);

  vel_msg.linear.y = 0;
  vel_msg.linear.z = 0;

  //set the angular velocity in the y axis
  vel_msg.angular.x = 0;
  vel_msg.angular.y = 0;
  vel_msg.angular.z = 0;

  //t0 :currenttime +publish the velocity and estimate the distance by loop and then the current distance moved
  //                                            (speed * t1-t0)
  double t0 = ros::Time::now().toSec();
  double current_ditance =0;
  ros::Rate loop_rate(100);
  while (current_ditance < distance) {
    velocity_publisher.publish(vel_msg);
    double t1 = ros::Time::now().toSec();
    current_ditance = speed * (t1 - t0);
    ros::spinOnce();
    loop_rate.sleep();
  }
  vel_msg.linear.x = 0;
  velocity_publisher.publish(vel_msg);

}

/**
  * method to rotate the turtle
  * @param angular_speed : vitesse angulaire
  * @param angle: valeur de l'angle en radian
  * @param clockwise : sens horloge de la rotation
*/
void rotate(double angular_speed, double angle,bool clockwise)
{
    geometry_msgs::Twist vel_msg;

    //set random linear
    vel_msg.linear.x = 0;
    vel_msg.linear.y = 0;
    vel_msg.linear.z = 0;

    //set random angular
    vel_msg.angular.x = 0;
    vel_msg.angular.y = 0;

    //gestion du clockwise
    if (clockwise)
    {
      vel_msg.angular.z = -fabs(angular_speed);
      // vel_msg.angular.z =10;
    }
    else
    {
      // vel_msg.angular.z =-10;
      vel_msg.angular.z = fabs(angular_speed);
    }

    double current_angle= 0.0;
    double t0 = ros::Time::now().toSec();
    ros::Rate loop_rate(1000);

    do {
      velocity_publisher.publish(vel_msg);
      // std::cout << "vel_msg : "<< vel_msg << '\n';
      double t1 = ros::Time::now().toSec();
      current_angle = angular_speed * (t1 - t0);
      ros::spinOnce();
      loop_rate.sleep();
    }while(current_angle < angle);

    vel_msg.angular.z = 0;
    velocity_publisher.publish(vel_msg);


}

double degrees2radians(double angle_in_degree)
{
  return angle_in_degree * PI/180.0;
}

/**
 *  turns the robot to a desried absolute angle
 */
double setDesiredOrientation(double desired_angle_radians)
{
	double relative_angle_radians = desired_angle_radians - turtlesim_pose.theta;
	//if we want to turn at a perticular orientation, we subtract the current orientation from it
	bool clockwise = ((relative_angle_radians<0)?true:false);
	//cout<<desired_angle_radians <<","<<turtlesim_pose.theta<<","<<relative_angle_radians<<","<<clockwise<<endl;
	rotate (fabs(relative_angle_radians), fabs(relative_angle_radians), clockwise);
}

/**
 *  callback function to update the pose of the robot
 */

void poseCallback(const turtlesim::Pose::ConstPtr & pose_message){
	turtlesim_pose.x=pose_message->x;
	turtlesim_pose.y=pose_message->y;
	turtlesim_pose.theta=pose_message->theta;
}

void moveGoal(turtlesim::Pose goal_pose, double distance_tolerance){
	//We implement a Proportional Controller. We need to go from (x,y) to (x',y'). Then, linear velocity v' = K ((x'-x)^2 + (y'-y)^2)^0.5 where K is the constant and ((x'-x)^2 + (y'-y)^2)^0.5 is the Euclidian distance. The steering angle theta = tan^-1(y'-y)/(x'-x) is the angle between these 2 points.
	geometry_msgs::Twist vel_msg;

	ros::Rate loop_rate(10);
	do{
		//linear velocity
		vel_msg.linear.x = 1.5*getDistance(turtlesim_pose.x, turtlesim_pose.y, goal_pose.x, goal_pose.y);
		vel_msg.linear.y = 0;
		vel_msg.linear.z = 0;
		//angular velocity
		vel_msg.angular.x = 0;
		vel_msg.angular.y = 0;
		vel_msg.angular.z = 4*(atan2(goal_pose.y - turtlesim_pose.y, goal_pose.x - turtlesim_pose.x)-turtlesim_pose.theta);

		velocity_publisher.publish(vel_msg);

		ros::spinOnce();
		loop_rate.sleep();

	}while(getDistance(turtlesim_pose.x, turtlesim_pose.y, goal_pose.x, goal_pose.y)>distance_tolerance);
	std::cout<<"end move goal"<<std::endl;
	vel_msg.linear.x = 0;
	vel_msg.angular.z = 0;
	velocity_publisher.publish(vel_msg);

}

double getDistance(double x1, double y1, double x2, double y2){
	return sqrt(pow((x2-x1),2) + pow((y2-y1),2));
}
