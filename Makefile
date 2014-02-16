all: git swift/Eve-Xin
	cd swift; scons Eve-Xin

config: configure

configure:
	cd swift; scons force-configure=1 Eve-Xin

git:
	git submodule init
	git submodule update

swift/Eve-Xin:
	ln -s .. swift/Eve-Xin

dist: all
	cd swift; scons dist=1 Eve-Xin Packages/Eve-Xin
