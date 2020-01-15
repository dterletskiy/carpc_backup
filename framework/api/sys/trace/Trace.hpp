#pragma once

#include "api/sys/trace/PrintfTrace.hpp"
#include "api/sys/trace/WriteTrace.hpp"



#ifndef NO_SYS_TRACE
   #define SYS_TYPE DBG_WRITE_MICROSECONDS_PID_TID_CODE
   #define SYS_SIMPLE_TYPE DBG_WRITE_CODE
   #define SYS_EXT_TYPE DBG_WRITE_DATE_TIME_MILLISECONDS_PID_TID_CODE
#else
   #define SYS_TYPE( ... )
   #define SYS_SIMPLE_TYPE( ... )
   #define SYS_EXT_TYPE( ... )
#endif

#ifndef NO_DBG_TRACE
   #define DBG_TYPE DBG_WRITE_MICROSECONDS_PID_TID_CODE
   #define DBG_SIMPLE_TYPE DBG_WRITE_CODE
   #define DBG_EXT_TYPE DBG_WRITE_DATE_TIME_MILLISECONDS_PID_TID_CODE
#else
   #define DBG_TYPE( ... )
   #define DBG_SIMPLE_TYPE( ... )
   #define DBG_EXT_TYPE( ... )
#endif



#define SYS_BG BG_BLACK CURSIVE
#define SYS_TRC( USER_FORMAT, ... )  SYS_TYPE( FG_WHITE         SYS_BG     USER_FORMAT, ##__VA_ARGS__ )
#define SYS_INF( USER_FORMAT, ... )  SYS_TYPE( FG_LIGHT_CYAN    SYS_BG     USER_FORMAT, ##__VA_ARGS__ )
#define SYS_MSG( USER_FORMAT, ... )  SYS_TYPE( FG_LIGHT_GREEN   SYS_BG     USER_FORMAT, ##__VA_ARGS__ )
#define SYS_WRN( USER_FORMAT, ... )  SYS_TYPE( FG_LIGHT_BLUE    SYS_BG     USER_FORMAT, ##__VA_ARGS__ )
#define SYS_ERR( USER_FORMAT, ... )  SYS_TYPE( FG_LIGHT_RED     SYS_BG     USER_FORMAT, ##__VA_ARGS__ )

#define DBG_BG BG_BLACK
#define DBG_TRC( USER_FORMAT, ... )  DBG_TYPE( FG_WHITE         DBG_BG     USER_FORMAT, ##__VA_ARGS__ )
#define DBG_INF( USER_FORMAT, ... )  DBG_TYPE( FG_LIGHT_CYAN    DBG_BG     USER_FORMAT, ##__VA_ARGS__ )
#define DBG_MSG( USER_FORMAT, ... )  DBG_TYPE( FG_LIGHT_GREEN   DBG_BG     USER_FORMAT, ##__VA_ARGS__ )
#define DBG_WRN( USER_FORMAT, ... )  DBG_TYPE( FG_LIGHT_BLUE    DBG_BG     USER_FORMAT, ##__VA_ARGS__ )
#define DBG_ERR( USER_FORMAT, ... )  DBG_TYPE( FG_LIGHT_RED     DBG_BG     USER_FORMAT, ##__VA_ARGS__ )
#define DBG_MSG_SIZEOF( TYPE ) DBG_MSG( "sizeof( %s ) = %zu", #TYPE, sizeof( TYPE ) );



#define SYS_SIMPLE_BG BG_BLACK
#define SYS_SIMPLE_TRC( USER_FORMAT, ... )  SYS_SIMPLE_TYPE( FG_WHITE         SYS_SIMPLE_BG     USER_FORMAT, ##__VA_ARGS__ )
#define SYS_SIMPLE_INF( USER_FORMAT, ... )  SYS_SIMPLE_TYPE( FG_LIGHT_CYAN    SYS_SIMPLE_BG     USER_FORMAT, ##__VA_ARGS__ )
#define SYS_SIMPLE_MSG( USER_FORMAT, ... )  SYS_SIMPLE_TYPE( FG_LIGHT_GREEN   SYS_SIMPLE_BG     USER_FORMAT, ##__VA_ARGS__ )
#define SYS_SIMPLE_WRN( USER_FORMAT, ... )  SYS_SIMPLE_TYPE( FG_LIGHT_BLUE    SYS_SIMPLE_BG     USER_FORMAT, ##__VA_ARGS__ )
#define SYS_SIMPLE_ERR( USER_FORMAT, ... )  SYS_SIMPLE_TYPE( FG_LIGHT_RED     SYS_SIMPLE_BG     USER_FORMAT, ##__VA_ARGS__ )

#define SYS_EXT_BG BG_BLACK
#define SYS_EXT_TRC( USER_FORMAT, ... )  SYS_EXT_TYPE( FG_WHITE         SYS_EXT_BG     USER_FORMAT, ##__VA_ARGS__ )
#define SYS_EXT_INF( USER_FORMAT, ... )  SYS_EXT_TYPE( FG_LIGHT_CYAN    SYS_EXT_BG     USER_FORMAT, ##__VA_ARGS__ )
#define SYS_EXT_MSG( USER_FORMAT, ... )  SYS_EXT_TYPE( FG_LIGHT_GREEN   SYS_EXT_BG     USER_FORMAT, ##__VA_ARGS__ )
#define SYS_EXT_WRN( USER_FORMAT, ... )  SYS_EXT_TYPE( FG_LIGHT_BLUE    SYS_EXT_BG     USER_FORMAT, ##__VA_ARGS__ )
#define SYS_EXT_ERR( USER_FORMAT, ... )  SYS_EXT_TYPE( FG_LIGHT_RED     SYS_EXT_BG     USER_FORMAT, ##__VA_ARGS__ )



#define DBG_SIMPLE_BG BG_BLACK
#define DBG_SIMPLE_TRC( USER_FORMAT, ... )  DBG_SIMPLE_TYPE( FG_WHITE         DBG_SIMPLE_BG     USER_FORMAT, ##__VA_ARGS__ )
#define DBG_SIMPLE_INF( USER_FORMAT, ... )  DBG_SIMPLE_TYPE( FG_LIGHT_CYAN    DBG_SIMPLE_BG     USER_FORMAT, ##__VA_ARGS__ )
#define DBG_SIMPLE_MSG( USER_FORMAT, ... )  DBG_SIMPLE_TYPE( FG_LIGHT_GREEN   DBG_SIMPLE_BG     USER_FORMAT, ##__VA_ARGS__ )
#define DBG_SIMPLE_WRN( USER_FORMAT, ... )  DBG_SIMPLE_TYPE( FG_LIGHT_BLUE    DBG_SIMPLE_BG     USER_FORMAT, ##__VA_ARGS__ )
#define DBG_SIMPLE_ERR( USER_FORMAT, ... )  DBG_SIMPLE_TYPE( FG_LIGHT_RED     DBG_SIMPLE_BG     USER_FORMAT, ##__VA_ARGS__ )

#define DBG_EXT_BG BG_BLACK
#define DBG_EXT_TRC( USER_FORMAT, ... )  DBG_EXT_TYPE( FG_WHITE         DBG_EXT_BG     USER_FORMAT, ##__VA_ARGS__ )
#define DBG_EXT_INF( USER_FORMAT, ... )  DBG_EXT_TYPE( FG_LIGHT_CYAN    DBG_EXT_BG     USER_FORMAT, ##__VA_ARGS__ )
#define DBG_EXT_MSG( USER_FORMAT, ... )  DBG_EXT_TYPE( FG_LIGHT_GREEN   DBG_EXT_BG     USER_FORMAT, ##__VA_ARGS__ )
#define DBG_EXT_WRN( USER_FORMAT, ... )  DBG_EXT_TYPE( FG_LIGHT_BLUE    DBG_EXT_BG     USER_FORMAT, ##__VA_ARGS__ )
#define DBG_EXT_ERR( USER_FORMAT, ... )  DBG_EXT_TYPE( FG_LIGHT_RED     DBG_EXT_BG     USER_FORMAT, ##__VA_ARGS__ )








// @TDA: experiments
#define DBG( USER_FORMAT, ... ) \
   write( STDOUT_FILENO, base::trace::write::simple::p_buffer, \
      sprintf( \
                 base::trace::write::simple::p_buffer \
               , "%s:%s:%d -> " USER_FORMAT \
               , CLASS_ABBR, __FUNCTION__, __LINE__ \
               , ##__VA_ARGS__ \
      ) \
   ); \
   write( STDOUT_FILENO, base::trace::write::simple::p_buffer, sprintf( base::trace::write::simple::p_buffer , NEW_LINE ) );
