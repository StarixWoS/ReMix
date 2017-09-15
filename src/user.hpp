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

    static QSortFilterProxyModel* tblProxy;
    static QStandardItemModel* tblModel;
    static QSettings* userData;
    static RandDev* randDev;
    static User* instance;

    static const QString keys[ USER_KEY_COUNT ];

    public:
        enum UserColumns{ cSERNUM = 0, cPINGS, cCALLS, cSEENDATE, cIP,
                          cRANK, cBANNED, cBANDATE, cREASON, cCOLS = 9 };

        enum PlayerRanks{ rUSER = 0, rGAMEMASTER, rCOADMIN, rADMIN,
                          rOWNER = 4 };

        enum UserKeys{ kSEEN = 0, kBIO, kIP, kDV, kWV, kRANK, kHASH, kSALT,
                       kREASON, kBANNED, kPINGS, kCALLS = 11 };

        enum BanTypes{ tSERNUM = 0, tIP, tDV, tWV = 3 };

        explicit User(QWidget* parent = nullptr);
        ~User();

        static User* getInstance();
        static void setInstance(User* value);

        static QSettings* getUserData();
        static void setUserData(QSettings* value);

        static void setData(const QString& key, const QString& subKey,
                            const QVariant& value);
        static QVariant getData(const QString& key, const QString& subKey);

        static bool makeAdmin(QString& sernum, QString& pwd);

        static bool getIsAdmin(QString& sernum);
        static bool getHasPassword(QString sernum);
        static bool cmpAdminPwd(QString& sernum, QString& value);

        static qint32 getAdminRank(QString& sernum);
        static void setAdminRank(QString& sernum, qint32 rank);

        static void removeBan(QString& value, qint32 type);
        static bool addBan(Player* admin, Player* target, QString& reason,
                           bool remote = false);

        static bool getIsBanned(QString value, BanTypes type);

        static void updateCallCount(QString serNum);
        static void logBIO(QString& serNum, QHostAddress& ip, QString& dv,
                           QString& wv, QString& bio);

    private:
        QModelIndex findModelIndex(QString value, UserColumns col);
        void loadUserInfo();
        void updateRowData(qint32 row, qint32 col, QVariant data);

    private slots:
        void updateDataValue(const QModelIndex& index,
                             const QModelIndex&,
                             const QVector<int>& = QVector<int> ());
    private:
        Ui::User *ui;
};

#endif // USER_HPP