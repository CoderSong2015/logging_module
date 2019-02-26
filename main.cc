#include "logging.h"
#include <sys/time.h>

int CycleClock_Now() {
  // TODO(hamaji): temporary impementation - it might be too slow.
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return static_cast<int>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

double WallTime_Now() {
  // Now, cycle clock is retuning microseconds since the epoch.
  return CycleClock_Now() * 0.000001;
}

int main(){

    double now = WallTime_Now();
    time_t timestamp_ = static_cast<time_t>(now);
    LogFileObject lf(1, "testFile");
    lf.Write(timestamp_, "song", 4);

    return 0;
}
