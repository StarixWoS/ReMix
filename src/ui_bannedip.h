/********************************************************************************
** Form generated from reading UI file 'bannedip.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BANNEDIP_H
#define UI_BANNEDIP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>

QT_BEGIN_NAMESPACE

class Ui_BannedIP
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QTableView *banTable;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QCheckBox *allowDupedIP;
    QCheckBox *banDupedIP;
    QCheckBox *banHackers;
    QCheckBox *reqSernums;
    QCheckBox *disconnectIdles;
    QCheckBox *allowSSV;
    QPushButton *removeIPBan;
    QPushButton *addIPBan;
    QLabel *label;
    QLineEdit *banIP;
    QLabel *label_2;
    QLineEdit *banReason;

    void setupUi(QDialog *BannedIP)
    {
        if (BannedIP->objectName().isEmpty())
            BannedIP->setObjectName(QStringLiteral("BannedIP"));
        BannedIP->resize(698, 313);
        gridLayout = new QGridLayout(BannedIP);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        frame = new QFrame(BannedIP);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        banTable = new QTableView(frame);
        banTable->setObjectName(QStringLiteral("banTable"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(banTable->sizePolicy().hasHeightForWidth());
        banTable->setSizePolicy(sizePolicy);
        banTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        banTable->setAlternatingRowColors(true);
        banTable->setSelectionMode(QAbstractItemView::SingleSelection);
        banTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        banTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        banTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        banTable->setSortingEnabled(true);
        banTable->setCornerButtonEnabled(false);
        banTable->horizontalHeader()->setDefaultSectionSize(100);
        banTable->horizontalHeader()->setMinimumSectionSize(50);
        banTable->horizontalHeader()->setStretchLastSection(true);
        banTable->verticalHeader()->setVisible(false);
        banTable->verticalHeader()->setMinimumSectionSize(30);

        gridLayout_2->addWidget(banTable, 0, 0, 2, 1);

        groupBox = new QGroupBox(frame);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        allowDupedIP = new QCheckBox(groupBox);
        allowDupedIP->setObjectName(QStringLiteral("allowDupedIP"));

        gridLayout_3->addWidget(allowDupedIP, 0, 0, 1, 3);

        banDupedIP = new QCheckBox(groupBox);
        banDupedIP->setObjectName(QStringLiteral("banDupedIP"));

        gridLayout_3->addWidget(banDupedIP, 1, 0, 1, 3);

        banHackers = new QCheckBox(groupBox);
        banHackers->setObjectName(QStringLiteral("banHackers"));

        gridLayout_3->addWidget(banHackers, 2, 0, 1, 3);

        reqSernums = new QCheckBox(groupBox);
        reqSernums->setObjectName(QStringLiteral("reqSernums"));

        gridLayout_3->addWidget(reqSernums, 3, 0, 1, 3);

        disconnectIdles = new QCheckBox(groupBox);
        disconnectIdles->setObjectName(QStringLiteral("disconnectIdles"));

        gridLayout_3->addWidget(disconnectIdles, 4, 0, 1, 3);

        allowSSV = new QCheckBox(groupBox);
        allowSSV->setObjectName(QStringLiteral("allowSSV"));

        gridLayout_3->addWidget(allowSSV, 5, 0, 1, 3);

        removeIPBan = new QPushButton(groupBox);
        removeIPBan->setObjectName(QStringLiteral("removeIPBan"));
        removeIPBan->setAutoDefault(false);

        gridLayout_3->addWidget(removeIPBan, 6, 0, 1, 3);

        addIPBan = new QPushButton(groupBox);
        addIPBan->setObjectName(QStringLiteral("addIPBan"));
        addIPBan->setAutoDefault(false);

        gridLayout_3->addWidget(addIPBan, 7, 0, 1, 3);

        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_3->addWidget(label, 8, 0, 1, 1);

        banIP = new QLineEdit(groupBox);
        banIP->setObjectName(QStringLiteral("banIP"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(banIP->sizePolicy().hasHeightForWidth());
        banIP->setSizePolicy(sizePolicy1);
        banIP->setMaximumSize(QSize(133, 16777215));

        gridLayout_3->addWidget(banIP, 8, 1, 1, 2);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_3->addWidget(label_2, 9, 0, 1, 2);

        banReason = new QLineEdit(groupBox);
        banReason->setObjectName(QStringLiteral("banReason"));
        sizePolicy1.setHeightForWidth(banReason->sizePolicy().hasHeightForWidth());
        banReason->setSizePolicy(sizePolicy1);
        banReason->setMaximumSize(QSize(105, 16777215));

        gridLayout_3->addWidget(banReason, 9, 2, 1, 1);


        gridLayout_2->addWidget(groupBox, 0, 1, 2, 2);


        gridLayout->addWidget(frame, 0, 0, 1, 1);


        retranslateUi(BannedIP);

        QMetaObject::connectSlotsByName(BannedIP);
    } // setupUi

    void retranslateUi(QDialog *BannedIP)
    {
        BannedIP->setWindowTitle(QApplication::translate("BannedIP", "Banned IP Addresses:", 0));
        groupBox->setTitle(QApplication::translate("BannedIP", "Ban Options:", 0));
        allowDupedIP->setText(QApplication::translate("BannedIP", "Allow Duplicate IP's", 0));
        banDupedIP->setText(QApplication::translate("BannedIP", "Ban Duplicate IP's", 0));
        banHackers->setText(QApplication::translate("BannedIP", "Auto-Ban on Hack", 0));
        reqSernums->setText(QApplication::translate("BannedIP", "Require Sernums", 0));
        disconnectIdles->setText(QApplication::translate("BannedIP", "Disconnect Idle Players", 0));
        allowSSV->setText(QApplication::translate("BannedIP", "Allow Server Variables", 0));
        removeIPBan->setText(QApplication::translate("BannedIP", "Remove IP From List", 0));
        addIPBan->setText(QApplication::translate("BannedIP", "Add IP to List", 0));
        label->setText(QApplication::translate("BannedIP", "IP:", 0));
        banIP->setText(QApplication::translate("BannedIP", "127.0.0.1", 0));
        label_2->setText(QApplication::translate("BannedIP", "Reason:", 0));
    } // retranslateUi

};

namespace Ui {
    class BannedIP: public Ui_BannedIP {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BANNEDIP_H
