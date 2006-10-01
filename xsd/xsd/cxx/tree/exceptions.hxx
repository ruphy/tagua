// file      : xsd/cxx/tree/exceptions.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_TREE_EXCEPTIONS_HXX
#define XSD_CXX_TREE_EXCEPTIONS_HXX

#include <string>
#include <vector>
#include <ostream>

#include <xsd/cxx/exceptions.hxx> // xsd::cxx::exception

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      //
      //
      template <typename C>
      struct exception: virtual xsd::cxx::exception
      {
        friend
        std::basic_ostream<C>&
        operator<< (std::basic_ostream<C>& os, const exception& e)
        {
          e.print (os);
          return os;
        }

      protected:
        virtual void
        print (std::basic_ostream<C>&) const = 0;
      };


      //
      //
      template <typename C>
      struct error
      {
        error (const std::basic_string<C>& id,
               unsigned long line,
               unsigned long column,
               const std::basic_string<C>& message)
            : id_ (id),
              line_ (line),
              column_ (column),
              message_ (message)
        {
        }

        const std::basic_string<C>&
        id () const
        {
          return id_;
        }

        unsigned long
        line () const
        {
          return line_;
        }

        unsigned long
        column () const
        {
          return column_;
        }

        const std::basic_string<C>&
        message () const
        {
          return message_;
        }

      private:
        std::basic_string<C> id_;
        unsigned long line_;
        unsigned long column_;
        std::basic_string<C> message_;
      };

      // See exceptions.ixx for operator<< (error).


      //
      //
      template <typename C>
      struct errors: std::vector<error<C> >
      {
      };

      // See exceptions.ixx for operator<< (errors).

      //
      //
      template <typename C>
      struct parsing: virtual exception<C>
      {
        virtual
        ~parsing () throw ()
        {
        }

        parsing ()
        {
        }

        parsing (const tree::errors<C>& errors)
            : errors_ (errors)
        {
        }

        const tree::errors<C>&
        errors () const
        {
          return errors_;
        }

        virtual const char*
        what () const throw ()
        {
          return "instance document parsing failed";
        }

      protected:
        virtual void
        print (std::basic_ostream<C>&) const;

      private:
        tree::errors<C> errors_;
      };


      //
      //
      template <typename C>
      struct expected_element: virtual exception<C>
      {
        virtual
        ~expected_element () throw ()
        {
        }

        expected_element (const std::basic_string<C>& name,
                          const std::basic_string<C>& namespace_)
            : name_ (name), namespace__ (namespace_)
        {
        }

        // Expected element.
        //
        const std::basic_string<C>&
        name () const
        {
          return name_;
        }

        const std::basic_string<C>&
        namespace_ () const
        {
          return namespace__;
        }

        virtual const char*
        what () const throw ()
        {
          return "expected element not encountered";
        }

      protected:
        virtual void
        print (std::basic_ostream<C>&) const;

      private:
        std::basic_string<C> name_;
        std::basic_string<C> namespace__;
      };


      //
      //
      template <typename C>
      struct unexpected_element: virtual exception<C>
      {
        virtual
        ~unexpected_element () throw ()
        {
        }

        unexpected_element (const std::basic_string<C>& encountered_name,
                            const std::basic_string<C>& encountered_namespace,
                            const std::basic_string<C>& expected_name,
                            const std::basic_string<C>& expected_namespace)
            : encountered_name_ (encountered_name),
              encountered_namespace_ (encountered_namespace),
              expected_name_ (expected_name),
              expected_namespace_ (expected_namespace)
        {
        }

        // Encountered element.
        //
        const std::basic_string<C>&
        encountered_name () const
        {
          return encountered_name_;
        }

        const std::basic_string<C>&
        encountered_namespace () const
        {
          return encountered_namespace_;
        }

        // Expected element. Can be empty.
        //
        const std::basic_string<C>&
        expected_name () const
        {
          return expected_name_;
        }

        const std::basic_string<C>&
        expected_namespace () const
        {
          return expected_namespace_;
        }

        virtual const char*
        what () const throw ()
        {
          return "unexpected element encountered";
        }

      protected:
        virtual void
        print (std::basic_ostream<C>&) const;

      private:
        std::basic_string<C> encountered_name_;
        std::basic_string<C> encountered_namespace_;
        std::basic_string<C> expected_name_;
        std::basic_string<C> expected_namespace_;
      };


      //
      //
      template <typename C>
      struct expected_attribute: virtual exception<C>
      {
        virtual
        ~expected_attribute () throw ()
        {
        }

        expected_attribute (const std::basic_string<C>& name,
                            const std::basic_string<C>& namespace_)
            : name_ (name), namespace__ (namespace_)
        {
        }

        // Expected attribute.
        //
        const std::basic_string<C>&
        name () const
        {
          return name_;
        }

        const std::basic_string<C>&
        namespace_ () const
        {
          return namespace__;
        }

        virtual const char*
        what () const throw ()
        {
          return "expected attribute not encountered";
        }

      protected:
        virtual void
        print (std::basic_ostream<C>&) const;

      private:
        std::basic_string<C> name_;
        std::basic_string<C> namespace__;
      };


      //
      //
      template <typename C>
      struct unexpected_enumerator: virtual exception<C>
      {
        virtual
        ~unexpected_enumerator () throw ()
        {
        }

        unexpected_enumerator (const std::basic_string<C>& enumerator)
            : enumerator_ (enumerator)
        {
        }

        const std::basic_string<C>&
        enumerator () const
        {
          return enumerator_;
        }

        virtual const char*
        what () const throw ()
        {
          return "unexpected enumerator encountered";
        }

      protected:
        virtual void
        print (std::basic_ostream<C>&) const;

      private:
        std::basic_string<C> enumerator_;
      };


      //
      //
      template <typename C>
      struct no_type_info: virtual exception<C>
      {
        virtual
        ~no_type_info () throw ()
        {
        }

        no_type_info (const std::basic_string<C>& type_name,
                      const std::basic_string<C>& type_namespace)
            : type_name_ (type_name),
              type_namespace_ (type_namespace)
        {
        }

        const std::basic_string<C>&
        type_name () const
        {
          return type_name_;
        }

        const std::basic_string<C>&
        type_namespace () const
        {
          return type_namespace_;
        }

        virtual const char*
        what () const throw ()
        {
          return "no type information registered for a type";
        }

      protected:
        virtual void
        print (std::basic_ostream<C>&) const;

      private:
        const std::basic_string<C>& type_name_;
        const std::basic_string<C>& type_namespace_;
      };

      //
      //
      template <typename C>
      struct not_derived: virtual exception<C>
      {
        virtual
        ~not_derived () throw ()
        {
        }

        not_derived (const std::basic_string<C>& base_type_name,
                     const std::basic_string<C>& base_type_namespace,
                     const std::basic_string<C>& derived_type_name,
                     const std::basic_string<C>& derived_type_namespace)
            : base_type_name_ (base_type_name),
              base_type_namespace_ (base_type_namespace),
              derived_type_name_ (derived_type_name),
              derived_type_namespace_ (derived_type_namespace)
        {
        }

        const std::basic_string<C>&
        base_type_name () const
        {
          return base_type_name_;
        }

        const std::basic_string<C>&
        base_type_namespace () const
        {
          return base_type_namespace_;
        }

        const std::basic_string<C>&
        derived_type_name () const
        {
          return derived_type_name_;
        }

        const std::basic_string<C>&
        derived_type_namespace () const
        {
          return derived_type_namespace_;
        }

        virtual const char*
        what () const throw ()
        {
          return "type is not derived";
        }

      protected:
        virtual void
        print (std::basic_ostream<C>&) const;

      private:
        const std::basic_string<C>& base_type_name_;
        const std::basic_string<C>& base_type_namespace_;
        const std::basic_string<C>& derived_type_name_;
        const std::basic_string<C>& derived_type_namespace_;
      };


      //
      //
      template <typename C>
      struct duplicate_id: virtual exception<C>
      {
        virtual
        ~duplicate_id () throw ()
        {
        }

        duplicate_id (const std::basic_string<C>& id)
            : id_ (id)
        {
        }

        const std::basic_string<C>&
        id () const
        {
          return id_;
        }

        virtual const char*
        what () const throw ()
        {
          return "ID already exist";
        }

      protected:
        virtual void
        print (std::basic_ostream<C>&) const;

      private:
        std::basic_string<C> id_;
      };


      //
      //
      template <typename C>
      struct serialization: virtual exception<C>
      {
        virtual
        ~serialization () throw ()
        {
        }

        serialization ()
        {
        }

        serialization (const tree::errors<C>& errors)
            : errors_ (errors)
        {
        }

        const tree::errors<C>&
        errors () const
        {
          return errors_;
        }

        virtual const char*
        what () const throw ()
        {
          return "representation serialization failed";
        }

      protected:
        virtual void
        print (std::basic_ostream<C>&) const;

      private:
        tree::errors<C> errors_;
      };


      //
      //
      template <typename C>
      struct no_namespace_mapping: virtual exception<C>
      {
        virtual
        ~no_namespace_mapping () throw ()
        {
        }

        no_namespace_mapping (const std::basic_string<C>& namespace_)
            : namespace__ (namespace_)
        {
        }

        // Namespace for which prefix-namespace mapping hasn't been
        // provided.
        //
        const std::basic_string<C>&
        namespace_ () const
        {
          return namespace__;
        }

        virtual const char*
        what () const throw ()
        {
          return "no mapping provided for a namespace";
        }

      protected:
        virtual void
        print (std::basic_ostream<C>&) const;

      private:
        std::basic_string<C> namespace__;
      };


      //
      //
      template <typename C>
      struct no_prefix_mapping: virtual exception<C>
      {
        virtual
        ~no_prefix_mapping () throw ()
        {
        }

        no_prefix_mapping (const std::basic_string<C>& prefix)
            : prefix_ (prefix)
        {
        }

        // Prefix for which prefix-namespace mapping hasn't been
        // provided.
        //
        const std::basic_string<C>&
        prefix () const
        {
          return prefix_;
        }

        virtual const char*
        what () const throw ()
        {
          return "no mapping provided for a namespace prefix";
        }

      protected:
        virtual void
        print (std::basic_ostream<C>&) const;

      private:
        std::basic_string<C> prefix_;
      };


      //
      //
      template <typename C>
      struct xsi_already_in_use: virtual exception<C>
      {
        virtual const char*
        what () const throw ()
        {
          return "namespace prefix 'xsi' is already in use and no "
            "user-defined mapping has been provided for namespace "
            "'http://www.w3.org/2001/XMLSchema-instance'";
        }

      protected:
        virtual void
        print (std::basic_ostream<C>&) const;
      };


      //
      //
      template <typename C>
      struct bounds: virtual exception<C>
      {
        virtual const char*
        what () const throw ()
        {
          return "buffer boundary rules have been violated";
        }

      protected:
        virtual void
        print (std::basic_ostream<C>&) const;
      };
    }
  }
}

#endif  // XSD_CXX_TREE_EXCEPTIONS_HXX

#include <xsd/cxx/tree/exceptions.ixx>
