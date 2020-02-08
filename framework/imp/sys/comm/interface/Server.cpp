#include "api/sys/service/ServiceThread.hpp"
#include "api/sys/service/ServiceProcess.hpp"
#include "api/sys/comm/interface/InterfaceRegistry.hpp"
#include "api/sys/comm/interface/Server.hpp"

#include "api/sys/trace/Trace.hpp"
#define CLASS_ABBR "BaseServer"



namespace base {



Server::Server( const std::string& name, const std::string& role, const bool is_ipc )
   : Interface( name, role, is_ipc, Interface::eType::server )
{
   SYS_TRC( "created" );
   register_server( this );
}

Server::~Server( )
{
   SYS_TRC( "destroyed" );
   unregister_server( this );
}

void Server::connected( const Client* const p_client ) const
{
   mp_client = p_client;
}

void Server::disconnected( const Client* const p_client ) const
{
   mp_client = nullptr;
}



} // namespace base
