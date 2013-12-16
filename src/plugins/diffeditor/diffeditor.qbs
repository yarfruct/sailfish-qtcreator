import qbs.base 1.0

import QtcPlugin

QtcPlugin {
    name: "DiffEditor"

    Depends { name: "Qt.widgets" }
    Depends { name: "Core" }
    Depends { name: "TextEditor" }


    files: [
        "diffeditor.cpp",
        "diffeditor.h",
        "diffeditor_global.h",
        "diffeditorconstants.h",
        "diffeditorcontroller.cpp",
        "diffeditorcontroller.h",
        "diffeditorfactory.cpp",
        "diffeditorfactory.h",
        "diffeditorfile.cpp",
        "diffeditorfile.h",
        "diffeditorplugin.cpp",
        "diffeditorplugin.h",
        "diffeditorwidget.cpp",
        "diffeditorwidget.h",
        "differ.cpp",
        "differ.h",
        "diffshoweditor.cpp",
        "diffshoweditor.h",
        "diffshoweditorfactory.cpp",
        "diffshoweditorfactory.h",
    ]
}

