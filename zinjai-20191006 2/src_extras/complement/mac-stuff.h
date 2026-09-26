#ifndef MAC_STUFF_H
#define MAC_STUFF_H

// NOTA (port ARM64/wx3): la API de Carbon (ProcessSerialNumber, GetCurrentProcess,
// TransformProcessType, SetFrontProcess) fue eliminada de macOS hace años y ya no
// existe en absoluto en Apple Silicon. El workaround original era para binarios
// sueltos sin bundle .app; con wxWidgets 3.x + wxIMPLEMENT_APP dentro de un bundle
// .app real (que es como se debe distribuir en Apple Silicon), este problema de foco
// no ocurre, así que la función queda como no-op.
static void fix_mac_focus_problem() {
}

#endif
