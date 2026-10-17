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
#ifndef PPMGRAPHSELECTIONWINDOW_H
#define PPMGRAPHSELECTIONWINDOW_H

#include <CoeffModel.h>
#include <PPM.h>
#include <QComboBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <mgl2/mgl.h>
#include <vector>

class PPMGraphSelectionWindow : public QWidget
{
  Q_OBJECT
public:
  PPMGraphSelectionWindow(QWidget *parent = nullptr);

private:
  void
  createWindow();

  QWidget *
  formCoefView();  

  void
  errorDialog(QVBoxLayout *v_box);

  void
  checkInput();

  void
  buildGraphPPM(mglGraph *gr);

  void
  buildGraphTurnover(mglGraph *gr);

  void
  buildGraphMoneyTurnover(mglGraph *gr);

  void
  buildGraphInflation(mglGraph *gr);

  std::vector<PPM> ppmv;
  std::vector<PPM> plot_v;
  std::vector<PPM> corrected_plot_v;
  std::vector<double> inflation;
  std::vector<std::tuple<double, double, double>> coef_v;

  enum Mode
  {
    Turnover,
    MoneyTurnover,
    PPMMode,
    Inflation
  };

  QComboBox *mode;
  QLineEdit *date_begin;
  QLineEdit *date_end;

  CoeffModel *money_turnover_coef;
};

#endif // PPMGRAPHSELECTIONWINDOW_H
