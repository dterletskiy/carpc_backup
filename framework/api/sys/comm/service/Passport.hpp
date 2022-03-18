#pragma once

#include "api/sys/comm/service/Address.hpp"
#include "api/sys/comm/service/Signature.hpp"



namespace carpc::service {

   struct Passport
   {
      using tVector = std::vector< Passport >;
      using tList = std::list< Passport >;
      using tSet = std::set< Passport >;

      Passport( );
      Passport( const Signature&, const Address& );

      const std::string name( ) const;

      bool to_stream( carpc::ipc::tStream& ) const;
      bool from_stream( carpc::ipc::tStream& );

      bool operator==( const Passport& ) const;
      bool operator!=( const Passport& ) const;
      bool operator<( const Passport& ) const;

      Signature   signature;
      Address     address;
   };

} // namespace carpc::service
