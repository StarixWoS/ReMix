/********************************************************************************
** Form generated from reading UI file 'ipbanwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IPBANWIDGET_H
#define UI_IPBANWIDGET_H

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

class Ui_IPBanWidget
{
public:
    QGridLayout *gridLayout;
    QTableView *ipBanTable;
    QLabel *label;
    QLineEdit *trgIPAddr;
    QLabel *label_2;
    QLineEdit *ipBanReason;
    QPushButton *addIPBan;
    QPushButton *removeIPBan;

    void setupUi(QWidget *IPBanWidget)
    {
        if (IPBanWidget->objectName().isEmpty())
            IPBanWidget->setObjectName(QStringLiteral("IPBanWidget"));
        IPBanWidget->resize(196, 133);
        gridLayout = new QGridLayout(IPBanWidget);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        ipBanTable = new QTableView(IPBanWidget);
        ipBanTable->setObjectName(QStringLiteral("ipBanTable"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ipBanTable->sizePolicy().hasHeightForWidth());
        ipBanTable->setSizePolicy(sizePolicy);
        ipBanTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ipBanTable->setAlternatingRowColors(true);
        ipBanTable->setSelectionMode(QAbstractItemView::SingleSelection);
        ipBanTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        ipBanTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        ipBanTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        ipBanTable->setSortingEnabled(true);
        ipBanTable->setCornerButtonEnabled(false);
        ipBanTable->horizontalHeader()->setDefaultSectionSize(100);
        ipBanTable->horizontalHeader()->setMinimumSectionSize(50);
        ipBanTable->horizontalHeader()->setStretchLastSection(true);
        ipBanTable->verticalHeader()->setVisible(false);
        ipBanTable->verticalHeader()->setDefaultSectionSize(20);
        ipBanTable->verticalHeader()->setMinimumSectionSize(20);

        gridLayout->addWidget(ipBanTable, 0, 0, 1, 4);

        label = new QLabel(IPBanWidget);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(label, 1, 0, 1, 1);

        trgIPAddr = new QLineEdit(IPBanWidget);
        trgIPAddr->setObjectName(QStringLiteral("trgIPAddr"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(trgIPAddr->sizePolicy().hasHeightForWidth());
        trgIPAddr->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(trgIPAddr, 1, 1, 1, 1);

        label_2 = new QLabel(IPBanWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(label_2, 1, 2, 1, 1);

        ipBanReason = new QLineEdit(IPBanWidget);
        ipBanReason->setObjectName(QStringLiteral("ipBanReason"));
        sizePolicy2.setHeightForWidth(ipBanReason->sizePolicy().hasHeightForWidth());
        ipBanReason->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(ipBanReason, 1, 3, 1, 1);

        addIPBan = new QPushButton(IPBanWidget);
        addIPBan->setObjectName(QStringLiteral("addIPBan"));
        sizePolicy2.setHeightForWidth(addIPBan->sizePolicy().hasHeightForWidth());
        addIPBan->setSizePolicy(sizePolicy2);
        addIPBan->setAutoDefault(false);

        gridLayout->addWidget(addIPBan, 2, 0, 1, 2);

        removeIPBan = new QPushButton(IPBanWidget);
        removeIPBan->setObjectName(QStringLiteral("removeIPBan"));
        removeIPBan->setEnabled(false);
        sizePolicy2.setHeightForWidth(removeIPBan->sizePolicy().hasHeightForWidth());
        removeIPBan->setSizePolicy(sizePolicy2);
        removeIPBan->setAutoDefault(false);

        gridLayout->addWidget(removeIPBan, 2, 2, 1, 2);


        retranslateUi(IPBanWidget);

        QMetaObject::connectSlotsByName(IPBanWidget);
    } // setupUi

    void retranslateUi(QWidget *IPBanWidget)
    {
        IPBanWidget->setWindowTitle(QApplication::translate("IPBanWidget", "Form", 0));
        label->setText(QApplication::translate("IPBanWidget", "IP:", 0));
        trgIPAddr->setText(QApplication::translate("IPBanWidget", "127.0.0.1", 0));
        label_2->setText(QApplication::translate("IPBanWidget", "Reason:", 0));
        addIPBan->setText(QApplication::translate("IPBanWidget", "Add IP to List", 0));
        removeIPBan->setText(QApplication::translate("IPBanWidget", "Forgive Selected IP", 0));
    } // retranslateUi

};

namespace Ui {
    class IPBanWidget: public Ui_IPBanWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IPBANWIDGET_H
