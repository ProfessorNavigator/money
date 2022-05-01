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

#include "GraphicWidget.h"

GraphicWidget::GraphicWidget (Glib::RefPtr<Gtk::Application> app, int width,
			      int height, Gtk::Window *mwin)
{
  App = app;
  Mwin = mwin;
  css_provider = Gtk::CssProvider::create ();
  AuxFunc af;
  std::filesystem::path p = std::filesystem::u8path (af.get_selfpath ());
  css_provider->load_from_path (
      Glib::ustring (
	  std::string (p.parent_path ().u8string ())
	      + "/../share/Money/graphicWidg.css"));
  plotincr.push_back (0);
  plotincr.push_back (0);
  plotincr.push_back (1);
  plotincr.push_back (1);
  Height = height;
  Width = width;
}

GraphicWidget::~GraphicWidget ()
{

}

void
GraphicWidget::plot (int variant, int graphic)
{
  if (variant == 0)
    {
      OpenDialog *openfile = new OpenDialog (App, Mwin);
      openfile->allfilepath.connect (
	  sigc::bind (sigc::mem_fun (*this, &GraphicWidget::graphic), graphic));
      openfile->deleteSign.connect ( [openfile]
      {
	delete openfile;
      });
    }
  if (variant == 1)
    {
      OpenDialogDeals *openfile = new OpenDialogDeals (App, Mwin);
      openfile->dealsfilepath.connect (
	  sigc::bind (sigc::mem_fun (*this, &GraphicWidget::graphic), graphic));
      openfile->deleteSign.connect ( [openfile]
      {
	delete openfile;
      });
    }
  if (variant == 2)
    {
      AuxFunc af;
      std::string file;
      af.homePath (&file);
      file = file + "/Money/PPm/PPm.csv";
      this->graphic (file, graphic);
    }
}

void
GraphicWidget::graphic (std::string file, int graphvar)
{
  std::filesystem::path p = std::filesystem::u8path (file);
  Gtk::Window *window = new Gtk::Window;
  Gtk::Overlay *overlay = Gtk::make_managed<Gtk::Overlay> ();
  window->set_title (Glib::ustring (p.filename ().stem ().u8string ()));
  window->set_child (*overlay);
  window->fullscreen ();

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid> ();
  grid->set_name ("buttonPanel");
  grid->get_style_context ()->add_provider (css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);
  grid->set_halign (Gtk::Align::END);
  grid->set_valign (Gtk::Align::CENTER);
  grid->set_margin (5);

  mglGraph *graph = new mglGraph;
  Gtk::DrawingArea *drar = Gtk::make_managed<Gtk::DrawingArea> ();
  drar->set_draw_func (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::on_draw), file, graph,
		  graphvar));

  Glib::RefPtr<Gtk::EventControllerScroll> scr =
      Gtk::EventControllerScroll::create ();
  scr->set_flags (
      Gtk::EventControllerScroll::Flags::VERTICAL
	  | Gtk::EventControllerScroll::Flags::DISCRETE);
  scr->signal_scroll ().connect (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::scrollEvent), graph,
		  drar),
      true);
  drar->add_controller (scr);

  Glib::RefPtr<Gtk::GestureDrag> drag = Gtk::GestureDrag::create ();
  drag->signal_drag_update ().connect (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::dragOperation), graph,
		  drar));
  drag->signal_drag_end ().connect (
      sigc::mem_fun (*this, &GraphicWidget::dragEnd));
  drar->add_controller (drag);

  Glib::RefPtr<Gtk::GestureClick> cl = Gtk::GestureClick::create ();
  cl->set_button (3);
  drar->add_controller (cl);

  overlay->set_child (*drar);
  overlay->add_overlay (*grid);
  if (p.filename ().stem ().u8string () != "PPm")
    {
      Gtk::Label *instrnm = Gtk::make_managed<Gtk::Label> ();
      instrnm->set_halign (Gtk::Align::START);
      instrnm->set_valign (Gtk::Align::START);
      instrnm->set_margin (5);
      instrnm->set_name ("instrumLab");
      instrnm->get_style_context ()->add_provider (css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
      instrnm->set_text (
	  gettext ("Instrument: ")
	      + Glib::ustring (p.filename ().stem ().u8string ()));
      overlay->add_overlay (*instrnm);
    }

  Gtk::Button *increase = Gtk::make_managed<Gtk::Button> ();
  increase->set_label (gettext ("Increase"));
  increase->set_halign (Gtk::Align::CENTER);
  increase->set_margin (5);
  increase->set_name ("sizeButton");
  increase->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  increase->signal_clicked ().connect (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::zoomGraph), graph, drar,
		  0));
  grid->attach (*increase, 0, 0, 1, 1);

  Gtk::Button *decrease = Gtk::make_managed<Gtk::Button> ();
  decrease->set_label (gettext ("Decrease"));
  decrease->set_halign (Gtk::Align::CENTER);
  decrease->set_margin (5);
  decrease->set_name ("sizeButton");
  decrease->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  decrease->signal_clicked ().connect (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::zoomGraph), graph, drar,
		  1));
  grid->attach (*decrease, 0, 1, 1, 1);

  Gtk::Button *up = Gtk::make_managed<Gtk::Button> ();
  up->set_label (gettext ("Move up"));
  up->set_halign (Gtk::Align::CENTER);
  up->set_margin (5);
  up->set_name ("sizeButton");
  up->get_style_context ()->add_provider (css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);
  up->signal_clicked ().connect (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::zoomGraph), graph, drar,
		  2));
  grid->attach (*up, 0, 2, 1, 1);

  Gtk::Button *down = Gtk::make_managed<Gtk::Button> ();
  down->set_label (gettext ("Move down"));
  down->set_halign (Gtk::Align::CENTER);
  down->set_margin (5);
  down->set_name ("sizeButton");
  down->get_style_context ()->add_provider (css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);
  down->signal_clicked ().connect (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::zoomGraph), graph, drar,
		  3));
  grid->attach (*down, 0, 3, 1, 1);

  Gtk::Button *left = Gtk::make_managed<Gtk::Button> ();
  left->set_label (gettext ("Move left"));
  left->set_halign (Gtk::Align::CENTER);
  left->set_margin (5);
  left->set_name ("sizeButton");
  left->get_style_context ()->add_provider (css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);
  left->signal_clicked ().connect (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::zoomGraph), graph, drar,
		  4));
  grid->attach (*left, 0, 4, 1, 1);

  Gtk::Button *right = Gtk::make_managed<Gtk::Button> ();
  right->set_label (gettext ("Move right"));
  right->set_halign (Gtk::Align::CENTER);
  right->set_margin (5);
  right->set_name ("sizeButton");
  right->get_style_context ()->add_provider (css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);
  right->signal_clicked ().connect (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::zoomGraph), graph, drar,
		  5));
  grid->attach (*right, 0, 5, 1, 1);

  Gtk::Button *restore = Gtk::make_managed<Gtk::Button> ();
  restore->set_label (gettext ("Recover"));
  restore->set_halign (Gtk::Align::CENTER);
  restore->set_margin (5);
  restore->set_name ("restoreButton");
  restore->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  restore->signal_clicked ().connect (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::zoomGraph), graph, drar,
		  6));
  grid->attach (*restore, 0, 6, 1, 1);

  Gtk::Button *savejpg = Gtk::make_managed<Gtk::Button> ();
  savejpg->set_label (gettext ("Save as jpg"));
  savejpg->set_halign (Gtk::Align::CENTER);
  savejpg->set_margin (5);
  savejpg->set_name ("saveButton");
  savejpg->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  savejpg->signal_clicked ().connect (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::saveGraph), graph, 0));
  grid->attach (*savejpg, 0, 7, 1, 1);

  Gtk::Button *savepng = Gtk::make_managed<Gtk::Button> ();
  savepng->set_label (gettext ("Save as png"));
  savepng->set_halign (Gtk::Align::CENTER);
  savepng->set_margin (5);
  savepng->set_name ("saveButton");
  savepng->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  savepng->signal_clicked ().connect (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::saveGraph), graph, 1));
  grid->attach (*savepng, 0, 8, 1, 1);

  Gtk::Button *minb = Gtk::make_managed<Gtk::Button> ();
  minb->set_label (gettext ("Minimize"));
  minb->set_halign (Gtk::Align::CENTER);
  minb->set_margin (5);
  minb->set_name ("minButton");
  minb->get_style_context ()->add_provider (css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);
  minb->signal_clicked ().connect ( [window]
  {
    window->minimize ();
  });
  grid->attach (*minb, 0, 9, 1, 1);

  Gtk::Button *close = Gtk::make_managed<Gtk::Button> ();
  close->set_label (gettext ("Close"));
  close->set_halign (Gtk::Align::CENTER);
  close->set_name ("closeButton");
  close->get_style_context ()->add_provider (css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);
  close->set_margin (5);
  grid->attach (*close, 0, 10, 1, 1);

  cl->signal_pressed ().connect (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::rightClick), cl, graph,
		  drar));
  close->signal_clicked ().connect (
      sigc::mem_fun (*window, &Gtk::Window::close));
  if (graphvar == 8)
    {
      Gtk::TreeModel::ColumnRecord record;
      Gtk::TreeModelColumn<std::string> dates;
      Gtk::TreeModelColumn<std::string> inflation;
      record.add (dates);
      record.add (inflation);

      Glib::RefPtr<Gtk::ListStore> model = Gtk::ListStore::create (record);
      Gtk::TreeView *treev = Gtk::make_managed<Gtk::TreeView> ();
      treev->set_name ("inflationTV");
      treev->get_style_context ()->add_provider (
	  css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
      treev->set_grid_lines (Gtk::TreeView::GridLines::BOTH);
      treev->set_margin (10);
      treev->set_halign (Gtk::Align::START);
      treev->set_valign (Gtk::Align::CENTER);

      std::filesystem::path filename = std::filesystem::u8path (file);
      std::string name = filename.parent_path ().u8string ();
      name = name + "/Inflation.csv";
      filename = std::filesystem::u8path (name);

      std::fstream f;
      std::string line, temp;
      int count = 0;
      if (!std::filesystem::exists (filename))
	{
	  std::cout << "Inflation file not opened" << std::endl;
	}
      else
	{
	  f.open (filename, std::ios_base::in);
	  while (!f.eof ())
	    {
	      getline (f, line);
	      if (line != "")
		{
		  temp = line;
		  temp = temp.substr (0, temp.find (";"));
		  if (temp == "Дата")
		    {
		      temp = gettext ("Date");
		    }
		  auto row = *(model->append ());
		  row[dates] = Glib::ustring (temp);
		  temp = line;
		  temp.erase (0, temp.find (";") + std::string (";").size ());
		  if (temp == "ИПС")
		    {
		      temp = gettext ("CPP");
		    }
		  if (count == 0)
		    {
		      row[inflation] = temp + " \%";
		    }
		  else
		    {
		      std::stringstream strm;
		      std::locale loc ("C");
		      double tmpd;
		      strm.imbue (loc);
		      strm << temp;
		      strm >> tmpd;
		      strm.str ("");
		      strm.clear ();
		      strm.imbue (std::locale (""));
		      strm << std::fixed;
		      strm << std::setprecision (2);
		      strm << tmpd;
		      row[inflation] = strm.str ();
		    }
		}
	      count = count + 1;
	    }
	  f.close ();
	}
      treev->set_model (model);
      Gtk::TreeViewColumn *column1 = Gtk::make_managed<Gtk::TreeViewColumn> ();
      Gtk::TreeViewColumn *column2 = Gtk::make_managed<Gtk::TreeViewColumn> ();
      column1->pack_start (dates);
      column2->pack_start (inflation);
      column2->set_alignment (Gtk::Align::CENTER);
      treev->append_column (*column1);
      treev->append_column (*column2);
      treev->set_headers_visible (false);
      Gtk::CellRenderer *ren = treev->get_column_cell_renderer (1);
      ren->set_alignment (0.5, 0.5);
      ren = treev->get_column_cell_renderer (0);
      ren->set_alignment (0.5, 0.5);
      Gtk::Requisition rq1, rq2;
      treev->get_preferred_size (rq1, rq2);
      Gtk::ScrolledWindow *ovlscrl = Gtk::make_managed<Gtk::ScrolledWindow> ();
      ovlscrl->set_policy (Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
      ovlscrl->set_min_content_height (Height);
      ovlscrl->set_min_content_width (rq1.get_width ());
      ovlscrl->set_halign (Gtk::Align::START);
      ovlscrl->set_valign (Gtk::Align::CENTER);
      ovlscrl->set_child (*treev);

      overlay->add_overlay (*ovlscrl);
    }

  window->signal_close_request ().connect ( [window, graph, this]
  {
    dateplot.clear ();
    window->hide ();
    graph->Zoom (0, 0, 1, 1);
    delete graph;
    delete window;
    this->class_finished.emit ();
    return true;
  },
					   false);
  window->set_application (App);
  window->present ();
}

void
GraphicWidget::on_draw (const Cairo::RefPtr<Cairo::Context> &cr, int width,
			int height, std::string file, mglGraph *graph,
			int variant)
{

  if (variant == 0)
    {
      PlotPriceAll prall (file, Height, Width, &dateplot);
      prall.Draw (graph);
    }
  if (variant == 1)
    {
      PSD psd (file, Height, Width, &dateplot);
      psd.Draw (graph);
    }
  if (variant == 2)
    {
      PlotVolumeAll val (file, Height, Width, &dateplot);
      val.Draw (graph);
    }

  if (variant == 3)
    {
      PlotPriceDeals pd (file, Height, Width, &dateplot);
      pd.Draw (graph);
    }
  if (variant == 4)
    {
      PSDDeals psdd (file, Height, Width, &dateplot);
      psdd.Draw (graph);
    }
  if (variant == 5)
    {
      PlotVolumeDeals vd (file, Height, Width, &dateplot);
      vd.Draw (graph);
    }
  if (variant == 6)
    {
      PlotAllCommon ac (file, Height, Width, &dateplot);
      ac.Draw (graph);
    }
  if (variant == 7)
    {
      PlotDealsCommon dc (file, Height, Width, &dateplot);
      dc.Draw (graph);
    }
  if (variant == 8)
    {
      PlotPSDGlobal psdg (file, Height, Width, &dateplot);
      psdg.Draw (graph);
    }
  if (variant == 9)
    {
      PlotVolumeGlobal pvg (file, Height, Width, &dateplot);
      pvg.Draw (graph);
    }
  if (variant == 10)
    {
      PlotMoneyGlobal pmg (file, Height, Width, &dateplot);
      pmg.Draw (graph);
    }
  if (variant == 11)
    {
      PlotGlobalCommon pgc (file, Height, Width, &dateplot);
      pgc.Draw (graph);
    }
  auto image = Gdk::Pixbuf::create_from_data (
      graph->GetRGB (), Gdk::Colorspace::RGB, false, 8, graph->GetWidth (),
      graph->GetHeight (), 3 * graph->GetWidth () * sizeof(guint8));
  Gdk::Cairo::set_source_pixbuf (cr, image, 0, 0);
  cr->rectangle (0, 0, image->get_width (), image->get_height ());
  cr->fill ();
}

void
GraphicWidget::zoomGraph (mglGraph *gr, Gtk::DrawingArea *drar, int id)
{
  double pl1, pl2;
  pl1 = (plotincr[2] - plotincr[0]) * 0.1;
  pl2 = (plotincr[3] - plotincr[1]) * 0.1;
  if (id == 0)
    {
      plotincr[0] = plotincr[0] + pl1;
      plotincr[1] = plotincr[1] + pl2;
      plotincr[2] = plotincr[2] - pl1;
      plotincr[3] = plotincr[3] - pl2;
    }
  if (id == 1)
    {
      plotincr[0] = plotincr[0] - pl1;
      plotincr[1] = plotincr[1] - pl2;
      plotincr[2] = plotincr[2] + pl1;
      plotincr[3] = plotincr[3] + pl2;
    }
  if (id == 2)
    {
      pl2 = (plotincr[3] - plotincr[1]) * 0.05;
      plotincr[1] = plotincr[1] - pl2;
      plotincr[3] = plotincr[3] - pl2;
    }
  if (id == 3)
    {
      pl2 = (plotincr[3] - plotincr[1]) * 0.05;
      plotincr[1] = plotincr[1] + pl2;
      plotincr[3] = plotincr[3] + pl2;
    }

  if (id == 4)
    {
      pl1 = (plotincr[2] - plotincr[0]) * 0.05;
      plotincr[0] = plotincr[0] + pl1;
      plotincr[2] = plotincr[2] + pl1;
    }

  if (id == 5)
    {
      pl1 = (plotincr[2] - plotincr[0]) * 0.05;
      plotincr[0] = plotincr[0] - pl1;
      plotincr[2] = plotincr[2] - pl1;
    }
  if (id == 6)
    {
      plotincr[0] = 0;
      plotincr[1] = 0;
      plotincr[2] = 1;
      plotincr[3] = 1;
    }

  gr->Zoom (plotincr[0], plotincr[1], plotincr[2], plotincr[3]);
  drar->set_draw_func (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::on_drawedit), gr));
}

void
GraphicWidget::on_drawedit (const Cairo::RefPtr<Cairo::Context> &cr, int width,
			    int height, mglGraph *graph)
{
  auto image = Gdk::Pixbuf::create_from_data (
      graph->GetRGB (), Gdk::Colorspace::RGB, false, 8, graph->GetWidth (),
      graph->GetHeight (), 3 * graph->GetWidth () * sizeof(guint8));
  Gdk::Cairo::set_source_pixbuf (cr, image, 0, 0);
  cr->rectangle (0, 0, image->get_width (), image->get_height ());
  cr->fill ();
}

void
GraphicWidget::saveGraph (mglGraph *gr, int mode)
{
  std::string filename;
  Gtk::Window *window = new Gtk::Window;
  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid> ();
  window->set_child (*grid);

  Gtk::Entry *file = Gtk::make_managed<Gtk::Entry> ();
  file->set_placeholder_text (gettext ("File name"));
  file->set_margin (5);
  grid->attach (*file, 0, 0, 2, 1);

  AuxFunc af;
  std::string fol;
  af.homePath (&fol);
  Glib::ustring folder (fol);
#ifdef __linux
  folder = folder + gettext ("/Images");
#endif

  Gtk::FileChooserWidget *filech = Gtk::make_managed<Gtk::FileChooserWidget> ();
  filech->set_margin (5);
  filech->set_current_folder (Gio::File::create_for_path (folder));
  grid->attach (*filech, 0, 1, 2, 1);

  Gtk::Button *cancel = Gtk::make_managed<Gtk::Button> ();
  cancel->set_label (gettext ("Cancel"));
  cancel->set_halign (Gtk::Align::START);
  cancel->set_margin (5);
  cancel->signal_clicked ().connect (
      sigc::mem_fun (*window, &Gtk::Window::close));
  grid->attach (*cancel, 0, 2, 1, 1);

  Gtk::Button *save = Gtk::make_managed<Gtk::Button> ();
  save->set_label (gettext ("Save"));
  save->set_halign (Gtk::Align::END);
  save->set_margin (5);

  grid->attach (*save, 1, 2, 1, 1);

  window->show ();
  save->signal_clicked ().connect (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::saveJPEG), window,
		  filename, filech, file, gr, mode));
}

void
GraphicWidget::saveJPEG (Gtk::Window *window, std::string filename,
			 Gtk::FileChooser *filech, Gtk::Entry *file,
			 mglGraph *gr, int mode)
{
  filename = filech->get_current_folder ()->get_path ();
  filename = filename + "/" + file->get_text ();
  std::string::size_type n;
  if (mode == 0)
    {
      n = filename.find (".jpg", 0);
      if (n == std::string::npos)
	{
	  n = filename.find (".jpeg", 0);
	}
      if (n == std::string::npos)
	{
	  filename = filename + ".jpg";
	}
    }

  if (mode == 1)
    {
      n = filename.find (".png", 0);
      if (n == std::string::npos)
	{
	  filename = filename + ".png";
	}
    }

  std::filesystem::path *p = new std::filesystem::path (
      std::filesystem::u8path (filename));
  if (std::filesystem::exists (*p))
    {
      Gtk::MessageDialog *dialog = new Gtk::MessageDialog (
	  *window, gettext ("File already exists. Replace?"), true,
	  Gtk::MessageType::INFO, Gtk::ButtonsType::NONE, true);
      Gtk::Button *accept = dialog->add_button (gettext ("Yes"), 0);
      accept->set_halign (Gtk::Align::END);
      accept->set_margin (5);
      Gtk::Button *cancel = dialog->add_button (gettext ("No"), 1);
      cancel->set_halign (Gtk::Align::START);
      cancel->set_margin (5);
      dialog->set_hide_on_close (true);
      dialog->show ();
      dialog->signal_response ().connect ( [dialog, p, window, gr, mode]
      (int id)
	{
	  if(id==1)
	    {
	      dialog->close();
	      delete p;
	      delete dialog;
	    }
	  if(id == 0)
	    {
	      std::filesystem::remove(*p);
	      if(mode == 0)
		{
		  gr->WriteJPEG(p->u8string().c_str());
		}

	      if(mode == 1)
		{
		  gr->WritePNG(p->u8string().c_str());
		}
	      dialog->close();
	      delete dialog;
	      delete p;
	      window->close();
	    }
	});
    }
  else
    {
      filename = p->u8string ();
      delete p;
      if (mode == 0)
	{
	  gr->WriteJPEG (filename.c_str ());
	}

      if (mode == 1)
	{
	  gr->WritePNG (filename.c_str ());
	}
      window->close ();
    }
}

bool
GraphicWidget::scrollEvent (double x, double y, mglGraph *gr,
			    Gtk::DrawingArea *drar)
{
  double pl1, pl2;
  pl1 = (plotincr[2] - plotincr[0]) * 0.1;
  pl2 = (plotincr[3] - plotincr[1]) * 0.1;
  if (y < 0)
    {
      plotincr[0] = plotincr[0] + pl1;
      plotincr[1] = plotincr[1] + pl2;
      plotincr[2] = plotincr[2] - pl1;
      plotincr[3] = plotincr[3] - pl2;
    }
  if (y > 0)
    {
      plotincr[0] = plotincr[0] - pl1;
      plotincr[1] = plotincr[1] - pl2;
      plotincr[2] = plotincr[2] + pl1;
      plotincr[3] = plotincr[3] + pl2;
    }
  gr->Zoom (plotincr[0], plotincr[1], plotincr[2], plotincr[3]);
  drar->set_draw_func (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::on_drawedit), gr));
  return true;
}

void
GraphicWidget::dragOperation (double x, double y, mglGraph *gr,
			      Gtk::DrawingArea *drar)
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

  gr->Zoom (plotincr[0], plotincr[1], plotincr[2], plotincr[3]);
  drar->set_draw_func (
      sigc::bind (sigc::mem_fun (*this, &GraphicWidget::on_drawedit), gr));

}

void
GraphicWidget::dragEnd (double x, double y)
{
  X = 0;
  Y = 0;
}

void
GraphicWidget::rightClick (int n_press, double x, double y,
			   Glib::RefPtr<Gtk::GestureClick> cl, mglGraph *gr,
			   Gtk::DrawingArea *drar)
{
  mglPoint p;

  p = gr->CalcXYZ (x, y);

  size_t index = round (p.val (0));

  if (dateplot.size () > 0 && index < dateplot.size () && index >= 0)
    {
      Gtk::Popover *popover = Gtk::make_managed<Gtk::Popover> ();
      popover->set_parent (*drar);
      Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid> ();
      Gtk::Label *label = Gtk::make_managed<Gtk::Label> ();
      label->set_text (Glib::ustring (std::get<0> (dateplot[index])));
      label->set_margin (5);
      label->set_halign (Gtk::Align::CENTER);
      grid->attach (*label, 0, 0, 1, 1);
      if (gr->GetSplId (x, Height - y) > 0)
	{
	  if (gr->GetSplId (x, Height - y) == 21)
	    {
	      Gtk::Label *labpsd = Gtk::make_managed<Gtk::Label> ();
	      labpsd->set_margin (5);
	      labpsd->set_halign (Gtk::Align::START);
	      double tmpd = std::get<1> (dateplot[index]);
	      std::stringstream strm;
	      std::locale loc ("C");
	      strm.imbue (loc);
	      strm << std::fixed << tmpd;
	      labpsd->set_text (gettext ("PPm ") + Glib::ustring (strm.str ()));
	      grid->attach (*labpsd, 0, 1, 1, 1);

	      Gtk::Label *labpsod = Gtk::make_managed<Gtk::Label> ();
	      labpsod->set_margin (5);
	      labpsod->set_halign (Gtk::Align::START);
	      tmpd = std::get<2> (dateplot[index]);
	      strm.clear ();
	      strm.str ("");
	      strm.imbue (loc);
	      strm << std::fixed << tmpd;
	      labpsod->set_text (
		  gettext ("PPTm ") + Glib::ustring (strm.str ()));
	      grid->attach (*labpsod, 0, 2, 1, 1);

	      Gtk::Label *labdif = Gtk::make_managed<Gtk::Label> ();
	      labdif->set_margin (5);
	      labdif->set_halign (Gtk::Align::START);
	      tmpd = std::get<3> (dateplot[index]);
	      strm.clear ();
	      strm.str ("");
	      strm.imbue (loc);
	      strm << std::fixed << tmpd;
	      labdif->set_text (
		  gettext ("PPTm/PPm ") + Glib::ustring (strm.str ()) + "%");
	      grid->attach (*labdif, 0, 3, 1, 1);

	      Gtk::Label *labsp = Gtk::make_managed<Gtk::Label> ();
	      labsp->set_margin (5);
	      labsp->set_halign (Gtk::Align::START);
	      tmpd = std::get<4> (dateplot[index]);
	      strm.clear ();
	      strm.str ("");
	      strm.imbue (loc);
	      strm << std::fixed << tmpd;
	      labsp->set_text (
		  gettext ("Everage speed ") + Glib::ustring (strm.str ()));
	      grid->attach (*labsp, 0, 4, 1, 1);
	    }
	  if (gr->GetSplId (x, Height - y) == 22)
	    {
	      Gtk::Label *labpsd = Gtk::make_managed<Gtk::Label> ();
	      labpsd->set_margin (5);
	      labpsd->set_halign (Gtk::Align::START);
	      double tmpd = std::get<1> (dateplot[index]);
	      std::stringstream strm;
	      std::locale loc ("C");
	      strm.imbue (loc);
	      strm << std::fixed << tmpd;
	      labpsd->set_text (gettext ("PPm ") + Glib::ustring (strm.str ()));
	      grid->attach (*labpsd, 0, 1, 1, 1);

	      Gtk::Label *labpsod = Gtk::make_managed<Gtk::Label> ();
	      labpsod->set_margin (5);
	      labpsod->set_halign (Gtk::Align::START);
	      tmpd = std::get<4> (dateplot[index]);
	      strm.clear ();
	      strm.str ("");
	      strm.imbue (loc);
	      strm << std::fixed << tmpd;
	      labpsod->set_text (
		  gettext ("PPTm ") + Glib::ustring (strm.str ()));
	      grid->attach (*labpsod, 0, 2, 1, 1);

	      Gtk::Label *labdif = Gtk::make_managed<Gtk::Label> ();
	      labdif->set_margin (5);
	      labdif->set_halign (Gtk::Align::START);
	      tmpd = std::get<2> (dateplot[index]);
	      strm.clear ();
	      strm.str ("");
	      strm.imbue (loc);
	      strm << std::fixed << tmpd;
	      labdif->set_text (
		  gettext ("Shares' turnover ") + Glib::ustring (strm.str ()));
	      grid->attach (*labdif, 0, 3, 1, 1);

	      Gtk::Label *labsp = Gtk::make_managed<Gtk::Label> ();
	      labsp->set_margin (5);
	      labsp->set_halign (Gtk::Align::START);
	      tmpd = std::get<3> (dateplot[index]);
	      strm.clear ();
	      strm.str ("");
	      strm.imbue (loc);
	      strm << std::fixed << tmpd;
	      labsp->set_text (
		  gettext ("Money turnover ") + Glib::ustring (strm.str ()));
	      grid->attach (*labsp, 0, 4, 1, 1);
	    }
	  if (gr->GetSplId (x, Height - y) == 32)
	    {
	      Gtk::Label *labpsd = Gtk::make_managed<Gtk::Label> ();
	      labpsd->set_margin (5);
	      labpsd->set_halign (Gtk::Align::START);
	      double tmpd = std::get<1> (dateplot[index]);
	      std::stringstream strm;
	      std::locale loc ("C");
	      strm.imbue (loc);
	      strm << std::fixed << std::setprecision (2) << tmpd;
	      labpsd->set_text (
		  gettext ("Money turnover ") + Glib::ustring (strm.str ()));
	      grid->attach (*labpsd, 0, 1, 1, 1);

	      Gtk::Label *labpsod = Gtk::make_managed<Gtk::Label> ();
	      labpsod->set_margin (5);
	      labpsod->set_halign (Gtk::Align::START);
	      tmpd = std::get<2> (dateplot[index]);
	      strm.clear ();
	      strm.str ("");
	      strm.imbue (loc);
	      strm << std::fixed << std::setprecision (2) << tmpd;
	      labpsod->set_text (
		  gettext ("Average turnover ") + Glib::ustring (strm.str ()));
	      grid->attach (*labpsod, 0, 2, 1, 1);
	    }
	  if (gr->GetSplId (x, Height - y) == 11)
	    {
	      Gtk::Label *labpsd = Gtk::make_managed<Gtk::Label> ();
	      labpsd->set_margin (5);
	      labpsd->set_halign (Gtk::Align::START);
	      double tmpd = std::get<1> (dateplot[index]);
	      std::stringstream strm;
	      std::locale loc ("C");
	      strm.imbue (loc);
	      strm << std::fixed << std::setprecision (2) << tmpd;
	      labpsd->set_text (
		  gettext ("Weighted average price ")
		      + Glib::ustring (strm.str ()));
	      grid->attach (*labpsd, 0, 1, 1, 1);

	      Gtk::Label *labpsod = Gtk::make_managed<Gtk::Label> ();
	      labpsod->set_margin (5);
	      labpsod->set_halign (Gtk::Align::START);
	      tmpd = std::get<2> (dateplot[index]);
	      strm.clear ();
	      strm.str ("");
	      strm.imbue (loc);
	      strm << std::fixed << std::setprecision (2) << tmpd;
	      labpsod->set_text (
		  gettext ("First price ") + Glib::ustring (strm.str ()));
	      grid->attach (*labpsod, 0, 2, 1, 1);

	      Gtk::Label *labpr = Gtk::make_managed<Gtk::Label> ();
	      labpr->set_margin (5);
	      labpr->set_halign (Gtk::Align::START);
	      tmpd = std::get<3> (dateplot[index]);
	      strm.clear ();
	      strm.str ("");
	      strm.imbue (loc);
	      strm << std::fixed << std::setprecision (2) << tmpd;
	      labpr->set_text (
		  gettext ("Last price ") + Glib::ustring (strm.str ()));
	      grid->attach (*labpr, 0, 3, 1, 1);

	      Gtk::Label *labost = Gtk::make_managed<Gtk::Label> ();
	      labost->set_margin (5);
	      labost->set_halign (Gtk::Align::START);
	      tmpd = std::get<4> (dateplot[index]);
	      strm.clear ();
	      strm.str ("");
	      strm.imbue (loc);
	      strm << std::fixed << tmpd;
	      labost->set_text ("ОСт " + Glib::ustring (strm.str ()));
	      grid->attach (*labost, 0, 4, 1, 1);
	    }
	  if (gr->GetSplId (x, Height - y) == 14)
	    {
	      Gtk::Label *labpsd = Gtk::make_managed<Gtk::Label> ();
	      labpsd->set_margin (5);
	      labpsd->set_halign (Gtk::Align::START);
	      double tmpd = std::get<1> (dateplot[index]);
	      std::stringstream strm;
	      std::locale loc ("C");
	      strm.imbue (loc);
	      strm << std::fixed << tmpd;
	      labpsd->set_text ("Цена " + Glib::ustring (strm.str ()));
	      grid->attach (*labpsd, 0, 1, 1, 1);

	      Gtk::Label *labost = Gtk::make_managed<Gtk::Label> ();
	      labost->set_margin (5);
	      labost->set_halign (Gtk::Align::START);
	      tmpd = std::get<2> (dateplot[index]);
	      strm.clear ();
	      strm.str ("");
	      strm.imbue (loc);
	      strm << std::fixed << tmpd;
	      labost->set_text ("ОСт " + Glib::ustring (strm.str ()));
	      grid->attach (*labost, 0, 2, 1, 1);
	    }
	  if (gr->GetSplId (x, Height - y) == 31)
	    {
	      Gtk::Label *labpsd = Gtk::make_managed<Gtk::Label> ();
	      labpsd->set_margin (5);
	      labpsd->set_halign (Gtk::Align::START);
	      double tmpd = std::get<1> (dateplot[index]);
	      std::stringstream strm;
	      std::locale loc ("C");
	      strm.imbue (loc);
	      strm << std::fixed << tmpd;
	      labpsd->set_text (gettext ("PPm ") + Glib::ustring (strm.str ()));
	      grid->attach (*labpsd, 0, 1, 1, 1);

	      Gtk::Label *labpsod = Gtk::make_managed<Gtk::Label> ();
	      labpsod->set_margin (5);
	      labpsod->set_halign (Gtk::Align::START);
	      tmpd = std::get<2> (dateplot[index]);
	      strm.clear ();
	      strm.str ("");
	      strm.imbue (loc);
	      strm << std::fixed << tmpd;
	      labpsod->set_text (
		  gettext ("PPTm ") + Glib::ustring (strm.str ()));
	      grid->attach (*labpsod, 0, 2, 1, 1);
	    }
	  if (gr->GetSplId (x, Height - y) == 12)
	    {
	      Gtk::Label *labpsd = Gtk::make_managed<Gtk::Label> ();
	      labpsd->set_margin (5);
	      labpsd->set_halign (Gtk::Align::START);
	      double tmpd = std::get<1> (dateplot[index]);
	      std::stringstream strm;
	      std::locale loc ("C");
	      strm.imbue (loc);
	      strm << std::fixed << std::setprecision (0) << tmpd;
	      labpsd->set_text (
		  gettext ("Turnover ") + Glib::ustring (strm.str ()));
	      grid->attach (*labpsd, 0, 1, 1, 1);

	      Gtk::Label *labpsod = Gtk::make_managed<Gtk::Label> ();
	      labpsod->set_margin (5);
	      labpsod->set_halign (Gtk::Align::START);
	      tmpd = std::get<2> (dateplot[index]);
	      strm.clear ();
	      strm.str ("");
	      strm.imbue (loc);
	      strm << std::fixed << std::setprecision (2) << tmpd;
	      labpsod->set_text (
		  gettext ("Average turnover ") + Glib::ustring (strm.str ()));
	      grid->attach (*labpsod, 0, 2, 1, 1);
	    }
	}
      popover->set_child (*grid);
      Gdk::Rectangle rect (x, y, 1, 1);
      popover->set_pointing_to (rect);
      popover->popup ();
      popover->signal_hide ().connect ( [popover]
      {
	popover->unparent ();
      });
      popover->present ();
    }
}
