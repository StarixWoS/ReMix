
#ifndef QT_PROTOTYPES_HPP
    #define QT_PROTOTYPES_HPP

//Required Qt Includes. --Unable to prototype in most instances.
//    #include <QElapsedTimer>
//    #include <QMainWindow>
//    #include <QModelIndex>
//    #include <QTcpServer>
//    #include <QObject>
//    #include <QDialog>
//    #include <QTimer>

    class QSortFilterProxyModel;
    class QCryptographicHash;
    class QStandardItemModel;
    class QNetworkInterface;
    class QTableWidgetItem;
    class QSystemTrayIcon;
    class QStandardItem;
    class QElapsedTimer;  //Requires Include within Class Headers.
    class QStyleFactory;
    class QApplication;
    class QHostAddress;
    class QInputDialog;
    class QModelIndex;  //Requires Include within Class Headers.
    class QMainWindow;  //Requires Include within Class Headers.
    class QMessageBox;
    class QCloseEvent;
    class QStringList;
    class QTextCursor;
    class QGridLayout;
    class QByteArray;
    class QScrollBar;
    class QTcpServer;  //Requires Include within Class Headers.
    class QTcpSocket;
    class QUdpSocket;
    class QTableView;
    class QTabWidget;
    class QHostInfo;
    class QDateTime;
    class QFileInfo;
    class QSettings;
    class QTextEdit;
    class QToolTip;
    class QVariant;
    class QtEndian;
    class QProcess;
    class QLayout;
    class QDialog;  //Requires Include within Class Headers.
    class QString;
    class QObject;  //Requires Include within Class Headers.
    class QTimer;  //Requires Include within Class Headers.
    class QDebug;
    class QEvent;
    class QPoint;
    class QChar;
    class QFile;
    class QMenu;
    class QDir;

    #ifdef DECRYPT_PACKET_PLUGIN
        class QCoreApplication;
        class QPluginLoader;
    #endif // DECRYPT_PACKET_PLUGIN

#endif // QT_PROTOTYPES_HPP

#ifndef REMIX_PROTOTYPES_HPP
    #define REMIX_PROTOTYPES_HPP

    namespace Helper
    {}

    //Table-View Classes.
    class UserSortProxyModel;
    class PlrSortProxyModel;
    class TblEventFilter;

    //Widget GUI Classes.
    class SettingsWidget;
    class MessagesWidget;
    class PlrListWidget;
    class ReMixWidget;
    class RulesWidget;

    //GUI and Data Storage Classes.
    class PacketHandler;
    class ServerInfo;
    class CmdHandler;
    class Settings;
    class Comments;
    class RandDev;
    class SendMsg;
    class Player;
    class Server;
    class Rules;
    class ReMix;
    class User;

    #ifdef DECRYPT_PACKET_PLUGIN
        class PacketDecryptInterface;
    #endif // DECRYPT_PACKET_PLUGIN

    #ifndef REMIX_GLOBALS

        //Anything above PACKET_FLOOD_LIMIT (2000MS)
        //will disconnect/ban the User.
        static const int PACKET_FLOOD_TIME{ 2000 };

        //Users are able to send 100 packets within PACKET_FLOOD_TIME.
        static const int PACKET_FLOOD_LIMIT{ 100 };

        //Minimum hex-SERNUM allowed. This is used to validate a hex-SERNUM.
        //Via: sernum & MIN_HEX_SERNUM
        static const int MIN_HEX_SERNUM{ 0x40000000 };

        //Maximum time a User may remain inactive with "Disconnect Idle Users"
        //enabled. 10 minutes - 600,000 milliseconds.
        static const int MAX_IDLE_TIME{ 600000 };

        //Maximum amout of time in MS (30 seconds) to allow for the Master to
        //respond to a checkin.
        static const int MAX_MASTER_RESPONSE_TIME{ 30000 };

        //Amount of times an un-authenticated (admin) can use commands.
        static const int MAX_CMD_ATTEMPTS{ 3 };

        //Amount of Players that are able to connect to the server.
        static const int MAX_PLAYERS{ 256 };

        //Length of a Remote Admin's password salt.
        static const int SALT_LENGTH{ 12 };

        //Count of Settings Keys Accessed via the Settings/SettingsWidget class.
        static const int SETTINGS_KEY_COUNT{ 5 };

        //Count of Rules Keys accesed via the Rules/RulesWidget class.
        static const int SETTINGS_SUBKEY_COUNT{ 18 };

        //Count of Settings Sub-Kets Accessed via the Helper namespace.
        static const int RULES_SUBKEY_COUNT{ 16 };

        //Count of currently supported Remote-Admin commands.
        static const int ADMIN_COMMAND_COUNT{ 17 };

        //Count of Keys accessed via the User class.
        static const int USER_KEY_COUNT{ 10 };

        //Time interval for updating the server's Usage array.
        static const int SERVER_USAGE_UPDATE{ 600000 };

        //Size of the Server Usage array corresponding to 48 hours.
        static const int SERVER_USAGE_48_HOURS{ 288 };

    #endif  // REMIX_GLOBALS

#endif // REMIX_PROTOTYPES_HPP
