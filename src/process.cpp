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

// : Return this process's ID
int Process::Pid() const { return pid_; }



// : Return this process's CPU utilization //changed to const to use it when overloading the < operator !! also changed in process.h
float Process::CpuUtilization() const{ 
  
  float activeTime = LinuxParser::ActiveJiffies(Pid()) / (float)sysconf(_SC_CLK_TCK);
  float process_startTime = (float)LinuxParser::UpTime(Pid()); 
  float system_uptime = (float) LinuxParser::UpTime();
  float process_uptime = system_uptime - process_startTime;
  float CPUutil = activeTime/process_uptime;
  return CPUutil;

}


// : Return the command that generated this process
string Process::Command() { 
  return LinuxParser::Command(Pid()); 
}

// : Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// : Return the user (name) that generated this process
string Process::User() { 
  return LinuxParser::User(Pid()); 
}

// : Return the age of this process (in seconds)
long int Process::UpTime() { 
  float process_startTime = (float)LinuxParser::UpTime(Pid()); 
  float system_uptime = (float) LinuxParser::UpTime();
  float process_uptime = system_uptime - process_startTime;
  return process_uptime;
}

// : Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
  return a.CpuUtilization() < CpuUtilization();
}