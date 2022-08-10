#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>

using std::vector;
using std::string;



// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  vector<string> values = LinuxParser::CpuUtilization();
  
  float user = stof(values[0]);
  float nice = stof(values[1]);
  float system = stof(values[2]);
  float idle = stof(values[3]);
  float iowait = stof(values[4]);
  float irq = stof(values[5]);
  float softirq = stof(values[6]);
  float steal = stof(values[7]);


  float prevIdleTime = previdle + previowait;
  float Idle = idle + iowait;
  
  float prevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
  float nonIdle = user + nice + system + irq + softirq + steal;
  
  float prevTotalTime = prevIdleTime + prevNonIdle;
  float Total = Idle + nonIdle;
  
  //process diff betw previous and now logic from https://github.com/donsylen/CppND-System-Monitor-Updated/blob/main/src/processor.cpp
  float totald = Total - prevTotalTime;
  float idled = Idle - prevIdleTime;
  float util_diff = (totald - idled) / totald;
  
  
  //set prev idle & active times to current value for next call
  previdle = idle;
  previowait = iowait;
  prevuser = user;
  prevnice = nice;
  prevsystem = system;
  previrq = irq;
  prevsoftirq = softirq;
  prevsteal = steal;
  
  return util_diff; 

}