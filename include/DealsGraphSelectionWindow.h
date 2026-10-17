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
#ifndef DEALSGRAPHSELECTIONWINDOW_H
#define DEALSGRAPHSELECTIONWINDOW_H

#include <Board.h>
#include <Deal.h>
#include <QComboBox>
#include <QLineEdit>
#include <QShowEvent>
#include <QWidget>
#include <Security.h>
#include <mgl2/mgl.h>

class DealsGraphSelectionWindow : public QWidget
{
  Q_OBJECT
public:
  DealsGraphSelectionWindow(QWidget *parent = nullptr);

  void
  createWindow();

signals:
  void
  signalLoaded();

protected:
  void
  showEvent(QShowEvent *event) override;

private:
  void
  setDate(int date_index);

  void
  setInstrument(int instrument_index);

  QWidget *
  times();

  void
  checkInput();

  void
  plotPrices(mglGraph *gr);

  void
  plotPPM(mglGraph *gr);

  void
  plotTurnover(mglGraph *gr);

  void
  plotMoneyTurnover(mglGraph *gr);

  QComboBox *date;
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

  QLineEdit *time_start;
  QLineEdit *time_end;

  std::vector<Security> securities;
  std::vector<Board> boards;
  std::vector<Deal> deals;
  std::vector<Deal> plot_v;
  std::string currency;

  bool showed = false;
};

#endif // DEALSGRAPHSELECTIONWINDOW_H
