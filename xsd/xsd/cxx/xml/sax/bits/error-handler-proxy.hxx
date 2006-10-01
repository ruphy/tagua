// file      : xsd/cxx/xml/sax/error-handler-proxy.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_XML_SAX_ERROR_HANDLER_PROXY_HXX
#define XSD_CXX_XML_SAX_ERROR_HANDLER_PROXY_HXX

#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include <xsd/cxx/xml/error-handler.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace xml
    {
      namespace sax
      {
        namespace bits
        {
          template <typename C>
          class error_handler_proxy: public xercesc::ErrorHandler
          {
          public:
            error_handler_proxy (error_handler<C>& eh)
                : failed_ (false), eh_ (&eh), native_eh_ (0)
            {
            }

            error_handler_proxy (xercesc::ErrorHandler& eh)
                : failed_ (false), eh_ (0), native_eh_ (&eh)
            {
            }

          public:
            virtual void
            warning (const xercesc::SAXParseException& e);

            virtual void
            error (const xercesc::SAXParseException& e);

            virtual void
            fatalError (const xercesc::SAXParseException& e);

          public:
            bool
            failed () const
            {
              return failed_;
            }

            virtual void
            resetErrors()
            {
              failed_ = false;
            }

          private:
            typedef typename error_handler<C>::severity severity;

            void
            handle (const xercesc::SAXParseException&, severity);

          private:
            bool failed_;
            error_handler<C>* eh_;
            xercesc::ErrorHandler* native_eh_;
          };


          template <typename C>
          void error_handler_proxy<C>::
          warning (const xercesc::SAXParseException& e)
          {
            if (native_eh_)
              native_eh_->warning (e);
            else
              handle (e, severity::warning);
          }


          template <typename C>
          void error_handler_proxy<C>::
          error (const xercesc::SAXParseException& e)
          {
            failed_ = true;

            if (native_eh_)
              native_eh_->error (e);
            else
              handle (e, severity::error);
          }


          template <typename C>
          void error_handler_proxy<C>::
          fatalError (const xercesc::SAXParseException& e)
          {
            failed_ = true;

            if (native_eh_)
              native_eh_->fatalError (e);
            else
              handle (e, severity::fatal);
          }


          template <typename C>
          void error_handler_proxy<C>::
          handle (const xercesc::SAXParseException& e, severity s)
          {
            //@@ I do not honor return values from the handler. This
            //   is not too bad at the moment because I set
            //   all-errors-are-fatal flag on the parser.
            //

            const XMLCh* id (e.getPublicId ()
                             ? e.getPublicId ()
                             : e.getSystemId ());

            XMLSSize_t l (e.getLineNumber ());
            XMLSSize_t c (e.getColumnNumber ());

            eh_->handle (transcode<C> (id),
                         (l == -1 ? 0 : static_cast<unsigned long> (l)),
                         (c == -1 ? 0 : static_cast<unsigned long> (c)),
                         s,
                         transcode<C> (e.getMessage ()));
          }
        }
      }
    }
  }
}

#endif  // XSD_CXX_XML_SAX_ERROR_HANDLER_PROXY_HXX

