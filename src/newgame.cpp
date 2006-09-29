/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <iostream>
#include <QComboBox>
#include <QHBoxLayout>
#include "common.h"
#include "highlevel.h"
#include "variants/variants.h"
#include "newgame.h"

NewGame::NewGame(QWidget* parent, bool allowCurrent)
: QDialog(parent)
, m_custom_opt_widget(NULL)
, m_custom_opt_layout(NULL) {
  setupUi(this);

  std::cout << "allow current = " << allowCurrent << std::endl;
  m_allow_current = allowCurrent;
  connect(cmbVariant, SIGNAL(currentIndexChanged(const QString&)),
          this, SLOT(variantChanged(const QString&)));

  m_custom_opt_layout = new QHBoxLayout(widgetCustom);

  const Variant::Variants& all = Variant::allVariants();
  for(Variant::Variants::const_iterator it = all.begin(); it != all.end(); ++it)
    cmbVariant->addItem(it->first, QVariant(it->first));
}

QString NewGame::variant() const {
  QString res = cmbVariant->itemData(cmbVariant->currentIndex()).toString();
  return res;
}

bool NewGame::playFromCurrent() const {
  return rdCurrent->isChecked();
}

bool NewGame::isCustom() const {
  return rdCustom->isChecked();
}

void NewGame::variantChanged(const QString& var) {
  std::cout << "var = " << var << std::endl;
  bool enabled = m_allow_current && var == "Chess";
  rdCurrent->setEnabled(enabled);
  if (!enabled)
    rdStandard->setChecked(true);

  if(m_custom_opt_widget)
    delete m_custom_opt_widget;
  m_custom_options = OptList();
  VariantInfo* vi = Variant::variant(var);
  if(vi) {
    m_custom_options = vi->positionOptions();
    m_custom_opt_widget = new OptionWidget(m_custom_options, widgetCustom);
    m_custom_opt_layout->addWidget(m_custom_opt_widget);
  }
}

#include "newgame.moc"
