// file      : xsd/cxx/parser/exceptions.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsd
{
  namespace cxx
  {
    namespace parser
    {
      // invalid_value
      //
      template <typename C>
      invalid_value<C>::
      ~invalid_value () throw ()
      {
      }

      template <typename C>
      invalid_value<C>::
      invalid_value (const C* type,
                     const std::basic_string<C>& value)
          : type_ (type), value_ (value)
      {
      }

      template <typename C>
      invalid_value<C>::
      invalid_value (const C* type,
                     const ro_string<C>& value)
          : type_ (type), value_ (value)
      {
      }

      template <typename C>
      invalid_value<C>::
      invalid_value (const std::basic_string<C>& type,
                     const std::basic_string<C>& value)
          : type_ (type), value_ (value)
      {
      }

      template <typename C>
      const char* invalid_value<C>::
      what () const throw ()
      {
        return "invalid value representation encountered";
      }
    }
  }
}
