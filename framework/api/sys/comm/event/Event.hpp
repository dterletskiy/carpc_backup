#pragma once

#include "api/sys/comm/event/TGenerator.hpp"
#include "api/sys/helpers/macros/enum.hpp"



#define DEFINE_EVENT( eventType, dataType, signatureType ) \
   namespace eventType { \
      class eventType; \
      using Generator      = base::TGenerator< base::NoServiceType, eventType, dataType, signatureType >; \
      using Event          = Generator::Config::tEvent; \
      using Signature      = Generator::Config::tSignature; \
      using UserSignature  = Generator::Config::tUserSignature; \
      using Data           = dataType; \
      using Consumer       = Generator::Config::tConsumer; \
   }

#define DEFINE_IPC_EVENT( eventType, dataType, signatureType ) \
   namespace eventType { \
      class ServiceType; \
      class eventType; \
      using Generator      = base::TGenerator< ServiceType, eventType, dataType, signatureType >; \
      using Event          = Generator::Config::tEvent; \
      using Signature      = Generator::Config::tSignature; \
      using UserSignature  = Generator::Config::tUserSignature; \
      using Data           = dataType; \
      using Consumer       = Generator::Config::tConsumer; \
   }

#define REGISTER_EVENT( eventType ) \
   base::IEvent::check_in( eventType::Signature::build_type_id( ), eventType::Event::create );

#define DUMP_IPC_EVENTS \
   base::IEvent::dump( );
