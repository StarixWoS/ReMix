/********************************************************************************
** Form generated from reading UI file 'settingswidget.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSWIDGET_H
#define UI_SETTINGSWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsWidget
{
public:
    QGridLayout *gridLayout;
    QTableWidget *settingsView;

    void setupUi(QWidget *SettingsWidget)
    {
        if (SettingsWidget->objectName().isEmpty())
            SettingsWidget->setObjectName(QStringLiteral("SettingsWidget"));
        SettingsWidget->resize(80, 80);
        gridLayout = new QGridLayout(SettingsWidget);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        settingsView = new QTableWidget(SettingsWidget);
        if (settingsView->columnCount() < 1)
            settingsView->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        settingsView->setHorizontalHeaderItem(0, __qtablewidgetitem);
        if (settingsView->rowCount() < 14)
            settingsView->setRowCount(14);
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
        settingsView->setVerticalHeaderItem(9, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        settingsView->setVerticalHeaderItem(10, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        settingsView->setVerticalHeaderItem(11, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        settingsView->setVerticalHeaderItem(12, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        settingsView->setVerticalHeaderItem(13, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        __qtablewidgetitem15->setCheckState(Qt::Unchecked);
        settingsView->setItem(0, 0, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        __qtablewidgetitem16->setCheckState(Qt::Checked);
        settingsView->setItem(1, 0, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        __qtablewidgetitem17->setCheckState(Qt::Checked);
        settingsView->setItem(2, 0, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        __qtablewidgetitem18->setCheckState(Qt::Unchecked);
        settingsView->setItem(3, 0, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        __qtablewidgetitem19->setCheckState(Qt::Checked);
        settingsView->setItem(4, 0, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        __qtablewidgetitem20->setCheckState(Qt::Checked);
        settingsView->setItem(5, 0, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        __qtablewidgetitem21->setCheckState(Qt::Unchecked);
        settingsView->setItem(6, 0, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        __qtablewidgetitem22->setCheckState(Qt::Checked);
        settingsView->setItem(7, 0, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        __qtablewidgetitem23->setCheckState(Qt::Checked);
        settingsView->setItem(8, 0, __qtablewidgetitem23);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        __qtablewidgetitem24->setCheckState(Qt::Unchecked);
        settingsView->setItem(9, 0, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        __qtablewidgetitem25->setCheckState(Qt::Unchecked);
        settingsView->setItem(10, 0, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        __qtablewidgetitem26->setCheckState(Qt::Unchecked);
        settingsView->setItem(11, 0, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        __qtablewidgetitem27->setCheckState(Qt::Unchecked);
        settingsView->setItem(12, 0, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        __qtablewidgetitem28->setCheckState(Qt::Unchecked);
        settingsView->setItem(13, 0, __qtablewidgetitem28);
        settingsView->setObjectName(QStringLiteral("settingsView"));
        settingsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        settingsView->setAlternatingRowColors(true);
        settingsView->setSelectionMode(QAbstractItemView::SingleSelection);
        settingsView->setSelectionBehavior(QAbstractItemView::SelectRows);
        settingsView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        settingsView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        settingsView->setCornerButtonEnabled(false);
        settingsView->horizontalHeader()->setVisible(false);
        settingsView->horizontalHeader()->setHighlightSections(false);
        settingsView->horizontalHeader()->setStretchLastSection(true);
        settingsView->verticalHeader()->setVisible(false);
        settingsView->verticalHeader()->setDefaultSectionSize(20);
        settingsView->verticalHeader()->setMinimumSectionSize(20);

        gridLayout->addWidget(settingsView, 0, 0, 1, 1);


        retranslateUi(SettingsWidget);

        QMetaObject::connectSlotsByName(SettingsWidget);
    } // setupUi

    void retranslateUi(QWidget *SettingsWidget)
    {
        SettingsWidget->setWindowTitle(QApplication::translate("SettingsWidget", "Form", 0));
        QTableWidgetItem *___qtablewidgetitem = settingsView->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("SettingsWidget", "Options: ( Double-click to enable or disable )", 0));
        QTableWidgetItem *___qtablewidgetitem1 = settingsView->verticalHeaderItem(0);
        ___qtablewidgetitem1->setText(QApplication::translate("SettingsWidget", "0", 0));
        QTableWidgetItem *___qtablewidgetitem2 = settingsView->verticalHeaderItem(1);
        ___qtablewidgetitem2->setText(QApplication::translate("SettingsWidget", "1", 0));
        QTableWidgetItem *___qtablewidgetitem3 = settingsView->verticalHeaderItem(2);
        ___qtablewidgetitem3->setText(QApplication::translate("SettingsWidget", "2", 0));
        QTableWidgetItem *___qtablewidgetitem4 = settingsView->verticalHeaderItem(3);
        ___qtablewidgetitem4->setText(QApplication::translate("SettingsWidget", "3", 0));
        QTableWidgetItem *___qtablewidgetitem5 = settingsView->verticalHeaderItem(4);
        ___qtablewidgetitem5->setText(QApplication::translate("SettingsWidget", "4", 0));
        QTableWidgetItem *___qtablewidgetitem6 = settingsView->verticalHeaderItem(5);
        ___qtablewidgetitem6->setText(QApplication::translate("SettingsWidget", "5", 0));
        QTableWidgetItem *___qtablewidgetitem7 = settingsView->verticalHeaderItem(6);
        ___qtablewidgetitem7->setText(QApplication::translate("SettingsWidget", "6", 0));
        QTableWidgetItem *___qtablewidgetitem8 = settingsView->verticalHeaderItem(7);
        ___qtablewidgetitem8->setText(QApplication::translate("SettingsWidget", "7", 0));
        QTableWidgetItem *___qtablewidgetitem9 = settingsView->verticalHeaderItem(8);
        ___qtablewidgetitem9->setText(QApplication::translate("SettingsWidget", "8", 0));
        QTableWidgetItem *___qtablewidgetitem10 = settingsView->verticalHeaderItem(9);
        ___qtablewidgetitem10->setText(QApplication::translate("SettingsWidget", "9", 0));
        QTableWidgetItem *___qtablewidgetitem11 = settingsView->verticalHeaderItem(10);
        ___qtablewidgetitem11->setText(QApplication::translate("SettingsWidget", "10", 0));
        QTableWidgetItem *___qtablewidgetitem12 = settingsView->verticalHeaderItem(11);
        ___qtablewidgetitem12->setText(QApplication::translate("SettingsWidget", "11", 0));
        QTableWidgetItem *___qtablewidgetitem13 = settingsView->verticalHeaderItem(12);
        ___qtablewidgetitem13->setText(QApplication::translate("SettingsWidget", "12", 0));
        QTableWidgetItem *___qtablewidgetitem14 = settingsView->verticalHeaderItem(13);
        ___qtablewidgetitem14->setText(QApplication::translate("SettingsWidget", "13", 0));

        const bool __sortingEnabled = settingsView->isSortingEnabled();
        settingsView->setSortingEnabled(false);
        QTableWidgetItem *___qtablewidgetitem15 = settingsView->item(0, 0);
        ___qtablewidgetitem15->setText(QApplication::translate("SettingsWidget", "Require Password", 0));
        QTableWidgetItem *___qtablewidgetitem16 = settingsView->item(1, 0);
        ___qtablewidgetitem16->setText(QApplication::translate("SettingsWidget", "Require Remote-Admin Auth", 0));
        QTableWidgetItem *___qtablewidgetitem17 = settingsView->item(2, 0);
        ___qtablewidgetitem17->setText(QApplication::translate("SettingsWidget", "Allow Duplicate IP's", 0));
        QTableWidgetItem *___qtablewidgetitem18 = settingsView->item(3, 0);
        ___qtablewidgetitem18->setText(QApplication::translate("SettingsWidget", "Ban Duplicate IP's", 0));
        QTableWidgetItem *___qtablewidgetitem19 = settingsView->item(4, 0);
        ___qtablewidgetitem19->setText(QApplication::translate("SettingsWidget", "Auto-Ban on Hack", 0));
        QTableWidgetItem *___qtablewidgetitem20 = settingsView->item(5, 0);
        ___qtablewidgetitem20->setText(QApplication::translate("SettingsWidget", "Require Sernums", 0));
        QTableWidgetItem *___qtablewidgetitem21 = settingsView->item(6, 0);
        ___qtablewidgetitem21->setText(QApplication::translate("SettingsWidget", "Disconnect Idle Users", 0));
        QTableWidgetItem *___qtablewidgetitem22 = settingsView->item(7, 0);
        ___qtablewidgetitem22->setText(QApplication::translate("SettingsWidget", "Allow Server Variables", 0));
        QTableWidgetItem *___qtablewidgetitem23 = settingsView->item(8, 0);
        ___qtablewidgetitem23->setText(QApplication::translate("SettingsWidget", "Log Comments to File", 0));
        QTableWidgetItem *___qtablewidgetitem24 = settingsView->item(9, 0);
        ___qtablewidgetitem24->setText(QApplication::translate("SettingsWidget", "Echo Comment to Admins", 0));
        QTableWidgetItem *___qtablewidgetitem25 = settingsView->item(10, 0);
        ___qtablewidgetitem25->setText(QApplication::translate("SettingsWidget", "Echo Comment to Users (Sender)", 0));
        QTableWidgetItem *___qtablewidgetitem26 = settingsView->item(11, 0);
        ___qtablewidgetitem26->setText(QApplication::translate("SettingsWidget", "Inform Admins of Login Events", 0));
        QTableWidgetItem *___qtablewidgetitem27 = settingsView->item(12, 0);
        ___qtablewidgetitem27->setText(QApplication::translate("SettingsWidget", "Minimize to System Tray", 0));
        QTableWidgetItem *___qtablewidgetitem28 = settingsView->item(13, 0);
        ___qtablewidgetitem28->setText(QApplication::translate("SettingsWidget", "Save Window Positions", 0));
        settingsView->setSortingEnabled(__sortingEnabled);

    } // retranslateUi

};

namespace Ui {
    class SettingsWidget: public Ui_SettingsWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSWIDGET_H
