import datetime

Import("env")

SConscript("Controllers/SConscript")

if env["SCONS_STAGE"] == "build" :
    if not GetOption("help") and not env.get("HAVE_OPENSSL", 0) and not env.get("HAVE_SCHANNEL", 0) :
        print "Error: Eve-Xin requires OpenSSL support, and OpenSSL was not found."
        if "Eve-Xin" in env["PROJECTS"] :
            env["PROJECTS"].remove("Eve-Xin")
    elif not GetOption("help") and not env.get("HAVE_QT", 0) :
        print "Error: Eve-Xin requires Qt. Not building Eve-Xin."
        if "Eve-Xin" in env["PROJECTS"] :
            env["PROJECTS"].remove("Eve-Xin")
    elif env["target"] == "native":
         SConscript("QtUI/SConscript")
