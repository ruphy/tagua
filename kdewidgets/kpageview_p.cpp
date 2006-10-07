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

#include <QApplication>
#include <QHeaderView>
#include <QPainter>
#include <QVBoxLayout>

#include "kpageview_p.h"

using namespace KDEPrivate;

/**
 * KPagePlainView
 */

KPagePlainView::KPagePlainView( QWidget *parent )
 : QAbstractItemView( parent )
{
  hide();
}

QModelIndex KPagePlainView::indexAt( const QPoint& ) const
{
  return QModelIndex();
}

void KPagePlainView::scrollTo( const QModelIndex&, ScrollHint )
{
}

QRect KPagePlainView::visualRect( const QModelIndex& ) const
{
  return QRect();
}

QModelIndex KPagePlainView::moveCursor(QAbstractItemView::CursorAction, Qt::KeyboardModifiers)
{
  return QModelIndex();
}

int KPagePlainView::horizontalOffset() const
{
  return 0;
}

int KPagePlainView::verticalOffset() const
{
  return 0;
}

bool KPagePlainView::isIndexHidden( const QModelIndex& ) const
{
  return false;
}

void KPagePlainView::setSelection( const QRect&, QFlags<QItemSelectionModel::SelectionFlag> )
{
}

QRegion KPagePlainView::visualRegionForSelection( const QItemSelection& ) const
{
  return QRegion();
}


/**
 * KPageListView
 */

KPageListView::KPageListView( QWidget *parent )
 : QListView( parent )
{
  setViewMode( QListView::ListMode );
  setMovement( QListView::Static );

  QFont boldFont( font() );
  boldFont.setBold( true );
  setFont( boldFont );

  setItemDelegate( new KPageListViewDelegate( this ) );
}

KPageListView::~KPageListView()
{
}

void KPageListView::setModel( QAbstractItemModel *model )
{
/*
  KPageListViewProxy *proxy = new KPageListViewProxy( this );
  proxy->setSourceModel( model );
  proxy->rebuildMap();

  connect( model, SIGNAL( layoutChanged() ), proxy, SLOT( rebuildMap() ) );
*/
  connect( model, SIGNAL( layoutChanged() ), this, SLOT( updateWidth() ) );

//  QListView::setModel( proxy );
  QListView::setModel( model );

  updateWidth();
}

void KPageListView::updateWidth()
{
  if ( !model() )
    return;

  int rows = model()->rowCount();

  int width = 0;
  for ( int i = 0; i < rows; ++i )
    width = qMax( width, sizeHintForIndex( model()->index( i, 0 ) ).width() );

  setFixedWidth( width + 20 );
}


/**
 * KPageTreeView
 */

KPageTreeView::KPageTreeView( QWidget *parent )
 : QTreeView( parent )
{
  header()->hide();
}

void KPageTreeView::setModel( QAbstractItemModel *model )
{
  connect( model, SIGNAL( layoutChanged() ), this, SLOT( updateWidth() ) );

  QTreeView::setModel( model );

  updateWidth();
}

void KPageTreeView::updateWidth()
{
  if ( !model() )
    return;

  int columns = model()->columnCount();

  int width = 0;
  for ( int i = 0; i < columns; ++i ) {
    resizeColumnToContents( i );
    width = qMax( width, sizeHintForColumn( i ) );
  }

  setFixedWidth( width + 20 );

  expandItems();
}

void KPageTreeView::expandItems( const QModelIndex &index )
{
  setExpanded( index, true );

  const int count = model()->rowCount( index );
  for ( int i = 0; i < count; ++i )
    expandItems( model()->index( i, 0, index ) );
}


/**
 * KPageTabbedView
 */

KPageTabbedView::KPageTabbedView( QWidget *parent )
  : QAbstractItemView( parent )
{
  // hide the viewport of the QAbstractScrollArea
  QList<QWidget*> list = findChildren<QWidget*>();
  for ( int i = 0; i < list.count(); ++i )
    list[ i ]->hide();

  setFrameShape( NoFrame );

  QGridLayout *layout = new QGridLayout( this );
  layout->setMargin( 0 );
  layout->setSpacing( 0 );

  mTabBar = new QTabBar( this );
  connect( mTabBar, SIGNAL( currentChanged( int ) ), this, SLOT( currentPageChanged( int ) ) );

  layout->addWidget( mTabBar, 0, 0, Qt::AlignBottom );
  layout->setColumnStretch( 1, 1 );
}

void KPageTabbedView::setModel( QAbstractItemModel *model )
{
  QAbstractItemView::setModel( model );

  connect( model, SIGNAL( layoutChanged() ), this, SLOT( layoutChanged() ) );

  layoutChanged();
}

QModelIndex KPageTabbedView::indexAt( const QPoint& ) const
{
  if ( model() )
    return model()->index( 0, 0 );
  else
    return QModelIndex();
}

void KPageTabbedView::scrollTo( const QModelIndex &index, ScrollHint )
{
  if ( !index.isValid() )
    return;

  mTabBar->setCurrentIndex( index.row() );
}

QRect KPageTabbedView::visualRect( const QModelIndex& ) const
{
  return QRect();
}

QModelIndex KPageTabbedView::moveCursor(QAbstractItemView::CursorAction, Qt::KeyboardModifiers)
{
  return QModelIndex();
}

int KPageTabbedView::horizontalOffset() const
{
  return 0;
}

int KPageTabbedView::verticalOffset() const
{
  return 0;
}

bool KPageTabbedView::isIndexHidden( const QModelIndex &index ) const
{
  return ( mTabBar->currentIndex() != index.row() );
}

void KPageTabbedView::setSelection( const QRect&, QFlags<QItemSelectionModel::SelectionFlag> )
{
}

QRegion KPageTabbedView::visualRegionForSelection( const QItemSelection& ) const
{
  return QRegion();
}

void KPageTabbedView::currentPageChanged( int index )
{
  if ( !model() )
    return;

  QModelIndex modelIndex = model()->index( index, 0 );

  selectionModel()->setCurrentIndex( modelIndex, QItemSelectionModel::Select );
}

void KPageTabbedView::layoutChanged()
{
  // save old position
  int pos = mTabBar->currentIndex();

  // clear tab bar
  int count = mTabBar->count();
  for ( int i = 0; i < count; ++i ) {
    mTabBar->removeTab( 0 );
  }

  if ( !model() )
    return;

  // add new tabs
  for ( int i = 0; i < model()->rowCount(); ++i ) {
    const QString title = model()->data( model()->index( i, 0 ) ).toString();
    const QIcon icon = model()->data( model()->index( i, 0 ), Qt::DecorationRole ).value<QIcon>();
    mTabBar->addTab( icon, title );
  }

  mTabBar->setCurrentIndex( pos );

  setFixedHeight( mTabBar->minimumSizeHint().height() );
}

/**
 * KPageListViewDelegate
 */

KPageListViewDelegate::KPageListViewDelegate( QObject *parent )
 : QAbstractItemDelegate( parent )
{
}

void KPageListViewDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
  if ( !index.isValid() )
    return;

  painter->setRenderHint( QPainter::Antialiasing );

  const QString text = index.model()->data( index, Qt::DisplayRole ).toString();
  const QIcon icon = index.model()->data( index, Qt::DecorationRole ).value<QIcon>();
  const QPixmap pixmap = icon.pixmap( 32, 32 );

  QFontMetrics fm = painter->fontMetrics();
  int ht = fm.boundingRect( 0, 0, 0, 0, Qt::AlignCenter, text ).height();
  int wt = fm.boundingRect( 0, 0, 0, 0, Qt::AlignCenter, text ).width();
  int wp = pixmap.width();
  int hp = pixmap.height();

  QPen pen = painter->pen();
  QPalette::ColorGroup cg = option.state & QStyle::State_Enabled
                            ? QPalette::Normal : QPalette::Disabled;
  if (cg == QPalette::Normal && !(option.state & QStyle::State_Active))
    cg = QPalette::Inactive;
  if (option.state & QStyle::State_Selected) {
    painter->fillRect(option.rect, option.palette.brush(cg, QPalette::Highlight));
    painter->setPen(option.palette.color(cg, QPalette::HighlightedText));
  } else {
    painter->setPen(option.palette.color(cg, QPalette::Text));
  }

  QFont font = painter->font();
  painter->setFont(option.font);

  painter->drawPixmap( option.rect.x() + (option.rect.width()/2)-(wp/2), option.rect.y() + 5, pixmap );
  if ( !text.isEmpty() )
    painter->drawText( option.rect.x() + (option.rect.width()/2)-(wt/2), option.rect.y() + hp+7, wt, ht, Qt::AlignCenter, text );

  painter->setFont(font);
  painter->setPen(pen);

  drawFocus( painter, option, option.rect );
}

QSize KPageListViewDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
  if ( !index.isValid() )
    return QSize( 0, 0 );

  const QString text = index.model()->data( index, Qt::DisplayRole ).toString();
  const QIcon icon = index.model()->data( index, Qt::DecorationRole ).value<QIcon>();
  const QPixmap pixmap = icon.pixmap( 32, 32 );

  QFontMetrics fm = option.fontMetrics;
  int ht = fm.boundingRect( 0, 0, 0, 0, Qt::AlignCenter, text ).height();
  int wt = fm.boundingRect( 0, 0, 0, 0, Qt::AlignCenter, text ).width() + 10;
  int wp = pixmap.width() + 10;

  int width, height;
  if ( text.isEmpty() )
    height = pixmap.height();
  else
    height = pixmap.height() + ht + 10;

  width = qMax( wt, wp );

  return QSize( width, height );
}

void KPageListViewDelegate::drawFocus( QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect ) const
{
  if (option.state & QStyle::State_HasFocus) {
    QStyleOptionFocusRect o;
    o.QStyleOption::operator=(option);
    o.rect = rect;
    o.state |= QStyle::State_KeyboardFocusChange;
    QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled)
                              ? QPalette::Normal : QPalette::Disabled;
    o.backgroundColor = option.palette.color(cg, (option.state & QStyle::State_Selected)
                                             ? QPalette::Highlight : QPalette::Background);
    QApplication::style()->drawPrimitive(QStyle::PE_FrameFocusRect, &o, painter);
  }
}


/**
 * KPageListViewProxy
 */

KPageListViewProxy::KPageListViewProxy( QObject *parent )
 : QAbstractProxyModel( parent )
{
}

KPageListViewProxy::~KPageListViewProxy()
{
}

int KPageListViewProxy::rowCount( const QModelIndex& ) const
{
  return mList.count();
}

int KPageListViewProxy::columnCount( const QModelIndex& ) const
{
  return 1;
}

QModelIndex KPageListViewProxy::index( int row, int column, const QModelIndex& ) const
{
  if ( column > 1 || row >= mList.count() )
    return QModelIndex();
  else
    return createIndex( row, column, mList[ row ].internalPointer() );
}

QModelIndex KPageListViewProxy::parent( const QModelIndex& ) const
{
  return QModelIndex();
}

QVariant KPageListViewProxy::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

  if ( index.row() >= mList.count() )
    return QVariant();

  return sourceModel()->data( mList[ index.row() ], role );
}

QModelIndex KPageListViewProxy::mapFromSource( const QModelIndex &index ) const
{
  if ( !index.isValid() )
    return QModelIndex();

  for ( int i = 0; i < mList.count(); ++i ) {
    if ( mList[ i ] == index )
      return createIndex( i, 0, index.internalPointer() );
  }

  return QModelIndex();
}

QModelIndex KPageListViewProxy::mapToSource( const QModelIndex &index ) const
{
  if ( !index.isValid() )
    return QModelIndex();

  return mList[ index.row() ];
}

void KPageListViewProxy::rebuildMap()
{
  mList.clear();

  const QAbstractItemModel *model = sourceModel();
  if ( !model )
    return;

  for ( int i = 0; i < model->rowCount(); ++i ) {
    addMapEntry( model->index( i, 0 ) );
  }

  for ( int i = 0; i < mList.count(); ++i )
    qDebug( "%d:0 -> %d:%d", i, mList[ i ].row(), mList[ i ].column() );

  emit layoutChanged();
}

void KPageListViewProxy::addMapEntry( const QModelIndex &index )
{
  if ( sourceModel()->rowCount( index ) == 0 ) {
    mList.append( index );
  } else {
    const int count = sourceModel()->rowCount( index );
    for ( int i = 0; i < count; ++i )
      addMapEntry( sourceModel()->index( i, 0, index ) );
  }
}

#include "kpageview_p.moc"