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



