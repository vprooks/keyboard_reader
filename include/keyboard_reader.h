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

#include <linux/input.h>
#include <string.h>
#include <map>
#include <vector>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <glob.h>	// for counting files in a dir (needed for counting event files in /dev/input)
#include <sstream>

#ifndef KEYBOARD_READER_H
#define KEYBOARD_READER_H

/** Opens the input device and checks whether its meaningful name (ie, EVIOCGNAME in ioctl()) contains substrings specified in valid_substring.
 *  @param dev file name of a linux event.
 *  @param mode open file mode; O_RDONLY will do.
 *  @return file descriptor if open and other checks have been succesfully passed; -1 otherwise.
 */
int open_keyboard(const char *dev, int mode);

/** Goes through all the event files in /dev/input/ to locate keyboard.
 *  @param mode open file mode.
 *  @return file descriptor if all checks out; -1 otherwise.
 */
int find_keyboard(int mode);

/** Searches the input event for type EV_KEY and returns event code (ie, which key) and value (ie, pressed or depressed).
 *  @param ev input event.
 *  @return vector containing two unsigned integers: event code (based on linux/input.h) and event value (1 for pressed, 0 for depressed); returns {0, 0} if no EV_KEY.
 */
std::vector <uint16_t> process_event(struct input_event *ev);

/** Reads event data, and returns relevant info only for EV_KEY events, and dismisses anything that is not a key being pressed or depressed.
 *  @param fd file descriptor for keyboard event file.
 *  @return vector containing two unsigned integers: event code (based on linux/input.h) and event value (1 for pressed, 0 for depressed).
 */
std::vector <uint16_t> get_key_event(int fd);

/** Return a string corresponding to a key code.
 *  @param key_code uint corresponding to key on a keyboard
 *  @return string corresponding to a key code
 */
std::string get_mapped_key_string(uint16_t key_code);

#endif