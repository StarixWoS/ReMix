/********************************************************************************
** Form generated from reading UI file 'ruleswidget.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RULESWIDGET_H
#define UI_RULESWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RulesWidget
{
public:
    QGridLayout *gridLayout;
    QTableWidget *rulesView;

    void setupUi(QWidget *RulesWidget)
    {
        if (RulesWidget->objectName().isEmpty())
            RulesWidget->setObjectName(QStringLiteral("RulesWidget"));
        RulesWidget->resize(80, 80);
        gridLayout = new QGridLayout(RulesWidget);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        rulesView = new QTableWidget(RulesWidget);
        if (rulesView->columnCount() < 1)
            rulesView->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        rulesView->setHorizontalHeaderItem(0, __qtablewidgetitem);
        if (rulesView->rowCount() < 15)
            rulesView->setRowCount(15);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        rulesView->setVerticalHeaderItem(0, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        rulesView->setVerticalHeaderItem(1, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        rulesView->setVerticalHeaderItem(2, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        rulesView->setVerticalHeaderItem(3, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        rulesView->setVerticalHeaderItem(4, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        rulesView->setVerticalHeaderItem(5, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        rulesView->setVerticalHeaderItem(6, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        rulesView->setVerticalHeaderItem(7, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        rulesView->setVerticalHeaderItem(8, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        rulesView->setVerticalHeaderItem(9, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        rulesView->setVerticalHeaderItem(10, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        rulesView->setVerticalHeaderItem(11, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        rulesView->setVerticalHeaderItem(12, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        rulesView->setVerticalHeaderItem(13, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        rulesView->setVerticalHeaderItem(14, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        __qtablewidgetitem16->setCheckState(Qt::Unchecked);
        rulesView->setItem(0, 0, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        __qtablewidgetitem17->setCheckState(Qt::Unchecked);
        rulesView->setItem(1, 0, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        __qtablewidgetitem18->setCheckState(Qt::Unchecked);
        rulesView->setItem(2, 0, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        __qtablewidgetitem19->setCheckState(Qt::Unchecked);
        rulesView->setItem(3, 0, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        __qtablewidgetitem20->setCheckState(Qt::Unchecked);
        rulesView->setItem(4, 0, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        __qtablewidgetitem21->setCheckState(Qt::Unchecked);
        rulesView->setItem(5, 0, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        __qtablewidgetitem22->setCheckState(Qt::Unchecked);
        rulesView->setItem(6, 0, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        __qtablewidgetitem23->setCheckState(Qt::Unchecked);
        rulesView->setItem(7, 0, __qtablewidgetitem23);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        __qtablewidgetitem24->setCheckState(Qt::Unchecked);
        rulesView->setItem(8, 0, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        __qtablewidgetitem25->setCheckState(Qt::Unchecked);
        rulesView->setItem(9, 0, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        __qtablewidgetitem26->setCheckState(Qt::Unchecked);
        rulesView->setItem(10, 0, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        __qtablewidgetitem27->setCheckState(Qt::Unchecked);
        rulesView->setItem(11, 0, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        __qtablewidgetitem28->setCheckState(Qt::Unchecked);
        rulesView->setItem(12, 0, __qtablewidgetitem28);
        QTableWidgetItem *__qtablewidgetitem29 = new QTableWidgetItem();
        __qtablewidgetitem29->setCheckState(Qt::Unchecked);
        rulesView->setItem(13, 0, __qtablewidgetitem29);
        QTableWidgetItem *__qtablewidgetitem30 = new QTableWidgetItem();
        __qtablewidgetitem30->setCheckState(Qt::Unchecked);
        rulesView->setItem(14, 0, __qtablewidgetitem30);
        rulesView->setObjectName(QStringLiteral("rulesView"));
        rulesView->setFrameShadow(QFrame::Sunken);
        rulesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        rulesView->setAlternatingRowColors(true);
        rulesView->setSelectionMode(QAbstractItemView::SingleSelection);
        rulesView->setSelectionBehavior(QAbstractItemView::SelectRows);
        rulesView->setTextElideMode(Qt::ElideRight);
        rulesView->setCornerButtonEnabled(false);
        rulesView->horizontalHeader()->setVisible(false);
        rulesView->horizontalHeader()->setHighlightSections(false);
        rulesView->horizontalHeader()->setStretchLastSection(true);
        rulesView->verticalHeader()->setVisible(false);
        rulesView->verticalHeader()->setDefaultSectionSize(20);
        rulesView->verticalHeader()->setMinimumSectionSize(20);

        gridLayout->addWidget(rulesView, 0, 0, 1, 1);


        retranslateUi(RulesWidget);

        QMetaObject::connectSlotsByName(RulesWidget);
    } // setupUi

    void retranslateUi(QWidget *RulesWidget)
    {
        RulesWidget->setWindowTitle(QApplication::translate("RulesWidget", "Form", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem = rulesView->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("RulesWidget", "Settings", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem1 = rulesView->verticalHeaderItem(0);
        ___qtablewidgetitem1->setText(QApplication::translate("RulesWidget", "1", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem2 = rulesView->verticalHeaderItem(1);
        ___qtablewidgetitem2->setText(QApplication::translate("RulesWidget", "2", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem3 = rulesView->verticalHeaderItem(2);
        ___qtablewidgetitem3->setText(QApplication::translate("RulesWidget", "12", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem4 = rulesView->verticalHeaderItem(3);
        ___qtablewidgetitem4->setText(QApplication::translate("RulesWidget", "3", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem5 = rulesView->verticalHeaderItem(4);
        ___qtablewidgetitem5->setText(QApplication::translate("RulesWidget", "4", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem6 = rulesView->verticalHeaderItem(5);
        ___qtablewidgetitem6->setText(QApplication::translate("RulesWidget", "5", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem7 = rulesView->verticalHeaderItem(6);
        ___qtablewidgetitem7->setText(QApplication::translate("RulesWidget", "6", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem8 = rulesView->verticalHeaderItem(7);
        ___qtablewidgetitem8->setText(QApplication::translate("RulesWidget", "7", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem9 = rulesView->verticalHeaderItem(8);
        ___qtablewidgetitem9->setText(QApplication::translate("RulesWidget", "8", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem10 = rulesView->verticalHeaderItem(9);
        ___qtablewidgetitem10->setText(QApplication::translate("RulesWidget", "15", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem11 = rulesView->verticalHeaderItem(10);
        ___qtablewidgetitem11->setText(QApplication::translate("RulesWidget", "14", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem12 = rulesView->verticalHeaderItem(11);
        ___qtablewidgetitem12->setText(QApplication::translate("RulesWidget", "9", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem13 = rulesView->verticalHeaderItem(12);
        ___qtablewidgetitem13->setText(QApplication::translate("RulesWidget", "10", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem14 = rulesView->verticalHeaderItem(13);
        ___qtablewidgetitem14->setText(QApplication::translate("RulesWidget", "13", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem15 = rulesView->verticalHeaderItem(14);
        ___qtablewidgetitem15->setText(QApplication::translate("RulesWidget", "11", Q_NULLPTR));

        const bool __sortingEnabled = rulesView->isSortingEnabled();
        rulesView->setSortingEnabled(false);
        QTableWidgetItem *___qtablewidgetitem16 = rulesView->item(0, 0);
        ___qtablewidgetitem16->setText(QApplication::translate("RulesWidget", "World Name", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem17 = rulesView->item(1, 0);
        ___qtablewidgetitem17->setText(QApplication::translate("RulesWidget", "Server Home", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem18 = rulesView->item(2, 0);
        ___qtablewidgetitem18->setText(QApplication::translate("RulesWidget", "All PKing", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem19 = rulesView->item(3, 0);
        ___qtablewidgetitem19->setText(QApplication::translate("RulesWidget", "Max Players", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem20 = rulesView->item(4, 0);
        ___qtablewidgetitem20->setText(QApplication::translate("RulesWidget", "Max AFK", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem21 = rulesView->item(5, 0);
        ___qtablewidgetitem21->setText(QApplication::translate("RulesWidget", "Min Version", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem22 = rulesView->item(6, 0);
        ___qtablewidgetitem22->setText(QApplication::translate("RulesWidget", "Report to Ladder", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem23 = rulesView->item(7, 0);
        ___qtablewidgetitem23->setText(QApplication::translate("RulesWidget", "No Cursing", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem24 = rulesView->item(8, 0);
        ___qtablewidgetitem24->setText(QApplication::translate("RulesWidget", "No Cheating", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem25 = rulesView->item(9, 0);
        ___qtablewidgetitem25->setText(QApplication::translate("RulesWidget", "No Eavesdropping", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem26 = rulesView->item(10, 0);
        ___qtablewidgetitem26->setText(QApplication::translate("RulesWidget", "No Migrating", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem27 = rulesView->item(11, 0);
        ___qtablewidgetitem27->setText(QApplication::translate("RulesWidget", "No Modders", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem28 = rulesView->item(12, 0);
        ___qtablewidgetitem28->setText(QApplication::translate("RulesWidget", "No Pets", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem29 = rulesView->item(13, 0);
        ___qtablewidgetitem29->setText(QApplication::translate("RulesWidget", "No PKing", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem30 = rulesView->item(14, 0);
        ___qtablewidgetitem30->setText(QApplication::translate("RulesWidget", "Arena PKing", Q_NULLPTR));
        rulesView->setSortingEnabled(__sortingEnabled);

    } // retranslateUi

};

namespace Ui {
    class RulesWidget: public Ui_RulesWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RULESWIDGET_H
