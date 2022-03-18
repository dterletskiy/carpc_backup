#include "api/sys/dsi/Types.hpp"
#include "api/sys/application/Process.hpp"
#include "imp/sys/application/SendReceive.hpp"
#include "imp/sys/application/ServiceEventConsumer.hpp"

#include "api/sys/trace/Trace.hpp"
#define CLASS_ABBR "ServiceEventConsumer"



namespace ev_i = carpc::events::service;
using namespace carpc::application;



ServiceEventConsumer::ServiceEventConsumer( SendReceive* const p_send_receive )
   : mp_send_receive( p_send_receive )
{
   ev_i::Action::Event::set_notification( this, { ev_i::eAction::RegisterServer } );
   ev_i::Action::Event::set_notification( this, { ev_i::eAction::UnregisterServer } );
   ev_i::Action::Event::set_notification( this, { ev_i::eAction::RegisterClient } );
   ev_i::Action::Event::set_notification( this, { ev_i::eAction::UnregisterClient } );
}

ServiceEventConsumer::~ServiceEventConsumer( )
{
   ev_i::Action::Event::clear_all_notifications( this );
}

void ServiceEventConsumer::process_event( const ev_i::Action::Event& event )
{
   const auto& id = event.info( ).id( );
   const carpc::service::Passport& service_passport = *(event.data( ));
   SYS_INF( "id: %s / passport: %s", ev_i::c_str( id ), service_passport.name( ).c_str( ) );

   dsi::eCommand command = dsi::eCommand::Undefined;
   switch( id )
   {
      case ev_i::eAction::RegisterServer:
      {
         command = dsi::eCommand::RegisterServer;
         break;
      }
      case ev_i::eAction::UnregisterServer:
      {
         command = dsi::eCommand::UnregisterServer;
         break;
      }
      case ev_i::eAction::RegisterClient:
      {
         command = dsi::eCommand::RegisterClient;
         break;
      }
      case ev_i::eAction::UnregisterClient:
      {
         command = dsi::eCommand::UnregisterClient;
         break;
      }
      default: break;
   }

   if( dsi::eCommand::Undefined == command )
      return;

   const dsi::SocketCongiguration configuration = static_cast< dsi::SocketCongiguration >( Process::instance( )->configuration( ).ipc_app.socket );
   dsi::Packet packet( command, service_passport, configuration );
   const bool result = mp_send_receive->send( packet, application::Context::invalid );
   SYS_INF( "result = %d", result );
}
