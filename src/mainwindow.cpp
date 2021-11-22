#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , cur_file(nullptr)
    , file_info(nullptr)
{
    ui->setupUi(this);
    file_menu = new QMenu(tr("文件 "), this);
    open_file = new QAction(tr("打开文件 "), file_menu);
    save_file = new QAction(tr("保存文件 "), file_menu);
    open_file->setShortcut(QKeySequence::Open);
    save_file->setShortcut(QKeySequence::Save);
    file_menu->addAction(open_file);
    file_menu->addAction(save_file);
    menuBar()->addMenu(file_menu);

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
}

MainWindow::~MainWindow()
{
    if(result->exists())
        result->remove();
    delete ui;
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
