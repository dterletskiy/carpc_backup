#pragma once

#include "api/sys/comm/service/Address.hpp"
#include "api/sys/comm/service/Signature.hpp"
#include "api/sys/comm/service/Passport.hpp"
#include "api/sys/comm/async/event/Event.hpp"



namespace base::events {

   namespace system {

      enum class eID : std::uint8_t { boot, shutdown, ping, undefined };
      const char* c_str( const eID );

      struct Data
      {
         std::string message;
      };
      DEFINE_EVENT( System, Data, base::async::id::TSignature< eID > );

   }



   namespace service {

      enum class eAction : std::uint8_t
      {
         RegisterServer,
         UnregisterServer,
         RegisterClient,
         UnregisterClient,
         Undefined
      };
      const char* c_str( const eAction );

      DEFINE_IPC_EVENT( Action, base::service::Passport, base::async::id::TSignature< eAction > );



      enum class eStatus : std::uint8_t
      {
         ServerConnected,
         ServerDisconnected,
         ClientConnected,
         ClientDisconnected,
         Undefined
      };
      const char* c_str( const eStatus );

      class SignatureStatus
      {
         public:
            SignatureStatus( ) = default;
            SignatureStatus( const base::service::Signature&, const eStatus );
            SignatureStatus( const SignatureStatus& );
            ~SignatureStatus( ) = default;

         public:
            const std::string name( ) const;
            bool operator<( const SignatureStatus& ) const;
            const bool to_stream( ipc::tStream& ) const;
            const bool from_stream( ipc::tStream& );

         public:
            const base::service::Signature& signature( ) const;
            const eStatus id( ) const;

         private:
            base::service::Signature      m_signature;
            eStatus                       m_id = eStatus::Undefined;
      };
      DEFINE_IPC_EVENT( Status, base::service::Address, SignatureStatus );

   }

} // namespace base::events
