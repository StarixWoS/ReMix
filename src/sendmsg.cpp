
#include "includes.hpp"
#include "sendmsg.hpp"
#include "ui_sendmsg.h"

SendMsg::SendMsg(QString serNum, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendMsg)
{
    ui->setupUi(this);

    QString title{ "Admin Message: [ %1 ]" };
    this->setWindowTitle( title.arg( serNum ) );

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

    //Install EventFilters.
    this->installEventFilter( this );
    ui->msgEditor->installEventFilter( this );
}

SendMsg::~SendMsg()
{
    if ( Settings::getSaveWindowPositions() )
    {
        Settings::setWindowPositions( this->saveGeometry(),
                                      this->metaObject()->className() );
    }
    delete ui;
}

bool SendMsg::sendToAll()
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

    emit this->forwardMessage( message );
    this->close();
}

bool SendMsg::eventFilter(QObject* obj, QEvent* event)
{
    if ( obj == nullptr || event == nullptr  )
        return false;

    QKeyEvent* key = static_cast<QKeyEvent*>( event );
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

    return QObject::eventFilter( obj, event );
}
