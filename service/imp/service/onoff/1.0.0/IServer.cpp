#include "IServer.hpp"



using namespace service::onoff::V1_0_0;



IServer::IServer( const std::string& role_name )
   : carpc::service::SERVICE_NAMESPACE::TServer< data::Types >( role_name, true )
{
}

void IServer::response_trigger_state( const bool result )
{
   response< data::ResponseTriggerStateData >( result );
}

void IServer::process_request_event( const tService::Event& event )
{
   switch( event.info( ).id( ) )
   {
      case eOnOff::RequestStart:
      {
         request_start( );
         break;
      }
      case eOnOff::RequestTriggerState:
      {
         if( const data::RequestTriggerStateData* request_data = get_event_data< data::RequestTriggerStateData >( event ) )
         {
            request_trigger_state( request_data->state, request_data->delay );
         }
         break;
      }
      default: break;
   }
}

void IServer::current_state( const std::string& _current_state )
{
   notify< data::NotificationCurrentStateData >( _current_state );
}

const std::string& IServer::current_state( ) const
{
   if( auto p_data = attribute< data::NotificationCurrentStateData >( ) )
      return p_data->state;

   static const std::string state = "";
   return state;
}