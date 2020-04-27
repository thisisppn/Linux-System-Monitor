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


// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

//  Return a container composed of the system's processes
vector<Process>& System::Processes() { 
  vector<Process> processes;
  vector<int> pids = LinuxParser::Pids();
  processes_ = {};  // reset the process vector
  for (int pid: pids){
    // Initialize all the as process objects and return
    Process process;
    
    process.setPid(pid);
    
    // WRF https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
    
    // cpu_uptime - start_time (process)
    long seconds = LinuxParser::UpTime() - LinuxParser::UpTime(pid);
    float cpu_utlization = ((float)LinuxParser::ActiveJiffies(pid) / sysconf(_SC_CLK_TCK)) / (float)seconds;
    
    process.setCpuUtilization(cpu_utlization);
    
    processes_.push_back(process);
  
  }
  std::sort(processes_.begin(),processes_.end());
  return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }