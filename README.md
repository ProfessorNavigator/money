# Money
Money is a simple program to collect statistics from [https://www.moex.com/](https://www.moex.com/) and plot corresponding diagrams. Can be used on Linux and Windows.

## Description

Money plots diagrams to show price and other parameters of different securities (mostly). Explanation of diagram's parameters can be found here: [http://samlib.ru/editors/b/bobylew_j_w/society.shtml](http://samlib.ru/editors/b/bobylew_j_w/society.shtml) (unfortunately only in Russian now).

## Installation

### Linux

`git clone https://github.com/ProfessorNavigator/money.git`\
`cd money`\
`meson -Dbuildtype=release build`\
`ninja -C build install`

You may need superuser privileges to execute last command.

### Windows

You can build Money from source by MSYS2 project [https://www.msys2.org/](https://www.msys2.org/). Follow installation instructions from their site. Install dependencies from [Dependencies](README.md#dependencies) section and git (mingw packages). Than create folder where you want to download source code (path must not include spaces or non Latin letters). Open mingw console and execute following commands (in example we download code to C:\Money)\

`cd /c/Money`\
`git clone https://github.com/ProfessorNavigator/money.git`\
`cd money`\
`meson -Dbuildtype=release build`\
`ninja -C build install`

If everything was correct, you can find money.exe file in `msys_folder/mingw_folder/bin`. Icon to create desktop shortcut can be found in `msys_folder/mingw_folder/share/icons/hicolor/128x128/money.ico` path.

## Dependencies

Money uses meson building system, so to build it from source you need meson and ninja.\
Also you need [gtkmm-4.0](http://www.gtkmm.org/), [libcurl](https://curl.se/libcurl/), [libzip](https://libzip.org/), [icu](https://icu.unicode.org/) (version >= 69), [mathgl](http://mathgl.sourceforge.net/), and [gmp](https://gmplib.org/) (actually you need its' C++ binding gmpxx). All libraries must have headers (for building), so if you use for example Debian Linux, you need ...-devel versions of packages.

## Usage

First of all you need to download data from [https://www.moex.com/](https://www.moex.com/). Press "Download data" and chose proper type of downloading.

After downloading completion you will be able to plot corresponding diagrams.

## License

GPLv3+ (see `COPYING`).

## Donation

If you want to help to develop this project, you can assist it by [donation](https://yoomoney.ru/to/4100117795409573)

## Contacts

You can contact author by email \
bobilev_yury@mail.ru