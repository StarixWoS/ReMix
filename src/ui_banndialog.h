/********************************************************************************
** Form generated from reading UI file 'banndialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BANNDIALOG_H
#define UI_BANNDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_BannDialog
{
public:

    void setupUi(QDialog *BannDialog)
    {
        if (BannDialog->objectName().isEmpty())
            BannDialog->setObjectName(QStringLiteral("BannDialog"));
        BannDialog->resize(400, 300);

        retranslateUi(BannDialog);

        QMetaObject::connectSlotsByName(BannDialog);
    } // setupUi

    void retranslateUi(QDialog *BannDialog)
    {
        BannDialog->setWindowTitle(QApplication::translate("BannDialog", "Dialog", 0));
    } // retranslateUi

};

namespace Ui {
    class BannDialog: public Ui_BannDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BANNDIALOG_H
