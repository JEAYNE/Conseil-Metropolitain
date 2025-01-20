@echo off
setlocal
chcp 1252
rem Ce fichier est encodé avec la page de code Windows-1252 (la norme pour un fichier sous Windows en Europe de l'ouest)
rem On ne peut pas utiliser UTF-8 (chcp 65001) car le file-system de Windows n'est pas en utf8
rem MAIS Le contenu des fichier produit par l'Insee et la sortie du programme perl étant en UTF-8 c'est le bordel !!
cls

set Y=%1
if [%Y%] NEQ [] goto okYear
echo ERREUR: Usage %0 [year]
echo [year] est 2019 ou 2015 ou 2031
goto fin

:okYear
set DATA=données\%Y%
if exist %DATA%\  goto okData
echo ERREUR: Le répertoire '%DATA%' n'existe pas.
goto fin

:okData
set RESULTS=.\résultats\%Y%
if exist %RESULTS%\  goto okResults
echo ERREUR: Le répertoire '%RESULTS%' n'existe pas.
goto fin

:okResults
if "%Y%" == "2019" (

  rem Pourquoi un fichier sur l'intercommunalité de 2021 pour 2019 ? Voir note dans insee\données\2019\README.md
  rem # Pages :   https://www.insee.fr/fr/information/2510634
  rem # Fichier : https://www.insee.fr/fr/statistiques/fichier/2510634/Intercommunalite_Metropole_au_01-01-2021.zip
  rem # Feuilles: EPCI et Composition_communale
  set Metropoles=%DATA%\Intercommunalite-Metropole_au_01-01-2021~EPCI.csv
  set Communes=%DATA%\Intercommunalite-Metropole_au_01-01-2021~Composition_communale.csv

  rem # Page :    https://www.insee.fr/fr/statistiques/6011070?sommaire=6011075
  rem # Fichier : https://www.insee.fr/fr/statistiques/fichier/6011070/ensemble.xlsx
  rem # Feuille : Communes
  set Populations=%DATA%\ensemble_2019~Communes.csv

  goto run
)
if "%Y%" == "2025" (

  rem # Page :    https://www.insee.fr/fr/information/2510634
  rem # Fichier : https://www.insee.fr/fr/statistiques/fichier/2510634/epci_au_01-01-2024.zip
  rem # Feuilles: EPCI et Composition_communale
  set Metropoles=%DATA%\EPCI_au_01-01-2024~EPCI.csv
  set Communes=%DATA%\EPCI_au_01-01-2024~Composition_communale.csv

  rem # Page :    https://www.insee.fr/fr/statistiques/8290591?sommaire=8290669
  rem # Fichier : https://www.insee.fr/fr/statistiques/fichier/8290591/ensemble.xlsx
  rem # Feuille : Communes
  set Populations=%DATA%\ensemble_2025~Communes.csv

  goto run
)
if "%Y%" == "2031" (

  rem # Page :
  rem # Fichier :
  rem # Feuilles:
  set Metropoles=%DATA%\undef.csv
  set Communes=%DATA%\undef.csv
  
  rem # Page :
  rem # Fichier :
  rem # Feuille :
  set Populations=%DATA%\undef.csv
  goto run
)
echo ERREUR: Les noms des fichiers de données pour l'année '%Y%' ne sont pas configurés dans ce script.
echo goto fin

:run
perl Build-CommunePopulation-par-metropole.pl "%Metropoles%" "%Communes%" "%Populations%" "%RESULTS%"

:fin
echo -- FIN --
echo.
endlocal
pause
