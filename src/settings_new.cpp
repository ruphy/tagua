#include "settings.h"

SettingConstRef::SettingConstRef(const QDomElement& element)
: m_element(element) {
  if (!m_element.isNull()) {
    if (m_element.firstChild().toText().isNull())
      m_element.appendChild(m_element.ownerDocument().createTextNode(""));
  }
}

SettingRefBase::operator bool() const {
  return element().isNull();
}

SettingRef::SettingRef(const QDomElement& parent, const QString& key)
: m_parent(parent)
, m_key(key) { }

Settings::Settings(const QDomElement& node)
: m_node(node) { }

SettingRef Settings::operator[](const QString& key) {
  return SettingRef(m_node, key);
}

SettingConstRef Settings::operator[](const QString& key) const {
  return SettingConstRef(m_node.firstChildElement(key));
}

Settings Settings::group(const QString& name) const {
  return Settings(m_node.firstChildElement(name));
}

SettingArray Settings::array(const QString& name) const {
  return SettingArray(m_node, name);
}


SettingArray::SettingArray(const QDomElement& node, const QString& name)
: m_node(node)
, m_name(name) {
  QDomNodeList elements = m_node.elementsByTagName(m_name);
  m_array.resize(elements.size());
  for (int i = 0; i < elements.size(); i++)
    m_array[i] = elements.item(i).toElement();
}
  
Settings SettingArray::get(int index) const {
  return m_array[index];
}

Settings SettingArray::insert(int index) {
  QDomElement el = m_node.ownerDocument().createElement(m_name);
  m_node.insertBefore(el, m_array[index].innerNode());
  Settings res = el;
  m_array.insert(m_array.begin() + index, res);
  return res;
}

Settings SettingArray::append() {
  QDomElement el = m_node.ownerDocument().createElement(m_name);
  m_node.appendChild(el);
  Settings res = el;
  m_array.push_back(res);
  return res;
}

