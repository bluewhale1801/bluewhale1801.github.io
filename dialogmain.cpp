#include "dialogmain.h"
#include "ui_dialogmain.h"

Dialogmain::Dialogmain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialogmain)
{
    ui->setupUi(this);
}

Dialogmain::~Dialogmain()
{
    delete ui;
}

void Dialogmain::on_pushButton_clicked()
{

}

