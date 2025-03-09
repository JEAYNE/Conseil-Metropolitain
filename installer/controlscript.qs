// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

function Controller(){
    var product =  installer.value("ProductName");
    console.log("ProductName and UUID: " + product);
    installer.setValue("ProductUUID", product);     // important: toujours le meme UUID == @ProductName@
}

Controller.prototype.TargetDirectoryPageCallback = function(){

    // Y a t il une version (avec c'est UUID) déjà installée ?
    var product =  installer.value("ProductName");
    var base = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + product;
    var previous = installer.value(base + "\\InstallLocation");
    if( previous == "" )
        return;

    // https://doc.qt.io/qtinstallerframework/qinstaller-messageboxhandler.html#details
    console.log("Detect this previous version: " + previous);
    var result = QMessageBox.question( "quit.question", "Installation de " + product,
    "Une version de " + product + " est déjà installée dans<br/>"
    + previous + "<br/>"
    + "Cliquez sur :<br/>"
    + " &nbsp; <b>Oui</b> pour la désinstaller immédiatement<br/>"
    + " &nbsp; <b>Non</b> pour l'écraser ou abandonner à la prochaine étape",
    QMessageBox.Yes | QMessageBox.No);

    if(result == QMessageBox.Yes){
        // desinstaller
        console.log("Uninstall this previous version: " + previous);
        installer.execute("@TargetDir@/uninstall.exe", ["purge", "-c"]);
    }else{
        // ecraser la version en place
        console.log("Overwrite this previous version: " + previous);
        installer.setValue("TargetDir", previous);                           // On reutilise le TargetDir de la version en place
        installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);  // On ne demande pas a l'utilisateur
    }

}