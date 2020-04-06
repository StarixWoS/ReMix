
//Class includes.
#include "sendmsg.hpp"
#include "ui_sendmsg.h"

//ReMix includes.
#include "settings.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QKeyEvent>

SendMsg::SendMsg(const QString& serNum, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SendMsg)
{
    ui->setupUi(this);

    QString title{ "Admin Message: [ %1 ]" };
    this->setWindowTitle( title.arg( serNum ) );

    if ( Settings::getSetting( SettingKeys::Setting, SettingSubKeys::SaveWindowPositions ).toBool() )
        this->restoreGeometry( Settings::getSetting( SettingKeys::Positions, this->metaObject()->className() ).toByteArray() );

    //Install EventFilters.
    this->installEventFilter( this );
    ui->msgEditor->installEventFilter( this );
}

SendMsg::~SendMsg()
{
    if ( Settings::getSetting( SettingKeys::Setting, SettingSubKeys::SaveWindowPositions ).toBool() )
        Settings::setSetting( this->saveGeometry(), SettingKeys::Positions, this->metaObject()->className() );

    delete ui;
}

bool SendMsg::sendToAll() const
{
    bool checked{ ui->checkBox->isChecked() };
    ui->checkBox->setChecked( false );

    return checked;
}

void SendMsg::on_sendMsg_clicked()
{
    QString message{ ui->msgEditor->toPlainText() };
    ui->msgEditor->clear();

    if ( message.isEmpty() )
    {
        this->close();
        return;
    }

    message = message.prepend( "Owner: " );
    Helper::stripNewlines( message );

    emit this->forwardMessageSignal( message );
    this->close();
}

bool SendMsg::eventFilter(QObject* obj, QEvent* event)
{
    if ( obj == nullptr || event == nullptr )
        return false;

    auto* key = dynamic_cast<QKeyEvent*>( event );
    bool accept{ false };

    if ( key != nullptr
      && key->type() == QEvent::KeyPress )
    {
        switch ( key->key() )
        {
            case Qt::Key_Escape:
                {
                    this->close();
                    event->accept();
                }
                accept = true;
            break;
            case Qt::Key_Enter:
            case Qt::Key_Return:
                {
                    emit ui->sendMsg->clicked();
                    event->accept();
                }
                accept = true;
            break;
            default:
                event->ignore();
            break;
        }
    }

    if ( accept )
        return accept;

    return QDialog::eventFilter( obj, event );
}
