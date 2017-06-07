
#include "selectworld.hpp"
#include "includes.hpp"

#include "ui_selectworld.h"

SelectWorld::SelectWorld(QWidget *parent, QStringList worldList) :
    QDialog(parent),
    ui(new Ui::SelectWorld)
{
    ui->setupUi(this);

    worldModel = new QStringListModel();
    worldModel->setStringList( worldList );

    ui->worldViewer->setModel( worldModel );

    //Remove the "Help" button from the window title bars.
    {
        QIcon icon = this->windowIcon();
        Qt::WindowFlags flags = this->windowFlags();
        flags &= ~Qt::WindowContextHelpButtonHint;

        this->setWindowFlags( flags );
        this->setWindowIcon( icon );
    }

}

SelectWorld::~SelectWorld()
{
    delete ui;
}

QString SelectWorld::getSelectedWorld()
{
    return world;
}

void SelectWorld::on_worldViewer_clicked(const QModelIndex &index)
{
    world = index.data().toString();
}

void SelectWorld::on_okButton_clicked()
{
    QString title{ "Select World:" };
    QString  prompt{ "Are you certain that you want to select the "
                     "world [ %1 ]?" };

    prompt = prompt.arg( this->getSelectedWorld() );
    if ( Helper::confirmAction( this, title, prompt ) )
    {
        this->accept();
    }
}

void SelectWorld::on_cancelButton_clicked()
{
    this->reject();
    this->close();
}
