/*
Copyright 2006-2007 Paolo Capriotti <paolo.capriotti@kdemail.net>

BSD License
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <map>
#include <vector>
#include <iostream>
#include <QDomElement>
#include <QDomText>
#include <QObject>
#include <QColor>
#include <QFont>

template <typename T> class TypedSettingRef;

template <typename T> struct ReturnType {
  typedef T type;
  typedef TypedSettingRef<type> wrapper;
};

template <typename T, size_t n> struct ReturnType<T[n]> {
  typedef const T* type;
  typedef type wrapper;
};

template <typename T>
struct SettingConversionBase {
  static typename ReturnType<T>::type getValue(const QDomElement&, const QDomText& text) {
    return text.data();
  }

  static void setValue(QDomElement, QDomText text, const T& val) {
    text.setData(val);
  }
};

template <typename T>
struct SettingConversion : public SettingConversionBase<T> { };

template <size_t n>
struct SettingConversion<char[n]> : public SettingConversionBase<char[n]> {
  static const char* getValue(const QDomElement&, const QDomText& text) {
    return qPrintable(text.data());
  }

  static void setValue(QDomElement, QDomText text, const char* val) {
    text.setData(val);
  }
};

template <>
struct SettingConversion<int> {
  static int getValue(const QDomElement&, const QDomText& text) {
    return text.data().toInt();
  }

  static void setValue(QDomElement, QDomText text, int val) {
    text.setData(QString::number(val));
  }
};

template <>
struct SettingConversion<QColor> {
  static QColor getValue(const QDomElement& e, const QDomText&) {
    int r = e.attribute("r", "0").toInt();
    int g = e.attribute("g", "0").toInt();
    int b = e.attribute("b", "0").toInt();

    return QColor(r, g, b);
  }

  static void setValue(QDomElement e, QDomText, const QColor& val) {
    e.setAttribute("r", val.red());
    e.setAttribute("g", val.green());
    e.setAttribute("b", val.blue());
  }
};

template <>
struct SettingConversion<Qt::GlobalColor> : public SettingConversion<QColor> { };

template <>
struct SettingConversion<QFont> {
  static QFont getValue(const QDomElement&, const QDomText& text) {
    QFont res;
    res.fromString(text.data());
    return res;
  }

  static void setValue(QDomElement, QDomText text, const QFont& font) {
    text.setData(font.toString());
  }
};

template <>
struct SettingConversion<bool> {
  static bool getValue(const QDomElement&, const QDomText& text) {
    return text.data().toLower() == "true";
  }

  static void setValue(QDomElement, QDomText text, bool val) {
    text.setData(val ? "true" : "false");
  }
};

class SettingRefBase {
protected:
  virtual QDomElement element() const = 0;
  virtual QDomText node() const { return element().firstChild().toText(); }
public:
  virtual ~SettingRefBase() { }
  template <typename T> typename ReturnType<T>::wrapper operator|(const T& val) const;
  template <typename T> typename ReturnType<T>::type value(const T& defaultValue) const;
  template <typename T> typename ReturnType<T>::type value() const;
  template <typename T> void operator>>(T& out) const;
  operator bool() const;
  bool flag(const QString& attr, bool def) const;
};

class SettingConstRef : public SettingRefBase {
  QDomElement m_element;
protected:
  virtual QDomElement element() const { return m_element; }
public:
  SettingConstRef(const QDomElement& node);
};

class SettingRef : public SettingRefBase {
  QDomElement m_parent;
  QString m_key;
protected:
  virtual QDomElement element() const { return m_parent.firstChildElement(m_key); }
public:
  SettingRef(const QDomElement& node, const QString& key);

  void remove();
  template <typename T> typename ReturnType<T>::wrapper operator|=(const T& val);
  template <typename T> typename ReturnType<T>::wrapper operator=(const T& val);
};

// class GenericSettingRef : public SettingRef {
//   QDomElement m_element;
// protected:
//   virtual QDomElement element() const { return m_element; }
// public:
//   GenericSettingRef(const QDomElement& element, const QString& key);
// };

template <typename T>
class TypedSettingRef {
  T m_value;
public:
  TypedSettingRef(const T& value)
  : m_value(value) { }

  operator typename ReturnType<T>::type() const { return value(); }
  template <typename U> void operator>>(U& out) const { out = m_value; }
  typename ReturnType<T>::type value() const { return m_value; }
};

template <typename T> class SettingMap;

/**
  * @brief The Settings class provides persistent application settings on an XML file.
  * 
  * Use this class to store your application settings if you need an hierarchical 
  * configuration stored on a human-readable text file.
  * 
  * Settings completely abstracts the serialization and deserialization procedures for
  * simple data types, and allows a customized XML serialization for user defined types.
  * 
  * The configuration is hierarchical, with a Settings object representing a node of the
  * tree. Each node can contain key-value pairs, accessed as
  * <code>settings["key"]</code>
  * both for reading and writing.
  * 
  * Accessing a group returns another Settings object, and there's no need to "close" the
  * group (like with the QSettings class) when you're done reading or writing.
  * 
  * Settings support aggregates of objects in the form of linear or associative arrays.
  */
class Settings {
  friend class SettingArray;
protected:
  QDomElement m_node;
  virtual QDomElement node() const { return m_node; }
public:
  /** Create an invalid Settings object. */
  Settings() { }
  virtual ~Settings() { }
  /** 
    * Create a Settings object attached on a specified XML node.
    * @param node An XML node to be used as the root of the Settings
    */
  Settings(const QDomElement& node);
  /**
    * Create a copy of the Settings object
    */
  Settings(const Settings& other);
  
  /**
    * Assign a Settings object to another.
    */
  Settings& operator=(const Settings& other);

  /** 
    * Read the value associated with @a key. Values returned from
    * a read operation on Settings can be converted using the
    * template member function @a value.
    */
  SettingRef operator[](const QString& key);

  /**
    * Just like the above function, but returns a constant reference.
    */
  SettingConstRef operator[](const QString& key) const;

  /**
    * Overload
    */
  SettingRef operator[](const char* key) { return operator[](QString(key)); }

  /**
    * Overload
    */
  SettingConstRef operator[](const char* key) const { return operator[](QString(key)); }

  /**
    * Access a setting group.
    * @param name The name of the group.
    * @returns A Settings object representing the specified group.
    */
  class SettingGroup group(const QString& name) const;

  /**
    * Shotcat to access a setting group, same as \a group
    * @param name The name of the group.
    * @returns A Settings object representing the specified group.
    */
  class SettingGroup operator()(const QString& name) const;

  /**
    * Access a setting map with keys of type @a T.
    * @param element The name of every map element.
    * @param key The element acting as a key.
    * @returns A SettingMap object representing the specified map.
    * @sa SettingMap
    */
  template <typename T> SettingMap<T> map(const QString& element, const QString& key) const;
  
  /**
   * Just like the above function, but clear the returned map. Use this
   * function if the desired behaviour is to overwrite the existing map
   * in the associated setting node.
   */
  template <typename T> SettingMap<T> newMap(const QString& element, const QString& key) const;
  
  /**
    * Access a setting array.
    * @param element The name of every array element.
    * @returns a SettingArray object representing the specified array.
    * @sa SettingArray
    */
  class SettingArray array(const QString& element) const;
  
  /**
   * Just like the above function, but clear the returned array. Use this
   * function if the desired behaviour is to overwrite the existing array
   * in the associated setting node.
   */  
  class SettingArray newArray(const QString& element) const;

  /** 
    * Retrieve a global boolean flag for the associated setting node.
    * Boolean flags are stored as XML attributes in the configuration file.
    * @param attr The name of the flag.
    * @param def The default value for the flag.
    * @returns The flag value, or @a def, in case the specified flag does not exist.
    */
  bool flag(const QString& attr, bool def) const;
  /** 
    * Set a global boolean flag for the associated setting node.
    * Boolean flags are stored as XML attributes in the configuration file.
    * @param attr The name of the flag.
    * @param value The value of the flag.
    */
  void setFlag(const QString& attr, bool value);

  /**
    * @note This function is internal to the Settings class framework. Do not use
    *       in client code.
    */
  static void ensureExistence(QDomElement& node, QDomElement parent, const QString& name);
  

#ifdef __SETTINGS_DEBUG
  void dump() const;
#endif
};

/**
  * @brief A group of settings.
  *
  * Client code can use instance of this class just like they were Settings objects.
  * The only external difference is that SettingGroup object can be implicitly
  * converted to bool to test their existence.
  * 
  * SettingGroup can be accessed with the Settings member function @a group.
  * @sa Settings::group
  */
class SettingGroup : public Settings {
  QString m_name;
  QDomElement m_parent;
protected:
  virtual QDomElement node() const;
public:
  SettingGroup(const QDomElement& parent, const QString& name);

  /**
    * Check if the SettingGroup exists.
    */
  operator bool() const { return !m_node.isNull(); }
  void remove();
};

/**
  * @brief A collection of setting nodes arranged as an associative container.
  * 
  * A setting map is a collection of setting nodes, all having the same
  * parent node and the same name, and containing an inner node acting as
  * a key.
  * The following is an example of how the configuration node containing a 
  * setting map may look like. The element name is "event" and the key name
  * is "name".
  * 
  * <pre>
  * <event>
  * <name>click</name>
  *   <action>action1</action>
  * </event>
  * <event>
  *   <name>double-click</name>
  *   <action>action2</action>
  * </event>
  * </pre>
  * 
  * Other than the key, each element of the map may contain any arbitrarily
  * nested nodes.
  * 
  * To use a map, you simply access its elements by keys using the @a get member
  * function, which returns a Settings object associated to the specified element.
  * 
  * Elements are cached as an STL map, which is kept in sync with the configuration
  * file, so read operation have the same complexity of the corresponding
  * STL operations (i.e. logarithmic).
  */
template <typename Key>
class SettingMap {
  std::map<Key, Settings> m_map;
protected:
  mutable QDomElement m_node;
  virtual QDomElement node() const { return m_node; }
  QString m_element;
  QString m_key;
public:
  SettingMap(const QDomElement& node, const QString& element, const QString& key);
  virtual ~SettingMap() { }
  /**
    * @returns The number of elements in the map.
    */
  uint size() const { return m_map.size(); }

  /**
    * Retrieve an element by key.
    * @param index The key of the element.
    * @returns The Settings object corresponding to the specified element.
    */
  Settings get(const Key& index) const;
  
  /**
    * Insert an element into the map.
    * @param index The key of the element.
    * @returns An Settings object containing only the specified key. This object
    *          can be used to add arbitrary content to the map element.
    */
  Settings insert(const Key& index);

  /**
    * Clear the map, removing all its elements.
    */
  void clear();
};

/**
  * @brief A collection of nodes, acting as a linear container.
  * 
  * A setting array is like a setting map without a special key
  * node inside every element.
  * 
  * The elements of a setting array can be only accessed by a numerical
  * index, just like an STL vector.
  * 
  * SettingsArray provides a STL-like container interface with
  * random access constant iterators, so that the collection can be
  * used for STL algorithms acting read-only on it.
  */
class SettingArray {
  std::vector<Settings> m_array;
protected:
  mutable QDomElement m_node;
  virtual QDomElement node() const { return m_node; }
  QString m_element;
public:
  typedef std::vector<Settings>::const_iterator iterator;
  typedef std::vector<Settings>::const_iterator const_iterator;

  SettingArray(const QDomElement& node, const QString& element);
  virtual ~SettingArray() { }
  
  /**
    * @returns The number of elements in the array.
    */
  uint size() const { return m_array.size(); }

  /**
    * Retrieve an element by index.
    * @param index The (0-based) index of the element.
    * @returns A Settings object associated to the specified element.
    */
  Settings get(int index) const;
  
  /**
    * Insert an element before the specified index.
    * @param index The (0-based) index of the element.
    * @returns An empty Settings object. This object can be used to add 
    *          arbitrary content to the array element.
    */
  Settings insert(int index);
  
  /**
    * Append an element into the array.
    * @returns An empty Settings object. This object can be used to add 
    *          arbitrary content to the array element.
    */
  Settings append();
  
  /**
    * Clear the array, removing all its elements.
    */
  void clear();

  /**
    * @returns A random access iterator pointing to the beginning of the array.
    */
  const_iterator begin() const { return m_array.begin(); }
  
  /**
    * @returns A random access iterator pointing past the end of the array.
    */
  const_iterator end() const { return m_array.end(); }
  
  operator bool() const { return !m_node.isNull(); }
};


// Implementation

template <typename T>
typename ReturnType<T>::type SettingRefBase::value(const T& def) const {
  if (element().isNull()) return def;
  else return SettingConversion<T>::getValue(element(), node());
}

template <typename T>
typename ReturnType<T>::type SettingRefBase::value() const {
  return value(T());
}

template <typename T>
typename ReturnType<T>::wrapper SettingRefBase::operator|(const T& val) const {
  return value(val);
}

template <typename T>
void SettingRefBase::operator>>(T& out) const {
  out = value(out);
}

template <typename T>
typename ReturnType<T>::wrapper SettingRef::operator|=(const T& val) {
  if (element().isNull())
    return operator=(val);
  return value(val);
}

template <typename T>
typename ReturnType<T>::wrapper SettingRef::operator=(const T& val) {
  if (element().isNull())
    m_parent.appendChild(m_parent.ownerDocument().createElement(m_key));
  element().appendChild(element().ownerDocument().createTextNode(""));
  SettingConversion<T>::setValue(element(), node(), val);
  return val;
}

template <typename T>
SettingMap<T> Settings::map(const QString& element, const QString& key) const {
  return SettingMap<T>(node(), element, key);
}

template <typename T>
SettingMap<T> Settings::newMap(const QString& element, const QString& key) const {
  SettingMap<T> res(node(), element, key);
  res.clear();
  return res;
}


template <typename Key>
SettingMap<Key>::SettingMap(const QDomElement& node, const QString& element, const QString& key)
: m_node(node)
, m_element(element)
, m_key(key) {
  if (!m_node.isNull()) {
    // scan all 'name' elements of node
    QDomNodeList elements = m_node.elementsByTagName(m_element);
    for (int i = 0; i < elements.size(); i++) {
      QDomElement el = elements.item(i).toElement();

      // find the key inside el
      QDomElement key_element = el.firstChildElement(m_key);
      Key k = SettingConstRef(key_element).value<Key>();

      // insert the corresponding Settings object into the map
      m_map[k] = el;
    }
  }
}

template <typename Key>
Settings SettingMap<Key>::get(const Key& key) const {
  return m_map.find(key)->second;
}

template <typename Key>
Settings SettingMap<Key>::insert(const Key& key) {
  typename std::map<Key, Settings>::const_iterator it = m_map.find(key);
  if (it != m_map.end())
    return it->second;
  else {
    QDomElement el = node().ownerDocument().createElement(m_element);
    node().appendChild(el);
    Settings res = el;
    m_map[key] = res;

    res[m_key] = key;
    return res;
  }
}

template <typename Key>
void SettingMap<Key>::clear() {
  QDomNode it = node().firstChild();
  while (!it.isNull()) {
    QDomNode nextit = it.nextSibling();
    QDomElement e = it.toElement();
    if (!e.isNull() && e.tagName() == m_element)
      node().removeChild(e);
    it = nextit;
  }
}

#endif // SETTINGS_H


