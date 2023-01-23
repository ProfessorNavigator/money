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

#include "DownloadAll.h"

DownloadAll::DownloadAll(
    int *cancel,
    std::vector<std::tuple<std::string, std::string, std::string>> *instruments,
    std::vector<std::tuple<std::string, std::string, std::string>> *instrumentsF,
    std::vector<std::tuple<std::string, std::string>> *boards, double db)
{
  Cancel = cancel;
  Instruments = *instruments;
  InstrumentsF = *instrumentsF;
  Boards = *boards;
  Db = db;
}

DownloadAll::~DownloadAll()
{
  // TODO Auto-generated destructor stub
}

void
DownloadAll::downloadAll()
{
  AuxFunc af;
  std::string dirstr = af.tempPath();
  dirstr = dirstr + "/Money";
  std::filesystem::path dir;
  dir = std::filesystem::u8path(dirstr);

  if(!std::filesystem::exists(dir))
    {
      std::filesystem::create_directory(dir);
    }

  int check = 0;
  int errorcount = 0;
  int linecount = 0;
  std::string line, tmp, tmp2, writevalue;
  double bd = Db;
  std::fstream f;
  dirstr = dirstr + "/Dates";
  for(;;)
    {
      if(pulse)
	{
	  pulse();
	}
      af.callNetwork(
	  "https://iss.moex.com/iss/history/engines/stock/markets/shares/dates.csv",
	  dirstr, &check);
      if(check > 0)
	{
	  break;
	}
      else
	{
	  usleep(100000);
	  errorcount++;
	}
      if(errorcount > 100)
	{
	  if(globError)
	    {
	      globError();
	    }
	  return void();
	}
    }
  check = 0;
  errorcount = 0;
  dir = dirstr;
  f.open(dir, std::ios_base::in);
  double dt = 0.0;
  while(!f.eof())
    {
      getline(f, line);
      if(linecount == 3)
	{
	  line = line.erase(0, line.find(";") + std::string(";").size());
	  int year, month, day;
	  tmp = line;
	  tmp = tmp.substr(0, tmp.find("-"));
	  std::stringstream strm;
	  std::locale loc("C");
	  strm.imbue(loc);
	  strm << tmp;
	  strm >> year;
	  tmp = line;
	  tmp = tmp.erase(0, tmp.find("-") + std::string("-").size());
	  tmp = tmp.substr(0, tmp.find("-"));

	  strm.clear();
	  strm.str();
	  strm.imbue(loc);
	  strm << tmp;
	  strm >> month;
	  tmp = line;
	  tmp = tmp.erase(0, tmp.find("-") + std::string("-").size());
	  tmp = tmp.erase(0, tmp.find("-") + std::string("-").size());

	  strm.clear();
	  strm.str();
	  strm.imbue(loc);
	  strm << tmp;
	  strm >> day;
	  dt = af.grigtojulian(day, month, year, 0, 0, 0.0);
	}
      linecount++;
    }
  f.close();
  linecount = 0;
  std::vector<std::vector<std::string>> boardsvect;
  for(size_t i = 0; i < Boards.size(); i++)
    {
      std::vector<std::string> vect;
      boardsvect.push_back(vect);
    }
  line.clear();
  if(bd < dt)
    {
      bd = bd + 1.0;
      std::string adress, filename;
      int dcount = 0;
      if(progress)
	{
	  progress(0);
	}
      for(size_t i = 0; i < Instruments.size(); i++)
	{
	  std::tuple<int, int, int, int, int> tupforsave;
	  if(pulse)
	    {
	      pulse();
	    }
	  if(*Cancel == 1)
	    {
	      if(canceled)
		{
		  canceled();
		}
	      return void();
	    }
	  dcount = 0;
	  check = 0;
	  errorcount = 0;
	  dirstr = af.tempPath();
	  dirstr = dirstr + "/Money/LoadAll";
	  dir = std::filesystem::u8path(dirstr);
	  std::filesystem::remove_all(dir);
	  std::filesystem::create_directories(dir);
	  for(;;)
	    {
	      if(pulse)
		{
		  pulse();
		}
	      if(*Cancel == 1)
		{
		  if(canceled)
		    {
		      canceled();
		    }
		  return void();
		}
	      adress =
		  "https://iss.moex.com/iss/history/engines/stock/markets/shares/securities/"
		      + std::get<0>(Instruments.at(i)) + ".csv?from="
		      + af.togrigyear(bd) + "-" + af.togrigmonth(bd) + "-"
		      + af.togrigday(bd) + "&till=" + af.togrigyear(dt) + "-"
		      + af.togrigmonth(dt) + "-" + af.togrigday(dt) + "&start="
		      + std::to_string(dcount);
	      std::string sustring(std::get<0>(Instruments.at(i)));
	      auto itinf = std::find_if(InstrumentsF.begin(),
					InstrumentsF.end(), [&sustring]
					(auto &el)
					  {
					    return std::get<0>(el) == sustring;
					  });
	      if(itinf != InstrumentsF.end())
		{
		  adress =
		      "https://iss.moex.com/iss/history/engines/stock/markets/foreignshares/securities/"
			  + std::get<0>(Instruments.at(i)) + ".csv?from="
			  + af.togrigyear(bd) + "-" + af.togrigmonth(bd) + "-"
			  + af.togrigday(bd) + "&till=" + af.togrigyear(dt)
			  + "-" + af.togrigmonth(dt) + "-" + af.togrigday(dt)
			  + "&start=" + std::to_string(dcount);
		}
	      filename = dirstr + "/" + std::get<0>(Instruments.at(i))
		  + std::to_string(dcount);
	      for(;;)
		{
		  af.callNetwork(adress, filename, &check);
		  if(check > 0)
		    {
		      break;
		    }
		  else
		    {
		      errorcount++;
		    }
		  if(errorcount > 100)
		    {
		      if(netError)
			{
			  netError();
			}
		      return void();
		    }
		}

	      linecount = 0;
	      f.open(std::filesystem::path(std::filesystem::u8path(filename)),
		     std::ios_base::in);
	      while(!f.eof())
		{
		  getline(f, line);
		  if(linecount == 3)
		    {
		      break;
		    }
		  linecount++;
		}
	      f.close();
	      if(line.empty())
		{
		  std::filesystem::path filepath;
		  filepath = filename;
		  std::filesystem::remove(filepath);
		  break;
		}
	      dcount = dcount + 100;
	    }
	  dcount = 0;
	  for(;;)
	    {
	      if(pulse)
		{
		  pulse();
		}
	      if(*Cancel == 1)
		{
		  if(canceled)
		    {
		      canceled();
		    }
		  return void();
		}
	      linecount = 0;
	      line = af.tempPath();
	      line = line + "/Money/LoadAll/" + std::get<0>(Instruments.at(i))
		  + std::to_string(dcount);
	      dir = std::filesystem::u8path(line);

	      if(std::filesystem::exists(dir))
		{
		  std::tuple<int, int> temptup;
		  f.open(dir, std::ios_base::in);
		  while(!f.eof())
		    {
		      getline(f, line);
		      af.cp1251toutf8(line);
		      std::string temp;
		      if(linecount == 2)
			{
			  tmp = line;
			  int countch = 0;
			  while(tmp.size() > 0)
			    {
			      temp = tmp.substr(0, tmp.find(";"));
			      std::string::size_type n;
			      if(temp == "BOARDID")
				{
				  std::get<0>(temptup) = countch;
				}
			      if(temp == "VOLUME")
				{
				  std::get<1>(temptup) = countch;
				  std::get<2>(tupforsave) = countch;
				}
			      if(temp == "TRADEDATE")
				{
				  std::get<0>(tupforsave) = countch;
				}
			      if(temp == "VALUE")
				{
				  std::get<1>(tupforsave) = countch;
				}
			      if(temp == "OPEN")
				{
				  std::get<3>(tupforsave) = countch;
				}
			      if(temp == "CLOSE")
				{
				  std::get<4>(tupforsave) = countch;
				}
			      n = tmp.find(";");
			      if(n != std::string::npos)
				{
				  tmp.erase(0, n + std::string(";").size());
				}
			      else
				{
				  break;
				}
			      countch++;
			    }
			}

		      if(linecount > 2 && !line.empty())
			{
			  tmp = line;
			  for(int i = 0; i < std::get<0>(temptup); i++)
			    {
			      temp = tmp.substr(0, tmp.find(";"));
			      tmp = tmp.erase(
				  0, temp.size() + std::string(";").size());
			    }
			  tmp = tmp.substr(0, tmp.find(";"));

			  tmp2 = line;
			  for(int i = 0; i < std::get<1>(temptup); i++)
			    {
			      temp = tmp2.substr(0, tmp2.find(";"));
			      tmp2 = tmp2.erase(
				  0, temp.size() + std::string(";").size());
			    }
			  tmp2 = tmp2.substr(0, tmp2.find(";"));
			  auto it = std::find_if(
			      Boards.begin(), Boards.end(), [tmp]
			      (auto &element)
				{
				  return std::get<0>(element) == tmp;
				});
			  if(it != Boards.end() && tmp2 != "0")
			    {
			      int index = std::distance(Boards.begin(), it);
			      boardsvect.at(index).push_back(line);
			    }
			}
		      linecount++;
		    }
		  f.close();
		}
	      else
		{
		  break;
		}
	      dcount = dcount + 100;
	    }
	  for(size_t j = 0; j < boardsvect.size(); j++)
	    {
	      if(boardsvect[j].size() > 0)
		{
		  saveRes(boardsvect[j], std::get<0>(Instruments.at(i)),
			  std::get<0>(Boards.at(j)), tupforsave);
		}
	    }
	  if(progress)
	    {
	      progress(
		  static_cast<double>(i + 1)
		      / static_cast<double>(Instruments.size()));
	    }
	  for(size_t j = 0; j < boardsvect.size(); j++)
	    {
	      boardsvect.at(j).clear();
	    }
	}
      if(progress)
	{
	  progress(1.0);
	}
      if(saveDate)
	{
	  if(dt < 2450531.0)
	    {
	      dt = 2450531.0;
	    }
	  saveDate(dt);
	}
      if(finished)
	{
	  finished();
	}
    }
  else
    {
      if(saveDate)
	{
	  if(dt < 2450531.0)
	    {
	      dt = 2450531.0;
	    }
	  saveDate(dt);
	}
      if(finished)
	{
	  finished();
	}
    }
}

void
DownloadAll::saveRes(std::vector<std::string> &source, std::string instrname,
		     std::string boardid,
		     std::tuple<int, int, int, int, int> tupforsave)
{
  std::string dirstr;
  AuxFunc af;
  af.homePath(&dirstr);
  std::filesystem::path dir;
  dirstr = dirstr + "/Money/" + boardid;
  dir = std::filesystem::u8path(dirstr);
  if(!std::filesystem::exists(dir))
    {
      std::filesystem::create_directories(dir);
    }
  mpf_set_default_prec(1024);
  mpf_class Volume(0), Money(0), PSD(0), PSOD(0);
  int linecount = 0;
  double money, volume;
  std::vector<std::string> finalv;
  std::string line, writevalue;
  std::fstream f;
  std::filesystem::path finalpath;
  af.homePath(&line);
  std::string finalpathstr = line + "/Money/" + boardid + "/" + instrname
      + ".csv";
  finalpath = std::filesystem::u8path(finalpathstr);
  std::vector<std::tuple<std::string, int, int, int>> split;
  std::string split_addr;
  af.homePath(&split_addr);
  split_addr = split_addr + "/.Money/SplitList";
  std::filesystem::path split_path = std::filesystem::u8path(split_addr);
  f.open(split_path, std::ios_base::in);
  if(f.is_open())
    {
      while(!f.eof())
	{
	  std::string line;
	  getline(f, line);
	  if(!line.empty())
	    {
	      std::tuple<std::string, int, int, int> ttup;
	      std::get<0>(ttup) = line.substr(0, line.find("<%>"));
	      if(instrname == std::get<0>(ttup))
		{
		  std::string date = line;
		  date.erase(0, date.find("<%>") + std::string("<%>").size());
		  std::string val = date.substr(0, date.find("."));
		  std::stringstream strm;
		  std::locale loc("C");
		  strm.imbue(loc);
		  strm << val;
		  int day;
		  strm >> day;

		  val = date;
		  val.erase(0, val.find(".") + std::string(".").size());
		  val = val.substr(0, val.find("."));
		  strm.clear();
		  strm.str("");
		  strm.imbue(loc);
		  strm << val;
		  int month;
		  strm >> month;

		  val = date;
		  val.erase(0, val.rfind(".") + std::string(".").size());
		  strm.clear();
		  strm.str("");
		  strm.imbue(loc);
		  strm << val;
		  int year;
		  strm >> year;

		  if(day > 0 && month > 0 && year > 0)
		    {
		      std::get<1>(ttup) = day;
		      std::get<2>(ttup) = month;
		      std::get<3>(ttup) = year;
		      split.push_back(ttup);
		    }
		}
	    }
	}
      f.close();
    }
  std::sort(
      split.begin(),
      split.end(),
      [&af]
      (auto &el1,
       auto &el2)
	 {
	   double f =
	   af.grigtojulian(std::get<1>(el1), std::get<2>(el1), std::get<3>(el1), 0, 0, 0);
	   double s =
	   af.grigtojulian(std::get<1>(el2), std::get<2>(el2), std::get<3>(el2), 0, 0, 0);
	   return f < s;
	 });

  std::vector<std::tuple<std::string, int, int, int>>::iterator itspl;
  if(split.size() > 0)
    {
      itspl = split.begin();
    }
  else
    {
      itspl = split.end();
    }
  if(!std::filesystem::exists(finalpath))
    {
      finalv.push_back(instrname + "\n");
      finalv.push_back(
	  "Дата;Средневзвешенная цена;Первая цена;Последняя цена;Объем, шт.;"
	  "Объем оборота денег;ПСОд;ПСд\n");
    }
  else
    {
      std::tuple<int, int> temptup;
      std::string temp;
      f.open(finalpath, std::ios_base::in);
      while(!f.eof())
	{
	  getline(f, line);
	  if(linecount == 1)
	    {
	      std::string tmp = line;
	      int countch = 0;
	      while(tmp.size() > 0)
		{
		  temp = tmp.substr(0, tmp.find(";"));
		  std::string::size_type n;
		  if(temp == "Объем, шт.")
		    {
		      std::get<0>(temptup) = countch;
		    }
		  if(temp == "Объем оборота денег")
		    {
		      std::get<1>(temptup) = countch;
		    }
		  n = tmp.find(";");
		  if(n != std::string::npos)
		    {
		      tmp.erase(0, n + std::string(";").size());
		    }
		  else
		    {
		      break;
		    }
		  countch++;
		}
	    }
	  if(!line.empty())
	    {
	      line = line + "\n";
	      finalv.push_back(line);
	      if(linecount > 1)
		{
		  std::string tmp;
		  tmp = line;
		  for(int i = 0; i < std::get<0>(temptup); i++)
		    {
		      temp = tmp.substr(0, tmp.find(";"));
		      tmp = tmp.erase(0, temp.size() + std::string(";").size());
		    }
		  tmp = tmp.substr(0, tmp.find(";"));
		  mpf_class tmpvol, tmpmon;
		  std::stringstream strm;
		  std::locale loc("C");
		  strm.imbue(loc);
		  strm << tmp;
		  strm >> tmpvol;

		  tmp = line;
		  for(int i = 0; i < std::get<1>(temptup); i++)
		    {
		      temp = tmp.substr(0, tmp.find(";"));
		      tmp = tmp.erase(0, temp.size() + std::string(";").size());
		    }
		  tmp = tmp.substr(0, tmp.find(";"));
		  strm.str("");
		  strm.clear();
		  strm.imbue(loc);
		  strm << tmp;
		  strm >> tmpmon;
		  Volume = Volume + tmpvol;
		  Money = Money + tmpmon;
		}
	    }
	  linecount++;
	}
      f.close();
    }
  std::string temp;
  bool apply_split = false;
  for(size_t i = 0; i < source.size(); i++)
    {
      line = source[i];
      for(int j = 0; j < std::get<2>(tupforsave); j++)
	{
	  temp = line.substr(0, line.find(";"));
	  line = line.erase(0, temp.size() + std::string(";").size());
	}
      line = line.substr(0, line.find(";"));
      std::stringstream strm;
      std::locale loc("C");
      strm.imbue(loc);
      strm << line;
      int checkl;
      strm >> checkl;

      if(checkl > 0)
	{
	  line = source[i];
	  for(int j = 0; j < std::get<0>(tupforsave); j++)
	    {
	      temp = line.substr(0, line.find(";"));
	      line = line.erase(0, temp.size() + std::string(";").size());
	    }
	  line = line.substr(0, line.find(";"));
	  std::string year, month, day;
	  std::string datestr;
	  year = line;
	  year = year.substr(0, year.find("-"));
	  datestr = year;
	  month = line;
	  month = month.erase(0, month.find("-") + std::string("-").size());
	  month = month.substr(0, month.find("-"));
	  datestr = month + "." + datestr;
	  if(month.substr(0, 0) == "0")
	    {
	      month.erase(0);
	    }
	  day = line;
	  day = day.erase(0, day.find("-") + std::string("-").size());
	  day = day.erase(0, day.find("-") + std::string("-").size());
	  datestr = day + "." + datestr;
	  if(day.substr(0, 0) == "0")
	    {
	      day.erase(0);
	    }
	  double td = af.grigtojulian(std::stoi(day), std::stoi(month),
				      std::stoi(year), 0, 0, 0.0);
	  if(itspl != split.end())
	    {
	      double spl_date = af.grigtojulian(std::get<1>(*itspl),
						std::get<2>(*itspl),
						std::get<3>(*itspl), 0, 0, 0);
	      if(td > spl_date)
		{
		  apply_split = true;
		  split.erase(itspl);
		  if(split.size() > 0)
		    {
		      itspl = split.begin();
		    }
		  else
		    {
		      itspl = split.end();
		    }
		}
	    }
	  writevalue = af.togrigday(td) + "." + af.togrigmonth(td) + "."
	      + af.togrigyear(td) + ";";

	  line = source[i];
	  for(int j = 0; j < std::get<1>(tupforsave); j++)
	    {
	      temp = line.substr(0, line.find(";"));
	      line = line.erase(0, temp.size() + std::string(";").size());
	    }
	  line = line.substr(0, line.find(";"));
	  strm.str("");
	  strm.clear();
	  strm.imbue(loc);
	  strm << line;
	  strm >> money;
	  if(apply_split)
	    {
	      Money = 0;
	    }
	  Money = Money + money;

	  line = source[i];
	  for(int j = 0; j < std::get<2>(tupforsave); j++)
	    {
	      temp = line.substr(0, line.find(";"));
	      line = line.erase(0, temp.size() + std::string(";").size());
	    }
	  line = line.substr(0, line.find(";"));
	  strm.str("");
	  strm.clear();
	  strm.imbue(loc);
	  strm << line;
	  strm >> volume;
	  if(apply_split)
	    {
	      Volume = 0;
	    }
	  Volume = Volume + volume;

	  double waprice = money / volume;
	  strm.str("");
	  strm.clear();
	  strm.imbue(loc);
	  strm << std::fixed;
	  strm << std::setprecision(3);
	  strm << waprice;
	  strm >> line;
	  writevalue = writevalue + line + ";";

	  line = source[i];
	  for(int j = 0; j < std::get<3>(tupforsave); j++)
	    {
	      temp = line.substr(0, line.find(";"));
	      line = line.erase(0, temp.size() + std::string(";").size());
	    }
	  line = line.substr(0, line.find(";"));
	  writevalue = writevalue + line + ";";

	  line = source[i];
	  for(int j = 0; j < std::get<4>(tupforsave); j++)
	    {
	      temp = line.substr(0, line.find(";"));
	      line = line.erase(0, temp.size() + std::string(";").size());
	    }
	  line = line.substr(0, line.find(";"));
	  writevalue = writevalue + line + ";";

	  strm.str("");
	  strm.clear();
	  strm.imbue(loc);
	  strm << std::fixed;
	  strm << std::setprecision(0);
	  strm << volume;
	  writevalue = writevalue + strm.str() + ";";

	  strm.str("");
	  strm.clear();
	  strm.imbue(loc);
	  strm << std::fixed;
	  strm << std::setprecision(3);
	  strm << money;
	  writevalue = writevalue + strm.str() + ";";

	  PSOD = mpf_class(volume) / mpf_class(money);
	  strm.str("");
	  strm.clear();
	  strm.imbue(loc);
	  strm << std::fixed;
	  strm << std::setprecision(22);
	  strm << PSOD;
	  writevalue = writevalue + strm.str() + ";";

	  PSD = Volume / Money;
	  strm.str("");
	  strm.clear();
	  strm.imbue(loc);
	  strm << std::fixed;
	  strm << std::setprecision(22);
	  strm << PSD;
	  writevalue = writevalue + strm.str() + "\n";
	  auto it = std::find_if(finalv.begin(), finalv.end(), [datestr]
	  (std::string &element)
	    {
	      std::string fnd = element;
	      fnd = fnd.substr(0, fnd.find(";"));
	      return datestr == fnd;
	    });
	  if(it == finalv.end())
	    {
	      finalv.push_back(writevalue);
	    }
	  if(apply_split)
	    {
	      apply_split = false;
	    }
	}
    }
  f.open(finalpath, std::ios_base::out | std::ios_base::binary);
  for(size_t i = 0; i < finalv.size(); i++)
    {
      f.write(finalv[i].c_str(), finalv[i].size());
    }
  f.close();
}
