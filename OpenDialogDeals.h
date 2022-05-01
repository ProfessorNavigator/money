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

#ifndef OPENDIALOGDEALS_H_
#define OPENDIALOGDEALS_H_

#include <gtkmm.h>
#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <tuple>
#include <libintl.h>
#include "AuxFunc.h"

class OpenDialogDeals
{
public:
  OpenDialogDeals (Glib::RefPtr<Gtk::Application> app, Gtk::Window *mwin);
  virtual
  ~OpenDialogDeals ();
  sigc::signal<void
  (std::string)> dealsfilepath;
  sigc::signal<void
  ()> deleteSign;
private:
  void
  createDialog ();
  void
  expandScroll (Gtk::Expander *exp, Gtk::Window *window, Gtk::Grid *grid);
  void
  dateSelection (const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column,
		 Glib::RefPtr<Gtk::ListStore> model,
		 Gtk::TreeModelColumn<unsigned int> id, Gtk::Expander *exp);
  void
  boardsSelection (const Gtk::TreeModel::Path &path,
		   Gtk::TreeViewColumn *column, Gtk::Expander *exp,
		   Gtk::TreeView *view);
  void
  refreshBoards (Gtk::TreeView *view, Gtk::TreeViewColumn *column,
		 Gtk::Expander *exp);
  void
  refreshSort (Gtk::Expander *exp);
  void
  sortSelection (const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column,
		 Gtk::Expander *exp, Gtk::TreeView *view);
  void
  instrSelection (const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column,
		  Gtk::Expander *exp, Gtk::TreeView *view);
  void
  refreshInstr (Gtk::TreeView *view, Gtk::TreeViewColumn *column,
		Gtk::Expander *exp);
  void
  openFile ();
  int selectedboard = 0;
  int selecteddate = 0;
  int sorting = 0;
  int selectedfile = 0;
  sigc::signal<void
  ()> refreshboards;
  sigc::signal<void
  ()> refreshsort;
  sigc::signal<void
  ()> refreshinstr;
  Gtk::TreeModelColumn<unsigned int> idbb;
  Gtk::TreeModelColumn<Glib::ustring> boardcolb;
  Gtk::TreeModelColumn<unsigned int> idss;
  Gtk::TreeModelColumn<Glib::ustring> sortcols;
  Gtk::TreeModelColumn<unsigned int> idii;
  Gtk::TreeModelColumn<Glib::ustring> icoli;
  std::vector<std::pair<Glib::ustring, Glib::ustring>> boards;
  std::vector<std::pair<Glib::ustring, Glib::ustring>> boardstosh;
  std::vector<std::string> dates;
  std::vector<std::tuple<int, int, std::string>> filelist;
  Glib::RefPtr<Gtk::CssProvider> css_provider;
  Glib::RefPtr<Gtk::Application> App;
  Gtk::Window *Mwin;
};

#endif /* OPENDIALOGDEALS_H_ */
