# Formicidae : Dossier de première soutenance

## 1. Le projet

### 1.1 Concept et besoin

Formicidae est un jeu de simulation de fourmilière. Il est développé sous Unreal Engine, en C++. Le cœur du jeu est un système de phéromones inspiré de la biologie réelle.

Les fourmis sortent de la fourmilière et explorent. Elles déposent des phéromones qui s'accumulent puis disparaissent avec le temps. Les chemins ne sont pas calculés à l'avance par un algorithme classique : ils apparaissent naturellement, à partir de ces dépôts. Le joueur ne contrôle pas les fourmis directement. Il peut lui aussi déposer des phéromones, pour guider l'exploration ou rediriger le trafic. Mais il ne remplace jamais totalement le comportement naturel de la colonie.

Ce projet répond à deux besoins. D'abord, un besoin personnel : apprendre Unreal Engine en profondeur, pour pouvoir ensuite mener un projet plus ambitieux après l'école. Ensuite, un besoin repéré dans le genre du jeu de simulation de colonie : la plupart de ces jeux donnent soit un contrôle total sur chaque unité (comme un RTS classique), soit aucun contrôle réel (comme un jeu idle où tout tourne seul). Peu de jeux mettent le joueur dans le rôle d'un guide indirect, qui agit avec le même outil que les fourmis elles-mêmes : la phéromone.

### 1.2 Objectifs du projet

- Apprendre Unreal Engine et le C++ en partant d'un niveau débutant.
- Concevoir un système de simulation complexe (dépôts de phéromones en espace continu, décroissance, fusion, détection entre plusieurs agents), basé sur de vraies recherches et pas sur un simple tutoriel.
- Livrer une boucle de jeu jouable du début à la fin, correspondant au périmètre "Must Have" défini (voir section 4).
- Produire un travail de qualité portfolio, qui montre une vraie conception de système et pas seulement un assemblage de contenu.

### 1.3 Public cible / utilisateurs

Le jeu vise les joueurs qui aiment les simulations à comportement émergent. C'est un public proche de celui de Dwarf Fortress ou SimAnt. Ces joueurs aiment observer et comprendre les règles d'un système, puis expérimenter avec, plutôt que de suivre un parcours entièrement scripté.

### 1.4 Fonctionnalités principales envisagées

- Système central de dépôts de phéromones (force, décroissance, fusion, marqueur d'origine).
- Essaim de fourmis à grande échelle (plusieurs centaines d'agents visés), avec formation de chemins qui émerge naturellement.
- Outils de phéromones pour le joueur : traçage d'un chemin de fourrage, redirection.
- Un système de caméra qui limite où le joueur peut agir.
- Un danger environnemental (pluie et flaques) qui a un vrai impact sur les chemins.
- Une vue de la colonie en coupe 2D : salles, castes de fourmis basées sur un ratio cible, avec un délai pour faire naître de nouvelles fourmis.
- Un passage libre entre les deux vues (exploration et colonie), qui continuent chacune de fonctionner indépendamment.

### 1.5 Valeur ajoutée du projet

Deux choix rendent ce projet original dans son genre.

Premièrement, une simulation vraiment émergente : les chemins ne sont pas prédéfinis, ni recalculés par un algorithme de pathfinding classique. Ils viennent uniquement de l'accumulation et de la disparition progressive des dépôts de phéromones.

Deuxièmement, deux vues reliées par une seule économie de ressources. Ce qui est trouvé en surface finance directement ce qui est construit sous terre. Ce ne sont pas deux jeux séparés collés ensemble.

### 1.6 Justification des choix

**Espèce (fourmi champignonniste / leafcutter) :** cette espèce a été choisie pour ses points forts en biologie, faciles à transformer en gameplay. Le jardin à champignons impose une boucle de ressources en deux étapes (récolter une feuille, puis la rendre compatible avec le champignon). La diversité réelle des castes de cette espèce (des petites ouvrières qui protègent les fourrageuses contre des mouches parasites) donne un mécanisme de défense clair, pas juste un chiffre dans une statistique.

**Unreal Engine (C++) :** ce moteur a été choisi pour l'accès direct au C++, sans passer par une couche de script. Il offre aussi des outils de rendu instancié adaptés pour afficher des centaines d'agents, et des outils météo/éclairage utiles pour le danger pluie/flaques. Ce choix correspond aussi à l'objectif d'apprendre ce moteur sur le long terme.

**Dépôts en espace continu plutôt qu'une grille :** ce choix évite un problème courant du genre, où les chemins suivent une structure fixe et prévisible. Ce choix est plus complexe à mettre en place, mais ce coût est assumé et documenté (voir la recherche du Checkpoint 1, partie technique).

---

## 2. La stack technique

### 2.1 Technologies, frameworks, langages et outils

| Outil | Rôle |
|---|---|
| Unreal Engine 5 (C++) | Moteur de rendu, simulation, outils éditeur |
| C++ | Langage principal, systèmes performance-critiques |
| Git + Git LFS | Contrôle de version, gestion des assets binaires |
| GitHub | Hébergement, Issues, Milestones, gestion de projet |
| GitHub Actions | Vérification des commits/branches (CI), mirroring de sauvegarde |
| JetBrains Rider | IDE, intégration native Unreal (RiderLink) |

### 2.2 Justification des choix techniques

Unreal a été choisi plutôt qu'un moteur plus léger comme Godot, pour son rendu instancié natif. C'est utile pour afficher des centaines d'agents en même temps, et pour ses outils météo/particules utiles au système de dangers environnementaux.

Le C++ a été choisi plutôt que le Blueprint seul, car le cœur du projet est un système de simulation, pas une suite d'événements scriptés. Une architecture en Blueprint pur aurait été trop lente pour l'échelle visée.

Git LFS est nécessaire dès qu'un projet Unreal contient des fichiers binaires (`.uasset`, `.umap`), que Git seul gère mal. GitHub Actions permet de garder les conventions de commit et de nom de branche, même en travaillant seul. C'est important vu le côté professionnalisant du projet.

### 2.3 Architecture envisagée

Le système sépare clairement les données et les règles. `FPheromoneDeposit` est une simple structure de données (position, force, décroissance, origine), sans comportement propre. `UPheromoneManager` est un sous-système du monde Unreal. Il garde la seule liste de dépôts et applique toutes les règles (décroissance, fusion, détection, validation des redirections).

Les fourmis et les outils du joueur passent tous par cette même interface, au lieu de dupliquer les règles chacun de leur côté. Cela garantit un comportement identique, peu importe qui a créé le dépôt.

La vue colonie communique avec la vue exploration par un flux de ressources à sens unique : une ressource rapportée par une fourmi alimente l'état de la colonie, qui peut ensuite ajuster la répartition des castes. Cela change le nombre de fourrageuses disponibles côté exploration.

### 2.4 Schéma de communication entre les briques

```
                     +--------------------------+
                     |    UPheromoneManager      |
                     |  (sous-systeme du monde)  |
                     |  - stockage des depots    |
                     |  - decroissance / fusion  |
                     |  - requetes de detection  |
                     |  - validation redirection |
                     +------------^------^-------+
                                  |      |
                     requetes /   |      |  depots crees
                     depots crees |      |
                 +----------------+      +----------------+
                 |                                        |
     +-----------+------------+             +-------------+------------+
     |      Agent fourmi       |             |   Outils du joueur       |
     |  - detection (rayon)    |             |  - tracage de chemin     |
     |  - marche ponderee      |             |  - redirection           |
     |  - depot en mouvement   |             |  - reperage camera       |
     +-----------^--------------+             +--------------------------+
                 |
                 | ressource rapportee
                 v
     +--------------------------+
     |       Vue colonie         |
     |  - salles fonctionnelles  |
     |  - systeme de castes      |
     |  - flux de ressources     |
     +--------------------------+
                 |
                 | ratio de castes ajuste
                 v
     (nombre de fourrageuses disponibles cote exploration)
```

Les deux vues tournent indépendamment. Elles communiquent seulement par ce flux de ressources et l'ajustement du ratio de castes, jamais directement entre elles.

---

## 3. La méthodologie de travail

### 3.1 Organisation de l'équipe

Projet en solo. Comme il n'y a pas d'équipe, les pratiques normalement réparties entre plusieurs rôles sont appliquées par une seule personne, avec les mêmes garde-fous qu'en équipe (revue avant fusion, conventions imposées par les outils et non par la confiance).

### 3.2 Répartition des responsabilités

Tous les rôles sont tenus par la même personne : conception du jeu et des systèmes, développement C++, recherche technique, gestion de projet, vérification et tests. Cette liste est écrite volontairement, pour garder en tête ce qu'une équipe aurait normalement réparti entre plusieurs personnes.

### 3.3 Méthode de gestion de projet

La méthode utilisée est un Kanban adapté au travail solo. Le travail est découpé en issues, classées par priorité selon la méthode MoSCoW (Must/Should/Could). Elles sont organisées par jalon toutes les deux semaines (Milestone GitHub), plutôt que par sprint avec des cérémonies fixes. Chaque issue a une checklist de critères à vérifier avant d'être fusionnée. Cela joue le rôle qu'une revue de code par un collègue jouerait en équipe.

### 3.4 Outils utilisés

- **GitHub Issues / Milestones / Labels** : suivi du travail, priorisation MoSCoW, lien avec chaque jalon.
- **Git + Git LFS** : contrôle de version, gestion des fichiers binaires.
- **GitHub Actions** : vérification automatique des messages de commit et des noms de branche à chaque pull request, sauvegarde vers un second dépôt.
- **Git hooks locaux** (`commit-msg`, `pre-push`) : retour immédiat, avant même la vérification côté serveur.
- **JetBrains Rider** : environnement de développement.

### 3.5 Fréquence des points d'avancement

Un point d'avancement (soutenance) toutes les deux semaines, soit 12 au total sur 24 semaines. Chaque jalon correspond exactement à une Milestone GitHub, avec sa propre date. L'avancement réel est donc vérifiable directement dans l'outil, plutôt que simplement annoncé.

---

## 4. La roadmap

### 4.1 MVP

Le MVP correspond exactement au périmètre "Must Have", terminé au Checkpoint 9. Il couvre la boucle de jeu complète : explorer, découvrir, former un chemin, rapporter une ressource, faire grandir la colonie, ajuster les castes, ce qui augmente la capacité de fourrage.

### 4.2 Fonctionnalités prévues après le MVP

Périmètre "Should Have" (Checkpoint 10) : un second danger (mouches parasites), un système de résolution de problèmes non bloquant, un placement procédural des ressources, une passe UI/UX.

Périmètre "Could Have" (Checkpoint 11) : une seconde espèce jouable, plus de flore et de faune, du polish visuel et audio.

### 4.3 Roadmap détaillée

| Étape | Date de soutenance | Livrable principal | Statut |
|---|---|---|---|
| 1 | 25/09/2026 | Recherche algorithmique (ACO, APF, fréquence de mise à jour) et architecture (FPheromoneDeposit / UPheromoneManager) | Pré-MVP |
| 2 | 09/10/2026 | Système central de dépôts (dépôt, décroissance, fusion, plafond) | Pré-MVP |
| 3 | 23/10/2026 | Agent fourmi unique, formation d'un chemin vers une ressource statique | Pré-MVP |
| 4 | 06/11/2026 | Essaim à l'échelle, calibrage et validation de performance | Pré-MVP |
| 5 | 20/11/2026 | Outils de phéromones du joueur (traçage, redirection) | Pré-MVP |
| 6 | 04/12/2026 | Repérage par caméra, premier danger (pluie/flaques) | Pré-MVP |
| 7 | 18/12/2026 | Fondations de la vue colonie (salles, flux de ressources) | Pré-MVP |
| 8 | 01/01/2027 | Système de castes (ratio cible, délai de couvain) | Pré-MVP |
| 9 | 15/01/2027 | Intégration de la boucle complète | **MVP livré** |
| 10 | 29/01/2027 | Périmètre Should Have complet | Post-MVP |
| 11 | 12/02/2027 | Bonus / polish (périmètre Could Have) | Post-MVP |
| 12 | 26/02/2027 | Build final, documentation, soutenance finale | Livraison finale |

### 4.4 Phase de tests, corrections et amélioration

Les tests ne sont pas gardés pour une seule phase à la fin du projet. Chaque issue a sa propre checklist de critères, vérifiée avant chaque fusion sur `main`. La vérification est donc répartie sur les 12 étapes, plutôt que concentrée à la fin.

Le Checkpoint 9 inclut une revue formelle du périmètre "Must Have" (les écarts éventuels sont notés et expliqués). Le Checkpoint 11 sert à la fois de zone bonus et de marge pour corriger ce qui aurait été reporté des étapes précédentes.

### 4.5 Livraison finale

Checkpoint 12 : build final packagé, documentation d'architecture et d'utilisation à jour, et une rétrospective écrite qui compare ce qui a été livré à ce qui était prévu.

---

## 5. Les risques

| Risque | Anticipation / résolution |
|---|---|
| **Technique : performance à l'échelle visée** (centaines d'agents) | Une recherche dédiée est faite dès le Checkpoint 1 (comparaison scan linéaire / grille spatiale, seuil estimé, pistes de parallélisation identifiées). Le Checkpoint 4 réserve du temps pour le profilage avant de construire d'autres systèmes par-dessus. |
| **Technique : apprentissage du moteur et du C++ depuis zéro** | Le périmètre "Must Have" est volontairement modeste, calculé sur le temps réellement disponible (temps partiel), pas sur une ambition théorique. |
| **Manque de temps** (24 semaines à temps partiel, environ 48 jours ouvrés) | Le découpage MoSCoW permet d'ajuster facilement le périmètre "Should/Could" en cas de retard, sans surprise de dernière minute. Les soutenances toutes les deux semaines permettent de repérer un retard tôt. |
| **Difficulté liée à une fonctionnalité spécifique** (comportement émergent difficile à lire, règle de redirection complexe) | Les règles de conception sont entièrement écrites avant d'être codées (voir documents de recherche et d'architecture). Cela réduit les zones floues au moment du code. Les étapes 3 et 4 sont dédiées au calibrage, avant que d'autres systèmes ne s'appuient dessus. |
| **Dépendance externe : Unreal Engine lui-même** (bugs, particularités Linux déjà rencontrées à l'installation) | La version du moteur est fixée une fois choisie. Pas de montée de version en cours de projet, sauf nécessité absolue. La chaîne d'outils est déjà stable après l'installation initiale. |
| **Organisation de l'équipe : point unique de défaillance** (projet solo, pas de redondance en cas de blocage) | Les soutenances toutes les deux semaines forcent une visibilité régulière depuis l'extérieur. Un blocage remonte donc vite, au lieu de rester invisible jusqu'à la fin. Le découpage MoSCoW absorbe une partie du risque de planning, sans besoin de redistribuer le travail à quelqu'un d'autre. |
