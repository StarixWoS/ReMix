/********************************************************************************
** Form generated from reading UI file 'settings.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_Settings
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QTableWidget *settingsView;

    void setupUi(QDialog *Settings)
    {
        if (Settings->objectName().isEmpty())
            Settings->setObjectName(QStringLiteral("Settings"));
        Settings->resize(294, 344);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Settings->sizePolicy().hasHeightForWidth());
        Settings->setSizePolicy(sizePolicy);
        Settings->setMinimumSize(QSize(294, 344));
        Settings->setMaximumSize(QSize(294, 344));
        gridLayout = new QGridLayout(Settings);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        frame = new QFrame(Settings);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        settingsView = new QTableWidget(frame);
        if (settingsView->columnCount() < 1)
            settingsView->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        settingsView->setHorizontalHeaderItem(0, __qtablewidgetitem);
        if (settingsView->rowCount() < 9)
            settingsView->setRowCount(9);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        settingsView->setVerticalHeaderItem(0, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        settingsView->setVerticalHeaderItem(1, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        settingsView->setVerticalHeaderItem(2, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        settingsView->setVerticalHeaderItem(3, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        settingsView->setVerticalHeaderItem(4, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        settingsView->setVerticalHeaderItem(5, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        settingsView->setVerticalHeaderItem(6, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        settingsView->setVerticalHeaderItem(7, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        settingsView->setVerticalHeaderItem(8, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        __qtablewidgetitem10->setCheckState(Qt::Checked);
        settingsView->setItem(0, 0, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        __qtablewidgetitem11->setCheckState(Qt::Checked);
        settingsView->setItem(1, 0, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        __qtablewidgetitem12->setCheckState(Qt::Checked);
        settingsView->setItem(2, 0, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        __qtablewidgetitem13->setCheckState(Qt::Unchecked);
        __qtablewidgetitem13->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsTristate);
        settingsView->setItem(3, 0, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        __qtablewidgetitem14->setCheckState(Qt::Checked);
        settingsView->setItem(4, 0, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        __qtablewidgetitem15->setCheckState(Qt::Checked);
        settingsView->setItem(5, 0, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        __qtablewidgetitem16->setCheckState(Qt::Unchecked);
        settingsView->setItem(6, 0, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        __qtablewidgetitem17->setCheckState(Qt::Unchecked);
        settingsView->setItem(7, 0, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        __qtablewidgetitem18->setCheckState(Qt::Unchecked);
        settingsView->setItem(8, 0, __qtablewidgetitem18);
        settingsView->setObjectName(QStringLiteral("settingsView"));
        settingsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        settingsView->setAlternatingRowColors(true);
        settingsView->setSelectionMode(QAbstractItemView::SingleSelection);
        settingsView->setSelectionBehavior(QAbstractItemView::SelectRows);
        settingsView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        settingsView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        settingsView->setCornerButtonEnabled(false);
        settingsView->horizontalHeader()->setStretchLastSection(true);
        settingsView->verticalHeader()->setVisible(false);
        settingsView->verticalHeader()->setDefaultSectionSize(20);
        settingsView->verticalHeader()->setMinimumSectionSize(20);

        gridLayout_2->addWidget(settingsView, 0, 0, 1, 1);


        gridLayout->addWidget(frame, 0, 0, 1, 1);


        retranslateUi(Settings);

        QMetaObject::connectSlotsByName(Settings);
    } // setupUi

    void retranslateUi(QDialog *Settings)
    {
        Settings->setWindowTitle(QApplication::translate("Settings", "Server Settings:", 0));
        QTableWidgetItem *___qtablewidgetitem = settingsView->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("Settings", "Options: ( Double-click to enable or disable )", 0));
        QTableWidgetItem *___qtablewidgetitem1 = settingsView->verticalHeaderItem(0);
        ___qtablewidgetitem1->setText(QApplication::translate("Settings", "0", 0));
        QTableWidgetItem *___qtablewidgetitem2 = settingsView->verticalHeaderItem(1);
        ___qtablewidgetitem2->setText(QApplication::translate("Settings", "1", 0));
        QTableWidgetItem *___qtablewidgetitem3 = settingsView->verticalHeaderItem(2);
        ___qtablewidgetitem3->setText(QApplication::translate("Settings", "2", 0));
        QTableWidgetItem *___qtablewidgetitem4 = settingsView->verticalHeaderItem(3);
        ___qtablewidgetitem4->setText(QApplication::translate("Settings", "3", 0));
        QTableWidgetItem *___qtablewidgetitem5 = settingsView->verticalHeaderItem(4);
        ___qtablewidgetitem5->setText(QApplication::translate("Settings", "4", 0));
        QTableWidgetItem *___qtablewidgetitem6 = settingsView->verticalHeaderItem(5);
        ___qtablewidgetitem6->setText(QApplication::translate("Settings", "5", 0));
        QTableWidgetItem *___qtablewidgetitem7 = settingsView->verticalHeaderItem(6);
        ___qtablewidgetitem7->setText(QApplication::translate("Settings", "6", 0));
        QTableWidgetItem *___qtablewidgetitem8 = settingsView->verticalHeaderItem(7);
        ___qtablewidgetitem8->setText(QApplication::translate("Settings", "7", 0));
        QTableWidgetItem *___qtablewidgetitem9 = settingsView->verticalHeaderItem(8);
        ___qtablewidgetitem9->setText(QApplication::translate("Settings", "8", 0));

        const bool __sortingEnabled = settingsView->isSortingEnabled();
        settingsView->setSortingEnabled(false);
        QTableWidgetItem *___qtablewidgetitem10 = settingsView->item(0, 0);
        ___qtablewidgetitem10->setText(QApplication::translate("Settings", "Require Password", 0));
        QTableWidgetItem *___qtablewidgetitem11 = settingsView->item(1, 0);
        ___qtablewidgetitem11->setText(QApplication::translate("Settings", "Require Remote-Admin Auth", 0));
        QTableWidgetItem *___qtablewidgetitem12 = settingsView->item(2, 0);
        ___qtablewidgetitem12->setText(QApplication::translate("Settings", "Allow Duplicate IP's", 0));
        QTableWidgetItem *___qtablewidgetitem13 = settingsView->item(3, 0);
        ___qtablewidgetitem13->setText(QApplication::translate("Settings", "Ban Duplicate IP's", 0));
        QTableWidgetItem *___qtablewidgetitem14 = settingsView->item(4, 0);
        ___qtablewidgetitem14->setText(QApplication::translate("Settings", "Auto-Ban on Hack", 0));
        QTableWidgetItem *___qtablewidgetitem15 = settingsView->item(5, 0);
        ___qtablewidgetitem15->setText(QApplication::translate("Settings", "Require Sernums", 0));
        QTableWidgetItem *___qtablewidgetitem16 = settingsView->item(6, 0);
        ___qtablewidgetitem16->setText(QApplication::translate("Settings", "Disconnect Idle Users", 0));
        QTableWidgetItem *___qtablewidgetitem17 = settingsView->item(7, 0);
        ___qtablewidgetitem17->setText(QApplication::translate("Settings", "Allow Server Variables", 0));
        QTableWidgetItem *___qtablewidgetitem18 = settingsView->item(8, 0);
        ___qtablewidgetitem18->setText(QApplication::translate("Settings", "Log Comments to File", 0));
        settingsView->setSortingEnabled(__sortingEnabled);

    } // retranslateUi

};

namespace Ui {
    class Settings: public Ui_Settings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_H
