#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
  // time units
  int hours_ = seconds/3600;
  int minutes_ = (seconds % 3600) / 60;
  int seconds_ = (seconds % 3600) % 60;
  
  // taken from reviewer suggestion from submission 1 and modified
  string sec = std::to_string(seconds_);
  if (sec.length() < 2) {
    sec.insert(0, 2 - sec.length(), '0');
  }
  
  string mins = std::to_string(minutes_);
  if (mins.length() < 2) {
  	mins.insert(0, 2 - mins.length(), '0');
  }

  string hrs = std::to_string(hours_);
  if (hrs.length() < 2) {
  hrs.insert(0, 2 - hrs.length(), '0');
  }
  std::string result = hrs + ":" + mins + ":" + sec;
  return result;
}