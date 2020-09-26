// Framework
#include "api/sys/service/ServiceProcess.hpp"
#include "api/sys/tools/Tools.hpp"
// Application
#include "imp/app/components/OnOff/Component.hpp"
#include "imp/app/components/Driver/Component.hpp"
#include "imp/app/components/Master/Component.hpp"
#include "imp/app/components/Slave/Component.hpp"

#include "api/sys/trace/Trace.hpp"
#define CLASS_ABBR "MAIN"



namespace memory {
   #ifdef HOOK_MEMORY_ALLOCATOR
      extern void dump( );
      extern void set_track_size( const size_t );
   #else
      inline void dump( ) { }
      inline void set_track_size( const size_t ) { }
   #endif
}



#ifdef USE_INSTRUMENTAL
   #include "api/sys/asm/cpu.hpp"
   extern "C" {
      void __cyg_profile_func_enter( void* this_fn, void* call_site ) __attribute__(( no_instrument_function ));
      void __cyg_profile_func_exit( void* this_fn, void* call_site ) __attribute__(( no_instrument_function ));

      void __cyg_profile_func_enter( void* this_fn, void* call_site )
      {
         printf( "ENTER: %p, <-- %p: %lld\n", this_fn, call_site, __rdtsc( ) );
      }
      void __cyg_profile_func_exit( void* this_fn, void* call_site )
      {
         printf( "EXIT:  %p, --> %p: %lld\n", this_fn, call_site, __rdtsc( ) );
      }
   }
#endif



const bool test( int argc, char** argv );

void boot( int argc, char** argv )
{
   memory::dump( );
   DBG_MSG( "SIGRTMIN = %d / SIGRTMAX = %d", SIGRTMIN, SIGRTMAX );

   base::tools::cmd::init( argc, argv );
   base::tools::cmd::print( );
   base::tools::cfg::init( base::tools::cmd::argument( "config" ).value_or( "application.cfg" ) );
   base::tools::cfg::print( );

   REGISTER_EVENT( base::onoff::ipc::OnOffEvent );
   REGISTER_EVENT( application::events::AppEvent );
   DUMP_IPC_EVENTS;

   base::ServiceThread::Info::tVector services =
   {
        { "OnOff_Service", { application::components::onoff::Component::creator }, 5 }
      , { "Driver_Service", { application::components::driver::Component::creator }, 10 }
      , { "Device_Service", { application::components::master::Component::creator, application::components::slave::Component::creator }, 10 }
   };

   base::ServiceProcess::tSptr p_process = base::ServiceProcess::instance( );
   if( p_process->start( services ) )
   {
      DBG_MSG( "Booting..." );
      p_process->boot( );
   }

   DBG_TRC( "Main: program exiting." );
   DBG_INF( "Main: program exiting." );
   DBG_MSG( "Main: program exiting." );
   DBG_WRN( "Main: program exiting." );
   DBG_ERR( "Main: program exiting." );

   memory::dump( );
}



#if OS == LINUX

   int main( int argc, char* argv[ ] )
   {
      DBG_MSG( "argc = %d", argc );

      if( test( argc, argv ) )
         boot( argc, argv );

      return 0;
   }

#elif OS == ANDROID

   #include <jni.h>
   #include "api/sys/oswrappers/Thread.hpp"

   base::os::Thread boot_thread __attribute__ (( section ("THREAD"), init_priority (101) )) = { boot };

   void __constructor__( ) __attribute__(( constructor(102) ));
   void __destructor__( ) __attribute__(( destructor(102) ));

   void __constructor__( ) { DBG_INF( "library loaded" ); }
   void __destructor__( ) { DBG_INF( "library unloaded" ); }

   extern "C" JNIEXPORT jstring JNICALL
   Java_com_tda_framework_MainActivity_jniStartFramework( JNIEnv* env, jobject /* this */ )
   {
      DBG_TRC( "JNI" );
      boot_thread.run( );

      return env->NewStringUTF( "@TDA: Hello from C++" );
   }

#endif


























#include "test/sys/math/TMatrix.hpp"
#include "test/other/nn/nn.hpp"
#include "api/sys/helpers/functions/print.hpp"
#include "api/sys/helpers/functions/format.hpp"

#include "api/app/proto/SensorData.pb.h"
#include <fstream>
namespace proto = com::tda::sensor::protos;



const bool test( int argc, char** argv )
{
   return true;
   SYS_ERR( "--------------- MARKER ---------------" );






   // base::math::tmatrixd::test::run( );
   // other::nn::test::run( );



   // proto::GeneralData general_data;
   // general_data.mutable_signdata( )->mutable_position( )->set_latitude( 0.123 );
   // general_data.mutable_signdata( )->mutable_position( )->set_longitude( 0.456 );
   // general_data.mutable_signdata( )->mutable_offset( )->set_x( 12.0 );
   // general_data.mutable_signdata( )->mutable_offset( )->set_y( 3.0 );
   // general_data.mutable_signdata( )->mutable_offset( )->set_z( 2.0 );
   // general_data.mutable_signdata( )->set_location( proto::SignLocation::LeftLocation );
   // general_data.mutable_signdata( )->set_type( proto::SignType::SpeedLimitSign );
   // general_data.mutable_signdata( )->set_detectedtime( 1234567890 );
   // general_data.mutable_cardata( )->set_softwareversion( "1.0.0" );
   // general_data.mutable_cardata( )->set_mapversion( "3.0.0" );
   // general_data.mutable_cardata( )->set_uuid( "12-34-56-78-90" );
   // general_data.mutable_cardata( )->set_vin( "A12DE45CF" );
   // general_data.mutable_cardata( )->set_speed( 54 );

   // std::ofstream ofs( "/home/scorpion/Desktop/sensor.data", std::ios_base::out | std::ios_base::binary );
   // general_data.SerializeToOstream( &ofs );






   return false;
}
