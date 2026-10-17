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
#ifndef DOWNLOADTECHNICALINFOWINDOW_H
#define DOWNLOADTECHNICALINFOWINDOW_H

#include <QCloseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWidget>
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <mutex>

class DownloadTechnicalInfoWindow : public QWidget
{
  Q_OBJECT
public:
  DownloadTechnicalInfoWindow(QWidget *parent = nullptr);

  virtual ~DownloadTechnicalInfoWindow();

signals:
  void
  signalFinished(const bool &correct);

protected:
  void
  closeEvent(QCloseEvent *event) override;

private:
  void
  createWindow();

  void
  errorDialog(QNetworkReply::NetworkError code);

  void
  formListingRequests();

  void
  sendListingRequest(const QString &url, const bool &foreign);

  void
  parseListingRequestReply(QNetworkReply *repl, const bool &foreign);

  void
  parseBoardsRequestReply(QNetworkReply *repl,
                          const std::filesystem::path &save_p);

  QNetworkAccessManager *network;

  int active_requests = 0;
  std::mutex active_requests_mtx;
  std::condition_variable active_requests_var;

  bool stop_listing_requests = false;

  std::atomic<bool> allow_close;

  std::atomic<bool> stop_all;

signals:
  void
  signalClose();

  void
  signalNewListingRequest(const QString &url, const bool &foreign);
};

#endif // DOWNLOADTECHNICALINFOWINDOW_H
