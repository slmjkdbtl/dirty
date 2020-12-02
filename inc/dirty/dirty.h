// wengwengweng

// MODULES
//
//   - app     windowing / input
//   - gfx     anything visual
//   - audio   anything audio
//   - math    math types / calcs
//   - fs      file system helpers
//
//   each .h file contains doc for specific modules
//
// EXAMPLE
//
//   here's a basic setup:
//
//     #include <dirty/dirty.h>
//
//     void frame() {
//         d_draw_text("oh hi mark", 24.0, 0.0, D_CENTER, WHITE);
//     }
//
//     int main() {
//         d_run((d_desc) {
//             .frame = frame,
//         });
//     }
//
//   check out 'demo/*.c' for more specific examples
//
// GENERAL NAMING
//
//   d_make_*(): create object with raw data
//   d_parse_*(): create object by parsing file content
//   d_load_*(): create object from file
//   d_free_*(): free object memory
//   d_*_ex(): do something with extra config
//   d_*_a(): the resource returned must be freed
//
//   function / type names are mostly self explanatory

#ifndef DIRTY_H
#define DIRTY_H

#include "platform.h"
#include "math.h"
#include "gl.h"
#include "gfx.h"
#include "app.h"
#include "audio.h"
#include "fs.h"
#include "utils.h"
#include "lua.h"

#endif

