/********************************************************************************
** Form generated from reading UI file 'snbanwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SNBANWIDGET_H
#define UI_SNBANWIDGET_H

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

class Ui_SNBanWidget
{
public:
    QGridLayout *gridLayout;
    QTableView *snBanTable;
    QLabel *label_3;
    QLineEdit *trgSerNum;
    QLabel *label_4;
    QLineEdit *snBanReason;
    QPushButton *addSernumBan;
    QPushButton *forgiveButton;

    void setupUi(QWidget *SNBanWidget)
    {
        if (SNBanWidget->objectName().isEmpty())
            SNBanWidget->setObjectName(QStringLiteral("SNBanWidget"));
        SNBanWidget->resize(240, 133);
        gridLayout = new QGridLayout(SNBanWidget);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        snBanTable = new QTableView(SNBanWidget);
        snBanTable->setObjectName(QStringLiteral("snBanTable"));
        snBanTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        snBanTable->setAlternatingRowColors(true);
        snBanTable->setSelectionMode(QAbstractItemView::SingleSelection);
        snBanTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        snBanTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        snBanTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        snBanTable->setSortingEnabled(true);
        snBanTable->setCornerButtonEnabled(false);
        snBanTable->horizontalHeader()->setMinimumSectionSize(50);
        snBanTable->horizontalHeader()->setStretchLastSection(true);
        snBanTable->verticalHeader()->setVisible(false);
        snBanTable->verticalHeader()->setDefaultSectionSize(20);
        snBanTable->verticalHeader()->setMinimumSectionSize(20);

        gridLayout->addWidget(snBanTable, 0, 0, 1, 4);

        label_3 = new QLabel(SNBanWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy);

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        trgSerNum = new QLineEdit(SNBanWidget);
        trgSerNum->setObjectName(QStringLiteral("trgSerNum"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(trgSerNum->sizePolicy().hasHeightForWidth());
        trgSerNum->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(trgSerNum, 1, 1, 1, 1);

        label_4 = new QLabel(SNBanWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        sizePolicy.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy);

        gridLayout->addWidget(label_4, 1, 2, 1, 1);

        snBanReason = new QLineEdit(SNBanWidget);
        snBanReason->setObjectName(QStringLiteral("snBanReason"));

        gridLayout->addWidget(snBanReason, 1, 3, 1, 1);

        addSernumBan = new QPushButton(SNBanWidget);
        addSernumBan->setObjectName(QStringLiteral("addSernumBan"));
        addSernumBan->setAutoDefault(false);

        gridLayout->addWidget(addSernumBan, 2, 0, 1, 2);

        forgiveButton = new QPushButton(SNBanWidget);
        forgiveButton->setObjectName(QStringLiteral("forgiveButton"));
        forgiveButton->setEnabled(false);
        forgiveButton->setAutoDefault(false);

        gridLayout->addWidget(forgiveButton, 2, 2, 1, 2);


        retranslateUi(SNBanWidget);

        QMetaObject::connectSlotsByName(SNBanWidget);
    } // setupUi

    void retranslateUi(QWidget *SNBanWidget)
    {
        SNBanWidget->setWindowTitle(QApplication::translate("SNBanWidget", "Form", 0));
        label_3->setText(QApplication::translate("SNBanWidget", "Sernum:", 0));
        label_4->setText(QApplication::translate("SNBanWidget", "Reason:", 0));
        addSernumBan->setText(QApplication::translate("SNBanWidget", "Add Sernum to List", 0));
        forgiveButton->setText(QApplication::translate("SNBanWidget", "Forgive Selected Sernum", 0));
    } // retranslateUi

};

namespace Ui {
    class SNBanWidget: public Ui_SNBanWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SNBANWIDGET_H
