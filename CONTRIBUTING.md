
<!--
=============================================================
  ARQYON PicShop -CONTRIBUTING.md
  Copyright (c) 2026 ARQYON. All Rights Reserved.
  Author: Adil Hassan | ARQYON.co
  Future Team Guide
=============================================================
-->

<div align="center">

**ARQYON PicShop** is a proprietary software product owned by **ARQYON**.
This document outlines contribution guidelines for future team members,
collaborators, and contractors working under direct permission from ARQYON.

**Website:** [ARQYON.co](https://ARQYON.co) <br>
**Author:** Adil Hassan <br>
**Email:** dev@ARQYON.co<br>

</div>

---

## ⚠️ Important Notice

ARQYON PicShop is **NOT open source.**
Contributions are only accepted from:

- Adil Hassan (Owner & Lead Developer)
- Team members hired directly by ARQYON
- Contractors working under a signed agreement with ARQYON

If you are not one of the above, **do not submit pull requests.**
Unauthorized contributions will be rejected and closed immediately.

---

## Table of Contents

1. [Code of Conduct](#code-of-conduct)
2. [Getting Started](#getting-started)
3. [Development Environment](#development-environment)
4. [Project Structure](#project-structure)
5. [Branching Strategy](#branching-strategy)
6. [Commit Message Format](#commit-message-format)
7. [Coding Standards](#coding-standards)
8. [Testing Requirements](#testing-requirements)
9. [Pull Request Process](#pull-request-process)
10. [Reporting Bugs](#reporting-bugs)
11. [Contact](#contact)

---

## Code of Conduct

All contributors must:

- Treat all team members with respect and professionalism
- Keep all source code, business logic, and company information strictly confidential
- Never share, leak, or discuss proprietary code outside the team
- Follow all instructions from the Lead Developer (Adil Hassan)
- Sign a Non-Disclosure Agreement (NDA) before accessing the codebase

Violation of any of the above results in immediate removal from the project
and potential legal action under Pakistan law.

---

## Getting Started

### Prerequisites

Before contributing, ensure you have the following installed:

```bash
# Required
- Windows 10 / 11 (64-bit)
- CMake 3.25+
- Qt6 (6.5 or above)
- GCC / MinGW-w64 or MSVC
- Ninja build system
- Rust (latest stable via rustup)
- Python 3.10+
- Git 2.40+

# Recommended
- Visual Studio Code with CMake Tools extension
- Qt Creator (for UI work)
- RustRover or CLion
```

### Clone the Repository

```bash
# Only authorized contributors have access
git clone https://github.com/ARQYON/SVPicShop.git
git clone https://github.com/adil-12-hassan/SVPicShop.git
cd SVPicShop
```

### Build for Development

```bash
# Configure Debug build
cmake --preset windows-debug

# Build
cmake --build --preset build-debug

# Run tests
ctest --preset test-debug
```

---

## Development Environment

### Recommended VS Code Extensions

```
ms-vscode.cpptools          -C/C++ IntelliSense
ms-vscode.cmake-tools       -CMake integration
rust-lang.rust-analyzer     -Rust IntelliSense
ms-python.python            -Python support
streetsidesoftware.code-spell-checker -Spell check
```

### Environment Variables

Create a `.env` file in the root (never commit this):

```env
ARQYON_DEV_MODE=true
ARQYON_API_URL=https://api.ARQYON.co
ARQYON_LICENSE_KEY=your-dev-license-key
```

---

## Project Structure

```
ARQYONPicShop/
├── core/           -C++17 image engine (no Qt)
├── ui/             -Qt6 GUI layer
├── rust/           -Rust memory + plugin sandbox
├── ai/             -Python AI features
├── assets/         -Icons, fonts, splash screen
├── docs/           -Technical documentation
├── tests/          -All tests (gtest + cargo + pytest)
├── scripts/        -Build and utility scripts
├── installer/      -Inno Setup Windows installer
└── website/        -MERN company website
```

**Golden Rule:**
- `core/` must NEVER depend on Qt -it is a pure C++ library
- `ui/` depends on `core/` and Qt6
- `ai/` is pure Python -no C++ includes
- `rust/` is a standalone Cargo workspace

---

## Branching Strategy

We follow **Git Flow:**

```
main          -Production ready code only. Protected.
develop       -Active development branch. All PRs merge here.
feature/*     -New features (e.g. feature/brush-tool)
fix/*         -Bug fixes (e.g. fix/layer-opacity-crash)
release/*     -Release preparation (e.g. release/v1.0.0)
hotfix/*      -Emergency fixes to main (e.g. hotfix/crash-on-startup)
```

### Branch Rules

- **Never commit directly to `main`**
- **Never commit directly to `develop`** without a Pull Request
- Branch names must be lowercase with hyphens
- Delete branches after merging

### Example Workflow

```bash
# Start a new feature
git checkout develop
git pull origin develop
git checkout -b feature/text-tool

# Work, commit, push
git add .
git commit -m "feat(ui): add text tool with font selection"
git push origin feature/text-tool

# Open Pull Request → develop
```

---

## Commit Message Format

We use **Conventional Commits** format:

```
type(scope): short description

[optional body]

[optional footer]
```

### Types

| Type | When to Use |
|------|-------------|
| `feat` | New feature added |
| `fix` | Bug fix |
| `docs` | Documentation only |
| `style` | Formatting, no logic change |
| `refactor` | Code restructure, no feature change |
| `test` | Adding or fixing tests |
| `build` | CMake, dependencies, build system |
| `perf` | Performance improvement |
| `ci` | CI/CD pipeline changes |
| `chore` | Maintenance tasks |

### Scopes

| Scope | Meaning |
|-------|---------|
| `core` | C++ image engine |
| `ui` | Qt6 GUI layer |
| `rust` | Rust system layer |
| `ai` | Python AI features |
| `installer` | Windows installer |
| `docs` | Documentation |
| `tests` | Test suite |

### Examples

```bash
feat(core): add Gaussian blur filter to image processor
fix(ui): resolve layer panel drag-and-drop crash
docs(readme): update installation instructions
build(cmake): upgrade Qt6 minimum version to 6.5
test(core): add unit tests for undo/redo system
refactor(rust): simplify plugin sandbox memory manager
perf(core): optimize pixel buffer rendering by 40%
```

---

## Coding Standards

### C++ (core/ and ui/)

```cpp
// File header -required on every file
// =============================================================
//   ARQYON PicShop -FileName.cpp
//   Copyright (c) 2026 ARQYON. All Rights Reserved.
//   Author: Adil Hassan | ARQYON.co
// =============================================================

// Naming conventions
class ImageProcessor {};          // PascalCase for classes
void applyGaussianBlur();         // camelCase for functions
int pixelWidth = 0;               // camelCase for variables
const int MAX_LAYERS = 100;       // UPPER_SNAKE for constants
int m_layerCount = 0;             // m_ prefix for member variables

// Always use nullptr, not NULL
int* ptr = nullptr;

// Always use smart pointers
std::unique_ptr<Layer> layer = std::make_unique<Layer>();
std::shared_ptr<Canvas> canvas = std::make_shared<Canvas>();

// Use const wherever possible
void renderLayer(const Layer& layer) const;

// No magic numbers -use named constants
const int UNDO_STACK_LIMIT = 50;  // Good
canvas.resize(1920, 1080);        // Bad -use named variables
```

### Rust (rust/)

```rust
// Follow standard Rust naming
struct PluginSandbox {}           // PascalCase structs
fn load_plugin() {}              // snake_case functions
const MAX_PLUGINS: u32 = 64;     // UPPER_SNAKE constants

// Always handle Results -never unwrap() in production
match plugin.load() {
    Ok(p) => p.run(),
    Err(e) => log::error!("Plugin load failed: {}", e),
}
```

### Python (ai/)

```python
# Follow PEP 8
class BackgroundRemover:          # PascalCase classes
    def remove_background(self):  # snake_case methods
        pass
MAX_IMAGE_SIZE = 4096             # UPPER_SNAKE constants
# Always use type hints
def process_image(image: np.ndarray) -> np.ndarray:
    pass
# Always use docstrings
def remove_background(image: np.ndarray) -> np.ndarray:
    """
    Remove background from image using AI model.
    Args:
        image: Input image as numpy array (H, W, C)
    Returns:
        Image with transparent background (H, W, 4)
    """
    pass
```

---

## Testing Requirements

**All new code must include tests. No exceptions.**

### C++ Tests (Google Test)

```bash
# Run C++ tests
cmake --preset windows-debug
cmake --build --preset build-debug
ctest --preset test-debug
```

- Every public function in `core/` needs at least one test
- Test files go in `tests/cpp/`
- Test file naming: `test_<module>.cpp`

### Rust Tests (Cargo Test)

```bash
cd rust/
cargo test
```

- Use built-in Rust `#[test]` attribute
- Test every public function and error path

### Python Tests (pytest)

```bash
cd ai/
pytest tests/ -v
```

- Test files go in `ai/tests/`
- File naming: `test_<module>.py`
- Minimum 80% code coverage required

---

## Pull Request Process

1. **Create branch** from `develop`
2. **Write code** following coding standards above
3. **Write tests** for your changes
4. **Run all tests** -must pass 100%
5. **Update CHANGELOG.md** under `[Unreleased]`
6. **Open Pull Request** to `develop`
7. **Fill PR template** completely
8. **Wait for review** from Adil Hassan
9. **Address review comments**
10. **Merge** after approval

### PR Title Format

```
feat(scope): short description of what was added
fix(scope): short description of what was fixed
```

### PR Checklist

```
[ ] Code follows style guidelines
[ ] All tests pass locally
[ ] New tests written for new code
[ ] CHANGELOG.md updated
[ ] No secrets or .env files committed
[ ] Branch is up to date with develop
```

---

## Reporting Bugs

If you find a bug while working on the codebase:

1. Check if it already exists in GitHub Issues
2. If not, create a new issue with this format:

```
Title: [BUG] Short description of the bug

## Description
What happened vs what was expected.

## Steps to Reproduce
1. Open PicShop
2. Load image
3. Apply blur filter
4. Crash occurs

## Environment
- OS: Windows 11 64-bit
- PicShop Version: 1.0.0
- Build Type: Debug / Release

## Screenshots / Logs
Attach relevant screenshots or crash logs.
```

---

## Contact

| Purpose | Contact |
|---------|---------|
| Development questions | dev@ARQYON.co |
| Legal / NDA | legal@ARQYON.co |
| General | hello@ARQYON.co |
| Website | https://ARQYON.co |

---

*Copyright (c) 2026 ARQYON -Adil Hassan. All Rights Reserved.*
*This document is confidential and intended for authorized contributors only.*