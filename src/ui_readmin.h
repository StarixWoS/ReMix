/********************************************************************************
** Form generated from reading UI file 'readmin.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_READMIN_H
#define UI_READMIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QAction *actionRevokeAdmin;
    QAction *actionChangeRank;
    QAction *actionGameMaster;
    QAction *actionCoAdmin;
    QAction *actionAdmin;
    QAction *actionOwner;
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QTableView *adminTable;
    QLineEdit *adminPwd;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *adminSerNum;
    QLabel *label_3;
    QComboBox *comboBox;
    QPushButton *makeAdmin;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(499, 247);
        actionRevokeAdmin = new QAction(Dialog);
        actionRevokeAdmin->setObjectName(QStringLiteral("actionRevokeAdmin"));
        actionChangeRank = new QAction(Dialog);
        actionChangeRank->setObjectName(QStringLiteral("actionChangeRank"));
        actionGameMaster = new QAction(Dialog);
        actionGameMaster->setObjectName(QStringLiteral("actionGameMaster"));
        actionCoAdmin = new QAction(Dialog);
        actionCoAdmin->setObjectName(QStringLiteral("actionCoAdmin"));
        actionAdmin = new QAction(Dialog);
        actionAdmin->setObjectName(QStringLiteral("actionAdmin"));
        actionOwner = new QAction(Dialog);
        actionOwner->setObjectName(QStringLiteral("actionOwner"));
        gridLayout = new QGridLayout(Dialog);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(10, 10, 10, 10);
        frame = new QFrame(Dialog);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        adminTable = new QTableView(frame);
        adminTable->setObjectName(QStringLiteral("adminTable"));
        adminTable->setContextMenuPolicy(Qt::CustomContextMenu);
        adminTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        adminTable->setSelectionMode(QAbstractItemView::SingleSelection);
        adminTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        adminTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        adminTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        adminTable->setSortingEnabled(true);
        adminTable->setCornerButtonEnabled(false);
        adminTable->horizontalHeader()->setMinimumSectionSize(50);
        adminTable->horizontalHeader()->setStretchLastSection(true);
        adminTable->verticalHeader()->setVisible(false);
        adminTable->verticalHeader()->setMinimumSectionSize(30);

        gridLayout_2->addWidget(adminTable, 0, 0, 1, 8);

        adminPwd = new QLineEdit(frame);
        adminPwd->setObjectName(QStringLiteral("adminPwd"));

        gridLayout_2->addWidget(adminPwd, 1, 3, 1, 1);

        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(label, 1, 0, 1, 1);

        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(label_2, 1, 2, 1, 1);

        adminSerNum = new QLineEdit(frame);
        adminSerNum->setObjectName(QStringLiteral("adminSerNum"));

        gridLayout_2->addWidget(adminSerNum, 1, 1, 1, 1);

        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));
        sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(label_3, 1, 4, 1, 1);

        comboBox = new QComboBox(frame);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        gridLayout_2->addWidget(comboBox, 1, 5, 1, 1);

        makeAdmin = new QPushButton(frame);
        makeAdmin->setObjectName(QStringLiteral("makeAdmin"));
        makeAdmin->setAutoDefault(false);

        gridLayout_2->addWidget(makeAdmin, 1, 6, 1, 2);


        gridLayout->addWidget(frame, 0, 0, 1, 1);


        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Remote Admins:", 0));
        actionRevokeAdmin->setText(QApplication::translate("Dialog", "Revoke Admin", 0));
        actionChangeRank->setText(QApplication::translate("Dialog", "Change Rank", 0));
        actionGameMaster->setText(QApplication::translate("Dialog", "Game Master", 0));
        actionCoAdmin->setText(QApplication::translate("Dialog", "Co-Admin", 0));
        actionAdmin->setText(QApplication::translate("Dialog", "Admin", 0));
        actionOwner->setText(QApplication::translate("Dialog", "Owner", 0));
        label->setText(QApplication::translate("Dialog", "SerNum:", 0));
        label_2->setText(QApplication::translate("Dialog", "Password:", 0));
        label_3->setText(QApplication::translate("Dialog", "Rank:", 0));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("Dialog", "GM", 0)
         << QApplication::translate("Dialog", "Co-Ad", 0)
         << QApplication::translate("Dialog", "Admin", 0)
         << QApplication::translate("Dialog", "Owner", 0)
        );
        makeAdmin->setText(QApplication::translate("Dialog", "Make Admin", 0));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_READMIN_H
