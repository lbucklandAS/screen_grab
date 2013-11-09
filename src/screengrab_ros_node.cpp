#include "ros/ros.h"
#include "sensor_msgs/Image.h"

// X Server includes
#include <X11/Xlib.h>
#include <X11/Xutil.h>

int main(int argc, char **argv)
{
  ros::init(argc, argv, "screengrab_ros_node");

  ros::NodeHandle nh;

  ros::Publisher screen_pub = nh.advertise<sensor_msgs::Image>(
      "screengrab", 5);
  // TBD make teh rate a ros param
  ros::Rate loop_rate(10);

  // X resources
  Display* display;
  Screen* screen;
  XImage* xImageSample;
  XColor col;

  int screen_w;
  int screen_h;

  // init
  // from vimjay screencap.cpp (https://github.com/lucasw/vimjay)
  {
  display = XOpenDisplay(NULL); // Open first (-best) display
  if (display == NULL) {
    ROS_ERROR_STREAM("bad display");
    return -1;
  }

  screen = DefaultScreenOfDisplay(display);
  if (screen == NULL) {
    ROS_ERROR_STREAM("bad screen");
    return -2;
  }

  Window wid = DefaultRootWindow( display );
  if ( 0 > wid ) {
    ROS_ERROR_STREAM("Failed to obtain the root windows Id "
        "of the default screen of given display.\n");
    return -3;
  }

  XWindowAttributes xwAttr;
  Status ret = XGetWindowAttributes( display, wid, &xwAttr );
  screen_w = xwAttr.width;
  screen_h = xwAttr.height;
  }


  while (ros::ok()) 
  {
    sensor_msgs::ImagePtr im; //(new sensor_msgs::Image);
    
    // grab the image
    {
      int startX = 0;
      int startY = 0;
      int widthX = 640;
      int heightY = 480;

      // Need to check against resolution
      if ((startX + widthX) > screen_w) {
        // TBD need to more intelligently cap these
        if (screen_w > widthX) {
          startX = screen_w - widthX;
        } else {
          startX = 0;
          widthX = screen_w;
        }
      }

      if ((startY + heightY) > screen_h) {
        // TBD need to more intelligently cap these
        if (screen_h > heightY) {
          startY = screen_h - heightY;
        } else {
          startY = 0;
          heightY = screen_h;
        }
      }

      xImageSample = XGetImage(display, DefaultRootWindow(display),
          startX, startY, widthX, heightY, AllPlanes, ZPixmap);

      // Check for bad null pointers
      if (xImageSample == NULL) {
        ROS_ERROR_STREAM("Error taking screenshot!");
        continue;
      }
      
      // convert to Image format
      
      XDestroyImage(xImageSample);

    }
    
    // screen_pub.publish(im);

    ros::spinOnce();
    loop_rate.sleep();
  }

  return 0;
}