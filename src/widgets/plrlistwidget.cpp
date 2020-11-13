
//Class includes.
#include "plrlistwidget.hpp"
#include "ui_plrlistwidget.h"

//Required ReMix Widget includes.
#include "views/plrsortproxymodel.hpp"
#include "views/tbleventfilter.hpp"

//ReMix includes.
#include "serverinfo.hpp"
#include "settings.hpp"
#include "sendmsg.hpp"
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

    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &PlrListWidget::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );

    server = svr;

    //Create our Context Menus
    contextMenu = new QMenu( this );

    //Setup the PlayerInfo TableView.
    plrModel = new QStandardItemModel( 0, 8, nullptr );
    plrModel->setHeaderData( static_cast<int>( PlrCols::IPPort ), Qt::Horizontal, "Player IP:Port" );
    plrModel->setHeaderData( static_cast<int>( PlrCols::SerNum ), Qt::Horizontal, "SerNum" );
    plrModel->setHeaderData( static_cast<int>( PlrCols::BytesOut ), Qt::Horizontal, "OUT" );
    plrModel->setHeaderData( static_cast<int>( PlrCols::BioData ), Qt::Horizontal, "BIO" );
    plrModel->setHeaderData( static_cast<int>( PlrCols::Alias ), Qt::Horizontal, "Alias" );
    plrModel->setHeaderData( static_cast<int>( PlrCols::BytesIn ), Qt::Horizontal, "IN" );
    plrModel->setHeaderData( static_cast<int>( PlrCols::Time ), Qt::Horizontal, "Time" );
    plrModel->setHeaderData( static_cast<int>( PlrCols::Age ), Qt::Horizontal, "Age" );

    //Proxy model to support sorting without actually altering the underlying model
    plrProxy = new PlrSortProxyModel();
    plrProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    plrProxy->setDynamicSortFilter( true );
    plrProxy->setSourceModel( plrModel );
    ui->playerView->setModel( plrProxy );

    ui->playerView->setColumnHidden( static_cast<int>( PlrCols::BioData ), true );
    ui->playerView->setColumnWidth( static_cast<int>( PlrCols::BytesOut ), 120 );
    ui->playerView->setColumnWidth( static_cast<int>( PlrCols::BytesIn ), 120 );
    ui->playerView->setColumnWidth( static_cast<int>( PlrCols::IPPort ), 130 );
    ui->playerView->setColumnWidth( static_cast<int>( PlrCols::SerNum ), 100 );
    ui->playerView->setColumnWidth( static_cast<int>( PlrCols::Alias ), 70 );
    ui->playerView->setColumnWidth( static_cast<int>( PlrCols::Time ), 50 );
    ui->playerView->setColumnWidth( static_cast<int>( PlrCols::Age ), 80 );
    ui->playerView->horizontalHeader()->setStretchLastSection( true );

    //Install Event Filter to enable Row-Deslection.
    tblEvFilter = new TblEventFilter( ui->playerView );
    ui->playerView->viewport()->installEventFilter( tblEvFilter );
}

PlrListWidget::~PlrListWidget()
{
    if ( messageDialog != nullptr )
    {
        if ( messageDialog->isVisible() )
        {
            messageDialog->close();
        }
        messageDialog->disconnect();
        messageDialog->deleteLater();
    }

    contextMenu->deleteLater();

    tblEvFilter->deleteLater();
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
    //ui->playerView->resizeColumnsToContents();
}

void PlrListWidget::initContextMenu()
{
    contextMenu->clear();

    ui->actionSendMessage->setText( "Send Message" );

    contextMenu->addAction( ui->actionDisconnectUser );
    contextMenu->addAction( ui->actionSendMessage );
    contextMenu->addAction( ui->actionMuteNetwork );
    contextMenu->addAction( ui->actionBANISHUser );
    contextMenu->addAction( ui->actionMakeAdmin );

    contextMenu->insertSeparator( ui->actionMuteNetwork );
}

void PlrListWidget::on_playerView_customContextMenuRequested(const QPoint& pos)
{
    QModelIndex menuIndex{ plrProxy->mapToSource( ui->playerView->indexAt( pos ) ) };

    this->initContextMenu();
    if ( menuIndex.row() >= 0 )
    {
        Player* plr{ server->getPlayer( server->getQItemSlot( plrModel->item( menuIndex.row(), 0 ) ) ) };
        if ( plr != nullptr )
            menuTarget = plr;

        if ( menuTarget != nullptr )
        {
            if ( menuTarget->getIsMuted() )
                ui->actionMuteNetwork->setText( "Un-Mute Network" );
            else
                ui->actionMuteNetwork->setText( "Mute Network" );

            if ( menuTarget->getIsAdmin() )
                ui->actionMakeAdmin->setText( "Revoke Admin" );
            else
                ui->actionMakeAdmin->setText( "Make Admin" );
        }

        contextMenu->popup( ui->playerView->viewport()->mapToGlobal( pos ) );
    }
    else
    {
        contextMenu->removeAction( ui->actionDisconnectUser );
        contextMenu->removeAction( ui->actionSendMessage );
        contextMenu->removeAction( ui->actionMuteNetwork );
        contextMenu->removeAction( ui->actionBANISHUser );
        contextMenu->removeAction( ui->actionMakeAdmin );
    }
    contextMenu->popup( ui->playerView->viewport()->mapToGlobal( pos ) );
}

void PlrListWidget::on_actionSendMessage_triggered()
{
    if ( menuTarget != nullptr )
    {
        if ( messageDialog == nullptr )
        {
            messageDialog = new SendMsg( menuTarget->getSernum_s() );
            QObject::connect( messageDialog, &SendMsg::forwardMessageSignal, messageDialog,
            [=](QString message)
            {
                if ( !message.isEmpty() )
                {
                    bool sendToAll{ messageDialog->sendToAll() };
                    if ( server != nullptr )
                    {
                        if ( sendToAll )
                            server->sendMasterMessage( message, nullptr, true );
                        else
                            server->sendMasterMessage( message, menuTarget, false );
                    }
                }
                messageDialog->disconnect();
                messageDialog->deleteLater();
                messageDialog = nullptr;
                menuTarget = nullptr;
            } );
        }

        if ( !messageDialog->isVisible() )
            messageDialog->show();
        else
            messageDialog->hide();
    }
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
        prompt = prompt.arg( Helper::serNumToIntStr( sernum, true ) );

        if ( Helper::confirmAction( this, title, prompt ) )
        {
            User::setAdminRank( sernum, GMRanks::GMaster );
            if ( User::getHasPassword( sernum ) )
                server->sendMasterMessage( reinstated, menuTarget, false );
            else
                menuTarget->setNewAdminPwdRequested( true );
        }
    }
    else
    {
        QString title{ "Revoke Admin:" };
        prompt = "Are you certain you want to REVOKE [ %1 ]'s powers?";
        prompt = prompt.arg( Helper::serNumToIntStr( sernum, true ) );

        if ( Helper::confirmAction( this, title, prompt ) )
        {
            User::setAdminRank( sernum, GMRanks::User );
            menuTarget->resetAdminAuth();
            server->sendMasterMessage( revoke, menuTarget, false );
        }
    }
    menuTarget = nullptr;
}

void PlrListWidget::on_actionMuteNetwork_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QString inform{ "The Server Host has %1 you. Reason: %2" };
    QString reason{ "Manual %1; %2" };
    QString type{ "" };
    bool mute{ true };

    QString title{ "%1 User:" };
    QString prompt{ "Are you certain you want to %1 [ %2 ]'s Network?" };
    if ( menuTarget->getIsMuted() )
    {
        title = title.arg( "Un-Mute" );
        prompt = prompt.arg( "Un-Mute" );
        type = "Un-Muted";
        reason = reason.arg( "Un-Mute" );

        mute = false;
    }
    else
    {
        title = title.arg( "Mute" );
        prompt = prompt.arg( "Mute" );
        type = "Muted";
        reason = reason.arg( "Mute" );
    }

    prompt = prompt.arg( menuTarget->getSernum_s() );

    if ( Helper::confirmAction( this, title, prompt ) )
    {
        reason = reason.arg( User::requestReason( this ) );
        inform = inform.arg( type )
                       .arg( reason );

        PunishDurations muteDuration{ PunishDurations::Invalid };
        if ( mute )
        {
            muteDuration = User::requestDuration();
            User::addMute( nullptr, menuTarget, reason, false, false, muteDuration );
        }
        else
            User::removePunishment( menuTarget->getSernumHex_s(), PunishTypes::Mute, PunishTypes::SerNum );

        menuTarget->setMuteDuration( static_cast<quint64>( muteDuration ) );

        QString logMsg{ "%1: [ %2 ], [ %3 ]" };
        logMsg = logMsg.arg( reason )
                       .arg( menuTarget->getSernum_s() )
                       .arg( menuTarget->getBioData() );

        emit this->insertLogSignal( server->getServerName(), logMsg, LogTypes::PUNISHMENT, true, true );

        server->sendMasterMessage( inform, menuTarget, false );
    }

    menuTarget = nullptr;
}

void PlrListWidget::on_actionDisconnectUser_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QString title{ "Disconnect User:" };
    QString prompt{ "Are you certain you want to DISCONNECT [ %1 ]?" };
            prompt = prompt.arg( menuTarget->getSernum_s() );

    QString inform{ "The Server Host has disconnected you from the Server. Reason: %1" };
    QString reason{ "Manual Disconnect; %1" };

    if ( Helper::confirmAction( this, title, prompt ) )
    {
        reason = reason.arg( Helper::getDisconnectReason( this ) );
        inform = inform.arg( reason );

        server->sendMasterMessage( inform, menuTarget, false );
        if ( menuTarget->waitForBytesWritten() )
            menuTarget->setDisconnected( true, DCTypes::IPDC );

        QString logMsg{ "%1: [ %2 ], [ %3 ]" };
                logMsg = logMsg.arg( reason )
                         .arg( menuTarget->getSernum_s() )
                         .arg( menuTarget->getBioData() );

        emit this->insertLogSignal( server->getServerName(), logMsg, LogTypes::PUNISHMENT, true, true );
    }
    menuTarget = nullptr;
}

void PlrListWidget::on_actionBANISHUser_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QString title{ "Ban SerNum:" };
    QString prompt{ "Are you certain you want to BANISH User [ %1 ]?" };
            prompt = prompt.arg( menuTarget->getSernum_s() );

    QString inform{ "The Server Host has BANISHED you. Reason: %1" };
    QString reason{ "Manual Banish; %1" };

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

        emit this->insertLogSignal( server->getServerName(), logMsg, LogTypes::PUNISHMENT, true, true );

        server->sendMasterMessage( inform, menuTarget, false );
        if ( menuTarget->waitForBytesWritten() )
            menuTarget->setDisconnected( true, DCTypes::IPDC );
    }
    menuTarget = nullptr;
}

void PlrListWidget::selectRowSlot(const qint32& row)
{
    if ( row >= 0 )
        ui->playerView->selectRow( row );
}
