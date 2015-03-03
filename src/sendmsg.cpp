
#include "includes.hpp"
#include "sendmsg.hpp"
#include "ui_sendmsg.h"

SendMsg::SendMsg(QWidget *parent, ServerInfo* svr, Player* trg) :
    QDialog(parent),
    ui(new Ui::SendMsg)
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
    target = trg;
    server = svr;

    //Install EventFilters.
    this->installEventFilter( this );
    ui->msgEditor->installEventFilter( this );
}

SendMsg::~SendMsg()
{
    delete ui;
}

void SendMsg::on_sendMsg_clicked()
{
    if ( server == nullptr )
        return;

    QString message{ ui->msgEditor->toPlainText() };
    if ( message.isEmpty() )
    {
        this->close();
        return;
    }

    message = message.prepend( "Owner: " );
    if ( ui->checkBox->isChecked() )
        server->sendMasterMessage( message, nullptr, true );
    else
    {
        if ( target == nullptr )
            return;

        server->sendMasterMessage( message, target, false );
    }

    this->close();
}

bool SendMsg::eventFilter(QObject* obj, QEvent* event)
{
    if ( obj == nullptr || event == nullptr  )
        return false;

    QKeyEvent* key = static_cast<QKeyEvent*>( event );
    QCloseEvent* close = static_cast<QCloseEvent*>( event );
    if ( close != nullptr )
    {
        if ( close->type() == QEvent::Close )
        {
            close->accept();
            this->deleteLater();

            return true;
        }
    }
    else if ( key != nullptr )
    {
        switch ( key->key() )
        {
            case Qt::Key_Escape:
                this->close();
                return true;
            break;
            case Qt::Key_Enter:
            case Qt::Key_Return:
                {
                    event->accept();
                    emit ui->sendMsg->clicked();
                }
                return true;
            break;
        }
    }
    return QObject::eventFilter( obj, event );
}
