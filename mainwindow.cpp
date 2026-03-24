#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Tokenize(QString line)
{
    QString helpstr = nullptr;
    linelist.append(line);

    for (int i = 0; i < line.size(); i++){
        //проверка на многострочный комментарий
        if (longComment){
            if (line[0] == "=" && line[1] == "c" && line[2] == "u" && line[3] == "t")
                longComment = false;
            else break;
        }

        if (i < 4 && line.size() >= 4)
            if (line[0] == "=" && line[1] == "p" && line[2] == "o" && line[3] == "d"){
                longComment = true;
                break;
            }

        //проверка на однострочный комментарий
        if(!insideString)
            if (line[i] == "#") break;

        //проверка на кавычки
        if (insideString == false && (line[i] == '"' || line[i] == "'")){
            insideString = true;
        }

        if (!insideString){
            if (line[i] == "}" && i == line.size()-1)
                stringlist.append("}");

            if (line[i] == "{" && i == line.size()-1)
                stringlist.append("{");

            if((line[i] == '(' ) && i == line.size() -1){
                helpstr += line[i];
                stringlist.append(helpstr);
                helpstr = nullptr;
                continue;
            }
            else if ((line[i] == " " || line[i] == ";" || line[i] == "{") && helpstr != nullptr){
                stringlist.append(helpstr);
                helpstr = nullptr;
                if (line[i] == ";") stringlist.append(";");
                if (line[i] == "{") stringlist.append("{");
                continue;
            }

            if (line[i] == "("){
                if (helpstr != nullptr)
                    stringlist.append(helpstr);
                stringlist.append("(");
                helpstr = nullptr;
                continue;
            }
            else if(line[i] == ")"){
                if (helpstr != nullptr)
                    stringlist.append(helpstr);
                stringlist.append(")");
                helpstr = nullptr;
                if(!line.contains("when")){
                    if (line[i+1] == ";")
                        stringlist.append(";");
                    else if (line[i+1] == "{")
                        stringlist.append("{");
                    else if (line[i+2] == "{")
                        stringlist.append("{");
                    break;
                }
                else
                    continue;
            }

            helpstr += line[i];
        }
        else if (insideString){
            if((line[i] == '"' )&& i == line.size() -1){
                helpstr += line[i];
                stringlist.append(helpstr);
                insideString = false;
                helpstr = nullptr;
                continue;
            }
            else if (((line[i] == ";" || line[i] == " " || line[i] == "," || line[i] == ")") && line[i-1] == '"') && helpstr != nullptr){
                if (line[i] == ")"){
                    stringlist.append(helpstr);
                    stringlist.append(")");
                    stringlist.append(";");
                    insideString = false;
                    helpstr = nullptr;
                    // возможно убрать точки с запятой
                    continue;
                }
                stringlist.append(helpstr);
                insideString = false;
                helpstr = nullptr;
                if (line[i] == ";")
                    stringlist.append(";");
                continue;
            }
            helpstr += line[i];
        }
    }
    ui->textEdit->append(line);
}

void MainWindow::FindOperands(){
    bool cancel = false;
    // проверка на скаляры, хеши и массивы
    for (int i = 0; i < wordslist.size(); i++){
        // тут будет проверка
        if (i != 0 && wordslist[i-1] != "my"){
            for (Operand& vectorItem : vector) {
                if (vectorItem.name.contains(wordslist[i])) {
                    if (wordslist[i+1] == "="){
                        QString helpstr = wordslist[i+2];
                        Operand helpstruct; helpstruct.name = helpstr; helpstruct.amount = 0;
                        vector.append(helpstruct);
                    }
                    break;
                }
            }
        }
        if (wordslist[i] == "my"){
            // проверка на повтор
            for (const auto& item : vector) {
                if (item.name == wordslist[i+1]) {
                    cancel = true;
                    break;
                }
            }
            if (cancel) {
                cancel = false;
                continue;
            }

            if (wordslist[i+1][0] == "$"){
                QString helpstr = wordslist[i+1];
                Operand helpstruct; helpstruct.name = helpstr; helpstruct.amount = 0;
                vector.append(helpstruct);
                if (wordslist[i+2] == "="){
                    helpstr = wordslist[i+3];
                    helpstruct.name = helpstr; helpstruct.amount = 0;
                    vector.append(helpstruct);
                }

            }
            else if (wordslist[i+1][0] == "%"){
                QString helpstr = wordslist[i+1];
                Operand helpstruct; helpstruct.name = helpstr; helpstruct.amount = 0;
                vector.append(helpstruct);
                if (wordslist[i+2] == "=" && wordslist[i+3] == "("){
                    for (int j = i+4; j < 100; j++){
                        if (wordslist[j] == ")") break;
                        if (wordslist[j] != "=>"){
                           helpstruct.name = wordslist[j]; helpstruct.amount = 0;
                           vector.append(helpstruct);
                        }
                    }
                }
            }
            else if (wordslist[i+1][0] == "@"){
                QString helpstr = wordslist[i+1];
                Operand helpstruct; helpstruct.name = helpstr; helpstruct.amount = 0;
                vector.append(helpstruct);
            }
        }
    }

    // проверить количество операндов
    for (QString& listItem : wordslist) {
        //QString helpstr = listItem.remove('"');
        for (Operand& vectorItem : vector) {
            /*
            if (vectorItem.name.contains(helpstr, Qt::CaseInsensitive)) {
                vectorItem.amount += 1;
                break;
            }
            */
            if (listItem.contains(vectorItem.name.remove('"'))) {
                vectorItem.amount += 1;
                break;
            }
        }
    }
}

void MainWindow::FindOperators(){
    //сначала проверяем функции
    bool cancel = false;
    for (int i = 0; i < wordslist.size(); i++){
        if (wordslist[i] == "sub"){
            QString helpstr = wordslist[i+1];
            Operator helpstruct; helpstruct.name = helpstr; helpstruct.amount = 0;
            Opvector.append(helpstruct);
        }
    }
    //потом проверяем print
    for (int i = 0; i < wordslist.size(); i++){
        if (wordslist[i] == "print"){
            QString helpstr = wordslist[i];
            Operator helpstruct; helpstruct.name = helpstr; helpstruct.amount = 0;
            Opvector.append(helpstruct);
            break;
        }
    }

    for (const QString& str : wordslist) {
        for (const QString& op : operators) {
            if (str == op/*str.contains(op)*/) {
                //проверка на повторки
                for (const auto& item : Opvector) {
                    if (item.name == str) {
                        cancel = true;
                        break;
                    }
                }
                if (cancel) {
                    cancel = false;
                    break;
                }
                //проверка на повторки закончена

                Operator helpstruct; helpstruct.name = op; helpstruct.amount = 0;
                Opvector.append(helpstruct);
                continue;
            }
        }
    }
    // проверить количество операторов
    for (QString& listItem : wordslist) {
        //QString helpstr = listItem.remove('"');
        for (Operator& vectorItem : Opvector) {
            /*
            if (vectorItem.name.contains(helpstr, Qt::CaseInsensitive)) {
                vectorItem.amount += 1;
                break;
            }
            */
            if (listItem.remove(" ") == vectorItem.name) {
                vectorItem.amount += 1;
                break;
            }
        }
    }
}

int MainWindow::findFirstNonSpace(const QString &str){
    QRegularExpression re ("\\S");
    QRegularExpressionMatch match = re.match(str);
    if(match.hasMatch()){
        return match.capturedStart();
    }
    return -1;
}

void MainWindow::on_openFile_clicked()
{
    insideString = false;
    longComment = false;

    QString filePath = QFileDialog::getOpenFileName(nullptr, "Выберите файл", "", "Текстовые файлы (*.txt);;Все файлы (*)");
    if (!filePath.isEmpty())
    {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly))
        {
            QTextStream in(&file);
            while (!in.atEnd())
            {
                QString line = in.readLine();
                Tokenize(line);
            }
            file.close();
            qDebug() << stringlist;
        }
    }

    for (int i = 0; i< stringlist.size(); i++){
        if(stringlist[i] == " ") continue;
        wordslist.append(stringlist[i]);
    }
    for (int i = 0; i< wordslist.size(); i++){
        ui ->textEdit->append(wordslist[i]);
    }

    FindOperands();
    FindOperators();
    ui->tableWidget->setRowCount(vector.size());
    for (int i = 0; i < vector.size(); i++){
        QString str = nullptr;

        str = vector[i].name;
        QTableWidgetItem *item1 = new QTableWidgetItem(str);
        item1->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 0, item1);

        str = QString::number(vector[i].amount);
        QTableWidgetItem *item2 = new QTableWidgetItem(str);
        item2->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 1, item2);
    }
    ui->n2_label->setText(QString::number(vector.size()));
    int number = 0;
    for (int i = 0; i < vector.size(); i++){
        number += vector[i].amount;
    }
    ui->N2_label->setText(QString::number(number));

    ui->tableWidget_2->setRowCount(Opvector.size());
    for (int i = 0; i < Opvector.size(); i++){
        QString str = nullptr;

        str = Opvector[i].name;
        QTableWidgetItem *item3 = new QTableWidgetItem(str);
        item3->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_2->setItem(i, 0, item3);

        str = QString::number(Opvector[i].amount);
        QTableWidgetItem *item4 = new QTableWidgetItem(str);
        item4->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_2->setItem(i, 1, item4);
    }
    ui->ni_label->setText(QString::number(Opvector.size()));
    int chislo = 0;
    for (int i = 0; i < Opvector.size(); i++){
        chislo += Opvector[i].amount;
    }
    ui->n1_label->setText(QString::number(chislo));

    ui->vocabulary->setText(QString::number(Opvector.size() + vector.size()));
    ui->length->setText(QString::number(chislo + number));
    ui->size->setText(QString::number((chislo + number) * std::log(Opvector.size() + vector.size()) / std::log(2)));

    // метрика джилба
    int amountCl = 0;
    for (int i =0; i < Opvector.size(); i++){
        if(Opvector[i].name == "if" || Opvector[i].name == "when" || Opvector[i].name == "for"){
            amountCl += Opvector[i].amount;
            //break;
        }
    }
    ui->CL_label->setText(QString::number(amountCl));
    if (amountCl != 0)
        ui->cl_label->setText(QString::number(static_cast<double>(amountCl) / chislo));
    else
        ui->cl_label->setText(QString::number(0));

    int index = 0;
    int previous = -1;
    int whenIndex = 0;
    int defaultIndex = 0;
    bool firstWhen = true;
    QVector <int> forWhen;

    for (int i = 0; i < linelist.size(); i++){
        if (linelist[i].contains("for") || linelist[i].contains("if")){
            index = findFirstNonSpace(linelist[i]);
            if (index > previous){
                previous = index;
            }
        }
        else if (linelist[i].contains("when")){
            whenCounter++;
            if(firstWhen){
                whenIndex = findFirstNonSpace(linelist[i]) - 4;
                firstWhen = false;
            }
        }
        else if (linelist[i].contains("default")){
            defaultIndex = whenIndex + (whenCounter-1) * 4;
            forWhen.append(defaultIndex);
            whenCounter = 0;
            firstWhen = true;
        }
    }
    int maxWhen = 0;
    for (int i = 0; i < forWhen.size(); i++){
        if (forWhen[i] > maxWhen)
            maxWhen = forWhen[i];
    }

    if (maxWhen > previous)
        NestingLevel = maxWhen / 4;
    else
        NestingLevel = previous / 4;

    // уже вывод
    ui ->CLI_label->setText(QString::number(NestingLevel));
}
