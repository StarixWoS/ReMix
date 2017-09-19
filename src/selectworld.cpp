
//Class includes.
#include "selectworld.hpp"
#include "ui_selectworld.h"

//ReMix includes.
#include "settings.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QFileSystemModel>

SelectWorld::SelectWorld(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SelectWorld)
{
    ui->setupUi(this);

    QFileSystemModel* model{ new QFileSystemModel( this ) };
                      model->setFilter( QDir::NoDotAndDotDot | QDir::Dirs );
                      model->setRootPath( Settings::getWorldDir() );
                      model->setReadOnly( true );

    QObject::connect( model, &QFileSystemModel::directoryLoaded, model,
    [=]()
    {
        ui->worldViewer->setModel( model );
        for (int i = 1; i < model->columnCount(); ++i)
            ui->worldViewer->hideColumn( i );

        ui->worldViewer->setRootIndex( model->index( Settings::getWorldDir() ) );
    });
}

SelectWorld::~SelectWorld()
{
    delete ui;
}

QString SelectWorld::getSelectedWorld()
{
    return world;
}

void SelectWorld::on_worldViewer_activated(const QModelIndex& index)
{
    world = index.data().toString();
}

void SelectWorld::on_worldViewer_clicked(const QModelIndex& index)
{
    world = index.data().toString();
}

void SelectWorld::on_worldViewer_entered(const QModelIndex& index)
{
    world = index.data().toString();
}

void SelectWorld::on_cancelButton_clicked()
{
    this->reject();
    this->close();
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
