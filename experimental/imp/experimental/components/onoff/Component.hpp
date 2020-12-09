#pragma once

#include "imp/experimental/base/component/Base.hpp"



namespace component::onoff {

   class Component : public fw::component::Base
   {
      public:
         static std::unique_ptr< fw::component::Base > creator( );

      public:
         Component( );
         ~Component( ) override;
   };

}