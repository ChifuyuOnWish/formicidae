# Cahier des charges

**Durée :** 24 semaines, temps partiel (jeudi/vendredi), environ 48 jours ouvrés  
**Format :** Développement solo, soutenance bimensuelle (12 points d'étape)

---

## 1. Description & objectif

Formicidae est une simulation de colonie de fourmis développée sous Unreal Engine
(C++), centrée sur un système de phéromones ancré dans la biologie réelle : les
fourmis quittent la fourmilière, explorent, et tracent progressivement des
chemins à travers des dépôts de phéromones émergents et se dégradant dans le
temps, non via un pathfinding scripté. Le joueur agit comme une source de
phéromones supplémentaire plutôt que comme un contrôleur direct, orientant
l'exploration et redirigeant le trafic sans jamais totalement prendre le pas sur
le comportement émergent propre à la colonie.

Le projet s'articule autour de deux vues fonctionnant en continu et de manière
indépendante : une **vue exploration** (3D, au niveau du sol, où se forment les
chemins et où les dangers menacent les fourrageuses) et une **vue colonie**
(coupe 2D verticale, où les ressources se transforment en croissance,
répartition des castes et construction de salles). Les deux vues sont reliées
par une économie de ressources commune, ce qui est trouvé en surface finance ce
qui est construit en sous-sol.

**Objectif initial :** ce projet constitue le point d'entrée vers une prise en
main approfondie d'Unreal Engine et du développement C++ pour la simulation/le
jeu vidéo, en préparation d'un projet de plus grande ampleur envisagé au-delà de
cette période scolaire. La portée ci-dessous est calibrée en conséquence :
assez ambitieux pour réellement apprendre l'architecture du moteur et construire
un vrai système technique, assez borné pour rester réalistement livrable en
solo, à temps partiel, sur 24 semaines.

---

## 2. Portée — MoSCoW

### 2.1 Must Have (indispensable)

- Système central de dépôts de phéromones : dépôts en espace continu, force,
  décroissance dans le temps, détection basée sur un rayon, fusion avec dérive de
  position, plafond de force, marqueur d'origine (Fourmi/Joueur).
- Un agent fourmi fonctionnel : apparaît, détecte les dépôts environnants, se
  déplace selon une marche aléatoire pondérée, dépose sa propre phéromone en se
  déplaçant.
- Simulation d'un essaim de fourmis à une échelle significative (objectif :
  plusieurs centaines d'agents simultanés) avec formation de chemins visiblement
  émergente.
- Outil de traçage de phéromones pour le joueur (chemin de fourrage) et outil de
  redirection (règle du double ancrage, non auto-renforçant, décroissance sur
  une durée fixe).
- Repérage par caméra / révélation de la carte dans la vue exploration.
- Un danger environnemental avec une véritable conséquence de gameplay
  (effacement des chemins par la pluie/les flaques).
- Vue colonie : coupe 2D verticale, au moins 3 types de salles fonctionnelles
  (nurserie, jardin à champignons, une salle de stockage/déchets), flux de
  ressources depuis la vue exploration finançant la croissance de la colonie.
- Système de castes basique : au moins deux castes (fourrageuse, nourrice) avec
  un mécanisme de ratio cible et un délai lié au cycle de couvain.
- Bascule libre entre les deux vues, chacune continuant de tourner de manière
  indépendante.
- La boucle de jeu complète et jouable de bout en bout : explorer → découvrir →
  formation d'un chemin → retour des ressources → croissance de la colonie →
  évolution de la répartition des castes → capacité de fourrage accrue/améliorée.

### 2.2 Should Have (souhaitable)

- Second type de danger : pression de prédation des mouches phorides, incluant
  le mécanisme d'escorte par les ouvrières minimes.
- Système de résolution de problèmes non bloquant (jauges de moisissure/
  surpopulation avec conséquences progressives) pour au moins un type de salle.
- Roster de castes étendu (soldats/majors) avec une salle dédiée débloquant
  chaque caste.
- Placement procédural des ressources sur un terrain borné et modélisé à la main
  (distribution des sources de feuilles/dangers basée sur du bruit procédural,
  pour la rejouabilité).
- Passe UI/UX basique : visualisation lisible de la force des chemins, écran de
  gestion des salles/castes, système de notification pour les problèmes de la
  colonie.

### 2.3 Could Have (optionnel visé)

- Une seconde espèce jouable avec une mécanique centrale réellement différente
  (pas un simple reskin), par exemple une colonie mobile façon fourmis
  légionnaires.
- Flore/faune plus approfondie : plusieurs types de sources de feuilles avec une
  compatibilité différente au champignon, un second type de prédateur.
- Passe de polish visuel/audio au-delà de la clarté fonctionnelle (éclairage,
  ambiance, effets sonores).
- Systèmes procéduraux étendus (cycles de pluie saisonniers, variation du
  terrain).

---

## 3. Feuille de route — Points d'étape bimensuels

Chaque point d'étape correspond à environ 4 jours ouvrés de travail (2 jours par
semaine × 2 semaines).

**Étape 1: Recherche algorithmique & architecture**
Les deux premières semaines sont consacrées exclusivement à la recherche et à
l'architecture, sans implémentation dans le dépôt principal. L'algorithme de
suivi de phéromones par un grand nombre de fourmis simultanées présente un risque
de coût de calcul important à l'échelle visée (plusieurs centaines d'agents) ;
cette étape couvre l'étude des approches existantes et des optimisations
possibles (structures spatiales, fréquence de mise à jour, parallélisation),
menée dans un dépôt de recherche dédié et distinct du projet. Le résultat de
cette recherche alimente directement les choix d'architecture du système central
de dépôts (`FPheromoneDeposit`/`UPheromoneManager`), présentés et justifiés lors
de cette soutenance.

**Étape 2: Système central de dépôts**
Dépôt, décroissance, fusion (avec dérive de position), marqueur d'origine,
plafond de force implémentés et démontrés via une visualisation de debug ou un
harnais de test.

**Étape 3: Agent fourmi unique**
Une fourmi détecte et suit les dépôts, dépose sa propre phéromone, forme
visiblement un chemin vers une ressource statique unique au fil de répétitions.

**Étape 4: Essaim à l'échelle**
Plusieurs agents fourmis simultanés (objectif : monter vers la centaine),
convergence des chemins visiblement émergente, fusion/décroissance calibrées
pour un comportement correct à l'échelle.

**Étape 5: Outils de phéromones du joueur**
Traçage de chemin de fourrage et redirection (règle du double ancrage
appliquée), interagissant de manière démontrable et correcte avec les chemins
tracés par les fourmis.

**Étape 6: Repérage + premier danger**
Révélation de carte par caméra conditionnant où les chemins peuvent être tracés ;
danger pluie/flaques effaçant les chemins et forçant l'usage de la redirection
dans un scénario réel.

**Étape 7: Fondations de la vue colonie**
Coupe 2D verticale, au moins 3 types de salles, flux de ressources depuis la vue
exploration finançant visiblement l'état de la colonie.

**Étape 8: Système de castes**
Castes fourrageuse/nourrice avec mécanisme de ratio cible et délai de cycle de
couvain ; effet visible sur le débit de fourrage.

**Étape 9: Intégration de la boucle complète**
Les deux vues tournant simultanément, bascule libre, boucle de jeu jouable de
bout en bout depuis un démarrage à froid. Portée Must Have complète.

**Étape 10: Portée Should complète**
Danger phoride + mécanisme d'escorte, système de résolution de problèmes,
placement procédural des ressources, passe UI/UX.

**Étape 11: Bonus / polish**
Approfondissement et éléments bonus de la portée Could Have.

**Étape 12: Soutenance finale**
Build jouable complet, documentation finale.