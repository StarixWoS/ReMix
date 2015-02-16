/********************************************************************************
** Form generated from reading UI file 'remix.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REMIX_H
#define UI_REMIX_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ReMix
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;

    void setupUi(QMainWindow *ReMix)
    {
        if (ReMix->objectName().isEmpty())
            ReMix->setObjectName(QStringLiteral("ReMix"));
        ReMix->resize(650, 256);
        ReMix->setMinimumSize(QSize(0, 256));
        ReMix->setMaximumSize(QSize(16777215, 395));
        ReMix->setDockNestingEnabled(false);
        centralWidget = new QWidget(ReMix);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(5);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        frame = new QFrame(centralWidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setSizeConstraint(QLayout::SetNoConstraint);
        gridLayout_2->setContentsMargins(0, 0, 0, 0);

        gridLayout->addWidget(frame, 0, 1, 1, 1);

        ReMix->setCentralWidget(centralWidget);

        retranslateUi(ReMix);

        QMetaObject::connectSlotsByName(ReMix);
    } // setupUi

    void retranslateUi(QMainWindow *ReMix)
    {
        ReMix->setWindowTitle(QApplication::translate("ReMix", "ReMix", 0));
    } // retranslateUi

};

namespace Ui {
    class ReMix: public Ui_ReMix {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REMIX_H
