/********************************************************************************
** Form generated from reading UI file 'selectworld.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELECTWORLD_H
#define UI_SELECTWORLD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SelectWorld
{
public:
    QHBoxLayout *horizontalLayout;
    QFrame *frame;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QListView *worldViewer;
    QSplitter *splitter;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *SelectWorld)
    {
        if (SelectWorld->objectName().isEmpty())
            SelectWorld->setObjectName(QStringLiteral("SelectWorld"));
        SelectWorld->resize(213, 305);
        horizontalLayout = new QHBoxLayout(SelectWorld);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 5, 5, 5);
        frame = new QFrame(SelectWorld);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        worldViewer = new QListView(frame);
        worldViewer->setObjectName(QStringLiteral("worldViewer"));

        verticalLayout->addWidget(worldViewer);

        splitter = new QSplitter(frame);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        okButton = new QPushButton(splitter);
        okButton->setObjectName(QStringLiteral("okButton"));
        splitter->addWidget(okButton);
        cancelButton = new QPushButton(splitter);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));
        splitter->addWidget(cancelButton);

        verticalLayout->addWidget(splitter);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        horizontalLayout->addWidget(frame);


        retranslateUi(SelectWorld);

        QMetaObject::connectSlotsByName(SelectWorld);
    } // setupUi

    void retranslateUi(QDialog *SelectWorld)
    {
        SelectWorld->setWindowTitle(QApplication::translate("SelectWorld", "Select World:", 0));
        okButton->setText(QApplication::translate("SelectWorld", "Ok", 0));
        cancelButton->setText(QApplication::translate("SelectWorld", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class SelectWorld: public Ui_SelectWorld {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELECTWORLD_H
