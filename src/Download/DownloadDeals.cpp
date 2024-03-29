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

#include "DownloadDeals.h"

DownloadDeals::DownloadDeals(
    int *canceling,
    std::vector<std::tuple<std::string, std::string, std::string>> *instruments,
    std::vector<std::tuple<std::string, std::string, std::string>> *instrumentsF,
    std::vector<std::tuple<std::string, std::string>> *boards, int selector)
{
  Canceling = canceling;
  Instruments = *instruments;
  InstrumentsF = *instrumentsF;
  Boards = *boards;
  Selector = selector;
}

DownloadDeals::~DownloadDeals()
{
  // TODO Auto-generated destructor stub
}

void
DownloadDeals::downloadAll()
{
  if(pulse)
    {
      pulse();
    }
  int turn = checkTurnovers();
  AuxFunc af;
  if(turn == 0)
    {
      if(noDeals)
	{
	  noDeals();
	}
      return void();
    }
  else if(turn < 0)
    {
      if(netError)
	{
	  netError();
	}
      return void();
    }
  else if(turn > 0)
    {
      if(pulse)
	{
	  pulse();
	}
      std::string line, tmp;
      std::string date;
      double juldate;
      time_t lt = time(nullptr);
      lt = lt + 3600 * 3;
      tm *loctm = gmtime(&lt);
      juldate = af.grigtojulian(loctm->tm_mday, loctm->tm_mon + 1,
				loctm->tm_year + 1900, loctm->tm_hour,
				loctm->tm_min,
				static_cast<double>(loctm->tm_sec));
      date = af.togrigday(juldate) + "." + af.togrigmonth(juldate) + "."
	  + af.togrigyear(juldate);
      std::string dirline = af.tempPath();
      dirline = dirline + "/Money/DownloadDeals/" + date;
      std::filesystem::path dir = std::filesystem::u8path(dirline);

      if(!std::filesystem::exists(dir))
	{
	  std::filesystem::create_directories(dir);
	}
      af.homePath(&dirline);
      dirline = dirline + "/Money/Deals";
      dir = std::filesystem::u8path(dirline);
      if(!std::filesystem::exists(dir))
	{
	  std::filesystem::create_directories(dir);
	}

      for(size_t i = 0; i < Boards.size(); i++)
	{
	  dirline = af.tempPath();
	  dirline = dirline + "/Money/DownloadDeals/" + date + "/"
	      + std::get<0>(Boards.at(i));
	  dir = std::filesystem::u8path(dirline);
	  std::filesystem::create_directories(dir);
	}

      if(Instruments.size() > 0)
	{
	  std::string adress, filename;
	  std::filesystem::path filepath;
	  int check = 0;
	  int count = 0;
	  int errorcount = 0;
	  int linecount = 0;
	  std::vector<std::vector<std::string>> boardvect;
	  for(size_t i = 0; i < Boards.size(); i++)
	    {
	      std::vector<std::string> vect;
	      boardvect.push_back(vect);
	    }
	  for(size_t i = 0; i < Instruments.size(); i++)
	    {
	      if(pulse)
		{
		  pulse();
		}
	      if(*Canceling > 0)
		{
		  if(canceled)
		    {
		      canceled();
		    }
		  return void();
		}
	      dirline = af.tempPath();
	      dirline = dirline + "/Money/Deals";
	      dir = std::filesystem::u8path(dirline);
	      if(std::filesystem::exists(dir))
		{
		  std::filesystem::remove_all(dir);
		}
	      std::filesystem::create_directories(dir);
	      count = 0;
	      std::vector<std::string> fileheader;
	      for(size_t j = 0; j < boardvect.size(); j++)
		{
		  boardvect[j].clear();
		  line.clear();
		  fileheader.push_back(line);
		}
	      for(;;)
		{
		  if(pulse)
		    {
		      pulse();
		    }
		  if(*Canceling > 0)
		    {
		      if(canceled)
			{
			  canceled();
			}
		      return void();
		    }
		  check = 0;
		  errorcount = 0;
		  adress =
		      "https://iss.moex.com/iss/engines/stock/markets/shares/securities/"
			  + std::get<0>(Instruments.at(i))
			  + "/trades.csv?start=" + std::to_string(count)
			  + "&limit=5000";
		  std::string insnm = std::get<0>(Instruments.at(i));
		  auto itinf = std::find_if(InstrumentsF.begin(),
					    InstrumentsF.end(), [insnm]
					    (auto &el)
					      {
						return std::get<0>(el) == insnm;
					      });
		  if(itinf != InstrumentsF.end())
		    {
		      adress =
			  "https://iss.moex.com/iss/engines/stock/markets/foreignshares/securities/"
			      + std::get<0>(Instruments.at(i))
			      + "/trades.csv?start=" + std::to_string(count)
			      + "&limit=5000";
		    }
		  filename = af.tempPath();
		  filename = filename + "/Money/Deals/"
		      + std::get<0>(Instruments.at(i)) + std::to_string(count);
		  filepath = std::filesystem::u8path(filename);

		  for(;;)
		    {
		      if(pulse)
			{
			  pulse();
			}
		      if(*Canceling > 0)
			{
			  if(canceled)
			    {
			      canceled();
			    }
			  return void();
			}
		      af.callNetwork(adress, filename, &check);
		      if(check > 0)
			{
			  break;
			}
		      if(check == 0)
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
		  std::fstream f;
		  f.open(filepath, std::ios_base::in);
		  linecount = 0;
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
		      std::filesystem::remove(filepath);
		      break;
		    }
		  count = count + 5000;
		}
	      count = 0;
	      linecount = 0;
	      for(;;)
		{
		  if(pulse)
		    {
		      pulse();
		    }
		  if(*Canceling > 0)
		    {
		      if(canceled)
			{
			  canceled();
			}
		      return void();
		    }
		  linecount = 0;
		  filename = af.tempPath();
		  filename = filename + "/Money/Deals/"
		      + std::get<0>(Instruments.at(i)) + std::to_string(count);
		  filepath = std::filesystem::u8path(filename);

		  if(!std::filesystem::exists(filepath))
		    {
		      break;
		    }
		  std::string filehline;
		  std::fstream f;
		  f.open(filepath, std::ios_base::in);
		  while(!f.eof())
		    {
		      getline(f, line);
		      af.cp1251toutf8(line);
		      if(linecount <= 2)
			{
			  filehline = filehline + line + "\n";
			}
		      if(linecount > 2)
			{
			  if(!line.empty())
			    {
			      tmp = line;
			      tmp = tmp.erase(
				  0, tmp.find(";") + std::string(";").size());
			      tmp = tmp.erase(
				  0, tmp.find(";") + std::string(";").size());
			      tmp = tmp.substr(0, tmp.find(";"));
			      line = line + "\n";
			      auto iter = std::find_if(
				  Boards.begin(), Boards.end(), [tmp]
				  (auto &element)
				    {
				      std::string chk = std::get<0>(element);
				      return chk == tmp;
				    });
			      size_t index = std::distance(Boards.begin(),
							   iter);
			      boardvect[index].push_back(line);
			      if(fileheader[index].size() == 0)
				{
				  fileheader[index] = filehline;
				}
			    }
			  else
			    {
			      break;
			    }
			}
		      linecount++;
		    }
		  f.close();
		  count = count + 5000;
		}

	      for(size_t k = 0; k < boardvect.size(); k++)
		{
		  if(boardvect[k].size() > 3)
		    {
		      filename = af.tempPath();
		      filename = filename + "/Money/DownloadDeals/" + date + "/"
			  + std::get<0>(Boards.at(k)) + "/"
			  + std::get<0>(Instruments.at(i));
		      filepath = std::filesystem::u8path(filename);

		      std::fstream f;
		      f.open(filepath,
			     std::ios_base::out | std::ios_base::binary);
		      line = fileheader[k];
		      f.write(line.c_str(), line.size());
		      for(size_t m = 0; m < boardvect[k].size(); m++)
			{
			  line = boardvect[k].at(m);
			  f.write(line.c_str(), line.size());
			}
		      f.close();
		    }
		}

	      if(progr)
		{
		  progr(
		      static_cast<double>(i + 1)
			  / static_cast<double>(Instruments.size()));
		}
	    }
	  dirline = af.tempPath();
	  dirline = dirline + "/Money/DownloadDeals/" + date;
	  dir = std::filesystem::u8path(dirline);
	  int checkemp = 0;
	  for(auto &dirit : std::filesystem::directory_iterator(dir))
	    {
	      std::filesystem::path p = dirit.path();
	      if(!std::filesystem::is_empty(p))
		{
		  checkemp = 1;
		  break;
		}
	    }
	  if(checkemp <= 0)
	    {
	      std::filesystem::remove_all(dir);
	      af.homePath(&filename);
	      filename = filename + "/Money/Deals/" + date + ".zip";
	      filepath = std::filesystem::u8path(filename);

	      std::filesystem::remove(filepath);
	      if(noDeals)
		{
		  noDeals();
		}
	    }
	  else
	    {
	      if(Selector == 0)
		{
		  af.homePath(&filename);
		  filename = filename + "/Money/Deals/" + date + ".zip";
		  filepath = std::filesystem::u8path(filename);

		  if(std::filesystem::exists(filepath))
		    {
		      std::filesystem::remove(filepath);
		      dirline = af.tempPath();
		      dirline = dirline + "/Money/DownloadDeals/" + date;
		      filepath = std::filesystem::u8path(dirline);
		      af.packing(filepath.u8string(), filename);

		      dirline = af.tempPath();
		      dirline = dirline + "/Money/DownloadDeals";
		      dir = std::filesystem::u8path(dirline);
		      std::filesystem::remove_all(dir);
		    }
		  else
		    {
		      dirline = af.tempPath();
		      dirline = dirline + "/Money/DownloadDeals/" + date;
		      af.packing(dirline, filename);

		      dirline = af.tempPath();
		      dirline = dirline + "/Money/DownloadDeals";
		      dir = std::filesystem::u8path(dirline);
		      std::filesystem::remove_all(dir);
		    }
		}
	      else
		{
		  if(pulse)
		    {
		      pulse();
		    }
		  af.homePath(&dirline);
		  dirline = dirline + "/Money/Deals/" + date + ".zip";
		  dir = std::filesystem::u8path(dirline);
		  if(std::filesystem::exists(dir))
		    {
		      std::string outpath;
		      outpath = af.tempPath();
		      outpath = outpath + "/Money/DealsSingl";
		      std::filesystem::path p = std::filesystem::u8path(
			  outpath);
		      if(std::filesystem::exists(p))
			{
			  std::filesystem::remove_all(p);
			}
		      std::filesystem::create_directories(p);
		      af.unpacking(dir.u8string(), p.u8string());
		      std::filesystem::remove(dir);
		    }
		  if(pulse)
		    {
		      pulse();
		    }
		  dirline = af.tempPath();
		  dirline = dirline + "/Money/DealsSingl/" + date;
		  for(size_t i = 0; i < Boards.size(); i++)
		    {
		      line = dirline + "/" + std::get<0>(Boards.at(i));
		      dir = std::filesystem::u8path(line);
		      std::filesystem::create_directories(dir);
		    }
		  dirline = af.tempPath();
		  dirline = dirline + "/Money/DownloadDeals/" + date;
		  dir = std::filesystem::u8path(dirline);
		  std::vector<std::string> dirnms;
		  for(auto &iter : std::filesystem::directory_iterator(dir))
		    {
		      filepath = iter.path();
		      if(std::filesystem::is_directory(filepath))
			{
			  if(std::filesystem::is_empty(filepath))
			    {
			      std::filesystem::remove_all(filepath);
			    }
			  else
			    {
			      line = filepath.filename().u8string();
			      dirnms.push_back(line);
			    }
			}
		    }
		  if(pulse)
		    {
		      pulse();
		    }
		  for(size_t o = 0; o < dirnms.size(); o++)
		    {
		      filename = af.tempPath();
		      filename = filename + "/Money/DealsSingl/" + date + "/"
			  + dirnms[o] + "/" + std::get<0>(Instruments.at(0));
		      filepath = std::filesystem::u8path(filename);
		      if(std::filesystem::exists(filepath))
			{
			  std::filesystem::remove(filepath);
			}
		      filename = af.tempPath();
		      filename = filename + "/Money/DownloadDeals/" + date + "/"
			  + dirnms[o] + "/" + std::get<0>(Instruments.at(0));
		      filepath = std::filesystem::u8path(filename);
		      dirline = af.tempPath();
		      dirline = dirline + "/Money/DealsSingl/" + date + "/"
			  + dirnms[o] + "/" + std::get<0>(Instruments.at(0));
		      dir = std::filesystem::u8path(dirline);
		      std::filesystem::copy(filepath, dir);
		    }
		  dirline = af.tempPath();
		  dirline = dirline + "/Money/DealsSingl/" + date;
		  dir = std::filesystem::u8path(dirline);
		  if(pulse)
		    {
		      pulse();
		    }
		  for(auto &iter : std::filesystem::directory_iterator(dir))
		    {
		      filepath = iter.path();
		      if(std::filesystem::is_empty(filepath))
			{
			  std::filesystem::remove_all(filepath);
			}
		    }
		  af.homePath(&filename);
		  filename = filename + "/Money/Deals/" + date + ".zip";
		  af.packing(dir.u8string(), filename);
		  dirline = af.tempPath();
		  dirline = dirline + "/Money/Deals";
		  dir = std::filesystem::u8path(dirline);
		  std::filesystem::remove_all(dir);
		  dirline = af.tempPath();
		  dirline = dirline + "/Money/DownloadDeals";
		  dir = std::filesystem::u8path(dirline);
		  std::filesystem::remove_all(dir);
		  dirline = af.tempPath();
		  dirline = dirline + "/Money/DealsSingl";
		  dir = std::filesystem::u8path(dirline);
		  std::filesystem::remove_all(dir);
		}
	    }

	  dirline = af.tempPath();
	  dirline = dirline + "/Money/Deals";
	  dir = std::filesystem::u8path(dirline);
	  std::filesystem::remove_all(dir);
	  if(finished)
	    {
	      finished();
	    }
	}
    }
}

int
DownloadDeals::checkTurnovers()
{
  int check = 0;
  int count = 0;
  AuxFunc af;

  for(;;)
    {

      std::string pathline;
      pathline = af.tempPath();
      pathline = pathline + "/Money/turnovers.csv";
      af.callNetwork(
	  "https://iss.moex.com/iss/engines/stock/markets/shares/turnovers.csv",
	  pathline, &check);
      if(check > 0)
	{
	  break;
	}
      else
	{
	  count++;
	}
      if(count >= 100)
	{
	  return -1;
	}
    }

  count = 0;
  std::string line, temp, temp2;
  std::vector<int> turn;
  std::fstream f;
  std::string pathline = af.tempPath();
  pathline = pathline + "/Money/turnovers.csv";
  std::filesystem::path path = std::filesystem::u8path(pathline);
  if(!std::filesystem::exists(path))
    {
      std::cout << "Turnovers not opened" << std::endl;
    }
  else
    {
      f.open(path, std::ios_base::in);
      int findstr = 0;
      while(!f.eof())
	{
	  getline(f, line);
	  af.cp1251toutf8(line);
	  if(count == 2)
	    {
	      std::string tmp = line;
	      af.cp1251toutf8(tmp);
	      int countch = 0;
	      while(tmp.size() > 0)
		{
		  temp = tmp.substr(0, tmp.find(";"));
		  std::string::size_type n;
		  if(temp == "UPDATETIME")
		    {
		      findstr = countch;
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
	  if(count > 2 && line.empty())
	    {
	      break;
	    }
	  if(count > 2)
	    {
	      std::string tmp = line;
	      for(int i = 0; i < findstr; i++)
		{
		  temp = tmp.substr(0, tmp.find(";"));
		  tmp = tmp.erase(0, temp.size() + std::string(";").size());
		}
	      tmp = tmp.substr(0, tmp.find(";"));
	      temp = tmp;

	      temp = temp.substr(0, temp.find(" "));
	      int day, month, year;
	      temp2 = temp;
	      temp2 = temp2.substr(0, temp2.find("-"));
	      year = std::stoi(temp2);
	      temp2 = temp;
	      temp2 = temp2.erase(0, temp2.find("-") + std::string("-").size());
	      temp2 = temp2.substr(0, temp2.find("-"));
	      month = std::stoi(temp2);
	      temp2 = temp;
	      temp2 = temp2.erase(0, temp2.find("-") + std::string("-").size());
	      temp2 = temp2.erase(0, temp2.find("-") + std::string("-").size());
	      day = std::stoi(temp2);
	      turn.push_back(
		  static_cast<int>(af.grigtojulian(day, month, year, 0, 0, 0.0)));
	    }
	  count++;
	}
      f.close();
    }

  check = 0;
  time_t moscow = time(nullptr);
  moscow = moscow + 3600 * 3;
  tm *msk = gmtime(&moscow);
  double msktime = af.grigtojulian(msk->tm_mday, msk->tm_mon + 1,
				   msk->tm_year + 1900, msk->tm_hour,
				   msk->tm_min,
				   static_cast<double>(msk->tm_sec));

  for(size_t i = 0; i < turn.size(); i++)
    {
      if(static_cast<int>(std::floor(msktime)) > turn[i])
	{
	  count = 0;
	}
      else
	{
	  count = 1;
	}
      check = check + count;
    }
  std::filesystem::remove(path);

  return check;
}

