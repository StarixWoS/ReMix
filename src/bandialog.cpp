
#include "includes.hpp"
#include "bandialog.hpp"
#include "ui_bandialog.h"

BanDialog::BanDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BanDialog)
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

    if ( Settings::getSaveWindowPositions() )
    {
        QByteArray geometry{ Settings::getWindowPositions(
                                    this->metaObject()->className() ) };
        if ( !geometry.isEmpty() )
        {
            this->restoreGeometry( Settings::getWindowPositions(
                                       this->metaObject()->className() ) );
        }
    }

    banWidget = new BanWidget( this );
    ui->widget->setLayout( new QGridLayout( ui->widget ) );
    ui->widget->layout()->setContentsMargins( 5, 5, 5, 5 );
    ui->widget->layout()->addWidget( banWidget );
}

BanDialog::~BanDialog()
{
    if ( Settings::getSaveWindowPositions() )
    {
        Settings::setWindowPositions( this->saveGeometry(),
                                      this->metaObject()->className() );
    }

    banWidget->deleteLater();
    delete ui;
}

void BanDialog::addBan(Player* plr, QString& reason)
{
    QString msg{ reason };
    if ( msg.isEmpty() )
    {
        msg = "Manual-Banish; Unknown reason: [ %1 ]";
        msg = msg.arg( plr->getSernum_s() );
    }
    banWidget->addBan( plr, msg );
}

void BanDialog::removeBan(QString& value, qint32 type)
{
    banWidget->removeBan( value, type );
}

void BanDialog::remoteAddBan(Player* admin, Player* target, QString& reason)
{
    QString msg{ reason };
    if ( msg.isEmpty() )
    {
        msg = "Remote-Banish by [ %1 ]; Unknown Reason: [ %2 ]";
        msg = msg.arg( admin->getSernum_s() )
                 .arg( target->getSernum_s() );
    }
    banWidget->addBan( target, msg );
}

bool BanDialog::getIsBanned(QString value)
{
    return BanWidget::getIsBanned( value ) >= 0;
}
