# Changelog

Historique des changements notables effectués dans le code de ce projet
depuis sa création en décembre 2024.
<br/>
## [2.0.0] - 2025-03-01
- Code et binaires rendus publics sur GitHub.
<br/>
## [1.9.0] - 2025-02-12
- upgrade de Qt 6.8.1 à Qt 6.8.2
- Rédaction du README.md principal avec copies d'écran
<br/>
## [1.8.0] - 2025-02-02
- Automatisation de l'extraction des listes de communes et de leurs populations municipales
  depuis les fichiers publiés par l'Insee (Cas de Dijon un peut compliqué)
- Réorganisation des données par années
- Données installées (à nouveau) par défaut
- Selection de la métropole depuis une liste (chargement d'un fichier csv toujours possible)
<br/>
## [1.7.0] - 2025-01-31
- Verification du nombre de conseillers municipaux vs métropolitains
- Deux options de calcul pour les sièges optionnels
- Rend l'installation des données (fichiers csv) facultative
  Leur installation est déactivée par défaut car toutes les données ne sont pas valides en 2025
<br/>
## [1.6.0] - 2025-01-25
- Dans l'onglet 'Options' ajout d'un test de conformité avec le
  Conseil Constitutionnel (2015-711 DC).
- Détection des cas spéciaux de sur-représentation (>20%) 
- Important refactoring des formats pour simplifier la gestion des tables.
<br/>
## [1.5.0] - 2025-01-22
- Redesign de certains tableaux.
- Ajout de plus d'informations chiffrées.
- Ajout de l'onglet 'Résumé'.
<br/>
## [1.4.0] - 2025-01-20
- Ajout de l'option d'impression pour chaque onglet.
- Ajout de la possibilité de sauver chaque onglet dans un fichier PDF.- Mise en place de l'installateur pour Windows.
<br/>
## [1.3.0] - 2025-01-18
- Redesign de l'onglet d'accueil.
- Ajout de l'onglet 'A Propos'.
- Corrige le problème des accents dans les noms de fichiers (Windows).
<br/>
## [1.2.0] - 2025-01-16
- Tentative de contournement de la lenteur extrême des QTextEdit
  quand ils sont mis à jour après un premier affichage.
<br/>
## [1.1.0] - 2025-01-13
- Lecture d'un fichier CSV pour obtenir la liste des communes et populations.
- Ajout au projet des fichiers CSV pour quelques métropoles:
    Bordeaux, Montpellier, Lille, Nantes, Nice, Toulouse.
    (ATTENTION: Seul Toulouse a les données officielles de l'Insee)
<br/>
## [1.0.0] - 2025-01-10
- Portage de la logique développée en Perl en C++17.
- Ajout d'une interface graphique (avec Qt 6.8) affichant les mêmes
  résultats que le programme Perl mais découpés en cinq onglets.
<br/>
## [0.1 .. 0.7] - 2024-12-xx
Version en ligne de commande écrite en Perl, avec les communes de Toulouse Métropole,
et leurs populations, hardcodées dans le programme.
<br/>
