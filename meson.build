project ('Money', 'cpp', default_options : ['cpp_std=c++17'])
src = ['AuxFunc.cpp', 'CalcIndex.cpp', 'DownloadAll.cpp', 'DownloadDeals.cpp', 'DownloadMenu.cpp', 'GraphicWidget.cpp', 'MainWindow.cpp', 'MoneyApplication.cpp', 'Money.cpp', 'OpenDialog.cpp', 'OpenDialogDeals.cpp', 'PlotAllCommon.cpp', 'PlotDealsCommon.cpp', 'PlotGlobalCommon.cpp', 'PlotMoneyGlobal.cpp', 'PlotPriceAll.cpp', 'PlotPriceDeals.cpp', 'PlotPSDGlobal.cpp', 'PlotVolumeAll.cpp', 'PlotVolumeDeals.cpp', 'PlotVolumeGlobal.cpp', 'PSD.cpp', 'PSDDeals.cpp']
gmp = dependency ('gmpxx', method : 'pkg-config')
gtkmm = dependency ('gtkmm-4.0', method : 'pkg-config')
icu = dependency ('icu-i18n', method : 'pkg-config')
zip = dependency ('libzip', method : 'pkg-config')
curl = dependency ('libcurl', method : 'pkg-config')
cpp = meson.get_compiler ('cpp')
mathgl = cpp.find_library ('mgl2', has_headers: ['mgl2/mgl.h'], required: true)
thread_dep = dependency ('threads')
subdir ('po')
executable ('money', sources : src, dependencies : [gmp, gtkmm, icu, zip, curl, mathgl, thread_dep], install : true, gui_app : true)
install_subdir ('Money', install_dir : 'share')
if host_machine.system() == 'linux'
	install_data ('money.png', install_dir : 'share/icons/hicolor/128x128/apps')
	install_data ('ru.mail.bobilev_yury.Money.desktop', install_dir : 'share/applications')
endif
if host_machine.system() == 'windows'
	install_data ('money.ico', install_dir : 'share/icons/hicolor/128x128/apps')
endif	
