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

        // affiche l'output de ffprobe
        ui->videoInfo->setText(out);

        // récupère la durée de la vidéo dans l'output de ffprobe
        out.remove(0, out.indexOf("duration=") + 9)
            .truncate(out.indexOf("\n") - 3);

        // affiche la durée de la vidéo dans le label prévu à cet effet
        ui->durationLabel->setText(out);

        // si ffprobe ne renvoie rien, ce n'est pas une vidéo
        validInput = out != "";
        if (!validInput) {
            ui->videoInfo->setText("<b>Not a video file</b>");
            ui->durationLabel->setText("");
        } else {
            // règle la fin de la vidéo en output à la durée de l'input
            ui->stopTime->setTime(
                QTime::fromString(ui->durationLabel->text(), TIME_FORMAT));
        }
    } else {
        // si le fichier n'existe pas, on affiche un message d'erreur
        ui->videoInfo->setText("<b>File not found</b>");
        ui->durationLabel->setText("");
        validInput = false;
    }

    // mise à jour de la ligne de commande résultante
    updateResult();
}

void MainWindow::updateResult() {
    // les fichiers d'input et output, ainsi que les temps de début et de fin
    // sont vérifiés
    QTime duration = QTime::fromString(ui->durationLabel->text(), TIME_FORMAT);
    if (!validInput) {
        ui->resultCmd->setText("");
        ui->resultCmd->setPlaceholderText("Invalid input file");
    } else if (!validOutput) {
        ui->resultCmd->setText("");
        ui->resultCmd->setPlaceholderText("Invalid output file path");
    } else if (ui->startTime->time().msecsTo(ui->stopTime->time()) <= 0) {
        ui->resultCmd->setText("");
        ui->resultCmd->setPlaceholderText(
            "Output video must start before it stops");
    } else if (ui->stopTime->time().msecsTo(duration) < 0) {
        ui->resultCmd->setText("");
        ui->resultCmd->setPlaceholderText(
            "Stop time is greater than source duration");
    } else {
        // les paramètres sont corrects => compile la ligne de commande
        int offsetMs = ui->startTime->time().msecsTo(ui->stopTime->time());
        ui->resultCmd->setText(
            "ffmpeg -i \"" + ui->inputFileName->text() + "\" -ss " +
            ui->startTime->time().toString(TIME_FORMAT) + " -t " +
            QTime::fromMSecsSinceStartOfDay(offsetMs).toString(TIME_FORMAT) +
            " -acodec copy -vcodec copy -y \"" + ui->outputFileName->text() +
            "\"");
    }
}

QString MainWindow::exec(const QString &cmd) const {
    QProcess process;
    process.start(cmd);
    process.waitForFinished(-1); // attend la fin de l'exécution de la commande
    QString stdout = process.readAllStandardOutput();
    return stdout;
}

void MainWindow::on_actionClose_2_triggered() { exit(EXIT_SUCCESS); }

void MainWindow::on_browseInput_clicked() {
    // ouvre le file explorer en mode "Open"
    ui->inputFileName->setText(QFileDialog::getOpenFileName());
}

void MainWindow::on_browseOutput_clicked() {
    // ouvre le file explorer en mode "Save"
    ui->outputFileName->setText(QFileDialog::getSaveFileName());
}

void MainWindow::on_outputFileName_textChanged(const QString &output) {
    QFileInfo file(output);
    QFileInfo dir(file.absoluteDir().absolutePath());

    validOutput = dir.isDir();

    // mise à jour de la ligne de commande résultante
    updateResult();
}

void MainWindow::on_stopTime_timeChanged() {
    // mise à jour de la ligne de commande résultante
    updateResult();
}

void MainWindow::on_startTime_timeChanged() {
    // mise à jour de la ligne de commande résultante
    updateResult();
}

void MainWindow::on_execCmd_clicked() {
    if (ui->resultCmd->text() != "") {
        // lance la ligne de commande résultant des paramètres entrés par
        // l'utilisateur
        exec(ui->resultCmd->text()).toStdString();

        // affiche un popup à la fin du processus
        QMessageBox done;
        done.setText("Done !");
        done.exec();
    } else {
        QMessageBox err;
        err.setText(ui->resultCmd->placeholderText());
        err.exec();
    }
}
