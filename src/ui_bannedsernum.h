/********************************************************************************
** Form generated from reading UI file 'bannedsernum.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BANNEDSERNUM_H
#define UI_BANNEDSERNUM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>

QT_BEGIN_NAMESPACE

class Ui_BannedSernum
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QTableView *sernumTable;
    QPushButton *forgiveButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *cancelButton;

    void setupUi(QDialog *BannedSernum)
    {
        if (BannedSernum->objectName().isEmpty())
            BannedSernum->setObjectName(QStringLiteral("BannedSernum"));
        BannedSernum->resize(565, 270);
        gridLayout = new QGridLayout(BannedSernum);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        frame = new QFrame(BannedSernum);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        sernumTable = new QTableView(frame);
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
        sernumTable->verticalHeader()->setMinimumSectionSize(30);

        gridLayout_2->addWidget(sernumTable, 0, 0, 1, 3);

        forgiveButton = new QPushButton(frame);
        forgiveButton->setObjectName(QStringLiteral("forgiveButton"));
        forgiveButton->setEnabled(false);
        forgiveButton->setAutoDefault(false);

        gridLayout_2->addWidget(forgiveButton, 1, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(310, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 1, 1, 1, 1);

        cancelButton = new QPushButton(frame);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));
        cancelButton->setAutoDefault(false);

        gridLayout_2->addWidget(cancelButton, 1, 2, 1, 1);


        gridLayout->addWidget(frame, 0, 0, 1, 1);


        retranslateUi(BannedSernum);

        QMetaObject::connectSlotsByName(BannedSernum);
    } // setupUi

    void retranslateUi(QDialog *BannedSernum)
    {
        BannedSernum->setWindowTitle(QApplication::translate("BannedSernum", "Banned Sernums:", 0));
        forgiveButton->setText(QApplication::translate("BannedSernum", "Forgive Selected Sernum", 0));
        cancelButton->setText(QApplication::translate("BannedSernum", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class BannedSernum: public Ui_BannedSernum {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BANNEDSERNUM_H
