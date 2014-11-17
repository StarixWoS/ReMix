/********************************************************************************
** Form generated from reading UI file 'messages.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESSAGES_H
#define UI_MESSAGES_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_Messages
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QTextEdit *motdEdit;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QTextEdit *banishedEdit;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_6;
    QTextEdit *rulesEdit;
    QSpacerItem *horizontalSpacer;
    QPushButton *saveSettings;
    QPushButton *reloadSettings;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_5;
    QCheckBox *reqPasword;
    QLineEdit *pwdEdit;

    void setupUi(QDialog *Messages)
    {
        if (Messages->objectName().isEmpty())
            Messages->setObjectName(QStringLiteral("Messages"));
        Messages->resize(449, 387);
        gridLayout = new QGridLayout(Messages);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        frame = new QFrame(Messages);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        groupBox = new QGroupBox(frame);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        motdEdit = new QTextEdit(groupBox);
        motdEdit->setObjectName(QStringLiteral("motdEdit"));
        motdEdit->setMinimumSize(QSize(0, 50));
        motdEdit->setMaximumSize(QSize(16777215, 50));
        motdEdit->setAcceptRichText(false);

        gridLayout_3->addWidget(motdEdit, 0, 0, 1, 1);


        gridLayout_2->addWidget(groupBox, 0, 0, 1, 3);

        groupBox_2 = new QGroupBox(frame);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout_4 = new QGridLayout(groupBox_2);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        banishedEdit = new QTextEdit(groupBox_2);
        banishedEdit->setObjectName(QStringLiteral("banishedEdit"));
        banishedEdit->setMinimumSize(QSize(0, 50));
        banishedEdit->setMaximumSize(QSize(16777215, 50));
        banishedEdit->setAcceptRichText(false);

        gridLayout_4->addWidget(banishedEdit, 0, 0, 1, 1);


        gridLayout_2->addWidget(groupBox_2, 1, 0, 1, 3);

        groupBox_4 = new QGroupBox(frame);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        gridLayout_6 = new QGridLayout(groupBox_4);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        rulesEdit = new QTextEdit(groupBox_4);
        rulesEdit->setObjectName(QStringLiteral("rulesEdit"));
        rulesEdit->setMinimumSize(QSize(0, 50));
        rulesEdit->setMaximumSize(QSize(16777215, 50));
        rulesEdit->setAcceptRichText(false);

        gridLayout_6->addWidget(rulesEdit, 0, 0, 1, 1);


        gridLayout_2->addWidget(groupBox_4, 2, 0, 1, 3);

        horizontalSpacer = new QSpacerItem(246, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 4, 0, 1, 1);

        saveSettings = new QPushButton(frame);
        saveSettings->setObjectName(QStringLiteral("saveSettings"));
        saveSettings->setAutoDefault(false);

        gridLayout_2->addWidget(saveSettings, 4, 1, 1, 1);

        reloadSettings = new QPushButton(frame);
        reloadSettings->setObjectName(QStringLiteral("reloadSettings"));
        reloadSettings->setAutoDefault(false);

        gridLayout_2->addWidget(reloadSettings, 4, 2, 1, 1);

        groupBox_3 = new QGroupBox(frame);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        gridLayout_5 = new QGridLayout(groupBox_3);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        reqPasword = new QCheckBox(groupBox_3);
        reqPasword->setObjectName(QStringLiteral("reqPasword"));
        reqPasword->setLayoutDirection(Qt::RightToLeft);

        gridLayout_5->addWidget(reqPasword, 0, 0, 1, 1);

        pwdEdit = new QLineEdit(groupBox_3);
        pwdEdit->setObjectName(QStringLiteral("pwdEdit"));
        pwdEdit->setMaxLength(512);
        pwdEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
        pwdEdit->setClearButtonEnabled(true);

        gridLayout_5->addWidget(pwdEdit, 0, 1, 1, 1);


        gridLayout_2->addWidget(groupBox_3, 3, 0, 1, 3);


        gridLayout->addWidget(frame, 0, 0, 1, 1);


        retranslateUi(Messages);

        QMetaObject::connectSlotsByName(Messages);
    } // setupUi

    void retranslateUi(QDialog *Messages)
    {
        Messages->setWindowTitle(QApplication::translate("Messages", "System Messages", 0));
        groupBox->setTitle(QApplication::translate("Messages", "Message of the Day:", 0));
        motdEdit->setHtml(QApplication::translate("Messages", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">The administrator of this server reserves the right to disconnect or banish you if you are sufficiently unpleasant in name, chat, or action..... Other than that, WELCOME TO MY SERVER!</span></p></body></html>", 0));
        groupBox_2->setTitle(QApplication::translate("Messages", "Banished Message:", 0));
        banishedEdit->setHtml(QApplication::translate("Messages", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">You are not allowed to use this server until your access is re-enabled by the administrator.</span></p></body></html>", 0));
        groupBox_4->setTitle(QApplication::translate("Messages", "Server Rules:", 0));
        saveSettings->setText(QApplication::translate("Messages", "Ok", 0));
        reloadSettings->setText(QApplication::translate("Messages", "Cancel", 0));
        groupBox_3->setTitle(QApplication::translate("Messages", "Password Notification:", 0));
        reqPasword->setText(QApplication::translate("Messages", "Password Required:", 0));
        pwdEdit->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Messages: public Ui_Messages {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESSAGES_H
