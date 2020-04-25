#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <iterator>

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
  string os, kernel;
  string line;
  string version;
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
  /**
  The file /proc/meminfo is of the following format, and I think we can calculate the memory Utilization 
  using the formulae (MemTotal - MemFree) / MemTotal
  
  MemTotal:       16425388 kB
  MemFree:        12342008 kB
  MemAvailable:   15365336 kB
  Buffers:          658992 kB
  Cached:          2265892 kB
  SwapCached:            0 kB
  Active:          1751788 kB
  Inactive:        1618480 kB
  Active(anon):     444280 kB
  Inactive(anon):    10676 kB
  Active(file):    1307508 kB
  Inactive(file):  1607804 kB
  **/
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  std::string line;
  int MemTotal;
  int MemFree;
  
  std::string name;
  int memory;
  std::string kb;
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> name >> memory >> kb;
      if (name == "MemTotal:"){
          MemTotal = memory;
      } else if (name == "MemFree:") {
          MemFree = memory;
//           std::cout << float(MemTotal - MemFree) / float(MemTotal) << "\n";
          return float(MemTotal - MemFree) / float(MemTotal);
      }
	};    
  };
  
  return 0.0; 
} 

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  /**
  The file /proc/uptime has the following contents
  
  1099.11 842.47
  
  This file contains two numbers (values in seconds): the uptime
  of the system (including time spent in suspend) and the amount
  of time spent in the idle process.
  
  Therefore we need to extract the first number. 
  */
  
  long total_uptime = 0;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::string line;
    
    
    long idle_time;
    
  	std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> total_uptime >> idle_time;
    
    return total_uptime;
  } else {
  	return total_uptime;
  }
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  
  long jiffies = 0;
  
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::string line;
    std::getline(filestream, line);
    std::stringstream linestream(line);
    
    // Referring to https://rosettacode.org/wiki/Linux_CPU_utilization
    linestream.ignore(5, ' '); // Skip the 'cpu' prefix.
//     std::vector<size_t> times;
    for (long time; linestream >> time; jiffies += time);
    
//     std::cout << "Total jiffies: " << jiffies;
    
    return jiffies;
  }
  
  
  return 0; 


}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
// long LinuxParser::ActiveJiffies(int pid) { 
//   /**
//   The content of the /prod/[pid]/stat file looks like this. 
  
//   40 (node) S 39 1 1 0 -1 4194560 13339 0 10 0 97 14 0 0 20 0 11 0 3009 1271238656 12594 18446744073709551615 4194304 33169308 140727177963744 0 0 0 0 4096 16896 0 0 0 17 3 0 0 56 0 0 35266560 35374480 40251392 140727177968961 140727177968982 140727177968982 140727177969636 0
  
//   As per the documentation, these have information like PID, The filename of the executable, in parentheses, State, ppid ... etc
  
//   However, for the sake of this function, we need the fields utime(14), stime(15), cutime(16), cstime(17) and add them to get the active jiffies for the PID. 
//   */
  
//   string kPidDirectory = '/' + std::to_string(pid) + '/';
  
//   std::ifstream filestream(kProcDirectory + kPidDirectory + kStatFilename);
  
//   if (filestream.is_open()) {
//     std::string line;
//     std::getline(filestream, line);
    
//     std::istringstream iss(line);
//     // Referred to the following link to split a string on spaces https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
//     std::vector<std::string> results(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>()); // Split the line on spaces
    
//     //Note: without using .c_str() we get some typecasting error. So we have to explicitely convert them to strings before converting to long
//     return long(results[13].c_str()) + long(results[14].c_str()) + long(results[15].c_str()) + long(results[16].c_str());
    
//   }
//   return 0; 

// }


long LinuxParser::ActiveJiffies(int pid) {
  /**
  The above commented function was my implementation, but there was some bug that I could not resolve, because of which I was getting weird numbers in the 
  Process CPU Utilization section. I have put up a question in Knowledge as well reqgarding the same problem that I was facing, but I did not get proper response
  from any technical mentor. 
  
  So, I checked online, and found an implementation and took reference from it. Following is the link.
  
  https://github.com/jpmarques19/CppND-SystemMonitor/blob/master/src/linux_parser.cpp
  
  */
  
  string line;
  string placeholder;
  long jiffies = 0;
  long process_jiffies = 0;
  string kPidDirectory = '/' + std::to_string(pid);
  std::ifstream filestream(kProcDirectory + kPidDirectory + kStatFilename);
  
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    
    for (int token_id = 1; token_id <= 17; ++token_id) {
      
      if (token_id == 14 ||
          token_id == 15 ||
          token_id == 16 ||
          token_id == 17) {
        linestream >> jiffies;
        process_jiffies += jiffies;
      } else {
        linestream >> placeholder;
      }
    }
  }
  return process_jiffies;
}


// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
	return Jiffies() - IdleJiffies();
};

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
	
  long idle_jiffies = 0;
  std::vector<long> times;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::string line;
    std::getline(filestream, line);
    std::stringstream linestream(line);
    
    // Referring to https://rosettacode.org/wiki/Linux_CPU_utilization
    linestream.ignore(5, ' '); // Skip the 'cpu' prefix.
    for (long time; linestream >> time; times.push_back(time));
    
    idle_jiffies = times[3]; // As per the man page, the 4th time represents the idle jiffies. http://man7.org/linux/man-pages/man5/proc.5.html
  }
  
  return idle_jiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
	
  std::string line;
  std::string key;
  
  int processes = 0;
  
  std::ifstream filestream(kProcDirectory + kStatFilename);
  
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "processes") {
          linestream >> processes;
          break;
      }
    }
  }
   return processes;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 

  std::string line;
  std::string key;
  
  int running_processes = 0;
  
  std::ifstream filestream(kProcDirectory + kStatFilename);
  
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "procs_running") {
          linestream >> running_processes;
          break;
      }
    }
  }
   return running_processes;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  /**
  This information is found in the /proc/[pid]/cmdline file. 
  */
  string cmd;
  string kPidDirectory = '/' + std::to_string(pid);
  std::ifstream filestream(kProcDirectory + kPidDirectory + kCmdlineFilename);
  
  if (filestream.is_open()) {
    std::getline(filestream, cmd);
  }
  
  return cmd;

}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
/**
this information can be found in the /proc/[pid]/status file. And as per the man docs, we are conserned with the "VmSize" key

NSpgid: 17248
NSsid:  17200
VmPeak:     131168 kB
VmSize:     131168 kB
VmLck:           0 kB
VmPin:           0 kB
VmHWM:       13484 kB
VmRSS:       13484 kB
RssAnon:     10264 kB
RssFile:      3220 kB

*/

	
  
  long ram = 0;
  
  string kPidDirectory = '/' + std::to_string(pid) + '/';
  std::ifstream filestream(kProcDirectory + kPidDirectory + kStatusFilename);
  if (filestream.is_open()) {
    string line;
  
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      string key;
      
      linestream >> key;
      if (key == "VmSize:") {
        linestream >> ram;
        break;
      }
    }
  }
  
  // Convert the RAM from KB to MB, since we are displaying in MB
  // Also, since the return type is string, we need to convert the long int to string, otherwise it gives error. 
  return std::to_string(ram/1000);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  /**
  Similar to the RAM function, this information is also found in the prod/[pid]/status file
  
  The only difference is that, here, we need the key "Uid". Which is of the following form. We just need the first number (real uid)
  
  Uid:    1000    1000    1000    1000
  */
  
  string uid;
  
  string kPidDirectory = '/' + std::to_string(pid) + '/';
  std::ifstream filestream(kProcDirectory + kPidDirectory + kStatusFilename);
  if (filestream.is_open()) {
    string line;
  
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      string key;
      
      linestream >> key;
      if (key == "Uid:") {
        linestream >> uid;
        break;
      }
    }
  }
  
  
  return uid; }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  /**
  The Uid above needs to be mapped with the User name which is stored in the /etc/passwd file. 
  The entried in that file are of the following format. Here we can see that, root has Uid 0, daemon has Uid 1 bin's Uid is 2 and so on.
  
  root:x:0:0:root:/root:/bin/bash
  daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
  bin:x:2:2:bin:/bin:/usr/sbin/nologin
  sys:x:3:3:sys:/dev:/usr/sbin/nologin
  sync:x:4:65534:sync:/bin:/bin/sync
  games:x:5:60:games:/usr/games:/usr/sbin/nologin
  
  */
  
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    string line;
    
    while (std::getline(filestream, line)) {
      
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream iss(line);

//       std::vector<std::string> results(std::istream_iterator<std::string>{iss},
//                                      std::istream_iterator<std::string>());
		
      // the following variables are taken with reference to the documentation https://www.ibm.com/support/knowledgecenter/ssw_aix_71/security/passwords_etc_passwd_file.html
      
      string user;
      string enc_password;
      string uid;
      
      iss >> user >> enc_password >> uid;
      
      if (uid == Uid(pid)){
      	return user;
      }
    }
  }
  
  return "";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  /**
  This information is available in the /proc/[pid]/stat file with the key starttime (23 idx)
  
  */
  long start_time;

  string kPidDirectory = '/' + std::to_string(pid) + '/';
  
  std::ifstream filestream(kProcDirectory + kPidDirectory + kStatFilename);
  
  if (filestream.is_open()) {
    std::string line;
    std::getline(filestream, line);
    
    std::istringstream iss(line);
    string placeholder;
    
    for (int idx = 1; idx <= 22; ++idx) {
      if (idx == 22) {
        iss >> start_time;
      } else {
        iss >> placeholder;
      }
    }
  }
  return start_time / sysconf(_SC_CLK_TCK);

}


// long LinuxParser::UpTime(int pid) {
//   string line;
//   string placeholder;
//   long start_time = 0;
//   string kPidDirectory = '/' + std::to_string(pid);
//   std::ifstream filestream(kProcDirectory + kPidDirectory + kStatFilename);
//   if (filestream.is_open()) {
//     std::getline(filestream, line);
//     std::istringstream linestream(line);
//     for (int token_id = 1; token_id <= 22; ++token_id) {
//       if (token_id == 22) {
//         linestream >> start_time;
//       } else {
//         linestream >> placeholder;
//       }
//     }
//   }
//   return start_time / sysconf(_SC_CLK_TCK);
// }