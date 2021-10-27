
//Class includes.
#include "plrlistwidget.hpp"
#include "ui_plrlistwidget.h"

//ReMix Widget includes.
#include "widgets/settingswidget.hpp"

//ReMix Views includes.
#include "views/plrsortproxymodel.hpp"
#include "views/tbleventfilter.hpp"

//ReMix includes.
#include "remixwidget.hpp"
#include "settings.hpp"
#include "sendmsg.hpp"
#include "player.hpp"
#include "helper.hpp"
#include "logger.hpp"
#include "server.hpp"
#include "remix.hpp"
#include "theme.hpp"
#include "user.hpp"

//Qt Includes.
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTcpSocket>
#include <QtCore>
#include <QMenu>
#include <QMap>

QHash<Server*, PlrListWidget*> PlrListWidget::plrViewInstanceMap;

PlrListWidget::PlrListWidget(ReMixWidget* parent, Server* svr) :
    ui(new Ui::PlrListWidget)
{
    ui->setupUi(this);
    remixWidget = parent;

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
    if ( Settings::getSetting( SKeys::Setting, SSubKeys::CensorIPInfo ).toBool() )
        this->censorUIIPInfoSlot( true );

    QObject::connect( SettingsWidget::getInstance(), &SettingsWidget::censorUIIPInfoSignal, this, &PlrListWidget::censorUIIPInfoSlot );

    ui->playerView->setColumnWidth( static_cast<int>( PlrCols::SerNum ), 100 );
    ui->playerView->setColumnWidth( static_cast<int>( PlrCols::Alias ), 70 );
    ui->playerView->setColumnWidth( static_cast<int>( PlrCols::Time ), 50 );
    ui->playerView->setColumnWidth( static_cast<int>( PlrCols::Age ), 80 );
    ui->playerView->horizontalHeader()->setStretchLastSection( true );

    //Install Event Filter to enable Row-Deslection.
    tblEvFilter = new TblEventFilter( ui->playerView );
    ui->playerView->viewport()->installEventFilter( tblEvFilter );

    QObject::connect( Theme::getInstance(), &Theme::themeChangedSignal, this,
    [=,this]()
    {
        ui->playerView->setPalette( Theme::getCurrentPal() );
    });
}

PlrListWidget::~PlrListWidget()
{
    if ( messageDialog != nullptr )
    {
        if ( messageDialog->isVisible() )
            messageDialog->close();

        messageDialog->disconnect();
        messageDialog->deleteLater();
    }

    contextMenu->deleteLater();

    tblEvFilter->deleteLater();
    plrModel->deleteLater();
    plrProxy->deleteLater();

    delete ui;
}

PlrListWidget* PlrListWidget::getInstance(ReMixWidget* parent, Server* server)
{
    PlrListWidget* instance{ plrViewInstanceMap.value( server, nullptr ) };
    if ( instance == nullptr )
    {
        instance = new PlrListWidget( parent, server );
        if ( instance != nullptr )
            plrViewInstanceMap.insert( server, instance );
    }
    return instance;
}

void PlrListWidget::deleteInstance(Server* server)
{
    PlrListWidget* instance{ plrViewInstanceMap.take( server ) };
    if ( instance != nullptr )
    {
        instance->disconnect();
        instance->setParent( nullptr );
        instance->deleteLater();
    }
}

QStandardItemModel* PlrListWidget::getPlrModel() const
{
    return plrModel;
}

void PlrListWidget::resizeColumns()
{
    //ui->playerView->resizeColumnsToContents();
}

bool PlrListWidget::getCensorUIIPInfo() const
{
    return censorUIIPInfo;
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

void PlrListWidget::updatePlrViewSlot(QStandardItem* object, const qint32& column, const QVariant& data, const qint32& role, const bool& isColor)
{
    if ( object != nullptr )
    {
        QStandardItemModel* sModel = object->model();
        if ( sModel != nullptr )
        {
            if ( !isColor )
            {
                if ( static_cast<PlrCols>( column ) == PlrCols::IPPort
                  && this->getCensorUIIPInfo() )
                {
                    sModel->setData( sModel->index( object->row(), column ), "***.***.***.***", role );
                }
                else
                    sModel->setData( sModel->index( object->row(), column ), data, role );
            }
            else
                sModel->setData( sModel->index( object->row(), column ), Theme::getColorBrush( static_cast<Colors>( data.toInt() ) ), role );
        }
    }
    ui->playerView->resizeColumnToContents( column );
}

void PlrListWidget::plrViewInsertRowSlot(const qintptr& peer, const QByteArray& data)
{
    int row{ plrModel->rowCount() };

    QStandardItem* item{ new QStandardItem() };
    plrModel->insertRow( row, item );

    emit this->insertedRowItemSignal( item, peer, data );
}

void PlrListWidget::plrViewRemoveRowSlot(QStandardItem* object)
{
    if ( object != nullptr )
        plrModel->removeRow( object->row() );
}

void PlrListWidget::censorUIIPInfoSlot(const bool& state)
{
    censorUIIPInfo = state;
}

void PlrListWidget::on_playerView_customContextMenuRequested(const QPoint& pos)
{
    QModelIndex menuIndex{ plrProxy->mapToSource( ui->playerView->indexAt( pos ) ) };

    this->initContextMenu();
    if ( menuIndex.row() >= 0
      && remixWidget != nullptr )
    {
        Player* plr{ server->getPlayer( remixWidget->getPeerFromQItem( plrModel->item( menuIndex.row(), 0 ) ) ) };
        if ( plr != nullptr )
            menuTarget = plr;

        if ( menuTarget != nullptr )
        {
            if ( menuTarget->getIsMuted() )
                ui->actionMuteNetwork->setText( "Un-Mute Network" );
            else
                ui->actionMuteNetwork->setText( "Mute Network" );

            if ( menuTarget->getIsAdmin() )
                ui->actionMakeAdmin->setText( "Change Admin" );
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
}

void PlrListWidget::on_actionSendMessage_triggered()
{
    if ( menuTarget != nullptr )
    {
        if ( messageDialog == nullptr )
            messageDialog = new SendMsg();

        messageDialog->setTitle( Helper::serNumToIntStr( menuTarget->getSernumHex_s(), true ) );

        QObject::connect( messageDialog, &SendMsg::forwardMessageSignal, messageDialog,
        [=, this](QString message)
        {
            if ( !message.isEmpty() )
            {
                if ( server != nullptr )
                {
                    bool sendToAll{ messageDialog->sendToAll() };
                    if ( sendToAll )
                        menuTarget = nullptr;

                    server->sendMasterMessage( message, menuTarget, sendToAll );
                }
            }
            menuTarget = nullptr;
        }, Qt::UniqueConnection );

        if ( !messageDialog->isVisible() )
            messageDialog->exec();
        else
            messageDialog->hide();
    }
}

void PlrListWidget::on_actionMakeAdmin_triggered()
{
    if ( menuTarget == nullptr )
        return;

    static const QString revoke{ "Your Remote Administrator privileges have been revoked by the Server Host. Please contact the Server Host "
                                 "if you believe this was in error." };
    static const QString reinstated{ "Your Remote Administrator privelages have been partially reinstated by the Server Host." };

    static const QString titleCreateStr{ "Create Admin:" };
    static const QString titleRevokeStr{ "Change Admin:" };
    QString sernum{ menuTarget->getSernumHex_s() };
    QString prompt{ "" };
    if ( !User::getIsAdmin( sernum ) )
    {
        prompt = "Are you certain you want to MAKE [ %1 ] a Remote Admin? \r\n\r\nPlease make sure you trust [ %1 ] as this will "
                 "allow the them to utilize Admin commands that can remove the ability for other users to connect to the Server.";
        prompt = prompt.arg( Helper::serNumToIntStr( sernum, true ) );

        if ( Helper::confirmAction( this, titleCreateStr, prompt ) )
        {
            User::setAdminRank( sernum, User::requestRank( this ) );
            if ( User::getHasPassword( sernum ) )
                server->sendMasterMessage( reinstated, menuTarget, false );
            else
                menuTarget->setNewAdminPwdRequested( true );
        }
    }
    else
    {
        prompt = "Are you certain you want to CHANGE [ %1 ]'s rank?";
        prompt = prompt.arg( Helper::serNumToIntStr( sernum, true ) );

        if ( Helper::confirmAction( this, titleRevokeStr, prompt ) )
        {
            User::setAdminRank( sernum, User::requestRank( this ) );
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

    QString sernum{ menuTarget->getSernumHex_s() };

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

    prompt = prompt.arg( Helper::serNumToIntStr( sernum, true ) );

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
            User::removePunishment( sernum, PunishTypes::Mute, PunishTypes::SerNum );

        QString logMsg{ "%1: [ %2 ], [ %3 ]" };
                logMsg = logMsg.arg( reason )
                               .arg( Helper::serNumToIntStr( sernum, true ) )
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

    QString sernum{ menuTarget->getSernumHex_s() };

    QString title{ "Disconnect User:" };
    QString prompt{ "Are you certain you want to DISCONNECT [ %1 ]?" };
            prompt = prompt.arg( Helper::serNumToIntStr( sernum, true ) );

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
                               .arg( Helper::serNumToIntStr( sernum, true ) )
                               .arg( menuTarget->getBioData() );

        emit this->insertLogSignal( server->getServerName(), logMsg, LogTypes::PUNISHMENT, true, true );
    }
    menuTarget = nullptr;
}

void PlrListWidget::on_actionBANISHUser_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QString sernum{ menuTarget->getSernumHex_s() };

    QString title{ "Ban SerNum:" };
    QString prompt{ "Are you certain you want to BANISH User [ %1 ]?" };
            prompt = prompt.arg( Helper::serNumToIntStr( sernum, true ) );

    QString inform{ "The Server Host has BANISHED you. Reason: %1" };
    QString reason{ "Manual Banish; %1" };

    if ( Helper::confirmAction( this, title, prompt ) )
    {
        reason = reason.arg( User::requestReason( this ) );
        inform = inform.arg( reason );

        User::addBan( nullptr, menuTarget, reason, false, User::requestDuration() );
        QString logMsg{ "%1: [ %2 ], [ %3 ]" };
                logMsg = logMsg.arg( reason )
                               .arg( Helper::serNumToIntStr( sernum, true ) )
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
