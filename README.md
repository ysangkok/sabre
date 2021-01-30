NEW README
===
90'es Linux combat flight sim. A review: http://www.tldp.org/LDP/LG/issue30/ayers3.html

Changes:
* works on 64-bit
* removed svgalib support (only works on i386 anyway)
* added DirectFB support
* new build system (scons based)
* removed C-style casts
* compiles cleanly on Clang 3.6 with many warnings enabled

There are a couple of demos included. You can launch one (after compilation using SCons)
using e.g.

    ./src/sabre -flt furball.flt -demo

Look in the `lib` directory for other demos.

You can view the plane from different POVs using the number keys.

Known bug: The cockpit sprite is only visible if you run with the argument
`-window 320 200`. I raised the default resolution to 1024x768, so by
default it is not visible.


OLD README
===

README file for Sabre Fighter Plane Simulator
Version 0.2.4  January, 1999

Thank you for taking the time to look at Sabre Fighter
Plane Simulator. Sabre is an on-going game development
for the Linux Operating System, worked on as a labor of
love by flight-simulation enthusiasts. 

For now, Sabre is focusing on the older jets and piston-
engined fighters of the Korean War / Cold War era. Featured
is the classic North American F-86 SabreJet, which engaged
the nimble MiG-15 in the skies over Korea. Also included as
of now are the F-84 ThunderJet, the F-51 Mustang, and the
Yak-9. All of the planes in the game are can be flown by the
player as well as the computer pilots.

Sabre can run in any svgalib 8-bit color mode, from 320X200
to 1024X768. By using a "window", higher resolutions can
be selected without loss of playing speed. See the WHATSNEW
document for more details.

REQUIREMENTS
------------
Sabre requires the Linux Operating System and svgalib version 
1.2.10 or greater. To compile and install this distribution, you
must have the standard GNU components. Running the configure
script will advise you of any missing elements, please see
the INSTALL file in this directory for complete installation
instructions. Also note that at this stage, Sabre does not
install itself in any directory other than this distribution
directory.

Because of svgalib requirements, you should be user
root when installing this distribution.

RUNNING Sabre
-------------
Because svgalib uses direct port access, running Sabre as
any user other than root will require you, as user root,
to 'setuid' on the Sabre binary: 

    chmod a+s src/sabre
Additionally, the use of svgalib requires that you be
in a "console screen", rather than an X-Window environment.
To get to a console screen while in fvwm, key <ctl><alt><f2> 
(or <f3>, etc).

For a quick test of whether Sabre works for you or not:

    ./fly
should have you flying over the mountains and valleys of Korea
in your North American SabreJet, armed and ready for action. 

For a more comprehensive view:

    ./RunSabre
will offer you a snappy menu-driven front-end to the game,
in which you can try out various missions, set video mode
and window size, view demos, and browse documentation. 

DOCUMENTATION
-------------
Documentation files can be browsed from within the RunSabre
script, or by viewing the contents of the ./doc directory. Most
useful is ./doc/keys.txt, which contains all the current
key functions available in the game.
 
CONTRIBUTING
------------
Being in an early stage, Sabre offers a tremendous variety
of interesting tasks for those who would find it rewarding.
Some ideas can be found in the TODO file in this directory.

Mail inquiries about contributing, suggestions and bug 
reports to dan@cobite.com

Check out the Sabre web page for current versions
and new information:
http://sabre.cobite.com

Enjoy!
