// Application
#include "imp/nav/components/Slave/Slave.hpp"

#include "api/sys/trace/Trace.hpp"
#define CLASS_ABBR "Slave"



namespace application::slave {



base::ComponentPtr creator( base::ServiceThreadPtr p_service )
{
   return std::make_shared< Slave >( p_service, "Slave" );
}



Slave::Slave( const base::ServiceThreadPtr p_service, const std::string& name )
   : base::Component( p_service, name )
{
   DBG_MSG( "Created: %s", base::Component::name( ).c_str( ) );
   events::PingSlaveEvent::Event::set_notification( this );
}

Slave::~Slave( )
{
   DBG_MSG( "Destroyed: %s", name( ).c_str( ) );
   events::PingSlaveEvent::Event::clear_notification( this );
}

void Slave::process_event( const events::PingSlaveEvent::Event& event )
{
   DBG_TRC( "type = %#zx, info = %s", static_cast< size_t >( event.data( )->type ), event.data( )->info.c_str( ) );
   switch( event.data( )->type )
   {
      case events::ePing::ping:
      {
         sleep( 15 );
         events::PingSlaveEvent::Event::create_send( { events::ePing::response, "Master <- Slave" }, base::eCommType::ETC );
         break;
      }
      case events::ePing::response:
      {
         break;
      }
      default:
      {
         break;
      }
   }
}


} // namespace application::slave
