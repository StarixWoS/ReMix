#ifndef USER_HPP
#define USER_HPP

#include "includes.hpp"
#include <QDialog>

namespace Ui {
    class User;
}

class User : public QDialog
{
    Q_OBJECT

    QSortFilterProxyModel* tblProxy{ nullptr };
    QStandardItemModel* tblModel{ nullptr };

    RandDev* randDev{ nullptr };
    static QSettings* userData;

    enum Keys{ kSEEN = 0, kBIO = 1, kIP = 2, kDV = 3, kWV = 4, kRANK = 5,
               kHASH = 6, kSALT = 7, kREASON = 8, kBANNED = 9 };

    enum Cols{ cSERNUM = 0, cSEEN = 1, cIP = 2, cRANK = 3, cBANNED = 4,
               cREASON = 5, cDATE = 6 };

    static const QString keys[ USER_KEY_COUNT ];

    public:
        explicit User(QWidget* parent = nullptr);
        ~User();

    enum Ranks{ rUSER = 0, rGAMEMASTER = 1, rCOADMIN = 2, rADMIN = 3,
                rOWNER = 4 };

    static void setData(const QString& key, const QString& subKey,
                        QVariant& value);
    static QVariant getData(const QString& key, const QString& subKey);

    bool makeAdmin(QString& sernum, QString& pwd);

    static bool getIsAdmin(QString& sernum);
    static bool getHasPassword(QString sernum);
    static bool cmpAdminPwd(QString& sernum, QString& value);

    static qint32 getAdminRank(QString& sernum);
    void setAdminRank(QString& sernum, qint32 rank);

    void removeBan(QString& value, qint32 type);
    bool addBan(Player* admin, Player* target, QString& reason,
                bool remote = false);

    static bool getIsBanned(QString value);

    void logBIO(QString& serNum, QHostAddress& ip, QString& dv,
                QString& wv, QString& bio);

    private:
        void loadUserInfo();

    private slots:
        void updateDataValue(const QModelIndex& index,
                             const QModelIndex&,
                             const QVector<int>& = QVector<int> ());
    private:
        Ui::User *ui;
};

#endif // USER_HPP
