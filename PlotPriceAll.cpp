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

#include "PlotPriceAll.h"

PlotPriceAll::PlotPriceAll (
    std::string file,
    int Height,
    int Width,
    std::vector<std::tuple<std::string, double, double, double, double>> *Plotdate)
{
  filepath = std::filesystem::u8path (file);
  height = Height;
  width = Width;
  plotdate = Plotdate;
}

PlotPriceAll::~PlotPriceAll ()
{
  // TODO Auto-generated destructor stub
}

void
PlotPriceAll::calcForDraw ()
{
  plotdate->clear ();
  mpf_set_default_prec (512);
  std::fstream f;

  std::string line, midd, temp;
  int count = 0;

  std::filesystem::path p;
  AuxFunc af;
  af.homePath (&line);
  line = line + "/.Money/BoardsList";
  p = af.utf8to (line);
  if (std::filesystem::exists (p))
    {
      f.open (p, std::ios_base::in);
      while (!f.eof ())
	{
	  getline (f, line);
	  if (count > 2 && line != "")
	    {
	      af.cp1251toutf8 (line);
	      std::string::size_type nusd, neur;
	      neur = line.find ("EUR");
	      nusd = line.find ("USD");
	      if (neur != std::string::npos || nusd != std::string::npos)
		{
		  temp = line;
		  temp.erase (0, temp.find (";") + std::string (";").size ());
		  temp.erase (0, temp.find (";") + std::string (";").size ());
		  temp = temp.substr (0, temp.find (";"));
		  if (neur != std::string::npos)
		    {
		      eurv.push_back (temp);
		    }
		  if (nusd != std::string::npos)
		    {
		      usdv.push_back (temp);
		    }
		}
	    }
	  count++;
	}
      f.close ();
    }
  line = "";
  temp = "";
  count = 0;

  if (!std::filesystem::exists (filepath))
    {
      std::cout << "File for plot price (all) not opened" << std::endl;
    }
  else
    {
      f.open (filepath, std::ios_base::in);
      while (!f.eof ())
	{
	  getline (f, line);

	  if (count > 1 && line != "")
	    {
	      midd = line;
	      midd = midd.substr (0, midd.find (";"));
	      std::tuple<std::string, double, double, double, double> ttup;
	      std::get<0> (ttup) = midd;

	      midd = line;
	      temp = line;
	      for (int i = 0; i < 1; i++)
		{
		  temp = midd.substr (0, midd.find (";"));
		  midd.erase (0, temp.size () + std::string (";").size ());
		}
	      midd = midd.substr (0, midd.find (";"));
	      double tmpdouble;
	      std::locale loc ("C");
	      std::stringstream strm;
	      strm.imbue (loc);
	      strm << midd;
	      strm >> tmpdouble;
	      Pricemid.push_back (tmpdouble);
	      std::get<1> (ttup) = tmpdouble;

	      midd = line;
	      temp = line;
	      for (int i = 0; i < 2; i++)
		{
		  temp = midd.substr (0, midd.find (";"));
		  midd.erase (0, temp.size () + std::string (";").size ());
		}
	      midd = midd.substr (0, midd.find (";"));
	      strm.str ("");
	      strm.clear ();
	      strm.imbue (loc);
	      strm << midd;
	      strm >> tmpdouble;
	      Pricebeg.push_back (tmpdouble);
	      std::get<2> (ttup) = tmpdouble;

	      midd = line;
	      temp = line;
	      for (int i = 0; i < 3; i++)
		{
		  temp = midd.substr (0, midd.find (";"));
		  midd.erase (0, temp.size () + std::string (";").size ());
		}
	      midd = midd.substr (0, midd.find (";"));
	      strm.str ("");
	      strm.clear ();
	      strm.imbue (loc);
	      strm << midd;
	      strm >> tmpdouble;
	      Priceend.push_back (tmpdouble);
	      std::get<3> (ttup) = tmpdouble;

	      midd = line;
	      temp = line;
	      for (int i = 0; i < 7; i++)
		{
		  temp = midd.substr (0, midd.find (";"));
		  midd.erase (0, temp.size () + std::string (";").size ());
		}
	      midd = midd.substr (0, midd.find (";"));
	      strm.str ("");
	      strm.clear ();
	      strm.imbue (loc);
	      strm << midd;
	      strm >> tmpdouble;
	      Tc.push_back (1 / tmpdouble);
	      std::get<4> (ttup) = 1 / tmpdouble;
	      plotdate->push_back (ttup);
	    }

	  count = count + 1;
	}
      f.close ();
    }

  if (plotdate->size () > 0)
    {
      datebeg = std::get<0> (plotdate->at (0));
      dateend = std::get<0> (plotdate->at (plotdate->size () - 1));
    }
}

int
PlotPriceAll::Draw (mglGraph *gr)
{
  calcForDraw ();
  std::vector<int> X;
  std::vector<double> comp;
  for (size_t i = 0; i < Pricemid.size (); i++)
    {
      X.push_back (i);
    }
  mglData x (X), y (Pricemid), y1 (Pricebeg), y2 (Priceend), y3 (Tc);

  comp.push_back (y.Maximal ());
  comp.push_back (y1.Maximal ());
  comp.push_back (y2.Maximal ());
  comp.push_back (y.Minimal ());
  comp.push_back (y1.Minimal ());
  comp.push_back (y2.Minimal ());

  double maxval = *std::max_element (comp.begin (), comp.end ());
  double minval = *std::min_element (comp.begin (), comp.end ());

  mglPoint p1 (x.Minimal (), maxval + (maxval - minval) * 0.02);
  mglPoint p5 (x.Maximal (), maxval + (maxval - minval) * 0.02);

  int d = 6;
  int number = X.size ();
  while (number % d > 0)
    {
      number = number + 1;
    }
  d = number / d;

  std::vector<mglPoint> Coord;
  std::vector<std::string> dates;
  for (size_t i = 0; i < X.size (); i = i + d)
    {
      if (i > 0)
	{
	  mglPoint p (i, maxval + (maxval - minval) * 0.02);
	  Coord.push_back (p);
	  dates.push_back (std::get<0> (plotdate->at (i)));
	}
    }

  AuxFunc af;
  std::string grnm = gettext ("Price");
  grnm = af.utf8to (grnm);

  gr->SetObjId (11);
  gr->SetSize (width, height);
  gr->Title (grnm.c_str (), "", 5);
  gr->SetQuality (3);
  gr->SetRange ('x', x);
  gr->SetRange ('y', minval, maxval);
  gr->SetFontSize (3);
  gr->SetOriginTick (false);

  std::vector<double> ticks;
  double tickstep = (maxval - minval) / 3;
  if (tickstep < 0)
    {
      tickstep = -tickstep;
    }
  double tickval = minval;
  std::stringstream strm;
  std::locale loc ("C");
  std::string tickstr, tick;
  for (int i = 0; i < 2; i++)
    {

      strm.str ("");
      strm.clear ();
      strm.imbue (loc);
      strm << std::fixed;
      strm << std::setprecision (3);
      tickval = tickval + tickstep;
      strm << tickval;
      tick = strm.str ();
      ticks.push_back (tickval);
      if (tickstr != "")
	{
	  tickstr = tickstr + "\n" + tick;
	}
      else
	{
	  tickstr = tick;
	}
    }
  mglData fortick (ticks);

  tickstr = af.utf8to (tickstr);

  gr->SetTicksVal ('y', fortick, tickstr.c_str ());
  gr->SetTicks ('x', d);
  gr->Grid ("xy", "{xA0A136}");
  gr->SetTickSkip (true);
  gr->Axis ("y", "k");
  gr->Axis ("!f0x", "k");
  std::string yname;
  yname = filepath.parent_path ().filename ().u8string ();
  auto itusd = std::find (usdv.begin (), usdv.end (), yname);
  if (itusd != usdv.end ())
    {
      yname = "USD";
    }
  auto iteur = std::find (eurv.begin (), eurv.end (), yname);
  if (iteur != eurv.end ())
    {
      yname = "EUR";
    }
  if (itusd == usdv.end () && iteur == eurv.end ())
    {
      yname = gettext ("Rubles");
    }
  gr->Label ('x', af.utf8to (gettext ("Days")).c_str (), 0);
  gr->Label ('y', af.utf8to (yname).c_str (), 0);
  datebeg = af.utf8to (datebeg);
  dateend = af.utf8to (dateend);

  gr->Puts (p1, datebeg.c_str (), "k", 3);
  gr->Puts (p5, dateend.c_str (), "k", 3);

  for (size_t i = 0; i < dates.size (); i++)
    {
      std::string tmpl = dates[i];
      tmpl = af.utf8to (tmpl);
      gr->Puts (Coord[i], tmpl.c_str (), "k", 3);
    }

  gr->Plot (x, y, "r");
  gr->Plot (x, y1, "b");
  gr->Plot (x, y2, "g");
  gr->Plot (x, y3, "k");
  gr->AddLegend (af.utf8to (gettext ("Weighted average price")).c_str (), "r");
  gr->AddLegend (af.utf8to (gettext ("First")).c_str (), "b");
  gr->AddLegend (af.utf8to (gettext ("Last")).c_str (), "g");
  gr->AddLegend (af.utf8to (gettext ("ECg")).c_str (), "k");
  gr->SetFontSize (3);
  gr->Legend (1.3, 1.5);

  return 0;
}

