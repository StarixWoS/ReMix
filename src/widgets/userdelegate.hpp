
#ifndef USERDELEGATE_HPP
#define USERDELEGATE_HPP

#include "prototypes.hpp"

#include <QItemDelegate>
#include <QComboBox>
#include <QObject>

class UserDelegate : public QItemDelegate
{
    Q_OBJECT

    public:
        UserDelegate(QObject* parent  =0);
        ~UserDelegate();

        void paint(QPainter* painter,
                   const QStyleOptionViewItem& option,
                   const QModelIndex& index) const;

        void setEditorData(QWidget* editor, const QModelIndex& index) const;

        QWidget* createEditor(QWidget* parent,
                              const QStyleOptionViewItem&,
                              const QModelIndex& index) const;
    public slots:
        void setModelData(QWidget* editor,
                          QAbstractItemModel* model,
                          const QModelIndex& index) const;

        void updateEditorGeometry(QWidget* editor,
                                  const QStyleOptionViewItem& option,
                                  const QModelIndex&) const;

};

#endif // USERDELEGATE_HPP
