# Swarm - Collection de Mods DayZ

Une collection de mods pour DayZ Standalone incluant des ajustements de gameplay, des configurations d'animaux et des fonctionnalités de qualité de vie.

## Changements Récents

### Nouvelles Fonctionnalités
- **SwarmSpectator** - Nouveau mod pour contrôles de caméra spectateur améliorés avec support Dolly Cam
- **launch.bat** - Lanceur automatique serveur + client pour tests locaux avec support BattleEye
- **DAYZ_CLIENT** variable d'environnement - Configurer le chemin d'installation du client DayZ
- **MODS** variable d'environnement - Définir l'ordre de chargement des mods pour les tests

### Organisation des Scripts
- Scripts de build consolidés : `build.bat` compile maintenant tous les packages automatiquement
- Renommage `validate_scripts.bat` → `validate.bat` pour cohérence
- Renommage du répertoire `validation/` → `validate/`
- Tous les répertoires temporaires utilisent maintenant un nommage cohérent (`.build_temp`, `.validate_temp`, `.launch_temp`)

### Améliorations BattleEye
- Correction des crashes serveur en gardant BattleEye **activé** (non désactivé)
- Le client utilise maintenant `DayZ_BE.exe` pour une initialisation BattleEye correcte
- Suppression des flags filePatching (non nécessaires pour tester les mods)
- La config serveur définit automatiquement `BattlEye = 1` pour la stabilité

### Nettoyage CI/CD
- Suppression des workflows GitHub Actions (`.github/workflows/`)
- Simplification vers un focus développement local avec scripts batch

## Mods Inclus

### SwarmTweaks
Modifications et ajustements de gameplay :
- **LessLaugh** - Fréquence de rire réduite
- **LessCough** - Fréquence de toux réduite
- **CraftImprovisedSuppressor** - Suppresseurs improvisés craftables
- **NoBuryItems** - Empêche d'enterrer des objets
- **ItemsInBack** - Objets visibles sur le dos du personnage

### SwarmAnimals
Configurations de dépeçage et de récolte pour divers animaux :
- Loup (Canis Lupus)
- Chèvre (Capra Hircus)
- Mouton (Ovis Aries)
- Renne (Rangifer Tarandus)
- Sanglier (Sus Scrofa)
- Ours brun (Ursus Arctos)
- Ours polaire (Ursus Maritimus)

### SwarmEarplugs
Système de contrôle du volume avec widget de bouchons d'oreilles en jeu :
- Niveaux de volume ajustables (Max, Moyen, Bas, Muet)
- Support de raccourcis personnalisés
- Indicateur visuel (widget)

## Structure du Projet

```
Swarm/
├── src/
│   ├── SwarmTweaks/        # Mod d'ajustements de gameplay
│   ├── SwarmAnimals/       # Configurations d'animaux
│   └── SwarmEarplugs/      # Contrôle du volume/bouchons
├── dist/                   # Sortie de build (gitignored)
│   └── @Swarm/
│       ├── Addons/         # Fichiers PBO compilés
│       ├── Keys/           # Clé publique de signature
│       └── meta.cpp
├── keys/                   # Clés de signature (clé privée gitignored)
├── scripts/                # Scripts d'aide au build + steamcmd.exe
├── validate/             # Configuration de validate des scripts
│   ├── mods/               # Mods de dépendance pour la validate
│   ├── serverDZ.cfg        # Config serveur personnalisée (gitignored)
│   └── serverDZ.default.cfg # Config de validate par défaut
├── .github/workflows/      # Pipelines CI/CD
└── *.bat                   # Scripts de build
```

## Prérequis

### Requis
- **DayZ Tools** - Installer via Steam (gratuit avec DayZ)
- **Windows** - Les scripts de build sont des fichiers batch Windows

### Optionnel (pour la validate des scripts)
- **DayZ Server** - Requis pour `validate.bat` et la validate à la publication

### Configuration de l'Environnement

Créez un fichier `.env` à la racine du projet avec vos chemins :

```ini
# Requis - Chemin vers l'installation de DayZ Tools
DAYZ_TOOLS=G:\SteamLibrary\steamapps\common\DayZ Tools

# Optionnel - Chemin vers DayZ Server (pour la validate des scripts)
DAYZ_SERVER=G:\SteamLibrary\steamapps\common\DayZServer

# Requis pour la publication - Identifiants Steam
STEAM_USERNAME=votre_nom_utilisateur_steam
WORKSHOP_ID=1234567890
```

Ou définissez les variables d'environnement manuellement :

```batch
setx DAYZ_TOOLS "G:\SteamLibrary\steamapps\common\DayZ Tools"
setx DAYZ_SERVER "G:\SteamLibrary\steamapps\common\DayZServer"
```

## Compilation

### Compiler Tous les Packages

```batch
build.bat --version 1.0.0
```

Cela va :
1. Mettre à jour les numéros de version dans tous les fichiers `meta.cpp` et `config.cpp`
2. Compiler les fichiers PBO avec DayZ Tools AddonBuilder
3. Générer dans `dist/@Swarm/Addons/`

### Sortie de Compilation

Après la compilation, vous trouverez :
```
dist/@Swarm/
├── Addons/
│   ├── SwarmTweaks.pbo
│   ├── SwarmAnimals.pbo
│   └── SwarmEarplugs.pbo
└── meta.cpp
```

## Validate des Scripts

Le système de validate permet de vérifier que vos scripts compilent correctement avant la publication en exécutant une instance de DayZ Server.

### Exécuter la Validate

```batch
validate.bat
```

Cela va :
1. Démarrer une instance minimale de DayZ Server
2. Charger votre mod et tous les mods de dépendance
3. Vérifier les erreurs de compilation de scripts
4. Signaler toutes les erreurs trouvées dans les logs

### Options de Validate

```batch
validate.bat --timeout 90      # Timeout personnalisé (défaut: 60s)
validate.bat --skip-build      # Ignorer la vérification de build
validate.bat --help            # Afficher l'aide
```

### Configuration du Serveur

Le système de validate utilise les configs serveur dans `validate/` :

| Fichier | Usage |
|---------|-------|
| `serverDZ.cfg` | Config personnalisée (gitignored) - pour votre map spécifique |
| `serverDZ.default.cfg` | Config par défaut - utilise Chernarus offline |

Pour utiliser une map différente (ex: Namalsk) :
1. Copiez `serverDZ.default.cfg` vers `serverDZ.cfg`
2. Modifiez le template de la classe `Missions`

Exemple de `serverDZ.cfg` pour Namalsk :
```cpp
// ... autres paramètres ...
class Missions {
    class Swarm_Validate {
        template="hardcore.namalsk";
    };
};
```

### Mods de Dépendance

Si votre mod dépend d'autres mods (CF, Community Framework, etc.), placez-les dans `validate/mods/` :

```
validate/mods/
├── @CF/
│   └── Addons/
│       └── CF.pbo
├── @Community-Online-Tools/
│   └── Addons/
│       └── ...
└── README.md
```

**Conseils :**
- Utilisez des **liens symboliques** pour éviter de dupliquer les gros dossiers de mods :
  ```batch
  mklink /D "@CF" "C:\Chemin\Vers\Steam\workshop\content\221100\@CF"
  ```
- **Ordre de chargement** : Les mods sont chargés par ordre alphabétique. Utilisez des préfixes numériques si nécessaire (ex: `@1_CF`, `@2_COT`)
- Le dossier `validate/mods/` est gitignored (sauf README.md)

## Publication sur le Steam Workshop

Le script `publish.bat` fournit un workflow complet : build, validate, signature et publication sur le Steam Workshop via SteamCMD.

### Configuration de SteamCMD

SteamCMD est inclus dans le dossier `scripts/`. Avant votre première publication, mettez en cache vos identifiants Steam :

```batch
scripts\steamcmd.exe +login votre_nom_utilisateur_steam +quit
```

Entrez votre code Steam Guard lorsque demandé - les identifiants seront mis en cache pour les prochaines exécutions.

### Workflow de Publication Complet

```batch
publish.bat --version 1.0.0
```

Cela va :
1. **Compiler** tous les packages avec la version spécifiée
2. **Valider** les scripts via DayZ Server (si `DAYZ_SERVER` est défini)
3. **Signer** tous les fichiers PBO avec `.bikey/.biprivatekey`
4. **Publier** sur le Steam Workshop via SteamCMD

### Options de Publication

| Option | Description |
|--------|-------------|
| `--version X.X.X` | Numéro de version (requis) |
| `--key NomClé` | Nom de clé personnalisé pour la signature (défaut: Swarm) |
| `--changelog "texte"` | Note de changement pour le Steam Workshop |
| `--workshop-id ID` | ID Steam Workshop (ou définir dans `.env`) |
| `--skip-build` | Ignorer l'étape de build (utiliser les PBO existants) |
| `--skip-validate` | Ignorer l'étape de validate des scripts |
| `--skip-sign` | Ignorer l'étape de signature des PBO |
| `--skip-publish` | Compiler et signer uniquement, sans upload |
| `--dry-run` | Prévisualiser ce qui serait fait sans exécuter |

### Exemples

```batch
# Publication complète avec changelog
publish.bat --version 1.0.0 --changelog "Corrections de bugs et nouvelles fonctionnalités"

# Compiler et signer uniquement (sans upload)
publish.bat --version 1.0.0 --skip-publish

# Ignorer la validate (non recommandé)
publish.bat --version 1.0.0 --skip-validate

# Prévisualiser ce qui se passerait
publish.bat --version 1.0.0 --dry-run

# Utiliser le build existant, juste signer et publier
publish.bat --version 1.0.0 --skip-build
```

### Première Configuration du Workshop

Pour la première publication, vous devez créer l'élément Workshop manuellement :

1. Allez sur le [Steam Workshop](https://steamcommunity.com/app/221100/workshop/) de DayZ
2. Cliquez sur **Create** pour créer un nouvel élément Workshop
3. Remplissez le titre, la description, la visibilité et uploadez votre dossier mod une première fois
4. Copiez l'**ID Workshop** depuis l'URL (ex: `https://steamcommunity.com/sharedfiles/filedetails/?id=1234567890`)
5. Ajoutez à votre fichier `.env` :
   ```ini
   WORKSHOP_ID=1234567890
   ```
6. Les mises à jour suivantes peuvent utiliser `publish.bat` automatiquement

### Après la Publication

Votre dossier de mod contiendra :
```
dist/@Swarm/
├── Addons/
│   ├── SwarmTweaks.pbo
│   ├── SwarmTweaks.pbo.Swarm.bisign
│   ├── SwarmAnimals.pbo
│   ├── SwarmAnimals.pbo.Swarm.bisign
│   ├── SwarmEarplugs.pbo
│   └── SwarmEarplugs.pbo.Swarm.bisign
├── Keys/
│   └── Swarm.bikey
├── meta.cpp
└── mod.cpp
```

### Dépannage de la Publication

| Problème | Solution |
|----------|----------|
| Échec de la première publication | Créez d'abord l'élément Workshop manuellement sur Steam, puis ajoutez WORKSHOP_ID au `.env` |
| Demande Steam Guard | Exécutez `scripts\steamcmd.exe +login votre_username +quit` pour mettre en cache les identifiants |
| Erreur d'authentification | Relancez SteamCMD manuellement pour mettre à jour les identifiants en cache |
| SteamCMD introuvable | Vérifiez que `scripts/steamcmd.exe` existe |
| Mauvais mot de passe | SteamCMD utilise les identifiants en cache - exécutez la connexion manuelle pour mettre à jour |

## Installation Serveur

### Pour les Propriétaires de Serveur

1. Abonnez-vous au mod sur le Steam Workshop, ou téléchargez la release
2. Copiez le dossier `@Swarm` dans le répertoire de mods de votre serveur
3. Ajoutez `@Swarm` au paramètre `-mod=` de votre serveur
4. Copiez `Swarm.bikey` depuis `@Swarm/Keys/` vers le dossier `keys/` de votre serveur

### Exemple de Lancement Serveur

```
DayZServer_x64.exe -config=serverDZ.cfg -mod=@Swarm
```

## Développement

### Ajouter une Nouvelle Fonctionnalité

1. Créez/modifiez les fichiers dans le sous-dossier `src/` approprié
2. Mettez à jour `config.cpp` si vous ajoutez de nouvelles classes
3. Exécutez `validate.bat` pour vérifier que les scripts compilent
4. Testez localement avec DayZ
5. Soumettez une pull request

### Types de Fichiers

| Extension | Usage |
|-----------|-------|
| `.cpp` | Fichiers de configuration (CfgPatches, CfgMods, etc.) |
| `.c` | Code EnforceScript |
| `.layout` | Définitions de layout UI |
| `.xml` | Bindings d'entrée et données |
| `.edds` | Textures compressées |

## CI/CD

Ce projet utilise GitHub Actions pour l'intégration continue :

| Workflow | Déclencheur | Usage |
|----------|-------------|-------|
| `build.yml` | Push/PR vers main | Valider les fichiers sources |
| `pr-check.yml` | Pull requests | Vérifications de qualité du code |
| `release.yml` | Tags de version | Créer des releases GitHub |
| `publish.yml` | Manuel | Compiler, signer et publier sur le Workshop |

### Créer une Release

```bash
git tag v1.0.0
git push origin v1.0.0
```

Voir [.github/CICD.md](.github/CICD.md) pour la documentation détaillée CI/CD.

## Notes de Sécurité

- **Ne jamais commit** les fichiers `.biprivatekey` (déjà dans `.gitignore`)
- **Ne jamais partager** votre clé de signature privée
- Le `.bikey` (clé publique) peut être distribué en toute sécurité
- Les propriétaires de serveur ont besoin du `.bikey` pour vérifier les signatures de votre mod
- Gardez votre fichier `.env` privé (il est gitignored)

## Crédits

- **Auteur** : Kize & Arky
- **DayZ Tools** : Bohemia Interactive
