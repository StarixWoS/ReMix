#ifndef SELECTWORLD_HPP
#define SELECTWORLD_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QFileSystemModel>
#include <QDialog>

namespace Ui {
    class SelectWorld;
}

class SelectWorld : public QDialog
{
        Q_OBJECT

        static QFileSystemModel* model;

        QString world{ "" };
        bool requireWorld{ false };

    public:
        explicit SelectWorld(QWidget* parent = nullptr);
        ~SelectWorld() override;

        QString& getSelectedWorld();
        void setRequireWorld(const bool& value);

    private slots:
        void on_worldViewer_activated(const QModelIndex& index);
        void on_worldViewer_clicked(const QModelIndex& index);
        void on_worldViewer_entered(const QModelIndex& index);
        void on_cancelButton_clicked();
        void on_okButton_clicked();
        void directoryLoadedSlot(const QString&);

    private:
        Ui::SelectWorld* ui;
};

#endif // SELECTWORLD_HPP
