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



