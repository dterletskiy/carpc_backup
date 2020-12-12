#include "api/sys/comm/event/Event.hpp"
#include "api/sys/comm/event/AsyncPriorityQueue.hpp"

#include "api/sys/trace/Trace.hpp"
#define CLASS_ABBR "AsyncPriorityQueue"



using namespace base::async;



AsyncPriorityQueue::AsyncPriorityQueue( const tPriority& max_priority, const std::string& name )
   : m_name( name )
{
   SYS_TRC( "'%s': created", m_name.c_str( ) );
   SYS_TRC( "max priority: %u", max_priority.value( ) );

   for( tPriority index = tPriority::zero( ); index < max_priority; ++index )
      m_collections.push_back( { } );
}

AsyncPriorityQueue::~AsyncPriorityQueue( )
{
   SYS_TRC( "'%s': destroyed", m_name.c_str( ) );
}

bool AsyncPriorityQueue::insert( const IAsync::tSptr p_async )
{
   SYS_TRC( "'%s': inserting async object (%s) with priority %u",
         m_name.c_str( ),
         p_async->signature( )->name( ).c_str( ),
         p_async->priority( ).value( )
      );

   m_buffer_cond_var.lock( );

   // If priority of current async object is higher then max supported priority it will be inserted
   // with highest priority.
   if( p_async->priority( ).value( ) >= m_collections.size( ) )
   {
      m_collections.back( ).push_back( p_async );
   }
   else
   {
      m_collections[ p_async->priority( ) ].push_back( p_async );
   }

   m_buffer_cond_var.notify( );
   m_buffer_cond_var.unlock( );

   return true;
}

IAsync::tSptr AsyncPriorityQueue::get( )
{
   SYS_TRC( "'%s':", m_name.c_str( ) );
   m_buffer_cond_var.lock( );

   // Check if there is any event with any priority starting from max priority to min.
   // Remember maximum priority where event has been found.
   std::optional< tPriority > priority_to_process = std::nullopt;
   for( tPriority index = tPriority( m_collections.size( ) - 1 ); index > tPriority::zero( ); --index )
   {
      if( false == m_collections[ index ].empty( ) )
      {
         priority_to_process = index;
         break;
      }
   }

   // Waiting for event in case if any event have not been found for any priority.
   // in this case 'priority_to_process == std::nullopt'
   if( std::nullopt == priority_to_process )
   {
      SYS_TRC( "'%s': waiting for async object...", m_name.c_str( ) );
      m_buffer_cond_var.wait( );
   }

   // In case priority is not defined for what event exists collection should be iterated again.
   if( std::nullopt == priority_to_process )
   {
      for( tPriority index = tPriority( m_collections.size( ) - 1 ); index > tPriority::zero( ); --index )
      {
         if( false == m_collections[ index ].empty( ) )
         {
            priority_to_process = index;
            break;
         }
      }
   }

   auto& collection = m_collections[ priority_to_process.value( ) ];
   IAsync::tSptr p_async = collection.front( );
   collection.pop_front( );
   SYS_TRC( "'%s': received async object (%s) with priority: %u",
         m_name.c_str( ),
         p_async->signature( )->name( ).c_str( ),
         p_async->priority( ).value( )
      );
   m_buffer_cond_var.unlock( );

   return p_async;
}

void AsyncPriorityQueue::dump( ) const
{
   SYS_DUMP_START( );
   SYS_INF( "%s:", m_name.c_str( ) )
   for( tPriority index = tPriority( m_collections.size( ) - 1 ); index > tPriority::zero( ); --index )
   {
      SYS_INF( "   priority: %s", index.name( ).c_str( ) )
      for( auto p_async : m_collections[ index ] )
      {
         SYS_INF( "      %s", p_async->signature( )->name( ).c_str( ) );
      }
   }
   SYS_DUMP_END( );
}
