
//Class includes.
#include "views/tbleventfilter.hpp"

//Qt Includes.
#include <QSortFilterProxyModel>
#include <QMouseEvent>
#include <QTableView>
#include <QListView>

QHash<TblEventFilter*, QTableView*> TblEventFilter::tableMap;

TblEventFilter::TblEventFilter(QTableView* tbl)
{
    tableView = tbl;
}

TblEventFilter::~TblEventFilter()
{
    this->deleteLater();
}

TblEventFilter* TblEventFilter::getInstance(QTableView* tbl)
{
    TblEventFilter* instance{ tableMap.key( tbl, nullptr ) };
    if ( instance == nullptr )
    {
        instance = new TblEventFilter( tbl );
        if ( instance != nullptr )
            tableMap.insert( instance, tbl );
    }
    return instance;
}

void TblEventFilter::deleteInstance(QTableView* tbl)
{
    TblEventFilter* instance{ tableMap.key( tbl, nullptr ) };
    if ( instance != nullptr )
    {
        tableMap.remove( instance );
        instance->disconnect();
        instance->setParent( nullptr );
        instance->deleteLater();
        instance = nullptr;
    }
}

bool TblEventFilter::eventFilter(QObject* obj, QEvent* event)
{
    if ( obj == nullptr || event == nullptr )
        return false;

    if ( tableView == nullptr )
        return false;

    QMouseEvent* mouseEvent{ dynamic_cast<QMouseEvent*>( event ) };
    if ( mouseEvent != nullptr )
    {
        if (( mouseEvent->type() == QEvent::MouseButtonPress
           || mouseEvent->type() == QEvent::MouseButtonDblClick )
          && mouseEvent->buttons() == Qt::LeftButton )
        {
            QModelIndex menuIndex{ tableView->indexAt( mouseEvent->pos() ) };
            if ( menuIndex.isValid() )
            {
                if ( prevIndex != menuIndex )
                {
                    if ( prevIndex.row() != menuIndex.row() )
                    {
                        tableView->setCurrentIndex( menuIndex );
                        prevIndex = menuIndex;

                        return true;
                    }
                }
            }
            prevIndex = QModelIndex();
            tableView->clearSelection();
            tableView->setCurrentIndex( prevIndex );

            return true;
        }
    }
    return QObject::eventFilter( obj, event );
}
