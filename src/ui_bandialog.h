/********************************************************************************
** Form generated from reading UI file 'bandialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
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
#include <QtWidgets/QSpacerItem>
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
    QTableView *banTable;
    QLabel *label;
    QLineEdit *banIP;
    QLabel *label_2;
    QLineEdit *banReason;
    QPushButton *removeIPBan;
    QPushButton *addIPBan;
    QWidget *tab_2;
    QGridLayout *gridLayout_4;
    QTableView *sernumTable;
    QSpacerItem *horizontalSpacer;
    QPushButton *forgiveButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *BanDialog)
    {
        if (BanDialog->objectName().isEmpty())
            BanDialog->setObjectName(QStringLiteral("BanDialog"));
        BanDialog->resize(515, 277);
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
        banTable = new QTableView(tab);
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
        banTable->verticalHeader()->setDefaultSectionSize(20);
        banTable->verticalHeader()->setMinimumSectionSize(20);

        gridLayout_2->addWidget(banTable, 0, 0, 1, 4);

        label = new QLabel(tab);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(label, 1, 0, 1, 1);

        banIP = new QLineEdit(tab);
        banIP->setObjectName(QStringLiteral("banIP"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(banIP->sizePolicy().hasHeightForWidth());
        banIP->setSizePolicy(sizePolicy2);

        gridLayout_2->addWidget(banIP, 1, 1, 1, 1);

        label_2 = new QLabel(tab);
        label_2->setObjectName(QStringLiteral("label_2"));
        sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(label_2, 1, 2, 1, 1);

        banReason = new QLineEdit(tab);
        banReason->setObjectName(QStringLiteral("banReason"));
        sizePolicy2.setHeightForWidth(banReason->sizePolicy().hasHeightForWidth());
        banReason->setSizePolicy(sizePolicy2);

        gridLayout_2->addWidget(banReason, 1, 3, 1, 1);

        removeIPBan = new QPushButton(tab);
        removeIPBan->setObjectName(QStringLiteral("removeIPBan"));
        sizePolicy2.setHeightForWidth(removeIPBan->sizePolicy().hasHeightForWidth());
        removeIPBan->setSizePolicy(sizePolicy2);
        removeIPBan->setAutoDefault(false);

        gridLayout_2->addWidget(removeIPBan, 2, 0, 1, 2);

        addIPBan = new QPushButton(tab);
        addIPBan->setObjectName(QStringLiteral("addIPBan"));
        sizePolicy2.setHeightForWidth(addIPBan->sizePolicy().hasHeightForWidth());
        addIPBan->setSizePolicy(sizePolicy2);
        addIPBan->setAutoDefault(false);

        gridLayout_2->addWidget(addIPBan, 2, 2, 1, 2);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        gridLayout_4 = new QGridLayout(tab_2);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        sernumTable = new QTableView(tab_2);
        sernumTable->setObjectName(QStringLiteral("sernumTable"));
        sernumTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        sernumTable->setSelectionMode(QAbstractItemView::SingleSelection);
        sernumTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        sernumTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        sernumTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        sernumTable->setSortingEnabled(true);
        sernumTable->horizontalHeader()->setMinimumSectionSize(50);
        sernumTable->horizontalHeader()->setStretchLastSection(true);
        sernumTable->verticalHeader()->setVisible(false);
        sernumTable->verticalHeader()->setDefaultSectionSize(20);
        sernumTable->verticalHeader()->setMinimumSectionSize(20);

        gridLayout_4->addWidget(sernumTable, 0, 0, 1, 4);

        horizontalSpacer = new QSpacerItem(200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer, 1, 1, 1, 1);

        forgiveButton = new QPushButton(tab_2);
        forgiveButton->setObjectName(QStringLiteral("forgiveButton"));
        forgiveButton->setEnabled(false);
        forgiveButton->setAutoDefault(false);

        gridLayout_4->addWidget(forgiveButton, 1, 2, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_2, 1, 3, 1, 1);

        tabWidget->addTab(tab_2, QString());

        gridLayout->addWidget(tabWidget, 0, 0, 1, 1);


        retranslateUi(BanDialog);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(BanDialog);
    } // setupUi

    void retranslateUi(QDialog *BanDialog)
    {
        BanDialog->setWindowTitle(QApplication::translate("BanDialog", "Ban Information:", 0));
        label->setText(QApplication::translate("BanDialog", "IP:", 0));
        banIP->setText(QApplication::translate("BanDialog", "127.0.0.1", 0));
        label_2->setText(QApplication::translate("BanDialog", "Reason:", 0));
        removeIPBan->setText(QApplication::translate("BanDialog", "Remove IP From List", 0));
        addIPBan->setText(QApplication::translate("BanDialog", "Add IP to List", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("BanDialog", "IP Bans", 0));
        forgiveButton->setText(QApplication::translate("BanDialog", "Forgive Selected Sernum", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("BanDialog", "SerNum Bans", 0));
    } // retranslateUi

};

namespace Ui {
    class BanDialog: public Ui_BanDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BANDIALOG_H
