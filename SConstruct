import os

clang = 1
everything = 1
do_vga = False
do_sdl = True
do_ncurses = True

if everything:
	if clang:
		warn = ["-Weverything"]
	else:
		warn = ["-Wall", "-Wextra", "-Wno-error=attributes"]
else:
	warn = ["-Wall"]

warn += ["-Werror"]

orgenv = Environment(
	CC="clang" if clang else "colorgcc", CFLAGS=warn+Split('-pg -g -ansi -pedantic -std=c11'), CXX="clang++" if clang else "colorgcc", CXXFLAGS=warn+Split('-pg -g -Wno-sign-conversion -ansi -pedantic -std=c++11'), LIBS=["m"], 
	LINK="clang++" if clang else "g++", 
	#CXXFLAGS="-nodefaultlibs -fno-exceptions -w", 
	CPPDEFINES = {"VERSION":"\\\"0.2.4b\\\"","REV_DATE":"\\\"11/21/99\\\"","JSTICK_INSTALLED":"1"},
	CPPPATH=(["gdev"] if do_vga else []) + ["src"]
)
orgenv['ENV']['TERM'] = os.environ['TERM']
#orgenv.Append(LINKFLAGS=Split("-pg -g -Wl,--gc-sections,--print-gc-sections"))
if clang:
	orgenv.Append(CXXFLAGS=["-stdlib=libc++", "-ferror-limit=5"])
	orgenv.Append(LINKFLAGS="-stdlib=libc++")
	if everything:
		orgenv.Append(CXXFLAGS=["-Wno-float-equal", "-Wno-padded", "-Wno-format-nonliteral", "-Wno-disabled-macro-expansion", "-Wno-c++98-compat-pedantic", "-Wno-exit-time-destructors", "-Wno-global-constructors"])
		orgenv.Append(CFLAGS=  ["-Wno-float-equal", "-Wno-padded", "-Wno-format-nonliteral", "-Wno-disabled-macro-expansion"])

env = orgenv.Clone()

if do_ncurses: env.Append(CPPDEFINES = {"HAVE_LIBNCURSES":"1"})
if do_vga: env.Append(CPPDEFINES = {"HAVE_LIBVGA":"1"})

if do_ncurses: env.ParseConfig('pkg-config --libs --cflags ncurses')

if do_sdl:
	env.ParseConfig('pkg-config --libs --cflags sdl')
	env.Append(CPPDEFINES = {"HAVE_LIBSDL": "1"})

if do_vga: env.Append(LIBS = ["vga", "vgagl"])

libgdev = Library([env.Object("gdev/gdev.C"), env.Object("gdev/gdev-svgalib.C"), env.Object("src/fontdev.C")])

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
