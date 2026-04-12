// Verifies co-inclusion compatibility: moth_graphics_fwd.h included before
// moth_graphics.h must not cause redefinition errors or ODR violations. This
// is the common consumer pattern — a header forward-declares via the fwd header,
// then the corresponding .cpp includes the full aggregate header.

#include "moth_graphics/moth_graphics_fwd.h"
#include "moth_graphics/moth_graphics.h"

#include <catch2/catch_all.hpp>

TEST_CASE("moth_graphics_fwd.h and moth_graphics.h are co-inclusion compatible", "[api][headers]") {
    SUCCEED();
}
