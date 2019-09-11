
//Class includes.
#include "views/tbleventfilter.hpp"

//Qt Includes.
#include <QSortFilterProxyModel>
#include <QMouseEvent>
#include <QTableView>

TblEventFilter::TblEventFilter(QTableView* tbl,
                               QSortFilterProxyModel* tblProxy)
{
    tableProxy = tblProxy;
    tableView = tbl;
}

TblEventFilter::~TblEventFilter()
{
    this->deleteLater();
}

bool TblEventFilter::eventFilter(QObject* obj, QEvent* event)
{
    if ( obj == nullptr || event == nullptr  )
        return false;

    if ( tableView == nullptr
      || tableProxy == nullptr )
    {
        return false;
    }

    auto* mouseEvent = dynamic_cast<QMouseEvent*>( event );
    if ( mouseEvent != nullptr )
    {
        if (( mouseEvent->type() == QEvent::MouseButtonPress
           || mouseEvent->type() == QEvent::MouseButtonDblClick )
          && mouseEvent->buttons() == Qt::LeftButton )
        {
            QModelIndex menuIndex = tableView->indexAt(
                                        mouseEvent->pos() );
            if ( menuIndex.isValid() )
            {
                if ( prevIndex != menuIndex )
                {
                    if ( prevIndex.row() != menuIndex.row() )
                    {
                        tableView->selectRow( menuIndex.row() );
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
