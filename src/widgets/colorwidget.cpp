
//Class includes.
#include "colorwidget.hpp"
#include "ui_colorwidget.h"

//ReMix includes.
#include "settings.hpp"
#include "server.hpp"
#include "helper.hpp"
#include "theme.hpp"

//Qt Includes.
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

    ui->colorWidget->insertItems( 0, rowList );

    QObject::connect( this, &ColorWidget::colorOverrideSignal, Theme::getInstance(), &Theme::colorOverrideSlot );
    QObject::connect( Theme::getInstance(), &Theme::themeChangedSignal, this,
    [=]()
    {
        ui->colorWidget->clearSelection();
        currentIndex = QModelIndex();
        this->setColorView( QColor() );
    } );

    ui->colorView->setAutoFillBackground( true );
}

ColorWidget::~ColorWidget()
{
    delete ui;
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
    const QColor color = QColorDialog::getColor( Qt::white, this, "Select Color Override:" );

    if ( color.isValid() )
    {
        this->setColorView( color );

        emit this->colorOverrideSignal( static_cast<Colors>( currentRow ), color.name() );
    }
}

void ColorWidget::setColorView(const QColor& color)
{
    if ( color.isValid() )
    {
        ui->colorView->setText( color.name() );
        ui->colorView->setPalette( QPalette( color ) );
    }
    else
    {
        ui->colorView->setText( "#0000000" );
        ui->colorView->setPalette( Theme::getDefaultPal() );
    }
}

void ColorWidget::on_colorWidget_currentRowChanged(int currentRow)
{
    currentIndex = ui->colorWidget->indexFromItem( ui->colorWidget->item( currentRow ) );

    const QColor color{ Theme::getColor( static_cast<Colors>( currentRow ) ) };
    this->setColorView( color );
}

void ColorWidget::on_resetColor_clicked()
{
    if ( currentIndex.isValid() )
    {
        const Colors colorRole{ static_cast<Colors>( currentIndex.row() ) };
        const QString colorString{ Theme::getDefaultColor( Theme::getThemeType(), colorRole ) };

        this->setColorView( QColor( colorString ) );
        emit this->colorOverrideSignal( colorRole, colorString );
    }
}

void ColorWidget::on_colorWidget_doubleClicked(const QModelIndex& index)
{
    currentIndex = index;
    this->selectColor( index.row() );
}


void ColorWidget::on_selectColor_clicked()
{
    this->selectColor( ui->colorWidget->currentRow() );
}

