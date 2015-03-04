
#include "includes.hpp"
#include "admin.hpp"
#include "ui_admin.h"

//Initialize QStrings.
const QString Admin::adminKeys[ 3 ] =
{
    "rank", "hash", "salt"
};

const QStringList Admin::ranks
{
    QStringList() << "Removed" << "Game Master" << "Co-Admin"
                  << "Admin" << "Owner"
};

Admin::Admin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Admin)
{
    ui->setupUi(this);

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
    tableProxy = new AdminSortProxyModel();
    tableProxy->setDynamicSortFilter( true );
    tableProxy->setSourceModel( tableModel );
    tableProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->adminTable->setModel( tableProxy );

    //Setup Objects.
    banDialog = new BanDialog( parent );

    this->loadServerAdmins();
    this->initContextMenu();

    //Install Event Filter to enable Row-Deslection.
    ui->adminTable->viewport()->installEventFilter(
                new TblEventFilter( ui->adminTable,
                                    tableProxy ) );
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
    tableProxy->removeRows( 0, tableProxy->rowCount() );
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
                                 ranks.at( rank ), Qt::DisplayRole );

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
    QString pwd = ui->adminPwd->text();
    QString sernum = ui->adminSerNum->text().simplified();
            sernum = Helper::sanitizeSerNum( sernum );

    if ( !sernum.isEmpty() && !pwd.isEmpty() )
    {
        if ( this->makeAdmin( sernum, pwd ) )
        {
            ui->adminSerNum->clear();
            ui->adminPwd->clear();
        }
    }
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
        //Remote Admins are based on Rank, not whether they're recorded.
        if ( this->getIsRemoteAdmin( sernum ) )
            return false;

        QString salt = Helper::genPwdSalt( randDev, SALT_LENGTH );
        QString hash( salt + pwd );
                hash = Helper::hashPassword( hash );

        qint32 rank{ ui->comboBox->currentIndex() + 1 };
        adminData.setValue( sernum % "/rank", rank );
        adminData.setValue( sernum % "/hash", hash );
        adminData.setValue( sernum % "/salt", salt );

        this->loadServerAdmins();
        return true;
    }
    return false;
}

void Admin::on_adminTable_customContextMenuRequested(const QPoint& pos)
{
    menuIndex = tableProxy->mapToSource(
                    ui->adminTable->indexAt( pos ) );
    if ( menuIndex.row() < 0 )
        return;

    contextMenu->popup(
                ui->adminTable->viewport()->mapToGlobal( pos ) );
}

void Admin::on_actionRevokeAdmin_triggered()
{
    if ( menuIndex.isValid() )
    {
        QString sernum = tableModel->data(
                             tableModel->index(
                                 menuIndex.row(), 0 ) ).toString();
                sernum = Helper::sanitizeSerNum( sernum );

        if ( !sernum.isEmpty() )
        {
            if ( this->deleteRemoteAdmin( this, sernum ) )
                this->loadServerAdmins();
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
                sernum = Helper::sanitizeSerNum( sernum );

        if ( !sernum.isEmpty() )
        {
            if ( this->changeRemoteAdminRank( this, sernum ) >= 0 )
                this->loadServerAdmins();
        }
    }
}

void Admin::setAdminData(const QString& key, const QString& subKey,
                         QVariant& value)
{
    QSettings adminData( "adminData.ini", QSettings::IniFormat );

    adminData.setValue( key % "/" % subKey, value );
}

QVariant Admin::getAdminData(const QString& key, const QString& subKey)
{
    QSettings adminData( "adminData.ini", QSettings::IniFormat );

    if ( subKey == adminKeys[ Admin::RANK ] )
        return adminData.value( key % "/" % subKey, 0 );

    return adminData.value( key % "/" % subKey );
}

bool Admin::getIsRemoteAdmin(QString& serNum)
{
    return getAdminData( serNum, adminKeys[ Admin::RANK ] )
              .toInt() >= 1;
}

bool Admin::cmpRemoteAdminPwd(QString& serNum, QString& value)
{
    QString recSalt = getAdminData( serNum, adminKeys[ Admin::SALT ] )
                              .toString();
    QString recHash = getAdminData( serNum, adminKeys[ Admin::HASH ] )
                              .toString();

    QString hash{ recSalt + value };
            hash = Helper::hashPassword( hash );

    return hash == recHash;
}

qint32 Admin::getRemoteAdminRank(QString& sernum)
{
    return getAdminData( sernum, adminKeys[ Admin::RANK ] )
              .toUInt();
}

void Admin::setRemoteAdminRank(QString& sernum, qint32 rank)
{
    QVariant value{ rank };
    setAdminData( sernum, adminKeys[ Admin::RANK ], value );
}

qint32 Admin::changeRemoteAdminRank(QWidget* parent, QString& sernum)
{
    bool ok{ false };
    QString item = QInputDialog::getItem( parent, "Admin Rank:",
                                          "Rank:", ranks, 0, false, &ok );
    qint32 rank{ -1 };
    if ( ok && !item.isEmpty() )
    {
        rank = ranks.indexOf( item );

        setRemoteAdminRank( sernum, rank );
    }
    return rank;
}

bool Admin::deleteRemoteAdmin(QWidget* parent, QString& sernum)
{
    QString title{ "Revoke Admin:" };
    QString prompt{ "Are you certain you want to REVOKE [ "
                   % Helper::serNumToIntStr( sernum )
                   % " ]'s powers?" };

    if ( Helper::confirmAction( parent, title, prompt ) )
    {
        setRemoteAdminRank( sernum, 0 );
        return true;
    }
    return false;
}

bool Admin::createRemoteAdmin(QWidget* parent, QString& sernum)
{
    sernum = Helper::serNumToIntStr( sernum );
    QString title{ "Create Admin:" };
    QString prompt{ "Are you certain you want to MAKE [ %1 ] a Remote Admin?"
                    "\r\n\r\nPlease make sure you trust [ %2 ] as this will "
                    "allow the them to utilize Admin commands that can remove "
                    "the ability for other users to connect to the Server." };
    prompt = prompt.arg( sernum )
                   .arg( sernum );

    if ( Helper::confirmAction( parent, title, prompt ) )
        return true;

    return false;
}

//bool Admin::remoteChangeRank(QString& sernum, qint32 rank)
//{
//    if ( !getIsRemoteAdmin( sernum ) )
//    {
//        QSettings adminData( "adminData.ini", QSettings::IniFormat );
//                  adminData.setValue( sernum % "/rank", rank );
//        return true;
//    }
//    return false;
//}

//bool Admin::remoteRemoveAdmin(QString& sernum)
//{
//    if ( getIsRemoteAdmin( sernum ) )
//    {
//        QSettings adminData( "adminData.ini", QSettings::IniFormat );
//                  adminData.remove( Helper::serNumToHexStr( sernum, 8 ) );
//        return true;
//    }
//    return false;
//}
