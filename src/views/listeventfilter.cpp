
//Class includes.
#include "views/listeventfilter.hpp"

//Qt Includes.
#include <QSortFilterProxyModel>
#include <QMouseEvent>
#include <QTableView>
#include <QListView>

QHash<ListEventFilter*, QListView*> ListEventFilter::listMap;

ListEventFilter::ListEventFilter(QListView* tbl)
{
    listView = tbl;
}

ListEventFilter::~ListEventFilter()
{
    this->deleteLater();
}

ListEventFilter* ListEventFilter::getInstance(QListView* tbl)
{
    ListEventFilter* instance{ listMap.key( tbl, nullptr ) };
    if ( instance == nullptr )
    {
        instance = new ListEventFilter( tbl );
        if ( instance != nullptr )
            listMap.insert( instance, tbl );
    }
    return instance;
}

void ListEventFilter::deleteInstance(QListView* tbl)
{
    ListEventFilter* instance{ listMap.key( tbl, nullptr ) };
    if ( instance != nullptr )
    {
        listMap.remove( instance );
        instance->disconnect();
        instance->setParent( nullptr );
        instance->deleteLater();
        instance = nullptr;
    }
}

bool ListEventFilter::eventFilter(QObject* obj, QEvent* event)
{
    if ( obj == nullptr || event == nullptr )
        return false;

    if ( listView == nullptr )
        return false;

    QMouseEvent* mouseEvent{ dynamic_cast<QMouseEvent*>( event ) };
    if ( mouseEvent != nullptr )
    {
        if (( mouseEvent->type() == QEvent::MouseButtonPress
           || mouseEvent->type() == QEvent::MouseButtonDblClick )
          && mouseEvent->buttons() == Qt::LeftButton )
        {
            QModelIndex menuIndex{ listView->indexAt( mouseEvent->pos() ) };
            if ( menuIndex.isValid() )
            {
                if ( prevIndex != menuIndex )
                {
                    if ( prevIndex.row() != menuIndex.row() )
                    {
                        listView->setCurrentIndex( menuIndex );
                        prevIndex = menuIndex;

                        return true;
                    }
                }
            }
            prevIndex = QModelIndex();
            listView->clearSelection();
            listView->setCurrentIndex( prevIndex );

            return true;
        }
    }
    return QObject::eventFilter( obj, event );
}
