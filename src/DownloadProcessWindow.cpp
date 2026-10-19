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

#include <Algorithm.h>
#include <DownloadProcessWindow.h>
#include <FileReader.h>
#include <MoneyAux.h>
#include <PPM.h>
#include <QDir>
#include <QLabel>
#include <QNetworkRequest>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <thread>

DownloadProcessWindow::DownloadProcessWindow(
    const std::vector<Security> &securities,
    const std::vector<Security> &foreign_securities, const Mode &mode,
    QWidget *parent)
    : QWidget(parent)
{
  this->securities = securities;
  this->foreign_securities = foreign_securities;
  this->mode = mode;

  allow_close.store(false, std::memory_order_relaxed);
  cancel_all.store(false, std::memory_order_relaxed);

  this->setWindowFlag(Qt::Window);
  this->setAttribute(Qt::WA_DeleteOnClose);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle(tr("Progress"));

  network = new QNetworkAccessManager(this);

  mpf_set_default_prec(2048);

  createWindow();
}

void
DownloadProcessWindow::startDownload()
{
  std::thread thr(&DownloadProcessWindow::formDownloadVectors, this);
  thr.detach();
}

void
DownloadProcessWindow::closeEvent(QCloseEvent *event)
{
  if(allow_close.load(std::memory_order_relaxed))
    {
      event->accept();
    }
  else
    {
      event->ignore();
    }
}

void
DownloadProcessWindow::createWindow()
{
  QVBoxLayout *v_box = new QVBoxLayout;
  this->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setText(tr("Progress") + ":");
  v_box->addWidget(lab, 0, Qt::AlignCenter);

  QProgressBar *progress = new QProgressBar;
  progress->setMinimum(0);
  progress->setMaximum(100);
  progress->setValue(0);
  connect(this, &DownloadProcessWindow::signalProgress, progress,
          &QProgressBar::setValue);
  v_box->addWidget(progress);

  QPushButton *cancel = new QPushButton;
  cancel->setText(tr("Cancel"));
  connect(cancel, &QPushButton::clicked, this,
          [this]
            {
              cancel_all.store(true, std::memory_order_relaxed);
#pragma omp atomic write
              cancel_all_omp = true;
            });
  v_box->addWidget(cancel, 0, Qt::AlignCenter);

  connect(this, &DownloadProcessWindow::signalFinished, this,
          [this]
            {
              finalDialog();
              allow_close.store(true, std::memory_order_relaxed);
              this->close();
            });

  connect(this, &DownloadProcessWindow::signalDownload, this,
          &DownloadProcessWindow::downloadUrl);

  connect(this, &DownloadProcessWindow::signalNewOperation, progress,
          [progress, lab](const QString &operation_name, const int &min,
                          const int &max)
            {
              lab->setText(operation_name);
              progress->reset();
              progress->setMinimum(min);
              progress->setMaximum(max);
            });
}

void
DownloadProcessWindow::formDownloadVectors()
{
  Algorithm::parallelSort(securities.begin(), securities.end(),
                          [](const Security &el1, const Security &el2)
                            {
                              return el1.id < el2.id;
                            });
  if(mode == Mode::Deals)
    {      
      for(auto it = securities.begin(); it != securities.end(); it++)
        {
          if(to_download.empty())
            {
              to_download.push_back(*it);
            }
          else
            {
              if(it->id != to_download.rbegin()->id)
                {
                  to_download.push_back(*it);
                }
            }
        }
      for(auto it = foreign_securities.begin(); it != foreign_securities.end();
          it++)
        {
          if(to_download_foreign.empty())
            {
              to_download_foreign.push_back(*it);
            }
          else
            {
              if(it->id != to_download_foreign.rbegin()->id)
                {
                  to_download_foreign.push_back(*it);
                }
            }
        }
      downloadDeals();
    }
  else
    {
      for(auto it = securities.begin(); it != securities.end(); it++)
        {
          if(to_download.empty())
            {
              to_download.push_back(*it);
              auto it_last = to_download.rbegin();
              if(it_last->history_from.empty())
                {
                  it_last->history_from = "1997-03-23";
                }
              if(it_last->history_till.empty())
                {
                  std::time_t time = std::chrono::system_clock::to_time_t(
                      std::chrono::system_clock::now());
                  std::tm *tm = std::localtime(&time);
                  std::ostringstream strm;
                  strm << std::put_time(tm, "%Y-%m-%d");
                  it_last->history_till = strm.str();
                }
            }
          else
            {
              auto it_last = to_download.rbegin();
              if(it_last->id == it->id)
                {
                  if(!it->history_from.empty())
                    {
                      std::tm tm = {};
                      std::stringstream strm;
                      strm.str(it_last->history_from);
                      strm >> std::get_time(&tm, "%Y-%m-%d");
                      std::chrono::time_point<std::chrono::system_clock>
                          last_tm = std::chrono::system_clock::from_time_t(
                              std::mktime(&tm));

                      tm = std::tm();
                      strm.clear();
                      strm.str(it->history_from);
                      strm >> std::get_time(&tm, "%Y-%m-%d");
                      std::chrono::time_point<std::chrono::system_clock> cur_tm
                          = std::chrono::system_clock::from_time_t(
                              std::mktime(&tm));
                      if(cur_tm < last_tm)
                        {
                          it_last->history_from = it->history_from;
                        }
                    }

                  if(!it->history_till.empty())
                    {
                      std::tm tm = {};
                      std::stringstream strm;
                      strm.str(it_last->history_till);
                      strm >> std::get_time(&tm, "%Y-%m-%d");
                      std::chrono::time_point<std::chrono::system_clock>
                          last_tm = std::chrono::system_clock::from_time_t(
                              std::mktime(&tm));

                      tm = std::tm();
                      strm.clear();
                      strm.str(it->history_till);
                      strm >> std::get_time(&tm, "%Y-%m-%d");
                      std::chrono::time_point<std::chrono::system_clock> cur_tm
                          = std::chrono::system_clock::from_time_t(
                              std::mktime(&tm));
                      if(cur_tm > last_tm)
                        {
                          it_last->history_till = it->history_till;
                        }
                    }
                }
              else
                {
                  to_download.push_back(*it);
                  auto it_last = to_download.rbegin();
                  if(it_last->history_from.empty())
                    {
                      it_last->history_from = "1997-03-23";
                    }
                  if(it_last->history_till.empty())
                    {
                      std::time_t time = std::chrono::system_clock::to_time_t(
                          std::chrono::system_clock::now());
                      std::tm *tm = std::localtime(&time);
                      std::ostringstream strm;
                      strm << std::put_time(tm, "%Y-%m-%d");
                      it_last->history_till = strm.str();
                    }
                }
            }
        }

      for(auto it = foreign_securities.begin(); it != foreign_securities.end();
          it++)
        {
          if(to_download_foreign.empty())
            {
              to_download_foreign.push_back(*it);
              auto it_last = to_download_foreign.rbegin();
              if(it_last->history_from.empty())
                {
                  it_last->history_from = "1997-03-23";
                }
              if(it_last->history_till.empty())
                {
                  std::time_t time = std::chrono::system_clock::to_time_t(
                      std::chrono::system_clock::now());
                  std::tm *tm = std::localtime(&time);
                  std::ostringstream strm;
                  strm << std::put_time(tm, "%Y-%m-%d");
                  it_last->history_till = strm.str();
                }
            }
          else
            {
              auto it_last = to_download_foreign.rbegin();
              if(it_last->id == it->id)
                {
                  if(!it->history_from.empty())
                    {
                      std::tm tm = {};
                      std::stringstream strm;
                      strm.str(it_last->history_from);
                      strm >> std::get_time(&tm, "%Y-%m-%d");
                      std::chrono::time_point<std::chrono::system_clock>
                          last_tm = std::chrono::system_clock::from_time_t(
                              std::mktime(&tm));

                      tm = std::tm();
                      strm.clear();
                      strm.str(it->history_from);
                      strm >> std::get_time(&tm, "%Y-%m-%d");
                      std::chrono::time_point<std::chrono::system_clock> cur_tm
                          = std::chrono::system_clock::from_time_t(
                              std::mktime(&tm));
                      if(cur_tm < last_tm)
                        {
                          it_last->history_from = it->history_from;
                        }
                    }

                  if(!it->history_till.empty())
                    {
                      std::tm tm = {};
                      std::stringstream strm;
                      strm.str(it_last->history_till);
                      strm >> std::get_time(&tm, "%Y-%m-%d");
                      std::chrono::time_point<std::chrono::system_clock>
                          last_tm = std::chrono::system_clock::from_time_t(
                              std::mktime(&tm));

                      tm = std::tm();
                      strm.clear();
                      strm.str(it->history_till);
                      strm >> std::get_time(&tm, "%Y-%m-%d");
                      std::chrono::time_point<std::chrono::system_clock> cur_tm
                          = std::chrono::system_clock::from_time_t(
                              std::mktime(&tm));
                      if(cur_tm > last_tm)
                        {
                          it_last->history_till = it->history_till;
                        }
                    }
                }
              else
                {
                  to_download_foreign.push_back(*it);
                  auto it_last = to_download_foreign.rbegin();
                  if(it_last->history_from.empty())
                    {
                      it_last->history_from = "1997-03-23";
                    }
                  if(it_last->history_till.empty())
                    {
                      std::time_t time = std::chrono::system_clock::to_time_t(
                          std::chrono::system_clock::now());
                      std::tm *tm = std::localtime(&time);
                      std::ostringstream strm;
                      strm << std::put_time(tm, "%Y-%m-%d");
                      it_last->history_till = strm.str();
                    }
                }
            }
        }

      downloadAll();
      if(!cancel_all.load(std::memory_order_relaxed))
        {
          calculatePPM();
        }
    }

  emit signalFinished();
}

void
DownloadProcessWindow::downloadAll()
{
  std::string str = QDir::homePath().toStdString();
  std::filesystem::path save_dir(std::u8string(str.begin(), str.end()));
  save_dir /= u8".local";
  save_dir /= u8"share";
  save_dir /= u8"Money";
  save_dir /= u8"Data";
  save_dir /= u8"All";
  std::filesystem::create_directories(save_dir);

  double progr_count = 0;
  double total
      = static_cast<double>(to_download.size() + to_download_foreign.size());
  for(int i = 1; i <= 2; i++)
    {
      std::vector<Security>::iterator begin;
      std::vector<Security>::iterator end;
      switch(i)
        {
        case 1:
          {
            begin = to_download.begin();
            end = to_download.end();
            break;
          }
        case 2:
          {
            begin = to_download_foreign.begin();
            end = to_download_foreign.end();
            break;
          }
        default:
          return void();
        }
      for(auto it = begin; it != end; it++)
        {
          if(cancel_all.load(std::memory_order_relaxed))
            {
              break;
            }

          std::u8string u8str(it->id.begin(), it->id.end());
          u8str += u8".csv";
          std::filesystem::path save_p = save_dir;
          save_p /= std::filesystem::path(u8str);
          double last_date = getLastDate(save_p, it->id);

          std::tm tm = {};
          std::stringstream strm;
          strm.str(it->history_till);
          strm >> std::get_time(&tm, "%Y-%m-%d");
          double cur_tm = MoneyAux::dateToJulianDay(
              tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, 0, 0, 0.0);

          double start = last_date;
          if(start > getBaseDate())
            {
              start += 1;
            }
          else
            {
              tm = {};
              std::stringstream strm;
              strm.clear();
              strm.str(it->history_from);
              strm >> std::get_time(&tm, "%Y-%m-%d");
              start = MoneyAux::dateToJulianDay(tm.tm_mday, tm.tm_mon + 1,
                                                tm.tm_year + 1900, 0, 0, 0.0);
            }
          if(static_cast<uint64_t>(start) >= static_cast<uint64_t>(cur_tm))
            {
              progr_count += 1.0;
              int progr = static_cast<int>(progr_count * 100.0 / total);
              emit signalProgress(progr);
              continue;
            }

          int day, month, year, hour, minut;
          double sec;

          MoneyAux::julianDateToDate(start, day, month, year, hour, minut,
                                     sec);

          QString base_url;

          switch(i)
            {
            case 1:
              {
                base_url
                    = "https://iss.moex.com/iss/history/engines/stock/markets/"
                      "shares/securities/";
                break;
              }
            case 2:
              {
                base_url = "https://iss.moex.com/iss/history/engines/stock/"
                           "markets/foreignshares/securities/";
                break;
              }
            }

          base_url += it->id.c_str();
          base_url += ".csv?from=";
          strm.clear();
          strm.str("");
          strm.imbue(std::locale("C"));
          strm << year;
          base_url += strm.str().c_str();

          strm.clear();
          strm.str("");
          strm << month;
          str = strm.str();
          if(str.size() == 1)
            {
              str = "0" + str;
            }
          base_url += "-";
          base_url += str.c_str();

          strm.clear();
          strm.str("");
          strm << day;
          str = strm.str();
          if(str.size() == 1)
            {
              str = "0" + str;
            }
          base_url += "-";
          base_url += str.c_str();
          base_url += "&till=";
          base_url += it->history_till.c_str();

          int count = 0;
          next_url_mtx.lock();
          stop_urls = false;
          next_url_mtx.unlock();
          for(;;)
            {
              if(cancel_all.load(std::memory_order_relaxed))
                {
                  break;
                }
              QString url;
              url.setNum(count, 10);
              url = base_url + "&start=" + url;
              url += "&limit=100";
              std::unique_lock<std::mutex> ullock(next_url_mtx);
              next_url = false;
              emit signalDownload(url, save_p);
              next_url_var.wait(ullock,
                                [this]
                                  {
                                    return next_url || stop_urls;
                                  });
              if(stop_urls)
                {
                  break;
                }
              count += 100;
            }
          progr_count += 1.0;
          int progr = static_cast<int>(progr_count * 100.0 / total);
          emit signalProgress(progr);
        }
    }
}

void
DownloadProcessWindow::downloadDeals()
{
  std::string str = QDir::homePath().toStdString();
  std::filesystem::path save_dir(std::u8string(str.begin(), str.end()));
  save_dir /= u8".local";
  save_dir /= u8"share";
  save_dir /= u8"Money";
  save_dir /= u8"Data";
  save_dir /= u8"Deals";

  time_t curtm = time(nullptr);
  std::tm tm = *gmtime(&curtm);
  double jd = MoneyAux::dateToJulianDay(
      tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min,
      static_cast<double>(tm.tm_sec));
  jd += 3.0 / 24.0;
  int day, month, year, hour, minut;
  double sec;
  MoneyAux::julianDateToDate(jd, day, month, year, hour, minut, sec);

  std::stringstream strm;
  strm.imbue(std::locale("C"));
  strm << year;
  str = strm.str();

  strm.clear();
  strm.str("");
  strm << month;
  str += "_";
  if(month < 10)
    {
      str += "0";
    }
  str += strm.str();

  strm.clear();
  strm.str("");
  strm << day;
  str += "_";
  if(day < 10)
    {
      str += "0";
    }
  str += strm.str();
  save_dir /= std::u8string(str.begin(), str.end());

  str += "tmp";
  std::filesystem::path tmp_dir
      = save_dir.parent_path() / std::u8string(str.begin(), str.end());

  std::filesystem::remove_all(tmp_dir);
  std::filesystem::create_directories(tmp_dir);

  double progr_count = 0;
  double total
      = static_cast<double>(to_download.size() + to_download_foreign.size());

  for(int i = 1; i <= 2; i++)
    {
      std::vector<Security>::iterator begin;
      std::vector<Security>::iterator end;
      switch(i)
        {
        case 1:
          {
            begin = to_download.begin();
            end = to_download.end();
            break;
          }
        case 2:
          {
            begin = to_download_foreign.begin();
            end = to_download_foreign.end();
            break;
          }
        default:
          return void();
        }
      for(auto it = begin; it != end; it++)
        {
          if(cancel_all.load(std::memory_order_relaxed))
            {
              break;
            }

          std::u8string u8str(it->id.begin(), it->id.end());
          u8str += u8".csv";
          std::filesystem::path save_p = tmp_dir;
          save_p /= std::filesystem::path(u8str);

          QString base_url;

          switch(i)
            {
            case 1:
              {
                base_url = "https://iss.moex.com/iss/engines/stock/markets/"
                           "shares/securities/";
                break;
              }
            case 2:
              {
                base_url = "https://iss.moex.com/iss/engines/stock/markets/"
                           "foreignshares/securities/";
                break;
              }
            }

          base_url += it->id.c_str();
          base_url += "/trades.csv?";

          int count = 0;
          next_url_mtx.lock();
          stop_urls = false;
          next_url_mtx.unlock();
          for(;;)
            {
              if(cancel_all.load(std::memory_order_relaxed))
                {
                  break;
                }
              QString url;
              url.setNum(count, 10);
              url = base_url + "start=" + url;
              url += "&limit=5000";
              std::unique_lock<std::mutex> ullock(next_url_mtx);
              next_url = false;
              emit signalDownload(url, save_p);
              next_url_var.wait(ullock,
                                [this]
                                  {
                                    return next_url || stop_urls;
                                  });
              if(stop_urls)
                {
                  break;
                }
              count += 5000;
            }
          progr_count += 1.0;
          int progr = static_cast<int>(progr_count * 100.0 / total);
          emit signalProgress(progr);
        }
    }
  if(cancel_all.load(std::memory_order_relaxed))
    {
      std::filesystem::remove_all(tmp_dir);
    }
  else
    {
      std::filesystem::create_directories(save_dir);
      if(std::filesystem::exists(save_dir) && std::filesystem::exists(tmp_dir))
        {
          std::vector<std::filesystem::path> tdp;
          for(auto &dir_it : std::filesystem::directory_iterator(tmp_dir))
            {
              tdp.push_back(dir_it.path());
            }

          for(auto it = tdp.begin(); it != tdp.end(); it++)
            {
              std::filesystem::path p = save_dir / it->filename();
              std::filesystem::remove_all(p);
              std::error_code ec;
              std::filesystem::rename(*it, p, ec);
              if(ec)
                {
                  std::cout << "DownloadProcessWindow::downloadDeals: "
                            << ec.message() << std::endl;
                  return void();
                }
            }
        }
      std::filesystem::remove_all(tmp_dir);
    }
}

double
DownloadProcessWindow::getLastDate(const std::filesystem::path &fp,
                                   const std::string &instrument_id)
{
  double result = getBaseDate();

  std::fstream f;
  f.open(fp, std::ios_base::in);
  if(!f.is_open())
    {
      return result;
    }

  std::vector<std::string> lines;
  while(!f.eof())
    {
      std::string line;
      std::getline(f, line);
      lines.emplace_back(line);
    }
  f.close();

  std::string find_str(";");

  for(auto it = lines.begin(); it != lines.end(); it++)
    {
      std::string::size_type n = it->find(instrument_id);
      if(n == std::string::npos)
        {
          continue;
        }
      n = it->find(find_str);
      if(n == std::string::npos)
        {
          return result;
        }
      n += find_str.size();

      std::string::size_type n2 = it->find(find_str, n);
      if(n2 == std::string::npos)
        {
          return result;
        }

      std::string buf(it->begin() + n, it->begin() + n2);

      std::tm tm = {};
      std::stringstream strm;
      strm.str(buf);
      strm >> std::get_time(&tm, "%Y-%m-%d");
      double res = MoneyAux::dateToJulianDay(tm.tm_mday, tm.tm_mon + 1,
                                             tm.tm_year + 1900, 0, 0, 0.0);
      if(res > result)
        {
          result = res;
        }
    }

  return result;
}

double
DownloadProcessWindow::getBaseDate()
{
  return MoneyAux::dateToJulianDay(23, 3, 1997, 0, 0, 0.0);
}

void
DownloadProcessWindow::downloadUrl(const QString &url,
                                   const std::filesystem::path &save_path)
{
  QNetworkRequest request(url);

  QNetworkReply *repl = network->get(request);
  connect(repl, &QNetworkReply::errorOccurred, this,
          [this](QNetworkReply::NetworkError code)
            {
              if(code == QNetworkReply::NoError)
                {
                  return void();
                }
              cancel_all.store(true, std::memory_order_relaxed);
#pragma omp atomic write
              cancel_all_omp = true;
              std::lock_guard<std::mutex> lglock(next_url_mtx);
              stop_urls = true;
              next_url = true;
              next_url_var.notify_all();
            });
  connect(repl, &QNetworkReply::finished, this,
          [this, repl, save_path]
            {
              parseReply(repl, save_path);
            });
}

void
DownloadProcessWindow::parseReply(QNetworkReply *repl,
                                  const std::filesystem::path &save_p)
{
  QByteArray data = repl->readAll();
  repl->deleteLater();

  std::string buf;
  buf.reserve(data.size());
  for(qsizetype i = 0; i < data.size(); i++)
    {
      buf.push_back(data[i]);
    }

  buf = MoneyAux::convertToEncodign(buf, "windows-1251", "UTF-8");
  if(buf.empty())
    {
      cancel_all.store(true, std::memory_order_relaxed);
#pragma omp atomic write
      cancel_all_omp = true;
      std::lock_guard<std::mutex> lglock(next_url_mtx);
      stop_urls = true;
      next_url = true;
      next_url_var.notify_all();
      return void();
    }

  if(mode == DownloadProcessWindow::All)
    {
      std::string find_str("history.cursor");
      std::string::size_type n = buf.rfind(find_str);
      if(n != std::string::npos)
        {
          buf.erase(buf.begin() + n, buf.end());
        }

      if(std::filesystem::exists(save_p))
        {
          find_str = "TRADE_SESSION_DATE\n";
          n = buf.find(find_str);
          if(n != std::string::npos)
            {
              buf.erase(buf.begin(), buf.begin() + n + find_str.size());
            }
        }
      while(buf.size() > 0)
        {
          char ch = *buf.rbegin();
          if(ch >= 0 && ch <= ' ')
            {
              buf.pop_back();
            }
          else
            {
              break;
            }
        }

      if(buf.empty())
        {
          std::lock_guard<std::mutex> lglock(next_url_mtx);
          stop_urls = true;
          next_url = true;
          next_url_var.notify_all();
          return void();
        }
      buf.push_back('\n');

      std::fstream f;
      f.open(save_p,
             std::ios_base::out | std::ios_base::app | std::ios_base::binary);
      if(!f.is_open())
        {
          cancel_all.store(true, std::memory_order_relaxed);
#pragma omp atomic write
          cancel_all_omp = true;
          std::lock_guard<std::mutex> lglock(next_url_mtx);
          stop_urls = true;
          next_url = true;
          next_url_var.notify_all();
          return void();
        }
      f.write(buf.c_str(), buf.size());
      f.close();
    }
  else
    {
      std::string find_str("TRADE_SESSION_DATE\n");
      std::string::size_type n = buf.find(find_str);
      if(n == std::string::npos)
        {
          cancel_all.store(true, std::memory_order_relaxed);
#pragma omp atomic write
          cancel_all_omp = true;
          std::lock_guard<std::mutex> lglock(next_url_mtx);
          stop_urls = true;
          next_url = true;
          next_url_var.notify_all();
          return void();
        }
      if(std::filesystem::exists(save_p))
        {
          buf.erase(buf.begin(), buf.begin() + n + find_str.size());
          n = 0;
        }
      else
        {
          n += find_str.size();
        }
      find_str = "\n\n";
      std::string::size_type n2 = buf.find(find_str, n);
      if(n2 == std::string::npos)
        {
          cancel_all.store(true, std::memory_order_relaxed);
#pragma omp atomic write
          cancel_all_omp = true;
          std::lock_guard<std::mutex> lglock(next_url_mtx);
          stop_urls = true;
          next_url = true;
          next_url_var.notify_all();
          return void();
        }
      else if(n2 == n)
        {
          std::lock_guard<std::mutex> lglock(next_url_mtx);
          stop_urls = true;
          next_url = true;
          next_url_var.notify_all();
          return void();
        }
      buf.erase(buf.begin() + n2, buf.end());
      buf += "\n";

      std::fstream f;
      f.open(save_p,
             std::ios_base::out | std::ios_base::app | std::ios_base::binary);
      if(!f.is_open())
        {
          cancel_all.store(true, std::memory_order_relaxed);
#pragma omp atomic write
          cancel_all_omp = true;
          std::lock_guard<std::mutex> lglock(next_url_mtx);
          stop_urls = true;
          next_url = true;
          next_url_var.notify_all();
          return void();
        }
      f.write(buf.c_str(), buf.size());
      f.close();
    }

  std::lock_guard<std::mutex> lglock(next_url_mtx);
  next_url = true;
  next_url_var.notify_all();
}

void
DownloadProcessWindow::calculatePPM()
{
  std::string str = QDir::homePath().toStdString();
  std::filesystem::path data_dir(std::u8string(str.begin(), str.end()));
  data_dir /= u8".local";
  data_dir /= u8"share";
  data_dir /= u8"Money";
  data_dir /= u8"Data";
  data_dir /= u8"All";

  std::error_code ec;

  std::vector<std::filesystem::path> paths;

  std::filesystem::path ext(u8".csv");
  for(auto &dir_it : std::filesystem::directory_iterator(data_dir, ec))
    {
      std::filesystem::path p = dir_it.path();
      if(ext == p.extension())
        {
          paths.emplace_back(p);
        }
    }
  if(ec)
    {
      std::cout << "DownloadProcessWindow::calculatePPM: " << ec.message()
                << std::endl;
    }

  emit signalNewOperation(tr("PPM calculation:"), 0, 100);
  emit signalProgress(0);

  double progr = 0.0;
  double total = static_cast<double>(paths.size());
  std::vector<PPM> ppm_v;
  FileReader fr;
#pragma omp parallel
#pragma omp for
  for(auto it = paths.begin(); it != paths.end(); it++)
    {
      bool cncl;
#pragma omp atomic read
      cncl = cancel_all_omp;
      if(cncl)
        {
          continue;
        }
      std::vector<TradeDay> tdv = fr.readAll(*it);
      tdv.erase(std::remove_if(tdv.begin(), tdv.end(),
                               [](const TradeDay &el)
                                 {
                                   if(el.turnover == 0)
                                     {
                                       return true;
                                     }
                                   if(el.currency_id != "SUR")
                                     {
                                       if(el.currency_id != "RUB")
                                         {
                                           return true;
                                         }
                                     }
                                   return false;
                                 }),
                tdv.end());
      if(tdv.empty())
        {
#pragma omp atomic update
          progr += 1.0;

          double l_progr;
#pragma omp atomic read
          l_progr = progr;

          int val = static_cast<int>(l_progr * 100 / total);
          emit signalProgress(val);
          continue;
        }

      for(auto it_tdv = tdv.begin(); it_tdv != tdv.end(); it_tdv++)
        {
          PPM ppm;
          ppm.date = it_tdv->date;
          ppm.set(mpf_class(static_cast<double>(it_tdv->turnover)),
                  mpf_class(it_tdv->money_turnover));

          uint64_t date = static_cast<uint64_t>(ppm.date);
#pragma omp critical
          {
            auto it_ppm = std::find_if(
                ppm_v.begin(), ppm_v.end(),
                [date](const PPM &el)
                  {
                    return date == static_cast<uint64_t>(el.date);
                  });
            if(it_ppm == ppm_v.end())
              {
                ppm_v.emplace_back(ppm);
              }
            else
              {
                it_ppm->update(ppm);
              }
          }
        }

#pragma omp atomic update
      progr += 1.0;

      double l_progr;
#pragma omp atomic read
      l_progr = progr;

      int val = static_cast<int>(l_progr * 100 / total);
      emit signalProgress(val);
    }

  if(ppm_v.empty())
    {
      return void();
    }

  Algorithm::parallelSort(ppm_v.begin(), ppm_v.end(),
                          [](const PPM &el1, const PPM &el2)
                            {
                              return el1.date < el2.date;
                            });

  std::filesystem::path fp = data_dir.parent_path() / u8"PPM.csv";

  std::filesystem::remove_all(fp);

  std::fstream f;
  f.open(fp, std::ios_base::out | std::ios_base::binary);
  if(!f.is_open())
    {
      return void();
    }

  std::string line("DATE;TURNOVER;MONEY_TURNOVER;DAILY_PPM;TOTAL_PPM\n");
  f.write(line.c_str(), line.size());

  mpf_class total_turn = 0.0;
  mpf_class total_money = 0.0;
  for(auto it = ppm_v.begin(); it != ppm_v.end(); it++)
    {
      total_turn += it->turnover;
      total_money += it->money_turnover;
      it->total_ppm = total_turn / total_money;

      line.clear();
      int day, month, year, hour, minut;
      double sec;
      MoneyAux::julianDateToDate(it->date, day, month, year, hour, minut, sec);

      std::stringstream strm;
      strm.imbue(std::locale("C"));
      strm << year;
      line = strm.str();

      strm.clear();
      strm.str("");
      strm << month;
      line += "-";
      if(month < 10)
        {
          line += "0";
        }
      line += strm.str();

      strm.clear();
      strm.str("");
      strm << day;
      line += "-";
      if(day < 10)
        {
          line += "0";
        }
      line += strm.str();

      line += ";";

      strm.clear();
      strm.str("");
      strm << std::fixed;
      strm << std::setprecision(0);
      strm << it->turnover;
      line += strm.str();
      line += ";";

      strm.clear();
      strm.str("");
      strm << std::fixed;
      strm << std::setprecision(2);
      strm << it->money_turnover;
      line += strm.str();
      line += ";";

      strm.clear();
      strm.str("");
      strm << std::fixed;
      strm << std::setprecision(22);
      strm << it->current_ppm;
      line += strm.str();
      line += ";";

      strm.clear();
      strm.str("");
      strm << std::fixed;
      strm << std::setprecision(22);
      strm << it->total_ppm;
      line += strm.str();
      line += "\n";

      f.write(line.c_str(), line.size());
    }
  f.close();
}

void
DownloadProcessWindow::finalDialog()
{
  QWidget *window = new QWidget(this->parentWidget());
  window->setWindowFlag(Qt::Window);
  window->setAttribute(Qt::WA_DeleteOnClose);
  window->setWindowModality(Qt::WindowModal);

  QVBoxLayout *v_box = new QVBoxLayout;
  window->setLayout(v_box);

  QLabel *lab = new QLabel;
  if(cancel_all.load(std::memory_order_relaxed))
    {
      lab->setWordWrap(true);
      lab->setText(tr("Downloading has been interrupted"));
    }
  else
    {
      lab->setText(tr("Downloading completed"));
    }
  lab->setAlignment(Qt::AlignCenter);
  v_box->addWidget(lab, 0, Qt::AlignCenter);

  QPushButton *close = new QPushButton;
  close->setText(tr("Close"));
  connect(close, &QPushButton::clicked, window, &QWidget::close);
  v_box->addWidget(close, 0, Qt::AlignCenter);

  window->show();
}
