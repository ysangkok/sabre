import os

clang = 1
everything = 1
do_vga = 0
do_sdl = 1
do_ncurses = 1
fast = 0
memdebug = 0
coverage = 0

warn = []
machine = ["-m32"]
#machine = []

if everything:
	if clang:
		warn += machine + ["-Weverything"]
	else:
		warn += machine + ["-Wall", "-Wextra"]
else:
	warn += machine + ["-Wall"]

if not clang:
	warn += ["-Wno-attributes", "-Wno-unused-local-typedefs"]

warn += ["-Werror"]

if fast:
	warn = machine + ["-w"]

# AddressSanitizer
if memdebug:
	debug_profile_and_coverage = Split("-fsanitize=undefined -fsanitize=memory -fno-omit-frame-pointer -fsanitize-memory-track-origins")
else: # prevent relocation R_X86_64_32S against `__libc_csu_fini' can not be used when making a shared object
# Profile
	debug_profile_and_coverage = Split("-pg")

# Stack protection
debug_profile_and_coverage += Split("-fstack-protector-all")

# Debug
debug_profile_and_coverage += Split("-ggdb3")

# Coverage
if coverage:
	debug_profile_and_coverage += Split("-ftest-coverage -fprofile-arcs")

debug_profile_and_coverage += Split("-fPIC")

if fast:
	debug_profile_and_coverage = []

orgenv = Environment(
	CC="clang" if clang else "gcc", CFLAGS=warn + debug_profile_and_coverage + ([] if fast else Split('-ansi -pedantic -std=c11')), CXX="clang++" if clang else "gcc", CXXFLAGS=warn + debug_profile_and_coverage + ["-std=c++11"] + ([] if fast else Split('-Wno-sign-conversion -ansi -pedantic')), LIBS=["m"], 
	LINK="clang++" if clang else "g++", 
	#CXXFLAGS="-nodefaultlibs -fno-exceptions -w", 
	CPPDEFINES = {"VERSION":"\\\"0.2.4b\\\"","REV_DATE":"\\\"11/21/99\\\"","JSTICK_INSTALLED":"1"},
	CPPPATH=(["gdev"] if do_vga else []) + ["src"]
)

orgenv['ENV']['TERM'] = os.environ['TERM']

orgenv.Append(LINKFLAGS=machine)

if not fast:
	orgenv.Append(LINKFLAGS=debug_profile_and_coverage + Split("-Wl,--gc-sections")) #,--print-gc-sections

if clang:
	orgenv.Append(CXXFLAGS=["-stdlib=libc++"] + (["-ferror-limit=5"] if not fast else []))
	orgenv.Append(LINKFLAGS="-stdlib=libc++")
	if not fast and everything:
		common_flags = ["-Wno-float-equal", "-Wno-padded", "-Wno-format-nonliteral", "-Wno-disabled-macro-expansion"]
		orgenv.Append(CXXFLAGS=common_flags + ["-Wno-c++11-compat", "-Wno-c++11-extensions", "-Wno-c++98-compat-pedantic", "-Wno-exit-time-destructors", "-Wno-global-constructors"])
		orgenv.Append(CFLAGS=common_flags)

env = orgenv.Clone()

if do_ncurses: env.Append(CPPDEFINES = {"HAVE_LIBNCURSES":"1"})
if do_vga: env.Append(CPPDEFINES = {"HAVE_LIBVGA":"1"})

if do_ncurses: env.ParseConfig('pkg-config --libs --cflags ncurses')

if do_sdl:
	env.ParseConfig('PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig/ pkg-config --libs --cflags sdl2')
	env.Append(CPPDEFINES = {"HAVE_LIBSDL": "1"})

if do_vga: env.Append(LIBS = ["vga", "vgagl"])

gdev_objs = [env.Object("gdev/gdev.C"),  env.Object("src/fontdev.C")]
if do_vga:
	gdev_objs += [env.Object("gdev/gdev-svgalib.C")]

libgdev = Library(gdev_objs)

objects = env.Object(Glob("src/*.C")) + env.Object(Glob("src/*.c")) + env.Object(Glob("libzip/*.c"))
if do_vga: objects += libgdev

env.Program("src/sabre", objects)

joyenv = orgenv.Clone()

joyenv.Append(CPPDEFINES = ["JOYCAL"])

joyenv.Program(joyenv.Object("tools/calibrate.C"))

gdevenv = orgenv.Clone()
gdevenv.Append(LIBS = ["vga", "vgagl"])
gdevenv.Append(CPPDEFINES = {"HAVE_LIBVGA":"1"})

if do_vga:
	gdevenv.Program("tools/fontedit", [gdevenv.Object("tools/fontedit.C"), gdevenv.Object("tools/fontutils.C"), libgdev])
	gdevenv.Program("tools/hello", [gdevenv.Object("tools/hello.C"), gdevenv.Object("tools/fontutils.C"), libgdev])

orgenv.Program(orgenv.Object("tools/mkterrain.c"))
orgenv = orgenv.Clone()
orgenv.Append(LIBS="m")
orgenv.Program(orgenv.Object("tools/rmptextr3.c"))
orgenv.Program(orgenv.Object("tools/mktextr3.c"))
orgenv.Program(orgenv.Object("tools/mktextr.c"))
