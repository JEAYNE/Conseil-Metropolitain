#!/usr/bin/perl
# Copyright (C) 2024-2025 JEAYNE@GitHub
# SPDX-License-Identifier: GNU General Public License v3.0 only
#
# NOTES
# - Ce fichier source est encod� en UTF-16 avec le code page 'Windows-1252' (pas UTF-8)
# Le nom des fichiers lus ou �crits par ce programme sont aussi encod�s avec cette page.
# - MAIS Le contenu des fichiers publi�s par l'Insee est encod� avec le format UTF-8
# Le contenu des fichiers csv g�n�r�s par ce programme est aussi encod� avec le format UTF-8

use strict;
use warnings;
use Encode qw(decode encode from_to);
use Data::Dump qw(dd);


##
## Jongleries entre les encodages utf8 et iso-8859-1 (aka Latin-1, utilis� par Windows)
##

sub utf8_to_win {
	my $str = shift;
	from_to($str, "utf8", "iso-8859-1");
	return $str;
}

sub win_to_utf8 {
	my $str = shift;
	from_to($str, "iso-8859-1", "utf8");
	return $str;
}


##
## Chargement de la liste des m�tropoles
##
## EPCI;LIBEPCI;NATURE_EPCI;NB_COM

my %listeMetropoles;
sub chargeMetropoles {
	my $data = shift;
	# 'ME' en 2019; 'METRO' en 2025;
	return unless $data->{NATURE_EPCI} =~ /^ME(TRO)?$/;
	# C'est une m�tropole
	$listeMetropoles{$data->{EPCI}} = {
		NB_COM  => $data->{NB_COM},
		LIBEPCI => $data->{LIBEPCI}
	};
}


##
## Chargement de la liste des communes des m�tropole
##
## CODGEO;LIBGEO;EPCI;LIBEPCI;DEP;REG

my %listeCommunes;
my %indexCommunesParMetropoles;		# un hash de array
sub chargeCommunes {
	my $data = shift;
	my $EPCI = $data->{EPCI};
	return unless exists $listeMetropoles{$EPCI};
	# C'est une commune appartenant � une m�tropole
	my $CODGEO = $data->{CODGEO};
	push @{$indexCommunesParMetropoles{$EPCI}}, $CODGEO;
	$listeCommunes{$CODGEO} = {
		DEP     => $data->{DEP},
		LIBGEO  => $data->{LIBGEO},
		EPCI    => $data->{EPCI}
	}
}


##
## Chargement de la population municipale des communes des m�tropoles
##
## ATTENTION Ce fichier ne semble pas fait pour �tre trait� par des programmes.
##   - Les noms des champs de cette feuille contiennent des espaces et des accents (UTF8)
##   - L'identifiant habituel d'une commune (CODGEO) est �clat� en deux champs 'Code d�partement' et 'Code commune'
##   - Des espaces ins�cables 'nbsp' (UTF8: \xC2\xA0) sont ins�r�s entre les milliers : '1 234' au lieu de '1234'
##   "Code r�gion" "Nom de la r�gion" "Code d�partement" "Code arrondissement" "Code canton" "Code commune"
##   "Nom de la commune" "Population municipale" "Population compt�e � part" "Population totale"
## De plus :
##   Les trois ville avec arrondissements (Paris, Lyon, Marseille) ne figurent pas dans le fichier ensemble.xlsx produit par l'Insee
##   mais leurs arrondissements y sont. Il faut donc agr�ger les populations des arrondissements pour avoir celle de la ville
##   et l'ins�rer dans la liste.

sub chargePopulations {
	my $data = shift;
	my $dep = $data->{win_to_utf8('Code d�partement')};
	my $CODGEO = $dep. $data->{'Code commune'};
	(my $pop = $data->{'Population municipale'}) =~ s/ |\xC2\xA0//g;  # supprime les espaces normaux et ins�cables (NBSP): 1 234 567 --> 1234567
	# Ajout du champ POPCOM � la liste %listeCommunes
	$listeCommunes{$CODGEO}{POPCOM} = $pop if exists $listeCommunes{$CODGEO};
	# D�tection des cas de Paris, Lyon et Marseille dont il faut agr�ger les arrondissements
	return unless ($dep eq "13") || ($dep eq "69") || ($dep eq "75");  # on utilise 'eq' et non '==' � cause des '2A' et '2B' de la Corse.
	return unless $data->{'Nom de la commune'} =~ /^(Paris|Lyon|Marseille)\s+\d+er?\s+Arrondissement$/;
	my $ville = $1;
	if(    $ville eq "Paris"     ) { $CODGEO = 75056; }
	elsif( $ville eq "Lyon"      ) { $CODGEO = 69123; }
	elsif( $ville eq "Marseille" ) { $CODGEO = 13055; }
	else{ die "La ville '$ville' n'est ni Paris ni Lyon ni Marseille"; }
	$listeCommunes{$CODGEO}{POPCOM} += $pop;
}


##
## Fonction g�n�rique pour charger un fichier CSV
##

sub dumpFields {
	print "dumpField: ";
	dd shift;
}

sub chargeFichierCSV {
	my %params = @_;
	my $trace   = $params{trace}   || 0;
	my $fichier = $params{fichier} || '';
	unless( $fichier ){
		die "Fichier � lire non d�finit � l'appel de chargeFichierCSV()\n";
	}
	print "Chargement du fichier '$fichier'\n";

	open(my $in, "<", $fichier)
	  || die "ERREUR : Ne peut pas ouvrir le fichier '$fichier'\n$!\n";

	my $sep     = $params{sep} || ",";
	my $process = $params{process} || \&dumpFields;

	my (@fields, @names, %namedData);
	my $lineCpt = 0;
	my $dataCpt = 0;
	while( my $line= <$in> ){
		$lineCpt ++;
		chomp $line;
		$trace && printf("ligne %6d: '%s'\n", $lineCpt, $line);
		next if $line =~ /^\s*(#.*)?\s*$/;
		@fields = split(/\s*$sep\s*/, $line);
		$dataCpt ++;
		if( $dataCpt==1 ){
			# premiere ligne utile: nom des champs
			@names = @fields;
			next;
		}
		@namedData{@names} = @fields;
		$trace && dd \%namedData;
		&{$process}(\%namedData);
	}
	close $in;
}


##
## Main
##

sub usage {
print << "EOM";

USAGE :

  $0  <Metropoles>  <Communes>  <Populations>  <Resultats>

  <Metropoles>
    Nom du fichier issue de l'extraction au format csv de la feuille 'EPCI'

  <Communes>
    Nom du fichier issue de l'extraction au format csv de la feuille 'Composition_communale'

  <Populations>
    Nom du fichier issue de l'extraction au format csv de la feuille 'communes'

  <Resultats>
    Nom du r�pertoire o� stoker les fichiers csv pour chaque m�troples.

  IMPORTANT: les fichiers csv en entr�e doivent �tre encod�s en UTF8.
             Les fichiers csv en sortie seront encod�s en UTF8.
EOM
}

#### V�rification des 4 param�tres de la ligne de commandes

if( @ARGV != 4 ){
    print "ERREUR : Nombre de param�tres invalide. 4 param�tres attendus.\n";
    usage();
    exit(1);
}

my $erreurs_cpt = 0;

my $fichierMetropoles = $ARGV[0];
if(! -f $fichierMetropoles ){
    print "ERREUR : Le premier fichier de donn�es '$fichierMetropoles' n'existe pas.\n";
	$erreurs_cpt ++;
}

my $fichierCommunes = $ARGV[1];
if(! -f $fichierCommunes ){
    print "ERREUR : Le deuxi�me fichier de donn�es '$fichierCommunes' n'existe pas.\n";
	$erreurs_cpt ++;
}

my $fichierPopulations = $ARGV[2];
if(! -f $fichierPopulations ){
    print "ERREUR : Le troisi�me fichier de donn�es '$fichierPopulations' n'existe pas.\n";
	$erreurs_cpt ++;
}

my $repResultats = $ARGV[3];
if(! -d $repResultats ){
    print "ERREUR : Le r�pertoire de sortie '$repResultats' n'existe pas.\n";
	$erreurs_cpt ++;
}

if( $erreurs_cpt ){
    usage();
    exit(1);
}

### Chargements

# Feuille 'EPCI'
chargeFichierCSV(
  fichier  => $fichierMetropoles,
  sep      => ";",
  process  => \&chargeMetropoles,
  trace    => 0
);
printf("  Chargement de %d m�tropoles\n", scalar(keys %listeMetropoles));

# Feuille 'Composition_communale'
chargeFichierCSV(
  fichier  => $fichierCommunes,
  sep      => ";",
  process  => \&chargeCommunes,
  trace    => 0
);
printf("  %d communes appartenant � une m�tropole\n", scalar(keys %listeCommunes));

# V�rification que chaque m�tropole a le bon nombre de communes
for my $EPCI (keys %indexCommunesParMetropoles){
	my $attendu = $listeMetropoles{$EPCI}{NB_COM};
	my $obtenu  = scalar @{$indexCommunesParMetropoles{$EPCI}};
	if( $attendu != $obtenu ){
		print "  ERREUR : la m�tropole", $listeMetropoles{$EPCI}{LIBEPCI},
			" devrait avoir $attendu communes, mais $obtenu ont �t� trouv�es.\n";
		$erreurs_cpt ++;
	}
}

exit(2) if $erreurs_cpt;

# Feuille 'Communes'

chargeFichierCSV(
  fichier  => $fichierPopulations,
  sep      => ";",
  process  => \&chargePopulations,
  trace    => 0
);


# V�rification que chaque commune d'une m�tropole a une population
# et determine le nom de la commune centre (le plus de population)
for my $EPCI (keys %indexCommunesParMetropoles){
	my $maxPop = 0;
	for my $commune (@{$indexCommunesParMetropoles{$EPCI}}){
		my $pop = $listeCommunes{$commune}{POPCOM} || 0;
		if( $pop > 0 ){
			if( $pop > $maxPop ){
				$listeMetropoles{$EPCI}{centre} = $commune;
				$maxPop = $pop;
			}
		}else{
			printf("  ERREUR : Population municipale non d�finit pour la commune '%s' de la m�tropole '%s'\n",
				$listeCommunes{$commune}{LIBGEO},
				$listeMetropoles{$EPCI}{LIBEPCI} );
			$erreurs_cpt ++;
		}
	}
}

exit(2) if $erreurs_cpt;

sub sortMetropoles {
	$listeCommunes{ $listeMetropoles{$a}{centre} }{LIBGEO} cmp $listeCommunes{ $listeMetropoles{$b}{centre} }{LIBGEO}
}

# Pour chaque m�trople, g�n�rer le fichier des communes de la m�tropole et de leurs populations
my $cptMetropoles = 0;
my $popTotal = 0;
my $comTotal = 0;
for my $EPCI (sort { sortMetropoles() } keys %indexCommunesParMetropoles){

	my $metropole = utf8_to_win($listeMetropoles{$EPCI}{LIBEPCI});
	my $centre = utf8_to_win( $listeCommunes{ $listeMetropoles{$EPCI}{centre} }{LIBGEO} );
	(my $fichier = $centre) =~ tr/' _/-/;
	$fichier = "$repResultats/$fichier.csv";

	open(my $out, ">", $fichier)
	  || die "ERREUR : Ne peut cr�er le fichier '$fichier'\n$!\n";
	my $cptCommunes = 0;
	my $popMetropole = 0;
	for my $commune (@{$indexCommunesParMetropoles{$EPCI}}){
		my $pop = $listeCommunes{$commune}{POPCOM};
		printf($out "%s;%s\n", $listeCommunes{$commune}{LIBGEO}, $pop);
		$popMetropole += $pop;
		$cptCommunes ++
	}
	close $out;

    my $popCentre = $listeCommunes{ $listeMetropoles{$EPCI}{centre} }{POPCOM};
	my $popP100 = int(100 * $popCentre/$popMetropole +0.5);
	print "\n==== $metropole ====\n";
	print "  Nombre de communes: $cptCommunes\n";
	print "          Population: $popMetropole\n";
	print "        Ville centre: $centre ($popCentre $popP100%)\n";
	print "             Fichier: $fichier\n";

	$popTotal += $popMetropole;
	$comTotal += $cptCommunes;
	$cptMetropoles++;
}

print "\n==== GLOBAL ====\n";
print "  Nombre de m�tropoles: $cptMetropoles\n";
print "    Nombre de communes: $comTotal\n";
print "     Population totale: $popTotal\n";
print "\n";
