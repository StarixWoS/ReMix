/********************************************************************************
** Form generated from reading UI file 'comments.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMMENTS_H
#define UI_COMMENTS_H

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

class Ui_Comments
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QTextEdit *msgView;

    void setupUi(QDialog *Comments)
    {
        if (Comments->objectName().isEmpty())
            Comments->setObjectName(QStringLiteral("Comments"));
        Comments->resize(279, 150);
        gridLayout = new QGridLayout(Comments);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        frame = new QFrame(Comments);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setSpacing(5);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(5, 5, 5, 5);
        msgView = new QTextEdit(frame);
        msgView->setObjectName(QStringLiteral("msgView"));
        msgView->setUndoRedoEnabled(true);
        msgView->setReadOnly(true);

        gridLayout_2->addWidget(msgView, 0, 0, 1, 1);


        gridLayout->addWidget(frame, 0, 0, 1, 1);


        retranslateUi(Comments);

        QMetaObject::connectSlotsByName(Comments);
    } // setupUi

    void retranslateUi(QDialog *Comments)
    {
        Comments->setWindowTitle(QApplication::translate("Comments", "User Comments:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Comments: public Ui_Comments {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMMENTS_H
