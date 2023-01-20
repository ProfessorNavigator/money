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

#ifndef INCLUDE_GUI_GRAPHICWIDGET_H_
#define INCLUDE_GUI_GRAPHICWIDGET_H_

#include <iostream>
#include <gtkmm.h>
#include <filesystem>
#include <fstream>
#include <mgl2/mgl.h>
#include <libintl.h>
#include "PlotPriceAll.h"
#include "PSD.h"
#include "PlotVolumeAll.h"
#include "PlotMoneyVolumeAll.h"
#include "PlotPriceDeals.h"
#include "PSDDeals.h"
#include "PlotVolumeDeals.h"
#include "PlotAllCommon.h"
#include "PlotDealsCommon.h"
#include "PlotPSDGlobal.h"
#include "PlotVolumeGlobal.h"
#include "PlotMoneyGlobal.h"
#include "PlotMoneyVolumeDeals.h"
#include "DownloadMenu.h"
#include "AuxFunc.h"

class GraphicWidget
{
public:
  GraphicWidget(int width, int height, Gtk::Window *mwin, std::string plotfnm);
  virtual
  ~GraphicWidget();
  void
  plot(int variant, int graph, std::string opendate);
private:
  void
  graphic(std::string file, Gtk::Window *win, int grvar);
  void
  on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height);
  void
  zoomGraph(Gtk::DrawingArea *drar, int id);
  void
  saveGraph(Gtk::Window *par_win, int mode);
  bool
  scrollEvent(double x, double y, Gtk::DrawingArea *drar);
  void
  dragOperation(double x, double y, Gtk::DrawingArea *drar);
  void
  dragEnd(double x, double y);
  void
  rightClick(int n_press, double x, double y,
	     Glib::RefPtr<Gtk::GestureClick> cl, Gtk::DrawingArea *drar);
  void
  plotMglFunc(std::string file, int variant);

  std::vector<double> plotincr;
  int Height, Width;
  int X = 0;
  int Y = 0;
  std::vector<std::tuple<std::string, double, double, double, double>> dateplot;
  Gtk::Window *Mwin;
  std::string Opendate;
  mglGraph *gr = nullptr;
  std::string plotfnm;
};

#endif /* INCLUDE_GUI_GRAPHICWIDGET_H_ */
