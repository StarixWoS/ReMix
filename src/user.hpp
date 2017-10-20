#ifndef USER_HPP
#define USER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
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
        static void setUserData(const QSettings* value);

        static void setData(const QString& key, const QString& subKey,
                            const QVariant& value);
        static QVariant getData(const QString& key, const QString& subKey);

        static bool makeAdmin(const QString& sernum, const QString& pwd);

        static bool getIsAdmin(const QString& sernum);
        static bool getHasPassword(const QString& sernum);
        static bool cmpAdminPwd(const QString& sernum, const QString& value);

        static qint32 getAdminRank(const QString& sernum);
        static void setAdminRank(const QString& sernum, const qint32& rank);

        static void removeBan(const QString& value, const qint32& type);
        static bool addBan(const Player* admin, const Player* target,
                           const QString& reason, const bool remote = false);

        static bool getIsBanned(const QString& value, const BanTypes& type);

        static void updateCallCount(const QString& serNum);
        static void logBIO(const QString& serNum, const QHostAddress& ip,
                           const QString& dv, const QString& wv,
                           const QString& bio);

    private:
        QModelIndex findModelIndex(const QString& value,
                                   const UserColumns& col);
        void loadUserInfo();
        void updateRowData(const qint32& row, const qint32& col,
                           const QVariant& data);

    private slots:
        void updateDataValue(const QModelIndex& index,
                             const QModelIndex&,
                             const QVector<int>& = QVector<int> ());
    private:
        Ui::User* ui;
};

#endif // USER_HPP
