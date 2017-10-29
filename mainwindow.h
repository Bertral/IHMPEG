#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
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

    void on_stopTime_timeChanged();

    void on_startTime_timeChanged();

    void on_execCmd_clicked();

  private:
    const QString TIME_FORMAT = "H:mm:ss.zzz";
    Ui::MainWindow *ui;
    bool validInput = false;  // indique validité du fichier en input
    bool validOutput = false; // indique la validité du fichier en output

    /**
     * @brief exec lance la commande en argument et renvoie la sortie standard
     * @param cmd
     * @return QString contenant le résultat lu dans stdout
     */
    QString exec(const QString &cmd) const;

    /**
     * @brief updateResult met à jour la ligne de commande résultante des
     * paramètres entrés par l'utilisateur
     */
    void updateResult();
};

#endif // MAINWINDOW_H
