/********************************************************************************
** Form generated from reading UI file 'remix.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REMIX_H
#define UI_REMIX_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ReMix
{
public:
    QAction *actionSendMessage;
    QAction *actionMakeAdmin;
    QAction *actionBANISHIPAddress;
    QAction *actionBANISHSerNum;
    QAction *actionRevokeAdmin;
    QAction *actionDisconnectUser;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QCheckBox *isPublicServer;
    QLineEdit *serverName;
    QLabel *onlineTime;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QLabel *callCount;
    QLabel *packetDCCount;
    QLabel *duplicateCount;
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
    QLabel *networkStatus;
    QTableView *playerView;

    void setupUi(QMainWindow *ReMix)
    {
        if (ReMix->objectName().isEmpty())
            ReMix->setObjectName(QStringLiteral("ReMix"));
        ReMix->resize(666, 272);
        ReMix->setMinimumSize(QSize(0, 272));
        ReMix->setMaximumSize(QSize(16777215, 414));
        ReMix->setDockNestingEnabled(false);
        actionSendMessage = new QAction(ReMix);
        actionSendMessage->setObjectName(QStringLiteral("actionSendMessage"));
        actionMakeAdmin = new QAction(ReMix);
        actionMakeAdmin->setObjectName(QStringLiteral("actionMakeAdmin"));
        actionBANISHIPAddress = new QAction(ReMix);
        actionBANISHIPAddress->setObjectName(QStringLiteral("actionBANISHIPAddress"));
        actionBANISHSerNum = new QAction(ReMix);
        actionBANISHSerNum->setObjectName(QStringLiteral("actionBANISHSerNum"));
        actionRevokeAdmin = new QAction(ReMix);
        actionRevokeAdmin->setObjectName(QStringLiteral("actionRevokeAdmin"));
        actionDisconnectUser = new QAction(ReMix);
        actionDisconnectUser->setObjectName(QStringLiteral("actionDisconnectUser"));
        centralWidget = new QWidget(ReMix);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        frame = new QFrame(centralWidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setSpacing(5);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        isPublicServer = new QCheckBox(frame);
        isPublicServer->setObjectName(QStringLiteral("isPublicServer"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(isPublicServer->sizePolicy().hasHeightForWidth());
        isPublicServer->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(isPublicServer, 0, 0, 1, 2);

        serverName = new QLineEdit(frame);
        serverName->setObjectName(QStringLiteral("serverName"));

        gridLayout_2->addWidget(serverName, 0, 2, 1, 1);

        onlineTime = new QLabel(frame);
        onlineTime->setObjectName(QStringLiteral("onlineTime"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(onlineTime->sizePolicy().hasHeightForWidth());
        onlineTime->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(onlineTime, 0, 3, 1, 1);

        groupBox_2 = new QGroupBox(frame);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy2);
        groupBox_2->setMinimumSize(QSize(130, 0));
        groupBox_2->setMaximumSize(QSize(130, 350));
        groupBox_2->setAlignment(Qt::AlignCenter);
        gridLayout_4 = new QGridLayout(groupBox_2);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gridLayout_4->setHorizontalSpacing(10);
        gridLayout_4->setVerticalSpacing(5);
        gridLayout_4->setContentsMargins(10, 10, 10, 10);
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
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(10, 10, 10, 10);
        callCount = new QLabel(groupBox);
        callCount->setObjectName(QStringLiteral("callCount"));
        sizePolicy1.setHeightForWidth(callCount->sizePolicy().hasHeightForWidth());
        callCount->setSizePolicy(sizePolicy1);
        callCount->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(callCount);

        packetDCCount = new QLabel(groupBox);
        packetDCCount->setObjectName(QStringLiteral("packetDCCount"));
        sizePolicy1.setHeightForWidth(packetDCCount->sizePolicy().hasHeightForWidth());
        packetDCCount->setSizePolicy(sizePolicy1);
        packetDCCount->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(packetDCCount);

        duplicateCount = new QLabel(groupBox);
        duplicateCount->setObjectName(QStringLiteral("duplicateCount"));
        sizePolicy1.setHeightForWidth(duplicateCount->sizePolicy().hasHeightForWidth());
        duplicateCount->setSizePolicy(sizePolicy1);
        duplicateCount->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(duplicateCount);

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


        gridLayout_4->addWidget(groupBox, 7, 0, 1, 2);

        openBanDialog = new QPushButton(groupBox_2);
        openBanDialog->setObjectName(QStringLiteral("openBanDialog"));
        QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(openBanDialog->sizePolicy().hasHeightForWidth());
        openBanDialog->setSizePolicy(sizePolicy4);
        openBanDialog->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(openBanDialog, 5, 0, 1, 2);

        openSysMessages = new QPushButton(groupBox_2);
        openSysMessages->setObjectName(QStringLiteral("openSysMessages"));
        QSizePolicy sizePolicy5(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(openSysMessages->sizePolicy().hasHeightForWidth());
        openSysMessages->setSizePolicy(sizePolicy5);
        openSysMessages->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(openSysMessages, 4, 0, 1, 2);

        openRemoteAdmins = new QPushButton(groupBox_2);
        openRemoteAdmins->setObjectName(QStringLiteral("openRemoteAdmins"));
        sizePolicy4.setHeightForWidth(openRemoteAdmins->sizePolicy().hasHeightForWidth());
        openRemoteAdmins->setSizePolicy(sizePolicy4);
        openRemoteAdmins->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(openRemoteAdmins, 2, 0, 1, 2);

        serverPort = new QLineEdit(groupBox_2);
        serverPort->setObjectName(QStringLiteral("serverPort"));
        QSizePolicy sizePolicy6(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(serverPort->sizePolicy().hasHeightForWidth());
        serverPort->setSizePolicy(sizePolicy6);
        serverPort->setMaximumSize(QSize(16777215, 19));
        serverPort->setMaxLength(5);

        gridLayout_4->addWidget(serverPort, 0, 0, 1, 2);

        enableNetworking = new QPushButton(groupBox_2);
        enableNetworking->setObjectName(QStringLiteral("enableNetworking"));
        sizePolicy5.setHeightForWidth(enableNetworking->sizePolicy().hasHeightForWidth());
        enableNetworking->setSizePolicy(sizePolicy5);
        enableNetworking->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(enableNetworking, 1, 0, 1, 2);

        openUserComments = new QPushButton(groupBox_2);
        openUserComments->setObjectName(QStringLiteral("openUserComments"));
        sizePolicy5.setHeightForWidth(openUserComments->sizePolicy().hasHeightForWidth());
        openUserComments->setSizePolicy(sizePolicy5);
        openUserComments->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(openUserComments, 6, 0, 1, 2);

        openSettings = new QPushButton(groupBox_2);
        openSettings->setObjectName(QStringLiteral("openSettings"));
        openSettings->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(openSettings, 3, 0, 1, 2);


        gridLayout_2->addWidget(groupBox_2, 1, 0, 2, 1);

        networkStatus = new QLabel(frame);
        networkStatus->setObjectName(QStringLiteral("networkStatus"));
        QSizePolicy sizePolicy7(QSizePolicy::Ignored, QSizePolicy::Fixed);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(networkStatus->sizePolicy().hasHeightForWidth());
        networkStatus->setSizePolicy(sizePolicy7);

        gridLayout_2->addWidget(networkStatus, 1, 1, 1, 3);

        playerView = new QTableView(frame);
        playerView->setObjectName(QStringLiteral("playerView"));
        playerView->setContextMenuPolicy(Qt::CustomContextMenu);
        playerView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        playerView->setAlternatingRowColors(true);
        playerView->setSelectionMode(QAbstractItemView::SingleSelection);
        playerView->setSelectionBehavior(QAbstractItemView::SelectRows);
        playerView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        playerView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        playerView->setSortingEnabled(true);
        playerView->setCornerButtonEnabled(false);
        playerView->horizontalHeader()->setDefaultSectionSize(100);
        playerView->horizontalHeader()->setStretchLastSection(false);
        playerView->verticalHeader()->setVisible(false);
        playerView->verticalHeader()->setDefaultSectionSize(25);
        playerView->verticalHeader()->setMinimumSectionSize(25);

        gridLayout_2->addWidget(playerView, 2, 1, 1, 3);


        gridLayout->addWidget(frame, 0, 1, 1, 1);

        ReMix->setCentralWidget(centralWidget);

        retranslateUi(ReMix);

        QMetaObject::connectSlotsByName(ReMix);
    } // setupUi

    void retranslateUi(QMainWindow *ReMix)
    {
        ReMix->setWindowTitle(QApplication::translate("ReMix", "ReMix", 0));
        actionSendMessage->setText(QApplication::translate("ReMix", "Send Message", 0));
        actionMakeAdmin->setText(QApplication::translate("ReMix", "Make Admin", 0));
        actionBANISHIPAddress->setText(QApplication::translate("ReMix", "BANISH IP Address", 0));
        actionBANISHSerNum->setText(QApplication::translate("ReMix", "BANISH SerNum", 0));
        actionRevokeAdmin->setText(QApplication::translate("ReMix", "Revoke Admin", 0));
        actionDisconnectUser->setText(QApplication::translate("ReMix", "Disconnect User", 0));
        isPublicServer->setText(QApplication::translate("ReMix", "Register as a public server", 0));
        serverName->setText(QApplication::translate("ReMix", "Server Name", 0));
        onlineTime->setText(QApplication::translate("ReMix", "00:00:00", 0));
        groupBox_2->setTitle(QApplication::translate("ReMix", "TCP/IP Port#", 0));
        groupBox->setTitle(QApplication::translate("ReMix", "Stats", 0));
        callCount->setText(QApplication::translate("ReMix", "#Calls: 0", 0));
        packetDCCount->setText(QApplication::translate("ReMix", "#PktDC: 0", 0));
        duplicateCount->setText(QApplication::translate("ReMix", "#DupDc: 0", 0));
        ipDCCount->setText(QApplication::translate("ReMix", "#IPDc: 0", 0));
        packetINBD->setText(QApplication::translate("ReMix", "#IN: 0 BD", 0));
        packetOUTBD->setText(QApplication::translate("ReMix", "#OUT: 0 BD", 0));
        openBanDialog->setText(QApplication::translate("ReMix", "Bans", 0));
        openSysMessages->setText(QApplication::translate("ReMix", "Messages", 0));
        openRemoteAdmins->setText(QApplication::translate("ReMix", "Remote Admins", 0));
        serverPort->setText(QApplication::translate("ReMix", "8888", 0));
        enableNetworking->setText(QApplication::translate("ReMix", "Accept Calls", 0));
        openUserComments->setText(QApplication::translate("ReMix", "User Comments", 0));
        openSettings->setText(QApplication::translate("ReMix", "Settings", 0));
        networkStatus->setText(QApplication::translate("ReMix", "Listening for incoming calls to: 127.0.0.1:8888 ( Need port forward from 192.168.1.1:8888 )", 0));
    } // retranslateUi

};

namespace Ui {
    class ReMix: public Ui_ReMix {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REMIX_H
