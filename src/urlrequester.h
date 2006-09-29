/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef URLREQUESTER_H
#define URLREQUESTER_H

#include <qwidget.h>
#include <qstring.h>

class QLineEdit;
class QToolButton;

/**
 * This class provides a widget with a lineedit and a button, which invokes
 * a url dialog (KFileDialog).
 *
 * The lineedit provides a preview of the selected url. The preview text can
 * be customized.
 *
 * \image html urlrequester.png
 *
 */
class UrlRequester : public QWidget
{
  Q_OBJECT

  Q_PROPERTY( QString url READ url WRITE setUrl )

  public:

    /**
     * Constructs a url requester widget.
     *
     * @param parent The parent widget.
     */
    explicit UrlRequester(QWidget *parent=0L);

    /**
     * @return The currently selected font in the requester.
     */
    QString url() const;

    /**
     * @return Pointer to the label used for preview.
     */
    QLineEdit *lineEdit() const { return m_lineEdit; }

    /**
     * @return Pointer to the pushbutton in the widget.
     */
    QToolButton *button() const { return m_button; }

    /**
     * Sets the currently selected url in the requester.
     *
     * @param url The url to select.
     */
    virtual void setUrl( const QString &font);

  Q_SIGNALS:
    /**
     * Emitted when a new @p url has been selected in the underlying dialog
     */
    void urlSelected( const QString &url );

  protected:

    void setToolTip();

  protected Q_SLOTS:

    virtual void buttonClicked();

  protected:

    QLineEdit *m_lineEdit;
    QToolButton *m_button;
};

#endif // URLREQUESTER_H

/* vim: et sw=2 ts=2
*/
