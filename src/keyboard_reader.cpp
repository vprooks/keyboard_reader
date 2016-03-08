// Copyright (c) 2015, The University of Texas at Austin
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the <organization> nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
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

/** 
 *	NOTE: If you get permission denied when starting this node. Use ' ls -l /dev/input/event* ' to learn which group can access the events.
 *	Then add your username to this group with ' sudo usermod -a -G group_name user_name '
 */

#include "keyboard_reader.h"

#define NUM_VALID_SUBSTRINGS 2
#define BUFFER_SIZE 64

/** Array of substrings that would indicate the selected device is a keyboard. */
static const char *valid_substring[NUM_VALID_SUBSTRINGS] = {
  "keyboard",
  "Keyboard"
};

/** Map key event code to key event string. */
std::map<uint16_t, std::string> keymap = {
 {KEY_RESERVED, "Reserved"}, {KEY_ESC, "Esc"},
 {KEY_1, "1"}, {KEY_2, "2"},
 {KEY_3, "3"}, {KEY_4, "4"},
 {KEY_5, "5"}, {KEY_6, "6"},
 {KEY_7, "7"}, {KEY_8, "8"},
 {KEY_9, "9"}, {KEY_0, "0"},
 {KEY_MINUS, "Minus"}, {KEY_EQUAL, "Equal"},
 {KEY_BACKSPACE, "Backspace"},  {KEY_TAB, "Tab"},
 {KEY_Q, "Q"}, {KEY_W, "W"},
 {KEY_E, "E"}, {KEY_R, "R"},
 {KEY_T, "T"}, {KEY_Y, "Y"},
 {KEY_U, "U"}, {KEY_I, "I"},
 {KEY_O, "O"}, {KEY_P, "P"},
 {KEY_LEFTBRACE, "LeftBrace"},  {KEY_RIGHTBRACE, "RightBrace"},
 {KEY_ENTER, "Enter"}, {KEY_LEFTCTRL, "LeftControl"},
 {KEY_A, "A"}, {KEY_S, "S"},
 {KEY_D, "D"}, {KEY_F, "F"},
 {KEY_G, "G"}, {KEY_H, "H"},
 {KEY_J, "J"}, {KEY_K, "K"},
 {KEY_L, "L"}, {KEY_SEMICOLON, "Semicolon"},
 {KEY_APOSTROPHE, "Apostrophe"}, {KEY_GRAVE, "Grave"},
 {KEY_LEFTSHIFT, "LeftShift"},  {KEY_BACKSLASH, "BackSlash"},
 {KEY_Z, "Z"}, {KEY_X, "X"},
 {KEY_C, "C"}, {KEY_V, "V"},
 {KEY_B, "B"}, {KEY_N, "N"},
 {KEY_M, "M"}, {KEY_COMMA, "Comma"},
 {KEY_DOT, "Dot"}, {KEY_SLASH, "Slash"},
 {KEY_RIGHTSHIFT, "RightShift"}, {KEY_KPASTERISK, "KPAsterisk"},
 {KEY_LEFTALT, "LeftAlt"},  {KEY_SPACE, "Space"},
 {KEY_CAPSLOCK, "CapsLock"},  {KEY_F1, "F1"},
 {KEY_F2, "F2"}, {KEY_F3, "F3"},
 {KEY_F4, "F4"}, {KEY_F5, "F5"},
 {KEY_F6, "F6"}, {KEY_F7, "F7"},
 {KEY_F8, "F8"}, {KEY_F9, "F9"},
 {KEY_F10, "F10"}, {KEY_NUMLOCK, "NumLock"},
 {KEY_SCROLLLOCK, "ScrollLock"}, {KEY_KP7, "KP7"},
 {KEY_KP8, "KP8"}, {KEY_KP9, "KP9"},
 {KEY_KPMINUS, "KPMinus"},  {KEY_KP4, "KP4"},
 {KEY_KP5, "KP5"}, {KEY_KP6, "KP6"},
 {KEY_KPPLUS, "KPPlus"},  {KEY_KP1, "KP1"},
 {KEY_KP2, "KP2"}, {KEY_KP3, "KP3"},
 {KEY_KP0, "KP0"}, {KEY_KPDOT, "KPDot"},
 {KEY_ZENKAKUHANKAKU, "Zenkaku/Hankaku"}, {KEY_102ND, "102nd"},
 {KEY_F11, "F11"}, {KEY_F12, "F12"},
 {KEY_RO, "RO"}, {KEY_KATAKANA, "Katakana"},
 {KEY_HIRAGANA, "HIRAGANA"},  {KEY_HENKAN, "Henkan"},
 {KEY_KATAKANAHIRAGANA, "Katakana/Hiragana"}, {KEY_MUHENKAN, "Muhenkan"},
 {KEY_KPJPCOMMA, "KPJpComma"},  {KEY_KPENTER, "KPEnter"},
 {KEY_RIGHTCTRL, "RightCtrl"},  {KEY_KPSLASH, "KPSlash"},
 {KEY_SYSRQ, "SysRq"}, {KEY_RIGHTALT, "RightAlt"},
 {KEY_LINEFEED, "LineFeed"},  {KEY_HOME, "Home"},
 {KEY_UP, "Up"}, {KEY_PAGEUP, "PageUp"},
 {KEY_LEFT, "Left"}, {KEY_RIGHT, "Right"},
 {KEY_END, "End"}, {KEY_DOWN, "Down"},
 {KEY_PAGEDOWN, "PageDown"},  {KEY_INSERT, "Insert"},
 {KEY_DELETE, "Delete"},  {KEY_MACRO, "Macro"},
 {KEY_MUTE, "Mute"}, {KEY_VOLUMEDOWN, "VolumeDown"},
 {KEY_VOLUMEUP, "VolumeUp"},  {KEY_POWER, "Power"},
 {KEY_KPEQUAL, "KPEqual"},  {KEY_KPPLUSMINUS, "KPPlusMinus"},
 {KEY_PAUSE, "Pause"}, {KEY_KPCOMMA, "KPComma"},
 {KEY_HANGUEL, "Hanguel"},  {KEY_HANJA, "Hanja"},
 {KEY_YEN, "Yen"}, {KEY_LEFTMETA, "LeftMeta"},
 {KEY_RIGHTMETA, "RightMeta"},  {KEY_COMPOSE, "Compose"},
 {KEY_STOP, "Stop"}, {KEY_AGAIN, "Again"},
 {KEY_PROPS, "Props"}, {KEY_UNDO, "Undo"},
 {KEY_FRONT, "Front"}, {KEY_COPY, "Copy"},
 {KEY_OPEN, "Open"}, {KEY_PASTE, "Paste"},
 {KEY_FIND, "Find"}, {KEY_CUT, "Cut"},
 {KEY_HELP, "Help"}, {KEY_MENU, "Menu"},
 {KEY_CALC, "Calc"}, {KEY_SETUP, "Setup"},
 {KEY_SLEEP, "Sleep"}, {KEY_WAKEUP, "WakeUp"},
 {KEY_FILE, "File"}, {KEY_SENDFILE, "SendFile"},
 {KEY_DELETEFILE, "DeleteFile"}, {KEY_XFER, "X-fer"},
 {KEY_PROG1, "Prog1"}, {KEY_PROG2, "Prog2"},
 {KEY_WWW, "WWW"}, {KEY_MSDOS, "MSDOS"},
 {KEY_COFFEE, "Coffee"},  {KEY_DIRECTION, "Direction"},
 {KEY_CYCLEWINDOWS, "CycleWindows"}, {KEY_MAIL, "Mail"},
 {KEY_BOOKMARKS, "Bookmarks"},  {KEY_COMPUTER, "Computer"},
 {KEY_BACK, "Back"}, {KEY_FORWARD, "Forward"},
 {KEY_CLOSECD, "CloseCD"},  {KEY_EJECTCD, "EjectCD"},
 {KEY_EJECTCLOSECD, "EjectCloseCD"}, {KEY_NEXTSONG, "NextSong"},
 {KEY_PLAYPAUSE, "PlayPause"},  {KEY_PREVIOUSSONG, "PreviousSong"},
 {KEY_STOPCD, "StopCD"},  {KEY_RECORD, "Record"},
 {KEY_REWIND, "Rewind"},  {KEY_PHONE, "Phone"},
 {KEY_ISO, "ISOKey"}, {KEY_CONFIG, "Config"},
 {KEY_HOMEPAGE, "HomePage"},  {KEY_REFRESH, "Refresh"},
 {KEY_EXIT, "Exit"}, {KEY_MOVE, "Move"},
 {KEY_EDIT, "Edit"}, {KEY_SCROLLUP, "ScrollUp"},
 {KEY_SCROLLDOWN, "ScrollDown"}, {KEY_KPLEFTPAREN, "KPLeftParenthesis"},
 {KEY_KPRIGHTPAREN, "KPRightParenthesis"}, {KEY_F13, "F13"},
 {KEY_F14, "F14"}, {KEY_F15, "F15"},
 {KEY_F16, "F16"}, {KEY_F17, "F17"},
 {KEY_F18, "F18"}, {KEY_F19, "F19"},
 {KEY_F20, "F20"}, {KEY_F21, "F21"},
 {KEY_F22, "F22"}, {KEY_F23, "F23"},
 {KEY_F24, "F24"}, {KEY_PLAYCD, "PlayCD"},
 {KEY_PAUSECD, "PauseCD"},  {KEY_PROG3, "Prog3"},
 {KEY_PROG4, "Prog4"}, {KEY_SUSPEND, "Suspend"},
 {KEY_CLOSE, "Close"}, {KEY_PLAY, "Play"},
 {KEY_FASTFORWARD, "Fast Forward"}, {KEY_BASSBOOST, "Bass Boost"},
 {KEY_PRINT, "Print"}, {KEY_HP, "HP"},
 {KEY_CAMERA, "Camera"},  {KEY_SOUND, "Sound"},
 {KEY_QUESTION, "Question"},  {KEY_EMAIL, "Email"},
 {KEY_CHAT, "Chat"}, {KEY_SEARCH, "Search"},
 {KEY_CONNECT, "Connect"},  {KEY_FINANCE, "Finance"},
 {KEY_SPORT, "Sport"}, {KEY_SHOP, "Shop"},
 {KEY_ALTERASE, "Alternate Erase"}, {KEY_CANCEL, "Cancel"},
 {KEY_BRIGHTNESSDOWN, "Brightness down"}, {KEY_BRIGHTNESSUP, "Brightness up"},
 {KEY_MEDIA, "Media"}, {KEY_UNKNOWN, "Unknown"},
 {BTN_0, "Btn0"}, {BTN_1, "Btn1"},
 {BTN_2, "Btn2"}, {BTN_3, "Btn3"},
 {BTN_4, "Btn4"}, {BTN_5, "Btn5"},
 {BTN_6, "Btn6"}, {BTN_7, "Btn7"},
 {BTN_8, "Btn8"}, {BTN_9, "Btn9"},
 {BTN_LEFT, "LeftBtn"}, {BTN_RIGHT, "RightBtn"},
 {BTN_MIDDLE, "MiddleBtn"},  {BTN_SIDE, "SideBtn"},
 {BTN_EXTRA, "ExtraBtn"},  {BTN_FORWARD, "ForwardBtn"},
 {BTN_BACK, "BackBtn"}, {BTN_TASK, "TaskBtn"},
 {BTN_TRIGGER, "Trigger"},  {BTN_THUMB, "ThumbBtn"},
 {BTN_THUMB2, "ThumbBtn2"},  {BTN_TOP, "TopBtn"},
 {BTN_TOP2, "TopBtn2"}, {BTN_PINKIE, "PinkieBtn"},
 {BTN_BASE, "BaseBtn"}, {BTN_BASE2, "BaseBtn2"},
 {BTN_BASE3, "BaseBtn3"},  {BTN_BASE4, "BaseBtn4"},
 {BTN_BASE5, "BaseBtn5"},  {BTN_BASE6, "BaseBtn6"},
 {BTN_DEAD, "BtnDead"}, {BTN_A, "BtnA"},
 {BTN_B, "BtnB"}, {BTN_C, "BtnC"},
 {BTN_X, "BtnX"}, {BTN_Y, "BtnY"},
 {BTN_Z, "BtnZ"}, {BTN_TL, "BtnTL"},
 {BTN_TR, "BtnTR"}, {BTN_TL2, "BtnTL2"},
 {BTN_TR2, "BtnTR2"}, {BTN_SELECT, "BtnSelect"},
 {BTN_START, "BtnStart"},  {BTN_MODE, "BtnMode"},
 {BTN_THUMBL, "BtnThumbL"},  {BTN_THUMBR, "BtnThumbR"},
 {BTN_TOOL_PEN, "ToolPen"},  {BTN_TOOL_RUBBER, "ToolRubber"},
 {BTN_TOOL_BRUSH, "ToolBrush"},  {BTN_TOOL_PENCIL, "ToolPencil"},
 {BTN_TOOL_AIRBRUSH, "ToolAirbrush"}, {BTN_TOOL_FINGER, "ToolFinger"},
 {BTN_TOOL_MOUSE, "ToolMouse"},  {BTN_TOOL_LENS, "ToolLens"},
 {BTN_TOUCH, "Touch"}, {BTN_STYLUS, "Stylus"},
 {BTN_STYLUS2, "Stylus2"},  {BTN_TOOL_DOUBLETAP, "Tool Doubletap"},
 {BTN_TOOL_TRIPLETAP, "Tool Tripletap"}, {BTN_GEAR_DOWN, "WheelBtn"},
 {BTN_GEAR_UP, "Gear up"},  {KEY_OK, "Ok"},
 {KEY_SELECT, "Select"},  {KEY_GOTO, "Goto"},
 {KEY_CLEAR, "Clear"}, {KEY_POWER2, "Power2"},
 {KEY_OPTION, "Option"},  {KEY_INFO, "Info"},
 {KEY_TIME, "Time"}, {KEY_VENDOR, "Vendor"},
 {KEY_ARCHIVE, "Archive"},  {KEY_PROGRAM, "Program"},
 {KEY_CHANNEL, "Channel"},  {KEY_FAVORITES, "Favorites"},
 {KEY_EPG, "EPG"}, {KEY_PVR, "PVR"},
 {KEY_MHP, "MHP"}, {KEY_LANGUAGE, "Language"},
 {KEY_TITLE, "Title"}, {KEY_SUBTITLE, "Subtitle"},
 {KEY_ANGLE, "Angle"}, {KEY_ZOOM, "Zoom"},
 {KEY_MODE, "Mode"}, {KEY_KEYBOARD, "Keyboard"},
 {KEY_SCREEN, "Screen"},  {KEY_PC, "PC"},
 {KEY_TV, "TV"}, {KEY_TV2, "TV2"},
 {KEY_VCR, "VCR"}, {KEY_VCR2, "VCR2"},
 {KEY_SAT, "Sat"}, {KEY_SAT2, "Sat2"},
 {KEY_CD, "CD"}, {KEY_TAPE, "Tape"},
 {KEY_RADIO, "Radio"}, {KEY_TUNER, "Tuner"},
 {KEY_PLAYER, "Player"},  {KEY_TEXT, "Text"},
 {KEY_DVD, "DVD"}, {KEY_AUX, "Aux"},
 {KEY_MP3, "MP3"}, {KEY_AUDIO, "Audio"},
 {KEY_VIDEO, "Video"}, {KEY_DIRECTORY, "Directory"},
 {KEY_LIST, "List"}, {KEY_MEMO, "Memo"},
 {KEY_CALENDAR, "Calendar"},  {KEY_RED, "Red"},
 {KEY_GREEN, "Green"}, {KEY_YELLOW, "Yellow"},
 {KEY_BLUE, "Blue"}, {KEY_CHANNELUP, "ChannelUp"},
 {KEY_CHANNELDOWN, "ChannelDown"}, {KEY_FIRST, "First"},
 {KEY_LAST, "Last"}, {KEY_AB, "AB"},
 {KEY_NEXT, "Next"}, {KEY_RESTART, "Restart"},
 {KEY_SLOW, "Slow"}, {KEY_SHUFFLE, "Shuffle"},
 {KEY_BREAK, "Break"}, {KEY_PREVIOUS, "Previous"},
 {KEY_DIGITS, "Digits"},  {KEY_TEEN, "TEEN"},
 {KEY_TWEN, "TWEN"}, {KEY_DEL_EOL, "Delete EOL"},
 {KEY_DEL_EOS, "Delete EOS"},  {KEY_INS_LINE, "Insert line"},
 {KEY_DEL_LINE, "Delete line"}
};

/** Return a string corresponding to a key code.
 *  @param key_code uint corresponding to key on a keyboard
 *  @return string corresponding to a key code
 */
std::string get_mapped_key_string(uint16_t key_code) {
    return keymap[key_code];
}

/** Opens the input device and checks whether its meaningful name (ie, EVIOCGNAME in ioctl()) contains substrings specified in valid_substring.
 *  @param dev file name of a linux event.
 *  @param mode open file mode; O_RDONLY will do.
 *  @return file descriptor if open and other checks have been succesfully passed; -1 otherwise.
 */
int open_keyboard(const char *dev, int mode) {
  printf("Opening device: %s \n", dev);
  int fd = open(dev, mode);	// file descriptor to the opened device
  char name[255];		// meaningful, ie EVIOCGNAME name

  if(fd < 0){			// if failed to open dev
    fprintf(stderr, "Unable to open \"%s\": %s\n", dev, strerror(errno));
    return -1;
  }

  /* NOTE: If your keyboard does not have substring specified in valid_substring in its EVIOCNAME, uncomment following line */
//   return fd;


  // Following is a fail-safe to avoid opening non-keyboard event by checking if the input dev has valid_substring in its name.

  if(ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0){ 		// fetches the meaningful (ie. EVIOCGNAME) name
    fprintf(stderr, "\"%s\": EVIOCGNAME failed: %s\n", dev, strerror(errno));
    close(fd);
    return -1;							// returns -1 if unable to fetch the meaningful name
  }
  
  std::ostringstream sstream;
  sstream << name;						// convert char* to stringstream
  std::string namestring = sstream.str();			// stringstream to string
  int i;
  for (i=0; i < NUM_VALID_SUBSTRINGS; i++) {
    std::size_t found = namestring.find(valid_substring[i]);	// does the meaningful name contain accepted substring
    if (found!=std::string::npos) {
      printf("Found '%s' device. Starting to read ... \n", name);
      return fd;						// if everything checks out, returns the file descriptor
    } // end if
  } // end for

  close(fd);
  printf("%s does not seem to be a keyboard.\n", dev);
  return -1;
} // end open_keyboard

/** Goes through all the event files in /dev/input/ to locate keyboard.
 *  @param mode open file mode.
 *  @return file descriptor if all checks out; -1 otherwise.
 */
int find_keyboard(int mode) {

  int i, r;

  // using glob() [see: http://linux.die.net/man/3/glob ] for getting event files in /dev/input/
  glob_t gl;
  int num_event_dev = 0;					// number of relevant event files found in /dev/input/
  if(glob("/dev/input/by-*/*kbd", 0, NULL, &gl) == 0)		// looks for filenames that match the given pattern
    num_event_dev = gl.gl_pathc;				// get the number of matching event files

  printf("\x1b[1;32mHere is the list of likely candiates for your keyboard. This function always starts with the \x1b[4mfirst one on the list.\x1b[0m ");
  printf("\x1b[1;33mIf nothing happens when you press keyboard keys, terminate this process and re-start with user-specified device.\n\x1b[0m");
  for(i=0; i<num_event_dev; i++){
    printf("[%d] %s \n",i, gl.gl_pathv[i]);			// print all the paths that were found by glob()
  }
  
  for(i=0; i<num_event_dev; i++){				// go through all relevant event files
    r = open_keyboard(gl.gl_pathv[i], mode);			// try to open an event file
    if(r >= 0) {						// if open_keyboard was succesful
      globfree(&gl);						// free memory allocated for globe struct
      return r;
    } // if
  } // for
  globfree(&gl);						// free memory allocated for globe struct

  return -1;							// return error -1 otherwise
} // end find_keyboard

/** Searches the input event for type EV_KEY and returns event code (ie, which key) and value (ie, pressed or depressed).
 *  @param ev input event.
 *  @return vector containing two unsigned integers: event code (based on linux/input.h) and event value (1 for pressed, 0 for depressed); returns {0, 0} if no EV_KEY.
 */
std::vector <uint16_t> process_event(struct input_event *ev) {

  std::vector <uint16_t> event;			// output vector
  
  switch(ev->type){				// switch to a case based on the event type
    case EV_SYN:				// this event is always present but no need to do anything
//       printf("EV_SYN: code=0x%04x, value=0x%08x\n", ev->code, ev->value);
      break; 
    case EV_MSC:				// this event is always present but no need to do anything
//       printf("EV_MSC: code=0x%04x, value=0x%08x\n", ev->code, ev->value);
      break;
    case EV_KEY:				// key event means that a key was either pressed or depressed
      if (ev->value == 1) {			// a key was pressed
	printf("A key was pressed: code=0x%04x, value=0x%08x\n", ev->code, ev->value);
      } else {					// a key was depressed
	printf("A key was depressed: code=0x%04x, value=0x%08x\n", ev->code, ev->value);
      }
      event.push_back(ev->code);		// push event code to output vector
      event.push_back(ev->value);		// push event value to output vector
//       printf("Decimal event_code=%d \n", event[0]);
      return event;				// return output vector containing event code and value
      break;
    default:					// default case
      printf("Warning: unexpected event type; ev->type = 0x%04x\n", ev->type);
  } // end switch

  return {0, 0};				// return vector with two zeros as elements
//   fflush(stdout);
} // end process_event

/** Reads event data, and returns relevant info only for EV_KEY events, and dismisses anything that is not a key being pressed or depressed.
 *  @param fd file descriptor for keyboard event file.
 *  @return vector containing two unsigned integers: event code (based on linux/input.h) and event value (1 for pressed, 0 for depressed).
 */
std::vector <uint16_t> get_key_event(int fd) {

  struct input_event ibuffer[BUFFER_SIZE];				// see: https://www.kernel.org/doc/Documentation/input/input.txt
  int r, events, i;
  std::vector <uint16_t> event_info;					// processed event
  
  r = read(fd, ibuffer, sizeof(struct input_event) * BUFFER_SIZE);	// read events to input_event buffer
  if( r > 0 ) {
      events = r / sizeof(struct input_event);				// getting the number of events
      for(i=0; i<events; i++) {						// going through all the read events
	event_info = process_event(&ibuffer[i]);			// call process_event() for every read event
// 	ROS_INFO("Ready to publish: %d", event_code);
	if (event_info[0] > 0) return event_info;			// return only the code for events different from 0; ie, only when key was pressed or depressed
      } //end for
    } else {
	fprintf(stderr, "read() failed: %s\n", strerror(errno));	// let user know that read() failed
	return {0, 0};
    }
    
  return {0, 0};
} // end get_key_event

