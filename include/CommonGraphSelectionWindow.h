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
#ifndef COMMONGRAPHSELECTIONWINDOW_H
#define COMMONGRAPHSELECTIONWINDOW_H

#include <Board.h>
#include <CoeffModel.h>
#include <QComboBox>
#include <QLineEdit>
#include <QWidget>
#include <Security.h>
#include <TradeDay.h>
#include <mgl2/mgl.h>

class CommonGraphSelectionWindow : public QWidget
{
  Q_OBJECT
public:
  CommonGraphSelectionWindow(QWidget *parent = nullptr);

private:
  void
  createWindow();

  void
  setBoards(int instrument_index);

  void
  setDates(int board_index);

  QWidget *
  dates();

  QWidget *
  turnoverCoef();

  QWidget *
  moneyTurnoverCoef();

  void
  checkInput();

  void
  buildPricesGrpah(mglGraph *gr);

  void
  buildPPMGrpah(mglGraph *gr);

  void
  buildTurnoverGrpah(mglGraph *gr);

  void
  buildMoneyTurnoverGrpah(mglGraph *gr);

  std::vector<Security> securities;
  std::vector<Board> boards;

  QComboBox *instrument;
  QComboBox *board;

  enum Mode
  {
    Prices,
    PPM,
    Turnover,
    MoneyTurnover
  };
  QComboBox *mode;

  CoeffModel *turnover_coef_model;
  CoeffModel *money_turnover_coef_model;

  QLineEdit *date_begin;
  QLineEdit *date_end;

  std::vector<TradeDay> values;
  std::vector<std::tuple<double, double, double>> money_coef_v;
  std::vector<std::tuple<double, double, double>> turnover_coef_v;
  std::vector<TradeDay> normalized;
};

#endif // COMMONGRAPHSELECTIONWINDOW_H
