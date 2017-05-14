/********************************************************************************
** Form generated from reading UI file 'remixwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REMIXWIDGET_H
#define UI_REMIXWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ReMixWidget
{
public:
    QGridLayout *gridLayout;
    QLabel *networkStatus;
    QLabel *onlineTime;
    QWidget *tmpWidget;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QLabel *callCount;
    QLabel *pingCount;
    QLabel *packetDCCount;
    QLabel *dupDCCount;
    QLabel *ipDCCount;
    QLabel *packetINBD;
    QLabel *packetOUTBD;
    QLineEdit *serverPort;
    QLabel *label;
    QPushButton *openSettings;
    QPushButton *enableNetworking;
    QPushButton *openUserInfo;
    QPushButton *openUserComments;
    QCheckBox *isPublicServer;

    void setupUi(QWidget *ReMixWidget)
    {
        if (ReMixWidget->objectName().isEmpty())
            ReMixWidget->setObjectName(QStringLiteral("ReMixWidget"));
        ReMixWidget->resize(856, 398);
        ReMixWidget->setMinimumSize(QSize(0, 0));
        gridLayout = new QGridLayout(ReMixWidget);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        networkStatus = new QLabel(ReMixWidget);
        networkStatus->setObjectName(QStringLiteral("networkStatus"));
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(networkStatus->sizePolicy().hasHeightForWidth());
        networkStatus->setSizePolicy(sizePolicy);

        gridLayout->addWidget(networkStatus, 0, 0, 1, 2);

        onlineTime = new QLabel(ReMixWidget);
        onlineTime->setObjectName(QStringLiteral("onlineTime"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(onlineTime->sizePolicy().hasHeightForWidth());
        onlineTime->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(onlineTime, 0, 2, 1, 1);

        tmpWidget = new QWidget(ReMixWidget);
        tmpWidget->setObjectName(QStringLiteral("tmpWidget"));

        gridLayout->addWidget(tmpWidget, 1, 1, 2, 2);

        groupBox_2 = new QGroupBox(ReMixWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy2);
        groupBox_2->setMinimumSize(QSize(130, 0));
        groupBox_2->setMaximumSize(QSize(130, 350));
        groupBox_2->setAlignment(Qt::AlignCenter);
        groupBox_2->setCheckable(false);
        groupBox_2->setChecked(false);
        gridLayout_4 = new QGridLayout(groupBox_2);
        gridLayout_4->setSpacing(5);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gridLayout_4->setContentsMargins(5, 5, 5, 5);
        groupBox = new QGroupBox(groupBox_2);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Ignored);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy3);
        groupBox->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(10, 10, 10, 10);
        callCount = new QLabel(groupBox);
        callCount->setObjectName(QStringLiteral("callCount"));
        sizePolicy1.setHeightForWidth(callCount->sizePolicy().hasHeightForWidth());
        callCount->setSizePolicy(sizePolicy1);
        callCount->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(callCount);

        pingCount = new QLabel(groupBox);
        pingCount->setObjectName(QStringLiteral("pingCount"));

        verticalLayout->addWidget(pingCount);

        packetDCCount = new QLabel(groupBox);
        packetDCCount->setObjectName(QStringLiteral("packetDCCount"));
        sizePolicy1.setHeightForWidth(packetDCCount->sizePolicy().hasHeightForWidth());
        packetDCCount->setSizePolicy(sizePolicy1);
        packetDCCount->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(packetDCCount);

        dupDCCount = new QLabel(groupBox);
        dupDCCount->setObjectName(QStringLiteral("dupDCCount"));
        sizePolicy1.setHeightForWidth(dupDCCount->sizePolicy().hasHeightForWidth());
        dupDCCount->setSizePolicy(sizePolicy1);
        dupDCCount->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(dupDCCount);

        ipDCCount = new QLabel(groupBox);
        ipDCCount->setObjectName(QStringLiteral("ipDCCount"));
        sizePolicy1.setHeightForWidth(ipDCCount->sizePolicy().hasHeightForWidth());
        ipDCCount->setSizePolicy(sizePolicy1);
        ipDCCount->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(ipDCCount);

        packetINBD = new QLabel(groupBox);
        packetINBD->setObjectName(QStringLiteral("packetINBD"));
        sizePolicy1.setHeightForWidth(packetINBD->sizePolicy().hasHeightForWidth());
        packetINBD->setSizePolicy(sizePolicy1);
        packetINBD->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(packetINBD);

        packetOUTBD = new QLabel(groupBox);
        packetOUTBD->setObjectName(QStringLiteral("packetOUTBD"));
        sizePolicy1.setHeightForWidth(packetOUTBD->sizePolicy().hasHeightForWidth());
        packetOUTBD->setSizePolicy(sizePolicy1);
        packetOUTBD->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(packetOUTBD);


        gridLayout_4->addWidget(groupBox, 7, 0, 1, 3);

        serverPort = new QLineEdit(groupBox_2);
        serverPort->setObjectName(QStringLiteral("serverPort"));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(serverPort->sizePolicy().hasHeightForWidth());
        serverPort->setSizePolicy(sizePolicy4);
        serverPort->setMinimumSize(QSize(0, 20));
        serverPort->setMaximumSize(QSize(16777215, 19));
        serverPort->setMaxLength(5);

        gridLayout_4->addWidget(serverPort, 1, 1, 1, 2);

        label = new QLabel(groupBox_2);
        label->setObjectName(QStringLiteral("label"));
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);

        gridLayout_4->addWidget(label, 1, 0, 1, 1);

        openSettings = new QPushButton(groupBox_2);
        openSettings->setObjectName(QStringLiteral("openSettings"));
        sizePolicy4.setHeightForWidth(openSettings->sizePolicy().hasHeightForWidth());
        openSettings->setSizePolicy(sizePolicy4);
        openSettings->setMinimumSize(QSize(0, 20));
        openSettings->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(openSettings, 3, 0, 1, 3);

        enableNetworking = new QPushButton(groupBox_2);
        enableNetworking->setObjectName(QStringLiteral("enableNetworking"));
        sizePolicy4.setHeightForWidth(enableNetworking->sizePolicy().hasHeightForWidth());
        enableNetworking->setSizePolicy(sizePolicy4);
        enableNetworking->setMinimumSize(QSize(0, 20));
        enableNetworking->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(enableNetworking, 2, 0, 1, 3);

        openUserInfo = new QPushButton(groupBox_2);
        openUserInfo->setObjectName(QStringLiteral("openUserInfo"));
        sizePolicy4.setHeightForWidth(openUserInfo->sizePolicy().hasHeightForWidth());
        openUserInfo->setSizePolicy(sizePolicy4);
        openUserInfo->setMinimumSize(QSize(0, 20));
        openUserInfo->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(openUserInfo, 4, 0, 1, 3);

        openUserComments = new QPushButton(groupBox_2);
        openUserComments->setObjectName(QStringLiteral("openUserComments"));
        sizePolicy4.setHeightForWidth(openUserComments->sizePolicy().hasHeightForWidth());
        openUserComments->setSizePolicy(sizePolicy4);
        openUserComments->setMinimumSize(QSize(0, 20));
        openUserComments->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(openUserComments, 6, 0, 1, 3);

        isPublicServer = new QCheckBox(groupBox_2);
        isPublicServer->setObjectName(QStringLiteral("isPublicServer"));

        gridLayout_4->addWidget(isPublicServer, 0, 0, 1, 3);


        gridLayout->addWidget(groupBox_2, 1, 0, 2, 1);


        retranslateUi(ReMixWidget);

        QMetaObject::connectSlotsByName(ReMixWidget);
    } // setupUi

    void retranslateUi(QWidget *ReMixWidget)
    {
        ReMixWidget->setWindowTitle(QApplication::translate("ReMixWidget", "Form", 0));
        networkStatus->setText(QApplication::translate("ReMixWidget", "Listening for incoming calls to: 127.0.0.1:8888 ( Need port forward from 192.168.1.1:8888 )", 0));
        onlineTime->setText(QApplication::translate("ReMixWidget", "00:00:00", 0));
        groupBox_2->setTitle(QApplication::translate("ReMixWidget", "Server Info", 0));
        groupBox->setTitle(QApplication::translate("ReMixWidget", "Stats", 0));
        callCount->setText(QApplication::translate("ReMixWidget", "#Calls: 0", 0));
        pingCount->setText(QApplication::translate("ReMixWidget", "#Pings: 0", 0));
        packetDCCount->setText(QApplication::translate("ReMixWidget", "#Pkt-DC: 0", 0));
        dupDCCount->setText(QApplication::translate("ReMixWidget", "#Dup-DC: 0", 0));
        ipDCCount->setText(QApplication::translate("ReMixWidget", "#IP-DC: 0", 0));
        packetINBD->setText(QApplication::translate("ReMixWidget", "#IN: 0 BD", 0));
        packetOUTBD->setText(QApplication::translate("ReMixWidget", "#OUT: 0 BD", 0));
        serverPort->setText(QApplication::translate("ReMixWidget", "8888", 0));
        label->setText(QApplication::translate("ReMixWidget", "Port:", 0));
        openSettings->setText(QApplication::translate("ReMixWidget", "Settings", 0));
        enableNetworking->setText(QApplication::translate("ReMixWidget", "Accept Calls", 0));
        openUserInfo->setText(QApplication::translate("ReMixWidget", "User Information", 0));
        openUserComments->setText(QApplication::translate("ReMixWidget", "User Comments", 0));
        isPublicServer->setText(QApplication::translate("ReMixWidget", "Public Server", 0));
    } // retranslateUi

};

namespace Ui {
    class ReMixWidget: public Ui_ReMixWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REMIXWIDGET_H
