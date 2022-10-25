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

#include "OpenDialog.h"

OpenDialog::OpenDialog(Glib::RefPtr<Gtk::Application> app, Gtk::Window *mwin)
{
  App = app;
  Mwin = mwin;
  css_provider = Gtk::CssProvider::create();
  AuxFunc af;
  std::filesystem::path p = std::filesystem::u8path(af.get_selfpath());
  css_provider->load_from_path(
    Glib::ustring(
      std::string(p.parent_path().u8string())
      + "/../share/Money/openDialog.css"));
  std::string filename;
  af.homePath(&filename);
  filename = filename + "/.Money/BoardsList";
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  std::fstream f;
  f.open(filepath, std::ios_base::in);
  int count = 0;

  if(f.is_open())
    {

      while(!f.eof())
        {
          std::string line;
          getline(f, line);
          af.cp1251toutf8(line);
          if(count > 2 && line != "")
            {
              std::pair<Glib::ustring, Glib::ustring> bp;
              std::string tmp;
              tmp = line;
              tmp.erase(0, tmp.find(";") + std::string(";").size());
              tmp.erase(0, tmp.find(";") + std::string(";").size());
              tmp = tmp.substr(0, tmp.find(";"));
              bp.first = tmp;
              tmp = line;
              tmp.erase(0, tmp.find(";") + std::string(";").size());
              tmp.erase(0, tmp.find(";") + std::string(";").size());
              tmp.erase(0, tmp.find(";") + std::string(";").size());
              tmp = tmp.substr(0, tmp.find(";"));
              bp.second = tmp;
              boards.push_back(bp);
            }
          count++;
        }
      f.close();
    }
  auto iter = std::find_if(boards.begin(), boards.end(), []
                           (auto & e)
  {
    return e.first == "TQBR";
  });
  if(iter != boards.end())
    {
      selectedboard = std::distance(boards.begin(), iter);
    }
  af.homePath(&filename);
  if(boards.size() > selectedboard)
    {
      filename = filename + "/Money/" + boards[selectedboard].first;
    }
  else
    {
      filename = "";
    }
  filepath = std::filesystem::u8path(filename);
  if(std::filesystem::exists(filepath))
    {
      for(auto &it : std::filesystem::recursive_directory_iterator(filepath))
        {
          std::filesystem::path pp = it.path();
          if(!std::filesystem::is_directory(pp))
            {
              std::string p = pp.filename().u8string();
              std::string::size_type n;
              n = p.find(".csv", 0);
              p.erase(n, std::string(".csv").size());
              instruments.push_back(p);
            }
        }
      std::sort(instruments.begin(), instruments.end(), [&af]
                (std::string & e1, std::string & e2)
      {
        std::string l1 = e1.substr(0, e1.rfind("-"));
        std::string l2 = e2.substr(0, e2.rfind("-"));
        af.stringToLower(l1);
        af.stringToLower(l2);
        return l1 < l2;
      });

      creatDialogAll();
    }
}
OpenDialog::~OpenDialog()
{

}

void
OpenDialog::creatDialogAll()
{
  Gtk::Window *window = new Gtk::Window;
  window->set_title(gettext("Instrument selection"));
  window->set_name("allWindow");
  window->set_transient_for(*Mwin);
  window->set_modal(true);
  window->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  auto grid = Gtk::make_managed<Gtk::Grid> ();
  grid->set_halign(Gtk::Align::CENTER);
  window->set_resizable(true);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label> ();
  label->set_text(gettext("Mode:"));
  label->set_halign(Gtk::Align::START);
  label->set_margin(5);
  grid->attach(*label, 0, 0, 1, 1);

  Gtk::TreeModel::ColumnRecord record;
  Gtk::TreeModelColumn<unsigned int> id;
  Gtk::TreeModelColumn<Glib::ustring> instrument;
  record.add(id);
  record.add(instrument);

  Glib::RefPtr<Gtk::ListStore> model = Gtk::ListStore::create(record);
  Gtk::TreeView *box = Gtk::make_managed<Gtk::TreeView> ();
  box->set_name("treeView");
  box->get_style_context()->add_provider(css_provider,
                                         GTK_STYLE_PROVIDER_PRIORITY_USER);
  auto scroll = Gtk::make_managed<Gtk::ScrolledWindow> ();
  scroll->set_expand();
  scroll->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

  Gtk::Expander *exp = Gtk::make_managed<Gtk::Expander> ();
  exp->set_name("expanderB");
  exp->get_style_context()->add_provider(css_provider,
                                         GTK_STYLE_PROVIDER_PRIORITY_USER);
  exp->set_child(*scroll);
  auto iter = std::find_if(boards.begin(), boards.end(), []
                           (auto & element)
  {
    return element.first == "TQBR";
  });
  if(iter != boards.end())
    {
      exp->set_label((*iter).second);
    }
  Gtk::Widget *widg = exp->get_label_widget();
  widg->set_margin(5);
  exp->set_margin(5);
  exp->set_valign(Gtk::Align::CENTER);
  scroll->set_child(*box);
  grid->attach(*exp, 0, 1, 1, 1);

  Gtk::Label *label2 = Gtk::make_managed<Gtk::Label> ();
  label2->set_text(gettext("Instrument:"));
  label2->set_halign(Gtk::Align::START);
  label2->set_margin(5);
  grid->attach(*label2, 0, 2, 1, 1);

  Gtk::ScrolledWindow *scrolli = Gtk::make_managed<Gtk::ScrolledWindow> ();
  scrolli->set_expand();
  scrolli->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
  Gtk::Expander *instr = Gtk::make_managed<Gtk::Expander> ();
  instr->set_margin(5);
  instr->set_name("expI");
  instr->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  instr->set_valign(Gtk::Align::CENTER);
  instr->set_child(*scrolli);
  if(instruments.size() > 0)
    {
      instr->set_label(Glib::ustring(instruments[instrsel]));
    }
  else
    {
      instr->set_label("");
    }
  Gtk::Widget *widg2 = instr->get_label_widget();
  widg2->set_margin(5);

  Gtk::TreeModel::ColumnRecord recordi;
  Gtk::TreeModelColumn<unsigned int> idi;
  Gtk::TreeModelColumn<Glib::ustring> instrumenti;
  recordi.add(idi);
  recordi.add(instrumenti);

  Glib::RefPtr<Gtk::ListStore> modeli = Gtk::ListStore::create(recordi);

  Gtk::TreeView *boxi = Gtk::make_managed<Gtk::TreeView> ();
  scrolli->set_child(*boxi);
  boxi->set_name("treeViewI");
  boxi->get_style_context()->add_provider(css_provider,
                                          GTK_STYLE_PROVIDER_PRIORITY_USER);

  grid->attach(*instr, 0, 3, 1, 1);

  Gtk::Button *chose = Gtk::make_managed<Gtk::Button> ();
  chose->set_label(gettext("Confirm"));
  chose->set_halign(Gtk::Align::CENTER);
  chose->set_margin(5);
  chose->set_name("chosebAI");
  chose->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  chose->signal_clicked().connect(
    sigc::mem_fun(*this, &OpenDialog::emitallFilepath));
  chose->signal_clicked().connect(
    sigc::mem_fun(*window, &Gtk::Window::close));
  grid->attach(*chose, 0, 4, 1, 1);

  Gtk::Button *close = Gtk::make_managed<Gtk::Button> ();
  close->set_label(gettext("Cancel"));
  close->set_halign(Gtk::Align::CENTER);
  close->set_margin(5);
  close->set_name("closeBAI");
  close->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  close->signal_clicked().connect(
    sigc::mem_fun(*window, &Gtk::Window::close));
  grid->attach(*close, 0, 5, 1, 1);

  window->signal_close_request().connect([window, this]
  {
    window->hide();
    this->deleteSign.emit();
    delete window;
    return true;
  },
  false);

  for(size_t i = 0; i < boards.size(); i++)
    {
      auto row = *(model->append());
      row[id] = i;
      row[instrument] = boards[i].second + "-" + boards[i].first;
    }

  for(size_t i = 0; i < instruments.size(); i++)
    {
      auto row = *(modeli->append());
      row[idi] = i;
      row[instrumenti] = Glib::ustring(instruments[i]);
    }
  idii = idi;
  instrumentii = instrumenti;
  Gtk::TreeViewColumn *column = Gtk::make_managed<Gtk::TreeViewColumn> ();
  box->set_model(model);
  column->pack_start(instrument);
  box->append_column(*column);
  box->set_headers_visible(false);
  box->set_activate_on_single_click(true);
  box->signal_row_activated().connect(
    sigc::bind(sigc::mem_fun(*this, &OpenDialog::boardSelection), model, id,
               exp));

  Gtk::TreeViewColumn *columni = Gtk::make_managed<Gtk::TreeViewColumn> ();
  boxi->set_model(modeli);
  columni->pack_start(instrumenti);
  boxi->append_column(*columni);
  boxi->set_headers_visible(false);
  boxi->set_activate_on_single_click(true);
  boxi->signal_row_activated().connect(
    sigc::bind(sigc::mem_fun(*this, &OpenDialog::instrSelection), instr,
               boxi));

  exp->property_expanded().signal_changed().connect(
    sigc::bind(sigc::mem_fun(*this, &OpenDialog::expandScroll), exp, window,
               grid));
  instr->property_expanded().signal_changed().connect(
    sigc::bind(sigc::mem_fun(*this, &OpenDialog::expandScroll), instr,
               window, grid));
  this->refreshi.connect(
    sigc::bind(sigc::mem_fun(*this, &OpenDialog::refreshInstr), boxi,
               columni, instr));
  int x, y, h, w;
  columni->cell_get_size(x, y, w, h);
  if(instruments.size() > 20 || instruments.size() == 0)
    {
      scrolli->set_min_content_height(20 * h);
    }
  else
    {
      scrolli->set_min_content_height(instruments.size() * h);
    }

  column->cell_get_size(x, y, w, h);
  if(boards.size() > 20 || boards.size() == 0)
    {
      scroll->set_min_content_height(20 * h);
    }
  else
    {
      scroll->set_min_content_height(boards.size() * h);
    }
  Gtk::Requisition rq1, rq2;
  exp->get_preferred_size(rq1, rq2);
  scroll->set_max_content_width(rq2.get_width());
  instr->get_preferred_size(rq1, rq2);
  scrolli->set_max_content_width(rq2.get_width());
  window->set_default_size(rq2.get_width() + 10, -1);
  window->set_application(App);
  window->present();
}

void
OpenDialog::expandScroll(Gtk::Expander *exp, Gtk::Window *window,
                         Gtk::Grid *grid)
{
  if(!exp->property_expanded().get_value())
    {
      Gtk::Requisition rq1, rq2;
      exp->get_preferred_size(rq1, rq2);
      window->set_default_size(rq1.get_width(), -1);
    }
}

void
OpenDialog::boardSelection(const Gtk::TreeModel::Path &path,
                           Gtk::TreeViewColumn *column,
                           Glib::RefPtr<Gtk::ListStore> model,
                           Gtk::TreeModelColumn<unsigned int> id,
                           Gtk::Expander *exp)
{
  const auto iter = model->get_iter(path);
  if(iter)
    {
      auto row = *iter;
      selectedboard = row[id];
      exp->set_label(boards[row[id]].second + "-" + boards[row[id]].first);
      Gtk::Widget *widg = exp->get_label_widget();
      widg->set_margin(5);
      exp->set_expanded(false);
    }
  refreshi.emit();
}

void
OpenDialog::instrSelection(const Gtk::TreeModel::Path &path,
                           Gtk::TreeViewColumn *column, Gtk::Expander *instr,
                           Gtk::TreeView *boxi)
{
  Glib::RefPtr<Gtk::TreeModel> model = boxi->get_model();
  const auto iter = model->get_iter(path);
  if(iter)
    {
      auto row = *iter;
      instrsel = row[idii];
      instr->set_label(Glib::ustring(instruments[instrsel]));
      Gtk::Widget *widg = instr->get_label_widget();
      widg->set_margin(5);
      instr->set_expanded(false);
    }
}

void
OpenDialog::refreshInstr(Gtk::TreeView *boxi, Gtk::TreeViewColumn *columni,
                         Gtk::Expander *instr)
{
  instruments.clear();
  instrsel = 0;
  AuxFunc af;
  std::string filename;
  std::filesystem::path filepath;
  af.homePath(&filename);
  filename = filename + "/Money/" + boards[selectedboard].first;
  filepath = std::filesystem::u8path(filename);
  if(std::filesystem::exists(filepath))
    {
      for(auto &it : std::filesystem::recursive_directory_iterator(filepath))
        {
          std::filesystem::path pp = it.path();
          if(!std::filesystem::is_directory(pp))
            {
              std::string p = pp.filename().u8string();
              std::string::size_type n;
              n = p.find(".csv", 0);
              p.erase(n, std::string(".csv").size());
              instruments.push_back(p);
            }
        }
      std::sort(instruments.begin(), instruments.end(), [&af]
                (std::string & e1, std::string & e2)
      {
        std::string l1 = e1;
        std::string l2 = e2;
        af.stringToLower(l1);
        af.stringToLower(l2);
        return l1 < l2;
      });
      boxi->unset_model();
      columni->clear();
      Gtk::TreeModel::ColumnRecord recordi;
      Gtk::TreeModelColumn<unsigned int> idi;
      Gtk::TreeModelColumn<Glib::ustring> instrumenti;
      recordi.add(idi);
      recordi.add(instrumenti);
      Glib::RefPtr<Gtk::ListStore> modeli = Gtk::ListStore::create(recordi);

      for(size_t i = 0; i < instruments.size(); i++)
        {
          auto row = *(modeli->append());
          row[idi] = i;
          row[instrumenti] = Glib::ustring(instruments[i]);
        }
      columni->pack_start(instrumenti);
      boxi->set_model(modeli);
      boxi->append_column(*columni);
      boxi->set_headers_visible(false);
      boxi->set_activate_on_single_click(true);

      if(instruments.size() > 0)
        {
          instr->set_label(Glib::ustring(instruments[instrsel]));
        }
      else
        {
          instr->set_label("");
        }
      Gtk::Widget *widg = instr->get_label_widget();
      widg->set_margin(5);
      idii = idi;
      instrumentii = instrumenti;
    }
  else
    {
      boxi->unset_model();
      instr->set_label("");
      Gtk::Widget *widg = instr->get_label_widget();
      widg->set_margin(5);
    }

}

void
OpenDialog::emitallFilepath()
{
  AuxFunc af;
  std::string filename;
  af.homePath(&filename);
  filename = filename + "/Money/" + boards[selectedboard].first + "/"
             + instruments[instrsel] + ".csv";
  allfilepath.emit(filename);
}
