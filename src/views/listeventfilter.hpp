
#ifndef LISTEVENTFILTER_HPP
#define LISTEVENTFILTER_HPP

#include "prototypes.hpp"

#include <QModelIndex>
#include <QListView>
#include <QObject>
#include <QHash>

class ListEventFilter : public QObject
{
    Q_OBJECT

    static QHash<ListEventFilter*, QListView*> listMap;

    QListView* listView{ nullptr };
    QModelIndex prevIndex;

    public:
        explicit ListEventFilter(QListView* tbl = nullptr);
        ~ListEventFilter() override;

    static ListEventFilter* getInstance(QListView* tbl);
    static void deleteInstance(QListView* tbl);

    public slots:
        bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // LISTEVENTFILTER_HPP
