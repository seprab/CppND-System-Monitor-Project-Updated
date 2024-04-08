#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization()
{
  auto totalJiffies = LinuxParser::Jiffies();
  auto activeJiffies = LinuxParser::ActiveJiffies();
  return activeJiffies / totalJiffies;
}