<!-- Vulnerability Reporting --> 
<!--
=============================================================
  ARQYON PicShop -SECURITY.md
  Copyright (c) 2026 ARQYON. All Rights Reserved.
  Author: Adil Hassan | ARQYON.co
=============================================================
-->

<div align="center">

**ARQYON takes the security of its software seriously.**
If you believe you have found a security vulnerability in
**ARQYON PicShop**, please report it responsibly.

**Website:** [ARQYON.co](https://ARQYON.co) <br>
**Security Email:** security@ARQYON.co <br>
**Author:** Adil Hassan <br>

</div>

---

## Table of Contents

1. [Supported Versions](#supported-versions)
2. [Reporting a Vulnerability](#reporting-a-vulnerability)
3. [What to Include in Your Report](#what-to-include)
4. [Our Response Process](#our-response-process)
5. [Security Best Practices for Users](#security-best-practices)
6. [Known Security Considerations](#known-security-considerations)
7. [Hall of Fame](#hall-of-fame)
8. [Contact](#contact)

---

## Supported Versions

Only the latest stable version of ARQYON PicShop
receives security updates.

| Version | Supported |
|---------|-----------|
| 1.x.x (Latest) | ✅ Actively supported |
| 0.x.x (Beta/Alpha) | ❌ Not supported |

> Always use the latest version downloaded from
> the official website: **https://ARQYON.co**
>
> Never download ARQYON PicShop from third-party
> websites, torrents, or unofficial sources.
> ARQYON is not responsible for modified or
> malicious versions distributed elsewhere.

---

## Reporting a Vulnerability

### ⚠️ Please DO NOT report security vulnerabilities publicly.

Do NOT open a GitHub Issue for security vulnerabilities.
Do NOT post about vulnerabilities on social media.
Do NOT disclose vulnerabilities to third parties before
we have had a chance to address them.

### ✅ How to Report Correctly

**Email us directly and privately:**

```
security@ARQYON.co
```

**Subject line format:**
```
[SECURITY] Brief description of vulnerability
```

**Example:**
```
[SECURITY] Buffer overflow in image file parser
[SECURITY] License activation bypass vulnerability
[SECURITY] Remote code execution via malformed .svp file
```

We will acknowledge your report within **48 hours**
and provide a detailed response within **7 days.**

---

## What to Include in Your Report

Please include as much detail as possible:

```
1. VULNERABILITY TYPE
   - Buffer overflow
   - Remote code execution
   - License bypass
   - Memory corruption
   - Privilege escalation
   - Path traversal
   - Other

2. AFFECTED COMPONENT
   - Core image engine (C++)
   - Qt6 UI layer
   - Rust plugin sandbox
   - Python AI module
   - License activation system
   - Installer
   - Other

3. AFFECTED VERSION
   - ARQYON PicShop version number
   - Windows version (10/11)
   - 32-bit or 64-bit

4. STEPS TO REPRODUCE
   - Clear step-by-step instructions
   - Minimum steps needed to trigger the issue

5. PROOF OF CONCEPT
   - Code, script, or file that demonstrates the issue
   - Screenshots or video if helpful

6. IMPACT
   - What can an attacker do with this vulnerability?
   - How severe do you consider it? (Low/Medium/High/Critical)

7. YOUR CONTACT INFO (optional)
   - Name or alias
   - Email for follow-up
```

---

## Our Response Process

After receiving your report we will:

| Step | Timeframe | Action |
|------|-----------|--------|
| Acknowledgement | Within 48 hours | Confirm we received your report |
| Initial Assessment | Within 7 days | Evaluate severity and validity |
| Status Update | Within 14 days | Share our findings and fix timeline |
| Fix Development | Depends on severity | Develop and test the patch |
| Security Release | ASAP for Critical | Release patched version |
| Public Disclosure | After fix is live | Acknowledge in CHANGELOG.md |

### Severity Levels

| Level | Description | Response Time |
|-------|-------------|---------------|
| 🔴 Critical | Remote code execution, data theft | 24-48 hours |
| 🟠 High | Privilege escalation, license bypass | 7 days |
| 🟡 Medium | Crash, minor data exposure | 14 days |
| 🟢 Low | Minor issues, hardening improvements | 30 days |

---

## Security Best Practices for Users

To keep ARQYON PicShop secure on your system:

### ✅ Do This
```
✅ Always download from official website only
   https://ARQYON.co/download
✅ Keep ARQYON PicShop updated to latest version
✅ Verify the installer checksum before installing 
   (SHA256 hash published on ARQYON.co/download)
✅ Only open image files from trusted sources
✅ Keep your Windows OS updated
✅ Use Windows Defender or reputable antivirus
✅ Report suspicious behavior immediately
   security@ARQYON.co
```

### ❌ Never Do This

```
❌ Never download from third-party sites or torrents
❌ Never run cracked or patched versions of PicShop
   (these may contain malware)
❌ Never open .svp project files received from
   unknown or untrusted sources
❌ Never disable Windows security features to
   run PicShop -it should never require that
❌ Never share your license key with anyone
```

---

## Known Security Considerations

### License Activation System

- The Software communicates with `api.ARQYON.co`
  during license activation (V2+ paid features only)
- V1 (free version) requires NO internet connection
- All API communication uses HTTPS (TLS 1.2+)
- We never transmit your image files or project data

### File Parsing

- ARQYON PicShop parses image files (PNG, JPG, etc.)
  and `.svp` project files
- We use industry-standard parsing libraries
- Malformed files are rejected with an error message
- Do not open files from untrusted sources

### Plugin System (V1.2+ Planned)

- Plugins run inside a Rust-powered sandbox
- Plugins cannot access your file system outside
  of designated folders
- Only install plugins from the official
  ARQYON plugin marketplace

### Python AI Module (V2+ Planned)

- AI features run locally on your machine
- No image data is sent to external servers
- AI models are downloaded from ARQYON.co only

---

## Hall of Fame

We sincerely thank the following security researchers
who responsibly disclosed vulnerabilities to us:

> *No vulnerabilities have been reported yet.*
> *Be the first -your name could be here!*

| Researcher | Vulnerability | Version Fixed | Severity |
|------------|--------------|---------------|----------|
| -| -| -| -|

---

## Responsible Disclosure Policy

ARQYON follows responsible disclosure principles:

1. **We will not take legal action** against researchers
   who follow this policy and report in good faith.
2. **We will credit you** in our Hall of Fame and
   CHANGELOG.md if you wish to be acknowledged.
3. **We ask that you:**
   - Give us reasonable time to fix before disclosing
   - Not exploit the vulnerability beyond what is
     necessary to demonstrate it
   - Not access, modify, or delete user data
   - Not perform denial of service attacks
4. **Out of scope** (please do not report):
   - Issues in third-party libraries we use
     (report those to the library maintainers)
   - Issues requiring physical access to the machine
   - Social engineering attacks
   - Issues in outdated unsupported versions
   
---

## Contact

| Purpose | Contact |
|---------|---------|
| 🔒 Security vulnerabilities | security@ARQYON.co |
| 🛠️ General support | support@ARQYON.co |
| ⚖️ Legal matters | legal@ARQYON.co |
| 🌐 Website | https://ARQYON.co |

**Response time for security issues: 48 hours maximum.**
For general support, see: https://ARQYON.co/support

---

*Copyright (c) 2026 ARQYON -Adil Hassan. All Rights Reserved.*
*Please report security issues responsibly to security@ARQYON.co*