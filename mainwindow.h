#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_inputFileName_textChanged(const QString &arg1);

    void on_actionClose_2_triggered();

    void on_browseInput_clicked();

    void on_browseOutput_clicked();

    void on_outputFileName_textChanged(const QString &arg1);

    void on_stopTime_timeChanged(const QTime &time);

    void on_startTime_timeChanged(const QTime &time);

    void on_execCmd_clicked();

private:
    Ui::MainWindow *ui;
    bool validInput = false;
    bool validOutput = false;
    QString exec(const QString& cmd) const;
    void updateResult();
};

#endif // MAINWINDOW_H
