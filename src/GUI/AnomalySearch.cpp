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

#include <AnomalySearch.h>

AnomalySearch::AnomalySearch(Gtk::Window *par_win)
{
  this->par_win = par_win;
}

AnomalySearch::~AnomalySearch()
{
  // TODO Auto-generated destructor stub
}

void
AnomalySearch::windowFunc()
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
  window->set_application(par_win->get_application());
  window->set_title(gettext("Anomaly search"));
  window->set_transient_for(*par_win);
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
      Glib::ustring("<b>") + Glib::ustring(gettext("Search parameters"))
	  + Glib::ustring("</b>"));
  grid->attach(*beg_date_lb, 0, 0, 3, 1);

  Gtk::Label *day_lb = Gtk::make_managed<Gtk::Label>();
  day_lb->set_margin(5);
  day_lb->set_halign(Gtk::Align::CENTER);
  day_lb->set_text(gettext("Day"));
  grid->attach(*day_lb, 0, 1, 1, 1);

  Gtk::Label *month_lb = Gtk::make_managed<Gtk::Label>();
  month_lb->set_margin(5);
  month_lb->set_halign(Gtk::Align::CENTER);
  month_lb->set_text(gettext("Month"));
  grid->attach(*month_lb, 1, 1, 1, 1);

  Gtk::Label *year_lb = Gtk::make_managed<Gtk::Label>();
  year_lb->set_margin(5);
  year_lb->set_halign(Gtk::Align::CENTER);
  year_lb->set_text(gettext("Year"));
  grid->attach(*year_lb, 2, 1, 1, 1);

  Gtk::Entry *day_ent = Gtk::make_managed<Gtk::Entry>();
  day_ent->set_margin(5);
  day_ent->set_halign(Gtk::Align::CENTER);
  day_ent->set_width_chars(2);
  day_ent->set_max_width_chars(2);
  day_ent->set_max_length(2);
  day_ent->set_input_purpose(Gtk::InputPurpose::DIGITS);
  day_ent->set_alignment(Gtk::Align::CENTER);
  if(date_val.size() > 0)
    {
      day_ent->set_text(date_val[0]);
    }
  grid->attach(*day_ent, 0, 2, 1, 1);

  Gtk::Entry *month_ent = Gtk::make_managed<Gtk::Entry>();
  month_ent->set_margin(5);
  month_ent->set_halign(Gtk::Align::CENTER);
  month_ent->set_width_chars(2);
  month_ent->set_max_width_chars(2);
  month_ent->set_max_length(2);
  month_ent->set_input_purpose(Gtk::InputPurpose::DIGITS);
  month_ent->set_alignment(Gtk::Align::CENTER);
  if(date_val.size() > 1)
    {
      month_ent->set_text(date_val[1]);
    }
  grid->attach(*month_ent, 1, 2, 1, 1);

  Gtk::Entry *year_ent = Gtk::make_managed<Gtk::Entry>();
  year_ent->set_margin(5);
  year_ent->set_halign(Gtk::Align::CENTER);
  year_ent->set_width_chars(4);
  year_ent->set_max_width_chars(4);
  year_ent->set_max_length(4);
  year_ent->set_input_purpose(Gtk::InputPurpose::DIGITS);
  year_ent->set_alignment(Gtk::Align::CENTER);
  if(date_val.size() > 2)
    {
      year_ent->set_text(date_val[2]);
    }
  grid->attach(*year_ent, 2, 2, 1, 1);

  Gtk::Label *depth_lb = Gtk::make_managed<Gtk::Label>();
  depth_lb->set_margin(5);
  depth_lb->set_halign(Gtk::Align::CENTER);
  depth_lb->set_text(gettext("Search depth, days"));
  grid->attach(*depth_lb, 0, 3, 1, 1);

  Gtk::Label *sensitivity_lb = Gtk::make_managed<Gtk::Label>();
  sensitivity_lb->set_margin(5);
  sensitivity_lb->set_halign(Gtk::Align::CENTER);
  sensitivity_lb->set_text(gettext("PPm deviation, percents"));
  grid->attach(*sensitivity_lb, 2, 3, 1, 1);

  Gtk::Entry *depth_ent = Gtk::make_managed<Gtk::Entry>();
  depth_ent->set_margin(5);
  depth_ent->set_halign(Gtk::Align::CENTER);
  depth_ent->set_width_chars(3);
  depth_ent->set_max_width_chars(3);
  depth_ent->set_max_length(3);
  depth_ent->set_input_purpose(Gtk::InputPurpose::DIGITS);
  depth_ent->set_alignment(Gtk::Align::CENTER);
  depth_ent->set_text("7");
  grid->attach(*depth_ent, 0, 4, 1, 1);

  Gtk::Entry *sens_ent = Gtk::make_managed<Gtk::Entry>();
  sens_ent->set_margin(5);
  sens_ent->set_halign(Gtk::Align::CENTER);
  sens_ent->set_width_chars(5);
  sens_ent->set_max_width_chars(5);
  sens_ent->set_max_length(5);
  sens_ent->set_input_purpose(Gtk::InputPurpose::DIGITS);
  sens_ent->set_alignment(Gtk::Align::CENTER);
  sens_ent->set_text("5");
  grid->attach(*sens_ent, 2, 4, 1, 1);

  Gtk::Button *search_b = Gtk::make_managed<Gtk::Button>();
  search_b->set_margin(5);
  search_b->set_halign(Gtk::Align::CENTER);
  search_b->set_name("allButton");
  search_b->set_label(gettext("Search"));
  Gtk::Window *wnd = par_win;
  search_b->signal_clicked().connect(
      [window, wnd, day_ent, month_ent, year_ent, sens_ent, depth_ent]
      {
	AnomalySearch as(wnd);
	as.searchWin(window, day_ent, month_ent, year_ent, sens_ent, depth_ent);
      });
  grid->attach(*search_b, 0, 5, 1, 1);

  Gtk::Button *clear_b = Gtk::make_managed<Gtk::Button>();
  clear_b->set_margin(5);
  clear_b->set_halign(Gtk::Align::CENTER);
  clear_b->set_name("downloadButton");
  clear_b->set_label(gettext("Clear"));
  clear_b->signal_clicked().connect(
      [day_ent, month_ent, year_ent, sens_ent, depth_ent]
      {
	day_ent->set_text("");
	month_ent->set_text("");
	year_ent->set_text("");
	sens_ent->set_text("");
	depth_ent->set_text("");
      });
  grid->attach(*clear_b, 1, 5, 1, 1);

  Gtk::Button *cancel_b = Gtk::make_managed<Gtk::Button>();
  cancel_b->set_margin(5);
  cancel_b->set_halign(Gtk::Align::CENTER);
  cancel_b->set_name("closeButton");
  cancel_b->set_label(gettext("Cancel"));
  cancel_b->signal_clicked().connect([window]
  {
    window->close();
  });
  grid->attach(*cancel_b, 2, 5, 1, 1);

  window->signal_close_request().connect([window]
  {
    window->hide();
    delete window;
    return true;
  },
					 false);

  window->present();
}

void
AnomalySearch::searchWin(Gtk::Window *win, Gtk::Entry *day_ent,
			 Gtk::Entry *month_ent, Gtk::Entry *year_ent,
			 Gtk::Entry *sens_ent, Gtk::Entry *depth_ent)
{
  std::string daystr(day_ent->get_text());
  std::string monthstr(month_ent->get_text());
  std::string yearstr(year_ent->get_text());
  std::string depthstr(depth_ent->get_text());
  std::string sensstr(sens_ent->get_text());

  int daynum = -1;
  int monthnum = -1;
  int yearnum = -1;
  int depth = -1;
  double sens = -1.0;
  std::stringstream strm;
  std::locale loc("C");
  strm.imbue(loc);
  AnomalySearch *as = new AnomalySearch(par_win);
  if(daystr.empty())
    {
      as->errDialog(1, win);
      delete as;
      return void();
    }
  else
    {
      strm.clear();
      strm.str("");
      strm.imbue(loc);
      strm << daystr;
      strm >> daynum;
      if(daynum < 1)
	{
	  as->errDialog(1, win);
	  delete as;
	  return void();
	}
    }

  if(monthstr.empty())
    {
      as->errDialog(2, win);
      delete as;
      return void();
    }
  else
    {
      strm.clear();
      strm.str("");
      strm.imbue(loc);
      strm << monthstr;
      strm >> monthnum;
      if(monthnum < 1 || monthnum > 12)
	{
	  as->errDialog(2, win);
	  delete as;
	  return void();
	}
      if(monthnum == 1 || monthnum == 3 || monthnum == 5 || monthnum == 7
	  || monthnum == 8 || monthnum == 10 || monthnum == 12)
	{
	  if(daynum > 31)
	    {
	      as->errDialog(1, win);
	      delete as;
	      return void();
	    }
	}
      else
	{
	  if(daynum > 30)
	    {
	      as->errDialog(1, win);
	      delete as;
	      return void();
	    }
	}
    }

  if(yearstr.empty())
    {
      as->errDialog(3, win);
      delete as;
      return void();
    }
  else
    {
      strm.clear();
      strm.str("");
      strm.imbue(loc);
      strm << yearstr;
      strm >> yearnum;
      if((yearnum & 3) == 0 && ((yearnum % 25) != 0 || (yearnum & 15) == 0))
	{
	  if(monthnum == 2)
	    {
	      if(daynum > 29)
		{
		  as->errDialog(1, win);
		  delete as;
		  return void();
		}
	    }
	}
      else
	{
	  if(monthnum == 2)
	    {
	      if(daynum > 28)
		{
		  as->errDialog(1, win);
		  delete as;
		  return void();
		}
	    }
	}
    }

  if(depthstr.empty())
    {
      as->errDialog(4, win);
      delete as;
      return void();
    }
  else
    {
      strm.clear();
      strm.str("");
      strm.imbue(loc);
      strm << depthstr;
      strm >> depth;
      if(depth <= 0)
	{
	  as->errDialog(4, win);
	  delete as;
	  return void();
	}
    }

  if(sensstr.empty())
    {
      as->errDialog(5, win);
      delete as;
      return void();
    }
  else
    {
      strm.clear();
      strm.str("");
      strm.imbue(loc);
      strm << sensstr;
      strm >> sens;
      if(sens <= 0)
	{
	  as->errDialog(5, win);
	  delete as;
	  return void();
	}
    }

  Gtk::Window *window = new Gtk::Window;
  window->set_application(win->get_application());
  window->set_transient_for(*win);
  window->set_title(gettext("Searching..."));
  window->set_deletable(false);
  window->set_modal(true);
  window->set_default_size(1, 1);
  window->set_name("downloadWindow");

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::FILL);
  grid->set_valign(Gtk::Align::FILL);
  window->set_child(*grid);

  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::CENTER);
  lab->set_text(gettext("Searching..."));
  grid->attach(*lab, 0, 0, 1, 1);

  Gtk::ProgressBar *prgb = Gtk::make_managed<Gtk::ProgressBar>();
  prgb->set_margin(5);
  prgb->set_halign(Gtk::Align::FILL);
  prgb->set_show_text(true);
  prgb->set_name("progBar");
  grid->attach(*prgb, 0, 1, 1, 1);

  int *cncl = new int(0);
  Gtk::Button *cancel_b = Gtk::make_managed<Gtk::Button>();
  cancel_b->set_margin(5);
  cancel_b->set_halign(Gtk::Align::CENTER);
  cancel_b->set_name("closeButton");
  cancel_b->set_label(gettext("Cancel"));
  cancel_b->signal_clicked().connect([cncl]
  {
    *cncl = 1;
  });
  grid->attach(*cancel_b, 0, 2, 1, 1);

  double *fract = new double(0.0);
  Glib::Dispatcher *progr_disp = new Glib::Dispatcher;
  progr_disp->connect([prgb, fract]
  {
    prgb->set_fraction(*fract);
  });
  as->signal_progr.connect([fract, progr_disp]
  (double fr)
    {
      *fract = fr;
      progr_disp->emit();
    });

  std::vector<std::tuple<std::string, std::string>> *result = new std::vector<
      std::tuple<std::string, std::string>>;
  Glib::Dispatcher *compl_disp = new Glib::Dispatcher;
  compl_disp->connect([window, result, progr_disp, compl_disp, cncl, win]
  {
    window->close();
    if(*cncl == 0)
      {
	AnomalySearch as(nullptr);
	as.resultWin(win, result);
      }
    delete progr_disp;
    delete result;
    delete cncl;
    delete compl_disp;
  });
  as->signal_serch_compl.connect([compl_disp]
  {
    compl_disp->emit();
  });

  window->signal_close_request().connect([window]
  {
    window->hide();
    delete window;
    return true;
  },
					 false);

  window->present();

  AuxFunc af;
  double jd = af.grigtojulian(daynum, monthnum, yearnum, 0, 0, 0);
  std::thread *thr = new std::thread([as, result, jd, depth, sens, cncl, fract]
  {
    as->searchFunc(result, jd, depth, sens, cncl);
    delete as;
    delete fract;
  });
  thr->detach();
  delete thr;
}

void
AnomalySearch::errDialog(int variant, Gtk::Window *win)
{
  Gtk::Window *window = new Gtk::Window;
  window->set_application(par_win->get_application());
  window->set_title(gettext("Error!"));
  window->set_name("errorMessage");
  window->set_transient_for(*win);
  window->set_default_size(1, 1);

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  window->set_child(*grid);
  grid->set_halign(Gtk::Align::CENTER);
  grid->set_valign(Gtk::Align::CENTER);

  Gtk::Label *errtxt = Gtk::make_managed<Gtk::Label>();
  errtxt->set_halign(Gtk::Align::CENTER);
  errtxt->set_margin(5);
  if(variant == 1)
    {
      errtxt->set_text(gettext("Wrong day input!"));
    }
  else if(variant == 2)
    {
      errtxt->set_text(gettext("Wrong month input!"));
    }
  else if(variant == 3)
    {
      errtxt->set_text(gettext("Wrong year input!"));
    }
  else if(variant == 4)
    {
      errtxt->set_text(gettext("Wrong depth input!"));
    }
  else if(variant == 5)
    {
      errtxt->set_text(gettext("Wrong sensitivity input!"));
    }

  grid->attach(*errtxt, 0, 0, 1, 1);

  Gtk::Button *close = Gtk::make_managed<Gtk::Button>();
  close->set_halign(Gtk::Align::CENTER);
  close->set_margin(5);
  close->set_label(gettext("Close"));
  close->set_name("closeButton");
  close->signal_clicked().connect(sigc::mem_fun(*window, &Gtk::Window::close));
  grid->attach(*close, 0, 1, 1, 1);

  window->signal_close_request().connect([window]
  {
    window->hide();
    delete window;
    return true;
  },
					 false);
  window->show();
}

void
AnomalySearch::searchFunc(
    std::vector<std::tuple<std::string, std::string>> *result, double jd,
    int depth, double sens, int *cancel)
{
  jd = jd - static_cast<double>(depth);
  double sns = std::abs(sens * 0.01);
  std::vector<std::filesystem::path> files;
  std::string filename;
  AuxFunc af;
  af.homePath(&filename);
  filename = filename + "/Money";
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  if(std::filesystem::exists(filepath))
    {
      for(auto &dirit : std::filesystem::directory_iterator(filepath))
	{
	  std::filesystem::path p = dirit.path();
	  if(std::filesystem::is_directory(p)
	      && p.filename().u8string() != "Deals"
	      && p.filename().u8string() != "PPm")
	    {
	      for(auto &itdir : std::filesystem::directory_iterator(p))
		{
		  std::filesystem::path p2 = itdir.path();
		  if(!std::filesystem::is_directory(p2))
		    {
		      files.push_back(p2);
		    }
		}
	    }
	}
      std::fstream f;
      std::vector<std::string> fv;
      for(size_t i = 0; i < files.size(); i++)
	{
	  if(*cancel > 0)
	    {
	      result->clear();
	      break;
	    }
	  fv.clear();
	  std::filesystem::path p = files[i];
	  f.open(p, std::ios_base::in);
	  if(f.is_open())
	    {
	      int count = 0;
	      std::string line;
	      while(!f.eof())
		{
		  getline(f, line);
		  if(!line.empty())
		    {
		      if(count > 1)
			{
			  fv.push_back(line);
			}
		    }
		  count++;
		}
	      f.close();
	    }
	  auto eit = fv.rbegin() + depth + 1;
	  if(eit > fv.rend())
	    {
	      eit = fv.rend();
	    }
	  auto itfv = std::find_if(fv.rbegin(), eit, [jd]
	  (auto &el)
	    {
	      std::string line = el;
	      line = line.substr(0, line.find(";"));
	      std::string day = line.substr(0, line.find("."));
	      std::string month = line;
	      month.erase(0, month.find(".") + std::string(".").size());
	      month = month.substr(0, month.find("."));
	      std::string year = line;
	      year.erase(0, year.find(".") + std::string(".").size());
	      year.erase(0, year.find(".") + std::string(".").size());

	      std::stringstream strm;
	      std::locale loc("C");
	      strm.imbue(loc);
	      strm << day;
	      int d;
	      strm >> d;

	      strm.clear();
	      strm.str("");
	      strm.imbue(loc);
	      strm << month;
	      int m;
	      strm >> m;

	      strm.clear();
	      strm.str("");
	      strm.imbue(loc);
	      strm << year;
	      int y;
	      strm >> y;

	      AuxFunc af;
	      double jdch = af.grigtojulian(d, m, y, 0, 0, 0);
	      if(jdch >= jd)
		{
		  return true;
		}
	      else
		{
		  return false;
		}
	    });
	  if(itfv < eit)
	    {
	      std::string line = *fv.rbegin();
	      line.erase(0, line.rfind(";") + std::string(";").size());
	      std::stringstream strm;
	      std::locale loc("C");
	      strm.imbue(loc);
	      strm << line;
	      double last;
	      strm >> last;

	      line = *(eit - 1);
	      line.erase(0, line.rfind(";") + std::string(";").size());
	      strm.clear();
	      strm.str("");
	      strm.imbue(loc);
	      strm << line;
	      double first;
	      strm >> first;

	      double chsns = (last - first) / first;
	      chsns = std::abs(chsns);
	      if(chsns >= sns)
		{
		  std::tuple<std::string, std::string> restup;
		  std::get<0>(restup) = p.parent_path().stem().u8string();
		  std::get<1>(restup) = p.stem().u8string();
		  result->push_back(restup);
		}
	    }
	  signal_progr.emit(
	      static_cast<double>(i + 1) / static_cast<double>(files.size()));
	}
      signal_serch_compl.emit();
    }
  else
    {
      signal_serch_compl.emit();
    }
}

void
AnomalySearch::resultWin(
    Gtk::Window *win, std::vector<std::tuple<std::string, std::string>> *result)
{
  DownloadMenu dm(nullptr);
  std::vector<std::tuple<std::string, std::string>> boards;
  boards = dm.formVectorBoards();
  std::vector<std::tuple<std::string, std::string, std::string>> instrbase;
  std::vector<std::tuple<std::string, std::string, std::string>> instrbasef;
  instrbase = dm.formVectorInstr(1);
  instrbasef = dm.formVectorInstr(2);
  std::copy(instrbasef.begin(), instrbasef.end(),
	    std::back_inserter(instrbase));
  instrbasef.clear();

  Gtk::Window *window = new Gtk::Window;
  window->set_application(win->get_application());
  window->set_default_size(1, 1);
  window->set_title(gettext("Result"));
  window->set_name("mainWindow");
  window->set_transient_for(*win);

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::CENTER);
  grid->set_valign(Gtk::Align::CENTER);
  window->set_child(*grid);

  Gtk::ScrolledWindow *resscrl = Gtk::make_managed<Gtk::ScrolledWindow>();
  resscrl->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
  resscrl->set_margin(5);
  grid->attach(*resscrl, 0, 0, 2, 1);

  Gtk::TreeModel::ColumnRecord record_result;
  Gtk::TreeModelColumn<std::string> board_id;
  Gtk::TreeModelColumn<std::string> instr_id;
  Gtk::TreeModelColumn<Glib::ustring> boards_nm;
  Gtk::TreeModelColumn<Glib::ustring> instr_nm;
  record_result.add(board_id);
  record_result.add(instr_id);
  record_result.add(boards_nm);
  record_result.add(instr_nm);
  Glib::RefPtr<Gtk::ListStore> model_result = Gtk::ListStore::create(
      record_result);

  for(size_t i = 0; i < result->size(); i++)
    {
      auto row = *(model_result->append());
      row[board_id] = std::get<0>(result->at(i));
      row[instr_id] = std::get<1>(result->at(i));

      std::string boardnm = std::get<0>(result->at(i));
      auto itb = std::find_if(boards.begin(), boards.end(), [boardnm]
      (auto &el)
	{
	  return std::get<0>(el) == boardnm;
	});
      if(itb != boards.end())
	{
	  row[boards_nm] = std::get<1>(*itb);
	}
      else
	{
	  row[boards_nm] = std::get<0>(result->at(i));
	}

      std::string instr = std::get<1>(result->at(i));
      auto itib = std::find_if(instrbase.begin(), instrbase.end(), [instr]
      (auto &el)
	{
	  return std::get<0>(el) == instr;
	});
      if(itib != instrbase.end())
	{
	  row[instr_nm] = std::get<2>(*itib);
	}
      else
	{
	  row[instr_nm] = std::get<1>(result->at(i));
	}
    }

  Gtk::TreeView *tv_res = Gtk::make_managed<Gtk::TreeView>();
  tv_res->set_model(model_result);
  tv_res->append_column(gettext("Board"), boards_nm);
  tv_res->append_column(gettext("Instrument"), instr_nm);
  tv_res->set_name("tvAnomality");
  tv_res->set_activate_on_single_click(false);
  Glib::RefPtr<Gtk::GestureClick> clck = Gtk::GestureClick::create();
  clck->set_button(3);
  tv_res->add_controller(clck);
  resscrl->set_child(*tv_res);

  Glib::RefPtr<Gio::SimpleActionGroup> acgroup =
      Gio::SimpleActionGroup::create();
  acgroup->add_action("plot", [tv_res, window]
  {
    auto sel_res = tv_res->get_selection();
    if(sel_res)
      {
	auto iter = sel_res->get_selected();
	if(iter)
	  {
	    std::string board;
	    iter->get_value(0, board);
	    std::string fnm;
	    iter->get_value(1, fnm);
	    AuxFunc af;
	    std::string filename;
	    af.homePath(&filename);
	    filename = filename + "/Money/" + board + "/" + fnm + ".csv";
	    std::filesystem::path filepath = std::filesystem::u8path(filename);
	    if(std::filesystem::exists(filepath))
	      {
		Glib::RefPtr<Gdk::Surface> surf = window->get_surface();
		Glib::RefPtr<Gdk::Display> disp = window->get_display();
		Glib::RefPtr<Gdk::Monitor> mon = disp->get_monitor_at_surface(surf);
		Gdk::Rectangle rec;
		mon->get_geometry(rec);
		GraphicWidget *grw = new GraphicWidget(rec.get_width(),
						       rec.get_height(), window,
						       filepath.u8string());
		grw->plot(0, 8, "");
	      }
	  }
      }
  });
  resscrl->insert_action_group("popup", acgroup);

  Glib::RefPtr<Gio::Menu> menu = Gio::Menu::create();
  menu->append(gettext("Diagram"), "popup.plot");

  Gtk::PopoverMenu *Menu = new Gtk::PopoverMenu;
  Menu->set_parent(*resscrl);
  Menu->set_menu_model(menu);
  Menu->set_has_arrow(true);

  clck->signal_pressed().connect([Menu]
  (int nm, double x, double y)
    {
      Gdk::Rectangle rect(x, y, 1, 1);
      Menu->set_pointing_to(rect);
      Menu->popup();
    });

  int x, y, w, h;
  Gtk::TreeViewColumn *col = tv_res->get_column(0);
  col->cell_get_size(x, y, w, h);

  resscrl->set_min_content_height(h * 20);
  Gtk::Requisition min, nat;
  tv_res->get_preferred_size(min, nat);
  resscrl->set_min_content_width(nat.get_width());

  auto row_res = model_result->children()[0];
  if(row_res)
    {
      Gtk::TreeModel::Path p = model_result->get_path(row_res.get_iter());
      tv_res->set_cursor(p);
    }

  Gtk::Button *plot = Gtk::make_managed<Gtk::Button>();
  plot->set_margin(5);
  plot->set_halign(Gtk::Align::CENTER);
  plot->set_name("allButton");
  plot->set_label(gettext("Diagram"));
  plot->insert_action_group("plotgr", acgroup);
  plot->set_action_name("plotgr.plot");
  grid->attach(*plot, 0, 1, 1, 1);

  Gtk::Button *close = Gtk::make_managed<Gtk::Button>();
  close->set_margin(5);
  close->set_halign(Gtk::Align::CENTER);
  close->set_name("closeButton");
  close->set_label(gettext("Close"));
  close->signal_clicked().connect([window]
  {
    window->close();
  });
  grid->attach(*close, 1, 1, 1, 1);

  window->signal_close_request().connect([window, Menu]
  {
    window->hide();
    delete Menu;
    delete window;
    return true;
  },
					 false);

  window->present();
}
