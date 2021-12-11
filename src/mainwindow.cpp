#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <io.h>
#include <regex>
using std::string;

void getFiles(QString path, vector<QString> &files, QString subdir)
{
    long long file_handle = 0;
    _finddata_t file_info;
    QString p = path+"/*.*";
    QByteArray tmp = p.toLocal8Bit();
    if ((file_handle = _findfirst(tmp.data(), &file_info)) != -1)
    {
        do
        {
            if (strcmp(file_info.name, ".") != 0 && strcmp(file_info.name, "..") != 0)
            {
                if (file_info.attrib == _A_SUBDIR)
                {
                       getFiles(path+"/"+QString::fromLocal8Bit(file_info.name), files, subdir+QString::fromLocal8Bit(file_info.name)+"/");
                }
                else
                {
                    files.push_back(subdir+QString::fromLocal8Bit(file_info.name));
                }
            }
        } while (_findnext(file_handle, &file_info) == 0);
        _findclose(file_handle);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , cur_file(nullptr)
    , file_info(nullptr)
    , root("D:/")
    , root_line_tip(nullptr)
    , search_result(nullptr)
    , search_dialog(nullptr)
{
    ui->setupUi(this);
    file_menu = new QMenu(tr("文件 "), this);
    open_file = new QAction(tr("打开文件 "), file_menu);
    save_file = new QAction(tr("保存文件 "), file_menu);
    search = new QAction(tr("搜索文件"), this);
    open_file->setShortcut(QKeySequence::Open);
    save_file->setShortcut(QKeySequence::Save);
    file_menu->addAction(open_file);
    file_menu->addAction(save_file);
    menuBar()->addMenu(file_menu);
    menuBar()->addAction(search);

    edit_text = new QTextEdit(this);
    view_text = new QWebEngineView(this);
    edit_text->setPlaceholderText(tr("编辑区 "));

    QWidget *widget = new QWidget();
    setCentralWidget(widget);
    QHBoxLayout *hbLayout = new QHBoxLayout();
    hbLayout->addWidget(edit_text);
    hbLayout->addWidget(view_text);
    hbLayout->setStretchFactor(edit_text, 1);
    hbLayout->setStretchFactor(view_text, 1);
    centralWidget()->setLayout(hbLayout);

    result = new QFile(tr("result.html"));

    connect(open_file, &QAction::triggered, this, &MainWindow::open_action);
    connect(save_file, &QAction::triggered, this, &MainWindow::save_action);
    connect(edit_text, &QTextEdit::textChanged, this, &MainWindow::preview);
    connect(search, &QAction::triggered, this, &MainWindow::search_file);
}

MainWindow::~MainWindow()
{
    if(result->exists())
        result->remove();
    delete ui;
}

void MainWindow::search_file()
{
    search_dialog = new QDialog(this);
    QVBoxLayout *vlayout = new QVBoxLayout(search_dialog);
    search_dialog->setFixedSize(600, 500);

    root_line_tip = new QLabel(tr("当前搜索根目录：")+root);
    search_result = new QListWidget(search_dialog);
    search_content = new QLineEdit(search_dialog);

    QLabel *content_tip = new QLabel(tr("文件关键词："), search_content);

    QPushButton *set_root = new QPushButton(tr("选择根目录"), search_dialog);

    search_dialog->setWindowTitle(tr("搜索文件"));
    search_dialog->setLayout(vlayout);

    QHBoxLayout *hlayout_0 = new QHBoxLayout;
    hlayout_0->addWidget(root_line_tip);
    hlayout_0->addWidget(set_root);

    QHBoxLayout *hlayout_1 = new QHBoxLayout;
    hlayout_1->addWidget(content_tip);
    hlayout_1->addWidget(search_content);

    vlayout->addLayout(hlayout_0);
    vlayout->addLayout(hlayout_1);
    vlayout->addWidget(search_result);

    connect(set_root, &QPushButton::clicked, this, &MainWindow::set_search_result);
    connect(search_content, &QLineEdit::textChanged, this, &MainWindow::draw_search_result);
    connect(search_result, &QListWidget::itemDoubleClicked, this, &MainWindow::open_file_list);
    search_dialog->setAttribute(Qt::WA_DeleteOnClose);
    search_dialog->exec();
    files.clear();
}

void MainWindow::open_file_list(QListWidgetItem *item)
{
    QString path = root+"/"+item->text();
    if(path.isEmpty())
        QMessageBox::information(this, tr("打开文件 "), tr("你没有选择任何文件 "));
    else
    {
        QFile *file = new QFile(path);
        if(!file->open(QIODevice::ReadWrite | QIODevice::Text))
            QMessageBox::warning(this, tr("打开文件 "), tr("无法打开文件！ "));
        else
        {
            cur_file = file;
            file->close();
            file_info = new QFileInfo(path);
            if(!file_info)
                QMessageBox::warning(this, tr("文件信息 "), tr("无法获取文件信息！ "));
            else
            {
                search_dialog->close();
                process();
            }
        }
    }
}

void MainWindow::draw_search_result()
{
    search_result->clear();
    try
    {
        std::regex name_reg(search_content->text().toStdString());
        for(auto item=files.begin(); item!=files.end(); item++)
        {
            QString file_name = (*item);
            QString match_name = (*item);
            qsizetype name_idx = match_name.lastIndexOf('/')+1;
            if(name_idx)
                match_name = match_name.mid(name_idx);

            if(file_name.mid(file_name.length()-3)==".md" ||
                    file_name.mid(file_name.length()-4)==".tex" ||
                        file_name.mid(file_name.length()-5)==".docx")
            {
                if(std::regex_match(match_name.toStdString(), name_reg) ||
                        file_name.contains(search_content->text()))
                    search_result->addItem(file_name);
            }
        }
    }
    catch (std::regex_error)
    {
        for(auto item=files.begin(); item!=files.end(); item++)
        {
            QString file_name = (*item);
            if(file_name.mid(file_name.length()-3)==".md" ||
                    file_name.mid(file_name.length()-4)==".tex" ||
                        file_name.mid(file_name.length()-5)==".docx")
            {
                if(file_name.contains(search_content->text()))
                    search_result->addItem(file_name);
            }
        }
    }
}

void MainWindow::set_search_result()
{
    QString tmp = QFileDialog::getExistingDirectory(this, tr("选择根目录"), root);
    if(tmp.isEmpty())
    {
        QMessageBox::information(search_dialog, tr("选择根目录"), tr("你没有选择根目录！"));
    }
    else
    {
        root = tmp;
        files.clear();
        root_line_tip->setText(tr("当前搜索根目录：")+root);
        getFiles(root, files, "");
        draw_search_result();
    }
}

void MainWindow::open_action()
{
    QString path = QFileDialog::getOpenFileName(this, tr("打开文件 "), "/", tr("Markdown文件 (*.md);;Latex文件 (*.tex);;Word文件 (*.docx);;Html文件 (*.html);;(*.*)"));
    if(path.isEmpty())
        QMessageBox::information(this, tr("打开文件 "), tr("你没有选择任何文件 "));
    else
    {
        QFile *file = new QFile(path);
        if(!file->open(QIODevice::ReadWrite | QIODevice::Text))
            QMessageBox::warning(this, tr("打开文件 "), tr("无法打开文件！ "));
        else
        {
            cur_file = file;
            file->close();
            file_info = new QFileInfo(path);
            if(!file_info)
                QMessageBox::warning(this, tr("文件信息 "), tr("无法获取文件信息！ "));
            else
                process();
        }
    }
}

void MainWindow::save_action()
{
    if(cur_file)
    {
        if(file_info->suffix()==tr("docx"))
            QMessageBox::information(this, tr("保存文件 "), tr("不支持编辑word文档 "));
        else if(!cur_file->open(QIODevice::WriteOnly | QIODevice::Text))
            QMessageBox::warning(this, tr("保存文件 "), tr("保存失败，无法打开该文件！ "));
        else
        {
            QTextStream text(cur_file);
            text << edit_text->toPlainText();
            cur_file->close();
            statusBar()->showMessage(tr("保存成功！ "), 1000);
        }
    }
    else
    {
        QString path = QFileDialog::getOpenFileName(this, tr("保存为 "), "/", tr("Markdown文件 (*.md);;Latex文件 (*.tex);;Html文件 (*.html);;(*.*)"));
        if(path.isEmpty())
            QMessageBox::information(this, tr("打开文件 "), tr("你没有选择保存的位置 "));
        else
        {
            QFile *file = new QFile(path);
            if(!file->open(QIODevice::WriteOnly | QIODevice::Text))
                QMessageBox::warning(this, tr("保存文件 "), tr("保存失败！ "));
            else
            {
                file_info = new QFileInfo(path);
                if(!file_info)
                    QMessageBox::warning(this, tr("文件信息 "), tr("无法获取文件信息！ "));
                else
                {
                    cur_file = file;
                    QTextStream text(file);
                    text << edit_text->toPlainText();
                    file->close();
                    statusBar()->showMessage(tr("保存成功！ "), 1000);
                }
            }
        }
    }
}

void MainWindow::process()
{
    QString suffix = file_info->suffix();
    if(!cur_file->open(QIODevice::ReadWrite | QIODevice::Text))
        QMessageBox::warning(this, tr("打开文件 "), tr("无法打开文件！ "));
    else
    {
        if (suffix == "md" || suffix == "tex" || suffix == "html")
        {
            QTextStream text(cur_file);
            edit_text->setPlainText(text.readAll());
            cur_file->close();
            preview();
        }
        else if(suffix == "docx")
        {
            edit_text->clear();
            preview();
        }
        else
            QMessageBox::warning(this, tr("文件处理 "), tr("不受支持的文件格式 "));
    }
}

void MainWindow::preview()
{
    if(!file_info || file_info->suffix() == "html")
    {
        if(!result->open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, tr("预览文件 "), tr("资源文件无法打开！ "));
            return;
        }
        QTextStream result_stream(result);
        result_stream << edit_text->toPlainText();
        result->close();
    }
    else
    {
        QString suffix = file_info->suffix();
        if (suffix == "md")
            preview_md();
        else if (suffix == "tex")
            preview_tex();
        else if (suffix == "docx")
            preview_docx();
        else
            QMessageBox::warning(this, tr("文件预览 "), tr("不受支持的文件格式 "));
    }
    view_text->load(QUrl(tr("file:/result.html")));
}

void MainWindow::preview_md()
{
    if(!result->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("预览文件 "), tr("资源文件无法打开！ "));
        return;
    }

    QTextStream result_stream(result);

    QString text(edit_text->toPlainText());
    Markdown transform(text);
    result_stream << transform.get_html();

    result->close();
}

void MainWindow::preview_tex()
{
    if(!result->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("预览文件 "), tr("资源文件无法打开！ "));
        return;
    }
    QTextStream result_stream(result);

    QString text(edit_text->toPlainText());
    Latex transform(text);
    result_stream << transform.get_html();

    result->close();
}


void MainWindow::preview_docx()
{
    if(!result->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("预览文件 "), tr("资源文件无法打开！ "));
        return;
    }
    QTextStream result_stream(result);

    Word transform(file_info->filePath());
    result_stream << transform.get_html();

    result->close();
}
