// wengwengweng

// MODULES
//
//   - app     windowing / input
//   - gfx     anything visual
//   - audio   anything audio
//   - math    math types / algorithms
//   - fs      file system helpers
//   - ui      immediate mode UI for dev
//
// USAGE
//
//   1. have SDL2 available for linking
//   2. build and copy 'libdirty.a' to your link dir
//   3. copy 'inc/dirty' to your include dir and
//
//     #include <dirty/dirty.h>
//
// EXAMPLE
//
//   here's a bsic setup:
//
//     int main() {
//
//         d_init("hi", 640, 480);
//
//         while (d_running()) {
//
//             d_clear();
//             // your main loop
//             d_frame();
//
//         }
//
//     }
//
//   check out 'demo/*.c' for more specific examples
//
// NAMING SCHEME
//
//   d_make_*(): create object with raw data
//   d_parse_*(): create object by parsing file content
//   d_load_*(): create object from file
//   d_free_*(): free object memory
//   d_*_ex(): do something with extra config
//   d_*_m(): the resource returned must be freed
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
#include "ui.h"
#include "utils.h"

#endif

