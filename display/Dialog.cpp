#include "Dialog.hpp"
#include "ui_Dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog(){
    delete ui;
}

/* Returns the text of the field
 *
 * @param Field to analyse
 */
QString Dialog::getText(){return ui->lineEdit->text();}
