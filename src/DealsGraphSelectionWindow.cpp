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
#include <DealsGraphSelectionWindow.h>
#include <DiagramWindow.h>
#include <FileReader.h>
#include <GraphDataWindow.h>
#include <LoadingDialog.h>
#include <MoneyAux.h>
#include <QCompleter>
#include <QCoreApplication>
#include <QDir>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <algorithm>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <thread>

DealsGraphSelectionWindow::DealsGraphSelectionWindow(QWidget *parent)
    : QWidget(parent)
{
  this->setWindowFlag(Qt::Window);
  this->setAttribute(Qt::WA_DeleteOnClose);
  this->setWindowTitle(tr("Deals"));
}

void
DealsGraphSelectionWindow::showEvent(QShowEvent *event)
{
  showed = true;
  QWidget::showEvent(event);
}

void
DealsGraphSelectionWindow::createWindow()
{
  std::string home_p = QDir::homePath().toStdString();
  std::filesystem::path fp(std::u8string(home_p.begin(), home_p.end()));
  fp /= u8".local";
  fp /= u8"share";
  fp /= u8"Money";
  fp /= u8"securities.csv";
  FileReader fr;
  securities = fr.readSecurities(fp);
  fp = fp.parent_path() / u8"foreign_securities.csv";
  std::vector<Security> sec = fr.readSecurities(fp);
  std::copy(sec.begin(), sec.end(), std::back_inserter(securities));

  fp = fp.parent_path() / u8"shares_boards.csv";
  boards = fr.readBoards(fp);

  fp = fp.parent_path() / u8"foreignshares_boards.csv";
  std::vector<Board> brds = fr.readBoards(fp);
  std::copy(brds.begin(), brds.end(), std::back_inserter(boards));

  QVBoxLayout *v_box = new QVBoxLayout;
  this->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setText(tr("Date") + ":");
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  date = new QComboBox;
  v_box->addWidget(date);

  std::filesystem::path dir_p(std::u8string(home_p.begin(), home_p.end()));
  dir_p /= u8".local";
  dir_p /= u8"share";
  dir_p /= u8"Money";
  dir_p /= u8"Data";
  dir_p /= u8"Deals";

  std::vector<std::tuple<QString, std::filesystem::path, double>> dates_list;
  std::error_code ec;
  for(auto &dir_it : std::filesystem::directory_iterator(dir_p, ec))
    {
      std::filesystem::path p = dir_it.path();
      if(!std::filesystem::is_directory(p))
        {
          continue;
        }

      std::u8string u8str = p.filename().u8string();
      std::stringstream strm;
      strm.str(std::string(u8str.begin(), u8str.end()));
      std::tm tm = {};
      strm >> std::get_time(&tm, "%Y_%m_%d");
      QString val;
      if(tm.tm_mday < 10)
        {
          val = "0";
        }
      val += QString::number(tm.tm_mday);
      val += ".";
      if(tm.tm_mon + 1 < 10)
        {
          val += "0";
        }
      val += QString::number(tm.tm_mon + 1);
      val += ".";
      val += QString::number(tm.tm_year + 1900);

      dates_list.push_back(std::make_tuple(
          val, p,
          MoneyAux::dateToJulianDay(tm.tm_mday, tm.tm_mon + 1,
                                    tm.tm_year + 1900, 0, 0, 0.0)));
    }

  if(ec)
    {
      std::cout << "DealsGraphSelectionWindow::createWindow: " << ec.message()
                << std::endl;
    }

  Algorithm::parallelSort(
      dates_list.begin(), dates_list.end(),
      [](const std::tuple<QString, std::filesystem::path, double> &el1,
         const std::tuple<QString, std::filesystem::path, double> &el2)
        {
          return std::get<2>(el1) > std::get<2>(el2);
        });

  for(auto it = dates_list.begin(); it != dates_list.end(); it++)
    {
      date->addItem(std::get<0>(*it), QVariant::fromValue(std::get<1>(*it)));
    }

  lab = new QLabel;
  lab->setText(tr("Instrument") + ":");
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  instrument = new QComboBox;
  v_box->addWidget(instrument);

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

  lab = new QLabel;
  lab->setText(tr("Board") + ":");
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  board = new QComboBox;
  v_box->addWidget(board);

  lab = new QLabel;
  lab->setText(tr("Diagram mode") + ":");
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  mode = new QComboBox;
  mode->addItem(tr("Prices"), QVariant::fromValue(Mode::Prices));
  mode->addItem(tr("PPM"), QVariant::fromValue(Mode::PPM));
  mode->addItem(tr("Turnover"), QVariant::fromValue(Mode::Turnover));
  mode->addItem(tr("Money turnover"),
                QVariant::fromValue(Mode::MoneyTurnover));
  v_box->addWidget(mode);

  v_box->addWidget(times(), 0, Qt::AlignCenter);

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  QPushButton *show = new QPushButton;
  show->setText(tr("Show diagram"));
  connect(show, &QPushButton::clicked, this,
          &DealsGraphSelectionWindow::checkInput);
  h_box->addWidget(show, 0, Qt::AlignCenter);

  QPushButton *cancel = new QPushButton;
  cancel->setText(tr("Cancel"));
  connect(cancel, &QPushButton::clicked, this,
          &DealsGraphSelectionWindow::close);
  h_box->addWidget(cancel, 0, Qt::AlignCenter);

  setDate(0);
  connect(date, &QComboBox::currentIndexChanged, this,
          &DealsGraphSelectionWindow::setDate);
  connect(instrument, &QComboBox::currentIndexChanged, this,
          &DealsGraphSelectionWindow::setInstrument);
}

void
DealsGraphSelectionWindow::setDate(int date_index)
{
  if(date_index >= date->count())
    {
      emit signalLoaded();
      return void();
    }
  std::filesystem::path dir_p
      = date->itemData(date_index).value<std::filesystem::path>();

  instrument->clear();

  std::vector<std::tuple<QString, std::filesystem::path, size_t>> files;
  std::error_code ec;
  for(auto &dir_it : std::filesystem::directory_iterator(dir_p, ec))
    {
      std::filesystem::path p = dir_it.path();
      std::u8string u8str = p.stem().u8string();
      std::string str(u8str.begin(), u8str.end());
      QString val(str.c_str());

      auto it = Algorithm::parallelFindIf(securities.begin(), securities.end(),
                                          [str](const Security &el)
                                            {
                                              return str == el.id;
                                            });
      if(it != securities.end())
        {
          if(!it->name.empty())
            {
              val += " ";
              val += it->name.c_str();
            }
        }
      std::error_code l_ec;
      size_t fsz = static_cast<size_t>(std::filesystem::file_size(p, l_ec));
      if(ec)
        {
          std::cout << "DealsGraphSelectionWindow::setInstrument: "
                    << l_ec.message() << std::endl;
          continue;
        }
      files.push_back(std::make_tuple(val, p, fsz));
    }
  if(ec)
    {
      std::cout << "DealsGraphSelectionWindow::setInstrument: " << ec.message()
                << std::endl;
    }

  if(files.size() > 1)
    {
      Algorithm::parallelSort(
          files.begin(), files.end(),
          [](const std::tuple<QString, std::filesystem::path, size_t> &el1,
             const std::tuple<QString, std::filesystem::path, size_t> &el2)
            {
              return std::get<2>(el1) > std::get<2>(el2);
            });
    }
  for(auto it = files.begin(); it != files.end(); it++)
    {
      instrument->addItem(std::get<0>(*it),
                          QVariant::fromValue(std::get<1>(*it)));
    }
  setInstrument(0);
}

void
DealsGraphSelectionWindow::setInstrument(int instrument_index)
{
  if(instrument_index >= instrument->count())
    {
      emit signalLoaded();
      return void();
    }
  std::filesystem::path p
      = instrument->itemData(instrument_index).value<std::filesystem::path>();
  std::u8string u8str = p.stem().u8string();
  std::string str(u8str.begin(), u8str.end());

  std::vector<std::string> brds;
#pragma omp parallel for
  for(auto it = securities.begin(); it != securities.end(); it++)
    {
      if(it->id == str)
        {
#pragma omp critical
          {
            brds.push_back(it->board_id);
          }
        }
    }

  board->clear();
  int count = 0;
  int cur = 0;
  for(auto it = brds.begin(); it != brds.end(); it++)
    {
      QString val(it->c_str());
      auto it_brd = Algorithm::parallelFindIf(boards.begin(), boards.end(),
                                              [it](const Board &el)
                                                {
                                                  return *it == el.id;
                                                });
      if(it_brd != boards.end())
        {
          if(!it_brd->name.empty())
            {
              val += " ";
              val += it_brd->name.c_str();
            }
        }
      board->addItem(val, QVariant::fromValue(*it));
      if(*it == "TQBR" || *it == "FQBR")
        {
          cur = count;
        }
      count++;
    }
  board->setCurrentIndex(cur);

  if(board->count() <= 0)
    {
      emit signalLoaded();
      return void();
    }

  std::string board_id = board->currentData().value<std::string>();
  if(board_id == "EQTD" || board_id == "TQFD" || board_id == "TQPD"
     || board_id == "TQTD" || board_id == "TQBD" || board_id == "TQDD")
    {
      currency = "USD";
    }
  else if(board_id == "EQTU" || board_id == "TQBE" || board_id == "TQFE"
          || board_id == "TQPE" || board_id == "TQTE")
    {
      currency = "EUR";
    }
  else if(board_id == "TQPH" || board_id == "TQTH" || board_id == "FQBH"
          || board_id == "FQDH")
    {
      currency = "HKD";
    }
  else if(board_id == "TQPY" || board_id == "TQTY" || board_id == "FQBY"
          || board_id == "FQDY")
    {
      currency = "CNY";
    }
  else
    {
      currency = "RUB";
    }

  if(showed)
    {
      LoadingDialog *ld = new LoadingDialog(this);
      connect(
          this, &DealsGraphSelectionWindow::signalLoaded, ld,
          [this, ld]
            {
              if(deals.size() > 0)
                {
                  int day, month, year, hour, minut;
                  double sec;
                  MoneyAux::julianDateToDate(deals.begin()->deal_time, day,
                                             month, year, hour, minut, sec);
                  QString val;
                  if(hour < 10)
                    {
                      val = "0";
                    }
                  val += QString::number(hour);
                  val += ":";
                  if(minut < 10)
                    {
                      val += "0";
                    }
                  val += QString::number(minut);
                  val += ":";
                  if(sec < 10.0)
                    {
                      val += "0";
                    }
                  val += QString::number(sec, 'f', 0);
                  time_start->setText(val);

                  val.clear();
                  MoneyAux::julianDateToDate(deals.rbegin()->deal_time, day,
                                             month, year, hour, minut, sec);
                  if(hour < 10)
                    {
                      val = "0";
                    }
                  val += QString::number(hour);
                  val += ":";
                  if(minut < 10)
                    {
                      val += "0";
                    }
                  val += QString::number(minut);
                  val += ":";
                  if(sec < 10.0)
                    {
                      val += "0";
                    }
                  val += QString::number(sec, 'f', 0);
                  time_end->setText(val);
                }
              ld->allowClose(true);
              ld->close();
            });
      ld->show();
    }
  else
    {
      std::shared_ptr<QMetaObject::Connection> con(
          new QMetaObject::Connection);
      *con = connect(
          this, &DealsGraphSelectionWindow::signalLoaded, this,
          [this, con]
            {
              if(deals.size() > 0)
                {
                  int day, month, year, hour, minut;
                  double sec;
                  MoneyAux::julianDateToDate(deals.begin()->deal_time, day,
                                             month, year, hour, minut, sec);
                  QString val;
                  if(hour < 10)
                    {
                      val = "0";
                    }
                  val += QString::number(hour);
                  val += ":";
                  if(minut < 10)
                    {
                      val += "0";
                    }
                  val += QString::number(minut);
                  val += ":";
                  if(sec < 10.0)
                    {
                      val += "0";
                    }
                  val += QString::number(sec, 'f', 0);
                  time_start->setText(val);

                  val.clear();
                  MoneyAux::julianDateToDate(deals.rbegin()->deal_time, day,
                                             month, year, hour, minut, sec);
                  if(hour < 10)
                    {
                      val = "0";
                    }
                  val += QString::number(hour);
                  val += ":";
                  if(minut < 10)
                    {
                      val += "0";
                    }
                  val += QString::number(minut);
                  val += ":";
                  if(sec < 10.0)
                    {
                      val += "0";
                    }
                  val += QString::number(sec, 'f', 0);
                  time_end->setText(val);
                }
              disconnect(*con);
            });
    }
  std::thread thr(
      [this, board_id, p]
        {
          FileReader fr;
          deals = fr.readDeals(p);
          deals.erase(Algorithm::parallelRemoveIf(deals.begin(), deals.end(),
                                                  [board_id](const Deal &el)
                                                    {
                                                      return el.board_id
                                                             != board_id;
                                                    }),
                      deals.end());
          emit signalLoaded();
        });
  thr.detach();
}

QWidget *
DealsGraphSelectionWindow::times()
{
  QWidget *result = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setContentsMargins(0, 0, 0, 0);
  result->setLayout(grid);

  QLabel *lab = new QLabel;
  lab->setText(tr("Start time") + ":");
  grid->addWidget(lab, 0, 0, Qt::AlignLeft);

  time_start = new QLineEdit;
  QString mask("99:99:99");
  time_start->setInputMask(mask);
  QFontMetrics fm = time_start->fontMetrics();
  QRect rect = fm.boundingRect(mask);
  QMargins mg = time_start->textMargins();
  time_start->setFixedWidth(rect.width() + mg.left() + mg.right()
                            + 2 * fm.averageCharWidth());
  grid->addWidget(time_start, 1, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(tr("End time") + ":");
  grid->addWidget(lab, 0, 1, Qt::AlignLeft);

  time_end = new QLineEdit;
  time_end->setInputMask(mask);
  fm = time_end->fontMetrics();
  rect = fm.boundingRect(mask);
  mg = time_end->textMargins();
  time_end->setFixedWidth(rect.width() + mg.left() + mg.right()
                          + 2 * fm.averageCharWidth());
  grid->addWidget(time_end, 1, 1, Qt::AlignLeft);

  return result;
}

void
DealsGraphSelectionWindow::checkInput()
{
  if(deals.empty())
    {
      return void();
    }

  std::string str = time_start->text().toStdString();
  std::stringstream strm;
  strm.str(str);
  std::tm tm = {};
  strm >> std::get_time(&tm, "%H:%M:%S");

  int day, month, year, hour, minut;
  double sec;
  MoneyAux::julianDateToDate(deals[0].deal_time, day, month, year, hour, minut,
                             sec);
  double db = MoneyAux::dateToJulianDay(
      day, month, year, tm.tm_hour, tm.tm_min, static_cast<double>(tm.tm_sec));

  tm = {};
  strm.clear();
  str = time_end->text().toStdString();
  strm.str(str);
  strm >> std::get_time(&tm, "%H:%M:%S");
  double de = MoneyAux::dateToJulianDay(
      day, month, year, tm.tm_hour, tm.tm_min, static_cast<double>(tm.tm_sec));
  if(de < db)
    {
      return void();
    }

  plot_v.clear();
  plot_v.reserve(deals.size());
  mpf_class t = 0.0, mt = 0.0, count = 1.0;
  for(auto it = deals.begin(); it != deals.end(); it++)
    {
      plot_v.push_back(*it);
      auto it_last = plot_v.rbegin();
      t += mpf_class(it->quantity);
      mt += mpf_class(it->money_volume);
      it_last->ppm = t / mt;
      it_last->ppm_price = mt / t;
      it_last->average_quan = t / count;
      it_last->average_money = mt / count;
      count += 1.0;
    }

  plot_v.erase(Algorithm::parallelRemoveIf(plot_v.begin(), plot_v.end(),
                                           [de, db](const Deal &el)
                                             {
                                               if(el.deal_time < db
                                                  || el.deal_time > de)
                                                 {
                                                   return true;
                                                 }
                                               return false;
                                             }),
               plot_v.end());
  if(plot_v.empty())
    {
      return void();
    }

  DiagramWindow *diagram_window = nullptr;
  switch(mode->currentData().value<Mode>())
    {
    case Mode::Prices:
      {
        diagram_window = new DiagramWindow(
            std::bind(&DealsGraphSelectionWindow::plotPrices, this,
                      std::placeholders::_1),
            this);
        break;
      }
    case Mode::PPM:
      {
        diagram_window
            = new DiagramWindow(std::bind(&DealsGraphSelectionWindow::plotPPM,
                                          this, std::placeholders::_1),
                                this);
        break;
      }
    case Mode::Turnover:
      {
        diagram_window = new DiagramWindow(
            std::bind(&DealsGraphSelectionWindow::plotTurnover, this,
                      std::placeholders::_1),
            this);
        break;
      }
    case Mode::MoneyTurnover:
      {
        diagram_window = new DiagramWindow(
            std::bind(&DealsGraphSelectionWindow::plotMoneyTurnover, this,
                      std::placeholders::_1),
            this);
        break;
      }
    default:
      break;
    }
  if(diagram_window != nullptr)
    {
      connect(
          diagram_window, &DiagramWindow::signalDataRequest, this,
          [diagram_window, this](const QPointF &global, const mglPoint &pos)
            {
              if(pos.x < 0)
                {
                  return void();
                }
              size_t index = static_cast<size_t>(pos.x - 1.0);
              if(index >= plot_v.size())
                {
                  return void();
                }
              GraphDataWindow *gdw = new GraphDataWindow(diagram_window);
              gdw->createWindow(plot_v[index]);
              gdw->show();

              gdw->move(diagram_window->mapFromGlobal(global).toPoint());
            });
      diagram_window->showMaximized();
    }
}

void
DealsGraphSelectionWindow::plotPrices(mglGraph *gr)
{
  double min = std::numeric_limits<double>::max(), max = 0.0;
  std::vector<double> x;
  std::vector<double> price;
  std::vector<double> ppm_price;
  for(size_t i = 0; i < plot_v.size(); i++)
    {
      x.push_back(static_cast<double>(i) + 1.0);
      double val = plot_v[i].price;
      price.push_back(val);
      if(val < min)
        {
          min = val;
        }
      if(val > max)
        {
          max = val;
        }
      val = plot_v[i].ppm_price.get_d();
      ppm_price.push_back(val);
      if(val < min)
        {
          min = val;
        }
      if(val > max)
        {
          max = val;
        }
    }

  gr->SetQuality(3);
  gr->Clf("{xffffffff}");

  std::string str
      = MoneyAux::convertToEncodign(tr("Prices").toStdString(), "UTF-8", "");
  gr->Title(str.c_str(), "", 4);

  gr->SetRange('y', min, max);
  gr->SetRange('x', 1.0, *x.rbegin());

  gr->SetTicks('y', (max - min) / 3.0);
  gr->SetTicks('x', *x.rbegin() / 4.0);
  gr->SetOriginTick(false);

  gr->Axis("xy0", "k");
  gr->Grid("xy", "k");

  mglData x_data(x);
  mglData y_data(price);
  gr->Plot(x_data, y_data, "g");

  y_data = mglData(ppm_price);
  gr->Plot(x_data, y_data, "r");

  str = MoneyAux::convertToEncodign(tr("Deal number").toStdString(), "UTF-8",
                                    "");
  gr->Label('x', str.c_str(), 0);
  str = MoneyAux::convertToEncodign(currency, "UTF-8", "");
  gr->Label('y', str.c_str(), 0);

  gr->SetFontSize(3.0);

  str = MoneyAux::convertToEncodign(tr("Price").toStdString(), "UTF-8", "");
  gr->AddLegend(str.c_str(), "g");
  str = MoneyAux::convertToEncodign(tr("PPM calculated price").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "r");
  gr->Legend(1.41, 1.3, "#-");
}

void
DealsGraphSelectionWindow::plotPPM(mglGraph *gr)
{
  double min = std::numeric_limits<double>::max(), max = 0.0;
  std::vector<double> x;
  std::vector<double> ppm;
  std::vector<double> ppm_deal;
  for(size_t i = 0; i < plot_v.size(); i++)
    {
      x.push_back(static_cast<double>(i) + 1.0);
      double val = plot_v[i].ppm.get_d();
      ppm.push_back(val);
      if(val < min)
        {
          min = val;
        }
      if(val > max)
        {
          max = val;
        }

      val = static_cast<double>(plot_v[i].quantity) / plot_v[i].money_volume;
      ppm_deal.push_back(val);
      if(val < min)
        {
          min = val;
        }
      if(val > max)
        {
          max = val;
        }
    }

  gr->SetQuality(3);
  gr->Clf("{xffffffff}");

  std::string str = MoneyAux::convertToEncodign(
      tr("Purchasing power of money").toStdString(), "UTF-8", "");
  gr->Title(str.c_str(), "", 4);

  gr->SetRange('y', min, max);
  gr->SetRange('x', 1.0, *x.rbegin());

  gr->SetTicks('y', (max - min) / 3.0);
  gr->SetTicks('x', *x.rbegin() / 4.0);
  gr->SetOriginTick(false);

  gr->Axis("x0", "k");
  gr->Axis("y", "k");
  gr->Grid("xy", "k");

  mglData x_data(x);
  mglData y_data(ppm_deal);
  gr->Plot(x_data, y_data, "g");

  y_data = mglData(ppm);
  gr->Plot(x_data, y_data, "r");

  str = MoneyAux::convertToEncodign(tr("Deal number").toStdString(), "UTF-8",
                                    "");
  gr->Label('x', str.c_str(), 0);
  str = tr("Shares").toStdString();
  str += "/" + currency;
  str = MoneyAux::convertToEncodign(str, "UTF-8", "");
  gr->Label('y', str.c_str(), 0);

  gr->SetFontSize(3.0);

  str = MoneyAux::convertToEncodign(tr("Deal PPM").toStdString(), "UTF-8", "");
  gr->AddLegend(str.c_str(), "g");
  str = MoneyAux::convertToEncodign(tr("PPM").toStdString(), "UTF-8", "");
  gr->AddLegend(str.c_str(), "r");
  gr->Legend(1.41, 1.3, "#-");
}

void
DealsGraphSelectionWindow::plotTurnover(mglGraph *gr)
{
  double min = std::numeric_limits<double>::max(), max = 0.0;
  std::vector<double> x;
  std::vector<double> turnover;
  std::vector<double> average_turnover;
  for(size_t i = 0; i < plot_v.size(); i++)
    {
      x.push_back(static_cast<double>(i) + 1.0);
      double val = static_cast<double>(plot_v[i].quantity);
      turnover.push_back(val);
      if(val < min)
        {
          min = val;
        }
      if(val > max)
        {
          max = val;
        }

      val = plot_v[i].average_quan.get_d();
      average_turnover.push_back(val);
      if(val < min)
        {
          min = val;
        }
      if(val > max)
        {
          max = val;
        }
    }

  gr->SetQuality(3);
  gr->Clf("{xffffffff}");

  std::string str
      = MoneyAux::convertToEncodign(tr("Turnover").toStdString(), "UTF-8", "");
  gr->Title(str.c_str(), "", 4);

  gr->SetRange('y', min, max);
  gr->SetRange('x', 1.0, *x.rbegin());

  gr->SetTicks('y', (max - min) / 3.0);
  gr->SetTicks('x', *x.rbegin() / 4.0);
  gr->SetOriginTick(false);

  gr->Axis("x0", "k");
  gr->Axis("y", "k");
  gr->Grid("xy", "k");

  mglData x_data(x);
  mglData y_data(turnover);
  gr->Plot(x_data, y_data, "g");

  y_data = mglData(average_turnover);
  gr->Plot(x_data, y_data, "r");

  str = MoneyAux::convertToEncodign(tr("Deal number").toStdString(), "UTF-8",
                                    "");
  gr->Label('x', str.c_str(), 0);
  str = tr("Shares").toStdString();
  str = MoneyAux::convertToEncodign(str, "UTF-8", "");
  gr->Label('y', str.c_str(), 0);

  gr->SetFontSize(3.0);

  str = MoneyAux::convertToEncodign(tr("Turnover").toStdString(), "UTF-8", "");
  gr->AddLegend(str.c_str(), "g");
  str = MoneyAux::convertToEncodign(tr("Average turnover").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "r");
  gr->Legend(1.41, 1.3, "#-");
}

void
DealsGraphSelectionWindow::plotMoneyTurnover(mglGraph *gr)
{
  double min = std::numeric_limits<double>::max(), max = 0.0;
  std::vector<double> x;
  std::vector<double> money_turnover;
  std::vector<double> average_money_turnover;
  for(size_t i = 0; i < plot_v.size(); i++)
    {
      x.push_back(static_cast<double>(i) + 1.0);
      double val = plot_v[i].money_volume;
      money_turnover.push_back(val);
      if(val < min)
        {
          min = val;
        }
      if(val > max)
        {
          max = val;
        }

      val = plot_v[i].average_money.get_d();
      average_money_turnover.push_back(val);
      if(val < min)
        {
          min = val;
        }
      if(val > max)
        {
          max = val;
        }
    }

  gr->SetQuality(3);
  gr->Clf("{xffffffff}");

  std::string str = MoneyAux::convertToEncodign(
      tr("Money turnover").toStdString(), "UTF-8", "");
  gr->Title(str.c_str(), "", 4);

  gr->SetRange('y', min, max);
  gr->SetRange('x', 1.0, *x.rbegin());

  gr->SetTicks('y', (max - min) / 3.0);
  gr->SetTicks('x', *x.rbegin() / 4.0);
  gr->SetOriginTick(false);

  gr->Axis("x0", "k");
  gr->Axis("y", "k");
  gr->Grid("xy", "k");

  mglData x_data(x);
  mglData y_data(money_turnover);
  gr->Plot(x_data, y_data, "g");

  y_data = mglData(average_money_turnover);
  gr->Plot(x_data, y_data, "r");

  str = MoneyAux::convertToEncodign(tr("Deal number").toStdString(), "UTF-8",
                                    "");
  gr->Label('x', str.c_str(), 0);
  str = MoneyAux::convertToEncodign(currency, "UTF-8", "");
  gr->Label('y', str.c_str(), 0);

  gr->SetFontSize(3.0);

  str = MoneyAux::convertToEncodign(tr("Money turnover").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "g");
  str = MoneyAux::convertToEncodign(tr("Average money turnover").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "r");
  gr->Legend(1.41, 1.3, "#-");
}
