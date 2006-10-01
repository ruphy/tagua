// file      : xsd/cxx/xml/string.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_XML_STRING_IXX
#define XSD_CXX_XML_STRING_IXX

#include <cassert>

// We sometimes need this functionality even if we are building for
// wchar_t.
//
namespace xsd
{
  namespace cxx
  {
    namespace xml
    {
      template <>
      inline std::basic_string<char>
      transcode<char> (const XMLCh* s, std::size_t length)
      {
        if (s == 0)
          return std::basic_string<char> ();

        const XMLCh* src (s);

        if (length != 0)
        {
          // Know a better way? Let me know.
          //
          XMLCh* tmp = new XMLCh[length + 1];
          memcpy (tmp, s, length * sizeof (XMLCh));

          tmp[length] = XMLCh (0);

          src = tmp;
        }

        char* buf (xercesc::XMLString::transcode (s));

        if (src != s)
          delete[] (src);

        std::basic_string<char> r (buf);

        xercesc::XMLString::release (&buf);

        return r;
      }

      template <>
      inline XMLCh*
      transcode_to_xmlch (const char* s)
      {
        return xercesc::XMLString::transcode (s);
      }

      template <>
      inline XMLCh*
      transcode_to_xmlch (const std::basic_string<char>& s)
      {
        return xercesc::XMLString::transcode (s.c_str ());
      }
    }
  }
}

#endif // XSD_CXX_XML_STRING_IXX


#if defined(XSD_USE_CHAR) || !defined(XSD_USE_WCHAR)

#ifndef XSD_CXX_XML_STRING_IXX_CHAR
#define XSD_CXX_XML_STRING_IXX_CHAR

#endif // XSD_CXX_XML_STRING_IXX_CHAR
#endif // XSD_USE_CHAR


#if defined(XSD_USE_WCHAR) || !defined(XSD_USE_CHAR)

#ifndef XSD_CXX_XML_STRING_IXX_WCHAR
#define XSD_CXX_XML_STRING_IXX_WCHAR

namespace xsd
{
  namespace cxx
  {
    namespace xml
    {

      template <>
      inline std::basic_string<wchar_t>
      transcode<wchar_t> (const XMLCh* s, std::size_t length)
      {
        if (s == 0)
          return std::basic_string<wchar_t> ();

        // std::wcerr << s << std::endl;

        std::size_t l (0);

        if (length)
        {
          for (; l < length; ++l)
          {
            if (s[l] == 0)
              break;
          }
        }
        else
          l = xercesc::XMLString::stringLen (s);

        std::basic_string<wchar_t> r (l, L'0');

        for (std::size_t i (0); i <= l; ++s, ++i)
        {
          r[i] = *s;
        }

        return r;
      }

      template <>
      inline XMLCh*
      transcode_to_xmlch (const wchar_t* s)
      {
        std::size_t l (std::char_traits<wchar_t>::length (s));

        XMLCh* r (new XMLCh[l + 1]);
        XMLCh* ir (r);

        for (std::size_t i (0); i < l; ++ir, ++i)
        {
          *ir = static_cast<XMLCh>(s[i]);
        }

        *ir = XMLCh (0);

        return r;
      }

      template <>
      inline XMLCh*
      transcode_to_xmlch (const std::basic_string<wchar_t>& s)
      {
        std::size_t l (s.length ());

        XMLCh* r (new XMLCh[l + 1]);
        XMLCh* ir (r);

        for (std::size_t i (0); i < l; ++ir, ++i)
        {
          *ir = static_cast<XMLCh>(s[i]);
        }

        *ir = XMLCh (0);

        return r;
      }
    }
  }
}

#endif // XSD_CXX_XML_STRING_IXX_WCHAR
#endif // XSD_USE_WCHAR
