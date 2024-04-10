#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization()
{
  float utilization{0};
  auto totalJiffies = LinuxParser::Jiffies();
  auto activeJiffies = LinuxParser::ActiveJiffies();
  utilization = activeJiffies / totalJiffies;
  return utilization;
}