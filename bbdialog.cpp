#include "bbdialog.h"
#include "ui_bbdialog.h"
#include <QDebug>

BBDialog::BBDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BBDialog)
{
    ui->setupUi(this);
    bbTemp = 50. ;
    hotFlag = true ;

}

BBDialog::~BBDialog()
{
    delete ui;
}

void BBDialog::setTemp (float t){
    QString st = QString ("%1").arg(t);
    ui->tempLE->setText (st) ;
}

void BBDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void BBDialog::on_bbacqAcqButton_clicked()
{

    qDebug () << ui->tempLE->text() ;
    bbTemp = ui->tempLE->text().toFloat() ;
    hotFlag = ui->hotRB->isChecked() ;
    this->accept () ;
}
