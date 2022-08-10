#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
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
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// extract process ids
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

// done: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line, key, value;
  float MemTotal, MemFree; //make them ints directly instead of reverting later
  float memUtil;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);//create stream to a file for opening it
  
  if(stream.is_open()) { //check if open)
    while(std::getline(stream, line)){ //as long as a next line in the file the lines below
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line); //pick the line and create a line stream
      while (linestream >> key >> value) { //while we can tokenize the linestream into key value storage pairs extract MemTotal and MemFree as defined below
        if(key == "MemTotal"){
          MemTotal = std::stof(value);
        }
        else if(key == "MemFree"){
          MemFree = std::stof(value);
        }
      }
    }
  }
  memUtil = (MemTotal - MemFree)/MemTotal;
  return memUtil;
}


// Done: Read and return the system uptime //time since last boot
long LinuxParser::UpTime() {
  string UptimeSystem, UptimeIdle;//long because long number of seconds
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> UptimeSystem >> UptimeIdle;
  }
  return std::stol(UptimeSystem);
}

// done: Read and return the number of jiffies for the system
//explanation on Jiffie values taken from https://www.linuxhowtos.org/System/procstat.htm and https://knowledge.udacity.com/questions/218355

long LinuxParser::Jiffies() {
  vector <string> Jiffies = CpuUtilization(); //get the vector of all system CPU processes
  long totalJiffies = stol(Jiffies[CPUStates::kUser_]) + stol(Jiffies[CPUStates::kNice_]) +
         stol(Jiffies[CPUStates::kSystem_]) + stol(Jiffies[CPUStates::kIRQ_]) +
         stol(Jiffies[CPUStates::kSoftIRQ_]) + stol(Jiffies[CPUStates::kSteal_])+
         stol(Jiffies[CPUStates::kGuest_]) + stol(Jiffies[CPUStates::kGuestNice_])+
         stol(Jiffies[CPUStates::kIdle_]) + stol(Jiffies[CPUStates::kIOwait_]); //goes through the whole vector and somes up jiffies of all processes
  return totalJiffies;
}

// done: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  long activeJiffies{0};
  string token, line;
  vector<string> values;
  
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> token){
      values.push_back(token);    
    }
  }
  
  long utime = std::stol(values[13]);
  long stime = std::stol(values[14]);
  //long cutime = std::stol(values[15]);
  //long cstime = std::stol(values[16]);
  //long starttime = std::stol(values[21]); //excluded to avoid warnings
  
  activeJiffies = utime + stime;
  return activeJiffies;
}

// done: Read and return the number of active jiffies for the system
//logic inspired by: https://github.com/donsylen/CppND-System-Monitor-Updated/blob/main/include/linux_parser.cpp
long LinuxParser::ActiveJiffies() { 
  vector <string> Jiffies = CpuUtilization(); //get the vector of all system CPU processes
  long activeJiffies = stol(Jiffies[CPUStates::kUser_]) + stol(Jiffies[CPUStates::kNice_]) +
         stol(Jiffies[CPUStates::kSystem_]) + stol(Jiffies[CPUStates::kIRQ_]) +
         stol(Jiffies[CPUStates::kSoftIRQ_]) + stol(Jiffies[CPUStates::kSteal_])+
         stol(Jiffies[CPUStates::kGuest_]) + stol(Jiffies[CPUStates::kGuestNice_]); //goes through vector for the active time amounts and converts to long
  return activeJiffies;
}

// done: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector <string> Jiffies = CpuUtilization(); //get the vector of all system CPU processes
  long idleJiffies = stol(Jiffies[CPUStates::kIdle_])+stol(Jiffies[CPUStates::kIOwait_]); //goes through vector for the idle time amounts and converts to long
  return idleJiffies;
}


// done: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line, key;
  string user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  vector<string> CPUvalues;
  
  std::ifstream stream(kProcDirectory + kStatFilename);

  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      while(linestream >> key >> user >> nice >> system >> idle >> iowait >>irq >> softirq >> steal >> guest >> guest_nice){
        if(key == "cpu"){
          CPUvalues.push_back(user);
          CPUvalues.push_back(nice);
          CPUvalues.push_back(system);
          CPUvalues.push_back(idle);
          CPUvalues.push_back(iowait);
          CPUvalues.push_back(irq);
          CPUvalues.push_back(softirq);
          CPUvalues.push_back(steal);
          CPUvalues.push_back(guest);
          CPUvalues.push_back(guest_nice);
        }
      }    
    }
  }
  
  return CPUvalues;
}

// done: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, key;
  string value; //make them ints directly instead of reverting later
  int num_proc;
  std::ifstream stream(kProcDirectory + kStatFilename);//create stream to a file for opening it
  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      while(linestream>>key>>value){
        if(key == "processes"){
          num_proc = std::stoi(value);
        }
      }
    }
  }
  return num_proc;
}

// done: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, key;
  string value; //make them ints directly instead of reverting later
  int num_proc;
  std::ifstream stream(kProcDirectory + kStatFilename);//create stream to a file for opening it
  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      while(linestream>>key>>value){
        if(key == "procs_running"){
          num_proc = std::stoi(value);
        }
      }
    }
  }
  return num_proc;
}

// done: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(stream.is_open()){
    std::getline(stream,line);
  }
  return line;
}

// done: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line, key, value, storage;
  string ram;
  long ramMB;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){ //as long as a next line in the file the lines below
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> key >> value >> storage){
        //using VmRSS instead of VmSize to show actual physical RAM reason see: manpages
        if(key == "VmRSS"){
          ram = value;
          ramMB = std::stol(value)/1000;
          ram = std::to_string(ramMB);
        }
      }
    }
  }
  return ram;
}

// done: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key, value;
  string uid;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){ //as long as a next line in the file the lines below
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key == "Uid"){
          uid = value;
        }
      }
    }
  }
  return uid;
}

// done: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  string line, key, value, user, x, UID, user_name;
  
  std::ifstream stream(kPasswordPath);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> user >> x >> UID){
        if(UID == uid){
          user_name = user;
        }
      }
    }
  }
  return user_name;
}

// done: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, token;
  vector <string> values;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    while(linestream >> token){
      values.push_back(token);
    }
  }
  return std::stol(values[21])/sysconf(_SC_CLK_TCK);
}
