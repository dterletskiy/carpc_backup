#pragma once

#include "api/sys/component/Component.hpp"



namespace base::component {

   class RootComponent : public Component
   {
      public:
         RootComponent( application::IThread&, const std::string& );
         ~RootComponent( ) override;

      private:
         const bool is_root( ) const override { return true; }

      protected:
         void boot( const std::string& info = "boot" ) override;
         void shutdown( const std::string& info = "shutdown" ) override;
   };

} // namespace base::component
