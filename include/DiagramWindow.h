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
#ifndef DIAGRAMWINDOW_H
#define DIAGRAMWINDOW_H

#include <DiagramWidget.h>
#include <QWidget>
#include <functional>
#include <mgl2/mgl.h>

class DiagramWindow : public QWidget
{
  Q_OBJECT
public:
  DiagramWindow(std::function<void(mglGraph *)> drawCallback,
                QWidget *parent = nullptr);

signals:
  void
  signalDataRequest(const QPointF &global, const mglPoint &pos);

private:
  void
  createWindow();

  void
  createControlPanel(QWidget *parent);

  void
  saveDialog();

  std::function<void(mglGraph *)> drawCallback;

  QWidget *control_panel;
  DiagramWidget *diagram_widget;
};

#endif // DIAGRAMWINDOW_H
