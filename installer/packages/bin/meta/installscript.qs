// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

// constructor
function Component(){
    // nothing to do, but this constructor is mandatory.
}


// https://doc.qt.io/qtinstallerframework/operations.html
Component.prototype.createOperations = function(){

    // First, call the base createOperations ...
    component.createOperations();

    // ... afterwards make our job
    console.log("Preparing shortcuts:");
    if (installer.value("os") == "win") {

        try {
            console.log(" - Preparing start menu entry to run program");
            component.addOperation("CreateShortcut",
                "@TargetDir@\\bin\\Conseil-Metropolitain.exe",
                "@StartMenuDir@\\Conseil-Metropolitain.lnk",
                "workingDirectory=@homeDir@"
            );
            console.log("   success");
        } catch(e){
            console.log("   failure");
        }

        try {
            console.log(" - Preparing start menu entry to uninstall program");
            component.addOperation("CreateShortcut",
                "@TargetDir@\\uninstall.exe",
                "@StartMenuDir@\\Desinstaller_Conseil-Metropolitain.lnk",
                "workingDirectory=@TargetDir@"
            );
            console.log("   success");
        } catch(e){
            console.log("   failure");
        }

        try {
            console.log(" - Preparing start menu shortcut to see data directory");
            var windir = installer.environmentVariable("WINDIR");
            var dataPath = installer.value("TargetDir") + "/donnees";
            component.addOperation("CreateShortcut",
                windir + "\\explorer.exe",
                "@StartMenuDir@\\Conseil-Metropolitain_DONNEES.lnk",
                dataPath.replace(/\//g, '\\')    // dans les parametres il faut faire la conversion / --> \ nous meme
            );
            console.log("   success");
        } catch(e){
            console.log("   failure");
        }

        try {
            console.log(" - Preparing Desktop shortcut to run program");
            var userProfile = installer.environmentVariable("USERPROFILE");
            installer.setValue("UserProfile", userProfile);
            component.addOperation("CreateShortcut",
                "@TargetDir@\\bin\\Conseil-Metropolitain.exe",
                "@UserProfile@\\Desktop\\Conseil-Metropolitain.lnk",
                "workingDirectory=@homeDir@"
            );
            console.log("   success");
        } catch(e){
            console.log("   failure");
        }

    }

}
