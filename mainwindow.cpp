#include "mainwindow.h"
#include "QDir"
#include "QFileInfo"
#include "iostream"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QProgressDialog>
#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_inputFileName_textChanged(const QString &input) {
    QFileInfo file(input);

    // vérifie si le fichier existe
    if (file.isFile()) {
        // lance ffprobe, récupère l'output
        QString out = exec("ffprobe -pretty -hide_banner -show_streams \"" +
                           file.absoluteFilePath() + "\"");
        ui->videoInfo->setText(out);
        out.remove(0, out.indexOf("duration=") + 9).truncate(out.indexOf("\n"));
        ui->durationLabel->setText(out);

        // ffprobe vérifie s'il s'agit d'une vidéo
        validInput = out != "";
        if(!validInput) {
            ui->durationLabel->setText("Not a video file");
        }
    } else {
        ui->videoInfo->setText("");
        ui->durationLabel->setText("File not found");
        validInput = false;
    }

    updateResult();
}

void MainWindow::updateResult() {
    // les fichiers d'input et output, ainsi que les temps de début et de fin
    // sont vérifiés
    if (validInput && validOutput &&
        ui->startTime->time().msecsTo(ui->stopTime->time()) > 0) {
        int offsetMs = ui->startTime->time().msecsTo(ui->stopTime->time());
        QTime zero(0, 0);

        ui->resultCmd->setText(
            "ffmpeg -i \"" + ui->inputFileName->text() + "\" -ss " +
            ui->startTime->time().toString("HH:mm:ss.zzz") + " -t " +
            zero.addMSecs(offsetMs).toString("HH:mm:ss.zzz") +
            " -acodec copy -vcodec copy -y \"" + ui->outputFileName->text() +
            "\"");
    } else {
        ui->resultCmd->setText("");
    }
}

QString MainWindow::exec(const QString &cmd) const {
    QProcess process;
    process.start(cmd);
    process.waitForFinished(-1);
    QString stderr = process.readAllStandardError();  // debug info
    QString stdout = process.readAllStandardOutput(); // actual info (ugly)
    return stdout;
}

void MainWindow::on_actionClose_2_triggered() { exit(EXIT_SUCCESS); }

void MainWindow::on_browseInput_clicked() {
    ui->inputFileName->setText(QFileDialog::getOpenFileName());
}

void MainWindow::on_browseOutput_clicked() {
    ui->outputFileName->setText(QFileDialog::getSaveFileName());
}

void MainWindow::on_outputFileName_textChanged(const QString &output) {
    QFileInfo file(output);
    QFileInfo dir(file.absoluteDir().absolutePath());

    validOutput = dir.isDir();

    updateResult();
}

void MainWindow::on_stopTime_timeChanged(const QTime &time) { updateResult(); }

void MainWindow::on_startTime_timeChanged(const QTime &time) { updateResult(); }

void MainWindow::on_execCmd_clicked() {
    if (ui->resultCmd->text() != "") {
        cout << exec(ui->resultCmd->text()).toStdString();

        QMessageBox done;
        done.setText("Done !");
        done.exec();
    }
}
