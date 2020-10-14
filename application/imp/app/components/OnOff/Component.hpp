#pragma once

// Framework
#include "api/sys/component/RootComponent.hpp"
#include "api/sys/comm/timer/Timer.hpp"
// Application
#include "imp/app/components/OnOff/server/Server.hpp"



namespace application::components::onoff {

   class Component
      : public base::component::RootComponent
      , public base::ITimerConsumer
   {
      public:
         static base::component::IComponent::tSptr creator( base::application::IThread& service );

      private:
         Component( base::application::IThread& service, const std::string& );
      public:
         ~Component( ) override;

      private:
         void boot( const std::string& ) override;

      private:
         Server m_server_onoff;

      // Timer
      private:
         void process_timer( const base::Timer::ID ) override;
         base::Timer m_timer;

      public:
         void on_timer( const base::Timer::ID );
   };

} // namespace application::components::onoff
