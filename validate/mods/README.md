# Validate Mods

Place dependency mods in this folder for script validate.

## Usage

If your mod depends on other mods (e.g., CF, Community Framework, etc.), copy or symlink them here:

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

The `validate.bat` script will automatically detect and load all `@*` folders as mods.

## Tips

- **Symlinks**: Use symbolic links to avoid duplicating large mod folders:
  ```batch
  mklink /D "@CF" "C:\Path\To\Steam\workshop\content\221100\@CF"
  ```

- **Workshop mods**: You can link directly to your Steam Workshop mod folders.

- **Load order**: Mods are loaded alphabetically. If load order matters, rename folders with numeric prefixes (e.g., `@1_CF`, `@2_COT`).

## Note

This folder is gitignored (except this README) - dependency mods won't be committed to the repository.
