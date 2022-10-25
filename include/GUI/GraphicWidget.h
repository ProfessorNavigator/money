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

#ifndef GRAPHICWIDGET_H_
#define GRAPHICWIDGET_H_

#include <iostream>
#include <sigc++/sigc++.h>
#include <gtkmm.h>
#include <filesystem>
#include <fstream>
#include <mgl2/mgl.h>
#include <libintl.h>
#include "OpenDialog.h"
#include "OpenDialogDeals.h"
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
#include "AuxFunc.h"

class GraphicWidget
{
  public:
    GraphicWidget(Glib::RefPtr<Gtk::Application> app, int width, int height,
                  Gtk::Window *mwin);
    virtual
    ~GraphicWidget();
    void
    plot(int variant, int graphic);
    sigc::signal<void
    ()> class_finished;
  private:
    void
    graphic(std::string file, int grvar);
    void
    on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height);
    void
    zoomGraph(Gtk::DrawingArea *drar, int id);
    void
    saveGraph(int mode);
    void
    saveJPEG(Gtk::Window *window, std::string filename, Gtk::FileChooser *filech,
             Gtk::Entry *file, int mode);
    bool
    scrollEvent(double x, double y, Gtk::DrawingArea *drar);
    void
    dragOperation(double x, double y, Gtk::DrawingArea *drar);
    void
    dragEnd(double x, double y);
    void
    rightClick(int n_press, double x, double y,
               Glib::RefPtr<Gtk::GestureClick> cl,
               Gtk::DrawingArea *drar);
    void
    plotMglFunc(std::string file, int variant);

    Glib::RefPtr<Gtk::CssProvider> css_provider;
    std::vector<double> plotincr;
    int Height, Width;
    int X = 0;
    int Y = 0;
    std::vector<std::tuple<std::string, double, double, double, double>> dateplot;
    Glib::RefPtr<Gtk::Application> App;
    Gtk::Window *Mwin;
    std::string Opendate;
    mglGraph *gr = nullptr;
};

#endif /* GRAPHICWIDGET_H_ */
