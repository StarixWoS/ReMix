
//Class includes.
#include "colorwidget.hpp"
#include "ui_colorwidget.h"

//ReMix includes.
#include "settings.hpp"
#include "server.hpp"
#include "helper.hpp"
#include "theme.hpp"

//Qt Includes.
#include <QStandardItemModel>
#include <QTableView>
#include <QColorDialog>

ColorWidget* ColorWidget::instance;
const QStringList ColorWidget::rowList
{
    "Gossip Chat",
    "Shout Chat",
    "Emote Chat",
    "Player Chat",
    "Owner Chat",
    "Comment Text",
    "Golden Soul",
    "White Soul",
    "Player Name",
    "Owner Name",
    "Timestamp",
    "Admin Valid",
    "Admin Invalid",
    "IP Valid",
    "IP Invalid",
    "IP Vanished",
    "Party Join",
    "PK Challenge",
    "Soul Incarnated",
    "Soul Left World",
};

ColorWidget::ColorWidget() :
    ui(new Ui::ColorWidget)
{
    ui->setupUi(this);

    colorModel = new QStandardItemModel( 0, 3, nullptr );
    colorModel->setHeaderData( 0, Qt::Horizontal, "Color Role" );
    colorModel->setHeaderData( 1, Qt::Horizontal, "Color Value" );
    colorModel->setHeaderData( 2, Qt::Horizontal, "Color" );

    ui->colorView->setModel( colorModel );

    for ( const QString& role : rowList )
    {
        const qint32 row{ colorModel->rowCount() };
        QStandardItem* item{ new QStandardItem() };
        colorModel->insertRow( colorModel->rowCount(), item );

        colorModel->setData( colorModel->index( row, 0 ), role, Qt::DisplayRole );
    }

    for ( int row = 0; row < colorModel->rowCount(); ++row )
    {
        for ( int col = 0; col < colorModel->columnCount(); ++col )
        {
            QModelIndex index = colorModel->index( row, col, QModelIndex() );
            if ( col == 1 || col == 2 )
            {
                QStandardItem* item = colorModel->itemFromIndex( index );
                               item->setFlags( item->flags() & Qt::ItemIsEnabled ); //Disable the Item within Column "col".
            }
        }
    }
    this->loadColors();

    QObject::connect( this, &ColorWidget::colorOverrideSignal, Theme::getInstance(), &Theme::colorOverrideSlot );
    QObject::connect( Theme::getInstance(), &Theme::themeChangedSignal, this,
    [=]()
    {
        this->loadColors();
        ui->colorView->setPalette( Theme::getCurrentPal() );
    } );

    ui->colorView->setAutoFillBackground( true );
}

ColorWidget::~ColorWidget()
{
    delete ui;
}

void ColorWidget::loadColors()
{
    for ( int i = 0; i <= static_cast<int>( Colors::ColorCount ); ++i )
    {
        const QColor color{ Theme::getColor( static_cast<Colors>( i ) ) };

        colorModel->setData( colorModel->index( i, 1 ), color.name(), Qt::DisplayRole );
        colorModel->setData( colorModel->index( i, 2 ), color, Qt::BackgroundRole );
    }
    ui->colorView->resizeColumnsToContents();
}

ColorWidget* ColorWidget::getInstance()
{
    if ( instance == nullptr )
        instance = new ColorWidget();

    return instance;
}

void ColorWidget::deleteInstance()
{
    if ( instance != nullptr )
    {
        instance->disconnect();
        instance->deleteLater();
    }
}

void ColorWidget::selectColor(int currentRow)
{
    const QModelIndex index{ colorModel->index( currentRow, 1 ) };
    QColor defaultColor{ Qt::white };
    if ( index.isValid() )
        defaultColor = index.data( Qt::DisplayRole ).toString();

    const QColor color = QColorDialog::getColor( defaultColor, this, "Select Color Override:" );

    if ( color.isValid() )
    {
        colorModel->setData( colorModel->index( currentRow, 1 ), color.name(), Qt::DisplayRole );
        colorModel->setData( colorModel->index( currentRow, 2 ), color, Qt::BackgroundRole );
        emit this->colorOverrideSignal( static_cast<Colors>( currentRow ), color.name() );
    }
}

void ColorWidget::on_resetColor_clicked()
{
    QModelIndex index{ ui->colorView->currentIndex() };
    if ( index.isValid() )
    {
        const Colors colorRole{ static_cast<Colors>( index.row() ) };
        const QString colorString{ Theme::getDefaultColor( Theme::getThemeType(), colorRole ) };

        colorModel->setData( colorModel->index( index.row(), 1 ), colorString, Qt::DisplayRole );
        colorModel->setData( colorModel->index( index.row(), 2 ), QColor( colorString ), Qt::BackgroundRole );

        emit this->colorOverrideSignal( colorRole, colorString );
    }
}

void ColorWidget::on_colorView_doubleClicked(const QModelIndex& index)
{
    this->selectColor( index.row() );
}

void ColorWidget::on_selectColor_clicked()
{
    this->selectColor( ui->colorView->currentIndex().row() );
}
