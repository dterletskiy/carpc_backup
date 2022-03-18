// Application
#include "imp/symbiont/components/onoff/Component.hpp"

#include "api/sys/trace/Trace.hpp"
#define CLASS_ABBR "OnOff"



using namespace symbiont::components::onoff;



carpc::application::IComponent::tSptr Component::creator( )
{
   return std::shared_ptr< Component >( new Component( "OnOff" ) );
}

Component::Component( const std::string& _name )
   : carpc::application::RootComponent( _name )
{
   MSG_DBG( "Created: %s", name( ).c_str( ) );
   events::AppEvent::Event::set_notification( this, events::eAppEventID::BOOT );
   events::AppEvent::Event::set_notification( this, events::eAppEventID::SHUTDOWN );
   events::AppEvent::Event::set_notification( this, events::eAppEventID::PING );
}

Component::~Component( )
{
   MSG_DBG( "Destroyed: %s", name( ).c_str( ) );
   events::AppEvent::Event::clear_all_notifications( this );
}

#include <thread>
void Component::boot( const std::string& command )
{
   MSG_DBG( "%s", command.c_str( ) );

   // if( true == m_client_someip.init( ) )
   //    m_client_someip.start( );

   std::thread thread(
      [ this ]( )
      {
         if( m_client_someip.init( ) ) m_client_someip.start( );
      }
   );
   thread.detach( );
}

void Component::process_event( const events::AppEvent::Event& event )
{
   MSG_DBG( "message = %s", event.data( )->message.c_str( ) );

   switch( event.info( ).id( ) )
   {
      case events::eAppEventID::BOOT:
      {
         m_client_someip.run_test( );
         break;
      }
      default: break;
   }
}
