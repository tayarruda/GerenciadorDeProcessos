#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void populandoTabela(QString command);
    void pegarPID();
    void executarComando(int idComando);
    void timerEvent(QTimerEvent *event);//refresh
    void exibirPorcentagemCPU();

public slots:
     void suspender();
     void continuar();
     void matar();
     void alterarCPU();
     void mudarPrioridade();
     void pegarPIDTabela(int r, int);
     void filtro();


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
