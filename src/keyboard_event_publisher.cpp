// Copyright (c) 2015, The University of Texas at Austin
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
// 
//     * Neither the name of the copyright holder nor the names of its
//       contributors may be used to endorse or promote products derived from
//       this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "ros/ros.h"
#include "keyboard_reader.h"
#include "keyboard_reader/Key.h"

/** Main function and a ROS publisher */
int main(int argc, char *argv[]) {
  
  ros::init(argc, argv, "keyboard_event_publisher");	// ROS init
  ros::AsyncSpinner spinner(1);				// using async spinner
  spinner.start();					// starting spinner
  
  int keyboard = -1;					// by default, keyboard is considered not opened

  // checking for input arguments
  if(argc == 1) {
    ROS_INFO("No keyboard specified, let's find one.");
    keyboard = find_keyboard(O_RDONLY); 		// if no arguments, find keyboard from existing input events
  } else {
    ROS_INFO("Opening user specified keyboard at: %s", argv[1]);
    keyboard = open_keyboard(argv[1], O_RDONLY);	// open the specified input event
  }

  if(keyboard < 0){					// if failed to open any keyboard input event, print info and exit
    ROS_INFO("Unable to locate keyboard.");
    ROS_INFO("Try: %s [device]\n", argv[0]);
    return 1;
  }

  // ROS node handle
  ros::NodeHandle nh;

  // Creates publisher that advertises Key messages on rostopic /griffin_powermate
  ros::Publisher pub_keyboard = nh.advertise<keyboard_reader::Key>("keyboard", 100);
  
  // Message for publishing key press events
  keyboard_reader::Key key_event;
  // Vector containing event data
  std::vector <uint16_t> event;
  
  while(ros::ok()) {
    event = get_key_event(keyboard);				// get key event
//     ROS_INFO("Ready to publish: %d", event[0]);
    
    // Compose a publishable message
    key_event.key_code = event[0];				// event code
    key_event.key_name = get_mapped_key_string(event[0]);	// string corresponding to event code
    key_event.key_pressed = (bool)event[1];			// true when key is pressed, false otherwise
    if (event[0] > 0) pub_keyboard.publish(key_event);		// publish a Key msg only if event code is greater than zero
    
  } // end while
  
  // Close keyboard event file
  close(keyboard);

  return 0;
} //end main