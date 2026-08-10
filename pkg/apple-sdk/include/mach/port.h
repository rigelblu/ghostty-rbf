#ifndef GHOSTTY_APPLE_SDK_MACH_PORT_H
#define GHOSTTY_APPLE_SDK_MACH_PORT_H

// cmux-rbf: works around an Xcode 26.0 SDK header that Zig 0.16 cannot parse.
//
// MacOSX26.0.sdk's <mach/port.h> defines MPG_PAYLOAD three times as
// `__attribute__((overloadable))` inline functions — a Clang C extension for
// function overloading. Zig 0.16 moved translate-c out of the compiler into a
// pinned external package, and that package ignores the attribute, so it sees
// three definitions of one name and fails the whole build with:
//
//   error: redefinition of 'MPG_PAYLOAD'
//
// It takes down `translate-c macos_c.h`, which every target needs, so neither
// macOS nor iOS can build. Zig 0.15.2's in-compiler translate-c handled it,
// which is why this only appeared when the 2026-08 upstream sync moved ghostty
// to a tree requiring 0.16.
//
// Apple removed the overloadable form by SDK 26.5 (this Mac's CommandLineTools
// 26.6 ships a clean copy; Xcode 26.0 does not). So this file is temporary:
// DELETE IT once Xcode ships an SDK >= 26.5. Verify with
//
//   grep -c overloadable "$(xcrun --show-sdk-path)/usr/include/mach/port.h"
//
// A zero there means this shim is dead weight.
//
// Approaches that do NOT work, so nobody re-tries them: SDKROOT is ignored
// (ghostty resolves the SDK through Zig's own LibCInstallation.findNative);
// DEVELOPER_DIR pointed at CommandLineTools gets the clean header but then
// fails with DarwinSdkNotFound, since CLT has no Platforms tree; and bumping
// the translate_c dependency to ghostty-org's newer pin changes nothing.
//
// Give each definition its own name so they stop colliding. Nothing in ghostty
// or cmux calls MPG_PAYLOAD — it is an Apple helper for mach guard-port
// payloads — so renaming it costs nothing, and the macro is undefined again
// below to keep the rename from leaking past this header.
#define GHOSTTY_MPG_PAYLOAD_CAT_(a, b) a##b
#define GHOSTTY_MPG_PAYLOAD_CAT(a, b) GHOSTTY_MPG_PAYLOAD_CAT_(a, b)
#define MPG_PAYLOAD(...) \
    GHOSTTY_MPG_PAYLOAD_CAT(ghostty_unused_mpg_payload_, __COUNTER__)(__VA_ARGS__)

// Resume the header search past this compatibility directory so we get the
// real SDK header. Same Clang extension the math.h wrapper here relies on.
#include_next <mach/port.h>

#undef MPG_PAYLOAD
#undef GHOSTTY_MPG_PAYLOAD_CAT
#undef GHOSTTY_MPG_PAYLOAD_CAT_

#endif
