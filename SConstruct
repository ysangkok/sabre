import os

clang = 1
everything = 0
do_vga = 0
do_sdl = not do_vga
memdebug = 1
coverage = 0
profile = 0
opt = []

warn = []
#machine = ["-m32"]
machine = []

if everything:
	if clang:
		warn += machine + ["-Weverything"]
		warn += ["-Wno-date-time"]
		#warn += ["-Wno-sign-conversion", "-Wno-old-style-cast", "-Wno-sign-compare"]
	else:
		warn += machine + ["-Wall", "-Wextra"]
		warn += ["-Wno-attributes", "-Wno-unused-local-typedefs"]
else:
	warn += machine + ["-Wall"]

lto = []
link_lto = []

if not memdebug:
	lto += ["-flto"]
if clang and not memdebug:
	link_lto += lto + ["-B/usr/lib/gold-ld"]

if everything:
	warn += ["-Wno-variadic-macros"]
	warn += ["-Werror"]

# AddressSanitizer
if memdebug:
        debug_profile_and_coverage = []
	#debug_profile_and_coverage += Split("-fsanitize=memory -fsanitize-blacklist=blacklist.txt -fno-omit-frame-pointer -fsanitize-memory-track-origins")
	#debug_profile_and_coverage += Split("-fsanitize=undefined")
	link_lto += debug_profile_and_coverage
else:
# Profile
	if profile:
		debug_profile_and_coverage = Split("-pg")
	else:
		debug_profile_and_coverage = []

# Stack protection
debug_profile_and_coverage += Split("-fstack-protector-all")

# Debug
debug_profile_and_coverage += Split("-ggdb3")

# Coverage
if coverage:
	debug_profile_and_coverage += Split("-ftest-coverage -fprofile-arcs")

debug_profile_and_coverage += Split("-fPIC")

orgenv = Environment(
	CC="clang" if clang else "gcc", CFLAGS=lto + opt + warn + debug_profile_and_coverage + ([] if not everything else Split('-ansi -pedantic -std=c11')), CXX="clang++" if clang else "gcc", CXXFLAGS=lto + opt + warn + debug_profile_and_coverage + ["-std=c++11"] + ([] if not everything else Split('-pedantic')), LIBS=["m"],
	LINK="clang++" if clang else "g++",
	#CXXFLAGS="-nodefaultlibs -fno-exceptions -w",
	CPPDEFINES = {"VERSION":"\\\"0.2.4b\\\"","REV_DATE":"\\\"11/21/99\\\"","JSTICK_INSTALLED":"1"},
	CPPPATH=(["gdev"] if do_vga else []) + ["src"]
)

orgenv['ENV']['TERM'] = os.environ['TERM']

orgenv.Append(LINKFLAGS=machine + link_lto)

if everything:
	orgenv.Append(LINKFLAGS=debug_profile_and_coverage + Split("-Wl,--gc-sections")) #,--print-gc-sections

if clang:
	orgenv.Append(CXXFLAGS=["-stdlib=libc++"] + (["-ferror-limit=5"] if everything else []))
	orgenv.Append(LINKFLAGS="-stdlib=libc++")
	if everything:
		common_flags = ["-Wno-c++11-long-long", "-Wno-float-equal", "-Wno-padded", "-Wno-format-nonliteral", "-Wno-disabled-macro-expansion", "-Wno-shift-sign-overflow"]
		orgenv.Append(CXXFLAGS=common_flags + ["-Wno-c99-extensions", "-Wno-c++11-compat", "-Wno-c++11-extensions", "-Wno-c++98-compat-pedantic", "-Wno-exit-time-destructors", "-Wno-global-constructors"])
		orgenv.Append(CFLAGS=common_flags)
else:
	orgenv.Append(CXXFLAGS=["-fdiagnostics-color=always"])

env = orgenv.Clone()

if do_vga: env.Append(CPPDEFINES = {"HAVE_LIBVGA":"1"})

if do_sdl:
	env.ParseConfig('PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig/ pkg-config --libs --cflags sdl2')
	env.Append(CPPDEFINES = {"HAVE_LIBSDL": "1"})

if do_vga: env.ParseConfig('pkg-config --libs --cflags directfb')

files = Split("""
src/aaaunit.C
src/aibase.C
src/aiflite.C
src/aigunner.C
src/aipilot2.C
src/aipilot3.C
src/aipilot4.C
src/aipilot5.C
src/aipilot6.C
src/aipilot.C
src/bits.C
src/clip.C
src/cockpit.C
src/colormap.C
src/colorspc.C
src/convpoly.C
src/copoly.C
src/cpoly.C
src/dvector.C
src/earth.C
src/flight.C
src/fltlite.C
src/fltmngr.C
src/fltobj.C
src/fltzview.C
src/font8x8.C
src/fontdev.C
src/game.C
src/globals.C
src/grndunit.C
src/group_3d.C
src/hud.C
src/instrmnt.C
src/key_map.C
src/led2.C
src/linux_joy.C
src/moveable.C
src/mytimer.C
src/obj_3d.C
src/pen.C
src/pilobj.C
src/pilot.C
src/plltt.C
src/port_3d.C
src/portkey.C
src/ppm.C
src/rendpoly.C
src/rndrpoly.C
src/rndzpoly.C
src/rotate.C
src/sairfld.C
src/sarray.C
src/sattkr.C
src/sbfltmdl.C
src/sbrkeys.C
src/scnedit.C
src/sfltmdl.C
src/sfrmtn.C
src/simfile.C
src/simfilex.C
src/siminput.C
src/simmath.C
src/simsnd.C
src/smath.C
src/smnvrst.C
src/sobject.C
src/spilcaps.C
src/splncaps.C
src/srunway.C
src/sslewer.C
src/stact.C
src/starget.C
src/swaypnt.C
src/sweapon.C
src/target.C
src/terrain.C
src/transblt.C
src/traveler.C
src/txtrmap.C
src/unguided.C
src/viewobj.C
src/vmath.C
src/vtable2.C
src/waypoint.C
src/weapons.C
src/zview.C
src/dhlist.c
src/spid.c
src/stime.c
libzip/bits.c
libzip/crc.c
libzip/deflate.c
libzip/inflate.c
libzip/trees.c
libzip/unc.c
""")

displ = Split("""
src/kbdhit.C
src/main.C
src/input.C
src/vga_13.C
""")

objects = [orgenv.Object(x) for x in files]
displobjects = [env.Object(x) for x in displ]

env.Program("src/sabre", objects + displ)

joyenv = orgenv.Clone()

joyenv.Append(CPPDEFINES = ["JOYCAL"])

joyenv.Program(joyenv.Object("tools/calibrate.C"))

gdevenv = orgenv.Clone()
gdevenv.ParseConfig('pkg-config --libs --cflags directfb')
gdevenv.Append(CPPDEFINES = {"HAVE_LIBVGA":"1"})

#if do_vga:
#	gdevenv.Program("tools/fontedit", [gdevenv.Object("tools/fontedit.C"), gdevenv.Object("tools/fontutils.C"), gdev_objs]) #gdev + svgalib
#	gdevenv.Program("tools/hello", [gdevenv.Object("tools/hello.C"), gdevenv.Object("tools/fontutils.C"), gdev_objs])

orgenv.Program(orgenv.Object("tools/mkterrain.c"))
orgenv = orgenv.Clone()
orgenv.Append(LIBS="m")
orgenv.Program(orgenv.Object("tools/rmptextr3.c"))
orgenv.Program(orgenv.Object("tools/mktextr3.c"))
orgenv.Program(orgenv.Object("tools/mktextr.c"))

