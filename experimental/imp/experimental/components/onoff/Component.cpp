#include "imp/experimental/components/onoff/servers/OnOff.hpp"
#include "imp/experimental/components/onoff/Component.hpp"

#include "api/sys/trace/Trace.hpp"
#define CLASS_ABBR "Component_OnOff"



using namespace component::onoff;



std::unique_ptr< fw::component::Base > Component::creator( )
{
   return std::make_unique< Component >( );
}

Component::Component( )
   : fw::component::Base( "OnOff" )
{
   mp_onoff_server = servers::OnOff::create( );
   mp_onoff_server->check_in( );

   // events::OnOffEvent::create( 12345, { "Starting" } )->send( );
}

Component::~Component( )
{
}
