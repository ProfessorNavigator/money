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

#include "PlotGlobalCommon.h"

PlotGlobalCommon::PlotGlobalCommon (
    std::string file,
    int Height,
    int Width,
    std::vector<std::tuple<std::string, double, double, double, double>> *Plotdate)
{
  filename = std::filesystem::u8path (file);
  height = Height;
  width = Width;
  plotdate = Plotdate;
}

PlotGlobalCommon::~PlotGlobalCommon ()
{
  // TODO Auto-generated destructor stub
}

void
PlotGlobalCommon::calcForDraw ()
{
  plotdate->clear ();
  std::string line, temp;
  int count = 0;
  std::fstream f;
  if (!std::filesystem::exists (filename))
    {
      std::cout << "File to plot common (Global) graphics not openned"
	  << std::endl;
    }
  else
    {
      f.open (filename, std::ios_base::in);
      while (!f.eof ())
	{
	  getline (f, line);
	  if (count > 0 && line != "")
	    {
	      temp = line;
	      for (int i = 1; i < 5; i++)
		{
		  temp.erase (0, temp.find (";") + std::string (";").size ());
		}
	      std::stringstream strm;
	      std::locale loc ("C");
	      double tmpd;
	      strm.imbue (loc);
	      strm << temp;
	      strm >> tmpd;
	      std::tuple<std::string, double, double, double, double> ttup;
	      Index.push_back (tmpd);
	      std::get<1> (ttup) = tmpd;

	      temp = line;
	      temp.erase (0, temp.find (";") + std::string (";").size ());
	      temp = temp.substr (0, temp.find (";"));
	      strm.str ("");
	      strm.clear ();
	      strm.imbue (loc);
	      strm << temp;
	      strm >> tmpd;
	      Volume.push_back (tmpd);
	      std::get<2> (ttup) = tmpd;

	      temp = line;
	      temp.erase (0, temp.find (";") + std::string (";").size ());
	      temp.erase (0, temp.find (";") + std::string (";").size ());
	      temp = temp.substr (0, temp.find (";"));
	      strm.str ("");
	      strm.clear ();
	      strm.imbue (loc);
	      strm << temp;
	      strm >> tmpd;
	      Money.push_back (tmpd);
	      std::get<3> (ttup) = tmpd;

	      temp = line;
	      for (int i = 1; i < 4; i++)
		{
		  temp.erase (0, temp.find (";") + std::string (";").size ());
		}
	      temp = temp.substr (0, temp.find (";"));
	      strm.str ("");
	      strm.clear ();
	      strm.imbue (loc);
	      strm << temp;
	      strm >> tmpd;
	      Daily.push_back (tmpd);
	      std::get<4> (ttup) = tmpd;

	      temp = line;
	      temp = temp.substr (0, temp.find (";"));
	      std::get<0> (ttup) = temp;
	      plotdate->push_back (ttup);
	    }
	  count = count + 1;
	}
      if (plotdate->size () > 0)
	{
	  datebeg = std::get<0> (plotdate->at (0));
	  dateend = std::get<0> (plotdate->at (plotdate->size () - 1));
	}
      f.close ();
    }
}

int
PlotGlobalCommon::Draw (mglGraph *gr)
{
  calcForDraw ();
  std::vector<int> X;
  for (size_t i = 0; i < Daily.size (); i++)
    {
      X.push_back (i);
    }
  mglData x (X), y (Daily), y2 (Index), y3 (Volume), y4 (Money);

  //Даты для ПСд
  mglPoint p1 (x.Minimal (),
	       y.Maximal () + ((y.Maximal () - y.Minimal ()) * 0.02));
  mglPoint p5 (x.Maximal (),
	       y.Maximal () + ((y.Maximal () - y.Minimal ()) * 0.02));

  //Даты для оборота акций
  mglPoint p2 (x.Minimal (),
	       y3.Maximal () + ((y3.Maximal () - y3.Minimal ()) * 0.02));
  mglPoint p3 (x.Maximal (),
	       y3.Maximal () + ((y3.Maximal () - y3.Minimal ()) * 0.02));

  //Даты для оборота денег
  mglPoint p4 (x.Minimal (),
	       y4.Maximal () + ((y4.Maximal () - y4.Minimal ()) * 0.02));
  mglPoint p6 (x.Maximal (),
	       y4.Maximal () + ((y4.Maximal () - y4.Minimal ()) * 0.02));

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
	  mglPoint p (i, y.Maximal () + ((y.Maximal () - y.Minimal ()) * 0.02));
	  Coord.push_back (p);
	  dates.push_back (std::get<0> (plotdate->at (i)));
	}
    }
  //График ПСд
  gr->SetSize (width, height);
  gr->SetObjId (22);
  gr->SubPlot (1, 3, 0, ">^_");
  AuxFunc af;
  gr->Title (af.utf8to (gettext ("Purchasing power of money")).c_str (), "", 5);
  gr->SetQuality (3);
  gr->SetRange ('x', x);
  gr->SetRange ('y', y);
  gr->SetFontSize (3);
  gr->SetOriginTick (false);

  std::vector<double> ticks;
  double tickstep = (y.Maximal () - y.Minimal ()) / 3;
  if (tickstep < 0)
    {
      tickstep = -tickstep;
    }
  double tickval = y.Minimal ();
  std::stringstream strm;
  std::locale loc ("C");
  std::string tickstr, tick;
  for (int i = 0; i < 2; i++)
    {
      strm.str ("");
      strm.clear ();
      strm.imbue (loc);
      strm << std::fixed;
      strm << std::setprecision (0);
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
  gr->SetTickSkip (true);
  gr->Grid ("xy", "{xA0A136}");
  gr->Axis ("y", "k");
  gr->Axis ("!f0x", "k");
  gr->Label ('x', af.utf8to (gettext ("Days")).c_str (), 0);
  gr->Label ('y', af.utf8to (gettext ("Shares/Ruble")).c_str (), 0);
  gr->Plot (x, y, "g");
  gr->Plot (x, y2, "r");
  gr->AddLegend (af.utf8to (gettext ("PPTm")).c_str (), "g");
  gr->AddLegend (af.utf8to (gettext ("PPm")).c_str (), "r");
  gr->Legend (1.0, 1.3);

  datebeg = af.utf8to (datebeg);
  dateend = af.utf8to (dateend);
  gr->Puts (p1, datebeg.c_str (), "k", 3);
  gr->Puts (p5, dateend.c_str (), "k", 3);

  for (size_t i = 0; i < dates.size (); i++)
    {
      std::string tmp = dates[i];
      tmp = af.utf8to (tmp);
      gr->Puts (Coord[i], tmp.c_str (), "k", 3);
    }

  //График оборота акций

  Coord.clear ();
  for (size_t i = 0; i < X.size (); i = i + d)
    {
      if (i > 0)
	{
	  mglPoint p (i,
		      y3.Maximal () + ((y3.Maximal () - y3.Minimal ()) * 0.02));
	  Coord.push_back (p);
	}
    }

  gr->SetObjId (22);
  gr->SubPlot (1, 3, 1, ">^_");
  gr->Title (af.utf8to (gettext ("Shares turnover")).c_str (), "", 5);
  gr->SetQuality (3);
  gr->SetRange ('x', x);
  gr->SetRange ('y', y3);
  gr->SetFontSize (3);
  gr->SetOriginTick (false);

  ticks.clear ();
  tickstep = (y3.Maximal () - y3.Minimal ()) / 3;
  if (tickstep < 0)
    {
      tickstep = -tickstep;
    }
  tickval = y3.Minimal ();
  tickstr = "";
  for (int i = 0; i < 2; i++)
    {
      strm.str ("");
      strm.clear ();
      strm.imbue (loc);
      strm << std::fixed;
      strm << std::setprecision (0);
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
  mglData fortick2 (ticks);
  tickstr = af.utf8to (tickstr);

  gr->SetTicksVal ('y', fortick2, tickstr.c_str ());
  gr->SetTickSkip (true);
  gr->Grid ("xy", "{xA0A136}");
  gr->Axis ("y", "k");
  gr->Axis ("!f0x", "k");
  gr->Label ('x', af.utf8to (gettext ("Days")).c_str (), 0);
  gr->Label ('y', af.utf8to (gettext ("Shares quantity")).c_str (), 0);

  gr->ClearLegend ();
  gr->Plot (x, y3, "r");
  gr->AddLegend (af.utf8to (gettext ("Shares turnover")).c_str (), "r");
  gr->Legend (1.0, 1.3);

  gr->Puts (p2, datebeg.c_str (), "k", 3);
  gr->Puts (p3, dateend.c_str (), "k", 3);
  for (size_t i = 0; i < dates.size (); i++)
    {
      std::string tmp = dates[i];
      tmp = af.utf8to (tmp);
      gr->Puts (Coord[i], tmp.c_str (), "k", 3);
    }

  //График оборота денег
  Coord.clear ();
  for (size_t i = 0; i < X.size (); i = i + d)
    {
      if (i > 0)
	{
	  mglPoint p (i,
		      y4.Maximal () + ((y4.Maximal () - y4.Minimal ()) * 0.02));
	  Coord.push_back (p);
	}
    }

  gr->SetObjId (22);
  gr->SubPlot (1, 3, 2, ">^_");
  gr->Title (af.utf8to (gettext ("Money turnover")).c_str (), "", 5);
  gr->SetQuality (3);
  gr->SetRange ('x', x);
  gr->SetRange ('y', y4);
  gr->SetFontSize (3);
  gr->SetOriginTick (false);

  ticks.clear ();
  tickstep = (y4.Maximal () - y4.Minimal ()) / 3;
  if (tickstep < 0)
    {
      tickstep = -tickstep;
    }
  tickval = y4.Minimal ();
  tickstr = "";
  for (int i = 0; i < 2; i++)
    {
      strm.str ("");
      strm.clear ();
      strm.imbue (loc);
      strm << std::fixed;
      strm << std::setprecision (0);
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
  mglData fortick3 (ticks);
  tickstr = af.utf8to (tickstr);

  gr->SetTicksVal ('y', fortick3, tickstr.c_str ());
  gr->SetTickSkip (true);
  gr->Grid ("xy", "{xA0A136}");
  gr->Axis ("y", "k");
  gr->Axis ("!f0x", "k");
  gr->Label ('x', af.utf8to (gettext ("Days")).c_str (), 0);
  gr->Label ('y', af.utf8to (gettext ("Rubles")).c_str (), 0);

  gr->Plot (x, y4, "{x8900FF}");
  gr->ClearLegend ();
  gr->AddLegend (af.utf8to (gettext ("Money turnover")).c_str (), "{x8900FF}");
  gr->Legend (1.0, 1.3);

  gr->Puts (p4, datebeg.c_str (), "k", 3);
  gr->Puts (p6, dateend.c_str (), "k", 3);
  for (size_t i = 0; i < dates.size (); i++)
    {
      std::string tmp = dates[i];
      tmp = af.utf8to (tmp);
      gr->Puts (Coord[i], tmp.c_str (), "k", 3);
    }

  return 0;
}

