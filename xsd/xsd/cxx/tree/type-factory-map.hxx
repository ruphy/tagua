// file      : xsd/cxx/tree/type-factory-map.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_TREE_TYPE_FACTORY_MAP_HXX
#define XSD_CXX_TREE_TYPE_FACTORY_MAP_HXX

#include <map>
#include <string>
#include <memory> // std::auto_ptr

#include <xercesc/dom/DOMElement.hpp>

#include <xsd/cxx/xml/string.hxx>        // xml::{string, transcode}
#include <xsd/cxx/xml/elements.hxx>      // xml::{prefix, uq_name}
#include <xsd/cxx/xml/bits/literals.hxx> // xml::bits::{xsi_namespace, type}
#include <xsd/cxx/xml/dom/elements.hxx>  // dom::element

#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/types.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      template <typename C>
      struct type_factory_map
      {
        typedef std::auto_ptr<type> (*factory) (const xercesc::DOMElement&,
                                                flags,
                                                type*);
        void
        register_type (const std::basic_string<C>& type_id,
                       factory f,
                       bool override = true)
        {
          if (override || type_map_.find (type_id) == type_map_.end ())
            type_map_[type_id] = f;
        }

        void
        register_element (const std::basic_string<C>& root_id,
                          const std::basic_string<C>& subst_id,
                          factory f)
        {
          element_map_[root_id][subst_id] = f;
        }


        template <typename X>
        std::auto_ptr<X>
        create (const std::basic_string<C>& name, // element name
                const std::basic_string<C>& ns,   // element namespace
                bool global,
                bool qualified,
                const xml::dom::element<C>& e,
                tree::flags flags,
                type* c) const
        {
          factory f = 0;

          // See if we've got a straight match.
          //
          if (e.name () == name &&
              (qualified ? e.namespace_ () == ns : ns.empty ()))
          {
            f = &traits_adapter<X>;
          }
          else if (global)
          {
            // See if we have a substitution.
            //
            typename element_map::const_iterator i (
              element_map_.find (name + C (' ') + ns));

            if (i != element_map_.end ())
            {
              f = find_substitution (i->second,
                                     e.name () + C (' ') + e.namespace_ ());
            }
          }

          if (f == 0)
            return std::auto_ptr<X> (0); // No match.

          // Check for xsi:type
          //
          {
            std::basic_string<C> xsi_type (
              e.attribute (xml::bits::type<C> (),
                           xml::bits::xsi_namespace<C> ()));

            if (!xsi_type.empty ())
              f = find_type (xsi_type, e.dom_element ());
          }

          // Now create and cast the instance.
          //
          std::auto_ptr<type> tmp (f (e.dom_element (), flags, c));
          std::auto_ptr<X> r (dynamic_cast<X*> (tmp.get ()));

          if (r.get ())
            tmp.release ();
          else
            throw not_derived<C> (
              std::basic_string<C> (), std::basic_string<C> (),
              std::basic_string<C> (), std::basic_string<C> ()); //@@ TODO

          return r;
        }

        type_factory_map ();

      private:
        template<typename X>
        static std::auto_ptr<type>
        traits_adapter (const xercesc::DOMElement& e, flags f, type* c)
        {
          std::auto_ptr<X> r (traits<X, C>::create (e, f, c));
          return std::auto_ptr<type> (r.release ());
        }

      private:
        typedef
        std::map<std::basic_string<C>, factory>
        type_map;

        // Map of (root-element to map of (subst-element to factory)).
        //
        typedef
        std::map<std::basic_string<C>, factory>
        subst_map;

        typedef
        std::map<std::basic_string<C>, subst_map>
        element_map;

        type_map type_map_;
        element_map element_map_;

      private:
        factory
        find_substitution (const subst_map& start,
                           const std::basic_string<C>& id) const
        {
          typename subst_map::const_iterator i (start.find (id));

          if (i != start.end ())
            return i->second;
          else
          {
            for (i = start.begin (); i != start.end (); ++i)
            {
              typename element_map::const_iterator j (
                element_map_.find (i->first));

              if (j != element_map_.end ())
              {
                if (factory f = find_substitution (j->second, id))
                  return f;
              }
            }
          }

          return 0;
        }

        // The name argument is as specified in xsi:type.
        //
        factory
        find_type (const std::basic_string<C>& name,
                   const xercesc::DOMElement& e) const
        {
          using std::basic_string;

          basic_string<C> ns_name, uq_name (xml::uq_name (name));

          // Copied with modifications from xml/dom/elements.hxx.
          //
          std::basic_string<C> p (xml::prefix (name));

          // 'xml' prefix requires special handling and Xerces folks refuse
          // to handle this in DOM so I have to do it myself.
          //
          if (p == xml::bits::xml_prefix<C> ())
            ns_name = xml::bits::xml_namespace<C> ();
          else
          {
            const XMLCh* xns (
              e.lookupNamespaceURI (
                p.empty () ? 0 : xml::string (p).c_str ()));

            if (xns != 0)
              ns_name = xml::transcode<C> (xns);
            else
            {
              // See if we've got any no-namespace types.
              //
              if (!p.empty ())
                throw no_prefix_mapping<C> (p);
            }
          }

          typename type_map::const_iterator i (
            type_map_.find (uq_name + C (' ') + ns_name));

          if (i == type_map_.end ())
            throw no_type_info<C> (uq_name, ns_name);

          return i->second;
        }
      };


      //
      //
      template<unsigned long id, typename C>
      struct type_factory_plate
      {
        static type_factory_map<C>* map;
        static unsigned long count;

        type_factory_plate ()
        {
          if (count == 0)
            map = new type_factory_map<C>;

          ++count;
        }

        ~type_factory_plate ()
        {
          if (--count == 0)
            delete map;
        }
      };

      template<unsigned long id, typename C>
      type_factory_map<C>* type_factory_plate<id, C>::map = 0;

      template<unsigned long id, typename C>
      unsigned long type_factory_plate<id, C>::count = 0;


      //
      //
      template<unsigned long id, typename C>
      type_factory_map<C>&
      type_factory_map_instance ()
      {
        return *type_factory_plate<id, C>::map;
      }

      //
      //
      template<typename C, typename X>
      std::auto_ptr<type>
      factory_impl (const xercesc::DOMElement& e, flags f, type* c)
      {
        return std::auto_ptr<type> (new X (e, f, c));
      }

      //
      //
      template<unsigned long id, typename C, typename X>
      struct type_factory_initializer
      {
        // Register type.
        //
        type_factory_initializer (const C* type_id)
        {
          type_factory_map_instance<id, C> ().register_type (
            type_id, &factory_impl<C, X>);

          // std::cerr << "registered " << type_id << std::endl;
        }

        // Register element.
        //
        type_factory_initializer (const C* root_id, const C* subst_id)
        {
          type_factory_map_instance<id, C> ().register_element (
            root_id, subst_id, &factory_impl<C, X>);

          // std::cerr << "registered " << type_id << std::endl;
        }
      };
    }
  }
}


#endif // XSD_CXX_TREE_TYPE_FACTORY_MAP_HXX

#include <xsd/cxx/tree/type-factory-map.ixx>
