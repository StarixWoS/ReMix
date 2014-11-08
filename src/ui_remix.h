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
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ReMix
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_3;
    QCheckBox *isPublicServer;
    QLineEdit *serverName;
    QLabel *onlineTime;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QLineEdit *serverPort;
    QCheckBox *bangBang;
    QPushButton *enableNetworking;
    QPushButton *openSysMessages;
    QPushButton *openBanIP;
    QPushButton *openBannedSernums;
    QPushButton *openUserComments;
    QLabel *networkStatus;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QLabel *ipDCCount;
    QLabel *packetDCCount;
    QLabel *packetOUTBD;
    QLabel *duplicateCount;
    QLabel *packetINBD;
    QLabel *callCount;
    QSplitter *splitter;
    QTableView *playerView;
    QTableView *serverView;

    void setupUi(QMainWindow *ReMix)
    {
        if (ReMix->objectName().isEmpty())
            ReMix->setObjectName(QStringLiteral("ReMix"));
        ReMix->resize(835, 420);
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
        gridLayout_3 = new QGridLayout(frame);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        isPublicServer = new QCheckBox(frame);
        isPublicServer->setObjectName(QStringLiteral("isPublicServer"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(isPublicServer->sizePolicy().hasHeightForWidth());
        isPublicServer->setSizePolicy(sizePolicy);
        isPublicServer->setMaximumSize(QSize(16777215, 20));

        gridLayout_3->addWidget(isPublicServer, 0, 0, 1, 2);

        serverName = new QLineEdit(frame);
        serverName->setObjectName(QStringLiteral("serverName"));
        serverName->setMinimumSize(QSize(590, 20));
        serverName->setMaximumSize(QSize(16777215, 20));

        gridLayout_3->addWidget(serverName, 0, 2, 1, 1);

        onlineTime = new QLabel(frame);
        onlineTime->setObjectName(QStringLiteral("onlineTime"));
        sizePolicy.setHeightForWidth(onlineTime->sizePolicy().hasHeightForWidth());
        onlineTime->setSizePolicy(sizePolicy);
        onlineTime->setMaximumSize(QSize(16777215, 20));

        gridLayout_3->addWidget(onlineTime, 0, 3, 1, 1);

        groupBox_2 = new QGroupBox(frame);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy1);
        groupBox_2->setMaximumSize(QSize(130, 16777215));
        groupBox_2->setAlignment(Qt::AlignCenter);
        gridLayout_4 = new QGridLayout(groupBox_2);
        gridLayout_4->setSpacing(10);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gridLayout_4->setContentsMargins(10, 10, 10, 10);
        serverPort = new QLineEdit(groupBox_2);
        serverPort->setObjectName(QStringLiteral("serverPort"));
        sizePolicy.setHeightForWidth(serverPort->sizePolicy().hasHeightForWidth());
        serverPort->setSizePolicy(sizePolicy);
        serverPort->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(serverPort, 0, 0, 1, 1);

        bangBang = new QCheckBox(groupBox_2);
        bangBang->setObjectName(QStringLiteral("bangBang"));
        sizePolicy.setHeightForWidth(bangBang->sizePolicy().hasHeightForWidth());
        bangBang->setSizePolicy(sizePolicy);
        bangBang->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(bangBang, 0, 1, 1, 1);

        enableNetworking = new QPushButton(groupBox_2);
        enableNetworking->setObjectName(QStringLiteral("enableNetworking"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(enableNetworking->sizePolicy().hasHeightForWidth());
        enableNetworking->setSizePolicy(sizePolicy2);
        enableNetworking->setMaximumSize(QSize(16777215, 23));

        gridLayout_4->addWidget(enableNetworking, 1, 0, 1, 2);

        openSysMessages = new QPushButton(groupBox_2);
        openSysMessages->setObjectName(QStringLiteral("openSysMessages"));
        sizePolicy2.setHeightForWidth(openSysMessages->sizePolicy().hasHeightForWidth());
        openSysMessages->setSizePolicy(sizePolicy2);
        openSysMessages->setMaximumSize(QSize(16777215, 23));

        gridLayout_4->addWidget(openSysMessages, 2, 0, 1, 2);

        openBanIP = new QPushButton(groupBox_2);
        openBanIP->setObjectName(QStringLiteral("openBanIP"));
        openBanIP->setMaximumSize(QSize(16777215, 23));

        gridLayout_4->addWidget(openBanIP, 3, 0, 1, 1);

        openBannedSernums = new QPushButton(groupBox_2);
        openBannedSernums->setObjectName(QStringLiteral("openBannedSernums"));
        openBannedSernums->setMaximumSize(QSize(16777215, 23));

        gridLayout_4->addWidget(openBannedSernums, 3, 1, 1, 1);

        openUserComments = new QPushButton(groupBox_2);
        openUserComments->setObjectName(QStringLiteral("openUserComments"));
        sizePolicy2.setHeightForWidth(openUserComments->sizePolicy().hasHeightForWidth());
        openUserComments->setSizePolicy(sizePolicy2);
        openUserComments->setMaximumSize(QSize(16777215, 23));

        gridLayout_4->addWidget(openUserComments, 4, 0, 1, 2);


        gridLayout_3->addWidget(groupBox_2, 1, 0, 2, 1);

        networkStatus = new QLabel(frame);
        networkStatus->setObjectName(QStringLiteral("networkStatus"));
        sizePolicy2.setHeightForWidth(networkStatus->sizePolicy().hasHeightForWidth());
        networkStatus->setSizePolicy(sizePolicy2);
        networkStatus->setMinimumSize(QSize(660, 20));
        networkStatus->setMaximumSize(QSize(16777215, 20));

        gridLayout_3->addWidget(networkStatus, 1, 1, 1, 3);

        groupBox = new QGroupBox(frame);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy3);
        groupBox->setMaximumSize(QSize(130, 16777215));
        groupBox->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setSpacing(10);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(10, 10, 10, 10);
        ipDCCount = new QLabel(groupBox);
        ipDCCount->setObjectName(QStringLiteral("ipDCCount"));
        sizePolicy.setHeightForWidth(ipDCCount->sizePolicy().hasHeightForWidth());
        ipDCCount->setSizePolicy(sizePolicy);
        ipDCCount->setMaximumSize(QSize(16777215, 20));

        gridLayout_2->addWidget(ipDCCount, 3, 0, 1, 1);

        packetDCCount = new QLabel(groupBox);
        packetDCCount->setObjectName(QStringLiteral("packetDCCount"));
        sizePolicy.setHeightForWidth(packetDCCount->sizePolicy().hasHeightForWidth());
        packetDCCount->setSizePolicy(sizePolicy);
        packetDCCount->setMaximumSize(QSize(16777215, 20));

        gridLayout_2->addWidget(packetDCCount, 1, 0, 1, 1);

        packetOUTBD = new QLabel(groupBox);
        packetOUTBD->setObjectName(QStringLiteral("packetOUTBD"));
        sizePolicy.setHeightForWidth(packetOUTBD->sizePolicy().hasHeightForWidth());
        packetOUTBD->setSizePolicy(sizePolicy);
        packetOUTBD->setMaximumSize(QSize(16777215, 20));

        gridLayout_2->addWidget(packetOUTBD, 5, 0, 1, 1);

        duplicateCount = new QLabel(groupBox);
        duplicateCount->setObjectName(QStringLiteral("duplicateCount"));
        sizePolicy.setHeightForWidth(duplicateCount->sizePolicy().hasHeightForWidth());
        duplicateCount->setSizePolicy(sizePolicy);
        duplicateCount->setMaximumSize(QSize(16777215, 20));

        gridLayout_2->addWidget(duplicateCount, 2, 0, 1, 1);

        packetINBD = new QLabel(groupBox);
        packetINBD->setObjectName(QStringLiteral("packetINBD"));
        sizePolicy.setHeightForWidth(packetINBD->sizePolicy().hasHeightForWidth());
        packetINBD->setSizePolicy(sizePolicy);
        packetINBD->setMaximumSize(QSize(16777215, 20));

        gridLayout_2->addWidget(packetINBD, 4, 0, 1, 1);

        callCount = new QLabel(groupBox);
        callCount->setObjectName(QStringLiteral("callCount"));
        sizePolicy.setHeightForWidth(callCount->sizePolicy().hasHeightForWidth());
        callCount->setSizePolicy(sizePolicy);
        callCount->setMaximumSize(QSize(16777215, 20));

        gridLayout_2->addWidget(callCount, 0, 0, 1, 1);


        gridLayout_3->addWidget(groupBox, 3, 0, 1, 1);

        splitter = new QSplitter(frame);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Vertical);
        playerView = new QTableView(splitter);
        playerView->setObjectName(QStringLiteral("playerView"));
        splitter->addWidget(playerView);
        playerView->horizontalHeader()->setDefaultSectionSize(100);
        playerView->horizontalHeader()->setStretchLastSection(true);
        playerView->verticalHeader()->setVisible(false);
        playerView->verticalHeader()->setMinimumSectionSize(30);
        serverView = new QTableView(splitter);
        serverView->setObjectName(QStringLiteral("serverView"));
        QSizePolicy sizePolicy4(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(serverView->sizePolicy().hasHeightForWidth());
        serverView->setSizePolicy(sizePolicy4);
        splitter->addWidget(serverView);
        serverView->horizontalHeader()->setDefaultSectionSize(100);
        serverView->horizontalHeader()->setMinimumSectionSize(30);
        serverView->horizontalHeader()->setStretchLastSection(true);
        serverView->verticalHeader()->setVisible(false);
        serverView->verticalHeader()->setMinimumSectionSize(30);

        gridLayout_3->addWidget(splitter, 2, 1, 2, 3);


        gridLayout->addWidget(frame, 0, 0, 1, 1);

        ReMix->setCentralWidget(centralWidget);

        retranslateUi(ReMix);

        QMetaObject::connectSlotsByName(ReMix);
    } // setupUi

    void retranslateUi(QMainWindow *ReMix)
    {
        ReMix->setWindowTitle(QApplication::translate("ReMix", "ReMix", 0));
        isPublicServer->setText(QApplication::translate("ReMix", "Register as a public server", 0));
        serverName->setText(QApplication::translate("ReMix", "Server Name", 0));
        onlineTime->setText(QApplication::translate("ReMix", "00:00:00", 0));
        groupBox_2->setTitle(QApplication::translate("ReMix", "TCP/IP Port#", 0));
        serverPort->setText(QApplication::translate("ReMix", "8888", 0));
        bangBang->setText(QApplication::translate("ReMix", "!!", 0));
        enableNetworking->setText(QApplication::translate("ReMix", "Accept Calls", 0));
        openSysMessages->setText(QApplication::translate("ReMix", "Msg Options", 0));
        openBanIP->setText(QApplication::translate("ReMix", "Ban IP", 0));
        openBannedSernums->setText(QApplication::translate("ReMix", "SerNum", 0));
        openUserComments->setText(QApplication::translate("ReMix", "User Comments", 0));
        networkStatus->setText(QApplication::translate("ReMix", "Listening for incoming calls to: 127.0.0.1:8888 ( Need port forward from 192.168.1.1:8888 ) ( Ping 000.00 MS )", 0));
        groupBox->setTitle(QApplication::translate("ReMix", "Stats", 0));
        ipDCCount->setText(QApplication::translate("ReMix", "#IPDc: 0", 0));
        packetDCCount->setText(QApplication::translate("ReMix", "#PktDC: 0", 0));
        packetOUTBD->setText(QApplication::translate("ReMix", "#OUT: 0 BD", 0));
        duplicateCount->setText(QApplication::translate("ReMix", "#DupDc: 0", 0));
        packetINBD->setText(QApplication::translate("ReMix", "#IN: 0 BD", 0));
        callCount->setText(QApplication::translate("ReMix", "#Calls: 0", 0));
    } // retranslateUi

};

namespace Ui {
    class ReMix: public Ui_ReMix {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REMIX_H
