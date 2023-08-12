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

#include "OpenDialogDeals.h"

OpenDialogDeals::OpenDialogDeals(Gtk::Window *mwin)
{
  Mwin = mwin;
}

OpenDialogDeals::~OpenDialogDeals()
{
  // TODO Auto-generated destructor stub
}

void
OpenDialogDeals::createDialog(int variant, Gdk::Rectangle rec)
{
  std::vector<std::tuple<std::string, std::string>> boards;
  DownloadMenu dm(nullptr);
  boards = dm.formVectorBoards();
  std::vector<std::tuple<std::string, std::string, std::string>> instrbase;
  std::vector<std::tuple<std::string, std::string, std::string>> instrbasef;
  instrbase = dm.formVectorInstr(1);
  instrbasef = dm.formVectorInstr(2);
  std::copy(instrbasef.begin(), instrbasef.end(),
	    std::back_inserter(instrbase));
  instrbasef.clear();

  std::vector<std::string> dates;
  std::string filename;
  AuxFunc af;
  af.homePath(&filename);
  filename = filename + "/Money/Deals";
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  if(std::filesystem::exists(filepath))
    {
      for(auto &dirit : std::filesystem::directory_iterator(filepath))
	{
	  std::filesystem::path p = dirit.path();
	  if(!std::filesystem::is_directory(p))
	    {
	      dates.push_back(p.stem().u8string());
	    }
	}
    }

  std::sort(dates.begin(), dates.end(), [&af]
  (auto &el1, auto &el2)
    {
      std::string f = el1;
      std::string s = el2;
      std::string day = f;
      day = day.substr(0, day.find("."));
      std::stringstream strm;
      std::locale loc("C");
      strm << day;
      int d;
      strm >> d;

      std::string month = f;
      month.erase(0, month.find(".") + std::string(".").size());
      month = month.substr(0, month.find("."));
      strm.clear();
      strm.str("");
      strm.imbue(loc);
      strm << month;
      int m;
      strm >> m;

      std::string year = f;
      year.erase(0, year.rfind(".") + std::string(".").size());
      strm.clear();
      strm.str("");
      strm.imbue(loc);
      strm << year;
      int y;
      strm >> y;
      double jd1 = af.grigtojulian(d, m, y, 0, 0, 0);

      day = s;
      day = day.substr(0, day.find("."));
      strm.clear();
      strm.str("");
      strm.imbue(loc);
      strm << day;
      strm >> d;

      month = s;
      month.erase(0, month.find(".") + std::string(".").size());
      month = month.substr(0, month.find("."));
      strm.clear();
      strm.str("");
      strm.imbue(loc);
      strm << month;
      strm >> m;

      year = s;
      year.erase(0, year.rfind(".") + std::string(".").size());
      strm.clear();
      strm.str("");
      strm.imbue(loc);
      strm << year;
      strm >> y;
      double jd2 = af.grigtojulian(d, m, y, 0, 0, 0);

      return jd1 > jd2;
    });

  Gtk::Window *window = new Gtk::Window;
  window->set_application(Mwin->get_application());
  window->set_title(gettext("Instrument selection"));
  window->set_name("instrChooseWindow");
  window->set_transient_for(*Mwin);
  window->set_default_size(1, 1);

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::FILL);
  grid->set_valign(Gtk::Align::FILL);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
  label->set_text(gettext("Date:"));
  label->set_halign(Gtk::Align::START);
  label->set_margin(5);
  grid->attach(*label, 0, 0, 1, 1);

  Gtk::TreeModel::ColumnRecord record_date;
  Gtk::TreeModelColumn<Glib::ustring> date_col;
  record_date.add(date_col);
  Glib::RefPtr<Gtk::ListStore> model_dates = Gtk::ListStore::create(
      record_date);
  for(size_t i = 0; i < dates.size(); i++)
    {
      auto row = *(model_dates->append());
      row[date_col] = Glib::ustring(dates[i]);
    }

  Gtk::TreeView *tv_dates = Gtk::make_managed<Gtk::TreeView>();
  tv_dates->set_name("treeViewI");
  tv_dates->set_model(model_dates);
  tv_dates->append_column("", date_col);
  tv_dates->set_headers_visible(false);
  tv_dates->set_activate_on_single_click(true);

  Gtk::ScrolledWindow *scr_date = Gtk::make_managed<Gtk::ScrolledWindow>();
  scr_date->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
  scr_date->set_child(*tv_dates);

  Gtk::Popover *pop_dates = Gtk::make_managed<Gtk::Popover>();
  pop_dates->set_child(*scr_date);

  Gtk::MenuButton *men_dates = Gtk::make_managed<Gtk::MenuButton>();
  men_dates->set_margin(5);
  men_dates->set_popover(*pop_dates);
  men_dates->set_name("boards");
  auto row_date = model_dates->children()[0];
  if(row_date)
    {
      Glib::ustring val;
      row_date.get_value(0, val);
      men_dates->set_label(val);
    }
  grid->attach(*men_dates, 0, 1, 1, 1);

  if(row_date)
    {
      Gtk::TreeModel::Path p = model_dates->get_path(row_date.get_iter());
      tv_dates->set_cursor(p);
    }

  int x, y, w, h;
  Gtk::TreeViewColumn *columndate = tv_dates->get_column(0);
  columndate->cell_get_size(x, y, w, h);

  if(dates.size() > 20)
    {
      scr_date->set_min_content_height(20 * h);
    }
  else
    {
      scr_date->set_min_content_height(dates.size() * h);
    }

  Gtk::Requisition min, nat;
  tv_dates->get_preferred_size(min, nat);
  scr_date->set_min_content_width(nat.get_width());

  Gtk::Label *label2 = Gtk::make_managed<Gtk::Label>();
  label2->set_text(gettext("Mode:"));
  label2->set_halign(Gtk::Align::START);
  label2->set_margin(5);
  grid->attach(*label2, 0, 2, 1, 1);

  Gtk::ScrolledWindow *scrboard = Gtk::make_managed<Gtk::ScrolledWindow>();
  scrboard->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

  Gtk::TreeModel::ColumnRecord record_boards;
  Gtk::TreeModelColumn<Glib::ustring> boards_code;
  Gtk::TreeModelColumn<Glib::ustring> boards_col;
  record_boards.add(boards_code);
  record_boards.add(boards_col);
  std::vector<std::tuple<int, int, std::string>> files;
  Glib::RefPtr<Gtk::ListStore> model_boards = Gtk::ListStore::create(
      record_boards);
  if(row_date)
    {
      Glib::ustring val;
      row_date.get_value(0, val);
      af.homePath(&filename);
      filename = filename + "/Money/Deals/" + std::string(val) + ".zip";
      filepath = std::filesystem::u8path(filename);
      if(std::filesystem::exists(filepath))
	{
	  af.fileNames(filepath.u8string(), files);
	  for(size_t i = 0; i < boards.size(); i++)
	    {
	      std::string code = std::get<0>(boards[i]);
	      auto itf = std::find_if(files.begin(), files.end(), [code]
	      (auto &el)
		{
		  std::string val = std::get<2>(el);
		  std::string::size_type n;
		  n = val.find(code);
		  if(n != std::string::npos)
		    {
		      return true;
		    }
		  else
		    {
		      return false;
		    }
		});
	      if(itf != files.end())
		{
		  auto row = *(model_boards->append());
		  row[boards_code] = Glib::ustring(code);
		  row[boards_col] = Glib::ustring(std::get<1>(boards[i]));
		}
	    }
	}
    }

  Gtk::TreeView *tv_boards = Gtk::make_managed<Gtk::TreeView>();
  tv_boards->set_name("treeViewI");
  tv_boards->set_model(model_boards);
  tv_boards->append_column("", boards_col);
  tv_boards->set_headers_visible(false);
  tv_boards->set_activate_on_single_click(true);
  scrboard->set_child(*tv_boards);

  Gtk::Popover *boards_pop = Gtk::make_managed<Gtk::Popover>();
  boards_pop->set_child(*scrboard);

  Gtk::MenuButton *men_boards = Gtk::make_managed<Gtk::MenuButton>();
  men_boards->set_margin(5);
  men_boards->set_popover(*boards_pop);
  men_boards->set_name("boards");
  for(auto row : model_boards->children())
    {
      if(row)
	{
	  Glib::ustring val;
	  row.get_value(0, val);
	  if(val == "TQBR")
	    {
	      val.clear();
	      row.get_value(1, val);
	      men_boards->set_label(val);
	      Gtk::TreeModel::Path p = model_boards->get_path(row.get_iter());
	      tv_boards->set_cursor(p);
	      break;
	    }
	}
    }
  grid->attach(*men_boards, 0, 3, 1, 1);

  Gtk::TreeViewColumn *columnboard = tv_boards->get_column(0);
  columnboard->cell_get_size(x, y, w, h);
  if(model_boards->children().size() > 20)
    {
      scrboard->set_min_content_height(20 * h);
    }
  else
    {
      scrboard->set_min_content_height(model_boards->children().size() * h);
    }
  tv_boards->get_preferred_size(min, nat);
  scrboard->set_min_content_width(nat.get_width());

  Gtk::Label *label3 = Gtk::make_managed<Gtk::Label>();
  label3->set_text(gettext("Sort by:"));
  label3->set_halign(Gtk::Align::START);
  label3->set_margin(5);
  grid->attach(*label3, 0, 4, 1, 1);

  Gtk::ScrolledWindow *scrsort = Gtk::make_managed<Gtk::ScrolledWindow>();
  scrsort->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

  Gtk::TreeModel::ColumnRecord records;
  Gtk::TreeModelColumn<unsigned int> ids;
  Gtk::TreeModelColumn<Glib::ustring> sortcol;
  records.add(ids);
  records.add(sortcol);
  Glib::RefPtr<Gtk::ListStore> model_sort = Gtk::ListStore::create(records);
  auto rows = *(model_sort->append());
  rows[ids] = 0;
  rows[sortcol] = gettext("Size");
  rows = *(model_sort->append());
  rows[ids] = 1;
  rows[sortcol] = gettext("Name");
  rows = *(model_sort->append());
  rows[ids] = 2;
  rows[sortcol] = gettext("Codename");

  Gtk::TreeView *tv_sort = Gtk::make_managed<Gtk::TreeView>();
  tv_sort->set_name("treeViewI");
  tv_sort->set_model(model_sort);
  tv_sort->append_column("", sortcol);
  tv_sort->set_headers_visible(false);
  tv_sort->set_activate_on_single_click(true);
  scrsort->set_child(*tv_sort);

  Gtk::Popover *sort_pop = Gtk::make_managed<Gtk::Popover>();
  sort_pop->set_child(*scrsort);

  Gtk::MenuButton *men_sort = Gtk::make_managed<Gtk::MenuButton>();
  men_sort->set_margin(5);
  men_sort->set_popover(*sort_pop);
  men_sort->set_name("boards");
  auto row_sort = model_sort->children()[0];
  if(row_sort)
    {
      Gtk::TreeModel::Path p = model_sort->get_path(row_sort.get_iter());
      tv_sort->set_cursor(p);
      Glib::ustring val;
      row_sort.get_value(1, val);
      men_sort->set_label(val);
    }
  grid->attach(*men_sort, 0, 5, 1, 1);

  tv_sort->signal_row_activated().connect([men_sort, tv_sort, window]
  (const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column)
    {
      OpenDialogDeals opd(nullptr);
      opd.sortSelection(path, column, men_sort, tv_sort, window);
    });

  Gtk::TreeViewColumn *columnsort = tv_sort->get_column(0);
  columnsort->cell_get_size(x, y, w, h);
  scrsort->set_min_content_height(3 * h);
  tv_sort->get_preferred_size(min, nat);
  scrsort->set_min_content_width(nat.get_width());

  Gtk::Label *label4 = Gtk::make_managed<Gtk::Label>();
  label4->set_text(gettext("Instrument:"));
  label4->set_halign(Gtk::Align::START);
  label4->set_margin(5);
  grid->attach(*label4, 0, 6, 1, 1);

  Gtk::ScrolledWindow *scrinstr = Gtk::make_managed<Gtk::ScrolledWindow>();
  scrinstr->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

  Gtk::TreeModel::ColumnRecord record_instr;
  Gtk::TreeModelColumn<int> instr_index;
  Gtk::TreeModelColumn<Glib::ustring> instr_nm_col;
  record_instr.add(instr_index);
  record_instr.add(instr_nm_col);
  Glib::RefPtr<Gtk::ListStore> model_instr = Gtk::ListStore::create(
      record_instr);
  std::sort(files.begin(), files.end(), []
  (auto &el1, auto &el2)
    {
      return std::get<1>(el1) > std::get<1>(el2);
    });
  std::vector<std::tuple<int, std::string>> filelist;
  for(size_t i = 0; i < files.size(); i++)
    {
      int sz = std::get<1>(files[i]);
      if(sz > 0)
	{
	  std::tuple<int, std::string> ttup;
	  std::get<0>(ttup) = std::get<0>(files[i]);
	  std::string fnm = std::get<2>(files[i]);
	  std::string::size_type n;
	  n = fnm.find("TQBR");
	  if(n != std::string::npos)
	    {
	      std::filesystem::path p = std::filesystem::u8path(fnm);
	      fnm = p.stem().u8string();
	      auto itib = std::find_if(instrbase.begin(), instrbase.end(), [fnm]
	      (auto &el)
		{
		  return std::get<0>(el) == fnm;
		});
	      if(itib != instrbase.end())
		{
		  std::get<1>(ttup) = std::get<2>(*itib) + "-"
		      + std::get<0>(*itib);
		}
	      else
		{
		  std::get<1>(ttup) = p.stem().u8string();
		}
	      filelist.push_back(ttup);
	    }
	}
    }
  for(size_t i = 0; i < filelist.size(); i++)
    {
      auto row = *(model_instr->append());
      row[instr_index] = std::get<0>(filelist[i]);
      row[instr_nm_col] = Glib::ustring(std::get<1>(filelist[i]));
    }

  Gtk::TreeView *tv_instr = Gtk::make_managed<Gtk::TreeView>();
  tv_instr->set_name("treeViewI");
  tv_instr->set_model(model_instr);
  tv_instr->append_column("", instr_nm_col);
  tv_instr->set_headers_visible(false);
  tv_instr->set_activate_on_single_click(true);
  scrinstr->set_child(*tv_instr);

  Gtk::Popover *instr_pop = Gtk::make_managed<Gtk::Popover>();
  instr_pop->set_child(*scrinstr);

  Gtk::MenuButton *men_instr = Gtk::make_managed<Gtk::MenuButton>();
  men_instr->set_margin(5);
  men_instr->set_popover(*instr_pop);
  men_instr->set_name("boards");
  auto row_instr = model_instr->children()[0];
  if(row_instr)
    {
      Glib::ustring val;
      row_instr.get_value(1, val);
      men_instr->set_label(val);
      Gtk::TreeModel::Path p = model_instr->get_path(row_instr.get_iter());
      tv_instr->set_cursor(p);
    }
  grid->attach(*men_instr, 0, 7, 1, 1);

  tv_instr->signal_row_activated().connect([men_instr, tv_instr, window]
  (const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column)
    {
      OpenDialogDeals opd(nullptr);
      opd.instrSelection(path, column, men_instr, tv_instr, window);
    });

  Gtk::TreeViewColumn *columni = tv_instr->get_column(0);
  columni->cell_get_size(x, y, w, h);
  if(model_instr->children().size() > 20)
    {
      scrinstr->set_min_content_height(20 * h);
    }
  else
    {
      scrinstr->set_min_content_height(model_instr->children().size() * h);
    }
  tv_instr->get_preferred_size(min, nat);
  scrinstr->set_min_content_width(nat.get_width());

  tv_dates->signal_row_activated().connect(
      [men_dates, tv_dates, tv_boards, tv_sort, tv_instr, men_sort, men_instr,
       men_boards, window]
      (const Gtk::TreeModel::Path &path,
       Gtk::TreeViewColumn *column)
	 {
	   OpenDialogDeals opd(nullptr);
	   opd.dateSelection(path, column, men_dates, tv_dates, window);
	   opd.refreshBoards(tv_boards, tv_dates, men_boards, window);
	   opd.refreshSort(tv_sort, men_sort);
	   opd.refreshInstr(tv_instr, tv_dates, tv_boards, tv_sort, men_instr, window);
	 });

  tv_boards->signal_row_activated().connect(
      [men_boards, men_sort, men_instr, tv_boards, tv_instr, tv_dates, tv_sort,
       window]
      (const Gtk::TreeModel::Path &path,
       Gtk::TreeViewColumn *column)
	 {
	   OpenDialogDeals opd(nullptr);
	   opd.boardsSelection(path, column, men_boards, tv_boards, window);
	   opd.refreshSort(tv_sort, men_sort);
	   opd.refreshInstr(tv_instr, tv_dates, tv_boards, tv_sort, men_instr, window);
	 });

  tv_sort->signal_row_activated().connect(
      [men_sort, men_instr, tv_sort, tv_instr, tv_boards, tv_dates, window]
      (const Gtk::TreeModel::Path &path,
       Gtk::TreeViewColumn *column)
	 {
	   OpenDialogDeals opd(nullptr);
	   opd.sortSelection(path, column, men_sort, tv_sort, window);
	   opd.refreshInstr(tv_instr, tv_dates, tv_boards, tv_sort, men_instr, window);
	 });

  Gtk::Button *open = Gtk::make_managed<Gtk::Button>();
  open->set_label(gettext("Confirm"));
  open->set_halign(Gtk::Align::CENTER);
  open->set_margin(5);
  open->signal_clicked().connect([rec, variant, window, tv_dates, tv_instr]
  {
    OpenDialogDeals opd(nullptr);
    opd.openFile(window, rec, variant, tv_dates, tv_instr);
  });
  open->set_name("confirmButton");
  grid->attach(*open, 0, 8, 1, 1);

  Gtk::Button *close = Gtk::make_managed<Gtk::Button>();
  close->set_label(gettext("Cancel"));
  close->set_halign(Gtk::Align::CENTER);
  close->set_margin(5);
  close->signal_clicked().connect(sigc::mem_fun(*window, &Gtk::Window::close));
  close->set_name("closeButton");
  grid->attach(*close, 0, 9, 1, 1);

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
OpenDialogDeals::dateSelection(const Gtk::TreeModel::Path &path,
			       Gtk::TreeViewColumn *column,
			       Gtk::MenuButton *men_dates, Gtk::TreeView *view,
			       Gtk::Window *win)
{
  Glib::RefPtr<Gtk::TreeModel> model = view->get_model();
  const auto iter = model->get_iter(path);
  if(iter)
    {
      Glib::ustring val;
      iter->get_value(0, val);
      win->set_default_size(1, 1);
      men_dates->set_label(val);
      men_dates->popdown();
    }
}

void
OpenDialogDeals::boardsSelection(const Gtk::TreeModel::Path &path,
				 Gtk::TreeViewColumn *column,
				 Gtk::MenuButton *men_boards,
				 Gtk::TreeView *view, Gtk::Window *win)
{
  Glib::RefPtr<Gtk::TreeModel> model = view->get_model();
  const auto iter = model->get_iter(path);
  if(iter)
    {
      Glib::ustring val;
      iter->get_value(1, val);
      win->set_default_size(1, 1);
      men_boards->set_label(val);
      men_boards->popdown();
    }
}

void
OpenDialogDeals::refreshBoards(Gtk::TreeView *tv_boards,
			       Gtk::TreeView *tv_dates,
			       Gtk::MenuButton *men_boards, Gtk::Window *win)
{
  tv_boards->unset_model();
  tv_boards->remove_all_columns();
  men_boards->set_label("");
  AuxFunc af;
  std::string filename;
  af.homePath(&filename);
  auto sel_dates = tv_dates->get_selection();
  std::string date_val;
  if(sel_dates)
    {
      auto iter = sel_dates->get_selected();
      if(iter)
	{
	  Glib::ustring val;
	  iter->get_value(0, val);
	  date_val = std::string(val);
	}
    }
  filename = filename + "/Money/Deals/" + date_val + ".zip";
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  if(std::filesystem::exists(filepath))
    {
      std::vector<std::tuple<std::string, std::string>> boards;
      DownloadMenu dm(nullptr);
      boards = dm.formVectorBoards();
      std::vector<std::tuple<int, int, std::string>> files;
      af.fileNames(filepath.u8string(), files);

      Gtk::TreeModel::ColumnRecord record_boards;
      Gtk::TreeModelColumn<Glib::ustring> boards_code;
      Gtk::TreeModelColumn<Glib::ustring> boards_col;
      record_boards.add(boards_code);
      record_boards.add(boards_col);
      Glib::RefPtr<Gtk::ListStore> model_boards = Gtk::ListStore::create(
	  record_boards);

      for(size_t i = 0; i < boards.size(); i++)
	{
	  std::string code = std::get<0>(boards[i]);
	  auto itf = std::find_if(files.begin(), files.end(), [code]
	  (auto &el)
	    {
	      std::string val = std::get<2>(el);
	      std::string::size_type n;
	      n = val.find(code);
	      if(n != std::string::npos)
		{
		  return true;
		}
	      else
		{
		  return false;
		}
	    });
	  if(itf != files.end())
	    {
	      auto row = *(model_boards->append());
	      row[boards_code] = Glib::ustring(code);
	      row[boards_col] = Glib::ustring(std::get<1>(boards[i]));
	    }
	}

      tv_boards->set_model(model_boards);
      tv_boards->append_column("", boards_col);
      tv_boards->set_headers_visible(false);
      tv_boards->set_activate_on_single_click(true);

      win->set_default_size(1, 1);
      for(auto &row : model_boards->children())
	{
	  if(row)
	    {
	      Glib::ustring val;
	      row.get_value(0, val);
	      if(val == "TQBR")
		{
		  val.clear();
		  row.get_value(1, val);
		  men_boards->set_label(val);
		  Gtk::TreeModel::Path p = model_boards->get_path(
		      row.get_iter());
		  tv_boards->set_cursor(p);
		}
	    }
	}
    }
}

void
OpenDialogDeals::refreshSort(Gtk::TreeView *tv_sort, Gtk::MenuButton *men_sort)
{
  men_sort->set_label("");
  Glib::RefPtr<Gtk::TreeModel> model_sort = tv_sort->get_model();
  if(model_sort)
    {
      auto row = model_sort->children()[0];
      if(row)
	{
	  Glib::ustring val;
	  row.get_value(1, val);
	  men_sort->set_label(val);
	  Gtk::TreeModel::Path p = model_sort->get_path(row.get_iter());
	  tv_sort->set_cursor(p);
	}
    }
}

void
OpenDialogDeals::sortSelection(const Gtk::TreeModel::Path &path,
			       Gtk::TreeViewColumn *column,
			       Gtk::MenuButton *men_sort, Gtk::TreeView *view,
			       Gtk::Window *win)
{
  Glib::RefPtr<Gtk::TreeModel> model = view->get_model();
  const auto iter = model->get_iter(path);
  if(iter)
    {
      Glib::ustring val;
      iter->get_value(1, val);
      win->set_default_size(1, 1);
      men_sort->set_label(val);
      men_sort->popdown();
    }
}

void
OpenDialogDeals::instrSelection(const Gtk::TreeModel::Path &path,
				Gtk::TreeViewColumn *column,
				Gtk::MenuButton *men_instr, Gtk::TreeView *view,
				Gtk::Window *win)
{
  Glib::RefPtr<Gtk::TreeModel> model = view->get_model();
  const auto iter = model->get_iter(path);
  if(iter)
    {
      Glib::ustring val;
      iter->get_value(1, val);
      win->set_default_size(1, 1);
      men_instr->set_label(val);
      men_instr->popdown();
    }
}

void
OpenDialogDeals::refreshInstr(Gtk::TreeView *tv_instr, Gtk::TreeView *tv_dates,
			      Gtk::TreeView *tv_boards, Gtk::TreeView *tv_sort,
			      Gtk::MenuButton *men_instr, Gtk::Window *win)
{
  tv_instr->unset_model();
  tv_instr->remove_all_columns();
  men_instr->set_label("");
  AuxFunc af;
  std::string filename;
  af.homePath(&filename);
  auto sel_dates = tv_dates->get_selection();
  std::string date_val;
  if(sel_dates)
    {
      auto iter = sel_dates->get_selected();
      if(iter)
	{
	  Glib::ustring val;
	  iter->get_value(0, val);
	  date_val = std::string(val);
	}
    }

  filename = filename + "/Money/Deals/" + date_val + ".zip";
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  if(std::filesystem::exists(filepath))
    {
      std::vector<std::tuple<int, int, std::string>> files;
      std::string line, archnm;
      af.fileNames(filename, files);
      files.erase(std::remove_if(files.begin(), files.end(), []
      (auto &el)
	{
	  return std::get<1>(el) == 0;
	}),
		  files.end());

      auto sel_boards = tv_boards->get_selection();
      if(sel_boards)
	{
	  auto iter = sel_boards->get_selected();
	  if(iter)
	    {
	      Glib::ustring val;
	      iter->get_value(0, val);
	      std::string sv(val);
	      files.erase(std::remove_if(files.begin(), files.end(), [sv]
	      (auto &el)
		{
		  std::string cv = std::get<2>(el);
		  std::string::size_type n;
		  n = cv.find(sv);
		  if(n == std::string::npos)
		    {
		      return true;
		    }
		  else
		    {
		      return false;
		    }
		}),
			  files.end());
	    }
	}

      unsigned int sorting = 0;
      auto sel_sort = tv_sort->get_selection();
      if(sel_sort)
	{
	  auto iter = sel_sort->get_selected();
	  if(iter)
	    {
	      iter->get_value(0, sorting);
	    }
	}

      std::vector<std::tuple<std::string, std::string, std::string>> instrbase;
      std::vector<std::tuple<std::string, std::string, std::string>> instrbasef;
      DownloadMenu dm(nullptr);
      instrbase = dm.formVectorInstr(1);
      instrbasef = dm.formVectorInstr(2);
      std::copy(instrbasef.begin(), instrbasef.end(),
		std::back_inserter(instrbase));
      instrbasef.clear();

      std::vector<std::tuple<int, int, std::string, std::string>> filelist;
      for(size_t i = 0; i < files.size(); i++)
	{
	  std::tuple<int, int, std::string, std::string> ttup;
	  std::get<0>(ttup) = std::get<0>(files[i]);
	  std::get<1>(ttup) = std::get<1>(files[i]);
	  std::filesystem::path p = std::filesystem::u8path(
	      std::get<2>(files[i]));
	  std::string fnm = p.stem().u8string();
	  auto itib = std::find_if(instrbase.begin(), instrbase.end(), [fnm]
	  (auto &el)
	    {
	      return std::get<0>(el) == fnm;
	    });
	  if(itib != instrbase.end())
	    {
	      std::get<2>(ttup) = std::get<2>(*itib);
	    }
	  else
	    {
	      std::get<2>(ttup) = p.stem().u8string();
	    }
	  std::get<3>(ttup) = fnm;
	  filelist.push_back(ttup);
	}
      files.clear();

      if(sorting == 0)
	{
	  std::sort(filelist.begin(), filelist.end(), []
	  (auto &el1, auto &el2)
	    {
	      return std::get<1>(el1) > std::get<1>(el2);
	    });
	}
      else if(sorting == 1)
	{
	  std::sort(filelist.begin(), filelist.end(), [&af]
	  (auto &el1, auto &el2)
	    {
	      std::string f = std::get<2>(el1);
	      std::string s = std::get<2>(el2);
	      f.erase(std::remove_if(f.begin(), f.end(), [](auto &el)
			{
			  return el == '\"';
			}), f.end());
	      s.erase(std::remove_if(s.begin(), s.end(), [](auto &el)
			{
			  return el == '\"';
			}), s.end());
	      while(*(f.begin()) == ' ')
		{
		  f.erase(f.begin());
		}
	      while(*(s.begin()) == ' ')
		{
		  s.erase(s.begin());
		}
	      af.stringToLower(f);
	      af.stringToLower(s);
	      return f < s;
	    });
	}
      else if(sorting == 2)
	{
	  std::sort(filelist.begin(), filelist.end(), [&af]
	  (auto &el1, auto &el2)
	    {
	      std::string f = std::get<3>(el1);
	      std::string s = std::get<3>(el2);
	      af.stringToLower(f);
	      af.stringToLower(s);
	      return f < s;
	    });
	}

      Gtk::TreeModel::ColumnRecord record_instr;
      Gtk::TreeModelColumn<int> index;
      Gtk::TreeModelColumn<Glib::ustring> instrument;
      record_instr.add(index);
      record_instr.add(instrument);
      Glib::RefPtr<Gtk::ListStore> model_instr = Gtk::ListStore::create(
	  record_instr);

      for(size_t i = 0; i < filelist.size(); i++)
	{
	  auto row = *(model_instr->append());
	  row[index] = std::get<0>(filelist[i]);
	  row[instrument] = std::get<2>(filelist[i]) + "-"
	      + std::get<3>(filelist[i]);
	}

      tv_instr->set_model(model_instr);
      tv_instr->append_column("", instrument);
      tv_instr->set_headers_visible(false);
      tv_instr->set_activate_on_single_click(true);

      auto row_instr = model_instr->children()[0];
      if(row_instr)
	{
	  Glib::ustring val;
	  row_instr.get_value(1, val);
	  men_instr->set_label(val);
	  Gtk::TreeModel::Path p = model_instr->get_path(row_instr.get_iter());
	  tv_instr->set_cursor(p);
	}
    }
}

void
OpenDialogDeals::openFile(Gtk::Window *win, Gdk::Rectangle rec, int variant,
			  Gtk::TreeView *tv_dates, Gtk::TreeView *tv_instr)
{
  auto sel_dates = tv_dates->get_selection();
  if(sel_dates)
    {
      auto iter_dates = sel_dates->get_selected();
      if(iter_dates)
	{
	  Glib::ustring val;
	  iter_dates->get_value(0, val);
	  std::string date(val);
	  AuxFunc af;
	  std::string openfile;
	  af.homePath(&openfile);
	  openfile = openfile + "/Money/Deals/" + date + ".zip";
	  std::filesystem::path filepath = std::filesystem::u8path(openfile);
	  if(std::filesystem::exists(filepath))
	    {
	      auto sel_instr = tv_instr->get_selection();
	      if(sel_instr)
		{
		  auto iter_instr = sel_instr->get_selected();
		  if(iter_instr)
		    {
		      int index;
		      iter_instr->get_value(0, index);
		      std::string outfolder = af.tempPath();
		      outfolder = outfolder + "/Money/DealsToShow";
		      std::filesystem::path out = std::filesystem::u8path(
			  outfolder);
		      if(std::filesystem::exists(out))
			{
			  std::filesystem::remove_all(out);
			}
		      af.unpackByIndex(openfile, outfolder, index);
		      if(std::filesystem::exists(out))
			{
			  for(auto &dirit : std::filesystem::directory_iterator(
			      out))
			    {
			      std::filesystem::path p = dirit.path();
			      if(!std::filesystem::is_directory(p))
				{
				  GraphicWidget *grw = new GraphicWidget(
				      rec.get_width(), rec.get_height(), win,
				      p.u8string());
				  grw->plot(1, variant, date);
				  break;
				}
			    }
			}
		    }
		}
	    }
	}
    }
}
