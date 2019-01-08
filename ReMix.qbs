import qbs
import qbs.Environment

CppApplication
{
    consoleApplication: false
    property string ReMixVersionDisplay: Environment.getEnv("APPVEYOR_REPO_TAG_NAME") || "2.4.1";
    property string ReMixVersion: Environment.getEnv("APPVEYOR_REPO_TAG_NAME") || "2.4.1";

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
        files: [
            "src/views/loggersortproxymodel.hpp",
            "src/widgets/commentviewwidget.hpp",
            "src/views/usersortproxymodel.hpp",
            "src/views/plrsortproxymodel.hpp",
            "src/widgets/settingswidget.hpp",
            "src/widgets/chatviewwidget.hpp",
            "src/widgets/remixtabwidget.hpp",
            "src/widgets/plrlistwidget.hpp",
            "src/views/tbleventfilter.hpp",
            "src/widgets/userdelegate.hpp",
            "src/widgets/remixwidget.hpp",
            "src/widgets/ruleswidget.hpp",
            "src/widgets/motdwidget.hpp",
            "src/createinstance.hpp",
            "src/appeventfilter.hpp",
            "src/packethandler.hpp",
            "src/worldshuffler.hpp",
            "src/packetforge.hpp",
            "src/selectworld.hpp",
            "src/cmdhandler.hpp",
            "src/serverinfo.hpp",
            "src/prototypes.hpp",
            "src/cmdtable.hpp",
            "src/chatview.hpp",
            "src/comments.hpp",
            "src/settings.hpp",
            "src/runguard.hpp",
            "src/randdev.hpp",
            "src/sendmsg.hpp",
            "src/server.hpp",
            "src/player.hpp",
            "src/helper.hpp",
            "src/logger.hpp",
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
        files: [
            "src/views/loggersortproxymodel.cpp",
            "src/widgets/commentviewwidget.cpp",
            "src/views/usersortproxymodel.cpp",
            "src/views/plrsortproxymodel.cpp",
            "src/widgets/settingswidget.cpp",
            "src/widgets/chatviewwidget.cpp",
            "src/widgets/remixtabwidget.cpp",
            "src/widgets/plrlistwidget.cpp",
            "src/views/tbleventfilter.cpp",
            "src/widgets/userdelegate.cpp",
            "src/widgets/remixwidget.cpp",
            "src/widgets/ruleswidget.cpp",
            "src/widgets/motdwidget.cpp",
            "src/createinstance.cpp",
            "src/appeventfilter.cpp",
            "src/packethandler.cpp",
            "src/worldshuffler.cpp",
            "src/packetforge.cpp",
            "src/selectworld.cpp",
            "src/cmdhandler.cpp",
            "src/serverinfo.cpp",
            "src/cmdtable.cpp",
            "src/chatview.cpp",
            "src/comments.cpp",
            "src/settings.cpp",
            "src/runguard.cpp",
            "src/randdev.cpp",
            "src/sendmsg.cpp",
            "src/server.cpp",
            "src/player.cpp",
            "src/helper.cpp",
            "src/logger.cpp",
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
        files: [ "ui/widgets/commentviewwidget.ui",
                 "ui/widgets/settingswidget.ui",
                 "ui/widgets/chatviewwidget.ui",
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
                 "ui/logger.ui",
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
