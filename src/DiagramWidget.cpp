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

#include <DiagramWidget.h>
#include <QImage>
#include <QPainter>

DiagramWidget::DiagramWidget(std::function<void(mglGraph *)> drawCallback,
                             QWidget *parent)
    : QWidget(parent)
{
  this->drawCallback = drawCallback;

  gr = new mglGraph;
}

DiagramWidget::~DiagramWidget()
{
  delete gr;
}

void
DiagramWidget::saveAs(const QString &path)
{
  int sz = gr->GetWidth() * gr->GetHeight() * 4;
  char *buf = new char[sz];
  gr->GetRGBA(buf, sz);

  QImage img(
      reinterpret_cast<uchar *>(buf), gr->GetWidth(), gr->GetHeight(),
      QImage::Format_RGBA8888,
      [](void *info)
        {
          char *buf = reinterpret_cast<char *>(info);
          delete[] buf;
        },
      buf);
  img.save(path);
}

void
DiagramWidget::resizeEvent(QResizeEvent *event)
{
  QSize sz = event->size();
  gr->DefaultPlotParam();
  gr->SetSize(sz.width(), sz.height());
  drawCallback(gr);
  QWidget::resizeEvent(event);
}

void
DiagramWidget::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::LosslessImageRendering);
  int sz = gr->GetWidth() * gr->GetHeight() * 4;
  char *buf = new char[sz];
  gr->GetRGBA(buf, sz);

  painter.fillRect(rect(), Qt::white);

  QImage img(
      reinterpret_cast<uchar *>(buf), gr->GetWidth(), gr->GetHeight(),
      QImage::Format_RGBA8888,
      [](void *info)
        {
          char *buf = reinterpret_cast<char *>(info);
          delete[] buf;
        },
      buf);
  painter.drawImage(0, 0, img);
}

void
DiagramWidget::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton)
    {
      QPointF p = event->position();
      mglPoint pmgl
          = gr->CalcXYZ(static_cast<int>(p.x()), static_cast<int>(p.y()));
      emit signalDataRequest(this->mapToGlobal(p), pmgl);
    }
  else
    {
      QWidget::mousePressEvent(event);
    }
}
