
//Class includes.
#include "plrlistwidget.hpp"
#include "ui_plrlistwidget.h"

//Required ReMix Widget includes.
#include "tblview/plrsortproxymodel.hpp"
#include "tblview/tbleventfilter.hpp"

//ReMix includes.
#include "serverinfo.hpp"
#include "settings.hpp"
#include "player.hpp"
#include "helper.hpp"
#include "user.hpp"

//Qt Includes.
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTcpSocket>
#include <QtCore>
#include <QMenu>

PlrListWidget::PlrListWidget(QWidget* parent, ServerInfo* svr) :
    QWidget(parent),
    ui(new Ui::PlrListWidget)
{
    ui->setupUi(this);

    server = svr;

    //Create our Context Menus
    contextMenu = new QMenu( this );

    //Setup the PlayerInfo TableView.
    plrModel = new QStandardItemModel( 0, 8, 0 );
    plrModel->setHeaderData( 0, Qt::Horizontal, "Player IP:Port" );
    plrModel->setHeaderData( 1, Qt::Horizontal, "SerNum" );
    plrModel->setHeaderData( 2, Qt::Horizontal, "Age" );
    plrModel->setHeaderData( 3, Qt::Horizontal, "Alias" );
    plrModel->setHeaderData( 4, Qt::Horizontal, "Time" );
    plrModel->setHeaderData( 5, Qt::Horizontal, "IN" );
    plrModel->setHeaderData( 6, Qt::Horizontal, "OUT" );
    plrModel->setHeaderData( 7, Qt::Horizontal, "BIO" );

    //Proxy model to support sorting without actually
    //altering the underlying model
    plrProxy = new PlrSortProxyModel();
    plrProxy->setDynamicSortFilter( true );
    plrProxy->setSourceModel( plrModel );
    plrProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->playerView->setModel( plrProxy );

    //Install Event Filter to enable Row-Deslection.
    ui->playerView->viewport()->installEventFilter(
                                    new TblEventFilter( ui->playerView,
                                                        plrProxy ) );
}

PlrListWidget::~PlrListWidget()
{
    contextMenu->deleteLater();

    plrModel->deleteLater();
    plrProxy->deleteLater();

    delete ui;
}

QStandardItemModel* PlrListWidget::getPlrModel() const
{
    return plrModel;
}

void PlrListWidget::resizeColumns()
{
    ui->playerView->resizeColumnsToContents();
}

void PlrListWidget::initContextMenu()
{
    contextMenu->clear();

    ui->actionSendMessage->setText( "Send Message" );
    contextMenu->addAction( ui->actionSendMessage );
    contextMenu->addAction( ui->actionMakeAdmin );
    contextMenu->addAction( ui->actionMuteNetwork );
    contextMenu->addAction( ui->actionDisconnectUser );
    contextMenu->addAction( ui->actionBANISHUser );

    contextMenu->insertSeparator( ui->actionMuteNetwork );
}

void PlrListWidget::on_playerView_customContextMenuRequested(const QPoint& pos)
{
    QModelIndex menuIndex = plrProxy->mapToSource(
                                ui->playerView->indexAt( pos ) );

    this->initContextMenu();
    if ( menuIndex.row() >= 0 )
    {
        Player* plr = server->getPlayer(
                          server->getQItemSlot(
                              plrModel->item( menuIndex.row(), 0 ) ) );
        if ( plr != nullptr )
            menuTarget = plr;

        if ( menuTarget != nullptr )
        {
            if ( menuTarget->getIsAdmin() )
                ui->actionMakeAdmin->setText( "Revoke Admin" );
            else
                ui->actionMakeAdmin->setText( "Make Admin" );

            if ( menuTarget->getNetworkMuted() )
                ui->actionMuteNetwork->setText( "Un-Mute Network" );
            else
                ui->actionMuteNetwork->setText( "Mute Network" );
        }

        contextMenu->popup( ui->playerView->viewport()->mapToGlobal( pos ) );
    }
    else
    {
        contextMenu->removeAction( ui->actionSendMessage );
        contextMenu->removeAction( ui->actionMakeAdmin );
        contextMenu->removeAction( ui->actionMuteNetwork );
        contextMenu->removeAction( ui->actionDisconnectUser );
        contextMenu->removeAction( ui->actionBANISHUser );
    }
    contextMenu->popup( ui->playerView->viewport()->mapToGlobal( pos ) );
}

void PlrListWidget::on_actionSendMessage_triggered()
{
    if ( menuTarget != nullptr )
    {
        menuTarget->sendMessage();
    }
    menuTarget = nullptr;
}

void PlrListWidget::on_actionMakeAdmin_triggered()
{
    bool send{ false };
    if ( menuTarget == nullptr )
        return;

    QString revoke{ "Your Remote Administrator privileges have been REVOKED "
                    "by either the Server Host. Please contact the Server Host "
                    "if you believe this was in error." };

    QString sernum{ menuTarget->getSernumHex_s() };
    if ( !User::getIsAdmin( sernum ) )
    {
        QString title{ "Create Admin:" };
        QString prompt{ "Are you certain you want to MAKE [ %1 ] a Remote Admin?"
                        "\r\n\r\nPlease make sure you trust [ %2 ] as this will "
                        "allow the them to utilize Admin commands that can remove "
                        "the ability for other users to connect to the Server." };
        prompt = prompt.arg( sernum )
                       .arg( sernum );

        if ( Helper::confirmAction( this, title, prompt ) )
        {
            User::setAdminRank( sernum, User::rGAMEMASTER );
            menuTarget->setNewAdminPwdRequested( true );
        }
    }
    else
    {
        QString title{ "Revoke Admin:" };
        QString prompt{ "Are you certain you want to REVOKE [ "
                       % Helper::serNumToIntStr( sernum )
                       % " ]'s powers?" };

        if ( Helper::confirmAction( this, title, prompt ) )
        {
            User::setAdminRank( sernum, User::rUSER );
            send = true;
        }
    }

    if ( send )
        menuTarget->sendMessage( revoke );

    menuTarget = nullptr;
}

void PlrListWidget::on_actionMuteNetwork_triggered()
{
    bool mute{ true };
    if ( menuTarget != nullptr )
    {
        QString title{ "%1 User:" };
        QString prompt{ "Are you certain you want to %1 [ %2 ]'s Network?" };
        if ( menuTarget->getNetworkMuted() )
        {
            title = title.arg( "Un-Mute" );
            prompt = prompt.arg( "Un-Mute" );

            mute = false;
        }
        else
        {
            title = title.arg( "Mute" );
            prompt = prompt.arg( "Mute" );
        }
        prompt = prompt.arg( menuTarget->getSernum_s() );

        if ( Helper::confirmAction( this, title, prompt ) )
        {
            QString msg{ "Manual %1 of [ %2 ] by Server Owner." };
                    msg = msg.arg( mute ? "Mute" : "UnMute" )
                             .arg( menuTarget->getSernum_s() );
            menuTarget->setNetworkMuted( mute, msg );
        }
    }
    menuTarget = nullptr;
}

void PlrListWidget::on_actionDisconnectUser_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QTcpSocket* sock = menuTarget->getSocket();
    if ( sock != nullptr )
    {
        QString title{ "Disconnect User:" };
        QString prompt{ "Are you certain you want to DISCONNECT [ " %
                        menuTarget->getSernum_s() % " ]?" };

        QString inform{ "The Server Host has disconnected you from the Server. "
                        "Reason: %1" };
        QString reason{ "Manual Disconnect; %1" };

        if ( Helper::confirmAction( this, title, prompt ) )
        {
            reason = reason.arg( Helper::getDisconnectReason( this ) );
            inform = inform.arg( reason );

            menuTarget->sendMessage( inform );
            if ( sock->waitForBytesWritten() )
            {
                menuTarget->setDisconnected( true );
                server->setIpDc( server->getIpDc() + 1 );
            }

            if ( Settings::getLogFiles() )
            {
                QString log{ "logs/DCLog.txt" };
                QString logMsg{ "%1: [ %2 ], [ %3 ]" };
                logMsg = logMsg.arg( reason )
                         .arg( menuTarget->getSernum_s() )
                         .arg( menuTarget->getBioData() );
                Helper::logToFile( log, logMsg, true, true );
            }
        }
    }
    menuTarget = nullptr;
}

void PlrListWidget::on_actionBANISHUser_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QString title{ "Ban SerNum:" };
    QString prompt{ "Are you certain you want to BANISH User [ "
                  % menuTarget->getSernum_s() % " ]?" };

    QString inform{ "The Server Host has BANISHED you. Reason: %1" };
    QString reason{ "Manual Banish; %1" };

    QTcpSocket* sock = menuTarget->getSocket();
    if ( sock != nullptr )
    {
        if ( Helper::confirmAction( this, title, prompt ) )
        {
            reason = reason.arg( Helper::getBanishReason( this ) );
            inform = inform.arg( reason );

            User::addBan( nullptr, menuTarget, reason );
            menuTarget->sendMessage( inform );
            if ( sock->waitForBytesWritten() )
            {
                menuTarget->setDisconnected( true );
                server->setIpDc( server->getIpDc() + 1 );
            }

            if ( Settings::getLogFiles() )
            {
                QString log{ "logs/BanLog.txt" };
                QString logMsg{ "%1: [ %2 ], [ %3 ]" };
                        logMsg = logMsg.arg( reason )
                                       .arg( menuTarget->getSernum_s() )
                                       .arg( menuTarget->getBioData() );
                Helper::logToFile( log, logMsg, true, true );
            }
        }
    }
    menuTarget = nullptr;
}
