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

#include "OpenDialogDeals.h"

OpenDialogDeals::OpenDialogDeals (Glib::RefPtr<Gtk::Application> app,
				  Gtk::Window *mwin, std::string *opendate)
{
  App = app;
  Mwin = mwin;
  Opendate = opendate;
  css_provider = Gtk::CssProvider::create ();
  AuxFunc af;
  std::filesystem::path p = std::filesystem::u8path (af.get_selfpath ());
  css_provider->load_from_path (
      Glib::ustring (
	  std::string (p.parent_path ().u8string ())
	      + "/../share/Money/openDialogDeals.css"));
  std::string filename;
  af.homePath (&filename);
  filename = filename + "/.Money/BoardsList";
  std::filesystem::path filepath = std::filesystem::u8path (filename);
  std::fstream f;
  f.open (filepath, std::ios_base::in);
  int count = 0;

  if (f.is_open ())
    {

      while (!f.eof ())
	{
	  std::string line;
	  getline (f, line);
	  af.cp1251toutf8 (line);
	  if (count > 2 && line != "")
	    {
	      std::pair<Glib::ustring, Glib::ustring> bp;
	      std::string tmp;
	      tmp = line;
	      tmp.erase (0, tmp.find (";") + std::string (";").size ());
	      tmp.erase (0, tmp.find (";") + std::string (";").size ());
	      tmp = tmp.substr (0, tmp.find (";"));
	      bp.first = tmp;
	      tmp = line;
	      tmp.erase (0, tmp.find (";") + std::string (";").size ());
	      tmp.erase (0, tmp.find (";") + std::string (";").size ());
	      tmp.erase (0, tmp.find (";") + std::string (";").size ());
	      tmp = tmp.substr (0, tmp.find (";"));
	      bp.second = tmp;
	      boards.push_back (bp);
	    }
	  count++;
	}
      f.close ();
    }
  af.homePath (&filename);
  filename = filename + "/Money/Deals";
  filepath = std::filesystem::u8path (filename);
  if (std::filesystem::exists (filepath))
    {
      for (auto &it : std::filesystem::recursive_directory_iterator (filepath))
	{
	  std::filesystem::path pp = it.path ();
	  if (!std::filesystem::is_directory (pp))
	    {
	      std::string p = pp.filename ().u8string ();
	      std::string::size_type n;
	      n = p.find (".zip", 0);
	      p.erase (n, std::string (".zip").size ());
	      dates.push_back (p);
	    }
	}
      std::sort (dates.begin (), dates.end (), [&af]
      (std::string &e1, std::string &e2)
	{ std::string tmp = e1;
	  int date1, date2;
	  int day, month, year;
	  tmp = tmp.substr(0, tmp.find ("."));
	  day = std::stoi(tmp);
	  tmp = e1;
	  tmp.erase(0, tmp.find (".") + std::string (".").size ());
	  tmp = tmp.substr(0, tmp.find ("."));
	  month = std::stoi(tmp);
	  tmp = e1;
	  tmp.erase(0, tmp.find (".") + std::string (".").size ());
	  tmp.erase(0, tmp.find (".") + std::string (".").size ());
	  year = std::stoi(tmp);
	  date1 = af.grigtojulian(day, month, year);

	  tmp = e2;
	  tmp = tmp.substr(0, tmp.find ("."));
	  day = std::stoi(tmp);
	  tmp = e2;
	  tmp.erase(0, tmp.find (".") + std::string (".").size ());
	  tmp = tmp.substr(0, tmp.find ("."));
	  month = std::stoi(tmp);
	  tmp = e2;
	  tmp.erase(0, tmp.find (".") + std::string (".").size ());
	  tmp.erase(0, tmp.find (".") + std::string (".").size ());
	  year = std::stoi(tmp);
	  date2 = af.grigtojulian(day, month, year);
	  return date1 > date2;});
      af.homePath (&filename);
      if (dates.size () > 0)
	{
	  filename = filename + "/Money/Deals/" + dates[selecteddate] + ".zip";
	}

      std::vector<std::tuple<int, int, std::string>> names;
      std::vector<std::string> folders;
      std::string line, archnm;
      af.fileNames (filename, names);
      for (size_t i = 0; i < names.size (); i++)
	{

	  line = std::get<2> (names[i]);
	  if (std::get<1> (names[i]) == 0)
	    {
	      folders.push_back (line);
	    }
	  else
	    {
	      filelist.push_back (names[i]);
	    }
	}
      for (size_t i = 0; i < boards.size (); i++)
	{
	  line = boards[i].first;
	  auto iter = std::find_if (folders.begin (), folders.end (), [&line]
	  (auto &el)
	    {
	      std::string::size_type n;
	      n = el.find (line, 0);
	      if (n != std::string::npos)
		{
		  return true;
		}
	      else
		{
		  return false;
		}
	    });
	  if (iter != folders.end ())
	    {
	      boardstosh.push_back (boards[i]);
	    }
	}
      auto iter = std::find_if (boardstosh.begin (), boardstosh.end (), []
      (auto &e)
	{ return e.first == "TQBR";});
      if (iter != boardstosh.end ())
	{
	  selectedboard = std::distance (boardstosh.begin (), iter);
	}
      filelist.erase (std::remove_if (filelist.begin (), filelist.end (), [this]
      (auto &el)
	{ std::string l = this->boardstosh[this->selectedboard].first;
	  std::string::size_type n;
	  n = (std::get<2>(el)).find(l, 0);
	  if(n!=std::string::npos)
	    {
	      return false;
	    }
	  else
	    {
	      return true;
	    }}),
		      filelist.end ());
      std::sort (filelist.begin (), filelist.end (), []
      (auto &el1, auto &el2)
	{
	  return std::get<1>(el1) > std::get<1>(el2);
	});
      for (size_t i = 0; i < filelist.size (); i++)
	{
	  std::filesystem::path p = std::filesystem::u8path (
	      std::get<2> (filelist[i]));
	  line = p.filename ().u8string ();
	  int one, two;
	  one = std::get<0> (filelist[i]);
	  two = std::get<1> (filelist[i]);
	  filelist[i] = std::make_tuple (one, two, line);
	}
      createDialog ();
    }
}

OpenDialogDeals::~OpenDialogDeals ()
{
  // TODO Auto-generated destructor stub
}

void
OpenDialogDeals::createDialog ()
{
  Gtk::Window *window = new Gtk::Window;
  window->set_title (gettext ("Instrument selection"));
  window->set_name ("windowD");
  window->set_transient_for (*Mwin);
  window->set_modal (true);
  window->get_style_context ()->add_provider (css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);
  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid> ();
  grid->set_halign (Gtk::Align::CENTER);
  window->set_child (*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label> ();
  label->set_text (gettext ("Date:"));
  label->set_halign (Gtk::Align::START);
  label->set_margin (5);
  grid->attach (*label, 0, 0, 1, 1);

  Gtk::Expander *datesexp = Gtk::make_managed<Gtk::Expander> ();
  if (dates.size () > 0)
    {
      datesexp->set_label (Glib::ustring (dates[0]));
    }
  Gtk::Widget *widg = datesexp->get_label_widget ();
  widg->set_margin (5);
  datesexp->set_margin (5);
  datesexp->set_name ("Exp");
  datesexp->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  datesexp->set_valign (Gtk::Align::CENTER);

  Gtk::ScrolledWindow *scrdate = Gtk::make_managed<Gtk::ScrolledWindow> ();
  scrdate->set_expand ();
  scrdate->set_policy (Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
  datesexp->set_child (*scrdate);

  Gtk::TreeModel::ColumnRecord recordd;
  Gtk::TreeModelColumn<unsigned int> idd;
  Gtk::TreeModelColumn<Glib::ustring> datecol;
  recordd.add (idd);
  recordd.add (datecol);

  Glib::RefPtr<Gtk::ListStore> modeld = Gtk::ListStore::create (recordd);
  Gtk::TreeView *dateview = Gtk::make_managed<Gtk::TreeView> ();
  dateview->set_name ("View");
  dateview->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  scrdate->set_child (*dateview);

  grid->attach (*datesexp, 0, 1, 1, 1);

  Gtk::Label *label2 = Gtk::make_managed<Gtk::Label> ();
  label2->set_text (gettext ("Mode:"));
  label2->set_halign (Gtk::Align::START);
  label2->set_margin (5);
  grid->attach (*label2, 0, 2, 1, 1);

  Gtk::Expander *boardexp = Gtk::make_managed<Gtk::Expander> ();
  boardexp->set_name ("Exp");
  boardexp->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  if (boardstosh.size () > 0)
    {
      boardexp->set_label (
	  Glib::ustring (
	      boardstosh[selectedboard].second + "-"
		  + boardstosh[selectedboard].first));
    }
  Gtk::Widget *widg2 = boardexp->get_label_widget ();
  widg2->set_margin (5);
  boardexp->set_margin (5);
  boardexp->set_valign (Gtk::Align::CENTER);

  Gtk::ScrolledWindow *scrboard = Gtk::make_managed<Gtk::ScrolledWindow> ();
  scrboard->set_expand ();
  scrboard->set_policy (Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
  boardexp->set_child (*scrboard);

  Gtk::TreeModel::ColumnRecord recordb;
  Gtk::TreeModelColumn<unsigned int> idb;
  Gtk::TreeModelColumn<Glib::ustring> boardcol;
  recordb.add (idb);
  recordb.add (boardcol);

  Glib::RefPtr<Gtk::ListStore> modelb = Gtk::ListStore::create (recordb);
  Gtk::TreeView *boardview = Gtk::make_managed<Gtk::TreeView> ();
  boardview->set_name ("View");
  boardview->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  scrboard->set_child (*boardview);

  grid->attach (*boardexp, 0, 3, 1, 1);

  Gtk::Label *label3 = Gtk::make_managed<Gtk::Label> ();
  label3->set_text (gettext ("Sort by:"));
  label3->set_halign (Gtk::Align::START);
  label3->set_margin (5);
  grid->attach (*label3, 0, 4, 1, 1);

  Gtk::Expander *sortexp = Gtk::make_managed<Gtk::Expander> ();
  sortexp->set_name ("Exp");
  sortexp->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  sortexp->set_label (gettext ("Size"));
  Gtk::Widget *widg3 = sortexp->get_label_widget ();
  widg3->set_margin (5);
  sortexp->set_margin (5);
  sortexp->set_valign (Gtk::Align::CENTER);

  Gtk::ScrolledWindow *scrsort = Gtk::make_managed<Gtk::ScrolledWindow> ();
  scrsort->set_expand ();
  scrsort->set_policy (Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
  sortexp->set_child (*scrsort);

  Gtk::TreeModel::ColumnRecord records;
  Gtk::TreeModelColumn<unsigned int> ids;
  Gtk::TreeModelColumn<Glib::ustring> sortcol;
  records.add (ids);
  records.add (sortcol);

  Glib::RefPtr<Gtk::ListStore> models = Gtk::ListStore::create (records);
  Gtk::TreeView *sortview = Gtk::make_managed<Gtk::TreeView> ();
  sortview->set_name ("View");
  sortview->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

  scrsort->set_child (*sortview);

  grid->attach (*sortexp, 0, 5, 1, 1);

  Gtk::Label *label4 = Gtk::make_managed<Gtk::Label> ();
  label4->set_text (gettext ("Instrument:"));
  label4->set_halign (Gtk::Align::START);
  label4->set_margin (5);
  grid->attach (*label4, 0, 6, 1, 1);

  Gtk::Expander *instrexp = Gtk::make_managed<Gtk::Expander> ();
  instrexp->set_name ("Exp");
  instrexp->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  if (filelist.size () > 0)
    {
      std::string line = std::get<2> (filelist[0]);
      std::string auxstr = line;
      line.erase (0, line.find ("-") + std::string ("-").size ());
      line = line + "-" + auxstr.substr (0, auxstr.find ("-"));
      instrexp->set_label (Glib::ustring (line));
    }
  Gtk::Widget *widg4 = instrexp->get_label_widget ();
  widg4->set_margin (5);
  instrexp->set_margin (5);
  instrexp->set_valign (Gtk::Align::CENTER);

  Gtk::ScrolledWindow *scrinstr = Gtk::make_managed<Gtk::ScrolledWindow> ();
  scrinstr->set_expand ();
  scrinstr->set_policy (Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
  instrexp->set_child (*scrinstr);

  Gtk::TreeModel::ColumnRecord recordi;
  Gtk::TreeModelColumn<unsigned int> idi;
  Gtk::TreeModelColumn<Glib::ustring> icol;
  recordi.add (idi);
  recordi.add (icol);

  Glib::RefPtr<Gtk::ListStore> modeli = Gtk::ListStore::create (recordi);
  Gtk::TreeView *iview = Gtk::make_managed<Gtk::TreeView> ();
  iview->set_name ("View");
  iview->get_style_context ()->add_provider (css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);

  scrinstr->set_child (*iview);

  grid->attach (*instrexp, 0, 7, 1, 1);

  Gtk::Button *open = Gtk::make_managed<Gtk::Button> ();
  open->set_label (gettext ("Confirm"));
  open->set_halign (Gtk::Align::CENTER);
  open->set_margin (5);
  open->signal_clicked ().connect (
      sigc::mem_fun (*this, &OpenDialogDeals::openFile));
  open->signal_clicked ().connect (
      sigc::mem_fun (*window, &Gtk::Window::close));
  open->set_name ("openButton");
  open->get_style_context ()->add_provider (css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);
  grid->attach (*open, 0, 8, 1, 1);

  Gtk::Button *close = Gtk::make_managed<Gtk::Button> ();
  close->set_label (gettext ("Cancel"));
  close->set_halign (Gtk::Align::CENTER);
  close->set_margin (5);
  close->signal_clicked ().connect (
      sigc::mem_fun (*window, &Gtk::Window::close));
  close->set_name ("closeButton");
  close->get_style_context ()->add_provider (css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);
  grid->attach (*close, 0, 9, 1, 1);

  window->signal_close_request ().connect ( [window, this]
  {
    window->hide ();
    this->deleteSign.emit ();
    delete window;
    return true;
  },
					   false);

  for (size_t i = 0; i < dates.size (); i++)
    {
      auto row = *(modeld->append ());
      row[idd] = i;
      row[datecol] = Glib::ustring (dates[i]);
    }
  for (size_t i = 0; i < boardstosh.size (); i++)
    {
      auto row = *(modelb->append ());
      row[idb] = i;
      row[boardcol] = boardstosh[i].second + "-" + boardstosh[i].first;
    }
  idbb = idb;
  boardcolb = boardcol;

  auto rows = *(models->append ());
  rows[ids] = 0;
  rows[sortcol] = gettext ("Size");
  rows = *(models->append ());
  rows[ids] = 1;
  rows[sortcol] = gettext ("Name");
  rows = *(models->append ());
  rows[ids] = 2;
  rows[sortcol] = gettext ("Codename");
  idss = ids;
  sortcols = sortcol;
  for (size_t i = 0; i < filelist.size (); i++)
    {
      auto row = *(modeli->append ());
      row[idi] = i;
      std::string line = std::get<2> (filelist[i]);
      std::string auxstr = line;
      line.erase (0, line.find ("-") + std::string ("-").size ());
      line = line + "-" + auxstr.substr (0, auxstr.find ("-"));
      row[icol] = Glib::ustring (line);
    }
  idii = idi;
  icoli = icol;

  Gtk::TreeViewColumn *columndat = Gtk::make_managed<Gtk::TreeViewColumn> ();
  dateview->set_model (modeld);
  columndat->pack_start (datecol);
  dateview->append_column (*columndat);
  dateview->set_headers_visible (false);
  dateview->set_activate_on_single_click (true);
  datesexp->property_expanded ().signal_changed ().connect (
      sigc::bind (sigc::mem_fun (*this, &OpenDialogDeals::expandScroll),
		  datesexp, window, grid));

  Gtk::TreeViewColumn *columnboard = Gtk::make_managed<Gtk::TreeViewColumn> ();
  boardview->set_model (modelb);
  columnboard->pack_start (boardcol);
  boardview->append_column (*columnboard);
  boardview->set_headers_visible (false);
  boardview->set_activate_on_single_click (true);
  boardexp->property_expanded ().signal_changed ().connect (
      sigc::bind (sigc::mem_fun (*this, &OpenDialogDeals::expandScroll),
		  boardexp, window, grid));

  Gtk::TreeViewColumn *columnsort = Gtk::make_managed<Gtk::TreeViewColumn> ();
  sortview->set_model (models);
  columnsort->pack_start (sortcol);
  sortview->append_column (*columnsort);
  sortview->set_headers_visible (false);
  sortview->set_activate_on_single_click (true);
  sortexp->property_expanded ().signal_changed ().connect (
      sigc::bind (sigc::mem_fun (*this, &OpenDialogDeals::expandScroll),
		  sortexp, window, grid));

  Gtk::TreeViewColumn *columni = Gtk::make_managed<Gtk::TreeViewColumn> ();
  iview->set_model (modeli);
  columni->pack_start (icol);
  iview->append_column (*columni);
  iview->set_headers_visible (false);
  iview->set_activate_on_single_click (true);
  instrexp->property_expanded ().signal_changed ().connect (
      sigc::bind (sigc::mem_fun (*this, &OpenDialogDeals::expandScroll),
		  instrexp, window, grid));
  dateview->signal_row_activated ().connect (
      sigc::bind (sigc::mem_fun (*this, &OpenDialogDeals::dateSelection),
		  modeld, idd, datesexp));
  boardview->signal_row_activated ().connect (
      sigc::bind (sigc::mem_fun (*this, &OpenDialogDeals::boardsSelection),
		  boardexp, boardview));
  this->refreshboards.connect (
      sigc::bind (sigc::mem_fun (*this, &OpenDialogDeals::refreshBoards),
		  boardview, columnboard, boardexp));
  this->refreshsort.connect (
      sigc::bind (sigc::mem_fun (*this, &OpenDialogDeals::refreshSort),
		  sortexp));
  sortview->signal_row_activated ().connect (
      sigc::bind (sigc::mem_fun (*this, &OpenDialogDeals::sortSelection),
		  sortexp, sortview));
  iview->signal_row_activated ().connect (
      sigc::bind (sigc::mem_fun (*this, &OpenDialogDeals::instrSelection),
		  instrexp, iview));
  this->refreshinstr.connect (
      sigc::bind (sigc::mem_fun (*this, &OpenDialogDeals::refreshInstr), iview,
		  columni, instrexp));

  int x, y, h, w;
  columndat->cell_get_size (x, y, w, h);

  if (dates.size () > 20)
    {
      scrdate->set_min_content_height (20 * h);
    }
  else
    {
      scrdate->set_min_content_height (dates.size () * h);
    }

  columnboard->cell_get_size (x, y, w, h);
  if (boardstosh.size () > 20)
    {
      scrboard->set_min_content_height (20 * h);
    }
  else
    {
      scrboard->set_min_content_height (boardstosh.size () * h);
    }

  columnsort->cell_get_size (x, y, w, h);
  scrsort->set_min_content_height (3 * h);

  columni->cell_get_size (x, y, w, h);
  if (filelist.size () > 20)
    {
      scrinstr->set_min_content_height (20 * h);
    }
  else
    {
      scrinstr->set_min_content_height (filelist.size () * h);
    }

  Gtk::Requisition rq1, rq2;
  instrexp->get_preferred_size (rq1, rq2);
  scrdate->set_max_content_width (rq2.get_width ());
  scrboard->set_max_content_width (rq2.get_width ());
  scrsort->set_max_content_width (rq2.get_width ());
  scrinstr->set_max_content_width (rq2.get_width ());
  window->set_default_size (rq2.get_width () + 10, -1);
  window->set_application (App);
  window->present ();
}

void
OpenDialogDeals::expandScroll (Gtk::Expander *exp, Gtk::Window *window,
			       Gtk::Grid *grid)
{
  if (!exp->property_expanded ().get_value ())
    {
      Gtk::Requisition rq1, rq2;
      exp->get_preferred_size (rq1, rq2);
      window->set_default_size (rq2.get_width (), -1);
    }
}

void
OpenDialogDeals::dateSelection (const Gtk::TreeModel::Path &path,
				Gtk::TreeViewColumn *column,
				Glib::RefPtr<Gtk::ListStore> model,
				Gtk::TreeModelColumn<unsigned int> id,
				Gtk::Expander *exp)
{
  const auto iter = model->get_iter (path);
  if (iter)
    {
      auto row = *iter;
      selecteddate = row[id];
      exp->set_label (dates[row[id]]);
      Gtk::Widget *widg = exp->get_label_widget ();
      widg->set_margin (5);
      exp->set_expanded (false);
    }
  refreshboards.emit ();
  refreshsort.emit ();
  refreshinstr.emit ();
}

void
OpenDialogDeals::boardsSelection (const Gtk::TreeModel::Path &path,
				  Gtk::TreeViewColumn *column,
				  Gtk::Expander *exp, Gtk::TreeView *view)
{
  Glib::RefPtr<Gtk::TreeModel> model = view->get_model ();
  const auto iter = model->get_iter (path);
  if (iter)
    {
      auto row = *iter;
      selectedboard = row[idbb];
      exp->set_label (boardstosh[selectedboard].second);
      Gtk::Widget *widg = exp->get_label_widget ();
      widg->set_margin (5);
      exp->set_expanded (false);
    }
  refreshsort.emit ();
  refreshinstr.emit ();
}

void
OpenDialogDeals::refreshBoards (Gtk::TreeView *view,
				Gtk::TreeViewColumn *column, Gtk::Expander *exp)
{
  boardstosh.clear ();
  AuxFunc af;
  std::string filename;
  af.homePath (&filename);
  if (dates.size () > 0)
    {
      filename = filename + "/Money/Deals/" + dates[selecteddate] + ".zip";
    }

  std::vector<std::tuple<int, int, std::string>> names;
  std::vector<std::string> folders;
  std::string line, archnm;
  af.fileNames (filename, names);
  for (size_t i = 0; i < names.size (); i++)
    {

      line = std::get<2> (names[i]);
      if (std::get<1> (names[i]) == 0)
	{
	  folders.push_back (line);
	}
    }
  for (size_t i = 0; i < boards.size (); i++)
    {
      line = boards[i].first;
      auto iter = std::find_if (folders.begin (), folders.end (), [&line]
      (auto &el)
	{ std::string::size_type n;
	  n = el.find (line, 0);
	  if (n != std::string::npos)
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }});
      if (iter != folders.end ())
	{
	  boardstosh.push_back (boards[i]);
	}
    }
  auto iter = std::find_if (boardstosh.begin (), boardstosh.end (), []
  (auto &e)
    { return e.first == "TQBR";});
  if (iter != boardstosh.end ())
    {
      selectedboard = std::distance (boardstosh.begin (), iter);
    }
  view->unset_model ();
  column->clear ();
  Gtk::TreeModel::ColumnRecord record;
  Gtk::TreeModelColumn<unsigned int> id;
  Gtk::TreeModelColumn<Glib::ustring> instrument;
  record.add (id);
  record.add (instrument);
  Glib::RefPtr<Gtk::ListStore> model = Gtk::ListStore::create (record);

  for (size_t i = 0; i < boardstosh.size (); i++)
    {
      auto row = *(model->append ());
      row[id] = i;
      row[instrument] = boardstosh[i].second + "-" + boardstosh[i].first;
    }
  idbb = id;
  boardcolb = instrument;
  view->set_model (model);
  column->pack_start (boardcolb);
  view->append_column (*column);
  view->set_headers_visible (false);
  view->set_activate_on_single_click (true);

  if (boardstosh.size () > 0)
    {
      exp->set_label (
	  Glib::ustring (
	      boardstosh[selectedboard].second + "-"
		  + boardstosh[selectedboard].first));
    }
  Gtk::Widget *widg = exp->get_label_widget ();
  widg->set_margin (5);
}

void
OpenDialogDeals::refreshSort (Gtk::Expander *exp)
{
  exp->set_label (gettext ("Size"));
  sorting = 0;
  Gtk::Widget *widg = exp->get_label_widget ();
  widg->set_margin (5);
}

void
OpenDialogDeals::sortSelection (const Gtk::TreeModel::Path &path,
				Gtk::TreeViewColumn *column, Gtk::Expander *exp,
				Gtk::TreeView *view)
{
  Glib::RefPtr<Gtk::TreeModel> model = view->get_model ();
  const auto iter = model->get_iter (path);
  if (iter)
    {
      auto row = *iter;
      sorting = row[idss];
      exp->set_label (row[sortcols]);
      Gtk::Widget *widg = exp->get_label_widget ();
      widg->set_margin (5);
      exp->set_expanded (false);
    }
  refreshinstr.emit ();
}

void
OpenDialogDeals::instrSelection (const Gtk::TreeModel::Path &path,
				 Gtk::TreeViewColumn *column,
				 Gtk::Expander *exp, Gtk::TreeView *view)
{
  Glib::RefPtr<Gtk::TreeModel> model = view->get_model ();
  const auto iter = model->get_iter (path);
  if (iter)
    {
      auto row = *iter;
      selectedfile = row[idii];
      exp->set_label (row[icoli]);
      Gtk::Widget *widg = exp->get_label_widget ();
      widg->set_margin (5);
      exp->set_expanded (false);
    }
}

void
OpenDialogDeals::refreshInstr (Gtk::TreeView *view, Gtk::TreeViewColumn *column,
			       Gtk::Expander *exp)
{
  filelist.clear ();
  AuxFunc af;
  std::string filename;
  af.homePath (&filename);
  if (dates.size () > 0)
    {
      filename = filename + "/Money/Deals/" + dates[selecteddate] + ".zip";
    }

  std::vector<std::tuple<int, int, std::string>> names;
  std::string line, archnm;
  af.fileNames (filename, names);
  for (size_t i = 0; i < names.size (); i++)
    {

      line = std::get<2> (names[i]);
      if (std::get<1> (names[i]) > 0)
	{
	  filelist.push_back (names[i]);
	}
    }
  filelist.erase (std::remove_if (filelist.begin (), filelist.end (), [this]
  (auto &el)
    { std::string l = this->boardstosh[this->selectedboard].first;
      std::string::size_type n;
      n = (std::get<2>(el)).find(l, 0);
      if(n!=std::string::npos)
	{
	  return false;
	}
      else
	{
	  return true;
	}}),
		  filelist.end ());
  if (sorting == 0)
    {
      std::sort (filelist.begin (), filelist.end (), []
      (auto &el1, auto &el2)
	{
	  return std::get<1>(el1) > std::get<1>(el2);
	});
    }
  if (sorting == 1)
    {
      std::sort (filelist.begin (), filelist.end (), [&af]
      (auto &el1, auto &el2)
	{
	  std::string l1, l2;
	  l1 = std::get<2>(el1);
	  l1.erase(0, l1.find("-") + 1);
	  af.stringToLower(l1);
	  l2 = std::get<2>(el2);
	  l2.erase(0, l2.find("-") + 1);
	  af.stringToLower(l2);
	  return l1 < l2;
	});
    }
  if (sorting == 2)
    {
      std::sort (filelist.begin (), filelist.end (), [&af]
      (auto &el1, auto &el2)
	{
	  std::string l1, l2;
	  l1 = std::get<2>(el1);
	  af.stringToLower(l1);
	  l2 = std::get<2>(el2);
	  af.stringToLower(l2);
	  return l1 < l2;
	});
    }
  for (size_t i = 0; i < filelist.size (); i++)
    {
      std::filesystem::path p = std::filesystem::u8path (
	  std::get<2> (filelist[i]));
      line = p.filename ().u8string ();
      int one, two;
      one = std::get<0> (filelist[i]);
      two = std::get<1> (filelist[i]);
      filelist[i] = std::make_tuple (one, two, line);
    }
  view->unset_model ();
  column->clear ();
  Gtk::TreeModel::ColumnRecord record;
  Gtk::TreeModelColumn<unsigned int> id;
  Gtk::TreeModelColumn<Glib::ustring> instrument;
  record.add (id);
  record.add (instrument);
  Glib::RefPtr<Gtk::ListStore> model = Gtk::ListStore::create (record);

  for (size_t i = 0; i < filelist.size (); i++)
    {
      auto row = *(model->append ());
      row[id] = i;
      std::string line = std::get<2> (filelist[i]);
      std::string auxstr = line;
      line.erase (0, line.find ("-") + std::string ("-").size ());
      line = line + "-" + auxstr.substr (0, auxstr.find ("-"));
      row[instrument] = Glib::ustring (line);
    }
  idii = id;
  icoli = instrument;
  view->set_model (model);
  column->pack_start (icoli);
  view->append_column (*column);
  view->set_headers_visible (false);
  view->set_activate_on_single_click (true);

  if (filelist.size () > 0)
    {
      std::string line = std::get<2> (filelist[0]);
      std::string auxstr = line;
      line.erase (0, line.find ("-") + std::string ("-").size ());
      line = line + "-" + auxstr.substr (0, auxstr.find ("-"));
      exp->set_label (Glib::ustring (line));
    }
  Gtk::Widget *widg = exp->get_label_widget ();
  widg->set_margin (5);
}

void
OpenDialogDeals::openFile ()
{
  AuxFunc af;
  std::string openfile, outfolder;
  af.homePath (&openfile);
  openfile = openfile + "/Money/Deals/" + dates[selecteddate] + ".zip";
  *Opendate = dates[selecteddate];
  outfolder = std::filesystem::temp_directory_path ().u8string ();
#ifdef __linux
  outfolder = outfolder + "/Money/DealsToShow";
#endif
#ifdef _WIN32
  outfolder = outfolder + "Money/DealsToShow";
#endif
  std::filesystem::path out = std::filesystem::u8path (outfolder);

  if (std::filesystem::exists (out))
    {
      std::filesystem::remove_all (out);
    }
  af.unpackByIndex (openfile, outfolder, std::get<0> (filelist[selectedfile]));
  outfolder = outfolder + "/" + std::get<2> (filelist[selectedfile]);
  dealsfilepath.emit (outfolder);
}
