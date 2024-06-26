#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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

float LinuxParser::MemoryUtilization()
{
  float memTotal, memFree, buffers;
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "MemTotal")
        {
          memTotal = std::stof(value);
          continue;
        }
        else if (key == "MemFree")
        {
          memFree = std::stof(value);
          continue;
        }
        else if (key == "Buffers")
        {
          buffers = std::stof(value);
          break;
        }
      }
    }
  }
  return (memTotal-memFree)/memTotal;
}

long LinuxParser::UpTime()
{
  string line;
  string value;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream stream(line);
    if(stream>>value)
    {
      return std::stol(value);
    }
  }
  return 0;
}

long LinuxParser::Jiffies()
{
  return LinuxParser::IdleJiffies() + LinuxParser::ActiveJiffies();
}

long LinuxParser::ActiveJiffies()
{
  vector<string> jiffies = CpuUtilization();
  return std::stol(jiffies[CPUStates::kUser_])+
         std::stol(jiffies[CPUStates::kSystem_])+
         std::stol(jiffies[CPUStates::kNice_])+
         std::stol(jiffies[CPUStates::kIRQ_])+
         std::stol(jiffies[CPUStates::kSoftIRQ_])+
         std::stol(jiffies[CPUStates::kSteal_]);
}

long LinuxParser::IdleJiffies()
{
  vector<string> jiffies = CpuUtilization();
  return std::stol(jiffies[CPUStates::kIdle_])+
         std::stol(jiffies[CPUStates::kIOwait_]);
}

vector<string> LinuxParser::CpuUtilization()
{
  string line, user, nice, system, idle, iowait, irq, softirq, steal,
      guest, guest_nice, title;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    if(linestream >> title >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice)
    {
      std::vector<std::string> elements{user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice};
      return elements;
    }
  }
  return {};
}

int LinuxParser::TotalProcesses()
{
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      if (linestream >> key >> value)
      {
        if (key == "processes")
        {
          return std::stoi(value);
          continue;
        }
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses()
{
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "procs_running")
        {
          return std::stoi(value);
        }
      }
    }
  }
  return 0;
}


long LinuxParser::ActiveJiffies(int pid)
{
  string line, key;
  vector<string> values;
  std::ifstream  filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> key)
    {
      values.push_back(key);
    }
  }
  long utime = std::stol(values[13]);
  long stime = std::stol(values[14]);
  long cutime = std::stol(values[15]);
  long cstime = std::stol(values[16]);
  return (utime + stime + cutime + cstime) / sysconf(_SC_CLK_TCK);
}

string LinuxParser::Command(int pid)
{
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open())
  {
    std::getline(filestream, line);
    return line;
  }
  return string();
}

string LinuxParser::Ram(int pid)
{
  string line, key, value;
  std::ifstream  filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "VmSize:")
        {
          return value;
        }
      }
    }
  }
  return "0";
}

string LinuxParser::Uid(int pid)
{
  string line, key, value;
  std::ifstream  filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "Uid:")
        {
          return value;
        }
      }
    }
  }
  return string();
}

string LinuxParser::User(int pid)
{
  string line, key, value, uid;
  std::ifstream  filestream(kPasswordPath);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::replace(line.begin(), line.end(), ':',' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value >> uid)
      {
        if (uid == LinuxParser::Uid(pid))
        {
          return key;
        }
      }
    }
  }
  return string();
}

long LinuxParser::UpTime(int pid)
{
  string line, key;
  vector<string> values;
  std::ifstream  filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> key)
    {
      values.push_back(key);
    }
    return LinuxParser::UpTime() - (std::stol(values[21])/ sysconf(_SC_CLK_TCK));
  }
  return 0;
}
