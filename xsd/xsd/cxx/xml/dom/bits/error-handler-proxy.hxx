// file      : xsd/cxx/xml/dom/bits/error-handler-proxy.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_XML_DOM_BITS_ERROR_HANDLER_PROXY_HXX
#define XSD_CXX_XML_DOM_BITS_ERROR_HANDLER_PROXY_HXX

#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

#include <xsd/cxx/xml/error-handler.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace xml
    {
      namespace dom
      {
        namespace bits
        {
          template <typename C>
          class error_handler_proxy: public xercesc::DOMErrorHandler
          {
          public:
            error_handler_proxy (error_handler<C>& eh)
                : failed_ (false), eh_ (&eh), native_eh_ (0)
            {
            }

            error_handler_proxy (xercesc::DOMErrorHandler& eh)
              : failed_ (false), eh_ (0), native_eh_ (&eh)
            {
            }

            virtual bool
            handleError (const xercesc::DOMError& e);

            bool
            failed () const
            {
              return failed_;
            }

          private:
            bool failed_;
            error_handler<C>* eh_;
            xercesc::DOMErrorHandler* native_eh_;
          };

          template <typename C>
          bool error_handler_proxy<C>::
          handleError (const xercesc::DOMError& e)
          {
            using xercesc::DOMError;

            if (e.getSeverity() != DOMError::DOM_SEVERITY_WARNING)
              failed_ = true;

            if (native_eh_)
              return native_eh_->handleError (e);
            else
            {
              typedef typename error_handler<C>::severity severity;

              severity s (severity::error);

              switch (e.getSeverity())
              {
              case DOMError::DOM_SEVERITY_WARNING:
                {
                  s = severity::warning;
                  break;
                }
              case DOMError::DOM_SEVERITY_ERROR:
                {
                  s = severity::error;
                  break;
                }
              case DOMError::DOM_SEVERITY_FATAL_ERROR:
                {
                  s = severity::fatal;
                  break;
                }
              }

              XMLSSize_t l (e.getLocation ()->getLineNumber ());
              XMLSSize_t c (e.getLocation ()->getColumnNumber ());

              return eh_->handle (
                transcode<C> (e.getLocation()->getURI ()),
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
}

#endif  // XSD_CXX_XML_DOM_BITS_ERROR_HANDLER_PROXY_HXX

