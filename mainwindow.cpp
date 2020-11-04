#define COLUMNS 5
#define SUSPENDER 1
#define MATAR 2
#define CONTINUAR 3


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QString>
#include <QList>

#include <iostream>
#include <stdlib.h>//system
#include <signal.h>//sinais
#include <stdio.h>
#include <unistd.h>
#include  <time.h>
#include <sched.h>
#include <stdlib.h>


#include <sys/time.h>
#include <sys/resource.h>

#define _GNU_SOURCE
#include <sched.h>


using namespace std;

QString pid;
QString prioridade;
int porcentagemCPU[8]={0};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString command = "ps --no-header -eo %cpu,pid,ni,stat,pmem,cmd,psr";
    populandoTabela(command);
    startTimer(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::populandoTabela(QString command)
{
    ui->tabela_4->clearContents();

    QProcess processo;
    processo.start("sh", QStringList() << "-c" << command);
    processo.waitForFinished(-1);

    QString saida = processo.readAllStandardOutput();
    QStringList dados = saida.split("\n");


    QStringList dado;

    ui->tabela_4->setRowCount(dados.length()-1);

    int r = 0;

    foreach(QString it, dados){
        QString dadoAux = it.simplified();
        dado = dadoAux.split(" ");
            for(auto c=0; c < dado.length(); c++){
                ui->tabela_4->setItem(r, c, new QTableWidgetItem(dado[c]));
                ui->tabela_4->update();
            }

        ui->tabela_4->setItem(r, 6, new QTableWidgetItem(dado[dado.length()-1]));
        r++;
    }

}

void MainWindow::pegarPID()
{
   pid = ui->textEditPID_4->toPlainText();

}

void MainWindow::executarComando(int idComando)
{
    pegarPID();
    QString comando;

    switch (idComando) {
    case MATAR:
        comando = kill(pid.toInt(), SIGKILL);
        break;
    case SUSPENDER:
       comando = kill(pid.toInt(),SIGSTOP);
        break;
    case CONTINUAR:
        comando = kill(pid.toInt(),SIGCONT);
        break;
    default:
        break;
    }

    QByteArray comandoConvertido = comando.toLocal8Bit();
    const char *comandoFinal = comandoConvertido.data();

    system(comandoFinal);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(ui->textEditFiltro_4->toPlainText().isEmpty()){
        QString comando = "ps --no-header -eo %cpu,pid,ni,stat,pmem,cmd,psr";
        populandoTabela(comando);

    }else{
        QString textoFiltro = ui->textEditFiltro_4->toPlainText();

        QString comando;
        comando = " ps --no-header -eo %cpu,pid,ni,stat,pmem,cmd,psr | grep " + textoFiltro ;

        QByteArray comandoConvertido = comando.toLocal8Bit();
        const char *comandoFinal = comandoConvertido.data();


        populandoTabela(comandoFinal);
    }
    exibirPorcentagemCPU();
}

void MainWindow::suspender()
{
    executarComando(SUSPENDER);
}

void MainWindow::continuar()
{
    executarComando(CONTINUAR);
}

void MainWindow::matar()
{
    executarComando(MATAR);
}

void MainWindow::alterarCPU()
{
    int novaCPU;
    pegarPID();
    novaCPU = ui->comboBoxCPU_4->currentText().toInt();


    cpu_set_t mask;
    CPU_ZERO(&mask);


    CPU_SET(novaCPU - 1, &mask);
    int result = sched_setaffinity(pid.toInt(), sizeof(mask), &mask);

    qDebug() << novaCPU;
}

void MainWindow::mudarPrioridade()
{
    QString novaPrioridade;

    novaPrioridade = ui->comboBoxPrioridade_4->currentText();

    pegarPID();

    setpriority(PRIO_PROCESS, pid.toInt() , novaPrioridade.toInt());

}

void MainWindow::pegarPIDTabela(int r, int)
{

    ui->tabela_4->selectRow(r);
    QTableWidgetItem* pri = ui->tabela_4->item(r,2);
    prioridade = pri->data(Qt::DisplayRole).toString();

    QTableWidgetItem* tabelaPID = ui->tabela_4->item(r,1);
    QString pidclicou = tabelaPID->data(Qt::DisplayRole).toString();
    ui->textEditPID_4->setText(pidclicou);



}

void MainWindow::filtro()
{
     QString textoFiltro = ui->textEditFiltro_4->toPlainText();

     QString comando;
     comando = " ps --no-header -eo %cpu,pid,ni,stat,pmem,cmd,psr | grep " + textoFiltro ;

     QByteArray comandoConvertido = comando.toLocal8Bit();
     const char *comandoFinal = comandoConvertido.data();

     populandoTabela(comandoFinal);
}

void MainWindow::exibirPorcentagemCPU()
{

    QProcess processo;

    QString comando;
    comando = " ps --no-header -eo %cpu,psr";


    processo.start("sh", QStringList() << "-c" << comando);
    processo.waitForFinished(-1);

    QString saida = processo.readAllStandardOutput();
    QStringList dados = saida.split("\n");

    QStringList dado;
    int cont = 0;


    foreach(QString it, dados){
        QString dadoAux = it.simplified();
        dado = dadoAux.split(" ");
        cont++;
        if(cont != dados.length()){
            //qDebug() << dado[0].toDouble();
            switch (dado[1].toInt()) {

            case 0:
                porcentagemCPU[0]+=dado[0].toDouble();
                break;
            case 1:
                porcentagemCPU[1]+=dado[0].toDouble();
                break;
            case 2:
                porcentagemCPU[2]+=dado[0].toDouble();
                break;
            case 3:
                porcentagemCPU[3]+=dado[0].toDouble();
                break;
            case 4:
                porcentagemCPU[4]+=dado[0].toDouble();
                break;
            case 5:
                porcentagemCPU[5]+=dado[0].toDouble();
                break;
            case 6:
                porcentagemCPU[6]+=dado[0].toDouble();
                break;
            case 7:
                porcentagemCPU[7]+=dado[0].toDouble();
                break;
            default:
                break;

            }
       }
    }
    qDebug() << porcentagemCPU[3];
    ui->CPU0->setValue(porcentagemCPU[0]);
    ui->progressBar->setValue(porcentagemCPU[1]);
    ui->progressBar_2->setValue(porcentagemCPU[2]);
    ui->progressBar_3->setValue(porcentagemCPU[3]);
    ui->progressBar_4->setValue(porcentagemCPU[4]);
    ui->progressBar_5->setValue(porcentagemCPU[5]);
    ui->progressBar_6->setValue(porcentagemCPU[6]);
    ui->progressBar_7->setValue(porcentagemCPU[7]);

    for (int i=0; i<8 ; i++ ) {
        porcentagemCPU[i]=0;
    }
}

