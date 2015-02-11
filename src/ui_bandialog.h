/********************************************************************************
** Form generated from reading UI file 'bandialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BANDIALOG_H
#define UI_BANDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BanDialog
{
public:
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_2;
    QTableView *ipBanTable;
    QLabel *label;
    QLineEdit *trgIPAddr;
    QLabel *label_2;
    QLineEdit *ipBanReason;
    QPushButton *addIPBan;
    QPushButton *removeIPBan;
    QWidget *tab_2;
    QGridLayout *gridLayout_3;
    QTableView *snBanTable;
    QLabel *label_3;
    QLineEdit *trgSerNum;
    QLabel *label_4;
    QPushButton *addSernumBan;
    QLineEdit *snBanReason;
    QPushButton *forgiveButton;

    void setupUi(QDialog *BanDialog)
    {
        if (BanDialog->objectName().isEmpty())
            BanDialog->setObjectName(QStringLiteral("BanDialog"));
        BanDialog->resize(479, 267);
        gridLayout = new QGridLayout(BanDialog);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(10, 10, 10, 10);
        tabWidget = new QTabWidget(BanDialog);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout_2 = new QGridLayout(tab);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        ipBanTable = new QTableView(tab);
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

        gridLayout_2->addWidget(ipBanTable, 0, 0, 1, 4);

        label = new QLabel(tab);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(label, 1, 0, 1, 1);

        trgIPAddr = new QLineEdit(tab);
        trgIPAddr->setObjectName(QStringLiteral("trgIPAddr"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(trgIPAddr->sizePolicy().hasHeightForWidth());
        trgIPAddr->setSizePolicy(sizePolicy2);

        gridLayout_2->addWidget(trgIPAddr, 1, 1, 1, 1);

        label_2 = new QLabel(tab);
        label_2->setObjectName(QStringLiteral("label_2"));
        sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(label_2, 1, 2, 1, 1);

        ipBanReason = new QLineEdit(tab);
        ipBanReason->setObjectName(QStringLiteral("ipBanReason"));
        sizePolicy2.setHeightForWidth(ipBanReason->sizePolicy().hasHeightForWidth());
        ipBanReason->setSizePolicy(sizePolicy2);

        gridLayout_2->addWidget(ipBanReason, 1, 3, 1, 1);

        addIPBan = new QPushButton(tab);
        addIPBan->setObjectName(QStringLiteral("addIPBan"));
        sizePolicy2.setHeightForWidth(addIPBan->sizePolicy().hasHeightForWidth());
        addIPBan->setSizePolicy(sizePolicy2);
        addIPBan->setAutoDefault(false);

        gridLayout_2->addWidget(addIPBan, 2, 0, 1, 2);

        removeIPBan = new QPushButton(tab);
        removeIPBan->setObjectName(QStringLiteral("removeIPBan"));
        removeIPBan->setEnabled(false);
        sizePolicy2.setHeightForWidth(removeIPBan->sizePolicy().hasHeightForWidth());
        removeIPBan->setSizePolicy(sizePolicy2);
        removeIPBan->setAutoDefault(false);

        gridLayout_2->addWidget(removeIPBan, 2, 2, 1, 2);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        gridLayout_3 = new QGridLayout(tab_2);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        snBanTable = new QTableView(tab_2);
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

        gridLayout_3->addWidget(snBanTable, 0, 0, 1, 5);

        label_3 = new QLabel(tab_2);
        label_3->setObjectName(QStringLiteral("label_3"));
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);

        gridLayout_3->addWidget(label_3, 1, 0, 1, 1);

        trgSerNum = new QLineEdit(tab_2);
        trgSerNum->setObjectName(QStringLiteral("trgSerNum"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(trgSerNum->sizePolicy().hasHeightForWidth());
        trgSerNum->setSizePolicy(sizePolicy3);

        gridLayout_3->addWidget(trgSerNum, 1, 1, 1, 1);

        label_4 = new QLabel(tab_2);
        label_4->setObjectName(QStringLiteral("label_4"));
        sizePolicy1.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy1);

        gridLayout_3->addWidget(label_4, 1, 2, 1, 1);

        addSernumBan = new QPushButton(tab_2);
        addSernumBan->setObjectName(QStringLiteral("addSernumBan"));
        addSernumBan->setAutoDefault(false);

        gridLayout_3->addWidget(addSernumBan, 2, 0, 1, 2);

        snBanReason = new QLineEdit(tab_2);
        snBanReason->setObjectName(QStringLiteral("snBanReason"));

        gridLayout_3->addWidget(snBanReason, 1, 4, 1, 1);

        forgiveButton = new QPushButton(tab_2);
        forgiveButton->setObjectName(QStringLiteral("forgiveButton"));
        forgiveButton->setEnabled(false);
        forgiveButton->setAutoDefault(false);

        gridLayout_3->addWidget(forgiveButton, 2, 2, 1, 3);

        tabWidget->addTab(tab_2, QString());

        gridLayout->addWidget(tabWidget, 0, 0, 1, 1);


        retranslateUi(BanDialog);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(BanDialog);
    } // setupUi

    void retranslateUi(QDialog *BanDialog)
    {
        BanDialog->setWindowTitle(QApplication::translate("BanDialog", "Ban Information:", 0));
        label->setText(QApplication::translate("BanDialog", "IP:", 0));
        trgIPAddr->setText(QApplication::translate("BanDialog", "127.0.0.1", 0));
        label_2->setText(QApplication::translate("BanDialog", "Reason:", 0));
        addIPBan->setText(QApplication::translate("BanDialog", "Add IP to List", 0));
        removeIPBan->setText(QApplication::translate("BanDialog", "Forgive Selected IP", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("BanDialog", "IP Bans", 0));
        label_3->setText(QApplication::translate("BanDialog", "Sernum:", 0));
        label_4->setText(QApplication::translate("BanDialog", "Reason:", 0));
        addSernumBan->setText(QApplication::translate("BanDialog", "Add Sernum to List", 0));
        forgiveButton->setText(QApplication::translate("BanDialog", "Forgive Selected Sernum", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("BanDialog", "SerNum Bans", 0));
    } // retranslateUi

};

namespace Ui {
    class BanDialog: public Ui_BanDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BANDIALOG_H
