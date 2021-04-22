#include "api/sys/oswrappers/linux/time.hpp"


namespace base::os::os_linux {



void local_time_of_date( tm*& _time_tm, size_t& _milliseconds )
{
   timeval tv;
   gettimeofday( &tv, NULL );
   _time_tm = localtime( &tv.tv_sec );
   _milliseconds = tv.tv_usec;
}

uint64_t time( const eGranularity gran, clockid_t clk_id )
{
   timespec time_timespec;
   clock_gettime( clk_id, &time_timespec );
   return time_timespec.tv_sec * static_cast< size_t >( gran ) + time_timespec.tv_nsec / ( 1000000000 / static_cast< size_t >( gran ) );
}

uint64_t milliseconds( clockid_t clk_id )
{
   timespec time_timespec;
   clock_gettime( clk_id, &time_timespec );
   return time_timespec.tv_sec * 1000 + time_timespec.tv_nsec / 1000000;
}

uint64_t microseconds( clockid_t clk_id )
{
   timespec time_timespec;
   clock_gettime( clk_id, &time_timespec );
   return time_timespec.tv_sec * 1000000 + time_timespec.tv_nsec / 1000;
}

uint64_t nanoseconds( clockid_t clk_id )
{
   timespec time_timespec;
   clock_gettime( clk_id, &time_timespec );
   return time_timespec.tv_sec * 1000000000 + time_timespec.tv_nsec;
}



} // namespace base::os::os_linux
