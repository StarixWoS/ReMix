/********************************************************************************
** Form generated from reading UI file 'user.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USER_H
#define UI_USER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableView>

QT_BEGIN_NAMESPACE

class Ui_User
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QTableView *userTable;

    void setupUi(QDialog *User)
    {
        if (User->objectName().isEmpty())
            User->setObjectName(QStringLiteral("User"));
        User->resize(560, 242);
        gridLayout = new QGridLayout(User);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        frame = new QFrame(User);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setSpacing(5);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(5, 5, 5, 5);
        userTable = new QTableView(frame);
        userTable->setObjectName(QStringLiteral("userTable"));
        userTable->setContextMenuPolicy(Qt::CustomContextMenu);
        userTable->setEditTriggers(QAbstractItemView::DoubleClicked);
        userTable->setAlternatingRowColors(true);
        userTable->setSelectionMode(QAbstractItemView::SingleSelection);
        userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        userTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        userTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        userTable->setSortingEnabled(true);
        userTable->setCornerButtonEnabled(false);
        userTable->horizontalHeader()->setHighlightSections(false);
        userTable->horizontalHeader()->setMinimumSectionSize(80);
        userTable->horizontalHeader()->setStretchLastSection(true);
        userTable->verticalHeader()->setVisible(false);
        userTable->verticalHeader()->setDefaultSectionSize(20);
        userTable->verticalHeader()->setMinimumSectionSize(20);

        gridLayout_2->addWidget(userTable, 0, 0, 1, 2);


        gridLayout->addWidget(frame, 0, 0, 1, 1);


        retranslateUi(User);

        QMetaObject::connectSlotsByName(User);
    } // setupUi

    void retranslateUi(QDialog *User)
    {
        User->setWindowTitle(QApplication::translate("User", "User Information:", 0));
    } // retranslateUi

};

namespace Ui {
    class User: public Ui_User {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USER_H
