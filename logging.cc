#include "logging.h"

#include <sstream>
#include <iostream>
#include <string>
#include <iomanip>
using std::string;
using std::ostringstream;
using std::setw;

base::Logger::~Logger(){
}

LogFileObject::LogFileObject(int severity,
                             const char* base_filename)
:base_filename_("traflog"),
 file_(NULL) {
}

LogFileObject::~LogFileObject(){
    MutexLock l(&file_lock_);
    if (file_ != NULL) {
        fclose(file_);
        file_ = NULL;
    }
}

void LogFileObject::Write(time_t timestamp,
                          const char* message,
                          int message_len){

    MutexLock l(&file_lock_);

    //do nothing when filename is null
    if (base_filename_.empty()) {
        return;
    }

    // create file when file is null
    // filename is base_filename+timestamp
    //
    if (file_ == NULL){
        string time_pid_string = get_time_pid_string(timestamp);
        if ( !CreateLogFile(time_pid_string) ){
            perror("Could not create log file");
            fprintf(stderr, "COULD NOT CREATE LOGFILE '%s'!\n",
                time_pid_string.c_str());
            return;
        }

        //TODO write file header into file log

    }

// Write to LOG file
  if ( !stop_writing ) {
    // fwrite() doesn't return an error when the disk is full, for
    // messages that are less than 4096 bytes. When the disk is full,
    // it returns the message length for messages that are less than
    // 4096 bytes. fwrite() returns 4096 for message lengths that are
    // greater than 4096, thereby indicating an error.
    errno = 0;
    fwrite(message, 1, message_len, file_);
    if ( errno == ENOSPC ) {  // disk full, stop writing to disk
      stop_writing = true;  // until the disk is
      return;
    } else {
      file_length_ += message_len;
      bytes_since_flush_ += message_len;
    }
  } else {
     //need to check if disk has free space
  }

  //TODO if force flush , buffer size>n or time cycle? flush
  // here we flush every time
  FlushUnLock();

}

void LogFileObject::SetBasename(const char* basename){
}

void LogFileObject::Flush(){
    MutexLock l(&file_lock_);
    FlushUnLock();
}

void LogFileObject::FlushUnLock(){
    if (file_ != NULL) {
        fflush(file_);
        bytes_since_flush_ = 0;
    }
}

bool LogFileObject::CreateLogFile(const string& time_pid_string){
    string file_name = base_filename_ + time_pid_string;
    //file_ = fopen(file_name.c_str(), "w");
    file_ = fopen("/home/haolin/haolinlog", "w");

    if( file_ == NULL){
        return false;
    }

    return true;

}

int LogFileObject::LogSize(){
    return file_length_;
};

string LogFileObject::get_time_pid_string(time_t timestamp){
    struct ::tm tm_time;
    localtime_r(&timestamp, &tm_time);

    // The logfile's filename will have the date/time & pid in it
    ostringstream time_pid_stream;
    time_pid_stream.fill('0');
    time_pid_stream << 1900+tm_time.tm_year
                    << setw(2) << 1+tm_time.tm_mon
                    << setw(2) << tm_time.tm_mday
                    << '-'
                    << setw(2) << tm_time.tm_hour
                    << setw(2) << tm_time.tm_min
                    << setw(2) << tm_time.tm_sec
                    << '.'
                    << getpid();
    const string& time_pid_string = time_pid_stream.str();

    return time_pid_string;
}
