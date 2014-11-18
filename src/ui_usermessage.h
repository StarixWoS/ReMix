/********************************************************************************
** Form generated from reading UI file 'usermessage.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERMESSAGE_H
#define UI_USERMESSAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_UserMessage
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QTextEdit *msgView;

    void setupUi(QDialog *UserMessage)
    {
        if (UserMessage->objectName().isEmpty())
            UserMessage->setObjectName(QStringLiteral("UserMessage"));
        UserMessage->resize(489, 301);
        gridLayout = new QGridLayout(UserMessage);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        frame = new QFrame(UserMessage);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        msgView = new QTextEdit(frame);
        msgView->setObjectName(QStringLiteral("msgView"));
        msgView->setReadOnly(true);
        msgView->setTextInteractionFlags(Qt::TextSelectableByMouse);

        gridLayout_2->addWidget(msgView, 0, 0, 1, 1);


        gridLayout->addWidget(frame, 0, 0, 1, 1);


        retranslateUi(UserMessage);

        QMetaObject::connectSlotsByName(UserMessage);
    } // setupUi

    void retranslateUi(QDialog *UserMessage)
    {
        UserMessage->setWindowTitle(QApplication::translate("UserMessage", "Player Comments:", 0));
    } // retranslateUi

};

namespace Ui {
    class UserMessage: public Ui_UserMessage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERMESSAGE_H
