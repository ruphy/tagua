#include "settings.h"
#include <iostream>

SettingConstRef::SettingConstRef(const QDomElement& element)
: m_element(element) {
  if (!m_element.isNull()) {
    if (m_element.firstChild().toText().isNull())
      m_element.appendChild(m_element.ownerDocument().createTextNode(""));
  }
}

SettingRefBase::operator bool() const {
  return !element().isNull();
}

bool SettingRefBase::flag(const QString& attr, bool def) const {
  return element().attribute(attr, def ? "true" : "false") == "true";
}

void SettingRefBase::setFlag(const QString& attr, bool val) {
  element().setAttribute(attr, val ? "true" : "false");
}

SettingRef::SettingRef(const QDomElement& parent, const QString& key)
: m_parent(parent)
, m_key(key) { }

void SettingRef::remove() {
  m_parent.removeChild(element());
}

Settings::Settings(const QDomElement& node)
: m_node(node) { }

SettingRef Settings::operator[](const QString& key) {
  return SettingRef(node(), key);
}

SettingConstRef Settings::operator[](const QString& key) const {
  return SettingConstRef(node().firstChildElement(key));
}

SettingGroup Settings::group(const QString& name) const {
  return SettingGroup(node(), name);
}

SettingArray Settings::array(const QString& name) const {
  return SettingArray(node(), name);
}

SettingArray Settings::newArray(const QString& name) const {
  SettingArray res(node(), name);
  res.clear();
  return res;
}

void Settings::ensureExistence(QDomElement& node, QDomElement parent, const QString& name) {
  if (node.isNull()) {
    node = parent.firstChildElement(name);
    if (node.isNull()) {
      node = parent.ownerDocument().createElement(name);
      parent.appendChild(node);
    }
  }
}

bool Settings::flag(const QString& attr, bool def) const {
  return node().attribute(attr, def ? "true" : "false") == "true";
}

void Settings::setFlag(const QString& attr, bool val) {
  node().setAttribute(attr, val ? "true" : "false");
}

SettingGroup::SettingGroup(const QDomElement& parent, const QString& name)
: m_name(name) 
, m_parent(parent) { }

QDomElement SettingGroup::node() const {
  ensureExistence(const_cast<QDomElement&>(m_node), m_parent, m_name);
  return m_node;
}

void SettingGroup::remove() {
  node().parentNode().removeChild(node());
}

SettingArray::SettingArray(const QDomElement& node, const QString& element)
: m_node(node)
, m_element(element) {
  QDomNodeList elements = m_node.elementsByTagName(m_element);
  m_array.resize(elements.size());
  for (int i = 0; i < elements.size(); i++)
    m_array[i] = elements.item(i).toElement();
}
  
Settings SettingArray::get(int index) const {
  return m_array[index];
}

Settings SettingArray::insert(int index) {
  QDomElement el = node().ownerDocument().createElement(m_element);
  node().insertBefore(el, m_array[index].node());
  Settings res = el;
  m_array.insert(m_array.begin() + index, res);
  return res;
}

Settings SettingArray::append() {
  QDomElement el = node().ownerDocument().createElement(m_element);
  node().appendChild(el);
  Settings res = el;
  m_array.push_back(res);
  return res;
}

void SettingArray::clear() {
  QDomNodeList children = node().childNodes();
  while (children.size() > 0) {
    QDomElement e = children.item(0).toElement();
    if (!e.isNull() || e.tagName() == m_element)
      node().removeChild(e);
  }
}

void Settings::dump() const {
  std::cout << "dumping" << std::endl;
  Q_ASSERT(!node().isNull());
  Q_ASSERT(!node().ownerDocument().isNull());
  std::cout << node().ownerDocument().toString() << std::endl;
}
