#include "api/sys/comm/event/Event.hpp"
#include "api/sys/service/ServiceThread.hpp"
#include "imp/sys/service/ServiceEventConsumer.hpp"

#include "api/sys/trace/Trace.hpp"
#define CLASS_ABBR "Srv"



namespace base {



bool ServiceThread::Comparator::operator( )( const IEventSignature* p_es1, const IEventSignature* p_es2 ) const
{
   return *p_es1 < *p_es2;
}



ServiceThread::ServiceThread( const ServiceInfo& info )
   : m_name( info.m_name )
   , m_wd_timeout( info.m_wd_timeout )
   , m_events( )
   , m_buffer_cond_var( )
   , m_event_consumers_map( )
   , m_components( )
   , m_component_creators( info.m_component_creators )
{
   mp_thread = std::make_shared< base::os::Thread >( std::bind( &ServiceThread::thread_loop, this ) );

   SYS_TRC( "'%s': created", m_name.c_str( ) );
}

ServiceThread::~ServiceThread( )
{
   SYS_TRC( "'%s': destroyed", m_name.c_str( ) );
}

const TID ServiceThread::id( ) const
{
   TID id = 0;
   if( mp_thread )
      id = mp_thread->id( );

   return id;
}

void ServiceThread::thread_loop( )
{
   SYS_INF( "'%s': enter", m_name.c_str( ) );
   m_started = true;

   // Creating components
   for( auto creator : m_component_creators )
      m_components.emplace_back( creator( shared_from_this( ) ) );

   ServiceEventConsumer service_event_consumer( shared_from_this( ) );

   while( started( ) )
   {
      EventPtr p_event = get_event( );
      SYS_TRC( "'%s': processing event (%s)", m_name.c_str( ), p_event->signature( )->name( ).c_str( ) );
      notify( p_event );
   }

   // Destroying components
   m_components.clear( );
   // Destroying interface registry

   SYS_INF( "'%s': exit", m_name.c_str( ) );
}

bool ServiceThread::start( )
{
   SYS_INF( "Starting service '%s'", m_name.c_str( ) );
   bool result = mp_thread->run( );
   if( false == result )
   {
      SYS_ERR( "ServiceThread '%s' can't be started", m_name.c_str( ) );
   }

   return result;
}

void ServiceThread::stop( )
{
   SYS_INF( "'%s': stopping", m_name.c_str( ) );
   m_started = false;
}

bool ServiceThread::insert_event( const EventPtr p_event )
{
   if( false == started( ) )
   {
      SYS_WRN( "'%s': is not started", m_name.c_str( ) );
      return false;
   }

   SYS_TRC( "'%s': inserting event (%s)", m_name.c_str( ), p_event->signature( )->name( ).c_str( ) );
   m_buffer_cond_var.lock( );
   m_events.push_back( p_event );
   m_buffer_cond_var.notify( );
   m_buffer_cond_var.unlock( );

   return true;
}

EventPtr ServiceThread::get_event( )
{
   m_buffer_cond_var.lock( );
   if( true == m_events.empty( ) )
   {
      SYS_TRC( "'%s': waiting for event...", m_name.c_str( ) );
      m_buffer_cond_var.wait( );
   }
   EventPtr p_event = m_events.front( );
   m_events.pop_front( );
   ++m_processed_events;
   SYS_TRC( "'%s': received event (%s)", m_name.c_str( ), p_event->signature( )->name( ).c_str( ) );
   m_buffer_cond_var.unlock( );

   return p_event;
}

void ServiceThread::notify( const EventPtr p_event )
{
   const auto& iterator = m_event_consumers_map.find( p_event->signature( ) );
   if( iterator == m_event_consumers_map.end( ) )
      return;

   // Here we create a copy of consumers set to avoid modifying consumers set during iterating it,
   // for example, during calling "clear_notification" from "process_event".
   auto consumers_set = iterator->second;
   SYS_TRC( "'%s': %zu consumers will be processed", m_name.c_str( ), consumers_set.size( ) );
   for( IEventConsumer* p_consumer : consumers_set )
   {
      m_process_started = time( nullptr );
      SYS_TRC( "'%s': start processing at %ld (%s)", m_name.c_str( ), m_process_started.value( ), p_event->signature( )->name( ).c_str( ) );
      p_event->process( p_consumer );
      SYS_TRC( "'%s': finished processing started at %ld (%s)", m_name.c_str( ), m_process_started.value( ), p_event->signature( )->name( ).c_str( ) );
   }
   m_process_started.reset( );
}

void ServiceThread::set_notification( const IEventSignature& signature, IEventConsumer* p_consumer )
{
   if( nullptr == p_consumer ) return;

   SYS_INF( "'%s': event id (%s) / consumer (%p)", m_name.c_str( ), signature.name( ).c_str( ), p_consumer );
   const auto& iterator = m_event_consumers_map.find( &signature );
   if( iterator == m_event_consumers_map.end( ) )
   {
      // Here signature is a temporary object created on a stack in concrete event static function.
      // So we should create a copy of this signature in a heap and store its pointer to consumers map.
      // Later on when number of consumers for this signature becase zero we must delete object by this pointer and remove poinetr from this map.
      auto p_signature = signature.create_copy( );
      m_event_consumers_map.emplace( std::pair< const IEventSignature*, std::set< IEventConsumer* > >( p_signature, { p_consumer } ) );
   }
   else
   {
      iterator->second.emplace( p_consumer );
   }
}

void ServiceThread::clear_notification( const IEventSignature& signature, IEventConsumer* p_consumer )
{
   if( nullptr == p_consumer ) return;

   const auto& iterator = m_event_consumers_map.find( &signature );
   if( iterator == m_event_consumers_map.end( ) )
      return;

   iterator->second.erase( p_consumer );

   if( true == iterator->second.empty( ) )
   {
      // Number of events for this event signature is zero (last one has just deleted), so we should remove pointer to this signature from the map
      // and must delete dynamicly created object (in function "set_notification")
      delete iterator->first;
      m_event_consumers_map.erase( iterator );
   }
}

void ServiceThread::clear_all_notifications( const IEventSignature& signature, IEventConsumer* p_consumer )
{
   if( nullptr == p_consumer ) return;

   for( auto iterator_map = m_event_consumers_map.begin( ); iterator_map != m_event_consumers_map.end( ); ++iterator_map )
   {
      if( signature.type_id( ) != iterator_map->first->type_id( ) )
         continue;

      auto& consumers_set = iterator_map->second;
      auto iterator_set = consumers_set.find( p_consumer );
      if( consumers_set.end( ) == iterator_set )
         continue;

      consumers_set.erase( iterator_set );

      if( true == consumers_set.empty( ) )
      {
         // Number of events for this event signature is zero (last one has just deleted), so we should remove pointer to this signature from the map
         // and must delete dynamicly created object (in function "set_notification")
         delete iterator_map->first;
         m_event_consumers_map.erase( iterator_map );
      }

      break;
   }
}

bool ServiceThread::is_subscribed( const EventPtr p_event )
{
   const auto& iterator = m_event_consumers_map.find( p_event->signature( ) );
   if( iterator == m_event_consumers_map.end( ) )
   {
      SYS_TRC( "'%s': is not subscribed on event (%s)", m_name.c_str( ), p_event->signature( )->name( ).c_str( ) );
      return false;
   }

   return true;
}



} // namespace base
