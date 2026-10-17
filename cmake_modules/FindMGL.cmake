find_path(MGL_INCLUDE_DIR
    NAMES mgl.h
    PATH_SUFFIXES "mgl" "mgl2"
)

find_library(MGL_LIBRARY
    NAMES mgl mgl2
    REQUIRED
)

find_path(MGL_QT6_INCLUDE_DIR
    NAMES qt.h
    PATH_SUFFIXES "mgl" "mgl2"
)

find_library(MGL_QT6_LIBRARY
    NAMES mgl-qt6 mgl2-qt6
    REQUIRED
)

include(FindPackageHandleStandardArgs)
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    find_file(MGL_DLL
        NAMES libmgl.dll libmgl2.dll
    )

    if(MGL_INCLUDE_DIR AND MGL_LIBRARY AND MGL_DLL)
        add_library(mgl SHARED IMPORTED GLOBAL)
        set_target_properties(mgl PROPERTIES
            IMPORTED_LOCATION "${MGL_DLL}"
            IMPORTED_IMPLIB "${MGL_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${MGL_INCLUDE_DIR}"
        )
    endif()

    find_package_handle_standard_args(MGL
        FOUND_VAR MGL_FOUND
        REQUIRED_VARS MGL_INCLUDE_DIR MGL_LIBRARY MGL_DLL
    )

    find_file(MGL_QT6_DLL
        NAMES libmgl-qt6.dll libmgl2-qt6.dll
    )

    if(MGL_QT6_INCLUDE_DIR AND MGL_QT6_LIBRARY AND MGL_QT6_DLL)
        add_library(mgl-qt6 SHARED IMPORTED GLOBAL)
        set_target_properties(mgl-qt6 PROPERTIES
            IMPORTED_LOCATION "${MGL_QT6_DLL}"
            IMPORTED_IMPLIB "${MGL_QT6_DLL}"
            INTERFACE_INCLUDE_DIRECTORIES "${MGL_QT6_INCLUDE_DIR}"
        )
    endif()

    find_package_handle_standard_args(MGL_QT6
        FOUND_VAR MGL_QT6_FOUND
        REQUIRED_VARS MGL_QT6_INCLUDE_DIR MGL_QT6_LIBRARY MGL_QT6_DLL
    )

elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    if(MGL_INCLUDE_DIR AND MGL_LIBRARY)
        add_library(mgl SHARED IMPORTED GLOBAL)
        set_target_properties(mgl PROPERTIES
            IMPORTED_IMPLIB "${MGL_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${MGL_INCLUDE_DIR}"
        )
    endif()

    find_package_handle_standard_args(MGL
        FOUND_VAR MGL_FOUND
        REQUIRED_VARS MGL_INCLUDE_DIR MGL_LIBRARY
    )

    if(MGL_QT6_INCLUDE_DIR AND MGL_QT6_LIBRARY)
        add_library(mgl-qt6 SHARED IMPORTED GLOBAL)
        set_target_properties(mgl-qt6 PROPERTIES
            IMPORTED_IMPLIB "${MGL_QT6_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${MGL_QT6_INCLUDE_DIR}"
        )
    endif()

    find_package_handle_standard_args(MGL_QT6
        FOUND_VAR MGL_QT6_FOUND
        REQUIRED_VARS MGL_QT6_INCLUDE_DIR MGL_QT6_LIBRARY
    )
endif()
