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
#ifndef DOWNLOADPROCESSWINDOW_H
#define DOWNLOADPROCESSWINDOW_H

#include <QCloseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWidget>
#include <Security.h>
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <vector>

class DownloadProcessWindow : public QWidget
{
  Q_OBJECT
public:
  enum Mode
  {
    All,
    Deals
  };

  DownloadProcessWindow(const std::vector<Security> &securities,
                        const std::vector<Security> &foreign_securities,
                        const Mode &mode, QWidget *parent = nullptr);

  void
  startDownload();

protected:
  void
  closeEvent(QCloseEvent *event) override;

private:
  void
  createWindow();

  void
  formDownloadVectors();

  void
  downloadAll();

  void
  downloadDeals();

  double
  getLastDate(const std::filesystem::path &fp,
              const std::string &instrument_id);

  double
  getBaseDate();

  void
  downloadUrl(const QString &url, const std::filesystem::path &save_path);

  void
  parseReply(QNetworkReply *repl, const std::filesystem::path &save_p);

  void
  calculatePPM();

  void
  finalDialog();

  std::vector<Security> securities;
  std::vector<Security> foreign_securities;
  Mode mode;

  std::atomic<bool> allow_close;
  std::atomic<bool> cancel_all;
  bool cancel_all_omp = false;

  QNetworkAccessManager *network;

  std::vector<Security> to_download;
  std::vector<Security> to_download_foreign;

  bool next_url = true;
  bool stop_urls = false;
  std::mutex next_url_mtx;
  std::condition_variable next_url_var;

signals:
  void
  signalDownload(const QString &url, const std::filesystem::path &save_path);

  void
  signalFinished();

  void
  signalProgress(int val);

  void
  signalNewOperation(const QString &operation_name, const int &min,
                     const int &max);
};

#endif // DOWNLOADPROCESSWINDOW_H
