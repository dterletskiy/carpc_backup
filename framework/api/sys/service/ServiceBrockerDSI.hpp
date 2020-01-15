#pragma once

#include "api/sys/oswrappers/Thread.hpp"
#include "api/sys/oswrappers/ConditionVariable.hpp"
#include "api/sys/comm/Types.hpp"
#include "api/sys/service/Types.hpp"



namespace base {



class ServiceBrockerDSI
   : public std::enable_shared_from_this< ServiceBrockerDSI >
{
public:
   ~ServiceBrockerDSI( );
   static ServiceBrockerDsiPtr instance( );
private:
   ServiceBrockerDSI( );
   ServiceBrockerDSI( const ServiceBrockerDSI& ) = delete;
   ServiceBrockerDSI& operator=( const ServiceBrockerDSI& ) = delete;
   static ServiceBrockerDsiPtr mp_instance;

public:
   bool start( );
   void stop( );
   bool started( ) const;
   bool wait( );

public:
   bool start_send( );
   void stop_send( );
   bool started_send( ) const;
   bool wait_send( );
private:
   void thread_loop_send( );
   os::ThreadPtr                 mp_thread_send;
   bool                          m_started_send = false;

public:
   bool start_receive( );
   void stop_receive( );
   bool started_receive( ) const;
   bool wait_receive( );
private:
   void thread_loop_receive( );
   os::ThreadPtr                 mp_thread_receive;
   bool                          m_started_receive = false;

public:
   bool insert_event( const EventPtr );
private:
   EventPtr get_event( );
   void notify( const EventPtr );
   std::deque< EventPtr >        m_events;
   os::ConditionVariable         m_buffer_cond_var;

private:
   bool setup_connection( );
   int                           m_master_socket;

private:
   int                           m_last_errno = 0;
};



inline
bool ServiceBrockerDSI::started( ) const
{
   return started_send( ) && started_receive( );
}

inline
bool ServiceBrockerDSI::wait( )
{
   return wait_send( ) && wait_receive( );
}

inline
bool ServiceBrockerDSI::started_send( ) const
{
   return m_started_send;
}

inline
bool ServiceBrockerDSI::wait_send( )
{
   m_started_send = mp_thread_send->join( );
   return !m_started_send;
}

inline
bool ServiceBrockerDSI::started_receive( ) const
{
   return m_started_receive;
}

inline
bool ServiceBrockerDSI::wait_receive( )
{
   m_started_receive = mp_thread_receive->join( );
   return !m_started_receive;
}



} // namespace base
