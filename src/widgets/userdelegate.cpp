
//Class includes.
#include "userdelegate.hpp"

//Qt Includes.
#include <QComboBox>
#include <QPainter>

UserDelegate::UserDelegate(QObject* parent) : QItemDelegate(parent)
{}

UserDelegate::~UserDelegate() = default;

void UserDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->save();
    QStyleOptionViewItem opt = QItemDelegate::setOptions( index, option );

    QString text{ "" };
    switch( static_cast<UserCols>( index.column() ) )
    {
        case UserCols::Rank:
            {
                QItemDelegate::drawBackground( painter, opt, index );
                switch( index.data().toInt() )
                {
                    case 0:
                        text = "User";
                    break;
                    case 1:
                        text = "Game Master";
                    break;
                    case 2:
                        text = "Co-Admin";
                    break;
                    case 3:
                        text = "Admin";
                    break;
                    case 4:
                        text = "Owner";
                    break;
                    case 5:
                        text = "Creator";
                    break;
                    default:
                        text = "User";
                    break;
                }
                QItemDelegate::drawDisplay( painter, opt, option.rect, text );
            }
        break;
        case UserCols::Muted:
            {
                {
                    QItemDelegate::drawBackground( painter, opt, index );
                    switch( index.data().toInt() )
                    {
                        case 0:
                            text = "Not Muted";
                        break;
                        case 1:
                            text = "Muted";
                        break;
                    }
                    QItemDelegate::drawDisplay( painter, opt, option.rect, text );
                }
            }
        break;
        case UserCols::Banned:
            {
                {
                    QItemDelegate::drawBackground( painter, opt, index );
                    switch( index.data().toInt() )
                    {
                        case 0:
                            text = "Not Banned";
                        break;
                        case 1:
                            text = "Banned";
                        break;
                    }
                    QItemDelegate::drawDisplay( painter, opt, option.rect, text );
                }
            }
        break;
        default:
            QItemDelegate::paint( painter, option, index );
        break;
    }
    painter->restore();
}

void UserDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QComboBox* combobox{ dynamic_cast<QComboBox*>( editor ) };
    switch( index.data().toInt() )
    {
        case 0:
            combobox->setCurrentIndex( 0 ); //User & Not Banned
        break;
        case 1:
            combobox->setCurrentIndex( 1 ); //Game Master & Banned
        break;
        case 2:
            combobox->setCurrentIndex( 2 ); //Co-Admin
        break;
        case 3:
            combobox->setCurrentIndex( 3 ); //Admin
        break;
        case 4:
            combobox->setCurrentIndex( 4 ); //Owner
        break;
        default:
            combobox->setCurrentIndex( 0 ); //User -- Default
        break;
    }
}

QWidget* UserDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const
{
    QComboBox* editor{ new QComboBox( parent ) };
               editor->setFocusPolicy( Qt::StrongFocus );

    if ( index.column() == *UserCols::Rank )
    {
        editor->addItem( "User" );
        editor->addItem( "Game Master" );
        editor->addItem( "Co-Admin" );
        editor->addItem( "Admin" );
        editor->addItem( "Owner" );
        editor->addItem( "Creator" );
    }
    else if ( index.column() == *UserCols::Muted )
    {
        editor->addItem( "Not Muted" );
        editor->addItem( "Muted" );
    }
    else if ( index.column() == *UserCols::Banned )
    {
        editor->addItem( "Not Banned" );
        editor->addItem( "Banned" );
    }
    else
        return nullptr;

    return editor;
}

void UserDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QComboBox* combobox{ dynamic_cast<QComboBox*>( editor ) };

    int value{ combobox->currentIndex() };
    switch( value )
    {
        case 0:
            model->setData( index, 0 ); //User & Not Banned
        break;
        case 1:
            model->setData( index, 1 ); //Game Master & Banned
        break;
        case 2:
            model->setData( index, 2 ); //Co-Admin
        break;
        case 3:
            model->setData( index, 3 ); //Admin
        break;
        case 4:
            model->setData( index, 4 ); //Owner
        break;
        case 5:
            model->setData( index, 5 ); //Creator
        break;
        default:
            model->setData( index, 0 ); //User -- Default
        break;
    }
}

void UserDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&) const
{
    editor->setGeometry( option.rect );
}
