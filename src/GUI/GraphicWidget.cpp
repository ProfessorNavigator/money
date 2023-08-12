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

#include "GraphicWidget.h"

GraphicWidget::GraphicWidget(int width, int height, Gtk::Window *mwin,
			     std::string plotfnm)
{
  Mwin = mwin;
  this->plotfnm = plotfnm;
  plotincr.push_back(0);
  plotincr.push_back(0);
  plotincr.push_back(1);
  plotincr.push_back(1);
  Height = height;
  Width = width;
}

GraphicWidget::~GraphicWidget()
{
  if(gr)
    {
      gr->SetPlotFactor(0);
      gr->Zoom(0.0, 0.0, 1.0, 1.0);
    }
  delete gr;
}

void
GraphicWidget::plot(int variant, int graph, std::string opendate)
{

  Opendate = opendate;
  if(variant == 0 || variant == 1)
    {
      gr = new mglGraph;
      plotMglFunc(plotfnm, graph);
    }
  else if(variant == 2)
    {
      AuxFunc af;
      std::string file;
      af.homePath(&file);
      file = file + "/Money/PPm/PPm.csv";
      gr = new mglGraph;
      plotMglFunc(file, graph);
      graphic(file, Mwin, graph);
    }
}

void
GraphicWidget::graphic(std::string file, Gtk::Window *win, int graphvar)
{
  std::filesystem::path p = std::filesystem::u8path(file);
  Gtk::Window *window = new Gtk::Window;
  window->set_application(Mwin->get_application());
  Gtk::Overlay *overlay = Gtk::make_managed<Gtk::Overlay>();
  window->set_title(gettext("Diagram"));
  window->set_child(*overlay);
  window->fullscreen();

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_name("mainWindow");
  grid->set_halign(Gtk::Align::END);
  grid->set_valign(Gtk::Align::CENTER);
  grid->set_margin(5);

  Gtk::DrawingArea *drar = Gtk::make_managed<Gtk::DrawingArea>();
  drar->set_draw_func(
      sigc::bind(sigc::mem_fun(*this, &GraphicWidget::on_draw)));

  Glib::RefPtr<Gtk::EventControllerScroll> scr =
      Gtk::EventControllerScroll::create();
  scr->set_flags(
      Gtk::EventControllerScroll::Flags::VERTICAL
	  | Gtk::EventControllerScroll::Flags::DISCRETE);
  scr->signal_scroll().connect(
      sigc::bind(sigc::mem_fun(*this, &GraphicWidget::scrollEvent), drar),
      true);
  drar->add_controller(scr);

  Glib::RefPtr<Gtk::GestureDrag> drag = Gtk::GestureDrag::create();
  drag->signal_drag_update().connect(
      sigc::bind(sigc::mem_fun(*this, &GraphicWidget::dragOperation), drar));
  drag->signal_drag_end().connect(
      sigc::mem_fun(*this, &GraphicWidget::dragEnd));
  drar->add_controller(drag);

  Glib::RefPtr<Gtk::GestureClick> cl = Gtk::GestureClick::create();
  cl->set_button(3);
  drar->add_controller(cl);

  overlay->set_child(*drar);
  overlay->add_overlay(*grid);
  if(p.filename().stem().u8string() != "PPm")
    {
      Gtk::Grid *instrgr = Gtk::make_managed<Gtk::Grid>();
      instrgr->set_halign(Gtk::Align::START);
      instrgr->set_valign(Gtk::Align::START);

      Gtk::Label *instrnm = Gtk::make_managed<Gtk::Label>();
      instrnm->set_halign(Gtk::Align::START);
      instrnm->set_margin(5);
      instrnm->set_name("instrumLab");
      std::vector<std::tuple<std::string, std::string, std::string>> instrbase;
      std::vector<std::tuple<std::string, std::string, std::string>> instrbasef;
      DownloadMenu dm(nullptr);
      instrbase = dm.formVectorInstr(1);
      instrbasef = dm.formVectorInstr(2);
      std::copy(instrbasef.begin(), instrbasef.end(),
		std::back_inserter(instrbase));
      instrbasef.clear();
      std::string val = p.filename().stem().u8string();
      auto itib = std::find_if(instrbase.begin(), instrbase.end(), [val]
      (auto &el)
	{
	  return std::get<0>(el) == val;
	});
      if(itib != instrbase.end())
	{
	  val = std::get<2>(*itib);
	  instrnm->set_text(gettext("Instrument: ") + Glib::ustring(val));
	}
      else
	{
	  val = p.filename().stem().u8string();
	  val = val.substr(0, val.find("-"));
	  itib = std::find_if(instrbase.begin(), instrbase.end(), [val]
	  (auto &el)
	    {
	      return std::get<0>(el) == val;
	    });
	  if(itib != instrbase.end())
	    {
	      val = std::get<2>(*itib);
	      instrnm->set_text(gettext("Instrument: ") + Glib::ustring(val));
	    }
	  else
	    {
	      instrnm->set_text(
		  gettext("Instrument: ")
		      + Glib::ustring(p.filename().stem().u8string()));
	    }
	}
      instrgr->attach(*instrnm, 0, 0, 1, 1);
      if(!Opendate.empty())
	{
	  Gtk::Label *date = Gtk::make_managed<Gtk::Label>();
	  date->set_halign(Gtk::Align::START);
	  date->set_margin(5);
	  date->set_name("instrumLab");
	  date->set_text(gettext("Date: ") + Glib::ustring(Opendate));
	  instrgr->attach(*date, 0, 1, 1, 1);
	}

      overlay->add_overlay(*instrgr);
    }

  Gtk::Button *increase = Gtk::make_managed<Gtk::Button>();
  increase->set_label(gettext("Increase"));
  increase->set_halign(Gtk::Align::CENTER);
  increase->set_margin(5);
  increase->set_name("allButton");
  increase->signal_clicked().connect(
      sigc::bind(sigc::mem_fun(*this, &GraphicWidget::zoomGraph), drar, 0));
  grid->attach(*increase, 0, 0, 1, 1);

  Gtk::Button *decrease = Gtk::make_managed<Gtk::Button>();
  decrease->set_label(gettext("Decrease"));
  decrease->set_halign(Gtk::Align::CENTER);
  decrease->set_margin(5);
  decrease->set_name("allButton");
  decrease->signal_clicked().connect(
      sigc::bind(sigc::mem_fun(*this, &GraphicWidget::zoomGraph), drar, 1));
  grid->attach(*decrease, 0, 1, 1, 1);

  Gtk::Button *up = Gtk::make_managed<Gtk::Button>();
  up->set_label(gettext("Move up"));
  up->set_halign(Gtk::Align::CENTER);
  up->set_margin(5);
  up->set_name("allButton");
  up->signal_clicked().connect(
      sigc::bind(sigc::mem_fun(*this, &GraphicWidget::zoomGraph), drar, 2));
  grid->attach(*up, 0, 2, 1, 1);

  Gtk::Button *down = Gtk::make_managed<Gtk::Button>();
  down->set_label(gettext("Move down"));
  down->set_halign(Gtk::Align::CENTER);
  down->set_margin(5);
  down->set_name("allButton");
  down->signal_clicked().connect(
      sigc::bind(sigc::mem_fun(*this, &GraphicWidget::zoomGraph), drar, 3));
  grid->attach(*down, 0, 3, 1, 1);

  Gtk::Button *left = Gtk::make_managed<Gtk::Button>();
  left->set_label(gettext("Move left"));
  left->set_halign(Gtk::Align::CENTER);
  left->set_margin(5);
  left->set_name("allButton");
  left->signal_clicked().connect(
      sigc::bind(sigc::mem_fun(*this, &GraphicWidget::zoomGraph), drar, 4));
  grid->attach(*left, 0, 4, 1, 1);

  Gtk::Button *right = Gtk::make_managed<Gtk::Button>();
  right->set_label(gettext("Move right"));
  right->set_halign(Gtk::Align::CENTER);
  right->set_margin(5);
  right->set_name("allButton");
  right->signal_clicked().connect(
      sigc::bind(sigc::mem_fun(*this, &GraphicWidget::zoomGraph), drar, 5));
  grid->attach(*right, 0, 5, 1, 1);

  Gtk::Button *restore = Gtk::make_managed<Gtk::Button>();
  restore->set_label(gettext("Recover"));
  restore->set_halign(Gtk::Align::CENTER);
  restore->set_margin(5);
  restore->set_name("aboutprButton");
  restore->signal_clicked().connect(
      sigc::bind(sigc::mem_fun(*this, &GraphicWidget::zoomGraph), drar, 6));
  grid->attach(*restore, 0, 6, 1, 1);

  Gtk::Button *savejpg = Gtk::make_managed<Gtk::Button>();
  savejpg->set_label(gettext("Save as jpg"));
  savejpg->set_halign(Gtk::Align::CENTER);
  savejpg->set_margin(5);
  savejpg->set_name("saveButton");
  savejpg->signal_clicked().connect(
      sigc::bind(sigc::mem_fun(*this, &GraphicWidget::saveGraph), window, 0));
  grid->attach(*savejpg, 0, 7, 1, 1);

  Gtk::Button *savepng = Gtk::make_managed<Gtk::Button>();
  savepng->set_label(gettext("Save as png"));
  savepng->set_halign(Gtk::Align::CENTER);
  savepng->set_margin(5);
  savepng->set_name("saveButton");
  savepng->signal_clicked().connect(
      sigc::bind(sigc::mem_fun(*this, &GraphicWidget::saveGraph), window, 1));
  grid->attach(*savepng, 0, 8, 1, 1);

  Gtk::Button *savesvg = Gtk::make_managed<Gtk::Button>();
  savesvg->set_label(gettext("Save as svg"));
  savesvg->set_halign(Gtk::Align::CENTER);
  savesvg->set_margin(5);
  savesvg->set_name("saveButton");
  savesvg->signal_clicked().connect(
      sigc::bind(sigc::mem_fun(*this, &GraphicWidget::saveGraph), window, 2));
  grid->attach(*savesvg, 0, 9, 1, 1);

  Gtk::Button *minb = Gtk::make_managed<Gtk::Button>();
  minb->set_label(gettext("Minimize"));
  minb->set_halign(Gtk::Align::CENTER);
  minb->set_margin(5);
  minb->set_name("confirmButton");
  minb->signal_clicked().connect([window]
  {
    window->minimize();
  });
  grid->attach(*minb, 0, 10, 1, 1);

  Gtk::Button *close = Gtk::make_managed<Gtk::Button>();
  close->set_label(gettext("Close"));
  close->set_halign(Gtk::Align::CENTER);
  close->set_name("closeButton");
  close->set_margin(5);
  grid->attach(*close, 0, 11, 1, 1);

  cl->signal_pressed().connect(
      sigc::bind(sigc::mem_fun(*this, &GraphicWidget::rightClick), cl, drar));
  close->signal_clicked().connect(sigc::mem_fun(*window, &Gtk::Window::close));
  if(graphvar == 10)
    {
      Gtk::TreeModel::ColumnRecord record;
      Gtk::TreeModelColumn<std::string> dates;
      Gtk::TreeModelColumn<std::string> inflation;
      record.add(dates);
      record.add(inflation);

      Glib::RefPtr<Gtk::ListStore> model = Gtk::ListStore::create(record);
      Gtk::TreeView *treev = Gtk::make_managed<Gtk::TreeView>();
      treev->set_name("inflationTV");
      treev->set_grid_lines(Gtk::TreeView::GridLines::BOTH);
      treev->set_margin(10);
      treev->set_halign(Gtk::Align::START);
      treev->set_valign(Gtk::Align::CENTER);

      std::filesystem::path filename = std::filesystem::u8path(file);
      std::string name = filename.parent_path().u8string();
      name = name + "/Inflation.csv";
      filename = std::filesystem::u8path(name);

      std::fstream f;
      std::string line, temp;
      int count = 0;
      if(!std::filesystem::exists(filename))
	{
	  std::cout << "Inflation file not opened" << std::endl;
	}
      else
	{
	  f.open(filename, std::ios_base::in);
	  while(!f.eof())
	    {
	      getline(f, line);
	      if(!line.empty())
		{
		  temp = line;
		  temp = temp.substr(0, temp.find(";"));
		  if(temp == "Дата")
		    {
		      temp = gettext("Date");
		    }
		  auto row = *(model->append());
		  row[dates] = Glib::ustring(temp);
		  temp = line;
		  temp.erase(0, temp.find(";") + std::string(";").size());
		  if(temp == "ИПС")
		    {
		      temp = gettext("CPP");
		    }
		  if(count == 0)
		    {
		      row[inflation] = temp + " \%";
		    }
		  else
		    {
		      std::stringstream strm;
		      std::locale loc("C");
		      double tmpd;
		      strm.imbue(loc);
		      strm << temp;
		      strm >> tmpd;
		      strm.str("");
		      strm.clear();
		      strm.imbue(std::locale("C"));
		      strm << std::fixed;
		      strm << std::setprecision(2);
		      strm << tmpd;
		      row[inflation] = strm.str();
		    }
		}
	      count = count + 1;
	    }
	  f.close();
	}
      treev->set_model(model);
      treev->append_column("", dates);
      treev->append_column("", inflation);
      treev->set_headers_visible(false);
      Gtk::CellRenderer *ren = treev->get_column_cell_renderer(1);
      ren->set_alignment(0.5, 0.5);
      ren = treev->get_column_cell_renderer(0);
      ren->set_alignment(0.5, 0.5);
      Gtk::Requisition rq1, rq2;
      treev->get_preferred_size(rq1, rq2);
      Gtk::ScrolledWindow *ovlscrl = Gtk::make_managed<Gtk::ScrolledWindow>();
      ovlscrl->set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
      ovlscrl->set_min_content_height(Height);
      ovlscrl->set_min_content_width(rq1.get_width());
      ovlscrl->set_halign(Gtk::Align::START);
      ovlscrl->set_valign(Gtk::Align::CENTER);
      ovlscrl->set_child(*treev);

      overlay->add_overlay(*ovlscrl);
    }

  window->signal_close_request().connect([window, this]
  {
    this->dateplot.clear();
    window->hide();
    delete this;
    delete window;
    return true;
  },
					 false);

  window->present();
}

void
GraphicWidget::plotMglFunc(std::string file, int variant)
{
  if(variant == 0)
    {
      PlotPriceAll *prall = new PlotPriceAll(file, Height, Width, &dateplot);
      dateQuieryPlotGlobal(file, variant, reinterpret_cast<void*>(prall));
    }
  if(variant == 1)
    {
      PSD *psd = new PSD(file, Height, Width, &dateplot);
      dateQuieryPlotGlobal(file, variant, reinterpret_cast<void*>(psd));
    }
  if(variant == 2)
    {
      PlotVolumeAll *val = new PlotVolumeAll(file, Height, Width, &dateplot);
      dateQuieryPlotGlobal(file, variant, reinterpret_cast<void*>(val));
    }
  if(variant == 3)
    {
      PlotMoneyVolumeAll *val = new PlotMoneyVolumeAll(file, Height, Width,
						       &dateplot);
      dateQuieryPlotGlobal(file, variant, reinterpret_cast<void*>(val));
    }

  if(variant == 4)
    {
      PlotPriceDeals *pd = new PlotPriceDeals(file, Height, Width, &dateplot);
      dateQuieryPlotGlobal(file, variant, reinterpret_cast<void*>(pd));
    }
  if(variant == 5)
    {
      PSDDeals *psdd = new PSDDeals(file, Height, Width, &dateplot);
      dateQuieryPlotGlobal(file, variant, reinterpret_cast<void*>(psdd));
    }
  if(variant == 6)
    {
      PlotVolumeDeals *vd = new PlotVolumeDeals(file, Height, Width, &dateplot);
      dateQuieryPlotGlobal(file, variant, reinterpret_cast<void*>(vd));
    }
  if(variant == 7)
    {
      PlotMoneyVolumeDeals *vd = new PlotMoneyVolumeDeals(file, Height, Width,
							  &dateplot);
      dateQuieryPlotGlobal(file, variant, reinterpret_cast<void*>(vd));
    }
  if(variant == 8)
    {
      PlotAllCommon *ac = new PlotAllCommon(file, Height, Width, &dateplot);
      dateQuieryPlotGlobal(file, variant, reinterpret_cast<void*>(ac));
    }
  if(variant == 9)
    {
      PlotDealsCommon *dc = new PlotDealsCommon(file, Height, Width, &dateplot);
      dateQuieryPlotGlobal(file, variant, reinterpret_cast<void*>(dc));
    }
  if(variant == 10)
    {
      PlotPSDGlobal psdg(file, Height, Width, &dateplot, Opendate);
      psdg.Draw(gr);
    }
  if(variant == 11)
    {
      PlotVolumeGlobal pvg(file, Height, Width, &dateplot, Opendate);
      pvg.Draw(gr);
    }
  if(variant == 12)
    {
      PlotMoneyGlobal pmg(file, Height, Width, &dateplot, Opendate);
      pmg.Draw(gr);
    }
  int grwarn = gr->GetWarn();
  if(grwarn != 0)
    {
      std::cerr << "MathGL warning number: " << grwarn << std::endl;
    }
  std::string grwstr(gr->GlobalWarn());
  if(!grwstr.empty())
    {
      std::cerr << "MathGL global warning: " << grwstr << std::endl;
    }
}

void
GraphicWidget::dateQuieryPlotGlobal(std::string file, int variant, void *point)
{
  std::vector<Glib::ustring> date_val;
  if(dateplot.size() > 0
      && (variant == 0 || variant == 1 || variant == 2 || variant == 3
	  || variant == 8))
    {
      std::string val = std::get<0>(*dateplot.begin());
      val = val.substr(0, val.find("."));
      date_val.push_back(Glib::ustring(val));

      val = std::get<0>(*dateplot.begin());
      val.erase(0, val.find(".") + std::string(".").size());
      val = val.substr(0, val.find("."));
      date_val.push_back(Glib::ustring(val));

      val = std::get<0>(*dateplot.begin());
      val.erase(0, val.rfind(".") + std::string(".").size());
      date_val.push_back(Glib::ustring(val));

      val = std::get<0>(*(dateplot.end() - 1));
      val = val.substr(0, val.find("."));
      date_val.push_back(Glib::ustring(val));

      val = std::get<0>(*(dateplot.end() - 1));
      val.erase(0, val.find(".") + std::string(".").size());
      val = val.substr(0, val.find("."));
      date_val.push_back(Glib::ustring(val));

      val = std::get<0>(*(dateplot.end() - 1));
      val.erase(0, val.rfind(".") + std::string(".").size());
      date_val.push_back(Glib::ustring(val));
    }
  else if(dateplot.size() > 0)
    {
      std::string val = std::get<0>(*dateplot.begin());
      val = val.substr(0, val.find(":"));
      date_val.push_back(Glib::ustring(val));

      val = std::get<0>(*dateplot.begin());
      val.erase(0, val.find(":") + std::string(":").size());
      val = val.substr(0, val.find(":"));
      date_val.push_back(Glib::ustring(val));

      val = std::get<0>(*dateplot.begin());
      val.erase(0, val.rfind(":") + std::string(":").size());
      date_val.push_back(Glib::ustring(val));

      val = std::get<0>(*(dateplot.end() - 1));
      val = val.substr(0, val.find(":"));
      date_val.push_back(Glib::ustring(val));

      val = std::get<0>(*(dateplot.end() - 1));
      val.erase(0, val.find(":") + std::string(":").size());
      val = val.substr(0, val.find(":"));
      date_val.push_back(Glib::ustring(val));

      val = std::get<0>(*(dateplot.end() - 1));
      val.erase(0, val.rfind(":") + std::string(":").size());
      date_val.push_back(Glib::ustring(val));
    }
  Gtk::Window *window = new Gtk::Window;
  window->set_application(Mwin->get_application());
  window->set_title(gettext("Interval"));
  window->set_transient_for(*Mwin);
  window->set_name("mainWindow");
  window->set_default_size(1, 1);

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::FILL);
  grid->set_valign(Gtk::Align::FILL);
  window->set_child(*grid);

  Gtk::Label *beg_date_lb = Gtk::make_managed<Gtk::Label>();
  beg_date_lb->set_margin(5);
  beg_date_lb->set_halign(Gtk::Align::CENTER);
  beg_date_lb->set_use_markup(true);
  beg_date_lb->set_markup(
      Glib::ustring("<b>") + Glib::ustring(gettext("Interval"))
	  + Glib::ustring("</b>"));
  grid->attach(*beg_date_lb, 0, 0, 4, 1);

  Gtk::Label *day_lb = Gtk::make_managed<Gtk::Label>();
  day_lb->set_margin(5);
  day_lb->set_halign(Gtk::Align::CENTER);
  if(variant == 0 || variant == 1 || variant == 2 || variant == 3
      || variant == 8)
    {
      day_lb->set_text(gettext("Day"));
    }
  else
    {
      day_lb->set_text(gettext("Hours"));
    }
  grid->attach(*day_lb, 1, 1, 1, 1);

  Gtk::Label *month_lb = Gtk::make_managed<Gtk::Label>();
  month_lb->set_margin(5);
  month_lb->set_halign(Gtk::Align::CENTER);
  if(variant == 0 || variant == 1 || variant == 2 || variant == 3
      || variant == 8)
    {
      month_lb->set_text(gettext("Month"));
    }
  else
    {
      month_lb->set_text(gettext("Minutes"));
    }
  grid->attach(*month_lb, 2, 1, 1, 1);

  Gtk::Label *year_lb = Gtk::make_managed<Gtk::Label>();
  year_lb->set_margin(5);
  year_lb->set_halign(Gtk::Align::CENTER);
  if(variant == 0 || variant == 1 || variant == 2 || variant == 3
      || variant == 8)
    {
      year_lb->set_text(gettext("Year"));
    }
  else
    {
      year_lb->set_text(gettext("Seconds"));
    }
  grid->attach(*year_lb, 3, 1, 1, 1);

  Gtk::Label *from_l = Gtk::make_managed<Gtk::Label>();
  from_l->set_margin(5);
  from_l->set_halign(Gtk::Align::CENTER);
  from_l->set_text(gettext("From:"));
  grid->attach(*from_l, 0, 2, 1, 1);

  Gtk::Entry *day_f_ent = Gtk::make_managed<Gtk::Entry>();
  day_f_ent->set_margin(5);
  day_f_ent->set_halign(Gtk::Align::CENTER);
  day_f_ent->set_width_chars(2);
  day_f_ent->set_max_width_chars(2);
  day_f_ent->set_max_length(2);
  day_f_ent->set_input_purpose(Gtk::InputPurpose::DIGITS);
  day_f_ent->set_alignment(Gtk::Align::CENTER);
  if(date_val.size() == 6)
    {
      day_f_ent->set_text(date_val[0]);
    }
  grid->attach(*day_f_ent, 1, 2, 1, 1);

  Gtk::Entry *month_f_ent = Gtk::make_managed<Gtk::Entry>();
  month_f_ent->set_margin(5);
  month_f_ent->set_halign(Gtk::Align::CENTER);
  month_f_ent->set_width_chars(2);
  month_f_ent->set_max_width_chars(2);
  month_f_ent->set_max_length(2);
  month_f_ent->set_input_purpose(Gtk::InputPurpose::DIGITS);
  month_f_ent->set_alignment(Gtk::Align::CENTER);
  if(date_val.size() == 6)
    {
      month_f_ent->set_text(date_val[1]);
    }
  grid->attach(*month_f_ent, 2, 2, 1, 1);

  Gtk::Entry *year_f_ent = Gtk::make_managed<Gtk::Entry>();
  year_f_ent->set_margin(5);
  year_f_ent->set_halign(Gtk::Align::CENTER);
  year_f_ent->set_width_chars(4);
  year_f_ent->set_max_width_chars(4);
  year_f_ent->set_max_length(4);
  year_f_ent->set_input_purpose(Gtk::InputPurpose::DIGITS);
  year_f_ent->set_alignment(Gtk::Align::CENTER);
  if(date_val.size() == 6)
    {
      year_f_ent->set_text(date_val[2]);
    }
  grid->attach(*year_f_ent, 3, 2, 1, 1);

  Gtk::Label *to_l = Gtk::make_managed<Gtk::Label>();
  to_l->set_margin(5);
  to_l->set_halign(Gtk::Align::CENTER);
  to_l->set_text(gettext("To:"));
  grid->attach(*to_l, 0, 3, 1, 1);

  Gtk::Entry *day_to_ent = Gtk::make_managed<Gtk::Entry>();
  day_to_ent->set_margin(5);
  day_to_ent->set_halign(Gtk::Align::CENTER);
  day_to_ent->set_width_chars(2);
  day_to_ent->set_max_width_chars(2);
  day_to_ent->set_max_length(2);
  day_to_ent->set_input_purpose(Gtk::InputPurpose::DIGITS);
  day_to_ent->set_alignment(Gtk::Align::CENTER);
  if(date_val.size() == 6)
    {
      day_to_ent->set_text(date_val[3]);
    }
  grid->attach(*day_to_ent, 1, 3, 1, 1);

  Gtk::Entry *month_to_ent = Gtk::make_managed<Gtk::Entry>();
  month_to_ent->set_margin(5);
  month_to_ent->set_halign(Gtk::Align::CENTER);
  month_to_ent->set_width_chars(2);
  month_to_ent->set_max_width_chars(2);
  month_to_ent->set_max_length(2);
  month_to_ent->set_input_purpose(Gtk::InputPurpose::DIGITS);
  month_to_ent->set_alignment(Gtk::Align::CENTER);
  if(date_val.size() == 6)
    {
      month_to_ent->set_text(date_val[4]);
    }
  grid->attach(*month_to_ent, 2, 3, 1, 1);

  Gtk::Entry *year_to_ent = Gtk::make_managed<Gtk::Entry>();
  year_to_ent->set_margin(5);
  year_to_ent->set_halign(Gtk::Align::CENTER);
  year_to_ent->set_width_chars(4);
  year_to_ent->set_max_width_chars(4);
  year_to_ent->set_max_length(4);
  year_to_ent->set_input_purpose(Gtk::InputPurpose::DIGITS);
  year_to_ent->set_alignment(Gtk::Align::CENTER);
  if(date_val.size() == 6)
    {
      year_to_ent->set_text(date_val[5]);
    }
  grid->attach(*year_to_ent, 3, 3, 1, 1);

  Gtk::Button *show = Gtk::make_managed<Gtk::Button>();
  show->set_margin(5);
  show->set_halign(Gtk::Align::CENTER);
  show->set_label(gettext("Show"));
  show->set_name("confirmButton");
  show->signal_clicked().connect(
      [window, variant, this, day_f_ent, month_f_ent, year_f_ent, day_to_ent,
       month_to_ent, year_to_ent, point, file]
      {
	std::string val(day_f_ent->get_text());
	if(val.empty())
	  {
	    return void();
	  }
	std::stringstream strm;
	std::locale loc("C");
	strm.imbue(loc);
	strm << val;
	int day;
	strm >> day;

	val = std::string(month_f_ent->get_text());
	if(val.empty())
	  {
	    return void();
	  }
	strm.clear();
	strm.str("");
	strm.imbue(loc);
	strm << val;
	int month;
	strm >> month;

	val = std::string(year_f_ent->get_text());
	if(val.empty())
	  {
	    return void();
	  }
	strm.clear();
	strm.str("");
	strm.imbue(loc);
	strm << val;
	int year;
	strm >> year;

	AuxFunc af;
	double JDbeg;
	if(variant == 0 || variant == 1 || variant == 2 || variant == 3
	    || variant == 8)
	  {
	    JDbeg = af.grigtojulian(day, month, year, 0, 0, 0.0);
	  }
	else
	  {
	    JDbeg = static_cast<double>(day * 3600 + month * 60 + year);
	  }

	val = std::string(day_to_ent->get_text());
	if(val.empty())
	  {
	    return void();
	  }
	strm.clear();
	strm.str("");
	strm.imbue(loc);
	strm << val;
	strm >> day;

	val = std::string(month_to_ent->get_text());
	if(val.empty())
	  {
	    return void();
	  }
	strm.clear();
	strm.str("");
	strm.imbue(loc);
	strm << val;
	strm >> month;

	val = std::string(year_to_ent->get_text());
	if(val.empty())
	  {
	    return void();
	  }
	strm.clear();
	strm.str("");
	strm.imbue(loc);
	strm << val;
	strm >> year;
	double JDend;
	if(variant == 0 || variant == 1 || variant == 2 || variant == 3
	    || variant == 8)
	  {
	    JDend = af.grigtojulian(day, month, year, 0, 0, 0.0);
	  }
	else
	  {
	    JDend = static_cast<double>(day * 3600 + month * 60 + year);
	  }

	if(JDend > JDbeg)
	  {
	    if(variant == 0)
	      {
		PlotPriceAll *prall = reinterpret_cast<PlotPriceAll*>(point);
		prall->cleanVectors(JDbeg, JDend);
		prall->Draw(this->gr);
	      }
	    else if(variant == 1)
	      {
		PSD *psd = reinterpret_cast<PSD*>(point);
		psd->cleanVectors(JDbeg, JDend);
		psd->Draw(this->gr);
	      }
	    else if(variant == 2)
	      {
		PlotVolumeAll *val = reinterpret_cast<PlotVolumeAll*>(point);
		val->cleanVectors(JDbeg, JDend);
		val->Draw(this->gr);
	      }
	    else if(variant == 3)
	      {
		PlotMoneyVolumeAll *val =
		    reinterpret_cast<PlotMoneyVolumeAll*>(point);
		val->cleanVectors(JDbeg, JDend);
		val->Draw(this->gr);
	      }
	    else if(variant == 4)
	      {
		PlotPriceDeals *pd = reinterpret_cast<PlotPriceDeals*>(point);
		pd->cleanVectors(static_cast<int>(JDbeg),
				 static_cast<int>(JDend));
		pd->Draw(this->gr);
	      }
	    else if(variant == 5)
	      {
		PSDDeals *psdd = reinterpret_cast<PSDDeals*>(point);
		psdd->cleanVectors(static_cast<int>(JDbeg),
				   static_cast<int>(JDend));
		psdd->Draw(this->gr);
	      }
	    else if(variant == 6)
	      {
		PlotVolumeDeals *vd = reinterpret_cast<PlotVolumeDeals*>(point);
		vd->cleanVectors(static_cast<int>(JDbeg),
				 static_cast<int>(JDend));
		vd->Draw(this->gr);
	      }
	    else if(variant == 7)
	      {
		PlotMoneyVolumeDeals *vd =
		    reinterpret_cast<PlotMoneyVolumeDeals*>(point);
		vd->cleanVectors(static_cast<int>(JDbeg),
				 static_cast<int>(JDend));
		vd->Draw(this->gr);
	      }
	    else if(variant == 8)
	      {
		PlotAllCommon *ac = reinterpret_cast<PlotAllCommon*>(point);
		ac->cleanVectors(JDbeg, JDend);
		ac->Draw(this->gr);
	      }
	    else if(variant == 9)
	      {
		PlotDealsCommon *dc = reinterpret_cast<PlotDealsCommon*>(point);
		dc->cleanVectors(static_cast<int>(JDbeg),
				 static_cast<int>(JDend));
		dc->Draw(this->gr);
	      }
	  }

	this->graphic(file, this->Mwin, variant);
	window->close();
      });
  grid->attach(*show, 0, 4, 2, 1);

  Gtk::Button *cancel = Gtk::make_managed<Gtk::Button>();
  cancel->set_margin(5);
  cancel->set_halign(Gtk::Align::CENTER);
  cancel->set_label(gettext("Cancel"));
  cancel->set_name("closeButton");
  cancel->signal_clicked().connect([window]
  {
    window->close();
  });
  grid->attach(*cancel, 2, 4, 2, 1);

  window->signal_close_request().connect([window, point, variant]
  {
    if(variant == 0)
      {
	PlotPriceAll *prall = reinterpret_cast<PlotPriceAll*>(point);
	delete prall;
      }
    else if(variant == 1)
      {
	PSD *psd = reinterpret_cast<PSD*>(point);
	delete psd;
      }
    else if(variant == 2)
      {
	PlotVolumeAll *val = reinterpret_cast<PlotVolumeAll*>(point);
	delete val;
      }
    else if(variant == 3)
      {
	PlotMoneyVolumeAll *val = reinterpret_cast<PlotMoneyVolumeAll*>(point);
	delete val;
      }
    else if(variant == 4)
      {
	PlotPriceDeals *pd = reinterpret_cast<PlotPriceDeals*>(point);
	delete pd;
      }
    else if(variant == 5)
      {
	PSDDeals *psdd = reinterpret_cast<PSDDeals*>(point);
	delete psdd;
      }
    else if(variant == 6)
      {
	PlotVolumeDeals *vd = reinterpret_cast<PlotVolumeDeals*>(point);
	delete vd;
      }
    else if(variant == 7)
      {
	PlotMoneyVolumeDeals *vd =
	    reinterpret_cast<PlotMoneyVolumeDeals*>(point);
	delete vd;
      }
    else if(variant == 8)
      {
	PlotAllCommon *ac = reinterpret_cast<PlotAllCommon*>(point);
	delete ac;
      }
    else if(variant == 9)
      {
	PlotDealsCommon *dc = reinterpret_cast<PlotDealsCommon*>(point);
	delete dc;
      }
    window->hide();
    delete window;
    return true;
  },
					 false);

  window->show();
}

void
GraphicWidget::on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width,
		       int height)
{
  auto image = Gdk::Pixbuf::create_from_data(gr->GetRGB(), Gdk::Colorspace::RGB,
					     false, 8, gr->GetWidth(),
					     gr->GetHeight(),
					     3 * gr->GetWidth());
  image = image->scale_simple(width, height, Gdk::InterpType::BILINEAR);
  Gdk::Cairo::set_source_pixbuf(cr, image, 0, 0);
  cr->paint();
  cr->rectangle(0, 0, image->get_width(), image->get_height());
  cr->fill();
}

void
GraphicWidget::zoomGraph(Gtk::DrawingArea *drar, int id)
{
  double pl1, pl2;
  pl1 = (plotincr[2] - plotincr[0]) * 0.1;
  pl2 = (plotincr[3] - plotincr[1]) * 0.1;
  if(id == 0)
    {
      plotincr[0] = plotincr[0] + pl1;
      plotincr[1] = plotincr[1] + pl2;
      plotincr[2] = plotincr[2] - pl1;
      plotincr[3] = plotincr[3] - pl2;
    }
  if(id == 1)
    {
      plotincr[0] = plotincr[0] - pl1;
      plotincr[1] = plotincr[1] - pl2;
      plotincr[2] = plotincr[2] + pl1;
      plotincr[3] = plotincr[3] + pl2;
    }
  if(id == 2)
    {
      pl2 = (plotincr[3] - plotincr[1]) * 0.05;
      plotincr[1] = plotincr[1] - pl2;
      plotincr[3] = plotincr[3] - pl2;
    }
  if(id == 3)
    {
      pl2 = (plotincr[3] - plotincr[1]) * 0.05;
      plotincr[1] = plotincr[1] + pl2;
      plotincr[3] = plotincr[3] + pl2;
    }

  if(id == 4)
    {
      pl1 = (plotincr[2] - plotincr[0]) * 0.05;
      plotincr[0] = plotincr[0] + pl1;
      plotincr[2] = plotincr[2] + pl1;
    }

  if(id == 5)
    {
      pl1 = (plotincr[2] - plotincr[0]) * 0.05;
      plotincr[0] = plotincr[0] - pl1;
      plotincr[2] = plotincr[2] - pl1;
    }
  if(id == 6)
    {
      plotincr[0] = 0;
      plotincr[1] = 0;
      plotincr[2] = 1;
      plotincr[3] = 1;
    }

  gr->Zoom(plotincr[0], plotincr[1], plotincr[2], plotincr[3]);
  drar->queue_draw();
}

void
GraphicWidget::saveGraph(Gtk::Window *par_win, int mode)
{
  Gtk::FileChooserDialog *fch = new Gtk::FileChooserDialog(
      *par_win, gettext("Save as..."), Gtk::FileChooser::Action::SAVE, false);
  fch->set_application(par_win->get_application());

  if(mode == 0)
    {
      fch->set_current_name(Glib::ustring("diagram.jpg"));
    }
  else if(mode == 1)
    {
      fch->set_current_name(Glib::ustring("diagram.png"));
    }
  else if(mode == 2)
    {
      fch->set_current_name(Glib::ustring("diagram.svg"));
    }

  AuxFunc af;
  std::string filename;
  af.homePath(&filename);
  Glib::RefPtr<Gio::File> fl = Gio::File::create_for_path(filename);

  fch->set_current_folder(fl);

  Gtk::Box *box = fch->get_content_area();
  box->set_margin(5);

  Gtk::Button *cancel = fch->add_button(gettext("Cancel"),
					Gtk::ResponseType::CANCEL);
  cancel->set_margin(5);

  Gtk::Button *save = fch->add_button(gettext("Save"),
				      Gtk::ResponseType::APPLY);
  Gtk::Requisition min, nat;
  fch->get_preferred_size(min, nat);
  save->set_margin_bottom(5);
  save->set_margin_end(5);
  save->set_margin_top(5);
  save->set_margin_start(nat.get_width() - 15);

  fch->signal_response().connect(
      [fch, mode, this]
      (int resp_id)
	{
	  if(resp_id == Gtk::ResponseType::CANCEL)
	    {
	      fch->close();
	    }
	  else if(resp_id == Gtk::ResponseType::APPLY)
	    {
	      Glib::RefPtr<Gio::File> fl = fch->get_file();
	      if(fl)
		{
		  std::string filename = fl->get_path();
		  std::filesystem::path filepath = std::filesystem::u8path(filename);
		  if(mode == 0)
		    {
		      this->gr->WriteJPEG(filepath.string().c_str());
		    }
		  else if(mode == 1)
		    {
		      this->gr->WritePNG(filepath.string().c_str());
		    }
		  else if(mode == 2)
		    {
		      this->gr->WriteSVG(filepath.string().c_str());
		    }
		}
	      fch->close();
	    }
	});

  fch->signal_close_request().connect([fch]
  {
    fch->hide();
    delete fch;
    return true;
  },
				      false);
  fch->present();
}

bool
GraphicWidget::scrollEvent(double x, double y, Gtk::DrawingArea *drar)
{
  double pl1, pl2;
  pl1 = (plotincr[2] - plotincr[0]) * 0.1;
  pl2 = (plotincr[3] - plotincr[1]) * 0.1;
  if(y < 0)
    {
      plotincr[0] = plotincr[0] + pl1;
      plotincr[1] = plotincr[1] + pl2;
      plotincr[2] = plotincr[2] - pl1;
      plotincr[3] = plotincr[3] - pl2;
    }
  if(y > 0)
    {
      plotincr[0] = plotincr[0] - pl1;
      plotincr[1] = plotincr[1] - pl2;
      plotincr[2] = plotincr[2] + pl1;
      plotincr[3] = plotincr[3] + pl2;
    }
  gr->Zoom(plotincr[0], plotincr[1], plotincr[2], plotincr[3]);
  drar->queue_draw();
  return true;
}

void
GraphicWidget::dragOperation(double x, double y, Gtk::DrawingArea *drar)
{
  double pl1, pl2;
  pl1 = (plotincr[2] - plotincr[0]) * ((x - X) / Width);
  pl2 = (plotincr[3] - plotincr[1]) * ((y - Y) / Height);

  plotincr[0] = plotincr[0] - pl1;
  plotincr[1] = plotincr[1] + pl2;
  plotincr[2] = plotincr[2] - pl1;
  plotincr[3] = plotincr[3] + pl2;

  X = x;
  Y = y;

  gr->Zoom(plotincr[0], plotincr[1], plotincr[2], plotincr[3]);
  drar->queue_draw();
}

void
GraphicWidget::dragEnd(double x, double y)
{
  X = 0;
  Y = 0;
}

void
GraphicWidget::rightClick(int n_press, double x, double y,
			  Glib::RefPtr<Gtk::GestureClick> cl,
			  Gtk::DrawingArea *drar)
{
  mglPoint p;

  p = gr->CalcXYZ(static_cast<int>(x), static_cast<int>(y));

  size_t index = round(p.val(0));

  if(dateplot.size() > 0 && index < dateplot.size() && index >= 0)
    {
      Gtk::Popover *popover = Gtk::make_managed<Gtk::Popover>();
      popover->set_parent(*drar);
      Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
      Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
      label->set_text(Glib::ustring(std::get<0>(dateplot[index])));
      label->set_margin(5);
      label->set_halign(Gtk::Align::CENTER);
      grid->attach(*label, 0, 0, 1, 1);
      if(gr->GetSplId(x, Height - y) > 0)
	{
	  if(gr->GetSplId(x, Height - y) == 21)
	    {
	      Gtk::Label *labpsd = Gtk::make_managed<Gtk::Label>();
	      labpsd->set_margin(5);
	      labpsd->set_halign(Gtk::Align::START);
	      double tmpd = std::get<1>(dateplot[index]);
	      std::stringstream strm;
	      std::locale loc("C");
	      strm.imbue(loc);
	      strm << std::fixed << tmpd;
	      labpsd->set_text(gettext("PPm ") + Glib::ustring(strm.str()));
	      grid->attach(*labpsd, 0, 1, 1, 1);

	      Gtk::Label *labpsod = Gtk::make_managed<Gtk::Label>();
	      labpsod->set_margin(5);
	      labpsod->set_halign(Gtk::Align::START);
	      tmpd = std::get<2>(dateplot[index]);
	      strm.clear();
	      strm.str("");
	      strm.imbue(loc);
	      strm << std::fixed << tmpd;
	      labpsod->set_text(gettext("PPTm ") + Glib::ustring(strm.str()));
	      grid->attach(*labpsod, 0, 2, 1, 1);

	      Gtk::Label *labdif = Gtk::make_managed<Gtk::Label>();
	      labdif->set_margin(5);
	      labdif->set_halign(Gtk::Align::START);
	      tmpd = std::get<3>(dateplot[index]);
	      strm.clear();
	      strm.str("");
	      strm.imbue(loc);
	      strm << std::fixed << tmpd;
	      labdif->set_text(
		  gettext("PPTm/PPm ") + Glib::ustring(strm.str()) + "%");
	      grid->attach(*labdif, 0, 3, 1, 1);

	      Gtk::Label *labavvol = Gtk::make_managed<Gtk::Label>();
	      labavvol->set_margin(5);
	      labavvol->set_halign(Gtk::Align::START);
	      tmpd = std::get<4>(dateplot[index]);
	      strm.clear();
	      strm.str("");
	      strm.imbue(loc);
	      strm << std::fixed << tmpd;
	      labavvol->set_text(
		  gettext("Average shares volume ")
		      + Glib::ustring(strm.str()));
	      grid->attach(*labavvol, 0, 4, 1, 1);
	    }
	  if(gr->GetSplId(x, Height - y) == 32)
	    {
	      Gtk::Label *labpsd = Gtk::make_managed<Gtk::Label>();
	      labpsd->set_margin(5);
	      labpsd->set_halign(Gtk::Align::START);
	      double tmpd = std::get<1>(dateplot[index]);
	      std::stringstream strm;
	      std::locale loc("C");
	      strm.imbue(loc);
	      strm << std::fixed << std::setprecision(2) << tmpd;
	      labpsd->set_text(
		  gettext("Money turnover ") + Glib::ustring(strm.str()));
	      grid->attach(*labpsd, 0, 1, 1, 1);

	      Gtk::Label *labpsod = Gtk::make_managed<Gtk::Label>();
	      labpsod->set_margin(5);
	      labpsod->set_halign(Gtk::Align::START);
	      tmpd = std::get<2>(dateplot[index]);
	      strm.clear();
	      strm.str("");
	      strm.imbue(loc);
	      strm << std::fixed << std::setprecision(2) << tmpd;
	      labpsod->set_text(
		  gettext("Average turnover ") + Glib::ustring(strm.str()));
	      grid->attach(*labpsod, 0, 2, 1, 1);
	    }
	  if(gr->GetSplId(x, Height - y) == 11)
	    {
	      Gtk::Label *labpsd = Gtk::make_managed<Gtk::Label>();
	      labpsd->set_margin(5);
	      labpsd->set_halign(Gtk::Align::START);
	      double tmpd = std::get<1>(dateplot[index]);
	      std::stringstream strm;
	      std::locale loc("C");
	      strm.imbue(loc);
	      strm << std::fixed << std::setprecision(2) << tmpd;
	      labpsd->set_text(
		  gettext("Weighted average price ")
		      + Glib::ustring(strm.str()));
	      grid->attach(*labpsd, 0, 1, 1, 1);

	      Gtk::Label *labpsod = Gtk::make_managed<Gtk::Label>();
	      labpsod->set_margin(5);
	      labpsod->set_halign(Gtk::Align::START);
	      tmpd = std::get<2>(dateplot[index]);
	      strm.clear();
	      strm.str("");
	      strm.imbue(loc);
	      strm << std::fixed << std::setprecision(2) << tmpd;
	      labpsod->set_text(
		  gettext("First price ") + Glib::ustring(strm.str()));
	      grid->attach(*labpsod, 0, 2, 1, 1);

	      Gtk::Label *labpr = Gtk::make_managed<Gtk::Label>();
	      labpr->set_margin(5);
	      labpr->set_halign(Gtk::Align::START);
	      tmpd = std::get<3>(dateplot[index]);
	      strm.clear();
	      strm.str("");
	      strm.imbue(loc);
	      strm << std::fixed << std::setprecision(2) << tmpd;
	      labpr->set_text(
		  gettext("Last price ") + Glib::ustring(strm.str()));
	      grid->attach(*labpr, 0, 3, 1, 1);

	      Gtk::Label *labost = Gtk::make_managed<Gtk::Label>();
	      labost->set_margin(5);
	      labost->set_halign(Gtk::Align::START);
	      tmpd = std::get<4>(dateplot[index]);
	      strm.clear();
	      strm.str("");
	      strm.imbue(loc);
	      strm << std::fixed << tmpd;
	      labost->set_text(gettext("ECg ") + Glib::ustring(strm.str()));
	      grid->attach(*labost, 0, 4, 1, 1);
	    }
	  if(gr->GetSplId(x, Height - y) == 14)
	    {
	      Gtk::Label *labpsd = Gtk::make_managed<Gtk::Label>();
	      labpsd->set_margin(5);
	      labpsd->set_halign(Gtk::Align::START);
	      double tmpd = std::get<1>(dateplot[index]);
	      std::stringstream strm;
	      std::locale loc("C");
	      strm.imbue(loc);
	      strm << std::fixed << tmpd;
	      labpsd->set_text(gettext("Price ") + Glib::ustring(strm.str()));
	      grid->attach(*labpsd, 0, 1, 1, 1);

	      Gtk::Label *labost = Gtk::make_managed<Gtk::Label>();
	      labost->set_margin(5);
	      labost->set_halign(Gtk::Align::START);
	      tmpd = std::get<2>(dateplot[index]);
	      strm.clear();
	      strm.str("");
	      strm.imbue(loc);
	      strm << std::fixed << tmpd;
	      labost->set_text(gettext("ECg ") + Glib::ustring(strm.str()));
	      grid->attach(*labost, 0, 2, 1, 1);
	    }
	  if(gr->GetSplId(x, Height - y) == 31)
	    {
	      Gtk::Label *labpsd = Gtk::make_managed<Gtk::Label>();
	      labpsd->set_margin(5);
	      labpsd->set_halign(Gtk::Align::START);
	      double tmpd = std::get<1>(dateplot[index]);
	      std::stringstream strm;
	      std::locale loc("C");
	      strm.imbue(loc);
	      strm << std::fixed << tmpd;
	      labpsd->set_text(gettext("PPm ") + Glib::ustring(strm.str()));
	      grid->attach(*labpsd, 0, 1, 1, 1);

	      Gtk::Label *labpsod = Gtk::make_managed<Gtk::Label>();
	      labpsod->set_margin(5);
	      labpsod->set_halign(Gtk::Align::START);
	      tmpd = std::get<2>(dateplot[index]);
	      strm.clear();
	      strm.str("");
	      strm.imbue(loc);
	      strm << std::fixed << tmpd;
	      labpsod->set_text(gettext("PPTm ") + Glib::ustring(strm.str()));
	      grid->attach(*labpsod, 0, 2, 1, 1);
	    }
	  if(gr->GetSplId(x, Height - y) == 12)
	    {
	      Gtk::Label *labpsd = Gtk::make_managed<Gtk::Label>();
	      labpsd->set_margin(5);
	      labpsd->set_halign(Gtk::Align::START);
	      double tmpd = std::get<1>(dateplot[index]);
	      std::stringstream strm;
	      std::locale loc("C");
	      strm.imbue(loc);
	      strm << std::fixed << std::setprecision(2) << tmpd;
	      labpsd->set_text(
		  gettext("Turnover ") + Glib::ustring(strm.str()));
	      grid->attach(*labpsd, 0, 1, 1, 1);

	      Gtk::Label *labpsod = Gtk::make_managed<Gtk::Label>();
	      labpsod->set_margin(5);
	      labpsod->set_halign(Gtk::Align::START);
	      tmpd = std::get<2>(dateplot[index]);
	      strm.clear();
	      strm.str("");
	      strm.imbue(loc);
	      strm << std::fixed << std::setprecision(2) << tmpd;
	      labpsod->set_text(
		  gettext("Average turnover ") + Glib::ustring(strm.str()));
	      grid->attach(*labpsod, 0, 2, 1, 1);
	    }
	}
      popover->set_child(*grid);
      Gdk::Rectangle rect(x, y, 1, 1);
      popover->set_pointing_to(rect);
      popover->popup();
      popover->signal_hide().connect([popover]
      {
	popover->unparent();
      });
      popover->present();
    }
}
