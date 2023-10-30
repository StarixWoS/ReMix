
#ifndef CMDTABLEDELEGATE_HPP
#define CMDTABLEDELEGATE_HPP

#include "prototypes.hpp"

#include <QItemDelegate>
#include <QComboBox>
#include <QObject>

class CmdTableDelegate : public QItemDelegate
{
    Q_OBJECT

    public:
        CmdTableDelegate(QObject* parent = nullptr);
        ~CmdTableDelegate() override;

        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        void setEditorData(QWidget* editor, const QModelIndex& index) const override;
        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const override;

    public slots:
        void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
        void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&) const override;

};

#endif // CMDTABLEDELEGATE_HPP
