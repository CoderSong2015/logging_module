#ifndef __TRAF_LOG_H_
#define __TRAF_LOG_H_

#include <stdio.h>
#include <errno.h>
#include "mutex.h"

namespace base{

class Logger{

 public:
     virtual ~Logger();
     virtual void Write(time_t timestamp,
                        const char* message,
                        int message_len) = 0;
     virtual void Flush() = 0;

     virtual int LogSize() = 0;

};


}

// used for disk is full
static bool stop_writing = false;

class LogFileObject:public base::Logger{
 public:
    LogFileObject(int severity, const char* base_filename);
    ~LogFileObject();
    virtual void Write(time_t timestamp,
                       const char* message,
                       int message_len);

    // Configuration options
    void SetBasename(const char* basename);

    virtual void Flush();

    virtual int LogSize();
    //used when a lock has acquired
    //like forceflush when write
    virtual void FlushUnLock();
 private:
    Mutex file_lock_;
    FILE* file_;
    std::string base_filename_;
    bool CreateLogFile(const std::string& time_pid_string);

    //used for record the file length
    int file_length_;

    //TODO variable used for flush log in buffer at regular intervals

    // message buffer size. set to 0 if flush is called
    int bytes_since_flush_;

    std::string get_time_pid_string(time_t timestamp);

};

#endif __TRAF_LOG_H_
