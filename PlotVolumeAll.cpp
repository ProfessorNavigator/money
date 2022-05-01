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

#include "PlotVolumeAll.h"

PlotVolumeAll::PlotVolumeAll (
    std::string file,
    int Height,
    int Width,
    std::vector<std::tuple<std::string, double, double, double, double>> *Plotdate)
{
  filename = std::filesystem::u8path (file);
  width = Width;
  height = Height;
  plotdate = Plotdate;
}

PlotVolumeAll::~PlotVolumeAll ()
{
  // TODO Auto-generated destructor stub
}

void
PlotVolumeAll::calcForDraw ()
{
  plotdate->clear ();
  mpf_set_default_prec (512);
  int count = 0;
  std::fstream f;
  std::string line, midd, temp;
  if (!std::filesystem::exists (filename))
    {
      std::cout << "File for plot volume (All) not opened" << std::endl;
    }
  else
    {
      f.open (filename, std::ios_base::in);
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
	      for (int i = 0; i < 4; i++)
		{
		  temp = midd.substr (0, midd.find (";"));
		  midd.erase (0, temp.size () + std::string (";").size ());
		}
	      midd = midd.substr (0, midd.find (";"));
	      std::stringstream strm;
	      std::locale loc ("C");
	      double tmpdouble;
	      strm.imbue (loc);
	      strm << midd;
	      strm >> tmpdouble;
	      Vol.push_back (tmpdouble);
	      std::get<1> (ttup) = tmpdouble;
	      plotdate->push_back (ttup);
	    }
	  count = count + 1;
	}
      f.close ();
    }
  mpf_class summ (0);
  mpf_class vols (0);
  mpf_class res (0);
  for (size_t i = 0; i < Vol.size (); i++)
    {
      summ = summ + 1;
      vols = vols + Vol[i];
      res = vols / summ;
      Volmid.push_back (res.get_d ());
      std::get<2> (plotdate->at (i)) = Volmid[i];
    }

  if (plotdate->size () > 0)
    {
      datebeg = std::get<0> (plotdate->at (0));
      dateend = std::get<0> (plotdate->at (plotdate->size () - 1));
    }
}

int
PlotVolumeAll::Draw (mglGraph *gr)
{
  calcForDraw ();
  std::vector<int> X;
  for (size_t i = 0; i < Vol.size (); i++)
    {
      X.push_back (i);
    }
  mglData x (X), y (Vol), y2 (Volmid);

  //Координаты подписей оси х
  mglPoint p1 (x.Minimal (),
	       y.Maximal () + ((y.Maximal () - y.Minimal ()) * 0.02));
  mglPoint p5 (x.Maximal (),
	       y.Maximal () + ((y.Maximal () - y.Minimal ()) * 0.02));

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

  AuxFunc af;
  std::string grnm = gettext ("Shares turnover");
  grnm = af.utf8to (grnm);

  //Общие настройки графика
  gr->SetObjId (12);
  gr->SetSize (width, height);
  gr->Title (grnm.c_str (), "", 5);
  gr->SetQuality (3);
  gr->SetRanges (x, y);
  gr->SetFontSize (3);

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
  gr->Grid ("xy", "{xA0A136}");
  gr->SetTickSkip (true);
  gr->SetOriginTick (false);
  gr->Axis ("y", "k");
  gr->Axis ("x!f0", "k");
  gr->Label ('x', af.utf8to (gettext ("Days")).c_str (), 0);
  gr->Label ('y', af.utf8to (gettext ("Shares quantity")).c_str (), 0);

  //Подписи оси х
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

  //Отображение графика
  gr->Plot (x, y, "g");
  gr->Plot (x, y2, "r");
  gr->AddLegend (af.utf8to (gettext ("Daily turnover")).c_str (), "g");
  gr->AddLegend (af.utf8to (gettext ("Average turnover")).c_str (), "r");
  gr->SetFontSize (3);
  gr->Legend (1.1, 1.3);
  return 0;
}

