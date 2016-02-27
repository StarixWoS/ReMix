/********************************************************************************
** Form generated from reading UI file 'plrlistwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLRLISTWIDGET_H
#define UI_PLRLISTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlrListWidget
{
public:
    QAction *actionSendMessage;
    QAction *actionMakeAdmin;
    QAction *actionDisconnectUser;
    QAction *actionMuteNetwork;
    QAction *actionBANISHUser;
    QGridLayout *gridLayout;
    QTableView *playerView;

    void setupUi(QWidget *PlrListWidget)
    {
        if (PlrListWidget->objectName().isEmpty())
            PlrListWidget->setObjectName(QStringLiteral("PlrListWidget"));
        PlrListWidget->resize(70, 70);
        actionSendMessage = new QAction(PlrListWidget);
        actionSendMessage->setObjectName(QStringLiteral("actionSendMessage"));
        actionMakeAdmin = new QAction(PlrListWidget);
        actionMakeAdmin->setObjectName(QStringLiteral("actionMakeAdmin"));
        actionDisconnectUser = new QAction(PlrListWidget);
        actionDisconnectUser->setObjectName(QStringLiteral("actionDisconnectUser"));
        actionMuteNetwork = new QAction(PlrListWidget);
        actionMuteNetwork->setObjectName(QStringLiteral("actionMuteNetwork"));
        actionBANISHUser = new QAction(PlrListWidget);
        actionBANISHUser->setObjectName(QStringLiteral("actionBANISHUser"));
        gridLayout = new QGridLayout(PlrListWidget);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        playerView = new QTableView(PlrListWidget);
        playerView->setObjectName(QStringLiteral("playerView"));
        playerView->setMouseTracking(false);
        playerView->setContextMenuPolicy(Qt::CustomContextMenu);
        playerView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        playerView->setAlternatingRowColors(true);
        playerView->setSelectionMode(QAbstractItemView::SingleSelection);
        playerView->setSelectionBehavior(QAbstractItemView::SelectRows);
        playerView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        playerView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        playerView->setSortingEnabled(true);
        playerView->setCornerButtonEnabled(false);
        playerView->horizontalHeader()->setDefaultSectionSize(100);
        playerView->horizontalHeader()->setHighlightSections(false);
        playerView->horizontalHeader()->setStretchLastSection(false);
        playerView->verticalHeader()->setVisible(false);
        playerView->verticalHeader()->setDefaultSectionSize(20);
        playerView->verticalHeader()->setMinimumSectionSize(20);

        gridLayout->addWidget(playerView, 0, 0, 1, 1);


        retranslateUi(PlrListWidget);

        QMetaObject::connectSlotsByName(PlrListWidget);
    } // setupUi

    void retranslateUi(QWidget *PlrListWidget)
    {
        PlrListWidget->setWindowTitle(QApplication::translate("PlrListWidget", "Form", 0));
        actionSendMessage->setText(QApplication::translate("PlrListWidget", "Send Message", 0));
        actionMakeAdmin->setText(QApplication::translate("PlrListWidget", "Make Admin", 0));
        actionDisconnectUser->setText(QApplication::translate("PlrListWidget", "Disconnect User", 0));
        actionMuteNetwork->setText(QApplication::translate("PlrListWidget", "Mute Network", 0));
        actionBANISHUser->setText(QApplication::translate("PlrListWidget", "BANISH User", 0));
    } // retranslateUi

};

namespace Ui {
    class PlrListWidget: public Ui_PlrListWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLRLISTWIDGET_H
