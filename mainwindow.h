#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vector>

#include "color_spaces/lab.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void on_pushButton_clicked();

private:
  Ui::MainWindow *ui;

  std::vector<Lab> getPallete(const QImage &img);
  void showPallete(const std::vector<Lab> &palette);
  void showImage(const QImage &img);
  void loadExample();
};
#endif // MAINWINDOW_H
