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
#include <DiagramWindow.h>
#include <FileReader.h>
#include <GraphDataWindow.h>
#include <MoneyAux.h>
#include <PPMGraphSelectionWindow.h>
#include <QDir>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableView>
#include <ctime>
#include <filesystem>

PPMGraphSelectionWindow::PPMGraphSelectionWindow(QWidget *parent)
    : QWidget(parent)
{
  this->setWindowFlag(Qt::Window);
  this->setAttribute(Qt::WA_DeleteOnClose);
  this->setWindowTitle(tr("PPM diagram mode selection"));

  createWindow();
}

void
PPMGraphSelectionWindow::createWindow()
{
  QVBoxLayout *v_box = new QVBoxLayout;
  this->setLayout(v_box);

  std::string str = QDir::homePath().toStdString();
  std::filesystem::path p(std::u8string(str.begin(), str.end()));
  p /= u8".local";
  p /= u8"share";
  p /= u8"Money";
  p /= u8"Data";
  p /= u8"PPM.csv";

  if(!std::filesystem::exists(p))
    {
      errorDialog(v_box);
      return void();
    }

  FileReader fr;
  ppmv = fr.readPPM(p);
  if(ppmv.empty())
    {
      errorDialog(v_box);
      return void();
    }

  QLabel *lab = new QLabel;
  lab->setText(tr("Mode") + ":");
  v_box->addWidget(lab, 0, Qt::AlignCenter);

  mode = new QComboBox;
  mode->addItem(tr("PPM"), QVariant::fromValue(Mode::PPMMode));
  mode->addItem(tr("Turnover"), QVariant::fromValue(Mode::Turnover));
  mode->addItem(tr("Money turnover"),
                QVariant::fromValue(Mode::MoneyTurnover));
  mode->addItem(tr("Inflation"), QVariant::fromValue(Mode::Inflation));
  v_box->addWidget(mode);

  QGridLayout *grid = new QGridLayout;
  v_box->addLayout(grid);

  lab = new QLabel;
  lab->setText(tr("Begin date") + ":");
  grid->addWidget(lab, 0, 0, Qt::AlignLeft);

  date_begin = new QLineEdit;
  int day, month, year, hour, minut;
  double sec;
  MoneyAux::julianDateToDate(ppmv.begin()->date, day, month, year, hour, minut,
                             sec);
  QString qstr;
  if(day < 10)
    {
      qstr = "0";
    }
  qstr += QString::number(day);
  qstr += ".";
  if(month < 10)
    {
      qstr += "0";
    }
  qstr += QString::number(month);
  qstr += QString::number(year);
  date_begin->setText(qstr);
  QString mask("00.00.0000");
  date_begin->setInputMask(mask);
  QFontMetrics fm = date_begin->fontMetrics();
  QMargins mg = date_begin->textMargins();
  QRect rect = fm.boundingRect(mask);
  date_begin->setFixedWidth(rect.width() + mg.right() + mg.left()
                            + fm.averageCharWidth() * 2);
  grid->addWidget(date_begin, 1, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(tr("End date") + ":");
  grid->addWidget(lab, 0, 1, Qt::AlignLeft);

  date_end = new QLineEdit;
  MoneyAux::julianDateToDate(ppmv.rbegin()->date, day, month, year, hour,
                             minut, sec);
  qstr.clear();
  if(day < 10)
    {
      qstr = "0";
    }
  qstr += QString::number(day);
  qstr += ".";
  if(month < 10)
    {
      qstr += "0";
    }
  qstr += QString::number(month);
  qstr += QString::number(year);
  date_end->setText(qstr);
  date_end->setInputMask(mask);
  fm = date_end->fontMetrics();
  mg = date_end->textMargins();
  rect = fm.boundingRect(mask);
  date_end->setFixedWidth(rect.width() + mg.right() + mg.left()
                          + fm.averageCharWidth() * 2);
  grid->addWidget(date_end, 1, 1, Qt::AlignLeft);

  v_box->addWidget(formCoefView());

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  QPushButton *create_diagrm = new QPushButton;
  create_diagrm->setText(tr("Show diagram"));
  connect(create_diagrm, &QPushButton::clicked, this,
          &PPMGraphSelectionWindow::checkInput);
  h_box->addWidget(create_diagrm, 0, Qt::AlignCenter);

  QPushButton *cancel = new QPushButton;
  cancel->setText(tr("Cancel"));
  connect(cancel, &QPushButton::clicked, this,
          &PPMGraphSelectionWindow::close);
  h_box->addWidget(cancel, 0, Qt::AlignCenter);
}

QWidget *
PPMGraphSelectionWindow::formCoefView()
{
  QFrame *result = new QFrame;
  result->setFrameShape(QFrame::Box);

  QVBoxLayout *v_box = new QVBoxLayout;
  result->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setText(tr("Money turnover coefficients"));
  v_box->addWidget(lab, 0, Qt::AlignCenter);

  money_turnover_coef = new CoeffModel(this);
  double date_beg = MoneyAux::dateToJulianDay(23, 3, 1997, 0, 0, 0.0);
  double date_end = MoneyAux::dateToJulianDay(31, 12, 1997, 0, 0, 0.0);
  double coef = 0.001;
  money_turnover_coef->addCoef(date_beg, date_end, coef);

  QTableView *view = new QTableView;
  QAbstractItemModel *model = view->model();
  view->setModel(money_turnover_coef);
  delete model;
  view->setCurrentIndex(QModelIndex());
  view->setSizeAdjustPolicy(QTableView::AdjustToContentsOnFirstShow);
  QHeaderView *hh = view->horizontalHeader();
  for(int i = 0; i < hh->count(); i++)
    {
      hh->setSectionResizeMode(i, QHeaderView::Stretch);
    }
  v_box->addWidget(view);

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  QPushButton *add_coef = new QPushButton;
  add_coef->setText(tr("Add coefficient"));
  connect(add_coef, &QPushButton::clicked, this,
          [this]
            {
              AddCoefWindow *acw = new AddCoefWindow(this);
              connect(acw, &AddCoefWindow::signalCoef, money_turnover_coef,
                      &CoeffModel::addCoef);
              acw->show();
            });
  h_box->addWidget(add_coef, 0, Qt::AlignCenter);

  QPushButton *remove_coef = new QPushButton;
  remove_coef->setText(tr("Remove selected"));
  connect(remove_coef, &QPushButton::clicked, view,
          [this, view]
            {
              QModelIndex index = view->currentIndex();
              money_turnover_coef->removeCoef(index);
              view->setCurrentIndex(QModelIndex());
            });
  h_box->addWidget(remove_coef, 0, Qt::AlignCenter);

  QPushButton *reset_to_default = new QPushButton;
  reset_to_default->setText(tr("Reset to default"));
  connect(reset_to_default, &QPushButton::clicked, this,
          [this, date_beg, date_end, coef, view]
            {
              money_turnover_coef->removeAll();
              view->setCurrentIndex(QModelIndex());
              money_turnover_coef->addCoef(date_beg, date_end, coef);
            });
  h_box->addWidget(reset_to_default, 0, Qt::AlignCenter);

  return result;
}

void
PPMGraphSelectionWindow::errorDialog(QVBoxLayout *v_box)
{
  QLabel *lab = new QLabel;
  lab->setText(tr("Cannot find PPM file. Download data and try again."));
  lab->setWordWrap(true);
  lab->setAlignment(Qt::AlignCenter);
  v_box->addWidget(lab);

  QPushButton *close = new QPushButton;
  close->setText(tr("Close"));
  connect(close, &QPushButton::clicked, this, &PPMGraphSelectionWindow::close);
  v_box->addWidget(close, 0, Qt::AlignCenter);
}

void
PPMGraphSelectionWindow::checkInput()
{
  std::string str = date_begin->text().toStdString();
  std::stringstream strm;
  strm.str(str);
  std::tm tm = {};
  strm >> std::get_time(&tm, "%d.%m.%Y");
  double begin = MoneyAux::dateToJulianDay(tm.tm_mday, tm.tm_mon + 1,
                                           tm.tm_year + 1900, 0, 0, 0.0);

  str = date_end->text().toStdString();
  strm.clear();
  strm.str(str);
  tm = {};
  strm >> std::get_time(&tm, "%d.%m.%Y");
  double end = MoneyAux::dateToJulianDay(tm.tm_mday, tm.tm_mon + 1,
                                         tm.tm_year + 1900, 0, 0, 0.0);

  if(begin > end)
    {
      QWidget *window = new QWidget(this);
      window->setWindowFlag(Qt::Window);
      window->setAttribute(Qt::WA_DeleteOnClose);
      window->setWindowModality(Qt::WindowModal);

      QVBoxLayout *v_box = new QVBoxLayout;
      window->setLayout(v_box);

      QLabel *lab = new QLabel;
      lab->setText(tr("Incorrect dates!"));
      v_box->addWidget(lab, 0, Qt::AlignCenter);

      QPushButton *close = new QPushButton;
      close->setText("Close");
      connect(close, &QPushButton::clicked, window, &QWidget::close);
      v_box->addWidget(close, 0, Qt::AlignCenter);

      window->show();

      return void();
    }

  plot_v.clear();
  corrected_plot_v.clear();
  inflation.clear();
  coef_v = money_turnover_coef->getCoef();
  mpf_class mt = 0.0, t = 0.0, ppm, dppm;
  for(auto it = ppmv.begin(); it != ppmv.end(); it++)
    {
      mpf_class corrected;
      auto it_c = std::find_if(
          coef_v.begin(), coef_v.end(),
          [it](const std::tuple<double, double, double> &el)
            {
              if(it->date >= std::get<0>(el) && it->date <= std::get<1>(el))
                {
                  return true;
                }
              return false;
            });
      if(it_c != coef_v.end())
        {
          corrected = it->money_turnover * mpf_class(std::get<2>(*it_c));
          t += it->turnover;
          mt += corrected;
          ppm = t / mt;
          dppm = it->turnover / corrected;
        }
      else
        {
          corrected = it->money_turnover;
          t += it->turnover;
          mt += it->money_turnover;
          ppm = t / mt;
          dppm = it->current_ppm;
        }
      if(it->date < begin || it->date > end)
        {
          continue;
        }
      plot_v.push_back(*it);
      corrected_plot_v.push_back(*it);
      auto it_r = corrected_plot_v.rbegin();
      it_r->total_ppm = ppm;
      it_r->current_ppm = dppm;
      it_r->money_turnover = corrected;
    }
  if(plot_v.size() < 2)
    {
      return void();
    }

  DiagramWindow *dw = nullptr;
  Mode m = mode->currentData().value<Mode>();
  switch(m)
    {
    case Mode::PPMMode:
      {
        dw = new DiagramWindow(
            std::bind(&PPMGraphSelectionWindow::buildGraphPPM, this,
                      std::placeholders::_1),
            this);
        connect(dw, &DiagramWindow::signalDataRequest, this,
                [dw, this](const QPointF &global, const mglPoint &pos)
                  {
                    if(pos.x < 0)
                      {
                        return void();
                      }
                    size_t index = static_cast<size_t>(pos.x);
                    if(index >= plot_v.size())
                      {
                        return void();
                      }
                    GraphDataWindow *gdw = new GraphDataWindow(dw);
                    gdw->createWindow(plot_v[index]);
                    gdw->show();

                    gdw->move(dw->mapFromGlobal(global).toPoint());
                  });
        break;
      }
    case Mode::Turnover:
      {
        dw = new DiagramWindow(
            std::bind(&PPMGraphSelectionWindow::buildGraphTurnover, this,
                      std::placeholders::_1),
            this);
        connect(dw, &DiagramWindow::signalDataRequest, this,
                [dw, this](const QPointF &global, const mglPoint &pos)
                  {
                    if(pos.x < 0)
                      {
                        return void();
                      }
                    size_t index = static_cast<size_t>(pos.x);
                    if(index >= plot_v.size())
                      {
                        return void();
                      }
                    GraphDataWindow *gdw = new GraphDataWindow(dw);
                    gdw->createWindow(plot_v[index]);
                    gdw->show();

                    gdw->move(dw->mapFromGlobal(global).toPoint());
                  });
        break;
      }
    case Mode::MoneyTurnover:
      {
        dw = new DiagramWindow(
            std::bind(&PPMGraphSelectionWindow::buildGraphMoneyTurnover, this,
                      std::placeholders::_1),
            this);
        connect(dw, &DiagramWindow::signalDataRequest, this,
                [dw, this](const QPointF &global, const mglPoint &pos)
                  {
                    if(pos.x < 0)
                      {
                        return void();
                      }
                    size_t index = static_cast<size_t>(pos.x);
                    if(index >= plot_v.size())
                      {
                        return void();
                      }
                    GraphDataWindow *gdw = new GraphDataWindow(dw);
                    gdw->createWindow(plot_v[index]);
                    gdw->show();

                    gdw->move(dw->mapFromGlobal(global).toPoint());
                  });
        break;
      }
    case Mode::Inflation:
      {
        mpf_class base = corrected_plot_v[0].total_ppm;
        for(size_t i = 0; i < corrected_plot_v.size(); i++)
          {
            mpf_class val
                = ((base - corrected_plot_v[i].total_ppm) / base) * 100.0;
            inflation.push_back(val.get_d());
          }
        if(inflation.size() < 2)
          {
            break;
          }
        dw = new DiagramWindow(
            std::bind(&PPMGraphSelectionWindow::buildGraphInflation, this,
                      std::placeholders::_1),
            this);
        connect(dw, &DiagramWindow::signalDataRequest, this,
                [dw, this](const QPointF &global, const mglPoint &pos)
                  {
                    if(pos.x < 0)
                      {
                        return void();
                      }
                    size_t index = static_cast<size_t>(pos.x);
                    if(index >= plot_v.size())
                      {
                        return void();
                      }
                    GraphDataWindow *gdw = new GraphDataWindow(dw);
                    gdw->createWindow(plot_v[index].date, inflation[index]);
                    gdw->show();

                    gdw->move(dw->mapFromGlobal(global).toPoint());
                  });
        break;
      }
    default:
      break;
    }

  if(dw != nullptr)
    {
      dw->showMaximized();
    }
}

void
PPMGraphSelectionWindow::buildGraphPPM(mglGraph *gr)
{
  mpf_class min = 0.0, max = 0.0;
  std::vector<double> x;
  std::vector<double> total_ppm;
  std::vector<double> daily_ppm;
  for(size_t i = 0; i < corrected_plot_v.size(); i++)
    {
      if(corrected_plot_v[i].current_ppm > max)
        {
          max = corrected_plot_v[i].current_ppm;
        }
      if(corrected_plot_v[i].current_ppm < min)
        {
          min = corrected_plot_v[i].current_ppm;
        }
      if(corrected_plot_v[i].total_ppm > max)
        {
          max = corrected_plot_v[i].total_ppm;
        }
      if(corrected_plot_v[i].total_ppm < min)
        {
          min = corrected_plot_v[i].total_ppm;
        }
      x.push_back(static_cast<double>(i));
      total_ppm.push_back(corrected_plot_v[i].total_ppm.get_d());
      daily_ppm.push_back(corrected_plot_v[i].current_ppm.get_d());
    }

  gr->SetQuality(3);
  gr->Clf("{xffffffff}");

  std::string str = MoneyAux::convertToEncodign(
      tr("Purchasing power of money").toStdString(), "UTF-8", "");
  gr->Title(str.c_str(), "", 4);

  gr->SetRange('y', min.get_d(), max.get_d());
  gr->SetRange('x', 0.0, static_cast<double>(corrected_plot_v.size() - 1));  

  gr->SetTicks('x', static_cast<double>(x.size() - 1) / 4.0);
  gr->SetTicks('y', (max.get_d() - min.get_d()) / 3.0);
  gr->SetOriginTick(false);

  gr->Axis("xy0", "k");
  gr->Grid("xy", "k");

  mglData x_data(x);
  mglData y_data(total_ppm);
  gr->Plot(x_data, y_data, "r");

  y_data = mglData(daily_ppm);
  gr->Plot(x_data, y_data, "g");

  for(auto it = coef_v.begin(); it != coef_v.end(); it++)
    {
      double b = std::get<0>(*it);
      double e = std::get<1>(*it);
      if(b > corrected_plot_v[0].date && b < corrected_plot_v.rbegin()->date)
        {
          auto it_cpv
              = std::find_if(corrected_plot_v.begin(), corrected_plot_v.end(),
                             [b](const PPM &el)
                               {
                                 return b > el.date;
                               });
          if(it_cpv != corrected_plot_v.end())
            {
              double dist = static_cast<double>(
                  std::distance(corrected_plot_v.begin(), it_cpv));
              x.clear();
              x.push_back(dist);
              x.push_back(dist);
              x_data = mglData(x);
              x.clear();
              x.push_back(min.get_d());
              x.push_back(max.get_d());
              y_data = mglData(x);
              gr->Plot(x_data, y_data, "{x304bb9}");
            }
        }
      if(e > corrected_plot_v[0].date && e < corrected_plot_v.rbegin()->date)
        {

          auto it_cpv
              = std::find_if(corrected_plot_v.begin(), corrected_plot_v.end(),
                             [e](const PPM &el)
                               {
                                 return e < el.date;
                               });
          if(it_cpv != corrected_plot_v.end())
            {
              double dist = static_cast<double>(
                  std::distance(corrected_plot_v.begin(), it_cpv));
              x.clear();
              x.push_back(dist);
              x.push_back(dist);
              x_data = mglData(x);
              x.clear();
              x.push_back(min.get_d());
              x.push_back(max.get_d());
              y_data = mglData(x);
              gr->Plot(x_data, y_data, "{xa530b9}");
            }
        }
    }

  str = MoneyAux::convertToEncodign(tr("Days").toStdString(), "UTF-8", "");
  gr->Label('x', str.c_str(), 0);
  str = MoneyAux::convertToEncodign(tr("Shares/Rouble").toStdString(), "UTF-8",
                                    "");
  gr->Label('y', str.c_str(), 0);

  gr->SetFontSize(3.0);

  str = MoneyAux::convertToEncodign(tr("PPM").toStdString(), "UTF-8", "");
  gr->AddLegend(str.c_str(), "r");
  str = MoneyAux::convertToEncodign(tr("Daily PPM").toStdString(), "UTF-8",
                                    "");
  gr->AddLegend(str.c_str(), "g");
  str = MoneyAux::convertToEncodign(tr("Coefficient start").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "{x304bb9}");
  str = MoneyAux::convertToEncodign(tr("Coefficient end").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "{xa530b9}");
  gr->Legend(1.41, 1.3, "#-");
}

void
PPMGraphSelectionWindow::buildGraphTurnover(mglGraph *gr)
{
  mpf_class min = std::numeric_limits<double>::max(), max = 0.0;
  std::vector<double> x;
  std::vector<double> turnover;
  std::vector<double> average_turnover;
  mpf_class summ;
  size_t ticks = 0;
  for(auto it = ppmv.begin(); it != ppmv.end(); it++)
    {
      summ += it->turnover;
      if(it->date >= plot_v[0].date)
        {
          break;
        }
      ticks++;
    }
  for(size_t i = 0; i < plot_v.size(); i++)
    {
      summ += plot_v[i].turnover;
      mpf_class av = summ / mpf_class(static_cast<double>(ticks + i + 1));
      average_turnover.push_back(av.get_d());
      turnover.push_back(plot_v[i].turnover.get_d());
      x.push_back(static_cast<double>(i));
      if(plot_v[i].turnover < min)
        {
          min = plot_v[i].turnover;
        }
      if(plot_v[i].turnover > max)
        {
          max = plot_v[i].turnover;
        }
      if(av < min)
        {
          min = av;
        }
      if(av > max)
        {
          max = av;
        }
    }

  gr->SetQuality(3);
  gr->Clf("{xffffffff}");

  std::string str
      = MoneyAux::convertToEncodign(tr("Turnover").toStdString(), "UTF-8", "");
  gr->Title(str.c_str(), "", 4);

  gr->SetRange('y', min.get_d(), max.get_d());
  gr->SetRange('x', 0.0, static_cast<double>(plot_v.size() - 1));

  gr->SetTicks('x', static_cast<double>(x.size() - 1) / 4.0);
  gr->SetTicks('y', (max.get_d() - min.get_d()) / 3.0);
  gr->SetOriginTick(false);

  gr->Axis("xy0", "k");
  gr->Grid("xy", "k");

  mglData x_data(x);
  mglData y_data(average_turnover);
  gr->Plot(x_data, y_data, "r");

  y_data = mglData(turnover);
  gr->Plot(x_data, y_data, "g");

  str = MoneyAux::convertToEncodign(tr("Days").toStdString(), "UTF-8", "");
  gr->Label('x', str.c_str(), 0);
  str = MoneyAux::convertToEncodign(tr("Shares").toStdString(), "UTF-8", "");
  gr->Label('y', str.c_str(), 0);

  gr->SetFontSize(3.0);

  str = MoneyAux::convertToEncodign(tr("Average turnover").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "r");
  str = MoneyAux::convertToEncodign(tr("Daily turnover").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "g");
  gr->Legend(1.41, 1.3);
}

void
PPMGraphSelectionWindow::buildGraphMoneyTurnover(mglGraph *gr)
{
  mpf_class min = std::numeric_limits<double>::max(), max = 0.0;
  std::vector<double> x;
  std::vector<double> money_turnover;
  std::vector<double> average_money_turnover;
  mpf_class summ;
  size_t ticks = 0;
  for(auto it = ppmv.begin(); it != ppmv.end(); it++)
    {
      auto it_cv = std::find_if(
          coef_v.begin(), coef_v.end(),
          [it](const std::tuple<double, double, double> &el)
            {
              if(it->date >= std::get<0>(el) && it->date <= std::get<1>(el))
                {
                  return true;
                }
              return false;
            });
      if(it_cv == coef_v.end())
        {
          summ += it->money_turnover;
        }
      else
        {
          summ += it->money_turnover * mpf_class(std::get<2>(*it_cv));
        }
      if(it->date >= corrected_plot_v[0].date)
        {
          break;
        }
      ticks++;
    }
  for(size_t i = 0; i < corrected_plot_v.size(); i++)
    {
      summ += corrected_plot_v[i].money_turnover;
      mpf_class av = summ / (mpf_class(static_cast<double>(ticks + i + 1)));
      average_money_turnover.push_back(av.get_d());
      money_turnover.push_back(corrected_plot_v[i].money_turnover.get_d());
      x.push_back(static_cast<double>(i));
      if(corrected_plot_v[i].money_turnover < min)
        {
          min = corrected_plot_v[i].money_turnover;
        }
      if(corrected_plot_v[i].money_turnover > max)
        {
          max = corrected_plot_v[i].money_turnover;
        }
    }

  gr->SetQuality(3);
  gr->Clf("{xffffffff}");

  std::string str = MoneyAux::convertToEncodign(
      tr("Money turnover").toStdString(), "UTF-8", "");
  gr->Title(str.c_str(), "", 4);

  gr->SetRange('y', min.get_d(), max.get_d());
  gr->SetRange('x', 0.0, static_cast<double>(plot_v.size() - 1));  

  gr->SetTicks('y', (max.get_d() - min.get_d()) / 3.0);
  gr->SetTicks('x', static_cast<mreal>((x.size() - 1) / 4.0));
  gr->SetOriginTick(false);

  gr->Axis("xy0", "k");
  gr->Grid("xy", "k");

  mglData x_data(x);
  mglData y_data(average_money_turnover);
  gr->Plot(x_data, y_data, "r");

  y_data = mglData(money_turnover);
  gr->Plot(x_data, y_data, "g");

  for(auto it = coef_v.begin(); it != coef_v.end(); it++)
    {
      double b = std::get<0>(*it);
      double e = std::get<1>(*it);
      if(b > corrected_plot_v[0].date && b < corrected_plot_v.rbegin()->date)
        {
          auto it_cpv
              = std::find_if(corrected_plot_v.begin(), corrected_plot_v.end(),
                             [b](const PPM &el)
                               {
                                 return b > el.date;
                               });
          if(it_cpv != corrected_plot_v.end())
            {
              double dist = static_cast<double>(
                  std::distance(corrected_plot_v.begin(), it_cpv));
              x.clear();
              x.push_back(dist);
              x.push_back(dist);
              x_data = mglData(x);
              x.clear();
              x.push_back(min.get_d());
              x.push_back(max.get_d());
              y_data = mglData(x);
              gr->Plot(x_data, y_data, "{x304bb9}");
            }
        }
      if(e > corrected_plot_v[0].date && e < corrected_plot_v.rbegin()->date)
        {

          auto it_cpv
              = std::find_if(corrected_plot_v.begin(), corrected_plot_v.end(),
                             [e](const PPM &el)
                               {
                                 return e < el.date;
                               });
          if(it_cpv != corrected_plot_v.end())
            {
              double dist = static_cast<double>(
                  std::distance(corrected_plot_v.begin(), it_cpv));
              x.clear();
              x.push_back(dist);
              x.push_back(dist);
              x_data = mglData(x);
              x.clear();
              x.push_back(min.get_d());
              x.push_back(max.get_d());
              y_data = mglData(x);
              gr->Plot(x_data, y_data, "{xa530b9}");
            }
        }
    }

  str = MoneyAux::convertToEncodign(tr("Days").toStdString(), "UTF-8", "");
  gr->Label('x', str.c_str(), 0);
  str = MoneyAux::convertToEncodign(tr("Rubles").toStdString(), "UTF-8", "");
  gr->Label('y', str.c_str(), 0);

  gr->SetFontSize(3.0);

  str = MoneyAux::convertToEncodign(tr("Average turnover").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "r");
  str = MoneyAux::convertToEncodign(tr("Daily turnover").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "g");
  str = MoneyAux::convertToEncodign(tr("Coefficient start").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "{x304bb9}");
  str = MoneyAux::convertToEncodign(tr("Coefficient end").toStdString(),
                                    "UTF-8", "");
  gr->AddLegend(str.c_str(), "{xa530b9}");
  gr->Legend(1.41, 1.3, "#-");
}

void
PPMGraphSelectionWindow::buildGraphInflation(mglGraph *gr)
{
  double min = 0.0, max = 0.0;
  std::vector<double> x;

  for(size_t i = 0; i < inflation.size(); i++)
    {
      if(inflation[i] > max)
        {
          max = inflation[i];
        }
      if(inflation[i] < min)
        {
          min = inflation[i];
        }
      x.push_back(static_cast<double>(i));
    }

  gr->SetQuality(3);

  std::string str = MoneyAux::convertToEncodign(tr("Inflation").toStdString(),
                                                "UTF-8", "");
  gr->Title(str.c_str(), "", 4);

  gr->SetRange('y', min, max);
  gr->SetRange('x', 0.0, static_cast<double>(inflation.size() - 1));

  gr->SetTicks('x', static_cast<double>(x.size() - 1) / 4.0);
  gr->SetTicks('y', (max - min) / 3.0);
  gr->SetOriginTick(false);

  gr->Axis("xy0", "k");
  gr->Grid("xy", "k");

  mglData x_data(x);
  mglData y_data(inflation);
  gr->Plot(x_data, y_data, "r");

  str = MoneyAux::convertToEncodign(tr("Days").toStdString(), "UTF-8", "");
  gr->Label('x', str.c_str(), 0);
  str = "%";
  gr->Label('y', str.c_str(), 0);

  gr->SetFontSize(3.0);

  str = MoneyAux::convertToEncodign(tr("Inflation").toStdString(), "UTF-8",
                                    "");
  gr->AddLegend(str.c_str(), "r");
  gr->Legend(1.41, 1.3);
}
