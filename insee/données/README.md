# insee/données/yyyy

Répertoires contenant des informations publiées par l'Insee sur les métropoles et 
les communes qui les composent ainsi que sur les populations de ces communes.
Classement par année yyyy d'entrée en vigueur des données sur les populations.

> En principe, pour des raison légales, yyyy est l'année avant les élections 
municipales (... 2019, 2025, 2031 ...)

La procédure d'extraction et de mise en forme des données nécessaires au programme
```Conseil-Metropolitain.exe``` est la suivante (A répéter pour chaque année yyyy) :

## Etape manuelle

Cette première étape consiste à :
* télécharger 2 fichiers Excel depuis le site de l'Insee,
* en extraire 3 feuilles au format csv (encodage UTF8),
* préserver l'information texte (pour humains) en tête des 3 fichiers csv en ajoutant un '#' en début de ligne.

> Le README de chaque répertoire ```insee/données/yyyy/``` contient les liens 
vers les fichiers de l'Insee pour l'année yyyy ainsi que le nom des feuilles 
à extraire (avec un encodage UTF8).

### Exemple

Cette extraction d'une feuille au format CSV, encodage UTF-8
```
Intercommunalité et Métropole
France - Liste des intercommunalités
Mise en ligne le 18/03/2024       Géographie au 01/01/2024
Source(s) : Insee, Code officiel géographique
EPCI;LIBEPCI;NATURE_EPCI;NB_COM
200000172;Communauté de communes Faucigny-Glières;CC;7
200000438;Communauté de communes du Pays de Pontchâteau Saint-Gildas-des-Bois;CC;9
```
est éditée manuellement et devient :
```
# Intercommunalité et Métropole
# France - Liste des intercommunalités
# Mise en ligne le 18/03/2024       Géographie au 01/01/2024
# Source(s) : Insee, Code officiel géographique
EPCI;LIBEPCI;NATURE_EPCI;NB_COM
200000172;Communauté de communes Faucigny-Glières;CC;7
200000438;Communauté de communes du Pays de Pontchâteau Saint-Gildas-des-Bois;CC;9
```	
Notez que la ligne juste avant les données (celle qui définit le nom des champs) n'est pas modifiée.

On peut en profiter pour ajouter d'autres lignes commençant par # pour indiquer 
les liens vers les données d'origines sur le site de l'Insee.

## Etape automatique

A partir des 3 fichiers csv créés manuellement pour l'année yyyy, le script Perl
```Build-CommunePopulation-par-metropole.pl``` construit pour chaque métropole un
fichier csv contenant le nom des communes de la métropole et leurs populations municipales.

### Exemple:
```
Deûlémont;1801
Hœnheim;11413
L'Union;12410
Toulouse;511684
```
Le batch ```RunBuild.bat yyyy``` simplifie l'éxecution du script Perl en passant les bon noms des 
fichiers à utiliser pour l'année yyyy et de désigner le répertoire de sortie approprié
(en principe insee/Résultats/yyyy/)
