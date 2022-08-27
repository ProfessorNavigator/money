/*
 Copyright 2021-2022 Yury Bobylev <bobilev_yury@mail.ru>

 This file is part of Money.
 Money is free software: you can redistribute it and/or
 modify it under the terms of the GNU General Public License as
 published by the Free Software Foundation, either version 3 of
 the License, or (at your option) any later version.
 Money is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with Money. If not,
 see <https://www.gnu.org/licenses/>.
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <gtkmm.h>
#include <glibmm/dispatcher.h>
#include <sigc++/sigc++.h>
#include <filesystem>
#include <thread>
#include <fstream>
#include <iostream>
#include <libintl.h>
#include "AuxFunc.h"
#include "GraphicWidget.h"
#include "DownloadMenu.h"

class MainWindow : public Gtk::ApplicationWindow
{
public:
  MainWindow ();
  virtual
  ~MainWindow ();
private:
  void
  windowFunc ();
  void
  plotPriceall ();
  void
  plotPSDall ();
  void
  plotVolumeAll ();
  void
  plotMoneyVolumeAll ();
  void
  plotPriceDeals ();
  void
  plotPSDDeals ();
  void
  plotVolumeDeals ();
  void
  plotMoneyVolDeals ();
  void
  plotAllCommon ();
  void
  plotDealsCommon ();
  void
  plotPSDGlobal ();
  void
  plotVolumeGlobal ();
  void
  plotMoneyGlobal ();
  void
  downloadMenu ();
  void
  deleteDialog (int id, Gtk::MessageDialog *dl, DownloadMenu *menu,
		Gtk::Button *button);
  void
  ondwnldmenuCall (int Signid);
  Gdk::Rectangle
  screenRes ();
  void
  aboutProg ();
  int signid;
  Glib::RefPtr<Gtk::CssProvider> css_provider;
  Glib::Dispatcher *dispv = nullptr;
};

#endif /* MAINWINDOW_H_ */
