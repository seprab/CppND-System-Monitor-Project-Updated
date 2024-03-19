#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu()
{
  return cpu_;
}

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes()
{
  vector<Process> processes;
  vector<int> pids = LinuxParser::Pids();
  for(int pid : pids)
  {
    Process process(pid);
    processes.push_back(process);
  }
  processes_ = processes;
  return processes_;
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel()
{
  std::string kernel = LinuxParser::Kernel();
  return kernel;
}

// TODO: Return the system's memory utilization
float System::MemoryUtilization()
{
  return LinuxParser::MemoryUtilization();
}

// TODO: Return the operating system name
std::string System::OperatingSystem()
{
  std::string os = LinuxParser::OperatingSystem();
  return os;
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses()
{
  return 0; }

// TODO: Return the total number of processes on the system
int System::TotalProcesses()
{
  return processes_.size();
}

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return 0; }