cmake_minimum_required(VERSION 3.16)
project(Money LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets LinguistTools Network)

qt_standard_project_setup()

qt_add_executable(Money
    WIN32 MACOSX_BUNDLE
)

qt_add_translations(Money
    TS_FILES translations/Money_ru_RU.ts
)

qt_add_resources(Money "icon"
    PREFIX "/icons"
    BASE styles
    FILES "styles/money.png"
)

add_subdirectory(include)
add_subdirectory(src)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

option(USE_INTERNAL_MGL_MODULE "Use internal fallback module to find MathGL library" OFF)

if(USE_INTERNAL_MGL_MODULE)
    find_package(MGL REQUIRED)
else()
    find_package(MathGL2 REQUIRED)
endif()

find_package(Threads REQUIRED)
find_package(OpenMP COMPONENTS CXX REQUIRED)
find_package(LibUDB REQUIRED)

find_package(PkgConfig REQUIRED)

pkg_check_modules(ICUI18N REQUIRED IMPORTED_TARGET icu-i18n)
pkg_check_modules(ICUUC REQUIRED IMPORTED_TARGET icu-uc)
pkg_check_modules(GMPXX REQUIRED IMPORTED_TARGET gmpxx)

target_include_directories(Money
    PRIVATE
        include
)

target_link_libraries(Money
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network        
        Threads::Threads
        OpenMP::OpenMP_CXX
        PkgConfig::ICUI18N
        PkgConfig::ICUUC
        PkgConfig::GMPXX
        LibUDB::udb
)

if(USE_INTERNAL_MGL_MODULE)
    target_include_directories(Money
        PRIVATE ${MGL_INCLUDE_DIR}
    )
    target_link_libraries(Money
        PRIVATE mgl
    )
else()
    target_include_directories(Money
        PRIVATE ${MathGL2_INCLUDE_DIRS}
    )
    target_link_libraries(Money
        PRIVATE mgl
    )
endif()

install(TARGETS Money
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(FILES ru.mail.bobilev_yury.Money.desktop
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")
    install(FILES styles/money.png
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/128x128/apps")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(FILES styles/money.ico
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/128x128/apps")
endif()
