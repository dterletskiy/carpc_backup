#pragma once

#include "api/sys/comm/event/Event.hpp"
#include "api/sys/comm/event/Runnable.hpp"
#include "api/sys/application/Context.hpp"
#include "api/sys/events/Events.hpp"
#include "api/sys/comm/service/Types.hpp"



namespace base::service {

   class Address;



   class IConnection : public events::service::Status::Consumer
   {
      public:
         IConnection( const base::async::tAsyncTypeID&, const std::string&, const bool );
         virtual ~IConnection( );

      protected:
         enum class eStatus { Connected, Disconnected };

      protected:
         virtual void status( const Address&, const eStatus ) = 0;
         virtual void connected( const Address& ) = 0;
         virtual void disconnected( const Address& ) = 0;
         virtual void connected( ) = 0;
         virtual void disconnected( ) = 0;
         virtual const bool is_connected( ) const = 0;

      private:
         void process_event( const events::service::Status::Event& ) override;

      public:
         const Signature& signature( ) const;
      private:
         Signature               m_signature;

      public:
         const application::Context& context( ) const;
      protected:
         application::Context    m_context;

      public:
         const ID& id( ) const;
      protected:
         ID                      m_id;

      public:
         bool is_external( ) const;
      protected:
         bool                    m_is_external = false;
   };



   inline
   const Signature& IConnection::signature( ) const
   {
      return m_signature;
   }

   inline
   const application::Context& IConnection::context( ) const
   {
      return m_context;
   }

   inline
   const ID& IConnection::id( ) const
   {
      return m_id;
   }

   inline
   bool IConnection::is_external( ) const
   {
      return m_is_external;
   }

} // namespace base::service