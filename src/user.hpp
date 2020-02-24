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
    static User* instance;

    static const QStringList keys;
    static const QVector<PunishDurations> punishDurations;

    public:
        enum UserKeys{ kSEEN = 0, kBIO, kIP, kDV, kWV, kRANK, kHASH, kSALT,
                       kMUTED, kMUTEDURATION, kMUTEREASON,
                       kBANNED, kBANDURATION, kBANREASON, kPINGS, kCALLS = 15 };

        explicit User(QWidget* parent = nullptr);
        ~User() override;

        static User* getInstance();
        static void setInstance(User* value);

        static QString requestReason(QWidget* parent = nullptr);
        static PunishDurations requestDuration(QWidget* parent = nullptr);

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
        static void setAdminRank(const QString& sernum, const GMRanks& rank);

        static void removeBan(const QString& value, const qint32& type);
        static bool addBan(const Player* admin, const Player* target,
                           const QString& reason, const bool remote,
                           const PunishDurations duration);

        static bool getIsBanned(const QString& value, const BanTypes& type,
                                const QString& plrSernum = "");

        static void removeMute(const QString& value, const qint32& type);
        static bool addMute(const Player* admin, const Player* target,
                            const QString& reason, const bool remote,
                            const PunishDurations duration);
        static bool getIsMuted(const QString& value, const BanTypes& type,
                               const QString& plrSernum = "");

        static void updateCallCount(const QString& serNum);
        static void logBIO(const QString& serNum, const QHostAddress& ip,
                           const QString& dv, const QString& wv,
                           const QString& bio);

        static QByteArray getBIOData(const QString& sernum);

    private:
        QModelIndex findModelIndex(const QString& value,
                                   const UserCols& col);
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
