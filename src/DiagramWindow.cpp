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

#include <DiagramWindow.h>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QImageWriter>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

DiagramWindow::DiagramWindow(std::function<void(mglGraph *)> drawCallback,
                             QWidget *parent)
    : QWidget(parent)
{
  this->drawCallback = drawCallback;

  this->setWindowFlag(Qt::Window);
  this->setAttribute(Qt::WA_DeleteOnClose);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle(tr("Diagram"));

  createWindow();
}

void
DiagramWindow::createWindow()
{
  QVBoxLayout *v_box = new QVBoxLayout;
  v_box->setContentsMargins(0, 0, 0, 0);
  this->setLayout(v_box);

  diagram_widget = new DiagramWidget(drawCallback);
  connect(diagram_widget, &DiagramWidget::signalDataRequest, this,
          [this](const QPointF &global, const mglPoint &pos)
            {
              emit signalDataRequest(global, pos);
            });

  QScrollArea *scrl = new QScrollArea;
  scrl->setWidgetResizable(true);
  scrl->setWidget(diagram_widget);
  v_box->addWidget(scrl);

  createControlPanel(scrl);
}

void
DiagramWindow::createControlPanel(QWidget *parent)
{
  control_panel = new QWidget(parent);

  QVBoxLayout *v_box = new QVBoxLayout;
  control_panel->setLayout(v_box);

  QPushButton *increase = new QPushButton;
  increase->setText(tr("Increase"));
  connect(increase, &QPushButton::clicked, parent,
          [parent]
            {
              QScrollArea *scrl = dynamic_cast<QScrollArea *>(parent);
              if(scrl == nullptr)
                {
                  return void();
                }
              QWidget *w = scrl->widget();
              if(w == nullptr)
                {
                  return void();
                }
              QSize sz = w->size();
              sz.setHeight(sz.height() * 1.1);
              sz.setWidth(sz.width() * 1.1);
              w->setFixedSize(sz);
            });
  v_box->addWidget(increase, 0, Qt::AlignCenter);

  QPushButton *decrease = new QPushButton;
  decrease->setText(tr("Decrease"));
  connect(decrease, &QPushButton::clicked, parent,
          [parent]
            {
              QScrollArea *scrl = dynamic_cast<QScrollArea *>(parent);
              if(scrl == nullptr)
                {
                  return void();
                }
              QWidget *w = scrl->widget();
              if(w == nullptr)
                {
                  return void();
                }
              QSize sz = w->size();
              sz.setHeight(sz.height() * 0.9);
              sz.setWidth(sz.width() * 0.9);
              w->setFixedSize(sz);
            });
  v_box->addWidget(decrease, 0, Qt::AlignCenter);

  QPushButton *resize_to_window = new QPushButton;
  resize_to_window->setText(tr("Resize to fit window"));
  connect(resize_to_window, &QPushButton::clicked, parent,
          [parent]
            {
              QScrollArea *scrl = dynamic_cast<QScrollArea *>(parent);
              if(scrl == nullptr)
                {
                  return void();
                }
              QWidget *w = scrl->widget();
              if(w == nullptr)
                {
                  return void();
                }
              QSize sz = scrl->size();
              w->setFixedSize(sz);
            });
  v_box->addWidget(resize_to_window, 0, Qt::AlignCenter);

  QPushButton *save_as = new QPushButton;
  save_as->setText(tr("Save as") + "...");
  connect(save_as, &QPushButton::clicked, this, &DiagramWindow::saveDialog);
  v_box->addWidget(save_as, 0, Qt::AlignCenter);
}

void
DiagramWindow::saveDialog()
{
  QFileDialog *fd = new QFileDialog(this);
  fd->setAttribute(Qt::WA_DeleteOnClose);
  fd->setWindowModality(Qt::WindowModal);

  fd->setAcceptMode(QFileDialog::AcceptSave);
  fd->setDirectory(QDir::homePath());

  QList<QByteArray> filters = QImageWriter::supportedImageFormats();
  QStringList list;
  for(qsizetype i = 0; i < filters.size(); i++)
    {
      QString val = "*." + QString(filters[i]);
      list.append(val);
    }
  fd->selectNameFilter("*.png");
  fd->setNameFilters(list);
  fd->setDefaultSuffix("png");
  fd->selectFile("diagram.png");

  connect(fd, &QFileDialog::fileSelected, diagram_widget,
          &DiagramWidget::saveAs);

  fd->show();
}
