#ifndef CREATEINSTANCE_H
#define CREATEINSTANCE_H

#include "prototypes.hpp"

//Required Qt Includes.
#include <QDialog>

namespace Ui {
    class CreateInstance;
}

class CreateInstance : public QDialog
{
        Q_OBJECT

        QString serverArgs{ "" };

        static const QString gameNames[ GAME_NAME_COUNT ];

    public:
        explicit CreateInstance(QWidget *parent = 0);
        ~CreateInstance();

        QString getServerArgs() const;
        void setServerArgs(const QString& value);

    private slots:
        void on_initializeServer_clicked();
        void on_close_clicked();

        void closeEvent(QCloseEvent* event);

    private:
        Ui::CreateInstance *ui;
};

#endif // CREATEINSTANCE_H
