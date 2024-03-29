#include "carpc/tools/Tools.hpp"
#include "ConnectionProcessor.hpp"

#include "carpc/trace/Trace.hpp"
#define CLASS_ABBR "MAIN"



using tStart = void (*)( int, char**, char** );
using tExit = void (*)( void );

void preinit( int argc, char** argv, char** envp )
{
   carpc::tools::PCE configuration( argc, argv, envp );
   configuration.print( );

   const carpc::trace::eLogStrategy trace_strategy = carpc::trace::log_strategy_from_string(
         configuration.value_or( "trace_log", "CONSOLE" ).c_str( )
      );
   const std::size_t trace_buffer = static_cast< std::size_t >( std::stoll(
         configuration.value_or( "trace_buffer", "4096" )
      ) );
   const std::string trace_app_name = configuration.value_or( "trace_app_name", "SBR" );
   const carpc::trace::eLogLevel trace_level = carpc::trace::log_level_from_number(
         std::stoll(
            configuration.value_or( "trace_level", "6" )
         )
      );
   carpc::trace::Logger::init( trace_strategy, trace_app_name, trace_buffer, trace_level );

   MSG_INF( "preinit_array" );
}
__attribute__(( section(".preinit_array") )) tStart __preinit__ = preinit;

void init( int argc, char** argv, char** envp )
{
   MSG_INF( "init_array" );
}
__attribute__(( section(".init_array") )) tStart __init__ = init;

void fini( )
{
   MSG_INF( "fini_array" );
}
__attribute__(( section(".fini_array") )) tExit __fini__ = fini;

void __constructor__( ) __attribute__(( constructor(101) ));
void __constructor__( )
{
   MSG_INF( "constructor" );
}

void __destructor__( ) __attribute__(( destructor(101) ));
void __destructor__( )
{
   MSG_INF( "destructor" );
}



int main( int argc, char** argv, char** envp )
{
   carpc::tools::PCE configuration( argc, argv, envp );
   configuration.print( );

   carpc::os::os_linux::socket::configuration socket_configuration{
      carpc::os::os_linux::socket::socket_domain_from_string(
            configuration.value( "ipc_servicebrocker_domain" ).value( ).c_str( )
         ),
      carpc::os::os_linux::socket::socket_type_from_string(
            configuration.value( "ipc_servicebrocker_type" ).value( ).c_str( )
         ),
      static_cast< int >( std::stoll( configuration.value( "ipc_servicebrocker_protocole" ).value( ) ) ),
      configuration.value( "ipc_servicebrocker_address" ).value( ),
      static_cast< int >( std::stoll( configuration.value( "ipc_servicebrocker_port" ).value( ) ) )
   };

   std::size_t buffer_size = static_cast< std::size_t >( std::stoll(
         configuration.value( "ipc_servicebrocker_buffer_size" ).value( )
      ) );

   ConnectionProcessor conn( socket_configuration, buffer_size );
   conn.init( );
   conn.connection_loop( );



   return 0;
}
