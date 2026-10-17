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

#include <GraphDataWindow.h>
#include <MoneyAux.h>
#include <QGridLayout>
#include <QLabel>

GraphDataWindow::GraphDataWindow(QWidget *parent) : QWidget(parent)
{
  this->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
  this->setAttribute(Qt::WA_DeleteOnClose);
}

void
GraphDataWindow::createWindow(const PPM &ppm)
{
  QGridLayout *grid = new QGridLayout;
  this->setLayout(grid);

  QLabel *lab = new QLabel;
  lab->setText(tr("Date") + ":");
  grid->addWidget(lab, 0, 0, Qt::AlignLeft);

  int day, month, year, hour, minut;
  double sec;

  MoneyAux::julianDateToDate(ppm.date, day, month, year, hour, minut, sec);

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

  qstr += ".";
  qstr += QString::number(year);

  lab = new QLabel;
  lab->setText(qstr);
  grid->addWidget(lab, 0, 1, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(tr("PPM") + ":");
  grid->addWidget(lab, 1, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(ppm.total_ppm.get_d(), 'f', 5));
  grid->addWidget(lab, 1, 1, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(tr("Daily PPM") + ":");
  grid->addWidget(lab, 2, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(ppm.current_ppm.get_d(), 'f', 5));
  grid->addWidget(lab, 2, 1, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(tr("Turnover") + ":");
  grid->addWidget(lab, 3, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(ppm.turnover.get_d(), 'f', 0));
  grid->addWidget(lab, 3, 1, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(tr("Money turnover") + ":");
  grid->addWidget(lab, 4, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(ppm.money_turnover.get_d(), 'f', 2));
  grid->addWidget(lab, 4, 1, Qt::AlignLeft);
}

void
GraphDataWindow::createWindow(const double &date, const double &inflation)
{
  QGridLayout *grid = new QGridLayout;
  this->setLayout(grid);

  QLabel *lab = new QLabel;
  lab->setText(tr("Date") + ":");
  grid->addWidget(lab, 0, 0, Qt::AlignLeft);

  int day, month, year, hour, minut;
  double sec;

  MoneyAux::julianDateToDate(date, day, month, year, hour, minut, sec);

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

  qstr += ".";
  qstr += QString::number(year);

  lab = new QLabel;
  lab->setText(qstr);
  grid->addWidget(lab, 0, 1, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(tr("Inflation") + ":");
  grid->addWidget(lab, 1, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(inflation, 'f', 2) + "%");
  grid->addWidget(lab, 1, 1, Qt::AlignLeft);
}

void
GraphDataWindow::createWindow(const TradeDay &raw, const TradeDay &normalized)
{
  QGridLayout *grid = new QGridLayout;
  this->setLayout(grid);

  int row = 0;

  QLabel *lab = new QLabel;
  lab->setText(tr("Date") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  int day, month, year, hour, minut;
  double sec;

  MoneyAux::julianDateToDate(raw.date, day, month, year, hour, minut, sec);

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

  qstr += ".";
  qstr += QString::number(year);

  lab = new QLabel;
  lab->setText(qstr);
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("First price") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(raw.first_price, 'f', 2));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("First price (normalized)") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(normalized.first_price, 'f', 2));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Last price") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(raw.last_price, 'f', 2));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Last price (normalized)") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(normalized.last_price, 'f', 2));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Weighted average price") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(raw.wa_price, 'f', 2));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Weighted average price (normalized)") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(normalized.wa_price, 'f', 2));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("PPM calculated price (normalized)") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(normalized.ppm_price.get_d(), 'f', 2));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Turnover") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(raw.turnover));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Turnover (normalized)") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(normalized.turnover));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Average turnover") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(raw.average_turnover.get_d(), 'f', 0));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Average turnover (normalized)") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(normalized.average_turnover.get_d(), 'f', 0));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Money turnover") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(raw.money_turnover, 'f', 2));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Money turnover (normalized)") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(normalized.money_turnover, 'f', 2));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Average money turnover") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(raw.average_money_turnover.get_d(), 'f', 2));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Average money turnover (normalized)") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(
      QString::number(normalized.average_money_turnover.get_d(), 'f', 2));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
}

void
GraphDataWindow::createWindow(const Deal &deal)
{
  QGridLayout *grid = new QGridLayout;
  this->setLayout(grid);

  int row = 0;

  QLabel *lab = new QLabel;
  lab->setText(tr("Time") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  int day, month, year, hour, minut;
  double sec;
  MoneyAux::julianDateToDate(deal.deal_time, day, month, year, hour, minut,
                             sec);
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

  lab = new QLabel;
  lab->setText(val);
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Trade number") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(deal.trade_num));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Price") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(deal.price, 'f', 2));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("PPM calculated price") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(deal.ppm_price.get_d(), 'f', 2));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Turnover") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(deal.quantity));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Average turnover") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(deal.average_quan.get_d(), 'f', 0));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Money turnover") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(deal.money_volume, 'f', 2));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Average money turnover") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(deal.average_money.get_d(), 'f', 2));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("PPM") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(deal.ppm.get_d(), 'f'));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;

  lab = new QLabel;
  lab->setText(tr("Deal PPM") + ":");
  grid->addWidget(lab, row, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(QString::number(
      static_cast<double>(deal.quantity) / deal.money_volume, 'f'));
  grid->addWidget(lab, row, 1, Qt::AlignLeft);
  row++;
}
