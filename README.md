Eve-Xin
=======

Skill planner for Eve Online

Status
======
This is very much in the early stages. It is possible to create skill plans, but they're not terribly useful. Still, give it a go if you want.

Getting the source
==================

Eve-Xin is built using the Qt toolkit and on top of several convenience classes provided by the Swift XMPP projects, and it also hijacks the build system from Swift (which uses scons, so you'll need to have python2). These instructions assume you're using Qt 5, installed in `/opt/Qt/5.2.0/clang_64/`. If you need to adjust the path, please do. If you're using Qt4, remove the `qt5 = 1` line from config.py.

First fetch the Swift source
```
git clone git://swift.im/swift
```
and cd in there
```
cd swift
```
This is the folder in which you'll run the build system.
Now get the Eve-Xin source into the Eve-Xin folder (yes, it must be in this folder).
```
git clone https://github.com/Kev/evexin.git Eve-Xin
```
Now populate `config.py` with the same rules used by Swift, for example:
```
qt = '/opt/Qt/5.2.0/clang_64/'
debug = 1
optimize = 0
qt5 = 1
linkflags = "-lsqlite3"
```
Finally, to build just issue
```
scons Eve-Xin
```
and out will pop a binary for you in the Eve-Xin folder.
