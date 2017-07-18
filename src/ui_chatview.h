/********************************************************************************
** Form generated from reading UI file 'chatview.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATVIEW_H
#define UI_CHATVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_ChatView
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QLineEdit *chatInput;
    QTextEdit *chatView;

    void setupUi(QDialog *ChatView)
    {
        if (ChatView->objectName().isEmpty())
            ChatView->setObjectName(QStringLiteral("ChatView"));
        ChatView->resize(464, 344);
        gridLayout = new QGridLayout(ChatView);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        frame = new QFrame(ChatView);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setSpacing(5);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(5, 5, 5, 5);
        chatInput = new QLineEdit(frame);
        chatInput->setObjectName(QStringLiteral("chatInput"));

        gridLayout_2->addWidget(chatInput, 2, 0, 1, 1);

        chatView = new QTextEdit(frame);
        chatView->setObjectName(QStringLiteral("chatView"));
        chatView->setMinimumSize(QSize(442, 295));
        chatView->setReadOnly(true);

        gridLayout_2->addWidget(chatView, 0, 0, 2, 1);


        gridLayout->addWidget(frame, 0, 0, 1, 1);


        retranslateUi(ChatView);

        QMetaObject::connectSlotsByName(ChatView);
    } // setupUi

    void retranslateUi(QDialog *ChatView)
    {
        ChatView->setWindowTitle(QApplication::translate("ChatView", "Chat View:", 0));
    } // retranslateUi

};

namespace Ui {
    class ChatView: public Ui_ChatView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATVIEW_H
