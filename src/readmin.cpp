
#include "readmin.hpp"
#include "ui_readmin.h"

#include "helper.hpp"

namespace Admin
{
    void setAdminData(const QString& key, const QString& subKey, QVariant& value)
    {
        QSettings adminData( "adminData.ini", QSettings::IniFormat );
        adminData.setValue( key + "/" + subKey, value );
    }

    QVariant getAdminData(const QString& key, const QString& subKey)
    {
        QSettings adminData( "adminData.ini", QSettings::IniFormat );
        if ( subKey == "rank" )
            return adminData.value( key + "/" + subKey, -1 );
        else
            return adminData.value( key + "/" + subKey );
    }

    void setReqAdminAuth(QVariant& value)
    {
        Helper::setSetting( Helper::keys[ Helper::Options ],
                            Helper::subKeys[ Helper::ReqAdminAuth ], value );
    }

    bool getReqAdminAuth()
    {
        return Helper::getSetting( Helper::keys[ Helper::Options ],
                                   Helper::subKeys[ Helper::ReqAdminAuth ] ).toBool();
    }

    bool getIsRemoteAdmin(QString& serNum)
    {
        return getAdminData( serNum, adminKeys[ Admin::RANK ] ).toInt() >= 0;
    }

    bool cmpRemoteAdminPwd(QString& serNum, QVariant& value)
    {
        QString recSalt = getAdminData( serNum, adminKeys[ Admin::SALT ] ).toString();
        QString recHash = getAdminData( serNum, adminKeys[ Admin::HASH ] ).toString();

        QVariant pwd( value.toString() + recSalt );
        QString hash = Helper::hashPassword( pwd );

        return hash == recHash;
    }
}

ReAdmin::ReAdmin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    //Setup our Random Device
    randDev = new RandDev();

    //Create our Context Menus
    contextMenu = new QMenu( this );
    rankMenu = new QMenu( this );

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

    this->setWindowModality( Qt::WindowModal );

    this->loadServerAdmins();
    this->initContextMenu();
}

ReAdmin::~ReAdmin()
{
    rankMenu->deleteLater();
    contextMenu->deleteLater();
    delete ui;
}

void ReAdmin::loadServerAdmins()
{
    if ( QFile( "adminData.ini" ).exists() )
    {
        QSettings adminData( "adminData.ini", QSettings::IniFormat );
        QStringList groups = adminData.childGroups();

        QString group;

        QString hash{ "" };
        QString salt{ "" };
        int rank{ 0 };

        int row = -1;
        for ( int i = 0; i < groups.count(); ++i )
        {
            group = groups.at( i );

            rank = adminData.value( group + "/rank", 0 ).toInt();
            hash = adminData.value( group + "/hash", "" ).toString();
            salt = adminData.value( group + "/salt", "" ).toString();

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

void ReAdmin::initContextMenu()
{
    rankMenu->addAction( ui->actionGameMaster );
    rankMenu->addAction( ui->actionCoAdmin );
    rankMenu->addAction( ui->actionAdmin );
    rankMenu->addAction( ui->actionOwner );

    ui->actionChangeRank->setMenu( rankMenu );

    contextMenu->addAction( ui->actionRevokeAdmin );
    contextMenu->addAction( ui->actionChangeRank );

}

void ReAdmin::setAdminRank(int rank, QModelIndex index)
{
    QSettings adminData( "adminData.ini", QSettings::IniFormat );
    if ( index.isValid() )
    {
        QString txt = tableModel->data( tableModel->index( index.row(), 0 ) ).toString();
        if ( !txt.isEmpty() )
        {
            adminData.setValue( txt + "/rank", rank );
            tableModel->setData( tableModel->index( index.row(), 1 ), rank, Qt::DisplayRole );
        }
    }
    menuIndex = QModelIndex();
}

void ReAdmin::on_makeAdmin_clicked()
{
    QSettings adminData( "adminData.ini", QSettings::IniFormat );

    QString serNum = ui->adminSerNum->text().simplified();
    QString adminPwd = ui->adminPwd->text();

    if ( !serNum.isEmpty()
      && !adminPwd.isEmpty()
      && !adminData.childGroups().contains( serNum, Qt::CaseInsensitive ) ) //Prevent overwriting an Admin's Data.
    {
        QString salt = Helper::intToStr( randDev->genRandNum( 0x10000000, 0x7FFFFFFF ), 16, 8 );

        QStringList groups = adminData.childGroups();
        QString j{ "" };
        for ( int i = 0; i < groups.count(); ++i )
        {
            j = adminData.value( groups.at( i ) + "/salt" ).toString();

            //Check if the Salt is already used.
            if ( j == salt )
            {   //If so, generate another and restart the loop.
                salt = Helper::intToStr( randDev->genRandNum( 0x10000000, 0x7FFFFFFF ), 16, 8 );
                i = 0;
            }
        }
        adminPwd += salt;

        QVariant hash( adminPwd );
                 hash = Helper::hashPassword( hash );

        adminData.setValue( serNum + "/rank", ui->comboBox->currentIndex() );
        adminData.setValue( serNum + "/hash", hash );
        adminData.setValue( serNum + "/salt", salt );

        int row = tableModel->rowCount();
        tableModel->insertRow( row );

        tableModel->setData( tableModel->index( row, 0 ), serNum, Qt::DisplayRole );
        tableModel->setData( tableModel->index( row, 1 ), ui->comboBox->currentIndex(), Qt::DisplayRole );
        tableModel->setData( tableModel->index( row, 2 ), hash, Qt::DisplayRole );
        tableModel->setData( tableModel->index( row, 3 ), salt, Qt::DisplayRole );

        ui->adminTable->selectRow( row );
    }
    ui->adminTable->resizeColumnsToContents();
}

void ReAdmin::on_adminTable_customContextMenuRequested(const QPoint& pos)
{
    menuIndex = tableProxy->mapToSource( ui->adminTable->indexAt( pos ) );
    if ( menuIndex.row() < 0 )
        return;

    contextMenu->popup( ui->adminTable->viewport()->mapToGlobal( pos ) );
}

void ReAdmin::on_actionRevokeAdmin_triggered()
{
    if ( menuIndex.isValid() )
    {
        QString txt = tableModel->data( tableModel->index( menuIndex.row(), 0 ) ).toString();
        if ( !txt.isEmpty() )
        {
            if ( QMessageBox::question( this, "Revoke Admin:", "Are you certain you want to REVOKE ( "
                                        + txt + " )'s powers?",
                                        QMessageBox::Yes | QMessageBox::No,
                                        QMessageBox::No ) == QMessageBox::Yes )
            {
                QSettings adminData( "adminData.ini", QSettings::IniFormat );
                adminData.remove( txt );

                tableModel->removeRow( menuIndex.row() );
            }
        }
    }
    menuIndex = QModelIndex();
}

void ReAdmin::on_actionGameMaster_triggered()
{
    this->setAdminRank( Ranks::GMASTER, menuIndex );
}

void ReAdmin::on_actionCoAdmin_triggered()
{
    this->setAdminRank( Ranks::COADMIN, menuIndex );
}

void ReAdmin::on_actionAdmin_triggered()
{
    this->setAdminRank( Ranks::ADMIN, menuIndex );
}

void ReAdmin::on_actionOwner_triggered()
{
    this->setAdminRank( Ranks::OWNER, menuIndex );
}
