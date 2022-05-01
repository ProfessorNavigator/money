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

#ifndef DOWNLOADMENU_H_
#define DOWNLOADMENU_H_

#include <gtkmm.h>
#include <glibmm/ustring.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <sigc++/sigc++.h>
#include <glibmm/dispatcher.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <string>
#include <libintl.h>
#include "AuxFunc.h"
#include "DownloadAll.h"
#include "CalcIndex.h"
#include "DownloadDeals.h"

#ifdef __unix__
#include <X11/Xlib.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif

class DownloadMenu
{
public:
  DownloadMenu (Glib::RefPtr<Gtk::Application> app, Gtk::Window *mwin);
  virtual
  ~DownloadMenu ();
  sigc::signal<void
  (int)> techdwnld;
  sigc::signal<void
  ()> class_finished;
  void
  cancelTechnical ();
  void
  downloadTechnical ();
  void
  downloadMenu ();
private:
  int
  availibaleDates ();
  int
  availibaleDatesF ();
  void
  formVector ();
  void
  expandScroll (Gtk::Expander *exp, Gtk::Window *window, Gtk::Grid *grid);
  void
  instrSelection (const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column,
		  Glib::RefPtr<Gtk::ListStore> model,
		  Gtk::TreeModelColumn<unsigned int> id, Gtk::Expander *exp);
  void
  downloadAll ();
  void
  errorMessage ();
  void
  finishMessage ();
  void
  saveDate (int dt);
  void
  calcIndex ();
  void
  downloadDeals ();
  void
  downlodSinglInstrAll ();
  void
  downlodSinglInstrDeals ();
  void
  noDeals (int sel);
  int CancelTech = 0;
  Gtk::Window *MainMenu = nullptr;
  std::vector<std::pair<Glib::ustring, Glib::ustring>> boards;
  std::vector<std::pair<Glib::ustring, Glib::ustring>> instruments;
  std::vector<std::pair<Glib::ustring, Glib::ustring>> instrumentsF;
  int dateserver;
  int datehome;
  int selectedinstr = 0;
  int downloadcancel = 0;
  Glib::RefPtr<Gtk::CssProvider> css_provider;
  Glib::RefPtr<Gtk::Application> App;
  Gtk::Window *Mwin;
};

#endif /* DOWNLOADMENU_H_ */
