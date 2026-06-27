# Linux Prebuilt Release Notes (`obs-browser`)

This note exists to keep Linux release packaging explicit and narrow in scope.

## What gets published

This repository should publish its own prebuilt archive separately from
`obs-streamelements-core`.

Recommended artifact shape:
- `obs-browser-<version>-<distro-tag>-<obs-version>.tar.gz`

The archive should contain:
- `obs-browser/bin/64bit/obs-browser.so`
- `obs-browser/bin/64bit/obs-browser-page` when present
- `obs-browser/data/obs-plugins/obs-browser/...`

## Why this is a separate release

This repo owns the browser plugin/runtime layer.

That means the release message should stay repo-specific:
- this repo ships the custom browser plugin needed by the Linux port flow
- `obs-streamelements-core` ships the StreamElements plugin layer
- users should install matching releases from both repos when using the full
  custom StreamElements Linux stack

## Compatibility guardrails

Do not describe Linux prebuilt artifacts as universal.

Each archive is only meaningful when these stay aligned:
- distro/runtime family
- OBS version family
- architecture
- Qt/CEF stack expectations
- whether the target machine already has another active `obs-browser` variant

Current practical stance:
- prefer one validated target at a time, for example Fedora 43 with OBS 32.1.x
- expand only after real validation on another target

## Suggested release message

```text
Linux prebuilt release for obs-browser

Target:
- distro/runtime: <distro-tag>
- OBS: <obs-version>
- architecture: x86_64

This artifact contains only the custom obs-browser plugin for this Linux port
flow.

Important:
- This is not a universal Linux build.
- Use it only on the validated distro/runtime and OBS family listed above.
- Keep only one active obs-browser variant in OBS at a time.
- If you are using the StreamElements Linux prototype, install the matching
  obs-streamelements-core release as well.

Install:
1. Close OBS.
2. Extract the archive.
3. Copy `obs-browser` into `~/.config/obs-studio/plugins/`.
4. Install the matching obs-streamelements-core release if needed.
5. Start OBS and validate browser source/panel behavior.
```

## Packaging command

Example:

```bash
./_scripts/package-linux-release.sh \
  --distro-tag fedora43 \
  --obs-version obs32.1.2
```
