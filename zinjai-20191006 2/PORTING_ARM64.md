# Port de ZinjaI a macOS ARM64 (Apple Silicon)

Este fork parte del código fuente oficial de ZinjaI (snapshot `zinjai-src-20191006`)
para intentar hacerlo compilar en Macs Apple Silicon (M1/M2/M3/M4), donde no existe
build oficial.

## Por qué no compilaba

El código original apuntaba a **wxWidgets 2.8 en modo ANSI** usando el puerto
**Carbon** de macOS:

- Carbon nunca soportó 64 bits y fue eliminado por completo de macOS hace años.
- El modo "ANSI" (no-Unicode) de wxWidgets fue eliminado desde la versión 3.0.
- No existe combinación de flags que compile ese código, tal cual, en Apple Silicon.

## Qué se cambió

1. **`src/Makefile.mac`**, **`src_extras/complement/Makefile.mac`** y
   **`src_extras/img_viewer/Makefile.mac`**: reescritos para usar `wx-config` de
   **wxWidgets 3.x** (Cocoa nativo, Unicode, arm64), con los componentes que usa
   ZinjaI: `std,aui,stc,richtext,html`.
2. **`src_extras/complement/mac-stuff.h`**: se quitó el workaround basado en la API
   de Carbon (`ProcessSerialNumber`, `GetCurrentProcess`, `TransformProcessType`,
   `SetFrontProcess`), que ya no existe en macOS moderno. La función queda como
   no-op; el problema de foco que resolvía no debería reaparecer al distribuir
   como bundle `.app` real.
3. **`.github/workflows/build-macos-arm64.yml`**: compila el proyecto en un runner
   `macos-14` (Apple Silicon) de GitHub Actions en cada push, para probar en
   hardware ARM64 real (nadie del equipo tiene por qué tener un Mac a mano).

## Estado actual

Este es un primer intento razonado, no un build ya verificado — el análisis del
código (374 archivos) no encontró más código específico de Mac fuera de lo ya
mencionado, pero es esperable que aparezcan errores de compilación por cambios de
API entre wxWidgets 2.8 → 3.x (nombres de métodos, headers, etc.) que solo se ven
compilando de verdad. Revisa el artifact `build-log` de la Action para ver el
resultado más reciente y el estado de esos errores.

## Cómo compilar localmente

```bash
brew install wxwidgets
make mac
```

El binario queda en `./zinjai`.
