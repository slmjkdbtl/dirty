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
// NAMING SCHEME
//
//   d_make_*: create object with raw data
//   d_parse_*: create object by parsing file content
//   d_load_*: create object from file
//   d_free_*: free object memory
//   d_*_ex: do something with extra config
//
//   module-specific naming schemes are written at the top of {mod}.h
//   function / type names are mostly self explanatory
//
// USAGE
//
//   1. have SDL2 available for linking
//   2. build and copy 'libdirty.a' to your link dir
//   3. copy 'inc/dirty' to your include dir and
//
//     #include <dirty/dirty.h>
//
//   check out 'demo/*.c' for code examples

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

