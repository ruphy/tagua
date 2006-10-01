// file      : xsd/cxx/xml/elements.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_XML_ELEMENTS_HXX
#define XSD_CXX_XML_ELEMENTS_HXX

#include <string>

#include <xercesc/util/PlatformUtils.hpp>

namespace xsd
{
  namespace cxx
  {
    namespace xml
    {
      template <typename C>
      class properties
      {
      public:
        struct argument {};


        // Schema location properties. Note that all locations are
        // relative to an instance document unless they are full
        // URIs. For example if you want to use a local schema then
        // you will need to use 'file:///absolute/path/to/your/schema'.
        //

        // Add a location for a schema with target namespace.
        //
        void
        schema_location (const std::basic_string<C>& namespace_,
                         const std::basic_string<C>& location)
        {
          if (namespace_.empty () || location.empty ())
            throw argument ();

          if (!schema_location_.empty ())
            schema_location_ += C (' ');

          schema_location_ += namespace_ + C (' ') + location;
        }


        // Add a location for a schema without target namespace.
        //
        void
        no_namespace_schema_location (const std::basic_string<C>& location)
        {
          if (location.empty ())
            throw argument ();

          if (!no_namespace_schema_location_.empty ())
            no_namespace_schema_location_ += C (' ');

          no_namespace_schema_location_ += location;
        }


      public:
        const std::basic_string<C>&
        schema_location () const
        {
          return schema_location_;
        }

        const std::basic_string<C>&
        no_namespace_schema_location () const
        {
          return no_namespace_schema_location_;
        }

      private:
        std::basic_string<C> schema_location_;
        std::basic_string<C> no_namespace_schema_location_;
      };


      //
      //

      template <typename C>
      std::basic_string<C>
      prefix (const std::basic_string<C>& n)
      {
        std::size_t i (0);

        while (i < n.length () && n[i] != ':')
          ++i;

        //std::wcerr << "prefix " << n << " "
        //           << std::wstring (n, i == n.length () ? i : 0, i)
        //           << std::endl;

        return std::basic_string<C> (n, i == n.length () ? i : 0, i);
      }

      template <typename C>
      std::basic_string<C>
      uq_name (const std::basic_string<C>& n)
      {
        std::size_t i (0);

        while (i < n.length () && n[i] != ':')
          ++i;

        return std::basic_string<C> (
          n.c_str () + (i == n.length () ? 0 : i + 1));
      }

      //
      //

      inline void
      initialize ()
      {
        xercesc::XMLPlatformUtils::Initialize ();
      }

      inline void
      terminate ()
      {
        xercesc::XMLPlatformUtils::Terminate ();
      }

      struct auto_initializer
      {
        auto_initializer (bool initialize = true, bool terminate = true)
            : terminate_ (initialize && terminate)
        {
          if (initialize)
            xml::initialize ();
        }

        ~auto_initializer ()
        {
          if (terminate_)
            terminate ();
        }

      private:
        bool terminate_;
      };
    }
  }
}

#endif  // XSD_CXX_XML_ELEMENTS_HXX
