// Application
#include "imp/app/api/onoff/Data.hpp"



namespace api::onoff {



BaseData::BaseData( const eOnOff _id )
   : m_id( _id )
{
}

tBaseDataPtr BaseData::create( base::ByteBufferT& buffer )
{
   eOnOff id = eOnOff::Undefined;
   if( false == buffer.pop( id ) )
      return nullptr;
   else if( eOnOff::Undefined == id )
      return nullptr;

   tBaseDataPtr ptr = nullptr;
   switch( id )
   {
      case eOnOff::RequestTriggerState:         ptr = std::make_shared< RequestTriggerStateData >( );       break;
      case eOnOff::ResponseTriggerState:        ptr = std::make_shared< ResponseTriggerStateData >( );      break;
      case eOnOff::NotificationCurrentState:    ptr = std::make_shared< NotificationCurrentStateData >( );  break;
      case eOnOff::Undefined:
      default:                                                                                              break;
   }

   if( nullptr != ptr )
      ptr->from_buffer( buffer );

   return  ptr;
}

const eOnOff BaseData::id( ) const
{
   return m_id;
}



RequestTriggerStateData::RequestTriggerStateData( )
   : BaseData( eOnOff::RequestTriggerState )
{
}

RequestTriggerStateData::RequestTriggerStateData( const std::string& _state )
   : BaseData( eOnOff::RequestTriggerState )
   , state( _state )
{
}

bool RequestTriggerStateData::to_buffer( base::ByteBufferT& buffer )
{
   return buffer.push( state );
}

bool RequestTriggerStateData::from_buffer( base::ByteBufferT& buffer )
{
   return buffer.pop( state );
}



ResponseTriggerStateData::ResponseTriggerStateData( )
   : BaseData( eOnOff::ResponseTriggerState )
{
}

ResponseTriggerStateData::ResponseTriggerStateData( const bool _result )
   : BaseData( eOnOff::ResponseTriggerState )
   , result( _result )
{
}

bool ResponseTriggerStateData::to_buffer( base::ByteBufferT& buffer )
{
   return buffer.push( result );
}

bool ResponseTriggerStateData::from_buffer( base::ByteBufferT& buffer )
{
   return buffer.pop( result );
}



NotificationCurrentStateData::NotificationCurrentStateData( )
   : BaseData( eOnOff::NotificationCurrentState )
{
}

NotificationCurrentStateData::NotificationCurrentStateData( const std::string& _state )
   : BaseData( eOnOff::NotificationCurrentState )
   , state( _state )
{
}

bool NotificationCurrentStateData::to_buffer( base::ByteBufferT& buffer )
{
   return buffer.push( state );
}

bool NotificationCurrentStateData::from_buffer( base::ByteBufferT& buffer )
{
   return buffer.pop( state );
}



OnOffEventData::OnOffEventData( )
{
}

OnOffEventData::OnOffEventData( tBaseDataPtr _ptr )
   : ptr( _ptr )
{
}

bool OnOffEventData::to_buffer( base::ByteBufferT& buffer ) const
{
   if( nullptr == ptr )
      return true;

   if( false == ptr->to_buffer( buffer ) )
      return false;
   return buffer.push( ptr->id( ) );
}

bool OnOffEventData::from_buffer( base::ByteBufferT& buffer )
{
   ptr = BaseData::create( buffer );
   return nullptr != ptr;
}



} // namespace api::onoff



INIT_DSI_EVENT( ServiceOnOff, OnOffEvent );
