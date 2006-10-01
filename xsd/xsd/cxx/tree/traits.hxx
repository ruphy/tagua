// file      : xsd/cxx/tree/traits.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_TREE_TRAITS_HXX
#define XSD_CXX_TREE_TRAITS_HXX

// Do not include this file directly. Rather include elements.hxx.
//

#include <sstream>

#include <xsd/cxx/xml/string.hxx> // xml::transcode

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      // bool
      //

      namespace bits
      {
        template<typename C>
        const C*
        true_ ();

        template<typename C>
        const C*
        one ();
      }

      template <typename C>
      struct traits<bool, C>
      {
        typedef bool type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (e.getTextContent ()), 0, f, c);
        }

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (a.getValue ()), 0, f, c);
        }

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*)
        {
          return (s == bits::true_<C> ()) || (s == bits::one<C> ());
        }
      };


      // 8 bit
      //

      template <typename C>
      struct traits<signed char, C>
      {
        typedef signed char type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (e.getTextContent ()), 0, f, c);
        }

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (a.getValue ()), 0, f, c);
        }

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*)
        {
          std::basic_istringstream<C> is (s);

          short t;
          is >> t;

          return static_cast<type> (t);
        }
      };


      template <typename C>
      struct traits<unsigned char, C>
      {
        typedef unsigned char type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (e.getTextContent ()), 0, f, c);
        }

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (a.getValue ()), 0, f, c);
        }

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*)
        {
          std::basic_istringstream<C> is (s);

          unsigned short t;
          is >> t;

          return static_cast<type> (t);
        }
      };


      // 16 bit
      //

      template <typename C>
      struct traits<short, C>
      {
        typedef short type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (e.getTextContent ()), 0, f, c);
        }

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (a.getValue ()), 0, f, c);
        }

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };


      template <typename C>
      struct traits<unsigned short, C>
      {
        typedef unsigned short type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (e.getTextContent ()), 0, f, c);
        }

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (a.getValue ()), 0, f, c);
        }

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };


      // 32 bit
      //

      template <typename C>
      struct traits<int, C>
      {
        typedef int type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (e.getTextContent ()), 0, f, c);
        }

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (a.getValue ()), 0, f, c);
        }

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };


      template <typename C>
      struct traits<unsigned int, C>
      {
        typedef unsigned int type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (e.getTextContent ()), 0, f, c);
        }

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (a.getValue ()), 0, f, c);
        }

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };


      // 64 bit
      //

      template <typename C>
      struct traits<long long, C>
      {
        typedef long long type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (e.getTextContent ()), 0, f, c);
        }

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (a.getValue ()), 0, f, c);
        }

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };


      template <typename C>
      struct traits<unsigned long long, C>
      {
        typedef unsigned long long type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (e.getTextContent ()), 0, f, c);
        }

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (a.getValue ()), 0, f, c);
        }

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };


      // floating point
      //

      template <typename C>
      struct traits<float, C>
      {
        typedef float type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (e.getTextContent ()), 0, f, c);
        }

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (a.getValue ()), 0, f, c);
        }

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };


      template <typename C>
      struct traits<double, C>
      {
        typedef double type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (e.getTextContent ()), 0, f, c);
        }

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (a.getValue ()), 0, f, c);
        }

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };


      template <typename C>
      struct traits<long double, C>
      {
        typedef long double type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (e.getTextContent ()), 0, f, c);
        }

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c)
        {
          return create (xml::transcode<C> (a.getValue ()), 0, f, c);
        }

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };
    }
  }
}

#endif  // XSD_CXX_TREE_TRAITS_HXX

#include <xsd/cxx/tree/traits.ixx>
