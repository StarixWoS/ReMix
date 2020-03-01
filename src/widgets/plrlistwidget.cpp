
//Class includes.
#include "plrlistwidget.hpp"
#include "ui_plrlistwidget.h"

//Required ReMix Widget includes.
#include "views/plrsortproxymodel.hpp"
#include "views/tbleventfilter.hpp"

//ReMix includes.
#include "serverinfo.hpp"
#include "settings.hpp"
#include "player.hpp"
#include "helper.hpp"
#include "logger.hpp"
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
    plrModel = new QStandardItemModel( 0, 8, nullptr );
    plrModel->setHeaderData( static_cast<int>( PlrCols::IPPort ),
                             Qt::Horizontal, "Player IP:Port" );
    plrModel->setHeaderData( static_cast<int>( PlrCols::SerNum ),
                             Qt::Horizontal, "SerNum" );
    plrModel->setHeaderData( static_cast<int>( PlrCols::Age ),
                             Qt::Horizontal, "Age" );
    plrModel->setHeaderData( static_cast<int>( PlrCols::Alias ),
                             Qt::Horizontal, "Alias" );
    plrModel->setHeaderData( static_cast<int>( PlrCols::Time ),
                             Qt::Horizontal, "Time" );
    plrModel->setHeaderData( static_cast<int>( PlrCols::BytesIn ),
                             Qt::Horizontal, "IN" );
    plrModel->setHeaderData( static_cast<int>( PlrCols::BytesOut ),
                             Qt::Horizontal, "OUT" );
    plrModel->setHeaderData( static_cast<int>( PlrCols::BioData ),
                             Qt::Horizontal, "BIO" );

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
    QModelIndex menuIndex{ plrProxy->mapToSource( ui->playerView->indexAt( pos ) ) };

    this->initContextMenu();
    if ( menuIndex.row() >= 0 )
    {
        Player* plr = server->getPlayer(
                          server->getQItemSlot( plrModel->item( menuIndex.row(), 0 ) ) );
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
    if ( menuTarget == nullptr )
        return;

    QString revoke{ "Your Remote Administrator privileges have been revoked by the Server Host. Please contact the Server Host "
                   "if you believe this was in error." };
    QString reinstated{ "Your Remote Administrator privelages have been partially reinstated by the Server Host." };

    QString sernum{ menuTarget->getSernumHex_s() };
    QString prompt{ "" };
    if ( !User::getIsAdmin( sernum ) )
    {
        QString title{ "Create Admin:" };
        prompt =  "Are you certain you want to MAKE [ %1 ] a Remote Admin? \r\n\r\nPlease make sure you trust [ %1 ] as this will "
                  "allow the them to utilize Admin commands that can remove the ability for other users to connect to the Server.";
        prompt = prompt.arg( Helper::serNumToIntStr( sernum ) );

        if ( Helper::confirmAction( this, title, prompt ) )
        {
            User::setAdminRank( sernum, GMRanks::GMaster );
            if ( User::getHasPassword( sernum ) )
                menuTarget->sendMessage( reinstated, false );
            else
                menuTarget->setNewAdminPwdRequested( true );
        }
    }
    else
    {
        QString title{ "Revoke Admin:" };
        prompt = "Are you certain you want to REVOKE [ %1 ]'s powers?";
        prompt = prompt.arg( Helper::serNumToIntStr( sernum ) );

        if ( Helper::confirmAction( this, title, prompt ) )
        {
            User::setAdminRank( sernum, GMRanks::User );
            menuTarget->setAdminPwdReceived( false );
            menuTarget->setAdminPwdRequested( false );
            menuTarget->sendMessage( revoke, false );
        }
    }

    menuTarget = nullptr;
}

void PlrListWidget::on_actionMuteNetwork_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QString inform{ "The Server Host has MUTED you. Reason: %1" };
    QString reason{ "Manual Mute; %1" };
    bool mute{ true };

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
        reason = reason.arg( User::requestReason( this ) );
        inform = inform.arg( reason );

        PunishDurations muteDuration{ User::requestDuration() };
        User::addMute( nullptr, menuTarget, reason, false, muteDuration );

        QString logMsg{ "%1: [ %2 ], [ %3 ]" };
        logMsg = logMsg.arg( reason )
                       .arg( menuTarget->getSernum_s() )
                       .arg( menuTarget->getBioData() );

        Logger::getInstance()->insertLog( server->getName(), logMsg,
                                          LogTypes::MUTE, true, true );

        menuTarget->sendMessage( inform, false );
        menuTarget->setNetworkMuted( mute );
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

        QString inform{ "The Server Host has disconnected you from the Server. Reason: %1" };
        QString reason{ "Manual Disconnect; %1" };

        if ( Helper::confirmAction( this, title, prompt ) )
        {
            reason = reason.arg( Helper::getDisconnectReason( this ) );
            inform = inform.arg( reason );

            menuTarget->sendMessage( inform, false );
            if ( sock->waitForBytesWritten() )
                menuTarget->setDisconnected( true, DCTypes::IPDC );

            QString logMsg{ "%1: [ %2 ], [ %3 ]" };
            logMsg = logMsg.arg( reason )
                           .arg( menuTarget->getSernum_s() )
                           .arg( menuTarget->getBioData() );

            Logger::getInstance()->insertLog( server->getName(), logMsg, LogTypes::DC, true, true );
        }
    }
    menuTarget = nullptr;
}

void PlrListWidget::on_actionBANISHUser_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QString title{ "Ban SerNum:" };
    QString prompt{ "Are you certain you want to BANISH User [ " % menuTarget->getSernum_s() % " ]?" };

    QString inform{ "The Server Host has BANISHED you. Reason: %1" };
    QString reason{ "Manual Banish; %1" };

    QTcpSocket* sock = menuTarget->getSocket();
    if ( sock != nullptr )
    {
        if ( Helper::confirmAction( this, title, prompt ) )
        {
            reason = reason.arg( User::requestReason( this ) );
            inform = inform.arg( reason );

            PunishDurations banDuration{ User::requestDuration() };
            User::addBan( nullptr, menuTarget, reason, false, banDuration );

            QString logMsg{ "%1: [ %2 ], [ %3 ]" };
            logMsg = logMsg.arg( reason )
                           .arg( menuTarget->getSernum_s() )
                           .arg( menuTarget->getBioData() );

            Logger::getInstance()->insertLog( server->getName(), logMsg, LogTypes::BAN, true, true );

            menuTarget->sendMessage( inform, false );
            if ( sock->waitForBytesWritten() )
                menuTarget->setDisconnected( true, DCTypes::IPDC );
        }
    }
    menuTarget = nullptr;
}
