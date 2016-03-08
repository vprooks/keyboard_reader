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