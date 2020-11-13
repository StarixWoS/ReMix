TODO:
  * Change the IP re-selection to allow the User to select the active network interface and select the working IP address.
  * Implement other remote administrator commands and sub-commands.


Version 2.6.11:
    Change:
      * 



    Bugfixes:
      * Fixed an issue where closing a Server instance tagged with the rule "AutoRestart" would not appear within the Create Instance Dialog list.
      * Fixed an issue where closing a Server instance would prompt to remove the rule "AutoRestart" whether or not the rule was even enabled.




Version 2.6.10:
    Change:
      * Added code to convert the bytes sent to and from Users into a more user friendly format.
      * Removed code related to calculating the classic baud (bytes per second) in leiu of simply showing how many bytes have been sent/received in a modern/ user friendly format.
      * Changed the code used to filter the Log View UI to be less verbose, more efficient, and over-all easier to undertsand what is happening.
      * When manually closing a server instance; in addition to confirming the instance closure, we now also check if the AutoRestart rule should be disabled.



    Bugfixes:
      * Correctly calculate the sizes of packets send/received into the over-all server statistics.




Version 2.6.9:
    Change:
      * Implemented the TODO from version 2.6.6.
        * The Log View UI can now be filtered via a dropdown menu placed within the Logger Class UI.
      * Added a checkbox within the Logger Class UI to allow for automatic clearing of logs.
        * The interval is 24 hours, and cannot be changed or set to another value.
      * Added a Button within the Logger Class UI to allow for manual clearing of logs.
        * There is a confirmation dialog when manually clearing so as to not accidentally clear logs.



    Bugfixes:
      * Fixed a malformed versioning packet beign sent to the Master Mix.
        * The malformed packet functioned correctly, but it's better to be safe and fix it.




Version 2.6.8:
    Change:
      * Added the ability to override the default Master Mix IP Address.
        * This override is set via the Settings dialog under the direct Settings Tab.
        * The format for this override is a String value: <IpAddress>< : ><Port>.
        * Note: The Colon( : ) is a required delimiter and any address not containing it will be accepted but will not work.
        * Note: This Override is used across all Server Instances as such only one game *type* may be hosted at once.
        * Note: When loading a Master Mix Override from file the process of fetching the Normal Master Address is skipped, thus a full ReMix restart is required to resume normal operation.
      * Converted a few C-Style enums to the new C++ style enum class.



    Bugfixes:
      * Fixed the issue of having the Create Instance dialog remaining open when auto-restarting server instances.




Version 2.6.7:
    Change:
      * Added a new Rule within the Settings/Rules dialog "Auto Restart" to enable ReMix to automatically restart a server when re-opening ReMix.
        * Known issue: The Create Instance Dialog will also open, and if the host has no other servers to run, must be closed manually.
      * Consolidated code related to initializing a ReMix instance to make it easier to understand.



    Bugfixes:
      * 




Version 2.6.6:
    Change:
      * Added a new Log File "PingLog" which will now log pings from Users.
        * Previously these were written to the "UsageLog".
        * TODO: Implement a method to hide certain log types from the Log Viewer.
      * Reduced usages of "magic" numbers in relation to converting strings to integers.
        * An enum encompassing the various valid formats within Qt has been added.



    Bugfixes:
      * Fixed an issue where ReMix could show Users as having connected for longer than the a Server Instance has been online.
      * Fixed an issue where a ping response was listed within the Log Viewer before the actual ping itself.
      * Fixed an issue where Players could connect using negative sernums.
        * Players connecting with a negative sernum will be disconnected as though they were a BlueCoded SerNum, regardless if the Host has enabled the setting.




Version 2.6.5:
    Change:
      * Comments to a Server Instance will now be echoed to all admins on all initialized Server Instances.
        * The setting "Echo Comments to Admins" must be enabled.
      * ReMix no longer tracks a User's Ping count.
      * User-list request responses are now omitted from the Log View if the Server has no connected Users.
      * The command "/info" will now only count Users as Admins if the Admin has logged in with a password.
        * Previously all Admins were counted regardless of login state.



    Bugfixes:
      * Negative SerNums are now correctly handled.
      * The commands: "/kick", "/ban", and "/mute" can now be used with the "all" sub-command if no message has been provided. e.g. "/ban all"
      * Commands using the sub-command "all" will no-longer spam the command isser with failures due to tank or User statuses.
      * Pets of the same level as a Player can no longer be called within another Player's camp (scene).
      * Fixed a crash related to the "/camp allow" sub-command.
        * If a User were to omit the soul-targeting (/camp allow soul 4000) command the command would fallthrough and access an invalid nullptr.




Version 2.6.4:
    Change:
      * Convert variable initializers to be C++11 brace initializers to be more consitent with newer code.
      * Reduced the usage of auto casted variables when the type is known within source.
      * Removed a few unused source files from the repository.



    Bugfixes:
      * Finalized fixes related to the improper conversion of SerNums from hex to dec.




Version 2.6.3:
    Change:
      * ReMix now checks the 'K' and 's' packet for Pet and Player levels. We also parse the Player's AFK status, but it is not meaningfully used.
        * If calling a pet into another Player's scene then we check these two levels and confirm that the Pet can be called by the Player.
        * If the pet is a higher level than the Player then we drop the packet and inform the Player that the pet cannot be called.



    Bugfixes:
      * Fixed an issue where GS sernums were being read as white souls.
      * Properly clean up the CampExemption object when closing ReMix.




Version 2.6.2:
    Change:
      * Implemented the ability for Users with the Remote Administrator rank Admin or above to alter a Player's camp(scene) status. Whether locked or allowing only current Players.
        * These features can be used by appending "soul *PlayerSerNum" to the normal sub-command. e.g. "/camp lock soul 4000".
        * As ReMix does not supprt sub-sub commands, this command does not appear within the output generated via "/list".
      * Implemented the ability for Players to individually allow specific Users to bypass a camp(scene)'s locked status.
        * This is done via the sub-command "allow", and the appended sub-command "soul". e.g. "/camp allow soul 4000".
          * Players are added to an exemption list which is saved to disk.
          * The command "allow" currently acts as a toggle, and allowing a previously exempted Player will remove them from the exemption list.
        * As noted previously, the "soul" command does not appear within the output generated via "/list" for the "/camp" command.
        * An example of the "/list" output: camp[ lock, unlock, onlycurrent, allownew, allow,  ], 
        




Version 2.6.1:
    Bugfixes:
      * The method for preventing new Players from entering old camps(scenes) should now correctly function and should allow Players to enter camps(scenes) that are set to the "allow all"/default status.




Version 2.6.0:
    Change:
      * Implemented the chat command "/camp" with 4 sub-commands; "lock", "unlock", "onlycurrent", and "allownew".
        * Using "/camp" without any subcommands will issue the Player their current camp(scene) settings.
        * "lock" Prevents all other Players from entering any scene(camp) hosted by the command issuer.
        * "unlock" Returns the camp(scene) to default functionality. 
          * Unless "onlycurrent" is toggled on.
        * "onlycurrent" Makes it so only Players online when the command issuers scene(camp) was created can enter.
          * This is an opt-out function and is not the default.
        * "allowall" Allows Players to enter a camp(scene) hosted by the command issuer. This is regardless of connection or camp(scene) creation time.
          * Unless "lock" is toggled on.
      * The locked state issued via the command "/camp lock" is not saved to the Player's data on disk and will only persist while the Player remains connected.
        * The "onlycurrent" and "allowall" states are however saved to disk and will persist permanently until changed by the Player.



    Compatibility:
      * The server rule "maxAFK" has been renamed to "maxIdle" thus compatibility with the old Rule is broken, and Server Hosts will need to re-set this rule if it was in use.




Version 2.5.11:
    Bugfixes:
      * Fixed an issue related to disconnecting idle users.
      * Idle duration is now correctly applied to all Player objects if changed after server initialization.
        * When showing the default Idle Duration, the Rule will remain un-checked.
      * The Rules Widget now reflects the default Idle Duration when the Host has not changed it.
        * This duration is only enforced when the Host has also enabled the Setting "Disconnect Idle Users".
      * UI Elements related to the Max Idle duration have been renamed to more correctly show their intended purpose.
        * Previously within the Rules Widget it was referred to as a "Max AFK Duration".




Version 2.5.10:
    Change:
      * ReMix will now attempt to re-fetch the 'synreal.ini' file every 24 hours down from previously 48 hours.
      * ReMix no longer parses any packets for Users within a Muted state.
      * Users in a disconnected state will no longer be parsed as a valid User from the Players Vector.



    Bugfixes:
      * ReMix will no longer Echo comments from Users to ReMote Administrators when comments are being forwarded to all Remote Administrators.
        * This is to prevent the same message from being sent to the originating User\Remote Administrator twice.
      * Attempt to prevent the 'synreal.ini' file from being created in a 'null' or empty state.
        * ReMix will now also properly re-initialize itself with any newly fetched information contained within the 'synreal.ini' file.
      * Reduce UI 'lag' in relation to resizing UI columns.
      * Player Objects will now correctly be placed in a disconnected state when the TcpSocket signals that it is disconnecting.




Version 2.5.9:
    Change:
      * The Server Class now only handles receiving a socketdescriptor and forwarding it to the ReMixWidget Class.
      * The Settings class now controls the plrBioHash.
      * The Player Class now handles it's own readyRead signal/slot, and emits incoming packets to the PacketHandler Class.
      * The ServerInfo class should now interact with the UPNP class much better to allow forwarding ports before checking in with the MixMaster.



    Bugfixes:
      * Added a few QMutexLockers within the Settings Class to prevent possible threading issues from reading rules/settings.
      * A ReMix Server's GameInfo string will once again be obtained from the RulesWidget Class and appended to any Ping Response.
      * Fixed a crash related to creating a Server Instance.
        * The crash was caused by deleting a (global) RandDev Object from within the Settings Class.
      * Fixed an issue when closing a ReMix Instance.
        * Instances would close far too fast to properly inform the MasterMix that it had indeed closed.
        * This issue is most apparent when closing a large number of servers at once.
      * ReMix will now warn the host when they have reached the maxiumum server count.
        * In the case that the Server Count has been reached, no Server Instance will be created.
        * Previously the Server Instance was created, and active without being appended to the ReMixTabWidget UI.
          * If a User were to attempt to create another Server Instance with ReMix in this state, the MasterMix would ban the User.
      * Fixed a possible crash related to not properly deleting the UdpThread Objects stored within the ServerInfo Class.




Version 2.5.8:
    Change:
      * Settings and Rules are now handled via directly accessing the values using the getSetting/getRule functions directly in conjunction with Keys and SubKeys.
        * The goal of these changes is to reduce the verbosity of the Settings and Rules class files.
      * Convert the remaining uses of the older style of accessing Settings.
        * One exemption is related to getting the ServerID. This is remanining in use due to generating the ServerID in place.
      * Completely merged the Rules Class use cases into the Settings Class.
      * Comments from Users within the Comments Dialog Window will now be printed more like the Chat View.
        * The CommentLog no-longer double-prints the timestamp or new (blank) lines.
      * Add the ability to delete Remix Servers from the CreateInstance Dialog.
        * Server deletions will be confirmed to prevent accidental deletions.
      * Unify Master Server Messages to a signal to the Player class.
        * This reduces the risk of sending a Master Message to a nullptr, and the need to iterate over every Player Object.
      * ReMix will no longer Parse a User's BIO data for the DVariable, or the WVariable.
      * When changing a ReMix Server Instance's Name, the Instance will first disconnect from the MasterMix before reconnecting with the new name.
      * The Player Class now Inherits from the QTcpSocket class.
        * This allows the Player class to send/recieve packets on it's own without the need for a QTcpSocket variable or checking it's validity.



    Bugfixes:
      * Comments are once again only written to disk when the corresponding Setting is toggled on.
      * Opening a ReMix Instance will no longer overwrite settings if the name of a previous Open Instance is used.
      * When both "Allow Duplicate IP's" and "Ban Duplicate IP's" enabled, ReMix will only ban a User if "Allow Duplicate IP's" is disabled.
        * TODO: A more elegant solution is to toggle each setting off when the other is enabled.
      * The Settings "Allow Duplicate IP's" and "Ban Duplicate IP's" will now toggle eachother off as needed. The previous change is undone.
        * The Rules "AllPK" and "NoPK" will also toggle eachother off as needed.
        * The Rules "AllPK" and "ArenaPK" will also toggle eachother off as needed.
      * Server Passwords are once again correctly checked.
      * The Rules dialog will now once again properly handle checking and unchecking the "Server password" option.
      * Revoking a ReMote Administrator who is in a pending state will now properly revoke the rights to register with the server.


     
    Remove:
      * Some redundant class files.
      * Files related to the WorldShuffler Class.
      * Files related to the Rules Class.
      * Removed unused/commented code from several Class Files.




Version 2.5.7:
    Change:
      * ReMix now properly sends a response when receiving a ":SR?" formatted packet.
        * The information sent within this packet is stored and used to verify that the receiving User is operating correctly.
        * When ReMix senses that the stored value is different from what the User is sendint, we disconnect the User and send them a message.
      
	  

    Bugfixes:
      * Fixed an issue where the UPNP class would become disconnected from the Logger class.
        * This issue was preventing the UPNP from inserting logs to the Logger UI and disk.
      * Fixed a string formatting error in relation to the new Packet Header checks.




Version 2.5.6:
    Change:
      * Added a new Threaded Class Object. UdpThread.
        * This class handles some interactions with Users and the Master Mix via the UDP protocol.
        * While the PacketHandler class still parses some packet information, the UdpClass parses and handles any packets that require a response.
        * The ServerInfo class retains control over MasterCheckin timing, and sends a signal to the UdpClass to send the packet itself.
      * The Logger class now has multiple file handles for each individual log type.
        * These handles are opened as they are needed, and will remain open until ReMix closes.
          * These file handles are also closed and re-opened when the date used for the Log Folder changes.
        * These changes are done in an attempt to reduce file opening and closing overhead.
      * Convert all calls to the Logger::insertLog() into QObject signals to the Logger class.
        * These Signals are forwarded to the WriteThread for writing.
      * Added a new threaded Class Object WriteThread.
        * This new class handles all file write operations within the Logger Class.
      
	  

    Bugfixes:
      * Reduced latency overhead when pinging a ReMix server hosting multiple Worlds/Games.
        * The MasterUDP Socket used is now threaded, and should be thread-safe.
      * Remote Administrator commands will now inform the Admin of an issue if they attempt to use a command on theirself.
      * Sernum Information is now properly parsed from the MIX5 and MIX6 packet types.
        * This allows an Admin to log-in before being prompted if they so desire.





Version 2.5.5:
    Change:
      * Reduced latency when forwarding packets to players.
        * Packets will be sent to all eligible players at the same time when it is received.
        * Packets sent by a Player will now properly be ignored when ReMix forwards it to that Player Object.
        * The Player class will now check if a packet is eligible to be sent to the selected Player.
        * Parsing of the MIX packets is now handled within the same function as the SR packets.
      * Consolidated the isBanned, isMuted, removeBan, and removeMute into semi-universal functions. e.g. isPunished, and removePunishment.
        * Ban and Mutes will no longer check for certain sub-values. Namely; PunishmentTypes::WV, and PunishmentTypes::DV.
        * These ban types are holdovers from the standard synMix and the checking required to remove the ban isn't worth the effort to retain them any longer.
        * This means that only the User SerNum and IP addresses are checked for punishments.
      * The Server class now handles the bioHash as a static object.
        * Various static functions have been added to facilitate accessing this data from the other Class Objects.
      * Add two new schemas to the UPNP class from the WiFiAlliance.
        * "urn:schemas-wifialliance-org:service:WFAWLANConfig:1" and "urn:schemas-wifialliance-org:device:WFADevice:1"
	  

    Bugfixes:
      * Properly inform Users when the server has automatically muted their connection.
      * Properly format the message sent to the User when Muting and Un-Muting from the PlayerListWidget Object.
        * ReMix also now properly applies the correct Mute duration to the Player Object.





Version 2.5.4:
    Change:

    Bugfixes:
      * ReMix now detects when a User is within a Well Scene and no longer saves the Camp packet associated with that scene.
        * Entering camp scenes created before connecting to the server will now correctly function.
      * Mutes will now once again check if the IP address of the User has an outstanding Mute.
      * Attempt to prevent checking in to the Master Mix until the Master Mix IP address has been found.





Version 2.5.3:
    Change:
      * ReMix will now log, and show within the Logger UI when a Server Side Variable is being accessed with a read/write.
        * The newly generated logs will appear within the log directory and sub directory with the name [QuestLog].
      * Bans issued via Remote Administrator commands issued with no duration will now default to a 7 day ban.
        * Previously this was 30 days.
      * ReMix will now send socket information to newly connecting Users.
        * This information is used within WoS' skin transfer routines.


    Bugfixes:
      * Server Side Variables when being written will now forward the updated variable information to all connected Users.
        * Previously no update information was being sent. It has been verified that the intended behaviour was to forward the update to connected Users.
      * Corrected a state mismatch with the message being sent with the /vanish admin command.
        * When no sub-command was issued, the state would always show as being "invisible" within the message being sent to the Remote Administrator.
      * ReMix will no longer attempt to re-forward a port via UPNP that it has decided should be a permanent forward.
      * Fix build error.

    Source:
      * Begin converting the source from a 80 column text limit to 180 columns.





Version 2.5.2:
    Change:
      * Users who have been Muted will no longer have the ability to send comments via the Admin Window to the Server Host.


    Bugfixes:
      * Remote Administrator commands will no longer send a warning that the selected target is an Admin or Offline if neither state is true.
      * Mutes, whether automatic or remote-initiated via Admin Commands will now default to 10 minutes.
        * This duration was previously 30 Days.
      * Chat from the Chat view will no longer appear within the Logger UI.
      * When the "Ban Duplicate IP's" option is enabled, Users will now only be banned for a duration of 10 Minutes.
        * This duration was previously 30 Days.




Version 2.5.1:
    Change:
      * Implemented Mute durations.


    Bugfixes:
      * User Camp packets are no longer spammed when a new User connects to a server with Camped Users.
        * Camp packets are now only sent to the newly connecting User.




Version 2.5.0:
    Change:
      * Implement the "/info" command.
        * Sends basic information about the server's up time, users connected, and admins connected.
      * Implement packet deobfuscation code from the PacketForge module within ReMix itself.
      * Added the ability to utilize ReMote Administrator commands from within the regular chat box (bypassing /admin) on GameTypes supporting commands.
        * Commands entered in this way must begin with "  `  " (tilde). e.g. `ban soul 4000 30m Testing the new command!
        * All chat beginning with `(tilde) will be ommitted from further packet processing and/or re-sending.
      * The "/help" command now sends both command description and usage and no longer has a "format" sub command.


    Bugfixes:
      * Prevent Clients from forging a MasterMix response packet.
      * Prevent setting the GameInfo string within a ServerInstance's name. e.g. "ReMix Server [world=Evergreen]" is no longer possible.
      * Prevent comments from Users that have not yet sent their SerNum.
        * This is related to another bugfix from version [2.4.7].
      * Mark certain functions and constructors as overrides or defaults.
      * Properly cast certain UI events to their correct type.
      * Correctly inform Remote Administrators when they attempt to use a command they lack access to.




Version 2.4.9:
    Buildsystem:
      * Convert the ReMix project from Qbs to CMake.




Version 2.4.8:
    Change:
      * ReMix will now store and re-send packets with the OpCode( "F", Camp Initiated ).
        * This only affects the Well of Souls[WoS].


    Bugfixes:
      * ReMix will now attempt to correct a User's BIO data when they have pinged with a SerNum that does not match their current SerNum.
        * This is an attempt to correct an oversight left by a BugFix in version [2.4.7].




Version 2.4.7:
    Change:
      * Added the ability for ReMote Administrators to set a punishment duration when banning a User.
        * Example usage: /ban soul 4000 hours 24 *Reason


    Bugfixes:
      * Bans will now properly be removed when a banned User pings the server.
        * ReMote Administrators may now enact punishments on other Remote Administrators of a lesser rank.
      * Remove a SerNum check regarding UDP information due to ping/SerNum change collisions within the same household.




Version 2.4.6:
    Change:
      * Player Chat is now logged to file.
      * Player Sernums will now be colored Gold when using a Golden Soul.
        * Remote Administrators will remain as Green while authenticated and Muted Users will remain as Red.
      * Added Punishment Durations. Ranging from one day to one year, and permanent.
        * The durations can currently only be selected when manually Banning a User.
        * Remote Administrator bans are currently limited to 30 days from the time of banning.
        * Mutes are currently unaffected and are therefore permanent until reconnecting to the server.
      * Bans originating from ReMix versions prior to 2.4.6 will automatically be removed.
        * The Ban information is logged to file so that the Server Host may manually re-ban the User if so desired.
      * Remote Administrators must now authenticate with the server regardless of any previously set setting.
        * The Setting is no longer available, and is thus ignored.


    Bugfixes:
      * ReMix Server Instances will now only ping the MasterMix every 30 seconds after the initial check-in.
        * This is changed to every 5 minutes once a MasterMix response is received.
      * ReMix Server Instances will now correctly check-in with the "Warpath[W97]" MasterMix every 5 minutes.
      * ReMix will now correctly shift to a valid Server Instance when closing another.
        * Previously the Server Instance would retain UI elements pertaining to the closed Server Instance.
      * Users attempting to use commands outside of their rank status will now properly be warned and once again be given a command attempt countdown.
      * Unauthenticated Remote Administrators can now properly use all commands available to the rank "GmRanks::User".
        * Previously a Remote Administrator would be unable to use the command "/login".
      * Revoking and Reinstating a Remote Administrator from the Player List is now correctly handled.
        * Previously a Remote Administrator that had been demoted would be requested to re-register a password.
        * Previously a demoted Remote Administrator could attempt to login with their previous password. (This would not succeeed.)
      * Banning a User from the Player List will now properly request a Punishment Duration.




Version 2.4.5:
    Change:
      * Moved Server specific information from the main ReMix UI to a ToolTip that is shown when hovering over a Server tab instance.
      * Added various log messages to further inform the Server Host as to what is happening in the background.
      * The ChatView will now attempt to use the ingame name of a Player as opposed to just their sernum or alias.


    Bugfixes:
      * The Server Host will now be informed when the PacketForge Module is or is not able to be loaded.
      * Packets sent to a Server Instance using the game type "Arcadia" will now properly decrypt packets when the PacketForge module is loaded.
      * ReMix will now properly select the correct MasterMix server to correspond to the GameType selected via the CreateInstance dialog.
      * ReMix will no longer attempt to continuously check-in with the "Warpath[W97]" MasterMix server.
        * The "Warpath[W97]" MasterMix does not support responding to the Server Check-In packets.
          * This is due to the MasterMix version being much older and not really compatible.




Version 2.4.4:
    Change:
      * The threshold for disconnecting a player has been increased to 1024 packets within 1,000MS ( 1 Second ).
        * The previous threshold was 512 packets within 2,000MS ( 2 Seconds ).
      * Players will no longer be automatically banned for packet flooding. They will however be disconnected.
        * This is due to WoS scenes, and the ability to unintentionally fill a scene with 148 entities.
      * The Option within the Settings UI related to "Banning Deviants" has been removed, and will no longer have an effect within the Settings class.
        * This is related to the Packet Flooding change.


    Bugfixes:
      * Now correctly check if an IP address is Banned when the User's BIO Information is obtained.
        * Previously a User with Banned Information could connect to a ReMix server with only their SerNum changed.
      * When comparing and searching connected Players for Banned Information we will now skip the currently connecting SerNum for all ban times excluding SerNum bans.
        * Previously some Banned Information was not checked.




Version 2.4.3:

    Bugfixes:
      * Fixed a crash when recieving comments from players.
      * Duplicate SerNums will now correctly be disconnected with prejudice.




Version 2.4.2:
    Change:
      * Added a port randomizing button to the server creation dialog in an effort to be more intuitive.
      * Added more logging information from the UPNP class to the Logger UI.

    Bugfixes:
      * Fixed an oversight related to UPNP where the Modem or Router would only support certain lease times.




Version 2.4.1:
    Change:
      * Add a new Dialog to enable in-application viewing of Log events.
        * Note: Not all events of note are yet logged to this Dialog. Only events logged to file are currently shown.
      * Add the early base layout for a future WorldShuffler feature.
        * This feature will eventually allow the Host to host a server that will change the selected Game/ToY/World without intervention from the Host.

    Bugfixes:
      * Converted many variable types to their proper (assumed) types.
        * Note: Many of these variables were being incorrectly used and otherwise being truncated to a different type alltogether,
      * Converted many uses of '0' to use the proper value of 'nullptr'.




Version 2.4.0:
    Change:
      * Add more profanity words to the chat filter.

    Bugfixes:
      * Changed the packet flooding threshold to 521 packets within 2,000 milliseconds (2 seconds) up from 100.




Version 2.3.9:
    Change:
      * Added a third sub-command to the command "/vanish".
        * The added sub-command is "status" and the usage is "/vanish status" and will output the current visibility of the Admin for other users.
      * The "/shutdown" and "/restart" commands now have an aditional sub-command to stop an in-progress shutdown or restart.
        * The new sub-command is "stop" with the usage "/shutdown stop" or "/restart stop".
      * Add icons next to a User's SerNum to show if they have spoken within the last 5 minutes.
        * There are two states. One shows the User 'talking' and the other shows the User as being AFK.

    Buildsystem:
      * Update the Appveyor project file.

    Bugfixes:
      * The "/vanish" command now correctly functions as an on/off toggle when no sub-commands are used in conjunction with it.
        * Previously the command would show that the Admin was invisible while it was not correct.




Version 2.3.8:
    Change:
      * Create and implemented a Command Table class (CmdTable).
      * Restructured the Remote Administrator commands, and added functionality for several "Helper" commands.
        * Commands added include: Help, List, MotD.
          * Help shows the description or syntax of a command.
          * List shows all commands available to the user.
          * MotD allows the User to change the servers message of the day.
        * Various sub-commands have also been added but are not yet completely implemented.
          * Commands with new subcommands include Shutdown, Restart, and Vanish.
      * The server logs are now stored in the path "logs/logType/[yy-mm-dd]/logType.txt".
        * This allows for the User to more quickly browse to a certain log type for a specific day.
          * This also makes it more obvious to the User if the log exists for a certain day.
	  
    Bugfixes:
      * The World Selection UDP command will now check that the User setting the World information at least has a current connection to the server.
      * The Random Device class (RandDev) once again properly generates a sufficiently random number and no longer re-seeds upon every use.




Version 2.3.7:
    Change:
      * When creating a server the Create Instance dialog how has a checkbox to enable the ability to toggle port randomization.
        * Randomization only occurs when the User inputs '0' or makes the input field blank.
      * Add a new Setting toggle to disallow Users using a BlueCoded SerNum.
        * The Setting is not enabled by default and unless enabled the following SerNums will remain valid and able to connect.
          * 1004, 1024, 1043, 1046, 1052, 1054, 1055, 1062, 1068, 1072, 1099, 1112, 1120, 1123, 1125, 1138, 1163, 1166, 1170, 1172, 1173, 1189,
          * 1204, 1210, 1217, 1275, 1292, 1307, 1308, 1312, 1332, 1338, 1367, 1369, 1370, 1520, 1547, 1551, 1565, 1600, 1607, 1611, 1656, 1675,
          * 1681, 1695, 1706, 1715, 1751, 1754, 1840, 1965, 2003, 2058, 2062, 2144, 2205, 2217, 2264, 2268, 2359, 1008, 1017, 1051, 1054, 1082,
          * 1099, 1104, 1105, 1181, 1199, 1222, 1279, 1343, 1358, 1388, 1456, 1528, 1677, 1773, 1777, 1778, 1780, 1796, 1799, 2156, 2167, 2241,
          * 2248, 2362, 2421, 1098, 1220, 1264, 1342, 1361, 1823, 2302, 2488, 2585, 2372, 1492, 1576, 1100, 1347, 1050, 1015, 1666, 1745, 2043,
          * 1200, 2628, 1016, 1739, 1853, 2708, 2757, 1498, 2448, 2801, 1031, 1265, 1414, 1420, 1429, 1214, 1489, 1707, 2543, 1101, 1283, 1604,
          * 1428, 2707, 1023, 1069, 1071, 1132, 1286, 1854, 2910, 1005, 2682, 1348, 2615, 2617, 1884, 1169, 1540, 1645, 1939, 1179, 3053, 1803,
          * 2377, 1000, 1021, 1500, 1501, 1515, 1547, 1803, 2377, 3111, 3202, 3191, 3149, 3,

    Bugfixes:
      * When creating a server the Create Instance dialog will no longer automatically generate ports when the User removes all text from the input field.
      * When blocking an IP address from use ReMix will now properly refresh the network configuration and select another IP address.
      * The Remote Administrator commands "Kick", "Ban", and "Disconnect" will no longer affect the Command Issuer or other Remote Administrators.




Version 2.3.6:
    Change:
      * Disabled update checking.
        * The feature didn't appear to work for most Users.
        * Maybe one day someone will add it in, but I doubt it will be useful.
      * Add a basic chat filter for the ReMix chat viewer.
        * The feature relies on the server host having the rule "No Cursing" enabled.
      * The RandDev class is now a static class and the device member can be accessed globally.

    Bugfixes:
      * Removed two emit and slot functions related to Remote Administrator authentication.
        * This improves the speed at which the User receives the notification.
      * Change how the User's sernum is shown to the server host in various dialogs.
        * This primarily improves consitency between the various dialogs.




Version 2.3.5:
  Change:
    * Add a set of colors to the Theme class that are utilized in certain UI and chat elements.
    * Add global variables to indicate how many Themes ReMix can use, and how many custom UI colors the Theme class can use.
    * Add an enumerator class for use with the new theme color array used within the Theme class.
    * Add an implementation file for the Theme class.
    * Convert a few basic enumerators into a strongly typed enumerator class.
    * Enable the remote administrator command '/vanish' for remote administrators to appear as 'disappeared' or to sit on a server unnoticed.
      * The command also prevents Users from seeing Vanished administrators on the User-list.
    * The Settings class nolonger has control of the RulesWidget or the MOTDWidget classes.
      * Each class now stored the objects currently in use, and can be fetched or deleted through static functions.
      * These changes were made for the possibility of implementing the ability for remote administrators to change a server's rule set or message of the day.
    * The WorldSelection dialog, when used, will now allow the User to not select a World.
      * If the User previously had a world selected, a confirmation dialog will appear to confirm the removal.
      * If no world was previously selected by the User, the 'Ok' button will function as though the User had clicked 'Cancel'.
    * Add an enumerator class for the PlrListWidget class' table columns.
    * Convert the User enumerator to a enumerator class, and renameed it to 'UserCols'.
    * Add an enumerator for use in converting a numeric time to a user readable format. 'Hours:Minutes:Seconds'.
    * Add two Helper function to convert seconds into a time format using the TimeFormat enumerator.
      * Used for the Server UpTime, and the User connection times.

  Bugfixes:
    * When logging SerNum mismatches to file, we should be logging the encrypted packet instead of the unencrypted packet.
      * This is to ensure that any packet using the ':SR' format aren't being encrypted, and that we will know the packet at issue in it's original state.
    * The Settings and User Information dialogs will now close once the last server has been exited.
      * The previous behavior left these two dialogs open for view and interaction while the Create Server dialog was shown.
    * Prevent the string "world=" within the GameInfo string sent to Users when the Server information is requested.
      * This usually doesn't appear outside of "ReBreather" chat clients.
      * It's better to patch it in ReMix than bothering with the "ReBreather" sorce, though.
    * Properly remove the World rule when the option is unselected.
    * When renaming a Server instance, the process will no longer toggle all rules in a disabled state.
      * This was unintended and otherwise caused clutter that left the important rules 'hidden' to Users.

  Note:
    * These changes to the Theme Class are intended to preceed the implementation of a Theme-Color selector.




Version 2.3.4:
  Change:
    * Consolidate information regarding log file names into the Helper class for ease of use.
    * All logs will now be seperated into subdirectories based on the current date to more easily browse to events on a certain day.
    * All disconnects are now logged to file if the option to log files is enabled.
    * Move various enumerators related to Remote Administrator commands into the prototypes header.

  Bugfixes:
    * Properly validate settings before logging certain information to file.
      * Before, the setting "Settings::getLogFiles()" was not being checked when certain events occured.
    * Setting a SerNum no longer calls the validation function.
      * The call was redundant, as validation occurs before setting in any case.
    * All disconnects will now properly increment the UI indicator.
    * The function to log files now checks if the file should be logged.




Version 2.3.3:
  Change:
    * Re-enable the remote administrator commands to shutdown and restart a server.
      * When the last server is shutdown via a remote command, the ReMix instance will also close.
      * When the last server is restarted via a remote admin command, the Create Instance dialog will be skipped, and the server will be recreated using the previous settings.

  Bugfixes:
    * Servers when closed will now correctly have their indices shifted to retain the proper name and position within the server map.
      * The previous behavior would cause a server to take on the name and settings dialog of another active server.
    * Correctly convert the server usage information to strings.
      * The previous behavior of using the raw data would eventually cause a segfault.




Version 2.3.2:
  Change:
    * Permanently show the designated GameInfo( World Name ) as selected within the RulesWidget class.
      * Instead of disabling the shown GameInfo( World Name ) when all users are disconnected, we now show it on the server list at all times.

  Bugfixes:
    * Correctly format and present the GameInfo( World Name ) on server lists.
    * When the GameInfo( World Name ) is changed within the RulesWidget class, emit a signal to update the information within the ServerInfo class.
    * Correctly convert integers to strings within the UPNP class for logging to file.
    * When selecting the option to toggle the update checker the stored state is now checked to prevent an update check when the option is already toggled.




Version 2.3.1:
  Bugfixes:
    * Properly remove Users from the online User list when they are banned, kicked, or otherwise disconnected.




Version 2.3.0:
  Bugfixes:
    * Fixed a crash related to accessing a nonexistint row within the SettingsWidget table.




Version 2.2.9:
  Change:
    * Remove references to a User's port number from Log files.
    * Add setting toggle to enable checking for ReMix updates.
    * Add a basic ChangeLog file to the repository.

Import: 
  Bugfixes:
    * When changing a server's name, the input dialog will now properly have the old server's name pre-entered.
      * If accepting a name change with the old name, a warning message will be shown to the User.
	* ReMix will nolonger automatically attempt to update when the setting is not toggled.

  Buildsystem:
    * Merge the QtSparkle Update Checking library into the repository.




Version 2.2.8:
  Bugfixes:
    * Sanitize 'QString.arg()' format strings to (hopefully) prevent any formatting bugs from messages containing '%' format operands.
    * A ban date is now again properly added to the User dialog when a user is banned.




Version 2.2.7:
  Bugfixes:
    * ReMix will no longer attempt to disable and remove UPNP Port Forwards if it was never enabled for the server being closed.
    * ReMix will now ignore any GameInfo Strings being sent by Users if one has already been set via another User or from the Rules dialog.
    
  Buildsystem:
    * Add PacketForge.dll to AppVeyor deploy.




Version 2.2.6:
  Feature:
    * Add toggles to enable or disable UPNP instead of being enabled by default.

  Change:
    * A Server Instances UPNP status will now be saved to settings.




Version 2.2.5:
  Change:
    * Rename the ThemeChange class to Theme.
    * The Theme class now handles changing the font style and will now return the current theme value as required.
    * Add the RunGuard and Theme classes into the prototypes header.
    * Inform the Server Owner of the inability to fetch a server's Comments dialog.

  Bugfixes:
    * Change the value for an invalid Game within the Games enum. Games::Invalid now has a value of '-1'.
    * The PacketForge class will now again ignore both "SR?" and "SR!" packets and will no longer attempt to decrypt them.

  Buildsystem:
    * Debugging information will now be stripped from release mode.
    * Revert the project file to a state that does not build ReMix in a Debug mode.




Version 2.2.4:
  Change:
    * Add new Helper function "getTimeAsString" to assist with converting the current date to a string.
    * Mark many class member functions to be const and prevent unwanted editing of member variables.

  Bugfixes:
    * Helper::logToFile() will now again correctly append the timestamp and newlines.
    * Information is again correctly displayed in the User Dialog.



	
Version 2.2.3:
  Change:
    * Refactor many Class member functions to recieve const references for incoming variables.

  Bugfixes:
    * Attempt to properly set a GameID and send the information to the MasterMix.

  Buildsystem:
    * Remove Tab-Spacing from the embedded AppVeyor project file.




Version 2.2.2:
  Change:
    * Restructure how file includes are handled.

  ReadMe:
    * Add a simple ReadMe to the project.

  Buildsystem:
    * Automatically set the Version number based on the Commit-Tag in artifacts.
    * Embed the AppVeyor build status into the ReadMe file.
    * Embed a copy of the AppVeyor project file into the repository.




Version 2.2.1:
  Buildsystem:
    * Begin using the AppVeyor build system.




Version 2.1.16:
  Changes:
    * Reduce verbosity in string comparisons.



