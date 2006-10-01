// file      : xsd/cxx/tree/types.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xercesc/util/Base64.hpp>
#include <xercesc/util/Janitor.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XercesVersion.hpp>

#include <xsd/cxx/xml/std-memory-manager.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {

      // base64_binary
      //

      template <typename C, typename B>
      base64_binary<C, B>::
      base64_binary (size_t size)
          : buffer<C> (size)
      {
      }

      template <typename C, typename B>
      base64_binary<C, B>::
      base64_binary (size_t size, size_t capacity)
          : buffer<C> (size, capacity)
      {
      }

      template <typename C, typename B>
      base64_binary<C, B>::
      base64_binary (const void* data, size_t size)
          : buffer<C> (data, size)
      {
      }

      template <typename C, typename B>
      base64_binary<C, B>::
      base64_binary (const void* data, size_t size, size_t capacity)
          : buffer<C> (data, size, capacity)
      {
      }

      template <typename C, typename B>
      base64_binary<C, B>::
      base64_binary (void* data, size_t size, size_t capacity, bool own)
          : buffer<C> (data, size, capacity, own)
      {
      }


      // It would have been cleaner to mention size, and data with the
      // using-declaration but HP aCC3 can't handle it in some non-
      // trivial to track down cases. So we are going to use the
      // old-n- ugly this-> techniques.
      //
      template <typename C, typename B>
      std::basic_string<C> base64_binary<C, B>::
      encode () const
      {
        // HP aCC3 cannot handle using namespace xercesc;
        //
        using xercesc::Base64;
        using xercesc::ArrayJanitor;

        unsigned int n;
        XMLByte* r (
          Base64::encode (
            reinterpret_cast<const XMLByte*> (this->data ()),
            static_cast<unsigned int> (this->size ()),
            &n));
        ArrayJanitor<XMLByte> jan (r);


        std::basic_string<C> str;

        if (r)
        {
          str.reserve (n + 1);
          str.resize (n);

          for (unsigned int i (0); i < n; ++i)
            str[i] = C (r[i]);
        }
        else
        {
          //@@ throw
        }

        return str;
      }

      template <typename C, typename B>
      void base64_binary<C, B>::
      decode (const XMLCh* src)
      {
        // HP aCC3 cannot handle using namespace xercesc;
        //
        using xercesc::Base64;
        using xercesc::ArrayJanitor;

        unsigned int size;

        // Xerces 2.6.0 and earlier do not have decodeToXMLByte which
        // makes my life harder and your code slower.
        //
#if _XERCES_VERSION >= 20700
        xml::std_memory_manager memory_manager;
        XMLByte* data (
          Base64::decodeToXMLByte (
            src, &size, &memory_manager, Base64::Conf_Schema));
        ArrayJanitor<XMLByte> jan (data);

        if (data)
        {
          buffer<C> tmp (data, size, size, true);
          jan.orphan ();
          this->swap (tmp); // g++ 4.1 likes it qualified, not sure why.
        }
        else
        {
          //@@ throw
        }
#else
#if _XERCES_VERSION >= 20600  // Xerces 2.5.0 does not have Conf_Schema.
        XMLCh* data (Base64::decode (src, &size, 0, Base64::Conf_Schema));
#else
        XMLCh* data (Base64::decode (src, &size));
#endif // _XERCES_VERSION >= 20600

        ArrayJanitor<XMLCh> jan (data);

        if (data)
        {
          buffer<C> tmp (size);
          for (unsigned int i (0); i < size; ++i)
            tmp.data ()[i] = static_cast<char> (data[i]);
          this->swap (tmp); // g++ 4.1 likes it qualified, not sure why.
        }
        else
        {
          //@@ throw
        }
#endif  //_XERCES_VERSION >= 20700
      }


      // hex_binary
      //

      template <typename C, typename B>
      hex_binary<C, B>::
      hex_binary (size_t size)
          : buffer<C> (size)
      {
      }

      template <typename C, typename B>
      hex_binary<C, B>::
      hex_binary (size_t size, size_t capacity)
          : buffer<C> (size, capacity)
      {
      }

      template <typename C, typename B>
      hex_binary<C, B>::
      hex_binary (const void* data, size_t size)
          : buffer<C> (data, size)
      {
      }

      template <typename C, typename B>
      hex_binary<C, B>::
      hex_binary (const void* data, size_t size, size_t capacity)
          : buffer<C> (data, size, capacity)
      {
      }

      template <typename C, typename B>
      hex_binary<C, B>::
      hex_binary (void* data, size_t size, size_t capacity, bool own)
          : buffer<C> (data, size, capacity, own)
      {
      }

      // It would have been cleaner to mention size, and data with the
      // using-declaration but HP aCC3 can't handle it in some non-
      // trivial to track down cases. So we are going to use the
      // old-n- ugly this-> techniques.
      //
      template <typename C, typename B>
      std::basic_string<C> hex_binary<C, B>::
      encode () const
      {
        std::basic_string<C> str;

        const char tab[] = "0123456789ABCDEF";

        if (size_t n = this->size ())
        {
          str.reserve (2 * n + 1);
          str.resize (2 * n);

          for (size_t i (0); i < n; ++i)
          {
            char byte (*(this->data () + i));
            unsigned char h (byte >> 4);
            unsigned char l (byte & 0x0F);

            str[2 * i] = C (tab[h]);
            str[2 * i + 1] = C (tab[l]);
          }
        }

        return str;
      }

      namespace bits
      {
        inline
        unsigned char
        hex_decode (XMLCh c)
        {
          unsigned char r;

          if (c >= '0' && c <= '9')
            r = static_cast<unsigned char> (c - '0');
          else if (c >= 'A' && c <= 'F')
            r = static_cast<unsigned char> (c - 'A');
          else if (c >= 'a' && c <= 'f')
            r = static_cast<unsigned char> (c - 'a');
          else
            r = 0xFF;

          return r;
        }
      }

      template <typename C, typename B>
      void hex_binary<C, B>::
      decode (const XMLCh* src)
      {
        size_t src_n (xercesc::XMLString::stringLen (src));

        if (src_n % 2 != 0)
          return; // @@ throw

        size_t n (src_n / 2);

        buffer<C> tmp (n);

        for (size_t i (0); i < n; ++i)
        {
          unsigned char h (bits::hex_decode (src[2 * i]));
          unsigned char l (bits::hex_decode (src[2 * i + 1]));

          if (h == 0xFF || l == 0xFF)
            return; //@@ throw

          tmp.data()[i] = (h << 4) | l;
        }

        this->swap (tmp); // g++ 4.1 likes it qualified, not sure why.
      }
    }
  }
}

