#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const {
    // similar concept to active jiffies(pid) from linux_parser.cpp
    string line;
    string value;
    float cpu_time = 0.0f;
    float cpu_util = 0.0f;
    float total_time = 0.0f;

    std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid_) + LinuxParser::kStatFilename);
    if (filestream.is_open()) {
        std::getline(filestream, line);
        std::istringstream linestream(line);
        vector<string> values;

        while (linestream >> value) {
            values.push_back(value);
        }

        // Check if values vector has enough elements to prevent out-of-bounds access
        if (values.size() > 21) {
            // active indices in the individual are 13,14,15,16
            vector<int> active_indices = {13, 14, 15, 16};
            for (const int& index : active_indices) {
                cpu_time += std::stol(values[index]);
            }

            // Get system uptime and process start time
            float system_uptime = LinuxParser::UpTime();
            float process_start_time = std::stol(values[21]) / sysconf(_SC_CLK_TCK);

            // Calculate total time the process has been running
            total_time = system_uptime - process_start_time;

            if (total_time > 0) {
                // Calculate CPU utilization
                cpu_util = (cpu_time / sysconf(_SC_CLK_TCK)) / total_time;
            }
        }
    }

    return cpu_util;
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
  	// had some issues rrunning this after changing for resubmission due to invalid pids, 
  	// had to add this check in to make it run
    if (pid_ <= 0) {
          return 0;
      }
    
    long int system_uptime = LinuxParser::UpTime();
    long int process_uptime = LinuxParser::UpTime(pid_);
	
  	return system_uptime - process_uptime;
}

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return (a.CpuUtilization() < this->CpuUtilization()); }
