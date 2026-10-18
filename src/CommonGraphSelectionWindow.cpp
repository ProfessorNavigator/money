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

#include <AddCoefWindow.h>
#include <Algorithm.h>
#include <CommonGraphSelectionWindow.h>
#include <DiagramWindow.h>
#include <FileReader.h>
#include <GraphDataWindow.h>
#include <MoneyAux.h>
#include <QCompleter>
#include <QDir>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QVBoxLayout>
#include <algorithm>
#include <filesystem>

CommonGraphSelectionWindow::CommonGraphSelectionWindow(QWidget *parent)
    : QWidget(parent)
{
  this->setWindowFlag(Qt::Window);
  this->setAttribute(Qt::WA_DeleteOnClose);
  this->setWindowTitle(tr("Common diagrams"));

  createWindow();
}

void
CommonGraphSelectionWindow::createWindow()
{
  QVBoxLayout *v_box = new QVBoxLayout;
  this->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setText(tr("Instrument") + ":");
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  instrument = new QComboBox;
  v_box->addWidget(instrument);

  std::string str = QDir::homePath().toStdString();
  std::filesystem::path p(std::u8string(str.begin(), str.end()));
  p /= u8".local";
  p /= u8"share";
  p /= u8"Money";
  p /= u8"securities.csv";

  FileReader fr;

  securities = fr.readSecurities(p);

  p = p.parent_path() / u8"foreign_securities.csv";
  std::vector<Security> sec = fr.readSecurities(p);
  std::copy(sec.begin(), sec.end(), std::back_inserter(securities));

  p = p.parent_path() / u8"shares_boards.csv";
  boards = fr.readBoards(p);

  std::vector<Board> brd;
  p = p.parent_path() / u8"foreignshares_boards.csv";
  brd = fr.readBoards(p);
  std::copy(brd.begin(), brd.end(), std::back_inserter(boards));

  std::string prev;
  for(auto it = securities.begin(); it != securities.end(); it++)
    {
      if(prev != it->id)
        {
          QString val = it->id.c_str();
          val += " ";
          val += it->name.c_str();
          instrument->addItem(val, QVariant::fromValue(it->id));
          prev = it->id;
        }
    }

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

  v_box->addWidget(dates(), 0, Qt::AlignCenter);
  v_box->addWidget(turnoverCoef());
  v_box->addWidget(moneyTurnoverCoef());

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  QPushButton *show = new QPushButton;
  show->setText(tr("Show diagram"));
  connect(show, &QPushButton::clicked, this,
          &CommonGraphSelectionWindow::checkInput);
  h_box->addWidget(show, 0, Qt::AlignCenter);

  QPushButton *cancel = new QPushButton;
  cancel->setText(tr("Cancel"));
  connect(cancel, &QPushButton::clicked, this,
          &CommonGraphSelectionWindow::close);
  h_box->addWidget(cancel, 0, Qt::AlignCenter);

  setBoards(0);

  connect(instrument, &QComboBox::currentIndexChanged, this,
          &CommonGraphSelectionWindow::setBoards);

  connect(board, &QComboBox::currentIndexChanged, this,
          &CommonGraphSelectionWindow::setDates);
}

void
CommonGraphSelectionWindow::setBoards(int instrument_index)
{
  std::string id = instrument->itemData(instrument_index).value<std::string>();

  std::vector<std::string> bids;
  for(auto it = securities.begin(); it != securities.end(); it++)
    {
      if(it->id == id)
        {
          bids.push_back(it->board_id);
        }
    }

  board->clear();
  int index = 0;
  for(size_t i = 0; i < bids.size(); i++)
    {
      std::string id = bids[i];
      auto it_brd = std::find_if(boards.begin(), boards.end(),
                                 [id](const Board &el)
                                   {
                                     return id == el.id;
                                   });
      QString val(id.c_str());
      if(it_brd != boards.end())
        {
          val += " ";
          val += it_brd->name.c_str();
        }
      board->addItem(val, QVariant::fromValue(id));
      if(id == "TQBR" || id == "FQBR")
        {
          index = static_cast<int>(i);
        }
    }
  board->setCurrentIndex(index);
  setDates(index);
}

void
CommonGraphSelectionWindow::setDates(int board_index)
{
  std::string brd_id = board->itemData(board_index).value<std::string>();
  std::string instr_id = instrument->currentData().value<std::string>();
  auto it = Algorithm::parallelFindIf(securities.begin(), securities.end(),
                                      [instr_id, brd_id](const Security &el)
                                        {
                                          if(el.id == instr_id
                                             && el.board_id == brd_id)
                                            {
                                              return true;
                                            }
                                          return false;
                                        });
  if(it == securities.end())
    {
      date_begin->setText("23.03.1997");
      time_t curtm = time(nullptr);
      std::tm tm = *localtime(&curtm);
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
      date_end->setText(val);
    }
  else
    {
      std::tm tm = {};
      std::stringstream strm;
      strm.str(it->history_from);
      strm >> std::get_time(&tm, "%Y-%m-%d");
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
      date_begin->setText(val);

      tm = {};
      strm.clear();
      strm.str(it->history_till);
      strm >> std::get_time(&tm, "%Y-%m-%d");
      val.clear();
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
      date_end->setText(val);
    }
}

QWidget *
CommonGraphSelectionWindow::dates()
{
  QWidget *result = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setContentsMargins(0, 0, 0, 0);
  result->setLayout(grid);

  QLabel *lab = new QLabel;
  lab->setText(tr("Begin date") + ":");
  grid->addWidget(lab, 0, 0, Qt::AlignLeft);

  date_begin = new QLineEdit;
  QString mask("00.00.0000");
  date_begin->setInputMask(mask);
  QFontMetrics fm = date_begin->fontMetrics();
  QRect rect = fm.boundingRect(mask);
  QMargins mg = date_begin->textMargins();
  date_begin->setFixedWidth(rect.width() + mg.left() + mg.right()
                            + 2 * fm.averageCharWidth());
  grid->addWidget(date_begin, 1, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(tr("End date") + ":");
  grid->addWidget(lab, 0, 1, Qt::AlignLeft);

  date_end = new QLineEdit;
  date_end->setInputMask(mask);
  fm = date_end->fontMetrics();
  rect = fm.boundingRect(mask);
  mg = date_end->textMargins();
  date_end->setFixedWidth(rect.width() + mg.left() + mg.right()
                          + 2 * fm.averageCharWidth());
  grid->addWidget(date_end, 1, 1, Qt::AlignLeft);

  return result;
}

QWidget *
CommonGraphSelectionWindow::turnoverCoef()
{
  QFrame *result = new QFrame;
  result->setFrameShape(QFrame::Box);

  QVBoxLayout *v_box = new QVBoxLayout;
  result->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setText(tr("Turnover coefficients"));
  v_box->addWidget(lab, 0, Qt::AlignCenter);

  turnover_coef_model = new CoeffModel(this);

  QTableView *view = new QTableView;
  QAbstractItemModel *model = view->model();
  view->setModel(turnover_coef_model);
  delete model;
  view->setSizeAdjustPolicy(QTableView::AdjustToContents);
  QHeaderView *hh = view->horizontalHeader();
  for(int i = 0; i < hh->count(); i++)
    {
      hh->setSectionResizeMode(i, QHeaderView::Stretch);
    }
  v_box->addWidget(view);

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  QPushButton *add = new QPushButton;
  add->setText(tr("Add"));
  connect(add, &QPushButton::clicked, this,
          [this]
            {
              AddCoefWindow *acw = new AddCoefWindow(this);
              connect(acw, &AddCoefWindow::signalCoef, turnover_coef_model,
                      &CoeffModel::addCoef);
              acw->show();
            });
  h_box->addWidget(add, 0, Qt::AlignCenter);

  QPushButton *remove_selected = new QPushButton;
  remove_selected->setText(tr("Remove selected"));
  connect(remove_selected, &QPushButton::clicked, view,
          [this, view]
            {
              QModelIndex index = view->currentIndex();
              turnover_coef_model->removeCoef(index);
              view->setCurrentIndex(QModelIndex());
            });
  h_box->addWidget(remove_selected, 0, Qt::AlignCenter);

  return result;
}

QWidget *
CommonGraphSelectionWindow::moneyTurnoverCoef()
{
  QFrame *result = new QFrame;
  result->setFrameShape(QFrame::Box);

  QVBoxLayout *v_box = new QVBoxLayout;
  result->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setText(tr("Money turnover coefficients"));
  v_box->addWidget(lab, 0, Qt::AlignCenter);

  money_turnover_coef_model = new CoeffModel(this);
  double date_beg = MoneyAux::dateToJulianDay(23, 3, 1997, 0, 0, 0.0);
  double date_end = MoneyAux::dateToJulianDay(31, 12, 1997, 0, 0, 0.0);
  double coef = 0.001;
  money_turnover_coef_model->addCoef(date_beg, date_end, coef);

  QTableView *view = new QTableView;
  QAbstractItemModel *model = view->model();
  view->setModel(money_turnover_coef_model);
  delete model;
  view->setSizeAdjustPolicy(QTableView::AdjustToContents);
  QHeaderView *hh = view->horizontalHeader();
  for(int i = 0; i < hh->count(); i++)
    {
      hh->setSectionResizeMode(i, QHeaderView::Stretch);
    }
  v_box->addWidget(view);

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  QPushButton *add = new QPushButton;
  add->setText(tr("Add"));
  connect(add, &QPushButton::clicked, this,
          [this]
            {
              AddCoefWindow *acw = new AddCoefWindow(this);
              connect(acw, &AddCoefWindow::signalCoef,
                      money_turnover_coef_model, &CoeffModel::addCoef);
              acw->show();
            });
  h_box->addWidget(add, 0, Qt::AlignCenter);

  QPushButton *remove_selected = new QPushButton;
  remove_selected->setText(tr("Remove selected"));
  connect(remove_selected, &QPushButton::clicked, view,
          [this, view]
            {
              QModelIndex index = view->currentIndex();
              money_turnover_coef_model->removeCoef(index);
              view->setCurrentIndex(QModelIndex());
            });
  h_box->addWidget(remove_selected, 0, Qt::AlignCenter);

  QPushButton *reset_to_default = new QPushButton;
  reset_to_default->setText(tr("Reset to default"));
  connect(reset_to_default, &QPushButton::clicked, this,
          [this, date_beg, date_end, coef, view]
            {
              money_turnover_coef_model->removeAll();
              view->setCurrentIndex(QModelIndex());
              money_turnover_coef_model->addCoef(date_beg, date_end, coef);
            });
  h_box->addWidget(reset_to_default, 0, Qt::AlignCenter);

  return result;
}

void
CommonGraphSelectionWindow::checkInput()
{
  std::stringstream strm;
  strm.str(date_begin->text().toStdString());
  std::tm tm = {};
  strm >> std::get_time(&tm, "%d.%m.%Y");
  double db = MoneyAux::dateToJulianDay(tm.tm_mday, tm.tm_mon + 1,
                                        tm.tm_year + 1900, 0, 0, 0.0);

  strm.clear();
  strm.str(date_end->text().toStdString());
  tm = {};
  strm >> std::get_time(&tm, "%d.%m.%Y");
  double de = MoneyAux::dateToJulianDay(tm.tm_mday, tm.tm_mon + 1,
                                        tm.tm_year + 1900, 0, 0, 0.0);

  if(db >= de)
    {
      return void();
    }

  std::string instr_id = instrument->currentData().value<std::string>();
  std::u8string u8str(instr_id.begin(), instr_id.end());
  u8str += u8".csv";

  std::string str = QDir::homePath().toStdString();
  std::filesystem::path p(std::u8string(str.begin(), str.end()));
  p /= u8".local";
  p /= u8"share";
  p /= u8"Money";
  p /= u8"Data";
  p /= u8"All";
  p /= u8str;

  FileReader fr;
  values = fr.readAll(p);

  std::string brd_id = board->currentData().value<std::string>();
  values.erase(Algorithm::parallelRemoveIf(values.begin(), values.end(),
                                           [brd_id](const TradeDay &el)
                                             {
                                               if(el.board_id != brd_id
                                                  || el.turnover == 0)
                                                 {
                                                   return true;
                                                 }
                                               return false;
                                             }),
               values.end());

  normalized.clear();

  money_coef_v = money_turnover_coef_model->getCoef();
  turnover_coef_v = turnover_coef_model->getCoef();

  normalized.reserve(values.size());

  mpf_class t_norm = 0.0, mt_norm = 0.0;
  mpf_class t = 0.0, mt = 0.0;
  mpf_class count = 1.0;
  for(auto it = values.begin(); it != values.end(); it++)
    {
      normalized.push_back(*it);
      t += mpf_class(static_cast<double>(it->turnover));
      mt += mpf_class(it->money_turnover);
      it->average_turnover = t / count;
      it->average_money_turnover = mt / count;
      auto it_last = normalized.rbegin();
      double date = it_last->date;
      auto it_cv = std::find_if(
          money_coef_v.begin(), money_coef_v.end(),
          [date](const std::tuple<double, double, double> &el)
            {
              if(date >= std::get<0>(el) && date <= std::get<1>(el))
                {
                  return true;
                }
              return false;
            });
      if(it_cv != money_coef_v.end())
        {
          it_last->first_price *= std::get<2>(*it_cv);
          it_last->last_price *= std::get<2>(*it_cv);
          it_last->wa_price *= std::get<2>(*it_cv);
          it_last->money_turnover *= std::get<2>(*it_cv);
        }

      it_cv = std::find_if(turnover_coef_v.begin(), turnover_coef_v.end(),
                           [date](const std::tuple<double, double, double> &el)
                             {
                               if(date >= std::get<0>(el)
                                  && date <= std::get<1>(el))
                                 {
                                   return true;
                                 }
                               return false;
                             });
      if(it_cv != turnover_coef_v.end())
        {
          it_last->turnover *= std::get<2>(*it_cv);
        }
      t_norm += mpf_class(static_cast<double>(it_last->turnover));
      mt_norm += mpf_class(it_last->money_turnover);
      it_last->ppm = t_norm / mt_norm;
      it_last->daily_ppm = it_last->turnover / it_last->money_turnover;
      it_last->ppm_price = mt_norm / t_norm;
      it_last->average_turnover = t_norm / count;
      it_last->average_money_turnover = mt_norm / count;
      count += 1.0;
    }

  values.erase(Algorithm::parallelRemoveIf(values.begin(), values.end(),
                                           [db, de](const TradeDay &el)
                                             {
                                               if(el.date < db || el.date > de)
                                                 {
                                                   return true;
                                                 }
                                               return false;
                                             }),
               values.end());

  normalized.erase(
      Algorithm::parallelRemoveIf(normalized.begin(), normalized.end(),
                                  [db, de](const TradeDay &el)
                                    {
                                      if(el.date < db || el.date > de)
                                        {
                                          return true;
                                        }
                                      return false;
                                    }),
      normalized.end());

  if(values.empty() || normalized.empty())
    {
      return void();
    }

  DiagramWindow *diagram_window = nullptr;
  switch(mode->currentData().value<Mode>())
    {
    case Mode::Prices:
      {
        diagram_window = new DiagramWindow(
            std::bind(&CommonGraphSelectionWindow::buildPricesGrpah, this,
                      std::placeholders::_1),
            this);
        break;
      }
    case Mode::PPM:
      {
        diagram_window = new DiagramWindow(
            std::bind(&CommonGraphSelectionWindow::buildPPMGrpah, this,
                      std::placeholders::_1),
            this);
        break;
      }
    case Mode::Turnover:
      {
        diagram_window = new DiagramWindow(
            std::bind(&CommonGraphSelectionWindow::buildTurnoverGrpah, this,
                      std::placeholders::_1),
            this);
        break;
      }
    case Mode::MoneyTurnover:
      {
        diagram_window = new DiagramWindow(
            std::bind(&CommonGraphSelectionWindow::buildMoneyTurnoverGrpah,
                      this, std::placeholders::_1),
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
              size_t index = static_cast<size_t>(pos.x);
              if(index >= values.size() || index >= normalized.size())
                {
                  return void();
                }
              GraphDataWindow *gdw = new GraphDataWindow(diagram_window);
              gdw->createWindow(values[index], normalized[index]);
              gdw->show();

              gdw->move(diagram_window->mapFromGlobal(global).toPoint());
            });
      diagram_window->showMaximized();
    }
}

void
CommonGraphSelectionWindow::buildPricesGrpah(mglGraph *gr)
{
  double min = std::numeric_limits<double>::max(), max = 0.0;
  std::vector<int> x;
  std::vector<double> first_price;
  std::vector<double> last_price;
  std::vector<double> wa_price;
  std::vector<double> ppm_price;
  for(size_t i = 0; i < normalized.size(); i++)
    {
      x.push_back(static_cast<int>(i));

      first_price.push_back(normalized[i].first_price);
      if(normalized[i].first_price < min)
        {
          min = normalized[i].first_price;
        }
      if(normalized[i].first_price > max)
        {
          max = normalized[i].first_price;
        }
      last_price.push_back(normalized[i].last_price);
      if(normalized[i].last_price < min)
        {
          min = normalized[i].last_price;
        }
      if(normalized[i].last_price > max)
        {
          max = normalized[i].last_price;
        }
      wa_price.push_back(normalized[i].wa_price);
      if(normalized[i].wa_price < min)
        {
          min = normalized[i].wa_price;
        }
      if(normalized[i].wa_price > max)
        {
          max = normalized[i].wa_price;
        }
      ppm_price.push_back(normalized[i].ppm_price.get_d());
      if(normalized[i].ppm_price.get_d() < min)
        {
          min = normalized[i].ppm_price.get_d();
        }
      if(normalized[i].ppm_price.get_d() > max)
        {
          max = normalized[i].ppm_price.get_d();
        }
    }

  gr->SetQuality(3);
  gr->Clf("{xffffffff}");

  std::string str
      = MoneyAux::convertToEncodign(tr("Prices").toStdString(), "UTF-8", "");
  gr->Title(str.c_str(), "", 4);

  gr->SetRange('y', min, max);
  gr->SetRange('x', 0.0, static_cast<double>(x.size() - 1));  

  gr->SetTicks('y', (max - min) / 3.0);
  gr->SetTicks('x', static_cast<double>(x.size() - 1) / 4.0);
  gr->SetOriginTick(false);

  gr->Axis("xy0", "k");
  gr->Grid("xy", "k");

  mglData x_data(x);
  mglData y_data(first_price);
  gr->Plot(x_data, y_data, "{x4a62f4}");

  y_data = mglData(last_price);
  gr->Plot(x_data, y_data, "{x109146}");

  y_data = mglData(wa_price);
  gr->Plot(x_data, y_data, "{xeb162c}");

  y_data = mglData(ppm_price);
  gr->Plot(x_data, y_data, "k");

  for(auto it = money_coef_v.begin(); it != money_coef_v.end(); it++)
    {
      double b = std::get<0>(*it);
      double e = std::get<1>(*it);
      if(b > normalized[0].date && b < normalized.rbegin()->date)
        {
          auto it_cpv = std::find_if(normalized.begin(), normalized.end(),
                                     [b](const TradeDay &el)
                                       {
                                         return b > el.date;
                                       });
          if(it_cpv != normalized.end())
            {
              double dist = static_cast<double>(
                  std::distance(normalized.begin(), it_cpv));
              x.clear();
              x.push_back(dist);
              x.push_back(dist);
              x_data = mglData(x);
              std::vector<double> y;
              y.push_back(min);
              y.push_back(max);
              y_data = mglData(y);
              gr->Plot(x_data, y_data, "{x304bb9}");
            }
        }
      if(e > normalized[0].date && e < normalized.rbegin()->date)
        {
          auto it_cpv = std::find_if(normalized.begin(), normalized.end(),
                                     [e](const TradeDay &el)
                                       {
                                         return e < el.date;
                                       });
          if(it_cpv != normalized.end())
            {
              int dist = static_cast<int>(
                  std::distance(normalized.begin(), it_cpv));
              x.clear();
              x.push_back(dist);
              x.push_back(dist);
              x_data = mglData(x);
              std::vector<double> y;
              y.push_back(min);
              y.push_back(max);
              y_data = mglData(y);
              gr->Plot(x_data, y_data, "{xa530b9}");
            }
        }
    }

  str = MoneyAux::convertToEncodign(tr("Days").toStdString(), "UTF-8", "");
  gr->Label('x', str.c_str(), 0);
  if(normalized[0].currency_id == "SUR" || normalized[0].currency_id.empty())
    {
      str = MoneyAux::convertToEncodign("RUB", "UTF-8", "");
    }
  else
    {
      str = MoneyAux::convertToEncodign(normalized[0].currency_id, "UTF-8",
                                        "");
    }
  gr->Label('y', str.c_str(), 0);

  gr->SetFontSize(3.0);

  str = MoneyAux::convertToEncodign(tr("First price").toStdString(), "UTF-8",
                                    "");
  gr->AddLegend(str.c_str(), "{x4a62f4}");
  str = MoneyAux::convertToEncodign(tr("Last price").toStdString(), "UTF-8",
                                    "");
  gr->AddLegend(str.c_str(), "{x109146}");
  str = MoneyAux::convertToEncodign(tr("Weighted price").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "{xeb162c}");
  str = MoneyAux::convertToEncodign(tr("PPM calculated price").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "k");
  gr->Legend(1.41, 1.3, "#-");
}

void
CommonGraphSelectionWindow::buildPPMGrpah(mglGraph *gr)
{
  mpf_class min = std::numeric_limits<double>::max(), max = 0.0;
  std::vector<double> x;
  std::vector<double> ppm;
  std::vector<double> daily_ppm;
  for(size_t i = 0; i < normalized.size(); i++)
    {
      x.push_back(static_cast<double>(i));

      ppm.push_back(normalized[i].ppm.get_d());
      if(normalized[i].ppm < min)
        {
          min = normalized[i].ppm;
        }
      if(normalized[i].ppm > max)
        {
          max = normalized[i].ppm;
        }
      daily_ppm.push_back(normalized[i].daily_ppm);
      if(normalized[i].daily_ppm < min.get_d())
        {
          min = normalized[i].daily_ppm;
        }
      if(normalized[i].daily_ppm > max.get_d())
        {
          max = normalized[i].daily_ppm;
        }
    }

  gr->SetQuality(3);
  gr->Clf("{xffffffff}");

  std::string str = MoneyAux::convertToEncodign(
      tr("Purchasing power of money").toStdString(), "UTF-8", "");
  gr->Title(str.c_str(), "", 4);

  gr->SetRange('y', min.get_d(), max.get_d());
  gr->SetRange('x', 0.0, static_cast<double>(x.size() - 1));  

  gr->SetTicks('y', (max.get_d() - min.get_d()) / 3.0);
  gr->SetTicks('x', static_cast<double>(x.size() - 1) / 4.0);
  gr->SetOriginTick(false);

  if(max - min >= 2)
    {
      gr->Axis("xy0", "k");
    }
  else
    {
      mpf_class diff = max - min;
      int count = 1;
      mpf_class coef = 0.1;
      while(diff < coef)
        {
          coef /= 10;
          count++;
        }

      std::stringstream strm;
      strm.imbue(std::locale("C"));
      strm << count;
      std::string str = "yf" + strm.str();
      gr->Axis("x0", "k");
      gr->Axis(str.c_str(), "k");
    }
  gr->Grid("xy", "k");

  mglData x_data(x);
  mglData y_data(ppm);
  gr->Plot(x_data, y_data, "r");

  y_data = mglData(daily_ppm);
  gr->Plot(x_data, y_data, "g");

  for(auto it = money_coef_v.begin(); it != money_coef_v.end(); it++)
    {
      double b = std::get<0>(*it);
      double e = std::get<1>(*it);
      if(b > normalized[0].date && b < normalized.rbegin()->date)
        {
          auto it_cpv = std::find_if(normalized.begin(), normalized.end(),
                                     [b](const TradeDay &el)
                                       {
                                         return b > el.date;
                                       });
          if(it_cpv != normalized.end())
            {
              double dist = static_cast<double>(
                  std::distance(normalized.begin(), it_cpv));
              x.clear();
              x.push_back(dist);
              x.push_back(dist);
              x_data = mglData(x);
              std::vector<double> y;
              y.push_back(min.get_d());
              y.push_back(max.get_d());
              y_data = mglData(y);
              gr->Plot(x_data, y_data, "{x304bb9}");
            }
        }
      if(e > normalized[0].date && e < normalized.rbegin()->date)
        {
          auto it_cpv = std::find_if(normalized.begin(), normalized.end(),
                                     [e](const TradeDay &el)
                                       {
                                         return e < el.date;
                                       });
          if(it_cpv != normalized.end())
            {
              int dist = static_cast<int>(
                  std::distance(normalized.begin(), it_cpv));
              x.clear();
              x.push_back(dist);
              x.push_back(dist);
              x_data = mglData(x);
              std::vector<double> y;
              y.push_back(min.get_d());
              y.push_back(max.get_d());
              y_data = mglData(y);
              gr->Plot(x_data, y_data, "{xa530b9}");
            }
        }
    }

  for(auto it = turnover_coef_v.begin(); it != turnover_coef_v.end(); it++)
    {
      double b = std::get<0>(*it);
      double e = std::get<1>(*it);
      if(b > normalized[0].date && b < normalized.rbegin()->date)
        {
          auto it_cpv = std::find_if(normalized.begin(), normalized.end(),
                                     [b](const TradeDay &el)
                                       {
                                         return b > el.date;
                                       });
          if(it_cpv != normalized.end())
            {
              double dist = static_cast<double>(
                  std::distance(normalized.begin(), it_cpv));
              x.clear();
              x.push_back(dist);
              x.push_back(dist);
              x_data = mglData(x);
              std::vector<double> y;
              y.push_back(min.get_d());
              y.push_back(max.get_d());
              y_data = mglData(y);
              gr->Plot(x_data, y_data, "{x304bb9}");
            }
        }
      if(e > normalized[0].date && e < normalized.rbegin()->date)
        {
          auto it_cpv = std::find_if(normalized.begin(), normalized.end(),
                                     [e](const TradeDay &el)
                                       {
                                         return e < el.date;
                                       });
          if(it_cpv != normalized.end())
            {
              int dist = static_cast<int>(
                  std::distance(normalized.begin(), it_cpv));
              x.clear();
              x.push_back(dist);
              x.push_back(dist);
              x_data = mglData(x);
              std::vector<double> y;
              y.push_back(min.get_d());
              y.push_back(max.get_d());
              y_data = mglData(y);
              gr->Plot(x_data, y_data, "{xa530b9}");
            }
        }
    }

  str = MoneyAux::convertToEncodign(tr("Days").toStdString(), "UTF-8", "");
  gr->Label('x', str.c_str(), 0);
  if(normalized[0].currency_id == "SUR" || normalized[0].currency_id.empty())
    {
      str = MoneyAux::convertToEncodign(tr("Shares/Ruble").toStdString(),
                                        "UTF-8", "");
    }
  else
    {
      str = MoneyAux::convertToEncodign(tr("Shares").toStdString(), "UTF-8",
                                        "");
      str += "/" + normalized[0].currency_id;
    }
  gr->Label('y', str.c_str(), 0);

  gr->SetFontSize(3.0);

  str = MoneyAux::convertToEncodign(tr("PPM").toStdString(), "UTF-8", "");
  gr->AddLegend(str.c_str(), "r");
  str = MoneyAux::convertToEncodign(tr("Daily PPM").toStdString(), "UTF-8",
                                    "");
  gr->AddLegend(str.c_str(), "g");
  gr->Legend(1.41, 1.3, "#-");
}

void
CommonGraphSelectionWindow::buildTurnoverGrpah(mglGraph *gr)
{
  double min = std::numeric_limits<double>::max(), max = 0.0;
  std::vector<double> x;
  std::vector<double> turnover;
  std::vector<double> average_turnover;
  for(size_t i = 0; i < normalized.size(); i++)
    {
      x.push_back(static_cast<double>(i));
      double t = static_cast<double>(normalized[i].turnover);
      turnover.push_back(t);
      if(t < min)
        {
          min = t;
        }
      if(t > max)
        {
          max = t;
        }
      t = normalized[i].average_turnover.get_d();
      average_turnover.push_back(t);
      if(t < min)
        {
          min = t;
        }
      if(t > max)
        {
          max = t;
        }
    }

  gr->SetQuality(3);
  gr->Clf("{xffffffff}");

  std::string str
      = MoneyAux::convertToEncodign(tr("Turnover").toStdString(), "UTF-8", "");
  gr->Title(str.c_str(), "", 4);

  gr->SetRange('y', min, max);
  gr->SetRange('x', 0.0, static_cast<double>(x.size() - 1));  

  gr->SetTicks('y', (max - min) / 3.0);
  gr->SetTicks('x', static_cast<double>(x.size() - 1) / 4.0);
  gr->SetOriginTick(false);

  gr->Axis("xy0", "k");
  gr->Grid("xy", "k");

  mglData x_data(x);
  mglData y_data(average_turnover);
  gr->Plot(x_data, y_data, "r");

  y_data = mglData(turnover);
  gr->Plot(x_data, y_data, "g");

  for(auto it = turnover_coef_v.begin(); it != turnover_coef_v.end(); it++)
    {
      double b = std::get<0>(*it);
      double e = std::get<1>(*it);
      if(b > normalized[0].date && b < normalized.rbegin()->date)
        {
          auto it_cpv = std::find_if(normalized.begin(), normalized.end(),
                                     [b](const TradeDay &el)
                                       {
                                         return b > el.date;
                                       });
          if(it_cpv != normalized.end())
            {
              double dist = static_cast<double>(
                  std::distance(normalized.begin(), it_cpv));
              x.clear();
              x.push_back(dist);
              x.push_back(dist);
              x_data = mglData(x);
              std::vector<double> y;
              y.push_back(min);
              y.push_back(max);
              y_data = mglData(y);
              gr->Plot(x_data, y_data, "{x304bb9}");
            }
        }
      if(e > normalized[0].date && e < normalized.rbegin()->date)
        {
          auto it_cpv = std::find_if(normalized.begin(), normalized.end(),
                                     [e](const TradeDay &el)
                                       {
                                         return e < el.date;
                                       });
          if(it_cpv != normalized.end())
            {
              int dist = static_cast<int>(
                  std::distance(normalized.begin(), it_cpv));
              x.clear();
              x.push_back(dist);
              x.push_back(dist);
              x_data = mglData(x);
              std::vector<double> y;
              y.push_back(min);
              y.push_back(max);
              y_data = mglData(y);
              gr->Plot(x_data, y_data, "{xa530b9}");
            }
        }
    }

  str = MoneyAux::convertToEncodign(tr("Days").toStdString(), "UTF-8", "");
  gr->Label('x', str.c_str(), 0);
  str = MoneyAux::convertToEncodign(tr("Shares").toStdString(), "UTF-8", "");
  gr->Label('y', str.c_str(), 0);

  gr->SetFontSize(3.0);

  str = MoneyAux::convertToEncodign(tr("Average turnover").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "r");
  str = MoneyAux::convertToEncodign(tr("Turnover").toStdString(), "UTF-8", "");
  gr->AddLegend(str.c_str(), "g");
  gr->Legend(1.41, 1.3, "#-");
}

void
CommonGraphSelectionWindow::buildMoneyTurnoverGrpah(mglGraph *gr)
{
  double min = std::numeric_limits<double>::max(), max = 0.0;
  std::vector<double> x;
  std::vector<double> money_turnover;
  std::vector<double> average_money_turnover;
  for(size_t i = 0; i < normalized.size(); i++)
    {
      x.push_back(static_cast<double>(i));

      double mt = normalized[i].money_turnover;
      money_turnover.push_back(mt);
      if(mt < min)
        {
          min = mt;
        }
      if(mt > max)
        {
          max = mt;
        }

      mt = normalized[i].average_money_turnover.get_d();
      average_money_turnover.push_back(mt);
      if(mt < min)
        {
          min = mt;
        }
      if(mt > max)
        {
          max = mt;
        }
    }

  gr->SetQuality(3);
  gr->Clf("{xffffffff}");

  std::string str = MoneyAux::convertToEncodign(
      tr("Money turnover").toStdString(), "UTF-8", "");
  gr->Title(str.c_str(), "", 4);

  gr->SetRange('y', min, max);
  gr->SetRange('x', 0.0, static_cast<double>(x.size() - 1));  

  gr->SetTicks('y', (max - min) / 3.0);
  gr->SetTicks('x', static_cast<double>(x.size() - 1) / 4.0);
  gr->SetOriginTick(false);

  gr->Axis("xy0", "k");
  gr->Grid("xy", "k");

  mglData x_data(x);
  mglData y_data(average_money_turnover);
  gr->Plot(x_data, y_data, "r");

  y_data = mglData(money_turnover);
  gr->Plot(x_data, y_data, "g");

  for(auto it = money_coef_v.begin(); it != money_coef_v.end(); it++)
    {
      double b = std::get<0>(*it);
      double e = std::get<1>(*it);
      if(b > normalized[0].date && b < normalized.rbegin()->date)
        {
          auto it_cpv = std::find_if(normalized.begin(), normalized.end(),
                                     [b](const TradeDay &el)
                                       {
                                         return b > el.date;
                                       });
          if(it_cpv != normalized.end())
            {
              double dist = static_cast<double>(
                  std::distance(normalized.begin(), it_cpv));
              x.clear();
              x.push_back(dist);
              x.push_back(dist);
              x_data = mglData(x);
              std::vector<double> y;
              y.push_back(min);
              y.push_back(max);
              y_data = mglData(y);
              gr->Plot(x_data, y_data, "{x304bb9}");
            }
        }
      if(e > normalized[0].date && e < normalized.rbegin()->date)
        {
          auto it_cpv = std::find_if(normalized.begin(), normalized.end(),
                                     [e](const TradeDay &el)
                                       {
                                         return e < el.date;
                                       });
          if(it_cpv != normalized.end())
            {
              int dist = static_cast<int>(
                  std::distance(normalized.begin(), it_cpv));
              x.clear();
              x.push_back(dist);
              x.push_back(dist);
              x_data = mglData(x);
              std::vector<double> y;
              y.push_back(min);
              y.push_back(max);
              y_data = mglData(y);
              gr->Plot(x_data, y_data, "{xa530b9}");
            }
        }
    }

  str = MoneyAux::convertToEncodign(tr("Days").toStdString(), "UTF-8", "");
  gr->Label('x', str.c_str(), 0);
  if(normalized[0].currency_id == "SUR" || normalized[0].currency_id.empty())
    {
      str = MoneyAux::convertToEncodign("RUB", "UTF-8", "");
    }
  else
    {
      str = MoneyAux::convertToEncodign(normalized[0].currency_id, "UTF-8",
                                        "");
    }
  gr->Label('y', str.c_str(), 0);

  gr->SetFontSize(3.0);

  str = MoneyAux::convertToEncodign(tr("Average money turnover").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "r");
  str = MoneyAux::convertToEncodign(tr("Money turnover").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "g");
  gr->Legend(1.41, 1.3, "#-");
}
