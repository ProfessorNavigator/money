/*
 * Copyright (C) 2026 Yury Bobylev <bobilev_yury@mail.ru>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <FileReader.h>
#include <MoneyAux.h>
#include <ctime>
#include <fstream>

FileReader::FileReader()
{
}

std::vector<TradeDay>
FileReader::readAll(const std::filesystem::path &fp)
{
  std::vector<TradeDay> result;

  std::fstream f;
  f.open(fp, std::ios_base::in | std::ios_base::binary);
  if(!f.is_open())
    {
      return result;
    }
  std::string buf;
  f.seekg(0, std::ios_base::end);
  buf.resize(static_cast<size_t>(f.tellg()));
  f.seekg(0, std::ios_base::beg);
  f.read(buf.data(), buf.size());
  f.close();

  std::string find_str("history\n\n");
  std::string::size_type n = buf.find(find_str);
  if(n != std::string::npos)
    {
      n += find_str.size();
    }
  else
    {
      n = 0;
    }

  std::vector<std::string> columns;

  find_str = "\n";
  std::string::size_type n2 = buf.find(find_str, n);
  if(n2 == std::string::npos)
    {
      return result;
    }
  std::string line(buf.begin() + n, buf.begin() + n2);
  n = n2 + find_str.size();

  find_str = ";";

  while(line.size() > 0)
    {
      std::string::size_type n_sc = line.find(find_str);
      if(n_sc == std::string::npos)
        {
          if(line.size() > 0)
            {
              columns.emplace_back(line);
            }
          line.clear();
        }
      else
        {
          columns.push_back(std::string(line.begin(), line.begin() + n_sc));
          line.erase(line.begin(), line.begin() + n_sc + find_str.size());
        }
    }

  find_str = "\n";
  std::string sc(";");
  size_t count;
  while(n < buf.size())
    {
      n2 = buf.find(find_str, n);
      if(n == std::string::npos)
        {
          break;
        }
      line = std::string(buf.begin() + n, buf.begin() + n2);

      n = n2 + find_str.size();
      count = 0;
      TradeDay td;
      while(count < columns.size() && line.size() > 0)
        {
          std::string::size_type n_sc = line.find(sc);
          std::string column;
          if(n_sc == std::string::npos)
            {
              column = std::string(line.begin(), line.end());
              line.clear();
            }
          else
            {
              column = std::string(line.begin(), line.begin() + n_sc);
              line.erase(line.begin(), line.begin() + n_sc + sc.size());
            }
          if(columns[count] == "BOARDID")
            {
              td.board_id = column;
            }
          else if(columns[count] == "TRADEDATE")
            {
              std::tm tm = {};
              std::stringstream strm;
              strm.str(column);
              strm >> std::get_time(&tm, "%Y-%m-%d");
              td.date = MoneyAux::dateToJulianDay(
                  tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, 0, 0, 0.0);
            }
          else if(columns[count] == "SECID")
            {
              td.instrument_id = column;
            }
          else if(columns[count] == "VALUE")
            {
              std::stringstream strm;
              strm.imbue(std::locale("C"));
              strm.str(column);
              strm >> td.money_turnover;
            }
          else if(columns[count] == "OPEN")
            {
              std::stringstream strm;
              strm.imbue(std::locale("C"));
              strm.str(column);
              strm >> td.first_price;
            }
          else if(columns[count] == "WAPRICE")
            {
              std::stringstream strm;
              strm.imbue(std::locale("C"));
              strm.str(column);
              strm >> td.wa_price;
            }
          else if(columns[count] == "CLOSE")
            {
              std::stringstream strm;
              strm.imbue(std::locale("C"));
              strm.str(column);
              strm >> td.last_price;
            }
          else if(columns[count] == "VOLUME")
            {
              std::stringstream strm;
              strm.imbue(std::locale("C"));
              strm.str(column);
              strm >> td.turnover;
            }
          else if(columns[count] == "CURRENCYID")
            {
              td.currency_id = column;
            }
          count++;
        }
      if(!td.instrument_id.empty())
        {
          result.emplace_back(td);
        }
    }

  return result;
}

std::vector<Security>
FileReader::readSecurities(const std::filesystem::path &fp)
{
  std::vector<Security> result;

  std::fstream f;
  f.open(fp, std::ios_base::in | std::ios_base::binary);
  if(!f.is_open())
    {
      return result;
    }
  std::string str;
  f.seekg(0, std::ios_base::end);
  str.resize(static_cast<size_t>(f.tellg()));
  f.seekg(0, std::ios_base::beg);
  f.read(str.data(), str.size());
  f.close();

  std::string line_end = "securities\n\n";
  std::string::size_type n_end = str.find(line_end);
  if(n_end != std::string::npos)
    {
      str.erase(0, n_end + line_end.size());
    }
  line_end = "\n";
  n_end = str.find(line_end);
  if(n_end == std::string::npos)
    {
      return result;
    }
  std::string line(str.begin(), str.begin() + n_end);
  str.erase(str.begin(), str.begin() + line.size() + line_end.size());
  std::vector<std::string> index_v;
  std::string item_end(";");

  while(line.size() > 0)
    {
      n_end = line.find(item_end);
      if(n_end == std::string::npos)
        {
          if(!line.empty())
            {
              index_v.push_back(line);
            }
          break;
        }
      else
        {
          std::string id(line.begin(), line.begin() + n_end);
          line.erase(line.begin(), line.begin() + n_end + item_end.size());
          index_v.push_back(id);
        }
    }

  n_end = 0;
  std::string::size_type n_beg;
  while(n_end < str.size())
    {
      n_beg = n_end;
      n_end = str.find(line_end, n_end);
      if(n_end == std::string::npos)
        {
          line = std::string(str.begin() + n_beg, str.end());
        }
      else
        {
          line = std::string(str.begin() + n_beg, str.begin() + n_end);
          n_end += line_end.size();
        }
      Security sec;
      size_t index = 0;
      while(line.size() > 0)
        {
          std::string::size_type n = line.find(item_end);
          std::string item;
          if(n == std::string::npos)
            {
              item = std::string(line.begin(), line.end());
              line.clear();
            }
          else
            {
              item = std::string(line.begin(), line.begin() + n);
              line.erase(line.begin(), line.begin() + n + item_end.size());
            }
          if(item.empty())
            {
              break;
            }
          if(index >= index_v.size())
            {
              break;
            }
          if(index_v[index] == "SECID")
            {
              sec.id = item;
            }
          else if(index_v[index] == "SHORTNAME")
            {
              sec.short_name = item;
            }
          else if(index_v[index] == "NAME")
            {
              sec.name = item;
            }
          else if(index_v[index] == "BOARDID")
            {
              sec.board_id = item;
            }
          else if(index_v[index] == "history_from")
            {
              sec.history_from = item;
            }
          else if(index_v[index] == "history_till")
            {
              sec.history_till = item;
            }
          index++;
        }
      if(!sec.id.empty())
        {
          result.emplace_back(sec);
        }
    }

  return result;
}

std::vector<PPM>
FileReader::readPPM(const std::filesystem::path &fp)
{
  std::vector<PPM> result;

  std::fstream f;
  f.open(fp, std::ios_base::in | std::ios_base::binary);
  if(!f.is_open())
    {
      return result;
    }
  std::string buf;
  f.seekg(0, std::ios_base::end);
  buf.resize(static_cast<size_t>(f.tellg()));
  f.seekg(0, std::ios_base::beg);
  f.read(buf.data(), buf.size());
  f.close();

  std::string find_str("\n");
  std::string::size_type n = buf.find(find_str);
  if(n == std::string::npos)
    {
      return result;
    }
  std::string line(buf.begin(), buf.begin() + n);
  n += find_str.size();

  find_str = ";";
  std::vector<std::string> columns;
  while(line.size() > 0)
    {
      std::string::size_type n_sc = line.find(find_str);
      if(n_sc == std::string::npos)
        {
          if(!line.empty())
            {
              columns.push_back(line);
            }
          line.clear();
        }
      else
        {
          columns.push_back(std::string(line.begin(), line.begin() + n_sc));
          line.erase(line.begin(), line.begin() + n_sc + find_str.size());
        }
    }

  find_str = "\n";
  std::string sc(";");
  while(n < buf.size())
    {
      line.clear();
      std::string::size_type n2 = buf.find(find_str, n);
      if(n2 == std::string::npos)
        {
          line = std::string(buf.begin() + n, buf.end());
          n = n2;
        }
      else
        {
          line = std::string(buf.begin() + n, buf.begin() + n2);
          n = n2 + find_str.size();
        }
      if(line.empty())
        {
          continue;
        }

      size_t count = 0;
      PPM ppm;
      while(line.size() > 0 && count < columns.size())
        {
          std::string::size_type n_sc = line.find(sc);
          std::string column;
          if(n_sc == std::string::npos)
            {
              column = line;
              line.clear();
            }
          else
            {
              column = std::string(line.begin(), line.begin() + n_sc);
              line.erase(line.begin(), line.begin() + n_sc + sc.size());
            }

          if(columns[count] == "DATE")
            {
              std::tm tm = {};
              std::stringstream strm;
              strm.str(column);
              strm >> std::get_time(&tm, "%Y-%m-%d");
              ppm.date = MoneyAux::dateToJulianDay(
                  tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, 0, 0, 0.0);
            }
          else if(columns[count] == "TURNOVER")
            {
              std::stringstream strm;
              strm.imbue(std::locale("C"));
              strm.str(column);
              strm >> ppm.turnover;
            }
          else if(columns[count] == "MONEY_TURNOVER")
            {
              std::stringstream strm;
              strm.imbue(std::locale("C"));
              strm.str(column);
              strm >> ppm.money_turnover;
            }
          else if(columns[count] == "DAILY_PPM")
            {
              std::stringstream strm;
              strm.imbue(std::locale("C"));
              strm.str(column);
              strm >> ppm.current_ppm;
            }
          else if(columns[count] == "TOTAL_PPM")
            {
              std::stringstream strm;
              strm.imbue(std::locale("C"));
              strm.str(column);
              strm >> ppm.total_ppm;
            }
          count++;
        }
      result.emplace_back(ppm);
    }

  return result;
}

std::vector<Board>
FileReader::readBoards(const std::filesystem::path &fp)
{
  std::vector<Board> result;

  std::fstream f;
  f.open(fp, std::ios_base::in | std::ios_base::binary);
  if(!f.is_open())
    {
      return result;
    }

  std::string buf;
  f.seekg(0, std::ios_base::end);
  buf.resize(static_cast<size_t>(f.tellg()));
  f.seekg(0, std::ios_base::beg);
  f.read(buf.data(), buf.size());
  f.close();

  std::string find_str("boards\n\n");
  std::string::size_type n = buf.find(find_str);
  if(n == std::string::npos)
    {
      return result;
    }
  n += find_str.size();

  find_str = "\n";
  std::string::size_type n2 = buf.find(find_str, n);
  if(n2 == std::string::npos)
    {
      return result;
    }
  std::string line(buf.begin() + n, buf.begin() + n2);
  n = n2 + find_str.size();

  std::vector<std::string> columns;
  find_str = ";";
  while(line.size() > 0)
    {
      std::string::size_type n_sc = line.find(find_str);
      std::string column;
      if(n_sc == std::string::npos)
        {
          column = std::string(line.begin(), line.end());
          line.clear();
        }
      else
        {
          column = std::string(line.begin(), line.begin() + n_sc);
          line.erase(line.begin(), line.begin() + n_sc + find_str.size());
        }
      if(column.size() > 0)
        {
          columns.emplace_back(column);
        }
    }

  find_str = "\n";
  std::string sc(";");
  while(n < buf.size())
    {
      n2 = buf.find(find_str, n);
      if(n2 == std::string::npos)
        {
          line = std::string(buf.begin() + n, buf.end());
          n = n2;
        }
      else
        {
          line = std::string(buf.begin() + n, buf.begin() + n2);
          n = n2 + find_str.size();
        }

      Board brd;
      size_t count = 0;
      while(line.size() > 0 && count < columns.size())
        {
          std::string::size_type n_sc = line.find(sc);
          std::string column;
          if(n_sc == std::string::npos)
            {
              column = std::string(line.begin(), line.end());
              line.clear();
            }
          else
            {
              column = std::string(line.begin(), line.begin() + n_sc);
              line.erase(line.begin(), line.begin() + n_sc + sc.size());
            }
          if(columns[count] == "boardid")
            {
              brd.id = column;
            }
          else if(columns[count] == "title")
            {
              brd.name = column;
            }
          count++;
        }
      if(brd.id.size() > 0)
        {
          result.emplace_back(brd);
        }
    }

  return result;
}

std::vector<Deal>
FileReader::readDeals(const std::filesystem::path &fp)
{
  std::vector<Deal> result;

  std::fstream f;
  f.open(fp, std::ios_base::in | std::ios_base::binary);
  if(!f.is_open())
    {
      return result;
    }
  std::string buf;
  f.seekg(0, std::ios_base::end);
  buf.resize(static_cast<size_t>(f.tellg()));
  f.seekg(0, std::ios_base::beg);
  f.read(buf.data(), buf.size());
  f.close();

  std::string find_str("trades\n\n");
  std::string::size_type n = buf.find(find_str);
  if(n == std::string::npos)
    {
      return result;
    }
  n += find_str.size();

  find_str = "\n";
  std::string::size_type n2 = buf.find(find_str, n);
  if(n2 == std::string::npos)
    {
      return result;
    }
  std::string line(buf.begin() + n, buf.begin() + n2);
  n = n2 + find_str.size();

  std::vector<std::string> columns;
  find_str = ";";
  while(line.size() > 0)
    {
      std::string::size_type n_sc = line.find(find_str);
      std::string column;
      if(n_sc == std::string::npos)
        {
          column = line;
          line.clear();
        }
      else
        {
          column = std::string(line.begin(), line.begin() + n_sc);
          line.erase(line.begin(), line.begin() + n_sc + find_str.size());
        }
      if(!column.empty())
        {
          columns.emplace_back(column);
        }
    }

  find_str = "\n";
  std::string sc(";");
  while(n < buf.size())
    {
      n2 = buf.find(find_str, n);
      if(n2 == std::string::npos)
        {
          line = std::string(buf.begin() + n, buf.end());
          n = n2;
        }
      else
        {
          line = std::string(buf.begin() + n, buf.begin() + n2);
          n = n2 + find_str.size();
        }

      size_t count = 0;
      Deal deal;
      std::tm tm = {};
      while(line.size() > 0 && count < columns.size())
        {
          std::string::size_type n_sc = line.find(sc);
          std::string column;
          if(n_sc == std::string::npos)
            {
              column = line;
              line.clear();
            }
          else
            {
              column = std::string(line.begin(), line.begin() + n_sc);
              line.erase(line.begin(), line.begin() + n_sc + sc.size());
            }

          if(columns[count] == "TRADENO")
            {
              std::stringstream strm;
              strm.imbue(std::locale("C"));
              strm.str(column);
              strm >> deal.trade_num;
            }
          else if(columns[count] == "TRADETIME")
            {
              std::stringstream strm;
              strm.str(column);
              strm >> std::get_time(&tm, "%H:%M:%S");
            }
          else if(columns[count] == "SECID")
            {
              deal.instrument_id = column;
            }
          else if(columns[count] == "PRICE")
            {
              std::stringstream strm;
              strm.imbue(std::locale("C"));
              strm.str(column);
              strm >> deal.price;
            }
          else if(columns[count] == "QUANTITY")
            {
              std::stringstream strm;
              strm.imbue(std::locale("C"));
              strm.str(column);
              strm >> deal.quantity;
            }
          else if(columns[count] == "BOARDID")
            {
              deal.board_id = column;
            }
          else if(columns[count] == "TRADEDATE")
            {
              std::stringstream strm;
              strm.str(column);
              strm >> std::get_time(&tm, "%Y-%m-%d");
            }

          count++;
        }

      if(deal.quantity > 0)
        {
          deal.deal_time = MoneyAux::dateToJulianDay(
              tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour,
              tm.tm_min, static_cast<double>(tm.tm_sec));
          deal.money_volume = static_cast<double>(deal.quantity) * deal.price;
          result.emplace_back(deal);
        }
    }

  return result;
}
