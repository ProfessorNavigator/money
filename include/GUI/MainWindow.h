/*
 Copyright 2021-2023 Yury Bobylev <bobilev_yury@mail.ru>

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

#ifndef INCLUDE_GUI_MAINWINDOW_H_
#define INCLUDE_GUI_MAINWINDOW_H_

#include <gtkmm.h>
#include <filesystem>
#include <thread>
#include <fstream>
#include <iostream>
#include <libintl.h>
#include "AuxFunc.h"
#include "OpenDialog.h"
#include "OpenDialogDeals.h"
#include "GraphicWidget.h"
#include "DownloadTechnical.h"
#include "DownloadMenu.h"
#include "AnomalySearch.h"

class MainWindow : public Gtk::ApplicationWindow
{
public:
  MainWindow();
  virtual
  ~MainWindow();
private:
  void
  windowFunc();
  void
  plotPriceall();
  void
  plotPSDall();
  void
  plotVolumeAll();
  void
  plotMoneyVolumeAll();
  void
  plotPriceDeals();
  void
  plotPSDDeals();
  void
  plotVolumeDeals();
  void
  plotMoneyVolDeals();
  void
  plotAllCommon();
  void
  plotDealsCommon();
  void
  plotPSDGlobal();
  void
  plotVolumeGlobal();
  void
  plotMoneyGlobal();
  void
  anomalySearch();
  void
  downloadMenu();
  void
  dtRespFunc(int *resp, Gtk::Window *window, Gtk::Grid *grid,
	     Gtk::Button *cancel, Gtk::Label *msg);
  Gdk::Rectangle
  screenRes();
  void
  aboutProg();
};

#endif /* INCLUDE_GUI_MAINWINDOW_H_ */
