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
#ifndef DIAGRAMWIDGET_H
#define DIAGRAMWIDGET_H

#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QWidget>
#include <functional>
#include <mgl2/mgl.h>

class DiagramWidget : public QWidget
{
  Q_OBJECT
public:
  DiagramWidget(std::function<void(mglGraph *)> drawCallback,
                QWidget *parent = nullptr);

  virtual ~DiagramWidget();

  void
  saveAs(const QString &path);

signals:
  void
  signalDataRequest(const QPointF &global, const mglPoint &pos);

protected:
  void
  resizeEvent(QResizeEvent *event) override;

  void
  paintEvent(QPaintEvent *event) override;

  void
  mousePressEvent(QMouseEvent *event) override;

private:
  std::function<void(mglGraph *)> drawCallback;

  mglGraph *gr;
};

#endif // DIAGRAMWIDGET_H
