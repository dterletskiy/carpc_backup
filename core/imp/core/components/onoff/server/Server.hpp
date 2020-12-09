#pragma once

#include "api/sys/comm/timer/Timer.hpp"
#include "api/sys/onoff/Server.hpp"



namespace core::components::onoff {

   class Server
      : public base::onoff::Server
      , public base::ITimerConsumer
   {
      public:
         Server( );
         ~Server( );

      private:
         void connected( ) override;
         void disconnected( ) override;

      public:
         void request_start( ) override;
         void request_trigger_state( const std::string&, const size_t ) override;

      // Timer
      private:
         void process_timer( const base::Timer::ID ) override;
         struct TimerSeqID
         {
            std::string current_state;
            std::shared_ptr< base::Timer > timer;
            base::service::tSequenceID seq_id;
         };
         std::list< TimerSeqID > m_timers;
   };

} // namespace core::components::onoff