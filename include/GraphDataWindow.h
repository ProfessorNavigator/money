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
#ifndef GRAPHDATAWINDOW_H
#define GRAPHDATAWINDOW_H

#include <Deal.h>
#include <PPM.h>
#include <QWidget>
#include <TradeDay.h>

class GraphDataWindow : public QWidget
{
  Q_OBJECT
public:
  GraphDataWindow(QWidget *parent = nullptr);

  void
  createWindow(const PPM &ppm);

  void
  createWindow(const double &date, const double &inflation);

  void
  createWindow(const TradeDay &raw, const TradeDay &normalized);

  void
  createWindow(const Deal &deal);
};

#endif // GRAPHDATAWINDOW_H
