import os, shutil, datetime, re, time
import Version

Import("env")

if env["SCONS_STAGE"] == "build":

  myenv = env.Clone()


  #FIXME: work out dynamics later
  myenv["EVEXIN_VERSION"] = "0.0.1"


  # Disable warnings that affect Qt
  myenv["CXXFLAGS"] = filter(lambda x : x != "-Wfloat-equal", myenv["CXXFLAGS"])
  if "clang" in env["CC"] :
      myenv.Append(CXXFLAGS = ["-Wno-float-equal", "-Wno-shorten-64-to-32", "-Wno-missing-prototypes", "-Wno-unreachable-code", "-Wno-disabled-macro-expansion", "-Wno-unused-private-field", "-Wno-extra-semi", "-Wno-duplicate-enum", "-Wno-missing-variable-declarations", "-Wno-conversion", "-Wno-undefined-reinterpret-cast"])

  myenv.UseFlags(env["SWIFT_CONTROLLERS_FLAGS"])
  myenv.UseFlags(env["SWIFTOOLS_FLAGS"])
  if myenv["HAVE_XSS"] :
    myenv.UseFlags(env["XSS_FLAGS"])
  if env["PLATFORM"] == "posix" :
    myenv.Append(LIBS = ["X11"])
  if myenv["HAVE_SPARKLE"] :
    myenv.UseFlags(env["SPARKLE_FLAGS"])
  myenv.UseFlags(env["SWIFTEN_FLAGS"])
  myenv.UseFlags(env["SWIFTEN_DEP_FLAGS"])
  if myenv.get("HAVE_BREAKPAD") :
    myenv.UseFlags(env["BREAKPAD_FLAGS"])
  if myenv.get("HAVE_GROWL", False) :
      myenv.UseFlags(myenv["GROWL_FLAGS"])
      myenv.Append(CPPDEFINES = ["HAVE_GROWL"])
  if myenv["swift_mobile"] :
      myenv.Append(CPPDEFINES = ["SWIFT_MOBILE"])
  if myenv.get("HAVE_SNARL", False) :
      myenv.UseFlags(myenv["SNARL_FLAGS"])
      myenv.Append(CPPDEFINES = ["HAVE_SNARL"])
  if myenv.get("HAVE_HUNSPELL", True):
      myenv.Append(CPPDEFINES = ["HAVE_HUNSPELL"])
      myenv.UseFlags(myenv["HUNSPELL_FLAGS"])
  if env["PLATFORM"] == "win32" :
      myenv.Append(LIBS = ["cryptui"])
  myenv.UseFlags(myenv["PLATFORM_FLAGS"])
  myenv.UseFlags(myenv["SQLITE_FLAGS"])

  myenv.Tool("qt4", toolpath = ["#/BuildTools/SCons/Tools"])
  myenv.Tool("nsis", toolpath = ["#/BuildTools/SCons/Tools"])
  myenv.Tool("wix", toolpath = ["#/BuildTools/SCons/Tools"])
  myenv.Tool("textfile", toolpath = ["#/BuildTools/SCons/Tools"])
  qt4modules = ['QtCore', 'QtWebKit', 'QtGui']
  if myenv["qt5"] :
    qt_version = '5'
    qt4modules += ['QtWidgets', 'QtWebKitWidgets', 'QtMultimedia']
  else :
    qt_version = '4'
  if env["PLATFORM"] == "posix" :
      qt4modules += ["QtDBus"]
  if env["PLATFORM"] != "win32" and env["PLATFORM"] != "darwin" :
      qt4modules += ["QtNetwork"]

  myenv.EnableQt4Modules(qt4modules, debug = False, version = qt_version)

  if env["PLATFORM"] == "win32" :
    #myenv.Append(LINKFLAGS = ["/SUBSYSTEM:CONSOLE"])
    myenv.Append(LINKFLAGS = ["/SUBSYSTEM:WINDOWS"])
    myenv.Append(LIBS = "qtmain")
    if myenv.get("HAVE_SCHANNEL", 0) :
      myenv.Append(LIBS = "Cryptui")
      myenv.Append(CPPDEFINES = "HAVE_SCHANNEL")
      if env["debug"] and not env["optimize"]:
        myenv.Append(LINKFLAGS = ["/NODEFAULTLIB:msvcrt"])


  sources = [
      "main.cpp",
      "Controllers/Character.cpp",
      "Controllers/DataController.cpp",
      "Controllers/DataStore.cpp",
      #"Controllers/FileDataStore.cpp",
      "Controllers/GeneralResult.cpp",
      "Controllers/HTTPRequest.cpp",
      "Controllers/MainController.cpp",
      "Controllers/Skill.cpp",
      "Controllers/SkillLevel.cpp",
      "Controllers/SkillPlan.cpp",
      "Controllers/SkillPlanList.cpp",
      "Controllers/SkillTree.cpp",
      "Controllers/SqliteDataStore.cpp",
      "QtUI/CharacterListDelegate.cpp",
      "QtUI/CharacterListModel.cpp",
      "QtUI/QtAPIKeyWindow.cpp",
      "QtUI/QtCharacterList.cpp",
      "QtUI/QtCharacterPane.cpp",
      "QtUI/QtEveXin.cpp",
      "QtUI/QtMainWindow.cpp",
      "QtUI/QtSkillDelegate.cpp",
      "QtUI/QtSkillModel.cpp",
      "QtUI/QtSkillPlannerWidget.cpp",
      "QtUI/QtUtilities.cpp",
    ]
  env.Append(UNITTEST_SOURCES = [
#      File("Controllers/UnitTest/SkillPlanTest.cpp"),
    ])


  # myenv.Uic4("QtUI/QtMainWindow.ui")

  # # Determine the version
  # myenv["SWIFT_VERSION"] = Version.getBuildVersion(env.Dir("#").abspath, "swift")
  # if env["PLATFORM"] == "win32" :
  #     swift_windows_version = Version.convertToWindowsVersion(myenv["SWIFT_VERSION"])
  #     myenv["SWIFT_VERSION_MAJOR"] = swift_windows_version[0]
  #     myenv["SWIFT_VERSION_MINOR"] = swift_windows_version[1]
  #     myenv["SWIFT_VERSION_PATCH"] = swift_windows_version[2]


  # if env["PLATFORM"] == "win32" :
  #     res_env = myenv.Clone()
  #     res_env.Append(CPPDEFINES = [
  #             ("SWIFT_COPYRIGHT_YEAR", "\"\\\"2010-%s\\\"\"" % str(time.localtime()[0])),
  #             ("SWIFT_VERSION_MAJOR", "${SWIFT_VERSION_MAJOR}"),
  #             ("SWIFT_VERSION_MINOR", "${SWIFT_VERSION_MINOR}"),
  #             ("SWIFT_VERSION_PATCH", "${SWIFT_VERSION_PATCH}"),
  #         ])
  #     res = res_env.RES("#/Swift/resources/Windows/Swift.rc")
  #   # For some reason, SCons isn't picking up the dependency correctly
  #     # Adding it explicitly until i figure out why
  #     myenv.Depends(res, "../Controllers/BuildVersion.h")
  #     sources += [
  #             "WinUIHelpers.cpp",
  #             "CAPICertificateSelector.cpp",
  #             "WindowsNotifier.cpp",
  #             "#/Swift/resources/Windows/Swift.res"
  #         ]

  

  if env["PLATFORM"] == "darwin" or env["PLATFORM"] == "win32" :
    swiftProgram = myenv.Program("Eve-Xin", sources)
  else :
    # sources += ["QtCertificateViewerDialog.cpp"];
    # myenv.Uic4("QtCertificateViewerDialog.ui");
    swiftProgram = myenv.Program("eve-xin", sources)

  

  # Resources
  commonResources = {
      "": []#"#/Swift/resources/sounds"]
  }

  myenv["TEXTFILESUFFIX"] = ""
  myenv.MyTextfile(target = "COPYING", source = [myenv.File("../COPYING.gpl"), myenv.File("../COPYING.thirdparty")], LINESEPARATOR = "\n\n========\n\n\n")

  ################################################################################
  # Translation
  ################################################################################

  # # Collect available languages
  # translation_languages = []
  # for file in os.listdir(Dir("#/Swift/Translations").abspath) :
  #     if file.startswith("swift_") and file.endswith(".ts") :
  #         translation_languages.append(file[6:-3])

  # # Generate translation modules
  # translation_sources = [env.File("#/Swift/Translations/swift.ts").abspath]
  # translation_modules = []
  # for lang in translation_languages :
  #     translation_resource = "#/Swift/resources/translations/swift_" + lang + ".qm"
  #     translation_source = "#/Swift/Translations/swift_" + lang + ".ts"
  #     translation_sources.append(env.File(translation_source).abspath)
  #     translation_modules.append(env.File(translation_resource).abspath)
  #     myenv.Qm(translation_resource, translation_source)
  #     commonResources["translations"] = commonResources.get("translations", []) + [translation_resource]

  # # LUpdate translation (if requested)
  # if ARGUMENTS.get("update_translations", False) :
  #     myenv.Precious(translation_sources)
  #     remove_obsolete_option = ""
  #     if ARGUMENTS.get("remove_obsolete_translations", False) :
  #         remove_obsolete_option = " -no-obsolete"
  #     for translation_source in filter(lambda x: not x.endswith("_en.ts"), translation_sources) :
  #         t = myenv.Command([translation_source], [], [myenv.Action("$QT4_LUPDATE -I " + env.Dir("#").abspath + remove_obsolete_option + " -silent -codecfortr utf-8 -recursive Swift -ts " + translation_source, cmdstr = "$QT4_LUPDATECOMSTR")])
  #         myenv.AlwaysBuild(t)
    

  ################################################################################

  if env["PLATFORM"] == "darwin" :
    frameworks = []
    if env["HAVE_SPARKLE"] :
      frameworks.append(env["SPARKLE_FRAMEWORK"])
    if env["HAVE_GROWL"] :
      frameworks.append(env["GROWL_FRAMEWORK"])
    commonResources[""] = commonResources.get("", [])# + ["#/Swift/resources/MacOSX/Swift.icns"]
    app = myenv.AppBundle("Eve-Xin", version = myenv["EVEXIN_VERSION"], resources = commonResources, frameworks = frameworks, handlesXMPPURIs = True)
    if env["DIST"] :
      myenv.Command(["#/Packages/Eve-Xin/Eve-Xin-${EVEXIN_VERSION}.dmg"], [app], ["Swift/Packaging/MacOSX/package.sh " + app.path + " Eve-Xin/Packaging/MacOSX/Eve-Xin.dmg.gz $TARGET $QTDIR"])
      dsym = myenv.Command(["Eve-Xin-${EVEXIN_VERSION}.dSYM"], ["Eve-Xin"], ["dsymutil -o ${TARGET} ${SOURCE}"])
      myenv.Command(["#/Packages/Eve-Xin/Eve-Xin-${EVEXIN_VERSION}.dSYM.zip"], dsym, ["cd ${SOURCE.dir} && zip -r ${TARGET.abspath} ${SOURCE.name}"])
    
  if env["PLATFORM"] == "win32" :
    if env["DIST"] or ARGUMENTS.get("dump_trace") :
      commonResources[""] = commonResources.get("", []) + [
          #"#/Swift/resources/images",  
        ]
      if env["SWIFTEN_DLL"] :
        commonResources[""] = commonResources.get("", []) + ["#/Swiften/${SWIFTEN_LIBRARY_FILE}"]
      qtplugins = {}
      qtplugins["imageformats"] = ["gif", "ico", "jpeg", "mng", "svg", "tiff"]
      qtlibs = ["QtCore", "QtGui", "QtNetwork", "QtWebKit", "QtXMLPatterns"]
      if qt_version == '4' :
        qtlibs.append("phonon")
        qtlibs = [lib + '4' for lib in qtlibs]
      else :
        qtlibs += ['QtQuick', 'QtQml', 'QtV8', 'QtMultimedia', 'QtSql', 'QtSensors', 'QtWidgets', 'QtWebKitWidgets', 'QtMultimediaWidgets', 'QtOpenGL', 'QtPrintSupport']
        qtlibs = [lib.replace('Qt', 'Qt5') for lib in qtlibs]
        qtlibs += ['icuin51', 'icuuc51', 'icudt51', 'libGLESv2', 'libEGL']
        qtplugins["platforms"] = ['windows']
        qtplugins["accessible"] = ["taccessiblewidgets"]
      windowsBundleFiles = myenv.WindowsBundle("Eve-Xin", 
        resources = commonResources,
        qtplugins = qtplugins,
        qtlibs = qtlibs,
              qtversion = qt_version)





