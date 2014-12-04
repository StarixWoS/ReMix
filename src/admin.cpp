
#include "admin.hpp"
#include "ui_admin.h"

#include "bandialog.hpp"
#include "helper.hpp"
#include "player.hpp"
#include "serverinfo.hpp"

//class ChatCommands
//{
//    QString cmd{ "" };
//    qint32 rank{ 0 };
//    QString cmdHelp{ "" };

//    public:
//        QString getCmd() const;
//        void setCmd(const QString& value);

//        qint32 getRank() const;
//        void setRank(const qint32& value);

//        QString getCmdHelp() const;
//        void setCmdHelp(const QString& value);
//};

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

    //Proxy model to support sorting without actually altering the underlying model
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

            tableModel->setData( tableModel->index( row, 0 ), group, Qt::DisplayRole );
            tableModel->setData( tableModel->index( row, 1 ), rank, Qt::DisplayRole );
            tableModel->setData( tableModel->index( row, 2 ), hash, Qt::DisplayRole );
            tableModel->setData( tableModel->index( row, 3 ), salt, Qt::DisplayRole );
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
        QString txt = tableModel->data( tableModel->index( index.row(), 0 ) ).toString();
        if ( !txt.isEmpty() )
        {
            adminData.setValue( txt % "/rank", rank );
            tableModel->setData( tableModel->index( index.row(), 1 ), rank, Qt::DisplayRole );
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

    if ( !sernum.isEmpty()
      && !pwd.isEmpty() )
    {
        //We already have an admin using this sernum on record. Return false. (Perhaps inform the would-be admin.)
        if ( adminData.childGroups().contains( sernum, Qt::CaseInsensitive ) )
            return false;

        QString salt = Helper::intToStr( randDev->genRandNum( 0x10000000, 0x7FFFFFFF ), 16, 8 );

        QStringList groups = adminData.childGroups();
        QString j{ "" };
        for ( int i = 0; i < groups.count(); ++i )
        {
            j = adminData.value( groups.at( i ) % "/salt" ).toString();

            //Check if the Salt is already used.
            if ( j == salt )
            {   //If so, generate another and restart the loop.
                salt = Helper::intToStr( randDev->genRandNum( 0x10000000, 0x7FFFFFFF ), 16, 8 );
                i = 0;
            }
        }

        QVariant hash( salt + pwd );
                 hash = Helper::hashPassword( hash );

        adminData.setValue( sernum % "/rank", ui->comboBox->currentIndex() );
        adminData.setValue( sernum % "/hash", hash );
        adminData.setValue( sernum % "/salt", salt );

        int row = tableModel->rowCount();
        tableModel->insertRow( row );

        tableModel->setData( tableModel->index( row, 0 ), sernum, Qt::DisplayRole );
        tableModel->setData( tableModel->index( row, 1 ), ui->comboBox->currentIndex(), Qt::DisplayRole );
        tableModel->setData( tableModel->index( row, 2 ), hash, Qt::DisplayRole );
        tableModel->setData( tableModel->index( row, 3 ), salt, Qt::DisplayRole );

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
        QString sernum = tableModel->data( tableModel->index( menuIndex.row(), 0 ) ).toString();
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
    QString sernum = tableModel->data( tableModel->index( menuIndex.row(), 0 ) ).toString();

    quint32 rank = -1;
    if ( !sernum.isEmpty() )
        rank = AdminHelper::changeRemoteAdminRank( this, sernum );

    tableModel->setData( tableModel->index( menuIndex.row(), 1 ),
                                            rank, Qt::DisplayRole );
}

//Handle Admin commands.
bool Admin::parseCommand(QString& packet, Player* plr)
{
    if ( packet.isEmpty()
      || plr == nullptr )
    {
        return false;
    }
    return true;
}
