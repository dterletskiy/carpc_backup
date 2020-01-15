#include "api/sys/configuration/DSI.hpp"
#include "api/sys/tools/Tools.hpp"
#include "ConnectionProcessor.hpp"

#include "api/sys/trace/Trace.hpp"
#define CLASS_ABBR "MAIN"



namespace memory {
   #ifdef HOOK_MEMORY_ALLOCATOR
      extern void dump( );
   #else
      inline void dump( ) { }
   #endif
}



int main( int argc, char *argv[] )
{
   ConnectionProcessor conn;
   conn.init(
                 base::configuration::dsi::socket_family
               , base::configuration::dsi::socket_type
               , base::configuration::dsi::socket_protocole
               , base::configuration::dsi::server_address
               , base::configuration::dsi::server_port
            );
   conn.connection_loop( );



   return 0;
}
