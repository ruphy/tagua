/*
    This file is part of the KDE Libraries

    Copyright (C) 2006 Tobias Koenig (tokoe@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB. If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KPAGEWIDGETMODEL_H
#define KPAGEWIDGETMODEL_H

#include "kpagemodel.h"

class QWidget;

//class KIcon;
typedef class QIcon KIcon;

/**
 * KPageWidgetItem is used by @see KPageWidget and represents
 * a page.
 *
 * <b>Example:</b>\n
 *
 * \code
 *  ColorPage *page = new ColorPage;
 *
 *  KPageWidgetItem *item = new KPageWidgetItem( page, i18n( "Colors" ) );
 *  item->setHeader( i18n( "Colors of Main Window" ) );
 *  item->setIcon( KIcon( "colors" ) );
 *
 *  KPageWidget *pageWidget = new KPageWidget( this );
 *  pageWidget->addPage( item );
 * \endcode
 *
 * @author Tobias Koenig (tokoe@kde.org)
 */
class KDEUI_EXPORT KPageWidgetItem : public QObject
{
  Q_OBJECT

  public:
    /**
     * Creates a new page widget item.
     *
     * @param widget The widget that is shown as page in the KPageWidget.
     * @param name The localized string that is show in the navigation view
     *             of the KPageWidget.
     */
    KPageWidgetItem( QWidget *widget, const QString &name );

    /**
     * Destroys the page widget item.
     */
    ~KPageWidgetItem();

    /**
     * Returns the widget of the page widget item.
     */
    QWidget* widget() const;

    /**
     * Returns the name of the page widget item.
     */
    QString name() const;

    /**
     * Sets the header of the @param header of the page widget item.
     */
    void setHeader( const QString &header );

    /**
     * Returns the header of the page widget item.
     */
    QString header() const;

    /**
     * Sets the @param icon of the page widget item.
     */
    void setIcon( const KIcon &icon );

    /**
     * Returns the icon of the page widget item.
     */
    KIcon icon() const;

    /**
     * Sets whether the page widget item is @param checkable
     * in the view.
     */
    void setCheckable( bool checkable );

    /**
     * Returns whether the page widget item is checkable.
     */
    bool isCheckable() const;

    /**
     * Sets the page widget item checked.
     */
    void setChecked( bool checked );

    /**
     * Returns whether the page widget item is checked.
     */
    bool isChecked() const;

  Q_SIGNALS:
    /**
     * This signal is emitted whenever the icon or header
     * is changed.
     */
    void changed();

    /**
     * This signal is emitted whenever the user checks or
     * unchecks the item of @see setChecked() is called.
     */
    void toggled( bool checked );

  private:
    class Private;
    Private* const d;
};

/**
 * This page model is used by @see KPageWidget to provide
 * a hierarchical layout of pages.
 */
class KDEUI_EXPORT KPageWidgetModel : public KPageModel
{
  Q_OBJECT

  public:
    /**
     * Creates a new page widget model.
     *
     * @param parent The parent object.
     */
    explicit KPageWidgetModel( QObject *parent = 0 );

    /**
     * Destroys the page widget model.
     */
    ~KPageWidgetModel();

    /**
     * Adds a new top level page to the model.
     *
     * @param widget The widget of the page.
     * @param name The name which is displayed in the navigation view.
     *
     * @returns The associated @see KPageWidgetItem.
     */
    KPageWidgetItem* addPage( QWidget *widget, const QString &name );

    /**
     * Adds a new top level page to the model.
     *
     * @param item The @see KPageWidgetItem which describes the page.
     */
    void addPage( KPageWidgetItem *item );

    /**
     * Inserts a new page in the model.
     *
     * @param before The new page will be insert before this @see KPageWidgetItem
     *               on the same level in hierarchy.
     * @param widget The widget of the page.
     * @param name The name which is displayed in the navigation view.
     *
     * @returns The associated @see KPageWidgetItem.
     */
    KPageWidgetItem* insertPage( KPageWidgetItem *before, QWidget *widget, const QString &name );

    /**
     * Inserts a new page in the model.
     *
     * @param before The new page will be insert before this @see KPageWidgetItem
     *               on the same level in hierarchy.
     *
     * @param item The @see KPageWidgetItem which describes the page.
     */
    void insertPage( KPageWidgetItem *before, KPageWidgetItem *item );

    /**
     * Inserts a new sub page in the model.
     *
     * @param parent The new page will be insert as child of this @see KPageWidgetItem.
     * @param widget The widget of the page.
     * @param name The name which is displayed in the navigation view.
     *
     * @returns The associated @see KPageWidgetItem.
     */
    KPageWidgetItem* addSubPage( KPageWidgetItem *parent, QWidget *widget, const QString &name );

    /**
     * Inserts a new sub page in the model.
     *
     * @param parent The new page will be insert as child of this @see KPageWidgetItem.
     *
     * @param item The @see KPageWidgetItem which describes the page.
     */
    void addSubPage( KPageWidgetItem *parent, KPageWidgetItem *item );

    /**
     * Removes the page associated with the given @see KPageWidgetItem.
     */
    void removePage( KPageWidgetItem *item );

    /**
     * These methods are reimplemented from QAsbtractItemModel.
     */
    virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    virtual bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole );
    virtual Qt::ItemFlags flags( const QModelIndex &index ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
    virtual QModelIndex parent( const QModelIndex &index ) const;
    virtual int rowCount( const QModelIndex &parent = QModelIndex() ) const;

    /**
     * Returns the @see KPageWidgetItem for a given index or 0 if the index is invalid.
     */
    KPageWidgetItem* item( const QModelIndex &index );

    /**
     * Returns the index for a given @see KPageWidgetItem. The index is invalid if the
     * item can't be found in the model.
     */
    QModelIndex index( const KPageWidgetItem *item ) const;

  Q_SIGNALS:
    /**
     * This signal is emitted whenever a checkable page changes its state. @param checked is true
     * when the @param page is checked, or false if the @param page is unchecked.
     */
    void toggled( KPageWidgetItem *page, bool checked );

  private:
    class Private;
    Private* const d;

    Q_PRIVATE_SLOT( d, void itemChanged() )
    Q_PRIVATE_SLOT( d, void itemToggled( bool ) )
};

#endif