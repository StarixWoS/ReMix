
//Class includes.
#include "sendmsg.hpp"
#include "ui_sendmsg.h"

//ReMix includes.
#include "settings.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QKeyEvent>

SendMsg::SendMsg(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SendMsg)
{
    ui->setupUi(this);

    if ( Settings::getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        this->restoreGeometry( Settings::getSetting( SKeys::Positions, this->metaObject()->className() ).toByteArray() );

    //Install EventFilters.
    this->installEventFilter( this );
    ui->msgEditor->installEventFilter( this );
}

SendMsg::~SendMsg()
{
    if ( Settings::getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        Settings::setSetting( this->saveGeometry(), SKeys::Positions, this->metaObject()->className() );

    delete ui;
}

void SendMsg::setTitle(const QString& title)
{
    QString msg{ "Admin Message: [ %1 ]" };
    this->setWindowTitle( msg.arg( title ) );
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

    QKeyEvent* key{ dynamic_cast<QKeyEvent*>( event ) };
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
