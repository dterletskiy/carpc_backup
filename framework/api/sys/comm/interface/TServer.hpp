#pragma once

#include "api/sys/comm/interface/IInterface.hpp"

#include "api/sys/trace/Trace.hpp"
#define CLASS_ABBR "TServer"



namespace base {

   enum class eRequestStatus : size_t { BUSY, READY };

   struct RequestStatus
   {
      eRequestStatus status = eRequestStatus::READY;
      SequenceID processing_server_seq_id = InvalidSequenceID;
   };

   struct RequestInfo
   {
      RequestInfo( const SequenceID _server_seq_id )
         : server_seq_id( _server_seq_id )
      { }
      RequestInfo( const SequenceID _server_seq_id, const SequenceID _client_seq_id, const void* _client_addr )
         : server_seq_id( _server_seq_id )
         , client_seq_id( _client_seq_id )
         , client_addr( _client_addr )
      { }
      const bool operator<( const RequestInfo& other ) const { return server_seq_id < other.server_seq_id; }

      SequenceID server_seq_id = InvalidSequenceID;
      SequenceID client_seq_id = InvalidSequenceID;
      const void* client_addr = nullptr;
   };

}



namespace base {



template< typename TYPES >
class TClient;



template< typename TYPES >
class TServer
   : public IInterface
   , public TYPES::tEventConsumer
{
protected:
   using tClient = TClient< TYPES >;
   using tRequestInfoSet = std::set< RequestInfo >;
   using tRequestStatusMap = std::map< typename TYPES::tEventID, RequestStatus >;
   using tRequestInfoMap = std::map< typename TYPES::tEventID, tRequestInfoSet >;

public:
   TServer( const std::string&, const std::string& );
   ~TServer( ) override;

private:
   void connected( const void* const ) override final;
   void disconnected( const void* const ) override final;
private:
   virtual void connected( ) = 0;
   virtual void disconnected( ) = 0;

private:
   std::set< const tClient* > mp_client_set;

protected:
   template< typename tResponseData, typename... Args >
      void create_send_response( const Args&... args );
   template< typename tRequestData >
      const tRequestData* get_event_data( const typename TYPES::tEvent& event );

private:
   void process_event( const typename TYPES::tEvent& ) override final;
   virtual void process_request_event( const typename TYPES::tEvent& ) = 0;
private:
   bool prepare_request( const typename TYPES::tEvent& );
   std::optional< RequestInfo > prepare_response( const typename TYPES::tEventID& );
public:
   const SequenceID unblock_request( );
   void prepare_response( const SequenceID );

protected:
   tRequestStatusMap                         m_request_status_map;
   tRequestInfoMap                           m_request_info_map;
   SequenceID                                m_seq_id = 0;
   std::optional< typename TYPES::tEventID > m_processing_event_id = std::nullopt;
   std::optional< SequenceID >               m_processing_seq_id = std::nullopt;
};




template< typename TYPES >
TServer< TYPES >::TServer( const std::string& name, const std::string& role_name )
   : IInterface( name, role_name )
   , TYPES::tEventConsumer( )
{
   for( auto rr_item : TYPES::RR )
   {
      m_request_status_map.emplace( std::make_pair( rr_item.request, RequestStatus{ } ) );
      m_request_info_map.emplace( std::make_pair( rr_item.request, tRequestInfoSet{ } ) );
      TYPES::tEvent::set_notification( this, typename TYPES::tEvent::Signature( role( ), rr_item.request, nullptr, this, 0 ) );
   }

   InterfaceEvent::Event::set_notification( this, { eInterface::ClientConnected } );
   InterfaceEvent::Event::set_notification( this, { eInterface::ClientDisconnected } );
   InterfaceEvent::Event::create_send( { eInterface::ServerConnected }, { this }, TYPES::COMM_TYPE );
}

template< typename TYPES >
TServer< TYPES >::~TServer( )
{
   TYPES::tEvent::clear_all_notifications( this );
   InterfaceEvent::Event::clear_all_notifications( this );
   InterfaceEvent::Event::create_send( { eInterface::ServerDisconnected }, { this }, TYPES::COMM_TYPE );
}

template< typename TYPES >
void TServer< TYPES >::connected( const void* const p_client )
{
   mp_client_set.emplace( reinterpret_cast< const tClient* >( p_client ) );
   connected( );
}

template< typename TYPES >
void TServer< TYPES >::disconnected( const void* const p_client )
{
   mp_client_set.erase( reinterpret_cast< const tClient* >( p_client ) );
   disconnected( );
}

template< typename TYPES >
void TServer< TYPES >::process_event( const typename TYPES::tEvent& event )
{
   m_processing_event_id = event.info( ).id( );
   SYS_TRC( "processing event: %s", event.info( ).name( ).c_str( ) );

   if( true == prepare_request( event ) )
   {
      process_request_event( event );
   }
   else
   {
      SYS_WRN( "not processable event: %s", event.info( ).name( ).c_str( ) )
   }

   m_processing_event_id.reset( );
}

template< typename TYPES >
bool TServer< TYPES >::prepare_request( const typename TYPES::tEvent& event )
{
   const typename TYPES::tEventID event_id = event.info( ).id( );
   const void* p_from_addr = event.info( ).from_addr( );
   const SequenceID seq_id = event.info( ).seq_id( );

   // Find request event_id in request status map
   auto iterator_status_map = m_request_status_map.find( event_id );
   if( m_request_status_map.end( ) == iterator_status_map )
   {
      SYS_WRN( "not a request ID: %s", to_string( event_id ).c_str( ) );
      return false;
   }

   // Check request status for current request ID
   RequestStatus& request_status = iterator_status_map->second;
   if( eRequestStatus::BUSY == request_status.status )
   {
      SYS_WRN( "request busy: %s", to_string( event_id ).c_str( ) );
      for( auto item : TYPES::RR )
      {
         if( event_id == item.request )
         {
            TYPES::tEvent::create_send(
               typename TYPES::tEvent::Signature( role( ), item.busy, this, p_from_addr, seq_id )
               , TYPES::COMM_TYPE
            );
            break;
         }
         SYS_WRN( "request busy event id has not been found" );
      }
      return false;
   }

   // Find request ID in request info map
   auto iterator_info_map = m_request_info_map.find( event_id );
   if( m_request_info_map.end( ) == iterator_info_map )
   {
      SYS_WRN( "not a request ID: %s", to_string( event_id ).c_str( ) );
      return false;
   }

   // Set request busy for current request ID
   request_status.status = eRequestStatus::BUSY;
   // Increment common sequence ID and set it's value to current processing sequence ID for current request ID
   request_status.processing_server_seq_id = ++m_seq_id;
   // Store RequestInfo structure to request info set 
   iterator_info_map->second.emplace( m_seq_id, seq_id, p_from_addr );
   return true;
}

template< typename TYPES >
std::optional< RequestInfo > TServer< TYPES >::prepare_response( const typename TYPES::tEventID& id )
{
   // Find request id in request status map
   auto iterator_status_map = m_request_status_map.find( id );
   if( m_request_status_map.end( ) == iterator_status_map )
   {
      SYS_WRN( "not a request ID: %s", to_string( id ).c_str( ) );
      return std::nullopt;
   }
   RequestStatus& request_status = iterator_status_map->second;

   // Find request ID in request info map
   auto iterator_info_map = m_request_info_map.find( id );
   if( m_request_info_map.end( ) == iterator_info_map )
   {
      SYS_WRN( "not a request ID: %s", to_string( id ).c_str( ) );
      return std::nullopt;
   }

   SequenceID serching_seq_id = m_processing_seq_id.value_or( request_status.processing_server_seq_id );
   // Search for RequestInfo structure in request infor set for current request ID
   auto& request_info_set = iterator_info_map->second;
   auto iterator_info_set = request_info_set.find( RequestInfo{ serching_seq_id } );
   if( request_info_set.end( ) == iterator_info_set )
   {
      SYS_WRN( "request info for currnet sequence id has not been found: %zu", serching_seq_id );
      return std::nullopt;
   }

   RequestInfo request_info = *iterator_info_set;

   request_info_set.erase( iterator_info_set );
   request_status.status = eRequestStatus::READY;
   m_processing_seq_id.reset( );

   return request_info;
}

template< typename TYPES >
const SequenceID TServer< TYPES >::unblock_request( )
{
   if( std::nullopt == m_processing_event_id )
   {
      SYS_WRN( "unable unblock request" );
      return InvalidSequenceID;
   }

   // Find request id in request status map
   auto iterator_status_map = m_request_status_map.find( m_processing_event_id.value( ) );
   if( m_request_status_map.end( ) == iterator_status_map )
   {
      SYS_WRN( "not a request ID: %s", to_string( m_processing_event_id.value( ) ).c_str( ) );
      return InvalidSequenceID;
   }

   RequestStatus& request_status = iterator_status_map->second;
   request_status.status = eRequestStatus::READY;
   return request_status.processing_server_seq_id;
}

template< typename TYPES >
void TServer< TYPES >::prepare_response( const SequenceID seq_id )
{
   m_processing_seq_id = seq_id;
}

template< typename TYPES >
template< typename tResponseData, typename... Args >
void TServer< TYPES >::create_send_response( const Args&... args )
{
   std::optional< RequestInfo > request_info = std::nullopt;
   for( auto item : TYPES::RR )
   {
      if( tResponseData::id == item.response )
      {
         request_info = prepare_response( item.request );
         break;
      }
      return;
   }

   if( std::nullopt == request_info )
      return;
   
   typename TYPES::tEventData data( std::make_shared< tResponseData >( args... ) );
   TYPES::tEvent::create_send(
      typename TYPES::tEvent::Signature( role( ), tResponseData::id, this, request_info.value( ).client_addr, request_info.value( ).client_seq_id )
      , data, TYPES::COMM_TYPE
   );
}

template< typename TYPES >
template< typename tRequestData >
const tRequestData* TServer< TYPES >::get_event_data( const typename TYPES::tEvent& event )
{
   return static_cast< tRequestData* >( event.data( )->ptr.get( ) );
}



} // namespace base



#undef CLASS_ABBR