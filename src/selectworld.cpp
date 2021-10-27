
//Class includes.
#include "selectworld.hpp"
#include "ui_selectworld.h"

//ReMix includes.
#include "settings.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QFileSystemModel>
#include <QDebug>

QFileSystemModel* SelectWorld::model{ nullptr };

SelectWorld::SelectWorld(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SelectWorld)
{
    ui->setupUi(this);

    if ( model == nullptr )
    {
        model = new QFileSystemModel( this );
        QObject::connect( model, &QFileSystemModel::directoryLoaded, model,
        [=, this]()
        {
            ui->worldViewer->setModel( model );
            for (int i = 1; i < model->columnCount(); ++i)
                ui->worldViewer->hideColumn( i );

            ui->worldViewer->setRootIndex( model->index( Settings::getSetting( SKeys::Setting, SSubKeys::WorldDir ).toString() ) );
        } );

        model->setFilter( QDir::NoDotAndDotDot | QDir::Dirs );
        model->setRootPath( Settings::getSetting( SKeys::Setting, SSubKeys::WorldDir ).toString() );
        model->setReadOnly( true );
    }
    this->setWindowModality( Qt::ApplicationModal );
}

SelectWorld::~SelectWorld()
{
    model->disconnect();
    model->deleteLater();
    model = nullptr;

    delete ui;
}

QString& SelectWorld::getSelectedWorld()
{
    return world;
}

void SelectWorld::setRequireWorld(const bool& value)
{
    requireWorld = value;
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
    QString  prompt{ "Are you certain that you want to select the world [ %1 ]?" };

    QString world{ this->getSelectedWorld() };
    bool accept{ true };

    if ( world.isEmpty() )
    {
        prompt = "Are you certain that you want to unselect the current world?";

        if ( !requireWorld )
            accept = false;
    }
    else
        prompt = prompt.arg( world );

    if ( accept )
    {
        if ( Helper::confirmAction( this, title, prompt ) )
            this->accept();
    }
    else
    {
        this->reject();
        this->close();
    }
}
