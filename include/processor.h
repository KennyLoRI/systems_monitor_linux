#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp


  // TODO: Declare any necessary private members
 private:
 float previdle;
 float previowait;
 float prevuser;
 float prevnice;
 float prevsystem;
 float previrq;
 float prevsoftirq;
 float prevsteal;
};

#endif