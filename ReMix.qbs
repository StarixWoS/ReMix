import qbs
import qbs.Environment

CppApplication
{
    consoleApplication: false
<<<<<<< HEAD
<<<<<<< .merge_file_a10732
    property string ReMixVersionDisplay: { if (Environment.getEnv("CI")) return Environment.getEnv("APPVEYOR_REPO_TAG_NAME"); else return '2.2.6'; }
    property string ReMixVersion: { if (Environment.getEnv("CI")) return Environment.getEnv("APPVEYOR_REPO_TAG_NAME"); else return '2.2.6'; }
=======
    property string ReMixVersionDisplay: { if (Environment.getEnv("CI")) return Environment.getEnv("APPVEYOR_REPO_TAG_NAME"); else return '2.3.6'; }
    property string ReMixVersion: { if (Environment.getEnv("CI")) return Environment.getEnv("APPVEYOR_REPO_TAG_NAME"); else return '2.3.6'; }
>>>>>>> .merge_file_a08060
=======
    property string ReMixVersionDisplay: { if (Environment.getEnv("CI")) return Environment.getEnv("APPVEYOR_REPO_TAG_NAME"); else return '2.3.7'; }
    property string ReMixVersion: { if (Environment.getEnv("CI")) return Environment.getEnv("APPVEYOR_REPO_TAG_NAME"); else return '2.3.7'; }
>>>>>>> develop

    Depends { name: "cpp" }
    Depends { name: "Qt";
              submodules: [ "core",
                            "gui",
                            "widgets",
                            "network",
                            "concurrent",
                          ]
            }

    //destinationDirectory: "libDir/"
    name: "ReMix-" + ReMixVersionDisplay + "-" + qbs.buildVariant

    cpp.includePaths: [ "resources", "ui", "ui/widgets",
                        "src", "src/tblview", "src/widgets",
                      ]

    cpp.defines: [ "RC_VERSION=" + ReMixVersion.replace(/\./g, ",") + ",0",
                   "RC_VERSION_STRING=" + ReMixVersionDisplay,
                   "REMIX_VERSION=" + "\"" + ReMixVersionDisplay + "\"",
                 ]
            .concat( base )

    cpp.cxxLanguageVersion: "c++14"
    cpp.debugInformation: qbs.buildVariant == "debug" ? true : false
    cpp.optimization: qbs.buildVariant == "debug" ? "none" : "fast"
    cpp.cxxFlags: ["-Os", "-s",]
    cpp.linkerFlags: [ "-s", ]

    destinationDirectory: "../" + qbs.buildVariant

    Group
    {
        name: "HPP"

        fileTags: ["hpp"]
        files: [ "src/tblview/usersortproxymodel.hpp",
                 "src/tblview/plrsortproxymodel.hpp",
                 "src/tblview/tbleventfilter.hpp",
                 "src/widgets/settingswidget.hpp",
                 "src/widgets/remixtabwidget.hpp",
                 "src/widgets/plrlistwidget.hpp",
                 "src/widgets/userdelegate.hpp",
                 "src/widgets/remixwidget.hpp",
                 "src/widgets/ruleswidget.hpp",
                 "src/widgets/motdwidget.hpp",
                 "src/createinstance.hpp",
                 "src/appeventfilter.hpp",
                 "src/packethandler.hpp",
                 "src/packetforge.hpp",
                 "src/selectworld.hpp",
                 "src/cmdhandler.hpp",
                 "src/serverinfo.hpp",
                 "src/prototypes.hpp",
                 "src/chatview.hpp",
                 "src/comments.hpp",
                 "src/settings.hpp",
                 "src/runguard.hpp",
                 "src/randdev.hpp",
                 "src/sendmsg.hpp",
                 "src/server.hpp",
                 "src/player.hpp",
                 "src/helper.hpp",
                 "src/theme.hpp",
                 "src/rules.hpp",
                 "src/remix.hpp",
                 "src/user.hpp",
                 "src/upnp.hpp",
        ]
    }

    Group
    {
        name: "CPP"

        fileTags: ["cpp"]
        files: [ "src/tblview/usersortproxymodel.cpp",
                 "src/tblview/plrsortproxymodel.cpp",
                 "src/tblview/tbleventfilter.cpp",
                 "src/widgets/settingswidget.cpp",
                 "src/widgets/remixtabwidget.cpp",
                 "src/widgets/plrlistwidget.cpp",
                 "src/widgets/userdelegate.cpp",
                 "src/widgets/remixwidget.cpp",
                 "src/widgets/ruleswidget.cpp",
                 "src/widgets/motdwidget.cpp",
                 "src/createinstance.cpp",
                 "src/appeventfilter.cpp",
                 "src/packethandler.cpp",
                 "src/packetforge.cpp",
                 "src/selectworld.cpp",
                 "src/cmdhandler.cpp",
                 "src/serverinfo.cpp",
                 "src/chatview.cpp",
                 "src/comments.cpp",
                 "src/settings.cpp",
                 "src/runguard.cpp",
                 "src/randdev.cpp",
                 "src/sendmsg.cpp",
                 "src/server.cpp",
                 "src/player.cpp",
                 "src/helper.cpp",
                 "src/theme.cpp",
                 "src/rules.cpp",
                 "src/remix.cpp",
                 "src/user.cpp",
                 "src/upnp.cpp",
                 "src/main.cpp",
        ]
    }

    Group
    {
        name: "UI"

        fileTags: ["ui"]
        files: [ "ui/widgets/settingswidget.ui",
                 "ui/widgets/plrlistwidget.ui",
                 "ui/widgets/remixwidget.ui",
                 "ui/widgets/ruleswidget.ui",
                 "ui/widgets/motdwidget.ui",
                 "ui/createinstance.ui",
                 "ui/selectworld.ui",
                 "ui/chatview.ui",
                 "ui/comments.ui",
                 "ui/settings.ui",
                 "ui/sendmsg.ui",
                 "ui/remix.ui",
                 "ui/user.ui",
                ]
    }

    Group
    {
        name: "RC"
        files: [ "resources/icons.qrc",
                 "resources/ReMix.rc",
        ]
    }
}
