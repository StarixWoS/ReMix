/********************************************************************************
** Form generated from reading UI file 'dvbanwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DVBANWIDGET_H
#define UI_DVBANWIDGET_H

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

class Ui_DVBanWidget
{
public:
    QGridLayout *gridLayout;
    QTableView *dvBanTable;
    QLabel *label_7;
    QLineEdit *trgDevice;
    QLabel *label_8;
    QLineEdit *dvBanReason;
    QPushButton *addDeviceBan;
    QPushButton *removeDVBan;

    void setupUi(QWidget *DVBanWidget)
    {
        if (DVBanWidget->objectName().isEmpty())
            DVBanWidget->setObjectName(QStringLiteral("DVBanWidget"));
        DVBanWidget->resize(454, 235);
        gridLayout = new QGridLayout(DVBanWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        dvBanTable = new QTableView(DVBanWidget);
        dvBanTable->setObjectName(QStringLiteral("dvBanTable"));
        dvBanTable->setAlternatingRowColors(true);
        dvBanTable->setSelectionMode(QAbstractItemView::SingleSelection);
        dvBanTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        dvBanTable->setCornerButtonEnabled(false);
        dvBanTable->horizontalHeader()->setVisible(false);
        dvBanTable->horizontalHeader()->setStretchLastSection(true);
        dvBanTable->verticalHeader()->setDefaultSectionSize(20);
        dvBanTable->verticalHeader()->setMinimumSectionSize(20);

        gridLayout->addWidget(dvBanTable, 0, 0, 1, 4);

        label_7 = new QLabel(DVBanWidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy);

        gridLayout->addWidget(label_7, 1, 0, 1, 1);

        trgDevice = new QLineEdit(DVBanWidget);
        trgDevice->setObjectName(QStringLiteral("trgDevice"));

        gridLayout->addWidget(trgDevice, 1, 1, 1, 1);

        label_8 = new QLabel(DVBanWidget);
        label_8->setObjectName(QStringLiteral("label_8"));
        sizePolicy.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy);

        gridLayout->addWidget(label_8, 1, 2, 1, 1);

        dvBanReason = new QLineEdit(DVBanWidget);
        dvBanReason->setObjectName(QStringLiteral("dvBanReason"));

        gridLayout->addWidget(dvBanReason, 1, 3, 1, 1);

        addDeviceBan = new QPushButton(DVBanWidget);
        addDeviceBan->setObjectName(QStringLiteral("addDeviceBan"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(addDeviceBan->sizePolicy().hasHeightForWidth());
        addDeviceBan->setSizePolicy(sizePolicy1);
        addDeviceBan->setAutoDefault(false);

        gridLayout->addWidget(addDeviceBan, 2, 0, 1, 2);

        removeDVBan = new QPushButton(DVBanWidget);
        removeDVBan->setObjectName(QStringLiteral("removeDVBan"));
        removeDVBan->setAutoDefault(false);

        gridLayout->addWidget(removeDVBan, 2, 2, 1, 2);


        retranslateUi(DVBanWidget);

        QMetaObject::connectSlotsByName(DVBanWidget);
    } // setupUi

    void retranslateUi(QWidget *DVBanWidget)
    {
        DVBanWidget->setWindowTitle(QApplication::translate("DVBanWidget", "Form", 0));
        label_7->setText(QApplication::translate("DVBanWidget", "Device:", 0));
        label_8->setText(QApplication::translate("DVBanWidget", "Reason:", 0));
        addDeviceBan->setText(QApplication::translate("DVBanWidget", "Add Device to List", 0));
        removeDVBan->setText(QApplication::translate("DVBanWidget", "Forgive Selected Device", 0));
    } // retranslateUi

};

namespace Ui {
    class DVBanWidget: public Ui_DVBanWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DVBANWIDGET_H
