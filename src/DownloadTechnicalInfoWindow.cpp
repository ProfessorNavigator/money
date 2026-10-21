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

#include <DownloadTechnicalInfoWindow.h>
#include <MoneyAux.h>
#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QNetworkRequest>
#include <QPushButton>
#include <QVBoxLayout>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <thread>

DownloadTechnicalInfoWindow::DownloadTechnicalInfoWindow(QWidget *parent)
    : QWidget(parent)
{
  this->setWindowFlag(Qt::Window);
  this->setAttribute(Qt::WA_DeleteOnClose);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle(tr("Downloading") + "...");

  allow_close.store(false, std::memory_order_relaxed);
  stop_all.store(false, std::memory_order_relaxed);

  network = new QNetworkAccessManager(this);

  connect(this, &DownloadTechnicalInfoWindow::signalClose, this,
          [this]
            {
              allow_close.store(true, std::memory_order_relaxed);
              this->close();
            });

  createWindow();
}

DownloadTechnicalInfoWindow::~DownloadTechnicalInfoWindow()
{
  std::unique_lock<std::mutex> ullock(active_requests_mtx);
  active_requests_var.wait(ullock,
                           [this]
                             {
                               return active_requests <= 0;
                             });
  emit signalFinished(!stop_all.load(std::memory_order_relaxed));
}

void
DownloadTechnicalInfoWindow::closeEvent(QCloseEvent *event)
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
DownloadTechnicalInfoWindow::createWindow()
{
  QVBoxLayout *v_box = new QVBoxLayout;
  this->setLayout(v_box);

  QLabel *lab = new QLabel;
  QFont font = lab->font();
  font.setItalic(true);
  lab->setFont(font);
  lab->setText(tr("Downloading technical info") + "...");
  v_box->addWidget(lab);

  QPushButton *cancel = new QPushButton;
  cancel->setText(tr("Cancel"));
  connect(cancel, &QPushButton::clicked, this,
          [this]
            {
              stop_all.store(true, std::memory_order_relaxed);
            });
  v_box->addWidget(cancel, 0, Qt::AlignCenter);

  std::string str = QDir::homePath().toStdString();

  std::filesystem::path p(std::u8string(str.begin(), str.end()));
  p /= u8".local";
  p /= u8"share";
  p /= u8"Money";

  for(int i = 1; i <= 2; i++)
    {
      QNetworkRequest req;
      std::filesystem::path save_p = p;
      switch(i)
        {
        case 1:
          {
            req.setUrl(QUrl("https://iss.moex.com/iss/engines/stock/markets/"
                            "shares/boards.csv"));
            save_p /= "shares_boards.csv";
            break;
          }
        case 2:
          {
            req.setUrl(QUrl("https://iss.moex.com/iss/engines/stock/markets/"
                            "foreignshares/boards.csv"));
            save_p /= "foreignshares_boards.csv";
            break;
          }
        default:
          {
            this->deleteLater();
            return void();
          }
        }

      QNetworkReply *active_request = network->get(req);
      active_requests_mtx.lock();
      active_requests++;
      active_requests_mtx.unlock();
      connect(active_request, &QNetworkReply::errorOccurred, this,
              &DownloadTechnicalInfoWindow::errorDialog);

      connect(active_request, &QNetworkReply::finished, this,
              [this, active_request, save_p]
                {
                  parseBoardsRequestReply(active_request, save_p);
                  std::lock_guard<std::mutex> lglock(active_requests_mtx);
                  active_requests--;
                  active_requests_var.notify_all();
                });
    }

  connect(this, &DownloadTechnicalInfoWindow::signalNewListingRequest, this,
          &DownloadTechnicalInfoWindow::sendListingRequest);

  std::thread thr(
      [this]
        {
          std::unique_lock<std::mutex> ullock(active_requests_mtx);
          active_requests_var.wait(ullock,
                                   [this]
                                     {
                                       return active_requests <= 0;
                                     });
          if(stop_all.load(std::memory_order_relaxed))
            {
              emit signalClose();
              return void();
            }
          ullock.unlock();
          formListingRequests();
        });
  thr.detach();
}

void
DownloadTechnicalInfoWindow::errorDialog(QNetworkReply::NetworkError code)
{
  switch(code)
    {
    case QNetworkReply::NoError:
    case QNetworkReply::OperationCanceledError:
      {
        return void();
      }
    default:
      break;
    }

  QWidget *window = new QWidget(this->parentWidget());
  window->setWindowFlag(Qt::Window);
  window->setAttribute(Qt::WA_DeleteOnClose);
  window->setWindowModality(Qt::WindowModal);

  QVBoxLayout *v_box = new QVBoxLayout;
  window->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setText(tr("Error occured!"));
  v_box->addWidget(lab, 0, Qt::AlignCenter);

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  lab = new QLabel;
  lab->setText(tr("Error code") + ":");
  lab->setAlignment(Qt::AlignRight);
  h_box->addWidget(lab);

  lab = new QLabel;
  QFont font = lab->font();
  font.setItalic(true);
  lab->setFont(font);
  QString qstr;
  qstr.setNum(code);
  lab->setAlignment(Qt::AlignLeft);
  lab->setText(qstr);
  h_box->addWidget(lab);

  window->show();

  stop_all.store(true);
  std::lock_guard<std::mutex> lglock(active_requests_mtx);
  active_requests--;
  active_requests_var.notify_all();
}

void
DownloadTechnicalInfoWindow::formListingRequests()
{
  std::string str = QDir::homePath().toStdString();
  std::filesystem::path p(std::u8string(str.begin(), str.end()));
  p /= u8".local";
  p /= u8"share";
  p /= u8"Money";
  p /= u8"foreign_securities.csv";
  std::filesystem::remove_all(p);
  p = p.parent_path() / u8"securities.csv";
  std::filesystem::remove_all(p);

  int count = 0;
  QString limit("100");
  for(;;)
    {
      if(stop_all.load(std::memory_order_relaxed))
        {
          break;
        }
      QString start;
      start.setNum(count, 10);
      QString url = "https://iss.moex.com/iss/history/engines/"
                    "stock/markets/shares/listing.csv?start=";
      url += start;
      url += "&limit=";
      url += limit;
      std::unique_lock<std::mutex> ullock(active_requests_mtx);
      active_requests++;
      emit signalNewListingRequest(url, false);
      active_requests_var.wait(ullock,
                               [this]
                                 {
                                   return active_requests <= 0;
                                 });
      if(stop_listing_requests)
        {
          break;
        }

      count += 100;
    }

  active_requests_mtx.lock();
  stop_listing_requests = false;
  active_requests_mtx.unlock();

  count = 0;
  for(;;)
    {
      if(stop_all.load(std::memory_order_relaxed))
        {
          break;
        }
      QString start;
      start.setNum(count, 10);
      QString url = "https://iss.moex.com/iss/history/engines/"
                    "stock/markets/foreignshares/listing.csv?start=";
      url += start;
      url += "&limit=";
      url += limit;
      std::unique_lock<std::mutex> ullock(active_requests_mtx);
      active_requests++;
      emit signalNewListingRequest(url, true);
      active_requests_var.wait(ullock,
                               [this]
                                 {
                                   return active_requests <= 0;
                                 });
      if(stop_listing_requests)
        {
          break;
        }
      count += 100;
    }
  emit signalClose();
}

void
DownloadTechnicalInfoWindow::sendListingRequest(const QString &url,
                                                const bool &foreign)
{
  QNetworkRequest req((QUrl(url)));

  QNetworkReply *repl = network->get(req);
  connect(repl, &QNetworkReply::errorOccurred, this,
          &DownloadTechnicalInfoWindow::errorDialog);

  connect(repl, &QNetworkReply::finished, this,
          [this, repl, foreign]
            {
              parseListingRequestReply(repl, foreign);
            });
}

void
DownloadTechnicalInfoWindow::parseListingRequestReply(QNetworkReply *repl,
                                                      const bool &foreign)
{
  QByteArray data = repl->readAll();
  repl->deleteLater();
  std::string buf;
  buf.reserve(static_cast<size_t>(data.size()));
  for(qsizetype i = 0; i < data.size(); i++)
    {
      buf.push_back(data[i]);
    }

  buf = MoneyAux::convertToEncodign(buf, "windows-1251", "UTF-8");

  std::string str = QDir::homePath().toStdString();
  std::filesystem::path p(std::u8string(str.begin(), str.end()));
  p /= u8".local";
  p /= u8"share";
  p /= u8"Money";
  if(foreign)
    {
      p /= u8"foreign_securities.csv";
    }
  else
    {
      p /= u8"securities.csv";
    }

  if(std::filesystem::exists(p))
    {
      str = "history_till\n";
      std::string::size_type n = buf.find(str);
      if(n == std::string::npos)
        {
          stop_all.store(true, std::memory_order_relaxed);
          std::unique_lock<std::mutex> ullock(active_requests_mtx);
          active_requests--;
          active_requests_var.notify_all();
          return void();
        }
      buf.erase(0, n + str.size());

      auto it = std::find_if(buf.begin(), buf.end(),
                             [](const char &el)
                               {
                                 if(el < 0 || el > ' ')
                                   {
                                     return true;
                                   }
                                 return false;
                               });
      if(it == buf.end())
        {
          std::unique_lock<std::mutex> ullock(active_requests_mtx);
          active_requests--;
          stop_listing_requests = true;
          active_requests_var.notify_all();
          return void();
        }
    }

  for(auto it = buf.begin(); it != buf.end();)
    {
      char ch = *it;
      if(ch >= 0 && ch <= ' ')
        {
          buf.erase(it);
        }
      else
        {
          break;
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
  buf.push_back('\n');

  std::fstream f;
  f.open(p, std::ios_base::out | std::ios_base::app | std::ios_base::binary);
  if(!f.is_open())
    {
      stop_all.store(true, std::memory_order_relaxed);
      std::unique_lock<std::mutex> ullock(active_requests_mtx);
      active_requests--;
      active_requests_var.notify_all();
      return void();
    }
  f.write(buf.c_str(), buf.size());
  f.close();

  std::unique_lock<std::mutex> ullock(active_requests_mtx);
  active_requests--;
  active_requests_var.notify_all();
}

void
DownloadTechnicalInfoWindow::parseBoardsRequestReply(
    QNetworkReply *repl, const std::filesystem::path &save_p)
{
  QByteArray data = repl->readAll();
  repl->deleteLater();
  if(data.size() <= 0)
    {
      return void();
    }

  std::string buf;
  buf.reserve(data.size());
  for(qsizetype i = 0; i < data.size(); i++)
    {
      buf.push_back(data[i]);
    }

  buf = MoneyAux::convertToEncodign(buf, "windows-1251", "UTF-8");

  if(buf.empty())
    {
      return void();
    }

  std::filesystem::path p = save_p.parent_path();
  std::filesystem::create_directories(p);

  p /= MoneyAux::randomFileName();

  std::fstream f;
  f.open(p, std::ios_base::out | std::ios_base::binary);
  if(!f.is_open())
    {
      return void();
    }
  f.write(buf.c_str(), buf.size());
  f.close();

  std::filesystem::remove_all(save_p);
  std::error_code ec;
  std::filesystem::rename(p, save_p, ec);
}
