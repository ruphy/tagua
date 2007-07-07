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

#include "settings.h"
#ifdef __SETTINGS_DEBUG
#include <QFile>
#include <QTextStream>
#endif

SettingConstRef::SettingConstRef(const QDomElement& element)
: m_element(element) {
  if (!m_element.isNull()) {
    if (m_element.firstChild().toText().isNull())
      m_element.appendChild(m_element.ownerDocument().createTextNode( QString() ));
  }
}

SettingRefBase::operator bool() const {
  return !element().isNull();
}

bool SettingRefBase::flag(const QString& attr, bool def) const {
  return element().attribute(attr, def ? "true" : "false") == "true";
}

SettingRef::SettingRef(const QDomElement& parent, const QString& key)
: m_parent(parent)
, m_key(key) { }

void SettingRef::remove() {
  m_parent.removeChild(element());
}

Settings::Settings(const QDomElement& node)
: m_node(node) { }

Settings::Settings(const Settings& other)
: m_node(other.node()) { }

Settings& Settings::operator=(const Settings& other) {
  m_node = other.node();
  return *this;
}

SettingRef Settings::operator[](const QString& key) {
  return SettingRef(node(), key);
}

SettingConstRef Settings::operator[](const QString& key) const {
  return SettingConstRef(node().firstChildElement(key));
}

SettingGroup Settings::group(const QString& name) const {
  return SettingGroup(node(), name);
}

SettingGroup Settings::operator()(const QString& name) const {
  return group(name);
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
  Q_ASSERT(!node.isNull());
}

bool Settings::flag(const QString& attr, bool def) const {
  //std::cout << "get flag " << node().isNull() << " " << attr << " = "
    //<< node().attribute(attr, def ? "true" : "false") << std::endl;
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
  Q_ASSERT(!m_node.isNull());
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

#ifdef __SETTINGS_DEBUG
void Settings::dump() const {
  QFile outf;
  outf.open(stdout, QIODevice::WriteOnly);
  QTextStream out(&outf);

  if (node().isNull()) {
    out << "<NULL NODE />" << endl;
    return;
  }

  QDomDocument temp;
  temp.appendChild(temp.importNode(node(), true));
  out << temp.toString() << endl;
}
#endif
