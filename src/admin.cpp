
#include "includes.hpp"
#include "admin.hpp"
#include "ui_admin.h"

//Initialize our accepted Command List.
const QStringList Admin::commands =
{
    QStringList() << "ban" << "ipban" << "kick" << "mute" << "msg"
};

Admin::Admin(QWidget *parent, ServerInfo* svr) :
    QDialog(parent),
    ui(new Ui::Admin)
{
    ui->setupUi(this);
    server = svr;

    //Remove the "Help" button from the window title bars.
    {
        QIcon icon = this->windowIcon();
        Qt::WindowFlags flags = this->windowFlags();
        flags &= ~Qt::WindowContextHelpButtonHint;

        this->setWindowFlags( flags );
        this->setWindowIcon( icon );

        //this->setWindowModality( Qt::WindowModal );
    }

    //Setup our Random Device
    randDev = new RandDev();

    //Create our Context Menus
    contextMenu = new QMenu( this );

    //Setup the ServerInfo TableView.
    tableModel = new QStandardItemModel( 0, 4, 0 );
    tableModel->setHeaderData( 0, Qt::Horizontal, "SerNum" );
    tableModel->setHeaderData( 1, Qt::Horizontal, "Rank" );
    tableModel->setHeaderData( 2, Qt::Horizontal, "Hash" );
    tableModel->setHeaderData( 3, Qt::Horizontal, "Salt" );

    //Proxy model to support sorting without actually
    //altering the underlying model
    tableProxy = new QSortFilterProxyModel();
    tableProxy->setDynamicSortFilter( true );
    tableProxy->setSourceModel( tableModel );
    tableProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->adminTable->setModel( tableProxy );

    //Setup Objects.
    banDialog = new BanDialog( parent );

    this->loadServerAdmins();
    this->initContextMenu();
}

Admin::~Admin()
{
    tableModel->deleteLater();
    tableProxy->deleteLater();
    contextMenu->deleteLater();

    banDialog->close();
    banDialog->deleteLater();

    delete randDev;
    delete ui;
}

BanDialog* Admin::getBanDialog() const
{
    return banDialog;
}

void Admin::showBanDialog()
{
    if ( banDialog->isVisible() )
        banDialog->hide();
    else
        banDialog->show();
}

void Admin::loadServerAdmins()
{
    tableModel->removeRows( 0, tableModel->rowCount() );
    if ( QFile( "adminData.ini" ).exists() )
    {
        QSettings adminData( "adminData.ini", QSettings::IniFormat );
        QStringList groups = adminData.childGroups();

        QString group;

        QString hash{ "" };
        QString salt{ "" };
        int rank{ 0 };

        int row{ -1 };
        for ( int i = 0; i < groups.count(); ++i )
        {
            group = groups.at( i );

            rank = adminData.value( group % "/rank", 0 ).toInt();
            hash = adminData.value( group % "/hash", "" ).toString();
            salt = adminData.value( group % "/salt", "" ).toString();

            row = tableModel->rowCount();
            tableModel->insertRow( row );

            tableModel->setData( tableModel->index( row, 0 ),
                                 Helper::serNumToIntStr( group ),
                                 Qt::DisplayRole );

            tableModel->setData( tableModel->index( row, 1 ),
                                 rank, Qt::DisplayRole );

            tableModel->setData( tableModel->index( row, 2 ),
                                 hash, Qt::DisplayRole );

            tableModel->setData( tableModel->index( row, 3 ),
                                 salt, Qt::DisplayRole );
        }
        ui->adminTable->selectRow( 0 );
        ui->adminTable->resizeColumnsToContents();
    }
}

void Admin::initContextMenu()
{
    contextMenu->clear();
    contextMenu->addAction( ui->actionRevokeAdmin );
    contextMenu->addAction( ui->actionChangeRank );
}

void Admin::setAdminRank(int rank, QModelIndex index)
{
    QSettings adminData( "adminData.ini", QSettings::IniFormat );
    if ( index.isValid() )
    {
        QString txt = tableModel->data(
                          tableModel->index(
                              index.row(), 0 ) ).toString();
        if ( !txt.isEmpty() )
        {
            adminData.setValue( txt % "/rank", rank );
            tableModel->setData(
                        tableModel->index(
                            index.row(), 1 ), rank, Qt::DisplayRole );
        }
    }
    menuIndex = QModelIndex();
}

void Admin::on_makeAdmin_clicked()
{
    QString sernum = ui->adminSerNum->text().simplified();
    QString pwd = ui->adminPwd->text();

    if ( !sernum.isEmpty() && !pwd.isEmpty() )
        this->makeAdmin( sernum, pwd );
}

bool Admin::makeAdmin(QString& sernum, QString& pwd)
{
    if ( !sernum.isEmpty() && !pwd.isEmpty() )
        return this->makeAdminImpl( sernum, pwd );

    return false;
}

bool Admin::makeAdminImpl(QString& sernum, QString& pwd)
{
    QSettings adminData( "adminData.ini", QSettings::IniFormat );

    QString hexSerNum{ sernum };
            hexSerNum = Helper::serNumToHexStr( hexSerNum, 8 );

    if ( !hexSerNum.isEmpty()
      && !pwd.isEmpty() )
    {
        //We already have an admin using this sernum on record.
        //Return false. (Perhaps inform the would-be admin.)
        if ( adminData.childGroups()
                      .contains( hexSerNum, Qt::CaseInsensitive ) )
        {
            return false;
        }

        QString salt = Helper::genPwdSalt( randDev, SALT_LENGTH );

        QStringList groups = adminData.childGroups();
        QString j{ "" };
        for ( int i = 0; i < groups.count(); ++i )
        {
            j = adminData.value( groups.at( i ) % "/salt" ).toString();

            //Check if the Salt is already used.
            if ( j == salt )
            {   //If so, generate another and restart the loop.
                salt = Helper::genPwdSalt( randDev, SALT_LENGTH );
                i = 0;
            }
        }

        QVariant hash( salt + pwd );
                 hash = Helper::hashPassword( hash );

        adminData.setValue( hexSerNum % "/rank",
                            ui->comboBox->currentIndex() );

        adminData.setValue( hexSerNum % "/hash", hash );
        adminData.setValue( hexSerNum % "/salt", salt );

        int row = tableModel->rowCount();
        tableModel->insertRow( row );

        //Display the SERNUM in the correct format as required.
        if ( sernum.contains( "SOUL", Qt::CaseInsensitive )
          && !sernum.contains( " " ) )
        {
            sernum = "SOUL " % Helper::getStrStr( sernum, "SOUL", "SOUL", "" );
        }
        else if ( !( sernum.toInt( 0, 16 ) & MIN_HEX_SERNUM )
               && !sernum.contains( "SOUL " ) )
        {
            sernum.prepend( "SOUL " );
        }

        tableModel->setData(
                    tableModel->index( row, 0 ),
                    sernum, Qt::DisplayRole );

        tableModel->setData(
                    tableModel->index( row, 1 ),
                    ui->comboBox->currentIndex(), Qt::DisplayRole );

        tableModel->setData(
                    tableModel->index( row, 2 ),
                    hash, Qt::DisplayRole );

        tableModel->setData(
                    tableModel->index( row, 3 ),
                    salt, Qt::DisplayRole );

        ui->adminTable->selectRow( row );
        ui->adminTable->resizeColumnsToContents();

        return true;
    }
    return false;
}

void Admin::on_adminTable_customContextMenuRequested(const QPoint& pos)
{
    menuIndex = tableProxy->mapToSource( ui->adminTable->indexAt( pos ) );
    if ( menuIndex.row() < 0 )
        return;

    contextMenu->popup( ui->adminTable->viewport()->mapToGlobal( pos ) );
}

void Admin::on_actionRevokeAdmin_triggered()
{
    if ( menuIndex.isValid() )
    {
        QString sernum = tableModel->data(
                             tableModel->index(
                                 menuIndex.row(), 0 ) ).toString();

        if ( !sernum.isEmpty() )
        {
            if (  AdminHelper::deleteRemoteAdmin( this, sernum ) )
                tableModel->removeRow( menuIndex.row() );
        }
    }
    menuIndex = QModelIndex();
}

void Admin::on_actionChangeRank_triggered()
{
    if ( menuIndex.isValid() )
    {
        QString sernum = tableModel->data(
                             tableModel->index(
                                 menuIndex.row(), 0 ) ).toString();

        qint32 rank{ -1 };
        if ( !sernum.isEmpty() )
            rank = AdminHelper::changeRemoteAdminRank( this, sernum );

        if ( rank >= 0 )
        {
            tableModel->setData( tableModel->index( menuIndex.row(), 1 ),
                                 rank, Qt::DisplayRole );
        }
    }
}

//Handle Admin commands.
bool Admin::parseCommand(QString& packet, Player* plr)
{
    bool retn{ false };

    if ( !packet.isEmpty()
      || plr != nullptr )
    {
        QString cmdReason{ "" };
        QString cmdArg{ "" };
        QString cmd{ "" };

        qint32 argIndex{ -1 };

        for ( int i = 0; i < commands.count(); ++i )
        {
            argIndex = -1;
            for ( int j = 0; j < commands.count(); ++j )
            {
                if ( packet.startsWith( "/cmd " % commands.at( j ),
                                      Qt::CaseInsensitive ) )
                {
                    argIndex = j;
                    break;
                }
            }

            if ( plr->getAdminRank() >= 0 )
            {
                //TODO: Check for sub-commands.
                cmd = commands.at( argIndex );

                cmdArg = Helper::getStrStr( packet, cmd, " ", "" );
                cmdReason = Helper::getStrStr( cmdArg, "", " ", "" );
                cmdArg = Helper::getStrStr( cmdArg, "", "", " " );

                switch ( argIndex )
                {
                    case CMDS::BAN:
                        {
//                            if ( !cmdArg.isEmpty() )
//                            {
//                                if ( !cmdArg.contains( "SOUL" ) )
//                                    cmdArg = Helper::serNumToIntStr( cmdArg );

//                                Player* plr{ nullptr };
//                                for ( int i = 0; i < MAX_PLAYERS; ++i )
//                                {
//                                    plr = server->getPlayer( i );
//                                    if ( plr != nullptr
//                                         && plr->getSernum_s() == cmdArg )
//                                    {
//                                        //TODO: More detailed Message to send to Banned Users.
//                                        server->sendMasterMessage(
//                                                    QString( "You have been banished by a remote administrator with the reason: "
//                                                             % cmdReason ),
//                                                    plr, false );
//                                        plr->setSoftDisconnect( true );
//                                    }
//                                }
//                                banDialog->addSerNumBan( cmdArg, cmdReason );
                                retn = true;
//                            }
                        }
                    break;
                    case CMDS::IPBAN:   //Disconnect all Users as required before banning the IP-Address.
                        {
//                            if ( !cmdArg.isEmpty() )
//                            {
//                                Player* plr{ nullptr };
//                                for ( int i = 0; i < MAX_PLAYERS; ++i )
//                                {
//                                    plr = server->getPlayer( i );
//                                    if ( plr != nullptr
//                                         && plr->getPublicIP() == cmdArg )
//                                    {
//                                        //TODO: More detailed Message to send to Banned Users.
//                                        server->sendMasterMessage(
//                                                    QString( "You have been banished by a remote administrator with the reason: "
//                                                             % cmdReason ),
//                                                    plr, false );
//                                        plr->setSoftDisconnect( true );
//                                    }
//                                }
//                                banDialog->addIPBan( cmdArg, cmdReason );
//                                retn = true;
//                            }
                        }
                    break;
                    case CMDS::KICK:
                        {
                            //Sub-Commands:
                            //  IP (if known)
                            //  SERNUM

                            //  ALL *IP or *SERNUM
                            //  The ALL command will remove all Users with the select IP or SERNUM.

                            //  If no SERNUM or IP is appended to the ALL command,
                            //  all connected Users will be disconnected.

                            retn = true;
                        }
                    break;
                    case CMDS::MUTE:
                        {
                            //Sub-Commands:
                            //  IP (if known)
                            //  SERNUM

                            //  ALL *IP or *SERNUM
                            //  The ALL command will mute all Users with the select IP or SERNUM.

                            //  If no SERNUM or IP is appended to the ALL command,
                            //  all connected Users will be muted.

                            retn = true;
                        }
                    break;
                    case CMDS::MSG:
                        {
                            //Sub-Commands:
                            //  IP (if known)
                            //  SERNUM

                            //  ALL *IP or *SERNUM
                            //  The ALL command forward the message to all
                            //  Users with the select IP or SERNUM.

                            //  If no SERNUM or IP is appended to the ALL command,
                            //  the message will be forwarded to all Users.

                            retn = true;
                        }
                    break;
                    default:
                    break;
                }
            }
            if ( argIndex >= 0 )
                break;
        }

        QString log{ "adminUsage.txt" };
        QString logMsg{ "Remote-Admin: [ %1 ] issued the command [ %2 ] with "
                        "argument [ %3 ] and reason [ %4 ]." };

        logMsg = logMsg.arg( plr->getSernum_s() )
                       .arg( cmd )
                       .arg( cmdArg )
                       .arg( cmdReason.trimmed() );
        Helper::logToFile( log, logMsg, true, true );
    }
    return retn;
}
