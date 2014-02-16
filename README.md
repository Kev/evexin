Eve-Xin
=======

Skill planner for Eve Online. It has a small website at http://kev.github.io/evexin/

Status
======
This is very much in the early stages. It is possible to create skill plans, but they're not terribly useful. Still, give it a go if you want.

Getting the source
==================

Eve-Xin is built using the Qt toolkit and on top of several convenience classes provided by the Swift XMPP projects, and it also hijacks the build system from Swift (which uses scons, so you'll need to have python2). These instructions assume you're using Qt 5, installed in `/opt/Qt/5.2.0/clang_64/`. If you need to adjust the path, please do. If you're using Qt4, remove the `qt5 = 1` line from config.py.

The helpful make stuff currently doesn't work on Windows. Speak to me if you'd like to build there.

First get the Eve-Xin source into a sensible place on your filesystem.
```
git clone https://github.com/Kev/evexin.git
```
Now fetch the depedencies
```
cd evexin
git submodule init
git submodule update
```
Now populate `swift/config.py`. This may take some fiddling - let me know what settings work for you if these don't. On Windows, use (change the path to Qt as appropriate):
```
debug = 1
optimize = 0
qt5 = 1
experimental = True
qt = "c:\\Qt\\5.2.0\\msvc2010"
```
On a Mac, you might want:
```
qt = '/opt/Qt/5.2.1/clang_64/'
debug = 1
optimize = 0
qt5 = 1
experimental = True
```
Finally, to build just issue
```
make
```
and out will pop a binary for you called Eve-Xin.app (Mac) or eve-xin
on Linux. Windows currently takes a little more fiddling - speak to me
if you want to do this.
