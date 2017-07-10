/********************************************************************************
** Form generated from reading UI file 'motdwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MOTDWIDGET_H
#define UI_MOTDWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MOTDWidget
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QTextEdit *motdEdit;

    void setupUi(QWidget *MOTDWidget)
    {
        if (MOTDWidget->objectName().isEmpty())
            MOTDWidget->setObjectName(QStringLiteral("MOTDWidget"));
        MOTDWidget->resize(139, 93);
        gridLayout = new QGridLayout(MOTDWidget);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        groupBox = new QGroupBox(MOTDWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setSpacing(5);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setContentsMargins(5, 5, 5, 5);
        motdEdit = new QTextEdit(groupBox);
        motdEdit->setObjectName(QStringLiteral("motdEdit"));
        motdEdit->setMinimumSize(QSize(0, 50));
        motdEdit->setAcceptRichText(false);

        gridLayout_3->addWidget(motdEdit, 0, 0, 1, 1);


        gridLayout->addWidget(groupBox, 0, 0, 1, 1);


        retranslateUi(MOTDWidget);

        QMetaObject::connectSlotsByName(MOTDWidget);
    } // setupUi

    void retranslateUi(QWidget *MOTDWidget)
    {
        MOTDWidget->setWindowTitle(QApplication::translate("MOTDWidget", "Form", 0));
        groupBox->setTitle(QApplication::translate("MOTDWidget", "Message of the Day:", 0));
        motdEdit->setHtml(QApplication::translate("MOTDWidget", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">The administrator of this server reserves the right to disconnect or banish you if you are sufficiently unpleasant in name, chat, or action..... Other than that, WELCOME TO MY SERVER!</span></p></body></html>", 0));
    } // retranslateUi

};

namespace Ui {
    class MOTDWidget: public Ui_MOTDWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MOTDWIDGET_H
