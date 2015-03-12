/********************************************************************************
** Form generated from reading UI file 'banwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BANWIDGET_H
#define UI_BANWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BanWidget
{
public:
    QGridLayout *gridLayout;
    QTableView *banTable;
    QLabel *label;
    QLineEdit *trgIP;
    QLabel *label_3;
    QLineEdit *trgSN;
    QLabel *label_7;
    QLineEdit *trgDV;
    QLabel *label_5;
    QLineEdit *trgWV;
    QLabel *label_8;
    QLineEdit *reason;
    QPushButton *addBan;
    QPushButton *removeBan;

    void setupUi(QWidget *BanWidget)
    {
        if (BanWidget->objectName().isEmpty())
            BanWidget->setObjectName(QStringLiteral("BanWidget"));
        BanWidget->resize(541, 256);
        gridLayout = new QGridLayout(BanWidget);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        banTable = new QTableView(BanWidget);
        banTable->setObjectName(QStringLiteral("banTable"));
        banTable->setAlternatingRowColors(true);
        banTable->setSelectionMode(QAbstractItemView::SingleSelection);
        banTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        banTable->setSortingEnabled(true);
        banTable->setCornerButtonEnabled(false);
        banTable->horizontalHeader()->setVisible(true);
        banTable->horizontalHeader()->setStretchLastSection(true);
        banTable->verticalHeader()->setVisible(false);
        banTable->verticalHeader()->setDefaultSectionSize(20);
        banTable->verticalHeader()->setMinimumSectionSize(20);

        gridLayout->addWidget(banTable, 0, 0, 1, 8);

        label = new QLabel(BanWidget);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);

        gridLayout->addWidget(label, 1, 0, 1, 1);

        trgIP = new QLineEdit(BanWidget);
        trgIP->setObjectName(QStringLiteral("trgIP"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(trgIP->sizePolicy().hasHeightForWidth());
        trgIP->setSizePolicy(sizePolicy1);
        trgIP->setMinimumSize(QSize(80, 0));

        gridLayout->addWidget(trgIP, 1, 1, 1, 1);

        label_3 = new QLabel(BanWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy);

        gridLayout->addWidget(label_3, 1, 2, 1, 1);

        trgSN = new QLineEdit(BanWidget);
        trgSN->setObjectName(QStringLiteral("trgSN"));
        sizePolicy1.setHeightForWidth(trgSN->sizePolicy().hasHeightForWidth());
        trgSN->setSizePolicy(sizePolicy1);
        trgSN->setMinimumSize(QSize(80, 0));

        gridLayout->addWidget(trgSN, 1, 3, 1, 1);

        label_7 = new QLabel(BanWidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        sizePolicy.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy);

        gridLayout->addWidget(label_7, 1, 4, 1, 1);

        trgDV = new QLineEdit(BanWidget);
        trgDV->setObjectName(QStringLiteral("trgDV"));
        trgDV->setMinimumSize(QSize(80, 0));

        gridLayout->addWidget(trgDV, 1, 5, 1, 1);

        label_5 = new QLabel(BanWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        sizePolicy.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy);

        gridLayout->addWidget(label_5, 1, 6, 1, 1);

        trgWV = new QLineEdit(BanWidget);
        trgWV->setObjectName(QStringLiteral("trgWV"));
        trgWV->setMinimumSize(QSize(80, 0));

        gridLayout->addWidget(trgWV, 1, 7, 1, 1);

        label_8 = new QLabel(BanWidget);
        label_8->setObjectName(QStringLiteral("label_8"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(label_8, 2, 0, 1, 1);

        reason = new QLineEdit(BanWidget);
        reason->setObjectName(QStringLiteral("reason"));
        reason->setMinimumSize(QSize(80, 0));

        gridLayout->addWidget(reason, 2, 1, 1, 7);

        addBan = new QPushButton(BanWidget);
        addBan->setObjectName(QStringLiteral("addBan"));
        sizePolicy1.setHeightForWidth(addBan->sizePolicy().hasHeightForWidth());
        addBan->setSizePolicy(sizePolicy1);
        addBan->setMinimumSize(QSize(200, 0));
        addBan->setAutoDefault(false);

        gridLayout->addWidget(addBan, 3, 0, 1, 3);

        removeBan = new QPushButton(BanWidget);
        removeBan->setObjectName(QStringLiteral("removeBan"));
        removeBan->setMinimumSize(QSize(200, 0));
        removeBan->setAutoDefault(false);

        gridLayout->addWidget(removeBan, 3, 5, 1, 3);


        retranslateUi(BanWidget);

        QMetaObject::connectSlotsByName(BanWidget);
    } // setupUi

    void retranslateUi(QWidget *BanWidget)
    {
        BanWidget->setWindowTitle(QApplication::translate("BanWidget", "Form", 0));
        label->setText(QApplication::translate("BanWidget", "IP:", 0));
        trgIP->setText(QApplication::translate("BanWidget", "127.0.0.1", 0));
        label_3->setText(QApplication::translate("BanWidget", "Sernum:", 0));
        label_7->setText(QApplication::translate("BanWidget", "Device:", 0));
        label_5->setText(QApplication::translate("BanWidget", "Date:", 0));
        label_8->setText(QApplication::translate("BanWidget", "Reason:", 0));
        addBan->setText(QApplication::translate("BanWidget", "Add Ban", 0));
        removeBan->setText(QApplication::translate("BanWidget", "Remove Ban", 0));
    } // retranslateUi

};

namespace Ui {
    class BanWidget: public Ui_BanWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BANWIDGET_H
