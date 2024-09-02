#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}


// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  // /proc/meminfo 
  // look at the top 4 key value pairs - MemTotal, MemFree, MemAvailable, Buffers
  string line;
  string key;
  string value;
  string unit;
  float MemTotal = 0.0;
  float MemFree = 0.0;
  float MemAvailable = 0.0;
  float Buffers = 0.0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value >> unit) {
        if (key == "MemTotal:") {
          MemTotal = std::stof(value);
        }
        else if (key == "MemFree:") {
          MemFree = std::stof(value);
        }
        else if (key == "MemAvailable:") {
          MemAvailable = std::stof(value);
        }
        else if (key == "Buffers:") {
          Buffers = std::stof(value);
        }
        if (MemTotal != 0 && MemFree != 0 && MemAvailable != 0.0 && Buffers != 0) {
          break;
        }
      }
    }
    // calculate memory utilization
    float used_memory = MemTotal - MemFree;
    float memory_utilization = used_memory / MemTotal;
    // return memory utilization
    return memory_utilization;
  }
  return 0.0; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  // /proc/uptime - two values, use them both
  string uptime;
  string idle_time;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idle_time;
  }
  return std::stol(uptime);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  vector<string> cpu_info = CpuUtilization();
  long total_jiffies = 0;
  for (const string& time : cpu_info) {
    total_jiffies += std::stol(time);
  }
  return total_jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string value;
  long jiffies = 0;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    vector<string> values;

    while (linestream >> value) {
      values.push_back(value); 
    }

    // active indices in the individual are 13,14,15,16
    vector<int> active_indicies = {13, 14, 15, 16};
    for (const int& index : active_indicies) {
      jiffies += std::stol(values[index]);
    }
  }
  return jiffies; 
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> cpu_info = CpuUtilization();
  //active jiffies are at index 0,1,2,5,6,7
  vector<int> active_indicies = {0, 1, 2, 5, 6, 7};
  long active_jiffies = 0;
  // sum over index
  for (const int& index : active_indicies) {
    active_jiffies += std::stol(cpu_info[index]);
  }
  return active_jiffies; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_info = CpuUtilization();
  //idle jiffies are at index 3,4
  vector<int> idle_indicies = {3, 4};
  long idle_jiffies = 0;
  // sum over index
  for (const int& index : idle_indicies) {
    idle_jiffies += std::stol(cpu_info[index]);
  }
  return idle_jiffies; 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  // use this to find the jiffies!
  string line;
  string key;
  vector<string> cpu_info;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key;
    //looking for cpu key
    if (key == "cpu") {
      string value;
      while (linestream >> value) {
        cpu_info.push_back(value);
      }
    }
  }
  
  return cpu_info; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  string value;
  int totalProcesses = 0;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      // check for processes
      if (key == "processes") {
        totalProcesses = std::stoi(value);
        return totalProcesses;
      }
    }
  }
  return totalProcesses;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  // /proc/stat key value pair - procs_running 
  string line;
  string key;
  string value;
  int processes_running = 0;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      // check for processes
      if (key == "procs_running") {
        processes_running = std::stoi(value);
        return processes_running;
      }
    }
  }
  return processes_running;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string command_line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, command_line); 
  }
  return command_line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  int ram = 0; 
  string line;
  string key;
  string value;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      // check for processes
      if (key == "VmRSS:") { 
        ram = std::stoi(value) / 1024;
        return to_string(ram);
      }
    }
  }
  return to_string(ram); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {   
  string uid = ""; 
  string line;
  string key;
  string value;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      // check for processes
      if (key == "Uid:") {
        uid = value;
        return uid; 
      }
    }
  }
  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line;
  string user;
  string x; //dummy variable
  string uid = Uid(pid);

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' '); 
      std::istringstream linestream(line);
      linestream >> user >> x >> x >> uid;
      if (uid == Uid(pid)) {
        return user;
      }
    }
  }
  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  long uptime_pid{0};
  string line;
  string uptime;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++) {
      linestream >> uptime;
    }
    uptime_pid = std::stol(uptime) / sysconf(_SC_CLK_TCK);
  }
  return uptime_pid;
}
