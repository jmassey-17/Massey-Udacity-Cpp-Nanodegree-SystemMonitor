#include <vector>
#include <string>

#include "processor.h"
#include "linux_parser.h"

using std::string;
using std::vector;
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
	// uses the LinuxParser::CpuUtilization() to select cpu times
  	// then aggregates them here
  
	vector<string> cpu_info = LinuxParser::CpuUtilization();
  	float util = 0.0;
  	float active = 0.0;
  	float idle = 0.0;
  	
    // idle and active jiffies are taken from respective functions in linux parser
    vector<int> active_indicies = {0, 1, 2, 5, 6, 7};
  	vector<int> idle_indicies = {3, 4};
  
  	for (const int& index : active_indicies) {
      active += std::stof(cpu_info[index]);
    }
  	for (const int& index : idle_indicies) {
      idle += std::stof(cpu_info[index]);
    }
  	util = (active)/(active + idle);
  return util;
}