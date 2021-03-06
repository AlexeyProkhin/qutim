import qbs.base 1.0

DynamicLibrary {
    name: "qutim-simplecontactlist"
    destination: {
        if (qbs.targetOS === "mac")
            return "qutim.app/Contents/MacOS";
        else
            return "lib";
    }

    Depends { name: "cpp" }
    Depends { name: "libqutim" }
    Depends { name: "qt"; submodules: [ "core", "gui" ] }
    Depends { name: "simplecontactlist-headers" }

    cpp.defines: "SIMPLECONTACTLIST_LIBRARY"
    cpp.visibility: 'hidden'

    Group {
        files: [
            "lib/*.h",
            "lib/*.cpp",
            "lib/*.ui"
        ]
    }

    ProductModule {
        Depends { name: "simplecontactlist-headers" }
    }
}
