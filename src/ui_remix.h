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
    QCheckBox *checkBox;
    QLineEdit *lineEdit;
    QLabel *label;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QLineEdit *lineEdit_2;
    QCheckBox *checkBox_2;
    QPushButton *pushButton_2;
    QPushButton *openSysMessages;
    QPushButton *openBanIP;
    QPushButton *openBannedSernums;
    QPushButton *pushButton;
    QLabel *label_3;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QLabel *label_7;
    QLabel *label_5;
    QLabel *label_9;
    QLabel *label_6;
    QLabel *label_8;
    QLabel *label_4;
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
        checkBox = new QCheckBox(frame);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(checkBox->sizePolicy().hasHeightForWidth());
        checkBox->setSizePolicy(sizePolicy);
        checkBox->setMaximumSize(QSize(16777215, 20));

        gridLayout_3->addWidget(checkBox, 0, 0, 1, 2);

        lineEdit = new QLineEdit(frame);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setMinimumSize(QSize(590, 20));
        lineEdit->setMaximumSize(QSize(16777215, 20));

        gridLayout_3->addWidget(lineEdit, 0, 2, 1, 1);

        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setMaximumSize(QSize(16777215, 20));

        gridLayout_3->addWidget(label, 0, 3, 1, 1);

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
        lineEdit_2 = new QLineEdit(groupBox_2);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        sizePolicy.setHeightForWidth(lineEdit_2->sizePolicy().hasHeightForWidth());
        lineEdit_2->setSizePolicy(sizePolicy);
        lineEdit_2->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(lineEdit_2, 0, 0, 1, 1);

        checkBox_2 = new QCheckBox(groupBox_2);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));
        sizePolicy.setHeightForWidth(checkBox_2->sizePolicy().hasHeightForWidth());
        checkBox_2->setSizePolicy(sizePolicy);
        checkBox_2->setMaximumSize(QSize(16777215, 20));

        gridLayout_4->addWidget(checkBox_2, 0, 1, 1, 1);

        pushButton_2 = new QPushButton(groupBox_2);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pushButton_2->sizePolicy().hasHeightForWidth());
        pushButton_2->setSizePolicy(sizePolicy2);
        pushButton_2->setMaximumSize(QSize(16777215, 23));

        gridLayout_4->addWidget(pushButton_2, 1, 0, 1, 2);

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

        pushButton = new QPushButton(groupBox_2);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        sizePolicy2.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy2);
        pushButton->setMaximumSize(QSize(16777215, 23));

        gridLayout_4->addWidget(pushButton, 4, 0, 1, 2);


        gridLayout_3->addWidget(groupBox_2, 1, 0, 2, 1);

        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));
        sizePolicy2.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy2);
        label_3->setMinimumSize(QSize(660, 20));
        label_3->setMaximumSize(QSize(16777215, 20));

        gridLayout_3->addWidget(label_3, 1, 1, 1, 3);

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
        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QStringLiteral("label_7"));
        sizePolicy.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy);
        label_7->setMaximumSize(QSize(16777215, 20));

        gridLayout_2->addWidget(label_7, 3, 0, 1, 1);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));
        sizePolicy.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy);
        label_5->setMaximumSize(QSize(16777215, 20));

        gridLayout_2->addWidget(label_5, 1, 0, 1, 1);

        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QStringLiteral("label_9"));
        sizePolicy.setHeightForWidth(label_9->sizePolicy().hasHeightForWidth());
        label_9->setSizePolicy(sizePolicy);
        label_9->setMaximumSize(QSize(16777215, 20));

        gridLayout_2->addWidget(label_9, 5, 0, 1, 1);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));
        sizePolicy.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy);
        label_6->setMaximumSize(QSize(16777215, 20));

        gridLayout_2->addWidget(label_6, 2, 0, 1, 1);

        label_8 = new QLabel(groupBox);
        label_8->setObjectName(QStringLiteral("label_8"));
        sizePolicy.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy);
        label_8->setMaximumSize(QSize(16777215, 20));

        gridLayout_2->addWidget(label_8, 4, 0, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));
        sizePolicy.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy);
        label_4->setMaximumSize(QSize(16777215, 20));

        gridLayout_2->addWidget(label_4, 0, 0, 1, 1);


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
        checkBox->setText(QApplication::translate("ReMix", "Register as a public server", 0));
        lineEdit->setText(QApplication::translate("ReMix", "Server Name", 0));
        label->setText(QApplication::translate("ReMix", "00:00:00", 0));
        groupBox_2->setTitle(QApplication::translate("ReMix", "TCP/IP Port#", 0));
        lineEdit_2->setText(QApplication::translate("ReMix", "8888", 0));
        checkBox_2->setText(QApplication::translate("ReMix", "!!", 0));
        pushButton_2->setText(QApplication::translate("ReMix", "Accept Calls", 0));
        openSysMessages->setText(QApplication::translate("ReMix", "Msg Options", 0));
        openBanIP->setText(QApplication::translate("ReMix", "Ban IP", 0));
        openBannedSernums->setText(QApplication::translate("ReMix", "SerNum", 0));
        pushButton->setText(QApplication::translate("ReMix", "User Comments", 0));
        label_3->setText(QApplication::translate("ReMix", "Listening for incoming calls to: 127.0.0.1:8888 ( Need port forward from 192.168.1.1:8888 ) ( Ping 000.00 MS )", 0));
        groupBox->setTitle(QApplication::translate("ReMix", "Stats", 0));
        label_7->setText(QApplication::translate("ReMix", "#IPDc: 0", 0));
        label_5->setText(QApplication::translate("ReMix", "#PktDC: 0", 0));
        label_9->setText(QApplication::translate("ReMix", "#OUT: 0 BD", 0));
        label_6->setText(QApplication::translate("ReMix", "#DupDc: 0", 0));
        label_8->setText(QApplication::translate("ReMix", "#IN: 0 BD", 0));
        label_4->setText(QApplication::translate("ReMix", "#Calls: 0", 0));
    } // retranslateUi

};

namespace Ui {
    class ReMix: public Ui_ReMix {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REMIX_H
