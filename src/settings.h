#ifndef SETTINGS_H
#define SETTINGS_H

#include "common.h"
#include <map>
#include <vector>
#include <iostream>
#include <QDomElement>
#include <QDomText>
#include <QObject>
#include <QColor>
#include <QFont>
#include "foreach.hpp"
#undef foreach
#define foreach BOOST_FOREACH

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
  void setFlag(const QString& attr, bool val);
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

class Settings {
  friend class SettingArray;
protected:
  QDomElement m_node;
  virtual QDomElement node() const { return m_node; }
public:
  Settings() { }
  virtual ~Settings() { }
  Settings(const QDomElement& node);

  SettingRef operator[](const QString& key);
  SettingConstRef operator[](const QString& key) const;
  
  class SettingGroup group(const QString& name) const;
  template <typename T> SettingMap<T> map(const QString& element, const QString& key) const;
  template <typename T> SettingMap<T> newMap(const QString& element, const QString& key) const;
  class SettingArray array(const QString& element) const;
  class SettingArray newArray(const QString& element) const;
  
  bool flag(const QString& attr, bool def) const;
  void setFlag(const QString& attr, bool val);
  
  static void ensureExistence(QDomElement& node, QDomElement parent, const QString& name);
  void dump() const;
};

class SettingGroup : public Settings {
  QString m_name;
  QDomElement m_parent;
protected:
  virtual QDomElement node() const; 
public:
  SettingGroup(const QDomElement& parent, const QString& name);
  
  operator bool() const { return !m_node.isNull(); }
  void remove();
};

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
  uint size() const { return m_map.size(); }
  
  Settings get(const Key& index) const;
  Settings insert(const Key& index);
  
  void clear();
};

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
  uint size() const { return m_array.size(); }
  
  Settings get(int index) const;
  Settings insert(int index);
  Settings append();
  void clear();
  
  const_iterator begin() const { return m_array.begin(); }
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
  return val;
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
    // scan all elements all 'name' elements of node
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
  else {
//    std::cout << "Error! creating a map on a null node" << std::endl;
    TRAP();
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
  QDomNodeList children = node().childNodes();
  while (children.size() > 0) {
    QDomElement e = children.item(0).toElement();
    if (!e.isNull() && e.tagName() == m_element)
      node().removeChild(e);
  }
}

#endif // SETTINGS_H


