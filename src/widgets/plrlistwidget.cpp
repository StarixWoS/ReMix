
//Class includes.
#include "plrlistwidget.hpp"
#include "ui_plrlistwidget.h"

//ReMix Widget includes.
#include "widgets/settingswidget.hpp"

//ReMix Views includes.
#include "views/plrsortproxymodel.hpp"
#include "views/tbleventfilter.hpp"

//ReMix includes.
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

QHash<QSharedPointer<Server>, PlrListWidget*> PlrListWidget::plrViewInstanceMap;

PlrListWidget::PlrListWidget(QSharedPointer<Server> svr) :
    server( svr ),
    ui(new Ui::PlrListWidget)
{
    ui->setupUi(this);

    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &PlrListWidget::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );

    //Create our Context Menus
    contextMenu = new QMenu( this );

    //Setup the PlayerInfo TableView.
    plrModel = new QStandardItemModel( 0, 9, nullptr );
    plrModel->setHeaderData( *PlrCols::IPPort, Qt::Horizontal, "Player IP:Port" );
    plrModel->setHeaderData( *PlrCols::PlrPing, Qt::Horizontal, "Ping" );
    plrModel->setHeaderData( *PlrCols::SerNum, Qt::Horizontal, "SerNum" );
    plrModel->setHeaderData( *PlrCols::BytesOut, Qt::Horizontal, "OUT" );
    plrModel->setHeaderData( *PlrCols::BioData, Qt::Horizontal, "BIO" );
    plrModel->setHeaderData( *PlrCols::Alias, Qt::Horizontal, "Alias" );
    plrModel->setHeaderData( *PlrCols::BytesIn, Qt::Horizontal, "IN" );
    plrModel->setHeaderData( *PlrCols::Name, Qt::Horizontal, "Name" );
    plrModel->setHeaderData( *PlrCols::Time, Qt::Horizontal, "Time" );
    plrModel->setHeaderData( *PlrCols::Age, Qt::Horizontal, "Age" );

    //Proxy model to support sorting without actually altering the underlying model
    plrProxy = new PlrSortProxyModel();
    plrProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    plrProxy->setDynamicSortFilter( true );
    plrProxy->setSourceModel( plrModel );
    ui->playerView->setModel( plrProxy );

    ui->playerView->setColumnHidden( *PlrCols::BioData, true );
    ui->playerView->setColumnWidth( *PlrCols::BytesOut, 120 );
    ui->playerView->setColumnWidth( *PlrCols::BytesIn, 120 );
    ui->playerView->setColumnWidth( *PlrCols::IPPort, 130 );
    if ( Settings::getSetting( SKeys::Setting, SSubKeys::CensorIPInfo ).toBool() )
        this->censorUIIPInfoSlot( true );

    QObject::connect( SettingsWidget::getInstance(), &SettingsWidget::censorUIIPInfoSignal, this, &PlrListWidget::censorUIIPInfoSlot );

    ui->playerView->setColumnWidth( *PlrCols::PlrPing, 20 );
    ui->playerView->setColumnWidth( *PlrCols::SerNum, 100 );
    ui->playerView->setColumnWidth( *PlrCols::Alias, 70 );
    ui->playerView->setColumnWidth( *PlrCols::Name, 70 );
    ui->playerView->setColumnWidth( *PlrCols::Time, 50 );
    ui->playerView->setColumnWidth( *PlrCols::Age, 80 );
    ui->playerView->horizontalHeader()->setStretchLastSection( true );

    //Install Event Filter to enable Row-Deslection.
    ui->playerView->viewport()->installEventFilter( TblEventFilter::getInstance( ui->playerView) );

    QObject::connect( Theme::getInstance(), &Theme::themeChangedSignal, this, &PlrListWidget::themeChangedSlot );
}

PlrListWidget::~PlrListWidget()
{
    this->disconnect();
    if ( messageDialog != nullptr )
    {
        if ( messageDialog->isVisible() )
            messageDialog->close();

        messageDialog->disconnect();
        messageDialog->deleteLater();
    }

    for ( auto plr : plrTableItems.keys() )
    {
        this->disconnect( plr.get() );
        plrTableItems.remove( plr );
    }

    contextMenu->deleteLater();

    TblEventFilter::deleteInstance( ui->playerView );
    plrModel->deleteLater();
    plrProxy->deleteLater();

    menuTarget = nullptr;
    server = nullptr;

    delete ui;
}

PlrListWidget* PlrListWidget::getInstance(QSharedPointer<Server> server)
{
    PlrListWidget* instance{ plrViewInstanceMap.value( server, nullptr ) };
    if ( instance == nullptr )
    {
        instance = new PlrListWidget( server );
        if ( instance != nullptr )
            plrViewInstanceMap.insert( server, instance );
    }
    return instance;
}

void PlrListWidget::deleteInstance(QSharedPointer<Server> server)
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
    contextMenu->addAction( ui->actionQuarantineUser );
    contextMenu->addAction( ui->actionBANISHUser );
    contextMenu->addAction( ui->actionMakeAdmin );

    contextMenu->insertSeparator( ui->actionMuteNetwork );
}

const QIcon& PlrListWidget::getIcon(const IconRoles& role)
{
    switch( role )
    {
        case IconRoles::GSoulGhost: return gSoulGhostIcon;
        break;
        case IconRoles::GSoulNPK: return gSoulNPKIcon;
        break;
        case IconRoles::GSoulPK: return gSoulPKIcon;
        break;
        case IconRoles::SoulAFK: return soulAFKIcon;
        break;
        case IconRoles::SoulAFKWell: return soulAFKWellIcon;
        break;
        case IconRoles::SoulGhost: return soulGhostIcon;
        break;
        case IconRoles::SoulNPK: return soulNPKIcon;
        break;
        case IconRoles::SoulPK: return soulPKIcon;
        break;
        case IconRoles::SoulWell: return soulWellIcon;
        break;
        case IconRoles::SoulCheater: return soulCheater;
        break;
        case IconRoles::SoulModder: return soulModder;
        break;
        case IconRoles::SoulMuted: return soulMuted;
        break;
        case IconRoles::Invalid:
        default:
            {
                return soulNPKIcon;
            }
        break;
    }

    //Fallthrough.
    return soulNPKIcon;
}

void PlrListWidget::updatePlrViewSlot(QSharedPointer<Player> plr, const qint32& column, const QVariant& data, const qint32& role, const bool& isColor)
{
    if ( plr == nullptr )
        return;

    QStandardItem* item{ plrTableItems.value( plr, nullptr ) };
    if ( item == nullptr )
    {
        this->plrViewInsertRowSlot( plr, plr->getIPPortAddress(), plr->getBioData().toLatin1() );
        item = plrTableItems.value( plr, nullptr );
        if ( item != nullptr )
            this->updatePlrView( item, column, data, role, isColor );
    }
    else
        this->updatePlrView( item, column, data, role, isColor );
}

void PlrListWidget::updatePlrView(QStandardItem* object, const qint32& column, const QVariant& data, const qint32& role, const bool& isColor)
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
                else if ( static_cast<PlrCols>( column ) == PlrCols::SerNum
                       && role == Qt::DecorationRole )
                {
                    const QVariant newData{ this->getIcon( static_cast<IconRoles>( data.toInt() ) ) };
                    sModel->setData( sModel->index( object->row(), column ), newData, role );
                }
                else
                    sModel->setData( sModel->index( object->row(), column ), data, role );
            }
            else
                sModel->setData( sModel->index( object->row(), column ), Theme::getColorBrush( static_cast<Colors>( data.toInt() ) ), role );

            sModel->setData( sModel->index( object->row(), column ), Qt::AlignCenter, Qt::TextAlignmentRole );
        }
    }
    ui->playerView->resizeColumnToContents( column );
}

void PlrListWidget::plrViewInsertRowSlot(QSharedPointer<Player> plr, const QString& ipPortStr, const QByteArray& data)
{
    QStandardItem* item{ plrTableItems.value( plr, nullptr ) };
    if ( item == nullptr )
    {
        item = new QStandardItem();

        if ( plrTableItems.value( plr, nullptr ) == nullptr )
            plrTableItems.insert( plr, item );

        int row{ plrModel->rowCount() };
        plrModel->insertRow( row, item );
    }

    this->updatePlrView( item, 0, ipPortStr, Qt::DisplayRole, false );

    if ( !data.isEmpty() )
    {
        const QString sernum{ Helper::getStrStr( data, "sernum", "=", "," ) };
        User::updateCallCount( Helper::serNumToHexStr( sernum ) );

        this->updatePlrView( item, *PlrCols::PlrPing, "0MS", Qt::DisplayRole, false );
        this->updatePlrView( item, *PlrCols::SerNum, sernum, Qt::DisplayRole, false );
        this->updatePlrView( item, *PlrCols::Age, Helper::getStrStr( data, "HHMM", "=", "," ), Qt::DisplayRole, false );
        this->updatePlrView( item, *PlrCols::Alias, Helper::getStrStr( data, "alias", "=", "," ), Qt::DisplayRole, false );
        this->updatePlrView( item, *PlrCols::BioData, data, Qt::DisplayRole, false );
    }
}

void PlrListWidget::plrViewRemoveRowSlot(QSharedPointer<Player> plr)
{
    QStandardItem* item{ plrTableItems.value( plr, nullptr ) };
    if ( item != nullptr )
    {
        plrModel->removeRow( item->row() );
        plrTableItems.remove( plr );
    }
}

void PlrListWidget::censorUIIPInfoSlot(const bool& state)
{
    censorUIIPInfo = state;
}

void PlrListWidget::on_playerView_customContextMenuRequested(const QPoint& pos)
{
    QModelIndex menuIndex{ plrProxy->mapToSource( ui->playerView->indexAt( pos ) ) };

    this->initContextMenu();
    if ( menuIndex.row() >= 0 )
    {
        QSharedPointer<Player> plr{ plrTableItems.key( plrModel->item( menuIndex.row(), 0 ) ) };
        if ( plr != nullptr )
        {
            menuTarget = plr;
            if ( plr->getIsMuted() )
                ui->actionMuteNetwork->setText( "Un-Mute Network" );
            else
                ui->actionMuteNetwork->setText( "Mute Network" );

            if ( plr->getIsAdmin() )
                ui->actionMakeAdmin->setText( "Change Admin" );
            else
                ui->actionMakeAdmin->setText( "Make Admin" );

            if ( plr->getIsQuarantined() )
                ui->actionQuarantineUser->setText( "Un-Quarantine User" );
            else
                ui->actionQuarantineUser->setText( "Quarantine User" );
        }
        contextMenu->popup( ui->playerView->viewport()->mapToGlobal( pos ) );
    }
    else
    {
        contextMenu->removeAction( ui->actionDisconnectUser );
        contextMenu->removeAction( ui->actionQuarantineUser );
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
        {
            messageDialog = new SendMsg();
            QObject::connect( messageDialog, &SendMsg::forwardMessageSignal, this, &PlrListWidget::forwardMessageSlot, Qt::UniqueConnection );
        }

        messageDialog->setTitle( menuTarget->getSernum_s() );

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
            if ( !User::getHasPassword( sernum ) )
                menuTarget->setNewAdminPwdRequested( true );
        }
    }
    else
    {
        prompt = "Are you certain you want to CHANGE [ %1 ]'s rank?";
        prompt = prompt.arg( Helper::serNumToIntStr( sernum, true ) );

        if ( Helper::confirmAction( this, titleRevokeStr, prompt ) )
        {
            GMRanks rank{ User::requestRank( this ) };

            User::setAdminRank( sernum, rank );
            menuTarget->setAdminRank( rank );
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

        qint64 muteDuration{ *PunishDurations::Invalid };
        if ( mute )
        {
            muteDuration = *User::requestDuration();
            User::addMute( menuTarget, inform, false, static_cast<PunishDurations>( muteDuration ) );
        }
        else
            menuTarget->setMuteDuration( 0 );
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

        emit this->insertLogSignal( server->getServerName(), logMsg, LKeys::PunishmentLog, true, true );
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

        User::addBan( menuTarget, reason, User::requestDuration() );

        server->sendMasterMessage( inform, menuTarget, false );
        if ( menuTarget->waitForBytesWritten() )
            menuTarget->setDisconnected( true, DCTypes::IPDC );
    }
    menuTarget = nullptr;
}

void PlrListWidget::on_actionQuarantineUser_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QString sernum{ menuTarget->getSernumHex_s() };

    QString inform{ "The Server Host has %1 you. Reason: %2" };
    QString reason{ "Manual %1; %2" };
    QString type{ "" };
    bool quarantine{ true };

    QString title{ "%1 User:" };
    QString prompt{ "Are you certain you want to %1 [ %2 ]?" };
    if ( menuTarget->getIsQuarantined() )
    {
        title = title.arg( "Un-Quarantine" );
        prompt = prompt.arg( "Un-Quarantine" );
        type = "Un-Quarantined";
        reason = reason.arg( "Un-Quarantine" );

        quarantine = false;
    }
    else
    {
        title = title.arg( "Quarantine" );
        prompt = prompt.arg( "Quarantine" );
        type = "Quarantined";
        reason = reason.arg( "Quarantine" );
    }

    prompt = prompt.arg( Helper::serNumToIntStr( sernum, true ) );

    if ( Helper::confirmAction( this, title, prompt ) )
    {
        reason = reason.arg( User::requestReason( this ) );
        inform = inform.arg( type )
                       .arg( reason );

        server->sendMasterMessage( inform, menuTarget, false );
        if ( quarantine )
        {
            QString append{ "You may only interact with other Quarantined Users." };
            server->sendMasterMessage( append, menuTarget, false );

            menuTarget->setQuarantined( true );
            menuTarget->setQuarantineOverride( false );
        }
        else
            menuTarget->setQuarantineOverride( true );

        QString logMsg{ "%1: [ %2 ], [ %3 ]" };
                logMsg = logMsg.arg( reason )
                               .arg( Helper::serNumToIntStr( sernum, true ) )
                               .arg( menuTarget->getBioData() );

        emit this->insertLogSignal( server->getServerName(), logMsg, LKeys::PunishmentLog, true, true );
    }
    menuTarget = nullptr;
}

void PlrListWidget::selectRowSlot(const qint32& row)
{
    if ( row >= 0 )
        ui->playerView->selectRow( row );
}

void PlrListWidget::themeChangedSlot()
{
    ui->playerView->setPalette( Theme::getCurrentPal() );
}

void PlrListWidget::forwardMessageSlot(const QString& message)
{
    if ( !message.isEmpty() )
    {
        if ( server != nullptr )
        {
            bool sendToAll{ messageDialog->sendToAll() };
            if ( sendToAll )
                menuTarget = nullptr;

            emit this->insertMasterMessageSignal( message, menuTarget, sendToAll, false );
        }
    }
    menuTarget = nullptr;
}
