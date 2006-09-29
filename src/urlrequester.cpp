/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "urlrequester.h"

#include <QLineEdit>
#include <QToolButton>
#include <qlayout.h>

#include <QFileDialog>
#include <QApplication>

inline QString i18n(const char* s) { return QApplication::tr(s); }
inline QString i18n(const char* s, const QString& a) { return QApplication::tr(s).arg(a); }

UrlRequester::UrlRequester( QWidget *parent)
    : QWidget( parent )
{
  QHBoxLayout *layout = new QHBoxLayout( this );
  layout->setMargin( 0 );
  layout->setSpacing( 2 );

  m_lineEdit = new QLineEdit( this );
  m_button = new QToolButton( this );
  m_button->setText(i18n( "..." ));

  setFocusProxy( m_button );

  layout->addWidget( m_lineEdit, 1 );
  layout->addWidget( m_button );

  connect( m_button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
  connect( m_lineEdit, SIGNAL(textChanged(const QString&)), this, SIGNAL(urlSelected(const QString&)) );

  setToolTip();
}

QString UrlRequester::url() const {
  return m_lineEdit->text();
}

void UrlRequester::setUrl( const QString &url) {
  m_lineEdit->setText(url);
}

void UrlRequester::buttonClicked()
{
  QString f = QFileDialog::getOpenFileName(this, "?", tr("Open PGN file") );

  if(!f.isEmpty())
    m_lineEdit->setText(f);
}

void UrlRequester::setToolTip()
{
  m_button->setToolTip( i18n( "Click to select a file" ) );

  m_lineEdit->setToolTip( i18n( "Selected URL" ) );
  m_lineEdit->setWhatsThis(  i18n( "This is a preview of the selected URL. You can change it"
      " by clicking the \"...\" button." ) );
}

#include "urlrequester.moc"
