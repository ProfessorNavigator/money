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

#include "MainWindow.h"

MainWindow::MainWindow()
{
  Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
  AuxFunc af;
  std::filesystem::path p = std::filesystem::u8path(af.get_selfpath());

  std::string css_styles;

  std::string filename = p.parent_path().u8string()
      + "/../share/Money/stylesMoney.css";
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  std::fstream f;
  f.open(filepath, std::ios_base::in | std::ios_base::binary);
  if(f.is_open())
    {
      css_styles.resize(std::filesystem::file_size(filepath));
      f.read(&css_styles[0], css_styles.size());
      f.close();
    }
  css_provider->load_from_data(css_styles);
  Glib::RefPtr<Gdk::Display> disp = this->get_display();
  Gtk::StyleContext::add_provider_for_display(disp, css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);
  windowFunc();
}

MainWindow::~MainWindow()
{
  // TODO Auto-generated destructor stub
}

void
MainWindow::windowFunc()
{
  std::string pathto;
  AuxFunc af;
  af.homePath(&pathto);
  pathto = pathto + "/.Money";
  std::filesystem::path path = std::filesystem::u8path(pathto);
  if(!std::filesystem::exists(path))
    {
      std::filesystem::create_directories(path);
    }
  pathto = path.u8string() + "/DateBegin";
  path = std::filesystem::u8path(pathto);
  if(!std::filesystem::exists(path))
    {
      std::fstream file;
      file.open(path, std::ios_base::out | std::ios_base::binary);
      std::string date = std::to_string(
	  static_cast<int>(af.grigtojulian(23, 3, 1997, 0, 0, 0.0)));
      file.write(date.c_str(), date.size());
      file.close();
    }
  pathto = af.tempPath();
  pathto = pathto + "/Money";

  path = std::filesystem::u8path(pathto);
  if(std::filesystem::exists(path))
    {
      std::filesystem::remove_all(path);
    }

  this->set_title("Money");
  this->set_name("mainWindow");
  this->set_default_size(1, 1);

  auto grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::CENTER);
  grid->set_valign(Gtk::Align::CENTER);
  this->set_child(*grid);

  Gtk::Button *priceall = Gtk::make_managed<Gtk::Button>(gettext("Price"));
  priceall->set_name("allButton");
  priceall->set_margin(5);
  priceall->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotPriceall));
  grid->attach(*priceall, 0, 0);

  Gtk::Button *moneyprice = Gtk::make_managed<Gtk::Button>(
      gettext("Purchasing power of money"));
  moneyprice->set_name("allButton");
  moneyprice->set_margin(5);
  moneyprice->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotPSDall));
  grid->attach(*moneyprice, 0, 1);

  Gtk::Button *moneyvolume = Gtk::make_managed<Gtk::Button>(
      gettext("Shares turnover"));
  moneyvolume->set_name("allButton");
  moneyvolume->set_margin(5);
  moneyvolume->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotVolumeAll));
  grid->attach(*moneyvolume, 0, 2);

  Gtk::Button *moneyvolumev = Gtk::make_managed<Gtk::Button>(
      gettext("Money turnover"));
  moneyvolumev->set_name("allButton");
  moneyvolumev->set_margin(5);
  moneyvolumev->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotMoneyVolumeAll));
  grid->attach(*moneyvolumev, 0, 3);

  Gtk::Button *pricedeals = Gtk::make_managed<Gtk::Button>(
      gettext("Prices for transactions"));
  pricedeals->set_name("dealsButton");
  pricedeals->set_margin(5);
  pricedeals->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotPriceDeals));
  grid->attach(*pricedeals, 0, 4);

  Gtk::Button *dealsmoneypr = Gtk::make_managed<Gtk::Button>(
      gettext("PPm for transactions"));
  dealsmoneypr->set_name("dealsButton");
  dealsmoneypr->set_margin(5);
  dealsmoneypr->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotPSDDeals));
  grid->attach(*dealsmoneypr, 0, 5);

  Gtk::Button *dealsvolume = Gtk::make_managed<Gtk::Button>(
      gettext("Shares turnover for transactions"));
  dealsvolume->set_name("dealsButton");
  dealsvolume->set_margin(5);
  dealsvolume->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotVolumeDeals));
  grid->attach(*dealsvolume, 0, 6);

  Gtk::Button *dealsmoneyvol = Gtk::make_managed<Gtk::Button>(
      gettext("Money turnover for transactions"));
  dealsmoneyvol->set_name("dealsButton");
  dealsmoneyvol->set_margin(5);
  dealsmoneyvol->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotMoneyVolDeals));
  grid->attach(*dealsmoneyvol, 0, 7);

  Gtk::Button *commongraphicall = Gtk::make_managed<Gtk::Button>(
      gettext("Prediction plots (common)"));
  commongraphicall->set_name("commongraphicallButton");
  commongraphicall->set_margin(5);
  commongraphicall->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotAllCommon));
  grid->attach(*commongraphicall, 0, 8);

  Gtk::Button *commongraphicdeals = Gtk::make_managed<Gtk::Button>(
      gettext("Prediction plots (daily)"));
  commongraphicdeals->set_name("commongraphicdealsButton");
  commongraphicdeals->set_margin(5);
  commongraphicdeals->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotDealsCommon));
  grid->attach(*commongraphicdeals, 0, 9);

  Gtk::Button *psd = Gtk::make_managed<Gtk::Button>(gettext("PPm global"));
  psd->set_name("allButton");
  psd->set_margin(5);
  psd->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotPSDGlobal));
  grid->attach(*psd, 0, 10);

  Gtk::Button *volumetotal = Gtk::make_managed<Gtk::Button>(
      gettext("Shares turnover global"));
  volumetotal->set_name("allButton");
  volumetotal->set_margin(5);
  volumetotal->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotVolumeGlobal));
  grid->attach(*volumetotal, 0, 11);

  Gtk::Button *moneytotal = Gtk::make_managed<Gtk::Button>(
      gettext("Money turnover global"));
  moneytotal->set_name("allButton");
  moneytotal->set_margin(5);
  moneytotal->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotMoneyGlobal));
  grid->attach(*moneytotal, 0, 12);

  Gtk::Button *anomalysearch = Gtk::make_managed<Gtk::Button>(
      gettext("Search anomaly"));
  anomalysearch->set_name("anomalyButton");
  anomalysearch->set_margin(5);
  anomalysearch->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::anomalySearch));
  grid->attach(*anomalysearch, 0, 13);

  Gtk::Button *download = Gtk::make_managed<Gtk::Button>(
      gettext("Download data"));
  download->set_name("downloadButton");
  download->set_margin(5);
  download->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::downloadMenu));
  grid->attach(*download, 0, 14);

  Gtk::Button *aboutpr = Gtk::make_managed<Gtk::Button>(gettext("About"));
  aboutpr->set_name("aboutprButton");
  aboutpr->set_margin(5);
  aboutpr->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::aboutProg));
  grid->attach(*aboutpr, 0, 15);
  this->signal_close_request().connect([this, &af]
  {
    std::vector<Gtk::Window*> win = this->get_application()->get_windows();
    for(size_t i = 0; i < win.size(); i++)
      {
	Gtk::Window *wind = win[i];
	if(wind != nullptr && wind != this)
	  {
	    wind->hide();
	    delete wind;
	  }
      }
    this->hide();
    std::string fp = af.tempPath();
    fp = fp + "/Money";

    std::filesystem::path path = std::filesystem::u8path(fp);
    if(std::filesystem::exists(path))
      {
	std::filesystem::remove_all(path);
      }
    return true;
  },
				       false);
}

void
MainWindow::plotPriceall()
{
  Gdk::Rectangle req = screenRes();
  OpenDialog op(this);
  op.creatDialogAll(0, req);
}

void
MainWindow::plotPSDall()
{
  Gdk::Rectangle req = screenRes();
  OpenDialog op(this);
  op.creatDialogAll(1, req);
}

void
MainWindow::plotVolumeAll()
{
  Gdk::Rectangle req = screenRes();
  OpenDialog op(this);
  op.creatDialogAll(2, req);
}

void
MainWindow::plotMoneyVolumeAll()
{
  Gdk::Rectangle req = screenRes();
  OpenDialog op(this);
  op.creatDialogAll(3, req);
}

void
MainWindow::plotPriceDeals()
{
  Gdk::Rectangle req = screenRes();
  OpenDialogDeals opd(this);
  opd.createDialog(4, req);
}

void
MainWindow::plotPSDDeals()
{
  Gdk::Rectangle req = screenRes();
  OpenDialogDeals opd(this);
  opd.createDialog(5, req);
}

void
MainWindow::plotVolumeDeals()
{
  Gdk::Rectangle req = screenRes();
  OpenDialogDeals opd(this);
  opd.createDialog(6, req);
}

void
MainWindow::plotMoneyVolDeals()
{
  Gdk::Rectangle req = screenRes();
  OpenDialogDeals opd(this);
  opd.createDialog(7, req);
}

void
MainWindow::plotAllCommon()
{
  Gdk::Rectangle req = screenRes();
  OpenDialog op(this);
  op.creatDialogAll(8, req);
}

void
MainWindow::plotDealsCommon()
{
  Gdk::Rectangle req = screenRes();
  OpenDialogDeals opd(this);
  opd.createDialog(9, req);
}

void
MainWindow::plotPSDGlobal()
{
  dateQuieryPlotGlobal(2, 10);
}

void
MainWindow::plotVolumeGlobal()
{
  dateQuieryPlotGlobal(2, 11);
}

void
MainWindow::plotMoneyGlobal()
{
  dateQuieryPlotGlobal(2, 12);
}

void
MainWindow::anomalySearch()
{
  AnomalySearch as(this);
  as.windowFunc();
}

void
MainWindow::downloadMenu()
{
  Gtk::Window *window = new Gtk::Window;
  window->set_application(this->get_application());
  window->set_decorated(false);
  window->set_transient_for(*this);
  window->set_modal(true);
  window->set_default_size(1, 1);
  window->set_name("dialog");

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::CENTER);
  grid->set_valign(Gtk::Align::CENTER);
  window->set_child(*grid);

  Gtk::Label *msg = Gtk::make_managed<Gtk::Label>();
  msg->set_margin(5);
  msg->set_halign(Gtk::Align::CENTER);
  msg->set_width_chars(30);
  msg->set_wrap(true);
  msg->set_wrap_mode(Pango::WrapMode::WORD);
  msg->set_justify(Gtk::Justification::CENTER);
  msg->set_use_markup(true);
  msg->set_markup(
      Glib::ustring("<span foreground='#EFFF00' size='large'>")
	  + Glib::ustring(gettext("Wait, shares list is downloading..."))
	  + Glib::ustring("</span>"));
  grid->attach(*msg, 0, 0, 1, 1);

  int *cncl = new int(0);
  DownloadTechnical *dt = new DownloadTechnical(cncl);
  Gtk::Button *cancel = Gtk::make_managed<Gtk::Button>();
  cancel->set_margin(5);
  cancel->set_halign(Gtk::Align::CENTER);
  cancel->set_label(gettext("Cancel"));
  cancel->set_name("closeButton");
  cancel->signal_clicked().connect(
      [window, cancel, grid, msg, cncl]
      {
	msg->set_markup(
	    Glib::ustring("<span foreground='#EFFF00' size='large'>")
		+ Glib::ustring(gettext("Cancelling operation..."))
		+ Glib::ustring("</span>"));
	window->set_default_size(1, 1);
	grid->remove(*cancel);

	*cncl = 1;
      });
  grid->attach(*cancel, 0, 1, 1, 1);

  int *dt_resp = new int(0);
  Glib::Dispatcher *dt_resp_disp = new Glib::Dispatcher;
  dt_resp_disp->connect([dt_resp, dt_resp_disp, window, msg, grid, cancel, this]
  {
    this->dtRespFunc(dt_resp, window, grid, cancel, msg);
    delete dt_resp;
    delete dt_resp_disp;
  });

  dt->techdwnld = [dt_resp, dt_resp_disp, dt]
  (int resp_id)
    {
      *dt_resp = resp_id;
      dt_resp_disp->emit();
      delete dt;
    };

  window->signal_close_request().connect([window]
  {
    window->hide();
    delete window;
    return true;
  },
					 false);

  window->present();

  std::thread *thr = new std::thread([dt, cncl]
  {
    dt->downloadTechnical();
    delete cncl;
  });
  thr->detach();
  delete thr;
}

void
MainWindow::dtRespFunc(int *resp, Gtk::Window *window, Gtk::Grid *grid,
		       Gtk::Button *cancel, Gtk::Label *msg)
{
  if(*resp == 0)
    {
      window->close();
    }
  else if(*resp == 1)
    {
      DownloadMenu menu(this);
      menu.downloadMenu();
      window->close();
    }
  else if(*resp == 2)
    {
      grid->remove(*cancel);

      window->set_default_size(1, 1);
      msg->set_markup(
	  Glib::ustring("<span foreground='#EFFF00' size='large'>")
	      + Glib::ustring(gettext("Network error"))
	      + Glib::ustring("</span>"));

      Gtk::Button *close = Gtk::make_managed<Gtk::Button>();
      close->set_margin(5);
      close->set_halign(Gtk::Align::CENTER);
      close->set_label(gettext("Close"));
      close->set_name("closeButton");
      close->signal_clicked().connect([window]
      {
	window->close();
      });
      grid->attach(*close, 0, 1, 1, 1);

      Glib::RefPtr<Glib::MainContext> mc = Glib::MainContext::get_default();
      while(mc->pending())
	{
	  mc->iteration(true);
	}
    }
}

void
MainWindow::dateQuieryPlotGlobal(int graph_ch, int variant)
{
  std::vector<Glib::ustring> date_val;
  std::string filename;
  AuxFunc af;
  af.homePath(&filename);
  filename = filename + "/.Money/DateBegin";
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  std::fstream f;
  f.open(filepath, std::ios_base::in | std::ios_base::binary);
  if(f.is_open())
    {
      std::string line;
      line.resize(std::filesystem::file_size(filepath));
      f.read(&line[0], line.size());
      f.close();
      std::stringstream strm;
      std::locale loc("C");
      strm.imbue(loc);
      strm << line;
      double JDN;
      strm >> JDN;
      if(JDN > 2450531.0)
	{
	  date_val.push_back(Glib::ustring(af.togrigday(JDN)));
	  date_val.push_back(Glib::ustring(af.togrigmonth(JDN)));
	  date_val.push_back(Glib::ustring(af.togrigyear(JDN)));
	}
    }
  if(date_val.size() == 0)
    {
      time_t ctm = time(NULL);
      ctm = ctm + 3600 * 3;
      ctm = ctm - 3600 * 24;
      tm *moscow = gmtime(&ctm);
      std::stringstream strm;
      std::locale loc("C");
      strm.imbue(loc);
      strm << moscow->tm_mday;
      date_val.push_back(Glib::ustring(strm.str()));

      strm.clear();
      strm.str("");
      strm.imbue(loc);
      strm << moscow->tm_mon + 1;
      if(moscow->tm_mon + 1 < 10)
	{
	  date_val.push_back(Glib::ustring("0") + Glib::ustring(strm.str()));
	}
      else
	{
	  date_val.push_back(Glib::ustring(strm.str()));
	}

      strm.clear();
      strm.str("");
      strm.imbue(loc);
      strm << moscow->tm_year + 1900;
      date_val.push_back(Glib::ustring(strm.str()));
    }

  Gtk::Window *window = new Gtk::Window;
  window->set_application(this->get_application());
  window->set_title(gettext("Interval"));
  window->set_transient_for(*this);
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
  day_lb->set_text(gettext("Day"));
  grid->attach(*day_lb, 1, 1, 1, 1);

  Gtk::Label *month_lb = Gtk::make_managed<Gtk::Label>();
  month_lb->set_margin(5);
  month_lb->set_halign(Gtk::Align::CENTER);
  month_lb->set_text(gettext("Month"));
  grid->attach(*month_lb, 2, 1, 1, 1);

  Gtk::Label *year_lb = Gtk::make_managed<Gtk::Label>();
  year_lb->set_margin(5);
  year_lb->set_halign(Gtk::Align::CENTER);
  year_lb->set_text(gettext("Year"));
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
  day_f_ent->set_text("23");
  grid->attach(*day_f_ent, 1, 2, 1, 1);

  Gtk::Entry *month_f_ent = Gtk::make_managed<Gtk::Entry>();
  month_f_ent->set_margin(5);
  month_f_ent->set_halign(Gtk::Align::CENTER);
  month_f_ent->set_width_chars(2);
  month_f_ent->set_max_width_chars(2);
  month_f_ent->set_max_length(2);
  month_f_ent->set_input_purpose(Gtk::InputPurpose::DIGITS);
  month_f_ent->set_alignment(Gtk::Align::CENTER);
  month_f_ent->set_text("03");
  grid->attach(*month_f_ent, 2, 2, 1, 1);

  Gtk::Entry *year_f_ent = Gtk::make_managed<Gtk::Entry>();
  year_f_ent->set_margin(5);
  year_f_ent->set_halign(Gtk::Align::CENTER);
  year_f_ent->set_width_chars(4);
  year_f_ent->set_max_width_chars(4);
  year_f_ent->set_max_length(4);
  year_f_ent->set_input_purpose(Gtk::InputPurpose::DIGITS);
  year_f_ent->set_alignment(Gtk::Align::CENTER);
  year_f_ent->set_text("1997");
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
  if(date_val.size() > 0)
    {
      day_to_ent->set_text(date_val[0]);
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
  if(date_val.size() > 1)
    {
      month_to_ent->set_text(date_val[1]);
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
  if(date_val.size() > 2)
    {
      year_to_ent->set_text(date_val[2]);
    }
  grid->attach(*year_to_ent, 3, 3, 1, 1);

  Gtk::Button *show = Gtk::make_managed<Gtk::Button>();
  show->set_margin(5);
  show->set_halign(Gtk::Align::CENTER);
  show->set_label(gettext("Show"));
  show->set_name("confirmButton");
  show->signal_clicked().connect(
      [graph_ch, variant, this, day_f_ent, month_f_ent, year_f_ent, day_to_ent,
       month_to_ent, year_to_ent]
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
	double JD = af.grigtojulian(day, month, year, 0, 0, 0.0);
	strm.clear();
	strm.str("");
	strm.imbue(loc);
	strm << std::fixed << JD;
	std::string plotvar = strm.str();

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

	JD = af.grigtojulian(day, month, year, 0, 0, 0.0);
	strm.clear();
	strm.str("");
	strm.imbue(loc);
	strm << std::fixed << JD;

	plotvar = plotvar + " " + strm.str();

	Gdk::Rectangle req = this->screenRes();
	GraphicWidget *gr = new GraphicWidget(req.get_width(), req.get_height(),
					      this, "");
	gr->plot(graph_ch, variant, plotvar);
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

  window->signal_close_request().connect([window]
  {
    window->hide();
    delete window;
    return true;
  },
					 false);

  window->show();
}

Gdk::Rectangle
MainWindow::screenRes()
{
  Glib::RefPtr<Gdk::Surface> surf = this->get_surface();
  Glib::RefPtr<Gdk::Display> disp = this->get_display();
  Glib::RefPtr<Gdk::Monitor> mon = disp->get_monitor_at_surface(surf);
  Gdk::Rectangle req;
  mon->get_geometry(req);
  return req;
}

void
MainWindow::aboutProg()
{
  Gtk::AboutDialog *aboutd = new Gtk::AboutDialog;
  aboutd->set_transient_for(*this);
  aboutd->set_application(this->get_application());
  aboutd->set_name("dialog");

  aboutd->set_program_name("Money");
  aboutd->set_version("3.2");
  aboutd->set_copyright(
      "Copyright 2021-2023 Yury Bobylev <bobilev_yury@mail.ru>");
  AuxFunc af;
  std::filesystem::path p = std::filesystem::u8path(af.get_selfpath());
  std::string filename = p.parent_path().u8string() + "/../share/Money/COPYING";
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  std::fstream f;
  Glib::ustring abbuf;
  f.open(filepath, std::ios_base::in | std::ios_base::binary);
  if(f.is_open())
    {
      size_t sz = std::filesystem::file_size(filepath);
      std::vector<char> ab;
      ab.resize(sz);
      f.read(&ab[0], ab.size());
      f.close();
      abbuf = Glib::ustring(ab.begin(), ab.end());
    }
  else
    {
      std::cerr << "Licence file not found" << std::endl;
    }

  if(abbuf.empty())
    {
      aboutd->set_license_type(Gtk::License::GPL_3_0_ONLY);
    }
  else
    {
      aboutd->set_license(abbuf);
    }

  filename = p.parent_path().u8string() + "/../share/Money/money.png";
  Glib::RefPtr<Gio::File> logofile = Gio::File::create_for_path(filename);
  aboutd->set_logo(Gdk::Texture::create_from_file(logofile));
  abbuf = gettext("Money is a programm to collect statistics from moex.com\n"
		  "Author Yury Bobylev <bobilev_yury@mail.ru>.\n"
		  "Program uses next libraries:\n"
		  "GTK https://www.gtk.org\n"
		  "Libcurl https://curl.se/libcurl/\n"
		  "Libzip https://libzip.org\n"
		  "GMP https://gmplib.org/\n"
		  "ICU https://icu.unicode.org");
  aboutd->set_comments(abbuf);

  aboutd->signal_close_request().connect([aboutd]
  {
    aboutd->hide();
    delete aboutd;
    return true;
  },
					 false);
  aboutd->show();
}
