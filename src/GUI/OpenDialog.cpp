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

#include "OpenDialog.h"

OpenDialog::OpenDialog(Gtk::Window *mwin)
{
  Mwin = mwin;

}
OpenDialog::~OpenDialog()
{
  // TODO Auto-generated destructor stub
}

void
OpenDialog::creatDialogAll(int variant, Gdk::Rectangle rec)
{
  AuxFunc af;
  std::vector<std::tuple<std::string, std::string>> boards;
  DownloadMenu dm(nullptr);
  boards = dm.formVectorBoards();
  std::vector<std::tuple<std::string, std::string, std::string>> instrbase;
  instrbase = dm.formVectorInstr(1);
  std::vector<std::tuple<std::string, std::string, std::string>> instrbasef;
  instrbasef = dm.formVectorInstr(2);
  std::copy(instrbasef.begin(), instrbasef.end(),
	    std::back_inserter(instrbase));
  instrbasef.clear();
  size_t selectedboard = 0;
  auto iter = std::find_if(boards.begin(), boards.end(), []
  (auto &e)
    {
      return std::get<0>(e) == "TQBR";
    });
  if(iter != boards.end())
    {
      selectedboard = std::distance(boards.begin(), iter);
    }

  std::string filename;
  af.homePath(&filename);
  if(boards.size() > selectedboard)
    {
      filename = filename + "/Money/" + std::get<0>(boards[selectedboard]);
    }
  else
    {
      filename.clear();
    }
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  std::vector<std::string> instruments;
  if(std::filesystem::exists(filepath))
    {
      for(auto &it : std::filesystem::recursive_directory_iterator(filepath))
	{
	  std::filesystem::path pp = it.path();
	  if(!std::filesystem::is_directory(pp))
	    {
	      std::string p = pp.stem().u8string();
	      instruments.push_back(p);
	    }
	}
      std::sort(
	  instruments.begin(),
	  instruments.end(),
	  [&af, &instrbase]
	  (auto &el1,
	   auto &el2)
	     {
	       std::string l1 = el1;
	       auto itib = std::find_if(instrbase.begin(), instrbase.end(), [l1](auto &el)
		     {
		       return std::get<0>(el) == l1;
		     });
	       if(itib != instrbase.end())
		 {
		   l1 = std::get<2>(*itib);
		 }
	       std::string l2 = el2;
	       itib = std::find_if(instrbase.begin(), instrbase.end(), [l2](auto &el)
		     {
		       return std::get<0>(el) == l2;
		     });
	       if(itib != instrbase.end())
		 {
		   l2 = std::get<2>(*itib);
		 }
	       l1.erase(std::remove_if(l1.begin(), l1.end(), [](auto &el)
			 {
			   return el == '\"';
			 }),l1.end());
	       l2.erase(std::remove_if(l2.begin(), l2.end(), [](auto &el)
			 {
			   return el == '\"';
			 }),l2.end());
	       while(*(l1.begin()) == ' ')
		 {
		   l1.erase(l1.begin());
		 }
	       while(*(l2.begin()) == ' ')
		 {
		   l2.erase(l2.begin());
		 }
	       af.stringToLower(l1);
	       af.stringToLower(l2);
	       return l1 < l2;
	     });
    }

  Gtk::Window *window = new Gtk::Window;
  window->set_application(Mwin->get_application());
  window->set_title(gettext("Instrument selection"));
  window->set_name("instrChooseWindow");
  window->set_default_size(1, 1);
  window->set_transient_for(*Mwin);

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::FILL);
  grid->set_valign(Gtk::Align::FILL);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
  label->set_text(gettext("Mode:"));
  label->set_halign(Gtk::Align::START);
  label->set_margin(5);
  grid->attach(*label, 0, 0, 1, 1);

  Gtk::TreeModel::ColumnRecord record_boards;
  Gtk::TreeModelColumn<Glib::ustring> boards_code;
  Gtk::TreeModelColumn<Glib::ustring> boards_name;
  record_boards.add(boards_code);
  record_boards.add(boards_name);
  Glib::RefPtr<Gtk::ListStore> model_boards = Gtk::ListStore::create(
      record_boards);
  for(size_t i = 0; i < boards.size(); i++)
    {
      auto row = *(model_boards->append());
      row[boards_code] = Glib::ustring(std::get<0>(boards[i]));
      row[boards_name] = Glib::ustring(std::get<1>(boards[i])) + "-"
	  + Glib::ustring(std::get<0>(boards[i]));
    }

  Gtk::TreeView *tv_boards = Gtk::make_managed<Gtk::TreeView>();
  tv_boards->set_model(model_boards);
  tv_boards->append_column("", boards_name);
  tv_boards->set_headers_visible(false);
  tv_boards->set_activate_on_single_click(true);

  tv_boards->set_name("treeViewI");
  Gtk::ScrolledWindow *scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
  scroll->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
  scroll->set_child(*tv_boards);

  Gtk::Popover *pop_boards = Gtk::make_managed<Gtk::Popover>();
  pop_boards->set_child(*scroll);

  Gtk::MenuButton *men_boards = Gtk::make_managed<Gtk::MenuButton>();
  men_boards->set_margin(5);
  men_boards->set_halign(Gtk::Align::FILL);
  men_boards->set_popover(*pop_boards);
  men_boards->set_name("boards");
  for(auto row : model_boards->children())
    {
      Glib::ustring val;
      row.get_value(0, val);
      if(val == "TQBR")
	{
	  val.clear();
	  row.get_value(1, val);
	  men_boards->set_label(val);
	  break;
	}
    }
  grid->attach(*men_boards, 0, 1, 1, 1);

  Gtk::Label *label2 = Gtk::make_managed<Gtk::Label>();
  label2->set_text(gettext("Instrument:"));
  label2->set_halign(Gtk::Align::START);
  label2->set_margin(5);
  grid->attach(*label2, 0, 2, 1, 1);

  Gtk::ScrolledWindow *scrolli = Gtk::make_managed<Gtk::ScrolledWindow>();
  scrolli->set_expand();
  scrolli->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

  Gtk::Popover *pop_instr = Gtk::make_managed<Gtk::Popover>();
  pop_instr->set_child(*scrolli);

  Gtk::TreeModel::ColumnRecord record_instr;
  Gtk::TreeModelColumn<Glib::ustring> fnm_col;
  Gtk::TreeModelColumn<Glib::ustring> instr_col;
  record_instr.add(fnm_col);
  record_instr.add(instr_col);
  Glib::RefPtr<Gtk::ListStore> model_instr = Gtk::ListStore::create(
      record_instr);
  for(size_t i = 0; i < instruments.size(); i++)
    {
      auto row = *(model_instr->append());
      std::string val = instruments[i];
      row[fnm_col] = Glib::ustring(val);
      auto itib = std::find_if(instrbase.begin(), instrbase.end(), [val]
      (auto &el)
	{
	  return std::get<0>(el) == val;
	});
      if(itib != instrbase.end())
	{
	  row[instr_col] = Glib::ustring(
	      std::get<2>(*itib) + "-" + std::get<0>(*itib));
	}
      else
	{
	  row[instr_col] = Glib::ustring(instruments[i]);
	}
    }

  Gtk::TreeView *tv_instr = Gtk::make_managed<Gtk::TreeView>();
  scrolli->set_child(*tv_instr);
  tv_instr->set_name("treeViewI");

  tv_instr->set_model(model_instr);
  tv_instr->append_column("", instr_col);
  tv_instr->set_headers_visible(false);
  tv_instr->set_activate_on_single_click(true);

  Gtk::MenuButton *men_instr = Gtk::make_managed<Gtk::MenuButton>();
  men_instr->set_margin(5);
  men_instr->set_name("boards");
  men_instr->set_popover(*pop_instr);
  auto row = model_instr->children()[0];
  if(row)
    {
      Glib::ustring val;
      row.get_value(1, val);
      men_instr->set_label(val);
    }
  grid->attach(*men_instr, 0, 3, 1, 1);

  tv_boards->signal_row_activated().connect(
      [men_boards, tv_boards, tv_instr, men_instr]
      (const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column)
	{
	  Glib::RefPtr<Gtk::TreeModel> model = tv_boards->get_model();
	  Gdk::Rectangle rec;
	  OpenDialog od(nullptr);
	  od.boardSelection(path, column, model, men_boards);
	  od.refreshInstr(tv_instr, tv_boards, men_instr);
	});

  tv_instr->signal_row_activated().connect([tv_instr, men_instr]
  (const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column)
    {
      Glib::RefPtr<Gtk::TreeModel> model = tv_instr->get_model();
      Gdk::Rectangle rec;
      OpenDialog od(nullptr);
      od.instrSelection(path, column, model, men_instr);
    });

  Gtk::Button *chose = Gtk::make_managed<Gtk::Button>();
  chose->set_label(gettext("Confirm"));
  chose->set_halign(Gtk::Align::CENTER);
  chose->set_margin(5);
  chose->set_name("confirmButton");
  chose->signal_clicked().connect([window, tv_boards, tv_instr, variant, rec]
  {
    OpenDialog od(nullptr);
    od.plotDiagram(window, tv_boards, tv_instr, variant, rec);
  });
  grid->attach(*chose, 0, 4, 1, 1);

  Gtk::Button *close = Gtk::make_managed<Gtk::Button>();
  close->set_label(gettext("Cancel"));
  close->set_halign(Gtk::Align::CENTER);
  close->set_margin(5);
  close->set_name("closeButton");
  close->signal_clicked().connect(sigc::mem_fun(*window, &Gtk::Window::close));
  grid->attach(*close, 0, 5, 1, 1);

  int x, y, h, w;
  Gtk::TreeViewColumn *columni = tv_instr->get_column(0);
  columni->cell_get_size(x, y, w, h);
  if(instruments.size() > 20 || instruments.size() == 0)
    {
      scrolli->set_min_content_height(20 * h);
    }
  else
    {
      scrolli->set_min_content_height(instruments.size() * h);
    }

  Gtk::TreeViewColumn *column = tv_boards->get_column(0);
  column->cell_get_size(x, y, w, h);
  if(boards.size() > 20 || boards.size() == 0)
    {
      scroll->set_min_content_height(20 * h);
    }
  else
    {
      scroll->set_min_content_height(boards.size() * h);
    }
  Gtk::Requisition min, nat;
  tv_boards->get_preferred_size(min, nat);
  scroll->set_min_content_width(nat.get_width());
  tv_instr->get_preferred_size(min, nat);
  scrolli->set_min_content_width(nat.get_width());

  window->signal_close_request().connect([window, this]
  {
    window->hide();
    delete window;
    return true;
  },
					 false);

  window->present();
}

void
OpenDialog::boardSelection(const Gtk::TreeModel::Path &path,
			   Gtk::TreeViewColumn *column,
			   Glib::RefPtr<Gtk::TreeModel> model,
			   Gtk::MenuButton *men_b)
{
  const auto iter = model->get_iter(path);
  if(iter)
    {
      Glib::ustring val;
      iter->get_value(1, val);
      Gtk::Grid *gr = dynamic_cast<Gtk::Grid*>(men_b->get_parent());
      Gtk::Window *win = dynamic_cast<Gtk::Window*>(gr->get_parent());
      win->set_default_size(1, 1);
      men_b->set_label(val);
      men_b->popdown();
    }
}

void
OpenDialog::instrSelection(const Gtk::TreeModel::Path &path,
			   Gtk::TreeViewColumn *column,
			   Glib::RefPtr<Gtk::TreeModel> model,
			   Gtk::MenuButton *men_instr)
{
  const auto iter = model->get_iter(path);
  if(iter)
    {
      Glib::ustring val;
      iter->get_value(1, val);
      Gtk::Grid *gr = dynamic_cast<Gtk::Grid*>(men_instr->get_parent());
      Gtk::Window *win = dynamic_cast<Gtk::Window*>(gr->get_parent());
      win->set_default_size(1, 1);
      men_instr->set_label(val);
      men_instr->popdown();
    }
}

void
OpenDialog::refreshInstr(Gtk::TreeView *tv_instr, Gtk::TreeView *tv_boards,
			 Gtk::MenuButton *men_instr)
{
  tv_instr->unset_model();
  tv_instr->remove_all_columns();
  AuxFunc af;
  DownloadMenu dm(nullptr);
  std::vector<std::tuple<std::string, std::string, std::string>> instrbase;
  instrbase = dm.formVectorInstr(1);
  std::vector<std::tuple<std::string, std::string, std::string>> instrbasef;
  instrbasef = dm.formVectorInstr(2);
  std::copy(instrbasef.begin(), instrbasef.end(),
	    std::back_inserter(instrbase));
  instrbasef.clear();
  std::string filename;
  std::filesystem::path filepath;
  auto sel = tv_boards->get_selection();
  if(sel)
    {
      auto iter = sel->get_selected();
      if(iter)
	{
	  Glib::ustring val;
	  iter->get_value(0, val);
	  af.homePath(&filename);
	  filename = filename + "/Money/" + std::string(val);
	  filepath = std::filesystem::u8path(filename);
	}
    }
  std::vector<std::string> instruments;
  if(std::filesystem::exists(filepath))
    {
      for(auto &it : std::filesystem::recursive_directory_iterator(filepath))
	{
	  std::filesystem::path pp = it.path();
	  if(!std::filesystem::is_directory(pp))
	    {
	      std::string p = pp.stem().u8string();
	      instruments.push_back(p);
	    }
	}
      std::sort(
	  instruments.begin(),
	  instruments.end(),
	  [&af, &instrbase]
	  (auto &el1,
	   auto &el2)
	     {
	       std::string l1 = el1;
	       auto itib = std::find_if(instrbase.begin(), instrbase.end(), [l1](auto &el)
		     {
		       return std::get<0>(el) == l1;
		     });
	       if(itib != instrbase.end())
		 {
		   l1 = std::get<2>(*itib);
		 }
	       std::string l2 = el2;
	       itib = std::find_if(instrbase.begin(), instrbase.end(), [l2](auto &el)
		     {
		       return std::get<0>(el) == l2;
		     });
	       if(itib != instrbase.end())
		 {
		   l2 = std::get<2>(*itib);
		 }
	       l1.erase(std::remove_if(l1.begin(), l1.end(), [](auto &el)
			 {
			   return el == '\"';
			 }),l1.end());
	       l2.erase(std::remove_if(l2.begin(), l2.end(), [](auto &el)
			 {
			   return el == '\"';
			 }),l2.end());
	       while(*(l1.begin()) == ' ')
		 {
		   l1.erase(l1.begin());
		 }
	       while(*(l2.begin()) == ' ')
		 {
		   l2.erase(l2.begin());
		 }
	       af.stringToLower(l1);
	       af.stringToLower(l2);
	       return l1 < l2;
	     });

      Gtk::TreeModel::ColumnRecord record_instr;
      Gtk::TreeModelColumn<Glib::ustring> fnm_col;
      Gtk::TreeModelColumn<Glib::ustring> instr_col;
      record_instr.add(fnm_col);
      record_instr.add(instr_col);
      Glib::RefPtr<Gtk::ListStore> model_instr = Gtk::ListStore::create(
	  record_instr);
      for(size_t i = 0; i < instruments.size(); i++)
	{
	  auto row = *(model_instr->append());
	  std::string val = instruments[i];
	  row[fnm_col] = Glib::ustring(val);
	  auto itib = std::find_if(instrbase.begin(), instrbase.end(), [val]
	  (auto &el)
	    {
	      return std::get<0>(el) == val;
	    });
	  if(itib != instrbase.end())
	    {
	      row[instr_col] = Glib::ustring(
		  std::get<2>(*itib) + "-" + std::get<0>(*itib));
	    }
	  else
	    {
	      row[instr_col] = Glib::ustring(instruments[i]);
	    }
	}

      tv_instr->set_model(model_instr);
      tv_instr->append_column("", instr_col);
      tv_instr->set_headers_visible(false);
      tv_instr->set_activate_on_single_click(true);

      auto row = model_instr->children()[0];
      if(row)
	{
	  Glib::ustring val;
	  row.get_value(1, val);
	  men_instr->set_label(val);
	}
    }
  else
    {
      tv_instr->unset_model();
      tv_instr->remove_all_columns();
      men_instr->set_label("");
    }

}

void
OpenDialog::plotDiagram(Gtk::Window *win, Gtk::TreeView *tv_boards,
			Gtk::TreeView *tv_instr, int variant,
			Gdk::Rectangle rec)
{
  std::string board;
  auto sel_boards = tv_boards->get_selection();
  if(sel_boards)
    {
      auto iter = sel_boards->get_selected();
      if(iter)
	{
	  Glib::ustring val;
	  iter->get_value(0, val);
	  board = std::string(val);
	}
      else
	{
	  board = "TQBR";
	}
    }

  std::string instrument;
  auto sel_instr = tv_instr->get_selection();
  if(sel_instr)
    {
      auto iter = sel_instr->get_selected();
      if(iter)
	{
	  Glib::ustring val;
	  iter->get_value(0, val);
	  instrument = std::string(val) + ".csv";
	}
      else
	{
	  auto model = tv_instr->get_model();
	  if(model)
	    {
	      auto row = model->children()[0];
	      if(row)
		{
		  Glib::ustring val;
		  row.get_value(0, val);
		  instrument = std::string(val) + ".csv";
		}
	    }
	}
    }
  AuxFunc af;
  std::string filename;
  af.homePath(&filename);
  filename = filename + "/Money/" + board + "/" + instrument;
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  if(std::filesystem::exists(filepath))
    {
      GraphicWidget *grw = new GraphicWidget(rec.get_width(), rec.get_height(),
					     win, filepath.u8string());
      grw->plot(0, variant, "");
    }
}
