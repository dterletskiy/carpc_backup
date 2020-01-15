#include "Master.hpp"

#include "Trace.hpp"
#define CLASS_ABBR "Master"


namespace application::master {



base::IComponentPtr creator( base::IServicePtr p_service )
{
   return std::make_shared< Master >( p_service, "Master" );
}



Master::Master( const base::IServicePtr p_service, const std::string& name )
   : base::Component( p_service, name )
{
   DBG_MSG( "Created: %s", base::Component::name( ).c_str( ) );
   events::PingMasterEvent::set_notification( true, this );
   events::PingSlaveEvent::set_notification( true, this );
}

Master::~Master( )
{
   DBG_MSG( "Destroyed: %s", name( ).c_str( ) );
   events::PingMasterEvent::set_notification( false, this );
   events::PingSlaveEvent::set_notification( false, this );
}

void Master::process_event( const events::PingMasterEvent& event )
{
   // DBG_TRC( "type = %#x, info = %s", event.data( )->type, event.data( )->info.c_str( ) );
   switch( event.data( )->type )
   {
      case events::ePing::ping:
      {
         DBG_MSG( "type = %#x, info = %s", event.data( )->type, event.data( )->info.c_str( ) );
         events::PingSlaveEvent::send_event( { events::ePing::ping, "Master -> Slave" }, base::eCommType::ETC );
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

void Master::process_event( const events::PingSlaveEvent& event )
{
   // DBG_TRC( "type = %#x, info = %s", event.data( )->type, event.data( )->info.c_str( ) );
   switch( event.data( )->type )
   {
      case events::ePing::ping:
      {
         break;
      }
      case events::ePing::response:
      {
         static size_t m_count = 0;
         if( 1 > ++m_count )
         {
            DBG_MSG( "type = %#x, info = %s", event.data( )->type, event.data( )->info.c_str( ) );
            events::PingSlaveEvent::send_event( { events::ePing::ping, "Master -> Slave" }, base::eCommType::ETC );
         }
         else
         {
            m_count = 0;
            DBG_MSG( "type = %#x, info = %s", event.data( )->type, event.data( )->info.c_str( ) );
            events::PingMasterEvent::send_event( { events::ePing::response, "Driver <- Master" }, base::eCommType::ITC );
         }
         break;
      }
      default:
      {
         break;
      }
   }
}



} // namespace application::master
