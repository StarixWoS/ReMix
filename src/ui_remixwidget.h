/********************************************************************************
** Form generated from reading UI file 'remixwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
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
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QLabel *callCount;
    QLabel *packetDCCount;
    QLabel *dupDCCount;
    QLabel *ipDCCount;
    QLabel *packetINBD;
    QLabel *packetOUTBD;
    QPushButton *openBanDialog;
    QPushButton *openSysMessages;
    QPushButton *openRemoteAdmins;
    QLineEdit *serverPort;
    QPushButton *enableNetworking;
    QPushButton *openUserComments;
    QPushButton *openSettings;
    QCheckBox *isPublicServer;
    QLineEdit *serverName;
    QLabel *onlineTime;
    QLabel *networkStatus;
    QWidget *tmpWidget;

    void setupUi(QWidget *ReMixWidget)
    {
        if (ReMixWidget->objectName().isEmpty())
            ReMixWidget->setObjectName(QStringLiteral("ReMixWidget"));
        ReMixWidget->resize(241, 245);
        gridLayout = new QGridLayout(ReMixWidget);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        groupBox_2 = new QGroupBox(ReMixWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy);
        groupBox_2->setMinimumSize(QSize(130, 0));
        groupBox_2->setMaximumSize(QSize(130, 350));
        groupBox_2->setAlignment(Qt::AlignCenter);
        gridLayout_4 = new QGridLayout(groupBox_2);
        gridLayout_4->setSpacing(5);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gridLayout_4->setContentsMargins(5, 5, 5, 5);
        groupBox = new QGroupBox(groupBox_2);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Ignored);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy1);
        groupBox->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(10, 10, 10, 10);
        callCount = new QLabel(groupBox);
        callCount->setObjectName(QStringLiteral("callCount"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(callCount->sizePolicy().hasHeightForWidth());
        callCount->setSizePolicy(sizePolicy2);
        callCount->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(callCount);

        packetDCCount = new QLabel(groupBox);
        packetDCCount->setObjectName(QStringLiteral("packetDCCount"));
        sizePolicy2.setHeightForWidth(packetDCCount->sizePolicy().hasHeightForWidth());
        packetDCCount->setSizePolicy(sizePolicy2);
        packetDCCount->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(packetDCCount);

        dupDCCount = new QLabel(groupBox);
        dupDCCount->setObjectName(QStringLiteral("dupDCCount"));
        sizePolicy2.setHeightForWidth(dupDCCount->sizePolicy().hasHeightForWidth());
        dupDCCount->setSizePolicy(sizePolicy2);
        dupDCCount->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(dupDCCount);

        ipDCCount = new QLabel(groupBox);
        ipDCCount->setObjectName(QStringLiteral("ipDCCount"));
        sizePolicy2.setHeightForWidth(ipDCCount->sizePolicy().hasHeightForWidth());
        ipDCCount->setSizePolicy(sizePolicy2);
        ipDCCount->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(ipDCCount);

        packetINBD = new QLabel(groupBox);
        packetINBD->setObjectName(QStringLiteral("packetINBD"));
        sizePolicy2.setHeightForWidth(packetINBD->sizePolicy().hasHeightForWidth());
        packetINBD->setSizePolicy(sizePolicy2);
        packetINBD->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(packetINBD);

        packetOUTBD = new QLabel(groupBox);
        packetOUTBD->setObjectName(QStringLiteral("packetOUTBD"));
        sizePolicy2.setHeightForWidth(packetOUTBD->sizePolicy().hasHeightForWidth());
        packetOUTBD->setSizePolicy(sizePolicy2);
        packetOUTBD->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(packetOUTBD);


        gridLayout_4->addWidget(groupBox, 7, 0, 1, 2);

        openBanDialog = new QPushButton(groupBox_2);
        openBanDialog->setObjectName(QStringLiteral("openBanDialog"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(openBanDialog->sizePolicy().hasHeightForWidth());
        openBanDialog->setSizePolicy(sizePolicy3);
        openBanDialog->setMinimumSize(QSize(0, 20));
        openBanDialog->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(openBanDialog, 5, 0, 1, 2);

        openSysMessages = new QPushButton(groupBox_2);
        openSysMessages->setObjectName(QStringLiteral("openSysMessages"));
        sizePolicy3.setHeightForWidth(openSysMessages->sizePolicy().hasHeightForWidth());
        openSysMessages->setSizePolicy(sizePolicy3);
        openSysMessages->setMinimumSize(QSize(0, 20));
        openSysMessages->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(openSysMessages, 4, 0, 1, 2);

        openRemoteAdmins = new QPushButton(groupBox_2);
        openRemoteAdmins->setObjectName(QStringLiteral("openRemoteAdmins"));
        sizePolicy3.setHeightForWidth(openRemoteAdmins->sizePolicy().hasHeightForWidth());
        openRemoteAdmins->setSizePolicy(sizePolicy3);
        openRemoteAdmins->setMinimumSize(QSize(0, 20));
        openRemoteAdmins->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(openRemoteAdmins, 2, 0, 1, 2);

        serverPort = new QLineEdit(groupBox_2);
        serverPort->setObjectName(QStringLiteral("serverPort"));
        sizePolicy3.setHeightForWidth(serverPort->sizePolicy().hasHeightForWidth());
        serverPort->setSizePolicy(sizePolicy3);
        serverPort->setMinimumSize(QSize(0, 20));
        serverPort->setMaximumSize(QSize(16777215, 19));
        serverPort->setMaxLength(5);

        gridLayout_4->addWidget(serverPort, 0, 0, 1, 2);

        enableNetworking = new QPushButton(groupBox_2);
        enableNetworking->setObjectName(QStringLiteral("enableNetworking"));
        sizePolicy3.setHeightForWidth(enableNetworking->sizePolicy().hasHeightForWidth());
        enableNetworking->setSizePolicy(sizePolicy3);
        enableNetworking->setMinimumSize(QSize(0, 20));
        enableNetworking->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(enableNetworking, 1, 0, 1, 2);

        openUserComments = new QPushButton(groupBox_2);
        openUserComments->setObjectName(QStringLiteral("openUserComments"));
        sizePolicy3.setHeightForWidth(openUserComments->sizePolicy().hasHeightForWidth());
        openUserComments->setSizePolicy(sizePolicy3);
        openUserComments->setMinimumSize(QSize(0, 20));
        openUserComments->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(openUserComments, 6, 0, 1, 2);

        openSettings = new QPushButton(groupBox_2);
        openSettings->setObjectName(QStringLiteral("openSettings"));
        sizePolicy3.setHeightForWidth(openSettings->sizePolicy().hasHeightForWidth());
        openSettings->setSizePolicy(sizePolicy3);
        openSettings->setMinimumSize(QSize(0, 20));
        openSettings->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(openSettings, 3, 0, 1, 2);


        gridLayout->addWidget(groupBox_2, 1, 0, 2, 1);

        isPublicServer = new QCheckBox(ReMixWidget);
        isPublicServer->setObjectName(QStringLiteral("isPublicServer"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(isPublicServer->sizePolicy().hasHeightForWidth());
        isPublicServer->setSizePolicy(sizePolicy4);

        gridLayout->addWidget(isPublicServer, 0, 0, 1, 2);

        serverName = new QLineEdit(ReMixWidget);
        serverName->setObjectName(QStringLiteral("serverName"));

        gridLayout->addWidget(serverName, 0, 2, 1, 1);

        onlineTime = new QLabel(ReMixWidget);
        onlineTime->setObjectName(QStringLiteral("onlineTime"));
        sizePolicy2.setHeightForWidth(onlineTime->sizePolicy().hasHeightForWidth());
        onlineTime->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(onlineTime, 0, 3, 1, 1);

        networkStatus = new QLabel(ReMixWidget);
        networkStatus->setObjectName(QStringLiteral("networkStatus"));
        QSizePolicy sizePolicy5(QSizePolicy::Ignored, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(networkStatus->sizePolicy().hasHeightForWidth());
        networkStatus->setSizePolicy(sizePolicy5);

        gridLayout->addWidget(networkStatus, 1, 1, 1, 3);

        tmpWidget = new QWidget(ReMixWidget);
        tmpWidget->setObjectName(QStringLiteral("tmpWidget"));

        gridLayout->addWidget(tmpWidget, 2, 1, 1, 3);


        retranslateUi(ReMixWidget);

        QMetaObject::connectSlotsByName(ReMixWidget);
    } // setupUi

    void retranslateUi(QWidget *ReMixWidget)
    {
        ReMixWidget->setWindowTitle(QApplication::translate("ReMixWidget", "Form", 0));
        groupBox_2->setTitle(QApplication::translate("ReMixWidget", "TCP/IP Port#", 0));
        groupBox->setTitle(QApplication::translate("ReMixWidget", "Stats", 0));
        callCount->setText(QApplication::translate("ReMixWidget", "#Calls: 0", 0));
        packetDCCount->setText(QApplication::translate("ReMixWidget", "#Pkt-DC: 0", 0));
        dupDCCount->setText(QApplication::translate("ReMixWidget", "#Dup-DC: 0", 0));
        ipDCCount->setText(QApplication::translate("ReMixWidget", "#IP-DC: 0", 0));
        packetINBD->setText(QApplication::translate("ReMixWidget", "#IN: 0 BD", 0));
        packetOUTBD->setText(QApplication::translate("ReMixWidget", "#OUT: 0 BD", 0));
        openBanDialog->setText(QApplication::translate("ReMixWidget", "Bans", 0));
        openSysMessages->setText(QApplication::translate("ReMixWidget", "Messages", 0));
        openRemoteAdmins->setText(QApplication::translate("ReMixWidget", "Remote Admins", 0));
        serverPort->setText(QApplication::translate("ReMixWidget", "8888", 0));
        enableNetworking->setText(QApplication::translate("ReMixWidget", "Accept Calls", 0));
        openUserComments->setText(QApplication::translate("ReMixWidget", "User Comments", 0));
        openSettings->setText(QApplication::translate("ReMixWidget", "Settings", 0));
        isPublicServer->setText(QApplication::translate("ReMixWidget", "Register as a public server", 0));
        serverName->setText(QApplication::translate("ReMixWidget", "Server Name", 0));
        onlineTime->setText(QApplication::translate("ReMixWidget", "00:00:00", 0));
        networkStatus->setText(QApplication::translate("ReMixWidget", "Listening for incoming calls to: 127.0.0.1:8888 ( Need port forward from 192.168.1.1:8888 )", 0));
    } // retranslateUi

};

namespace Ui {
    class ReMixWidget: public Ui_ReMixWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REMIXWIDGET_H
