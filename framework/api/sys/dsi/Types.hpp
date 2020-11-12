#pragma once

#include <cstdlib>
#include "api/sys/oswrappers/linux/socket.hpp"
#include "api/sys/common/ByteStream.hpp"



namespace base::dsi {

   using tByteStream = base::ByteStream;

   struct SocketCongiguration : public os::linux::socket::configuration
   {
      const std::string name( ) const;

      bool to_stream( tByteStream& stream ) const;
      bool from_stream( tByteStream& stream );
   };



   enum class eCommand : size_t
   {
      RegisterServer,
      UnregisterServer,
      RegisterClient,
      UnregisterClient,
      DetectedServer,
      DetectedClient,
      RegisterProcess,
      RegisterProcessAck,
      RegisterClientProcess,
      RegisterServerProcess,
      BroadcastEvent,
      Undefined
   };
   const char* c_str( const eCommand );

   class Package
   {
      public:
         using tVector = std::vector< Package >;

      public:
         Package( );
         template< typename ... TYPES >
            Package( const eCommand command, const TYPES& ... args );
         Package( const Package& pkg ) = delete;
         Package( Package&& pkg );
         ~Package( );

      public:
         bool to_stream( tByteStream& stream ) const;
         bool from_stream( tByteStream& stream );

      public:
         size_t size( ) const;
         const char* const c_str( ) const;

      public:
         eCommand command( ) const;
         tByteStream& data( );
         template< typename ... TYPES >
            bool data( TYPES& ... args );
      private:
         eCommand       m_command = eCommand::Undefined;
         tByteStream    m_data;
   };



   template< typename ... TYPES >
   Package::Package( const eCommand command, const TYPES& ... args )
      : m_command( command )
      , m_data( )
   {
      m_data.push( args... );
   }

   inline
   size_t Package::size( ) const
   {
      return sizeof( eCommand ) + m_data.size( ) + sizeof( size_t );
   }

   inline
   const char* const Package::c_str( ) const
   {
      return dsi::c_str( m_command );
   }

   inline
   eCommand Package::command( ) const
   {
      return m_command;
   }

   inline
   tByteStream& Package::data( )
   {
      return m_data;
   }

   template< typename ... TYPES >
   bool Package::data( TYPES& ... args )
   {
      return m_data.pop( args... );
   }

}



namespace base::dsi {

   class Packet
   {
      public:
         using tSptr = std::shared_ptr< Packet >;

      public:
         Packet( );
         template< typename ... TYPES >
            Packet( const eCommand _command, const TYPES& ... _values );
         ~Packet( );

      public:
         bool to_stream( tByteStream& ) const;
         bool from_stream( tByteStream& );
         bool test_stream( tByteStream& ) const;

      public:
         void add_package( Package&& );
         template< typename ... TYPES >
            void add_package( const eCommand _command, const TYPES& ... _values );
         Package::tVector& packages( );

      private:
         static constexpr size_t    m_begin_sign = 0xAABBCCDD;
         size_t                     m_size   = sizeof( size_t )   /* m_size */
                                             + sizeof( size_t )   /* m_packages vector count variable */
                                             + 0                  /* m_packages vector content (will be apdated during adding the package) */
                                             + sizeof( size_t );  /* m_crc */
         Package::tVector           m_packages;
         size_t                     m_crc;
         static constexpr size_t    m_end_sign = 0xFFEEDDCC;
   };



   template< typename ... TYPES >
   Packet::Packet( const eCommand _command, const TYPES& ... _values )
   {
      m_packages.emplace_back(  _command, _values... );
      m_size += m_packages.back( ).size( );
   }

   template< typename ... TYPES >
   void Packet::add_package( const eCommand _command, const TYPES& ... _values )
   {
      m_packages.emplace_back(  _command, _values... );
      m_size += m_packages.back( ).size( );
   }

   inline
   Package::tVector& Packet::packages( )
   {
      return m_packages;
   }

}
