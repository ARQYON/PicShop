<!-- Version History -->

All notable changes to **ARQYON PicShop** will be documented in this file.

This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html)
and [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) format.

> **Format:** `[Version] -YYYY-MM-DD`
> **Types:** `Added` `Changed` `Fixed` `Removed` `Security` `Deprecated`

---

## [Unreleased]

> Features currently in development for the next release.

### Added
- Project repository initialized
- Master CMake build system configured
- Proprietary license established
- Core folder structure defined

---

## [1.0.0] -2026-XX-XX -🎉 Initial Release (Free)

> First public release of ARQYON PicShop.
> Free for all users. No activation required.
> Windows only.

### Added

#### Core Engine (C++)
- Image loading support -PNG, JPG, JPEG, BMP, TIFF
- Image saving support -PNG, JPG, JPEG
- Canvas rendering pipeline using Qt6 OpenGL
- Undo / Redo system (up to 50 steps)
- Zoom in / Zoom out (10% -3200%)
- Pan / scroll canvas navigation

#### Layer System
- Create, delete, duplicate layers
- Reorder layers (drag and drop)
- Layer visibility toggle (show/hide)
- Layer opacity control (0% -100%)
- Layer rename
- Merge layers
- Flatten all layers to single image

#### Tools
- **Brush Tool** -adjustable size, hardness, opacity
- **Eraser Tool** -adjustable size and opacity
- **Crop Tool** -freehand and fixed ratio crop
- **Resize Tool** -resize canvas or image with aspect ratio lock
- **Selection Tool** -rectangular and elliptical selection
- **Text Tool** -add text with font, size, color options
- **Color Picker Tool** -pick any color from canvas
- **Fill Tool** (Paint Bucket) -fill area with selected color
- **Move Tool** -move layers and selections

#### Filters
- Brightness / Contrast adjustment
- Hue / Saturation / Lightness adjustment
- Grayscale conversion
- Invert colors
- Blur (Gaussian)
- Sharpen
- Noise reduction (basic)

#### User Interface (Qt6)
- Dark theme UI (default)
- Light theme UI (optional)
- Toolbar panel (left side)
- Layers panel (right side)
- Properties panel (right side)
- Color palette panel
- Menu bar (File, Edit, Image, Layer, Filter, View, Help)
- Keyboard shortcuts for all major tools
- Status bar (zoom level, canvas size, cursor position)

#### File System
- Open image files via file dialog
- Save as PNG / JPG / JPEG
- Export flattened image
- Save project as `.svp` (ARQYON Project format)
- Open `.svp` project files (restores all layers)
- Recent files list (last 10 files)

#### Installer
- Windows installer (`SVPicShop_Setup_v1.0.0.exe`)
- Desktop shortcut creation
- Start menu entry
- Uninstaller included
- No activation or internet required for V1

### Technical
- Built with C++17 and Qt6
- CMake build system
- Rust-powered memory-safe plugin sandbox
- Windows 10 / 11 support (64-bit)

---

## [0.9.0] -2026-XX-XX -🔧 Beta Release

> Internal beta -not publicly released.
> Testing and bug fixing phase.

### Added
- Beta build of all V1.0.0 features
- Internal QA testing suite
- Performance benchmarks

### Fixed
- Canvas rendering flicker on resize
- Layer opacity not saving correctly in .svp format
- Text tool font loading on fresh Windows install
- Undo system memory leak in large projects

### Changed
- Improved brush stroke smoothness algorithm
- Reduced installer size by 30%

---

## [0.5.0] -2026-XX-XX -🏗️ Alpha Release

> Internal alpha -core engine and basic UI only.

### Added
- Basic image load and save (PNG/JPG)
- Canvas rendering (no layers yet)
- Brush and eraser tools (basic)
- Crop and resize tools
- CMake build system complete
- Qt6 main window and menu bar

---

## [0.1.0] -2026-08-29 -🚀 Project Initialized

> ARQYON PicShop project created.
> Repository structure established.

### Added
- Repository initialized
- `.gitignore` configured for C++, Rust, Python, Qt
- `.gitattributes` configured for cross-platform line endings
- `CMakeLists.txt` -master build system
- `CMakePresets.json` -Debug / Release presets
- `LICENSE` -ARQYON Proprietary License V1.0
- `CHANGELOG.md` -this file
- `CONTRIBUTING.md` -contribution guidelines
- `SECURITY.md` -security policy
- `README.md` -project overview

---

## Versioning Guide

```
MAJOR.MINOR.PATCH

MAJOR -Breaking changes or complete rewrites
MINOR -New features added (backwards compatible)
PATCH -Bug fixes and small improvements

Examples:
1.0.0 -First stable free release
1.1.0 -New tool or feature added
1.0.1 -Bug fix patch
2.0.0 -Paid AI features introduced
```

---

## Upcoming Roadmap

| Version | Target Date | Highlights |
|---------|------------|------------|
| 1.0.0 | 2026 | Free release - core tools + layers |
| 1.1.0 | 2027 | PSD import/export support |
| 1.2.0 | 2027 | Plugin system (community plugins) |
| 2.0.0 | 2027 | AI features - paid version launch |
| 2.1.0 | 2027 | Background removal (AI) |
| 2.2.0 | 2027 | AI filters and style transfer |
| 3.0.0 | 2028 | Linux support |

---

## Links

- **Website:** https://ARQYON.co
- **Support:** https://ARQYON.co/support
- **Purchase:** https://ARQYON.co/picshop
- **Author:** Adil Hassan -ARQYON

---

*Copyright (c) 2026 ARQYON -Adil Hassan. All Rights Reserved.*