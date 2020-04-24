#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {

int hours, minutes;

minutes = seconds / 60;
hours = minutes / 60;
  
return std::to_string(int(hours)) + ":" + std::to_string(int(minutes%60)) + ":" + std::to_string(int(seconds%60));
  

}