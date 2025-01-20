# Insee/

Répertoire utilisé (en interne ;-) pour télécharger des fichiers publiés par l'Insee
et en extraire les informations nécessaires au programme ```Conseil-Metropolitain.exe```.

Les données pour l'année yyyy sont téléchargées puis mise en forme et stockées
dans ```insee/données/yyyy/```. Ensuite, le résultat de l'exécution du batch 
```inseee/RunBuild.bat yyyy``` sera stocké dans ```insee/résultats/yyyy/```.
Après vérifications ces données sont copiés dans le répertoire ```data/``` du 
programme final.

Consultez les README de ces sous répertoires pour plus de détails.
