#ifndef SETTINGS_H
#define SETTINGS_H

#include <map>
#include <vector>
#include <QDomElement>
#include <QDomText>
#include <QObject>
#include <QColor>
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
    // ...
    return QColor();
  }
  
  static void setValue(QDomElement, QDomText text, const QColor& val) {
    // ...
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
  
  operator typename ReturnType<T>::type() const { return m_value; }
  template <typename U> void operator>>(const U& out) const { out = m_value; }
};

template <typename T> class SettingMap;

class Settings {
  friend class SettingArray;
  QDomElement m_node;
  QDomElement innerNode() const { return m_node; }
public:
  Settings() { }
  Settings(const QDomElement& node);

  SettingRef operator[](const QString& key);
  SettingConstRef operator[](const QString& key) const;
  
  Settings group(const QString& name) const;
  template <typename T> SettingMap<T> map(const QString& element, const QString& key) const;
  class SettingArray array(const QString& element) const;
};

template <typename Key>
class SettingMap {
  std::map<Key, Settings> m_map;
  QDomElement m_node;
  QString m_name;
  QString m_key;
public:
  SettingMap(const QDomElement& node, const QString& element, const QString& key);
  uint size() const { return m_map.size(); }
  
  Settings get(const Key& index) const;
  Settings insert(const Key& index);
};

class SettingArray {
  std::vector<Settings> m_array;
  QDomElement m_node;
  QString m_name;
public:
  typedef std::vector<Settings>::const_iterator iterator;
  typedef std::vector<Settings>::const_iterator const_iterator;

  SettingArray(const QDomElement& node, const QString& name);
  uint size() const { return m_array.size(); }
  
  Settings get(int index) const;
  Settings insert(int index);
  Settings append();
  
  const_iterator begin() const { return m_array.begin(); }
//   iterator begin() { return m_array.begin(); }
  const_iterator end() const { return m_array.end(); }
//   iterator end() { return m_array.end(); }
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
  return SettingMap<T>(m_node, element, key);
}

template <typename Key>
SettingMap<Key>::SettingMap(const QDomElement& node, const QString& name, const QString& key)
: m_node(node)
, m_name(name)
, m_key(key) {
  if (!m_node.isNull()) {
    // scan all elements all 'name' elements of node
    QDomNodeList elements = m_node.elementsByTagName(name);
    for (int i = 0; i < elements.size(); i++) {
      QDomElement el = elements.item(i).toElement();
      
      // find the key inside el
      QDomElement key_element = el.firstChildElement(key);
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
  QDomElement el = m_node.ownerDocument().createElement(m_name);
  m_node.appendChild(el);
  Settings res = el;
  m_map[key] = res;
  
  res[m_key] = key;
  return res;
}

#endif // SETTINGS_H


