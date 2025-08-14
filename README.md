# INF3173-243-TP2 - Détection des fichiers identiques

L'outil à développer dans ce TP consiste à détecter des fichiers identiques. Par exemple, on pourrait libérer de l'espace en supprimant les fichiers redondants.

La façon simple serait de comparer chaque fichiers à tous les autres fichiers. Si nous avons N fichiers, alors il s'agit d'un algorithme ineffiscient en temps O(N^2). Comment faire mieux?

Une méthode efficace se base sur le calcul d'une somme de contrôle du fichier (hachage). Si la somme de contrôle est différente pour les deux fichiers, alors on est certain qu'il s'agit de deux fichiers différents. Il prend une valeur en entrée de taille variable, et produit un nombre pseudo-aléatoire en sortie. La même entrée produit toujours la même sortie. Voici un exemple avec une fonction simple (avec le nombre de sortie représenté en hexadécimal):

```
# somme de contrôle du caractère "b" 01100010
echo b | md5sum
3b5d5c3712955042212316173ccf37be

# somme de contrôle du caractère "c" 01100011
# une entrée légèrement différente (1 bit de différence)
# donne une somme de contrôle complètement différente
echo c | md5sum
2cd6ee2c70b0bde53fbe6cac3c8b8bb1

# recalculer la somme de contrôle sur la même entrée
# donne toujours un résultat identique
echo c | md5sum
2cd6ee2c70b0bde53fbe6cac3c8b8bb1
```

Si la somme de contrôle de deux fichiers est identique, alors il est probable qu'ils aient le même contenu. Dans de rares cas, deux fichiers différents pourraient donner la même somme de contrôle. On parle alors d'une collision. Pour comprendre, voici un exemple exagéré, dans lequel on aurait une somme de contrôle sur 2-bit seulement. Comme il n'existe que 4 possibilités (00, 01, 10, 11), alors il y aura forcément au moins une collision en calculant la somme de contrôle de 5 entrées différentes.

Pour les fins du présent travail, nous allons ignorer les collisions, mais en réalité, il faurait comparer le contenu des fichiers dont la somme de contrôle est identique avant de détruire une des deux copies présumées.

Utiliser une somme de contrôle permet d'ajouter les fichiers dans une table de hachage, dont la clé est la somme de contrôle, et la valeur est une liste de fichiers qui ont cette somme de contrôle. Ainsi, trouver les fichiers dupliqués dans une collection de N fichiers revient à calculer la somme de contrôle de chaque fichiers de manière indépendante, qui est une opération O(N).

Le code de départ contient 4 programmes. La compilation

* genfiles: utilitaire pour générer une collection de fichiers avec doublons
* finddup: utilitaire principal pour trouver les fichiers dupliqués
* test_finddup: tests permettant de vérifier votre implémentation
* bench_finddup: outil de mesure de performance

Le programme `finddup` est fourni avec une version simple fonctionnelle. Voici comment détecter les doublons dans le répertoire `repo`. Le programme affiche les groupes de fichiers qui ont la même somme de contrôle.

```
finddup -m simple -b 32 -p data/content_%06d.dat
# option -p : pattern pour les fichiers
# option -m : méthode (simple, fread, read, mmap)
# option -b : taille de bloc
```

Pour tester, vous pouvez générer votre propre collection de fichiers avec `genfiles`. Ceci permet de varier la taille, le nombre de fichiers et la probabilité d'avoir des doublons. Les fichiers sont créés dans le sous-répertoire `repo`. Voici un exemple.

```
genfiles -n 100 -s 2 -s 1024
# option -n : nombre de fichiers à créer
# option -s : nombre de fichiers différents
# option -b : taille des fichiers (tous les fichiers ont la même taille)
```

### Étape 1 : Implémentation

L'algorithme de hachage `crc32` est fourni et doit être utilisé. La fonction prend 3 arguments:

* la somme de contrôle de départ
* l'adresse d'un tampon de données
* la taille du tampon (en octets)

La fonction retourne la somme de contrôle. La fonction est incrémentale, c-à-d que la somme sur l'ensemble des données est égal à la somme sur les parties des données. Voici un exemple pour illustrer cette propriété.

```
int x, y;
// somme sur la totalité des données
x = crc32(0, {A, B}, 2);

// somme incrémentale sur la moitiée des données à la fois
y = crc32(0, {A}, 1);
y = crc32(y, {B}, 1);
x == y // vrai
```

Étudiez le fichier `finddup_simple.c` pour comprendre le fonctionnement général. La fonction contient une boucle sur tous les fichiers à traiter, et pour chacun d'eux, calcule la somme de contrôle et l'ajoute dans la table de hachage.

La version simple fournie comporte une limitation importante. On lit le fichier au complet en mémoire. Cela fonctionne seulement pour les fichiers relativement petits. Dans le cas d'un grand fichier (un fichier plus grand que la mémoire vive disponible), alors l'ordinateur pourrait manquer de mémoire. Pour éviter cette situation, vous devez implémenter des variantes basées sur un calcul incrémental de la somme de contrôle. Concrètement, vous devez compléter les 3 variantes suivantes (dans leurs fichiers respectifs):

* finddup_fread() : lecture basée sur fread()
* finddup_read() : lecture basée sur read()
* finddup_mmap() : lecture basée sur mmap()

Pour la variante mmap(), faites un seul appel pour projeter en mémoire la totalité du fichier. La différence avec le cas simple est que le système d'exploitation chargera les pages au fur et à mesure des accès, et les libèrera automatiquement si la mémoire libre est faible, ce qui fait en sorte que le programme ne risque pas d'épuiser la mémoire de l'ordinateur et pourra traiter des fichiers de taille arbitraire.

Par contre, vous *devez* respecter le paramètre de la taille de bloc `block_size`. Autrement dit, il faire un nombre d'appel minimal à `fread()` ou `read()` et de taille `block_size` maximum. Les tests vérifient ce comportement.

### Étape 2 : Validation

Des tests sont fournis pour vérifier le fonctionnement. La vérification se fait en comparant la table de hachage produite avec le résultat attendu, sur des fichiers préexistants dans l'archive de départ. On vérifie également le nombre d'appels à `fread()`, `read()` et `mmap()` pour vérifier le respect des contraintes.

Les tests sont réalisés avec Catch2 dans le fichier `test/test_finddup.cpp`. Vous n'avez pas besoin d'écrire des tests. Vous pouvez les modifier ou en ajouter selon votre choix, mais ils ne seront pas considérés pour la correction.

Vous pouvez vérifier les fuites de mémoire une fois l'implémentation complète et que les tests passent.

Assurez-vous que votre programme soit valide avant de passer à l'étape suivante. S'il se produit une fuite mémoire, ou si des fichiers ne sont pas fermés, alors la mesure de performance pourrait être faussée.

### Étape 3 : Mesure de performance

Le programme de mesure de performance `bench_finddup` mesure le temps d'exécution des 3 variantes, en faisant varier la taille de bloc.

Premièrement, vous devez recompiler votre programme avec les optimisations. Cela se fait en sélectionnant l'option `Release` dans QtCreator, ou avec l'option `CMAKE_BUILD_TYPE=Release`. Par exemple:

```
cd build
cmake -DCMAKE_BUILD_TYPE=Release .
ninja # ou make
```

Avant de rouler le test, assurez-vous de fermer tous les programmes qui pourraient perturber la mesure, comme un navigateur Web ou des jeux.

Par défaut, le banc d'essai répète chaque mesure 100 fois pour obtenir une moyenne. Pour réduire le temps d'exécution total, vous pouvez baisser le nombre de répétitions. Voici une suggestion de commande pour afficher seulement la moyenne et répéter chaque mesure 10 fois seulement. Les résultats seront produits à la sortie standard.

```
bench_finddup --benchmark-no-analysis --benchmark-samples=10
```

À l'aide de ces mesures, produisez un court rapport sous forme d'une présentation PowerPoint (3-4 diapos max). Voici les éléments de votre rapport.

* Identification du cours et du groupe, nom et code permanent des participants de l'équipe.
* Un graphique du temps d'exécution en fonction de la taille de bloc.
* Une analyse brève abordant l'effet de la taille de bloc, la fonction la plus performante, etc.
* Dans un logiciel en pratique, quelle taille de bloc utiliseriez-vous par défaut? Justifiez.

## Correction sur Hopper
(Note obtenue 130/130)

Votre code sera corrigé sur le système [Hopper](https://hopper.info.uqam.ca). Le système vous permet de former votre équipe pour ce TP. Faire l'archive avec `make remise` ou `ninja remise`, puis envoyer l'archive produite sur le serveur de correction. Votre note sera attribuée automatiquement. Vous pouvez soumettre votre travail plusieurs fois et la meilleure note sera conservée. D'autres détails et instructions pour l'utilisation de ce système seront fournis.

Vous devez imprimer et remettre votre rapport au secrétariat avant la date limite de remise.

Barème de correction:

 * Implémentation finddup_fread() : 33
 * Implémentation finddup_read() : 33
 * Implémentation finddup_mmap() : 34
 * Respect du style (voir fichier .clang-format): pénalité max 10
 * Qualité (fuite mémoire, gestion d'erreur, avertissements, etc.): pénalité max 10
 * Total code sur 100 points
 * Total rapport: 30 points (correction manuelle)
 * Grand total sur 130 points

Le non-respect de la mise en situation pourrait faire en sorte que des tests échouent. Il est inutile de modifier les tests pour les faire passer artificiellement, car un jeu de test privé est utilisé. Se référer aux résultats de test pour le détail des éléments vérifiés et validés. En cas de problème, contactez votre enseignant.

Bon travail !

# Note sur les logiciels externes

Le code intègre les librairies et les programmes suivants.

* https://github.com/catchorg/Catch2
* https://github.com/andyleejordan/c-list
* https://github.com/tidwall/hashmap.c
* https://github.com/eternalharvest/crc32-11

