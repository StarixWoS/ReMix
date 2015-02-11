/********************************************************************************
** Form generated from reading UI file 'dabanwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DABANWIDGET_H
#define UI_DABANWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DABanWidget
{
public:
    QGridLayout *gridLayout;
    QTableView *daBanTable;
    QLabel *label_5;
    QLineEdit *trgDate;
    QLabel *label_6;
    QLineEdit *daBanReason;
    QPushButton *addDateBan;
    QPushButton *removeDABan;

    void setupUi(QWidget *DABanWidget)
    {
        if (DABanWidget->objectName().isEmpty())
            DABanWidget->setObjectName(QStringLiteral("DABanWidget"));
        DABanWidget->resize(507, 251);
        gridLayout = new QGridLayout(DABanWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        daBanTable = new QTableView(DABanWidget);
        daBanTable->setObjectName(QStringLiteral("daBanTable"));
        daBanTable->setAlternatingRowColors(true);
        daBanTable->setSelectionMode(QAbstractItemView::SingleSelection);
        daBanTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        daBanTable->horizontalHeader()->setVisible(false);
        daBanTable->horizontalHeader()->setStretchLastSection(true);
        daBanTable->verticalHeader()->setDefaultSectionSize(20);
        daBanTable->verticalHeader()->setMinimumSectionSize(20);

        gridLayout->addWidget(daBanTable, 0, 0, 1, 4);

        label_5 = new QLabel(DABanWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 1, 0, 1, 1);

        trgDate = new QLineEdit(DABanWidget);
        trgDate->setObjectName(QStringLiteral("trgDate"));

        gridLayout->addWidget(trgDate, 1, 1, 1, 1);

        label_6 = new QLabel(DABanWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 1, 2, 1, 1);

        daBanReason = new QLineEdit(DABanWidget);
        daBanReason->setObjectName(QStringLiteral("daBanReason"));

        gridLayout->addWidget(daBanReason, 1, 3, 1, 1);

        addDateBan = new QPushButton(DABanWidget);
        addDateBan->setObjectName(QStringLiteral("addDateBan"));
        addDateBan->setAutoDefault(false);

        gridLayout->addWidget(addDateBan, 2, 0, 1, 2);

        removeDABan = new QPushButton(DABanWidget);
        removeDABan->setObjectName(QStringLiteral("removeDABan"));
        removeDABan->setAutoDefault(false);

        gridLayout->addWidget(removeDABan, 2, 2, 1, 2);


        retranslateUi(DABanWidget);

        QMetaObject::connectSlotsByName(DABanWidget);
    } // setupUi

    void retranslateUi(QWidget *DABanWidget)
    {
        DABanWidget->setWindowTitle(QApplication::translate("DABanWidget", "Form", 0));
        label_5->setText(QApplication::translate("DABanWidget", "Date:", 0));
        label_6->setText(QApplication::translate("DABanWidget", "Reason:", 0));
        addDateBan->setText(QApplication::translate("DABanWidget", "Add Date to List", 0));
        removeDABan->setText(QApplication::translate("DABanWidget", "Forgive Selected Date", 0));
    } // retranslateUi

};

namespace Ui {
    class DABanWidget: public Ui_DABanWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DABANWIDGET_H
