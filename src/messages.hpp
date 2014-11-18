#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include <QDialog>
#include <QString>

namespace Ui {
    class Messages;
}

class Messages : public QDialog
{
    Q_OBJECT
    Ui::Messages *ui;

    bool pwdHashed{ true };

    public:
        explicit Messages(QWidget *parent = 0);
        ~Messages();

        QString getServerMOTD();
        QString getBanishMsg();

        bool isPasswordEnabled();
        bool cmpPassword(QString& value);

    private slots:
        void on_saveSettings_clicked();
        void on_reloadSettings_clicked();
        void on_pwdEdit_textEdited(const QString&);
};

#endif // MESSAGES_HPP
