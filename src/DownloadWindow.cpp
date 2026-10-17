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

#include <DownloadProcessWindow.h>
#include <DownloadWindow.h>
#include <FileReader.h>
#include <MoneyAux.h>
#include <QCompleter>
#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <filesystem>

DownloadWindow::DownloadWindow(QWidget *parent) : QWidget(parent)
{
  this->setWindowFlag(Qt::Window);
  this->setAttribute(Qt::WA_DeleteOnClose);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle(tr("Download mode selection"));

  createWindow();
}

void
DownloadWindow::createWindow()
{
  QVBoxLayout *v_box = new QVBoxLayout;
  this->setLayout(v_box);

  QLabel *lab = new QLabel;
  QFont font = lab->font();
  font.setBold(true);
  lab->setFont(font);
  lab->setText(tr("Mode"));
  v_box->addWidget(lab, 0, Qt::AlignCenter);

  download_mode = new QComboBox;
  download_mode->addItem(tr("Common data"),
                         QVariant::fromValue(DownloadMode::AllAll));
  download_mode->addItem(tr("Common data for instrument"),
                         QVariant::fromValue(DownloadMode::AllInstrument));
  download_mode->addItem(tr("Deals for current date"),
                         QVariant::fromValue(DownloadMode::DailyAll));
  download_mode->addItem(tr("Deals for current date for instrument"),
                         QVariant::fromValue(DownloadMode::DailyInstrument));
  v_box->addWidget(download_mode, 0, Qt::AlignCenter);

  instrument = new QComboBox;
  instrument->setVisible(false);
  v_box->addWidget(instrument, 0, Qt::AlignCenter);

  connect(download_mode, &QComboBox::activated, this,
          [this](int index)
            {
              QVariant var = download_mode->itemData(index);
              DownloadMode mode = var.value<DownloadMode>();
              if(mode == DownloadMode::AllInstrument
                 || mode == DownloadMode::DailyInstrument)
                {
                  instrument->setVisible(true);
                }
              else
                {
                  instrument->setVisible(false);
                }
            });

  instrument->setEditable(true);
  instrument->setInsertPolicy(QComboBox::NoInsert);

  QSortFilterProxyModel *proxy = new QSortFilterProxyModel(instrument);
  proxy->setSourceModel(instrument->model());
  proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

  QCompleter *completer = new QCompleter(proxy, instrument);
  completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  instrument->setCompleter(completer);

  connect(instrument->lineEdit(), &QLineEdit::textEdited, proxy,
          &QSortFilterProxyModel::setFilterFixedString);

  formInstrumentCB(Instrument::Local);
  formInstrumentCB(Instrument::Foreign);

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  QPushButton *download = new QPushButton;
  download->setText(tr("Download"));
  connect(download, &QPushButton::clicked, this,
          &DownloadWindow::startDownload);
  h_box->addWidget(download, 0, Qt::AlignCenter);

  QPushButton *cancel = new QPushButton;
  cancel->setText(tr("Cancel"));
  connect(cancel, &QPushButton::clicked, this, &DownloadWindow::close);
  h_box->addWidget(cancel, 0, Qt::AlignCenter);
}

void
DownloadWindow::formInstrumentCB(const Instrument &instr)
{
  std::string str = QDir::homePath().toStdString();
  std::filesystem::path fp(std::u8string(str.begin(), str.end()));
  fp /= u8".local";
  fp /= u8"share";
  fp /= u8"Money";
  FileReader fr;
  std::string last;
  if(instr == Instrument::Local)
    {
      fp /= u8"securities.csv";
      securities = fr.readSecurities(fp);
      for(auto it = securities.begin(); it != securities.end(); it++)
        {
          if(it->id != last)
            {
              last = it->id;
              QString qstr = it->id.c_str();
              qstr += " ";
              qstr += it->name.c_str();
              instrument->addItem(qstr, QVariant::fromValue(*it));
            }
        }
    }
  else
    {
      fp /= u8"foreign_securities.csv";
      foreign_securities = fr.readSecurities(fp);
      for(auto it = foreign_securities.begin(); it != foreign_securities.end();
          it++)
        {
          if(it->id != last)
            {
              last = it->id;
              QString qstr = it->id.c_str();
              qstr += " ";
              qstr += it->name.c_str();
              instrument->addItem(qstr, QVariant::fromValue(*it));
            }
        }
    }
}

void
DownloadWindow::startDownload()
{
  DownloadMode mode = download_mode->currentData().value<DownloadMode>();
  DownloadProcessWindow *dpw;
  switch(mode)
    {
    case DownloadMode::AllAll:
      {
        dpw = new DownloadProcessWindow(securities, foreign_securities,
                                        DownloadProcessWindow::All, this);
        break;
      }
    case DownloadMode::AllInstrument:
      {
        Security instr = instrument->currentData().value<Security>();
        std::vector<Security> to_download;
        std::vector<Security> to_download_foreign;
        for(auto it = securities.begin(); it != securities.end(); it++)
          {
            if(it->id == instr.id)
              {
                to_download.push_back(*it);
              }
          }
        for(auto it = foreign_securities.begin();
            it != foreign_securities.end(); it++)
          {
            if(it->id == instr.id)
              {
                to_download_foreign.push_back(*it);
              }
          }

        dpw = new DownloadProcessWindow(to_download, to_download_foreign,
                                        DownloadProcessWindow::All, this);
        break;
      }
    case DownloadMode::DailyAll:
      {
        dpw = new DownloadProcessWindow(securities, foreign_securities,
                                        DownloadProcessWindow::Deals, this);
        break;
      }
    case DownloadMode::DailyInstrument:
      {
        Security instr = instrument->currentData().value<Security>();
        std::vector<Security> to_download;
        std::vector<Security> to_download_foreign;
        for(auto it = securities.begin(); it != securities.end(); it++)
          {
            if(it->id == instr.id)
              {
                to_download.push_back(*it);
              }
          }
        for(auto it = foreign_securities.begin();
            it != foreign_securities.end(); it++)
          {
            if(it->id == instr.id)
              {
                to_download_foreign.push_back(*it);
              }
          }

        dpw = new DownloadProcessWindow(to_download, to_download_foreign,
                                        DownloadProcessWindow::Deals, this);
        break;
      }
    default:
      return void();
    }

  dpw->show();
  dpw->startDownload();
}
