
#include "includes.hpp"
#include "plrlistwidget.hpp"
#include "ui_plrlistwidget.h"

PlrListWidget::PlrListWidget(QWidget *parent, ServerInfo* svr, Admin* adm) :
    QWidget(parent),
    ui(new Ui::PlrListWidget)
{
    ui->setupUi(this);

    server = svr;
    admin = adm;

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

QStandardItemModel* PlrListWidget::getPlrModel()
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
    contextMenu->addAction( ui->actionRevokeAdmin );
    contextMenu->addAction( ui->actionMakeAdmin );
    contextMenu->addAction( ui->actionMuteNetwork );
    contextMenu->addAction( ui->actionUnMuteNetwork );
    contextMenu->addAction( ui->actionDisconnectUser );
    contextMenu->addAction( ui->actionBANISHIPAddress );
    contextMenu->addAction( ui->actionBANISHSerNum );

    contextMenu->insertSeparator( ui->actionDisconnectUser );
}

void PlrListWidget::on_playerView_customContextMenuRequested(const QPoint &pos)
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

        if ( !menuTarget->getIsAdmin() )
            contextMenu->removeAction( ui->actionRevokeAdmin );
        else
            contextMenu->removeAction( ui->actionMakeAdmin );

        if ( plr != nullptr )
        {
            if( plr->getNetworkMuted() )
                contextMenu->removeAction( ui->actionMuteNetwork );
            else
                contextMenu->removeAction( ui->actionUnMuteNetwork );
        }
        contextMenu->popup( ui->playerView->viewport()->mapToGlobal( pos ) );
    }
    else
    {
        contextMenu->removeAction( ui->actionSendMessage );
        contextMenu->removeAction( ui->actionRevokeAdmin );
        contextMenu->removeAction( ui->actionMakeAdmin );
        contextMenu->removeAction( ui->actionMuteNetwork );
        contextMenu->removeAction( ui->actionUnMuteNetwork );
        contextMenu->removeAction( ui->actionDisconnectUser );
        contextMenu->removeAction( ui->actionBANISHSerNum );
        contextMenu->removeAction( ui->actionBANISHIPAddress );
    }
    contextMenu->popup( ui->playerView->viewport()->mapToGlobal( pos ) );
}

void PlrListWidget::on_actionSendMessage_triggered()
{
    SendMsg* adminMsg = new SendMsg( this, server, menuTarget );
             adminMsg->show();
    menuTarget = nullptr;
}

void PlrListWidget::on_actionRevokeAdmin_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QString msg{ "Your Remote Administrator privileges have been REVOKED "
                 "by either the Server Host. Please contact the Server Host if "
                 "you believe this was in error." };

    QString sernum{ menuTarget->getSernumHex_s() };
    if ( Admin::deleteRemoteAdmin( this, sernum ) )
    {
        //The User is no longer a registered Admin.
        //Revoke their current permissions.
        menuTarget->resetAdminAuth();

        server->sendMasterMessage( msg, menuTarget, false );
        admin->loadServerAdmins();
    }
    menuTarget = nullptr;
}

void PlrListWidget::on_actionMakeAdmin_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QString msg{ "The Server Host is attempting to register you as an "
                 "Admin with the server. Please reply to this message with "
                 "(/register *YOURPASS). Note: The server Host and other Admins"
                 " will not have access to this information." };

    QString sernum{ menuTarget->getSernumHex_s() };
    if ( !Admin::getIsRemoteAdmin( sernum ) )
    {
        if ( Admin::createRemoteAdmin( this, sernum ) )
        {
            server->sendMasterMessage( msg, menuTarget, false );
            menuTarget->setReqNewAuthPwd( true );
        }
    }
    menuTarget = nullptr;
}

void PlrListWidget::on_actionMuteNetwork_triggered()
{
    //Mute the selected User's Network.
    //We will not inform the User of this event.
    //This tells the Server to not re-send incoming
    //packets from this User to other connected Users.

    if ( menuTarget != nullptr )
    {
        QString title{ "Mute User:" };
        QString prompt{ "Are you certain you want to MUTE ( " %
                        menuTarget->getSernum_s() % " )'s Network?" };

        if ( Helper::confirmAction( this, title, prompt ) )
            menuTarget->setNetworkMuted( true );
    }
    menuTarget = nullptr;
}

void PlrListWidget::on_actionUnMuteNetwork_triggered()
{
    //Un-Mute the selected User's Network.
    //We do not inform the User of this event.
    //This tells the Server to re-send incoming
    //packets from this User to other connected Users.

    if ( menuTarget != nullptr )
    {
        QString title{ "Un-Mute User:" };
        QString prompt{ "Are you certain you want to UN-MUTE ( " %
                        menuTarget->getSernum_s() % " )'s Network?" };

        if ( Helper::confirmAction( this, title, prompt ) )
            menuTarget->setNetworkMuted( false );
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
        QString prompt{ "Are you certain you want to DISCONNECT ( " %
                        menuTarget->getSernum_s() % " )?" };

        QString inform{ "The Server Host has disconnected you from the Server. "
                        "Reason: %1" };

        if ( Helper::confirmAction( this, title, prompt ) )
        {
            inform = inform.arg( Helper::getDisconnectReason( this ) );
            server->sendMasterMessage( inform, menuTarget, false );

            if ( sock->waitForBytesWritten() )
            {
                menuTarget->setSoftDisconnect( true );
                server->setIpDc( server->getIpDc() + 1 );
            }
        }
    }
    menuTarget = nullptr;
}

void PlrListWidget::on_actionBANISHIPAddress_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QString ipAddr = menuTarget->getPublicIP();

    QString title{ "Ban IP Address:" };
    QString prompt{ "Are you certain you want to BANISH [ "
                  % menuTarget->getSernum_s() % " ]'s IP Address [ "
                  % ipAddr % " ]?" };

    QString inform{ "The Server Host has banned your IP Address [ %1 ]. "
                    "Reason: %2" };
    QString reason{ "Manual Banish; %1" };

    QTcpSocket* sock = menuTarget->getSocket();
    if ( sock != nullptr )
    {
        QHostAddress ip = sock->peerAddress();
        if ( Helper::confirmAction( this, title, prompt ) )
        {
            reason = reason.arg( Helper::getBanishReason( this ) );
            inform = inform.arg( ip.toString() )
                           .arg( reason ).toLatin1();
            server->sendMasterMessage( inform, menuTarget, false );

            reason = QString( "%1 [ %2:%3 ]: %4" )
                         .arg( reason )
                         .arg( menuTarget->getPublicIP() )
                         .arg( menuTarget->getPublicPort() )
                         .arg( QString( menuTarget->getBioData() ) );
            admin->getBanDialog()->addIPBan( ip.toString(), reason );

            if ( sock->waitForBytesWritten() )
            {
                menuTarget->setSoftDisconnect( true );
                server->setIpDc( server->getIpDc() + 1 );
            }
        }
    }
    menuTarget = nullptr;
}

void PlrListWidget::on_actionBANISHSerNum_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QString title{ "Ban SerNum:" };
    QString prompt{ "Are you certain you want to BANISH the SerNum [ "
                  % menuTarget->getSernum_s() % " ]?" };

    QString inform{ "The Server Host has banned your SerNum [ %1 ]. "
                    "Reason: %2" };
    QString reason{ "Manual Banish; %1" };

    QTcpSocket* sock = menuTarget->getSocket();
    if ( sock != nullptr )
    {
        if ( Helper::confirmAction( this, title, prompt ) )
        {
            reason = reason.arg( Helper::getBanishReason( this ) );
            inform = inform.arg( menuTarget->getSernum_s() )
                           .arg( reason ).toLatin1();
            server->sendMasterMessage( inform, menuTarget, false );

            reason = QString( "%1 [ %2:%3 ]: %4" )
                         .arg( reason )
                         .arg( menuTarget->getPublicIP() )
                         .arg( menuTarget->getPublicPort() )
                         .arg( QString( menuTarget->getBioData() ) );

            QString sernum{ menuTarget->getSernumHex_s() };
            admin->getBanDialog()->addSerNumBan( sernum, reason );

            if ( sock->waitForBytesWritten() )
            {
                menuTarget->setSoftDisconnect( true );
                server->setIpDc( server->getIpDc() + 1 );
            }
        }
    }
    menuTarget = nullptr;
}
