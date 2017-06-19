/********************************************************************************
** Form generated from reading UI file 'createinstance.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATEINSTANCE_H
#define UI_CREATEINSTANCE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_CreateInstance
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QLabel *label_2;
    QLineEdit *portNumber;
    QLabel *label_3;
    QCheckBox *isPublic;
    QLabel *label;
    QComboBox *gameName;
    QLineEdit *serverName;
    QPushButton *close;
    QPushButton *initializeServer;
    QComboBox *oldServers;
    QLabel *label_4;

    void setupUi(QDialog *CreateInstance)
    {
        if (CreateInstance->objectName().isEmpty())
            CreateInstance->setObjectName(QStringLiteral("CreateInstance"));
        CreateInstance->resize(306, 210);
        gridLayout = new QGridLayout(CreateInstance);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        frame = new QFrame(CreateInstance);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setSpacing(5);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(5, 5, 5, 5);
        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMaximumSize(QSize(150, 16777215));

        gridLayout_2->addWidget(label_2, 2, 0, 1, 1);

        portNumber = new QLineEdit(frame);
        portNumber->setObjectName(QStringLiteral("portNumber"));
        portNumber->setMaxLength(5);

        gridLayout_2->addWidget(portNumber, 3, 1, 1, 2);

        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setMaximumSize(QSize(150, 16777215));

        gridLayout_2->addWidget(label_3, 3, 0, 1, 1);

        isPublic = new QCheckBox(frame);
        isPublic->setObjectName(QStringLiteral("isPublic"));

        gridLayout_2->addWidget(isPublic, 4, 0, 1, 3);

        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setMaximumSize(QSize(150, 16777215));

        gridLayout_2->addWidget(label, 1, 0, 1, 1);

        gameName = new QComboBox(frame);
        gameName->setObjectName(QStringLiteral("gameName"));

        gridLayout_2->addWidget(gameName, 1, 1, 1, 2);

        serverName = new QLineEdit(frame);
        serverName->setObjectName(QStringLiteral("serverName"));
        serverName->setMaxLength(32);

        gridLayout_2->addWidget(serverName, 2, 1, 1, 2);

        close = new QPushButton(frame);
        close->setObjectName(QStringLiteral("close"));
        close->setAutoDefault(false);

        gridLayout_2->addWidget(close, 5, 2, 1, 1);

        initializeServer = new QPushButton(frame);
        initializeServer->setObjectName(QStringLiteral("initializeServer"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(initializeServer->sizePolicy().hasHeightForWidth());
        initializeServer->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(initializeServer, 5, 0, 1, 1);

        oldServers = new QComboBox(frame);
        oldServers->setObjectName(QStringLiteral("oldServers"));

        gridLayout_2->addWidget(oldServers, 0, 1, 1, 2);

        label_4 = new QLabel(frame);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_2->addWidget(label_4, 0, 0, 1, 1);


        gridLayout->addWidget(frame, 0, 0, 1, 1);


        retranslateUi(CreateInstance);

        initializeServer->setDefault(false);


        QMetaObject::connectSlotsByName(CreateInstance);
    } // setupUi

    void retranslateUi(QDialog *CreateInstance)
    {
        CreateInstance->setWindowTitle(QApplication::translate("CreateInstance", "Create Server:", Q_NULLPTR));
        label_2->setText(QApplication::translate("CreateInstance", "Server Name:", Q_NULLPTR));
        portNumber->setText(QApplication::translate("CreateInstance", "8888", Q_NULLPTR));
        label_3->setText(QApplication::translate("CreateInstance", "Port:", Q_NULLPTR));
        isPublic->setText(QApplication::translate("CreateInstance", "Register as Public Server", Q_NULLPTR));
        label->setText(QApplication::translate("CreateInstance", "Game:", Q_NULLPTR));
        gameName->clear();
        gameName->insertItems(0, QStringList()
         << QApplication::translate("CreateInstance", "Well of Souls", Q_NULLPTR)
         << QApplication::translate("CreateInstance", "Arcadia", Q_NULLPTR)
         << QApplication::translate("CreateInstance", "Rocket Club", Q_NULLPTR)
         << QApplication::translate("CreateInstance", "Warpath", Q_NULLPTR)
        );
        serverName->setText(QApplication::translate("CreateInstance", "AHitB ReMix Server", Q_NULLPTR));
        close->setText(QApplication::translate("CreateInstance", "Cancel", Q_NULLPTR));
        initializeServer->setText(QApplication::translate("CreateInstance", "Create Server", Q_NULLPTR));
        label_4->setText(QApplication::translate("CreateInstance", "Previous Servers:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CreateInstance: public Ui_CreateInstance {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEINSTANCE_H
