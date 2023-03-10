#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QFile>
#include <QFileDialog>
#include <QTextEdit>
#include <QMessageBox>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QTextStream>
#include <QWebEngineView>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <vector>
#include "markdown.h"
#include "latex.h"
#include "word.h"
using std::vector;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void open_action();
    void save_action();
    void process();

    void preview();
    void preview_md();
    void preview_tex();
    void preview_docx();

    void search_file();
    void set_search_result();
    void draw_search_result();
    void open_file_list(QListWidgetItem * item);

private:
    Ui::MainWindow *ui;
    QMenu *file_menu;
    QAction *open_file;
    QAction *save_file;
    QFile *result;
    QFile *cur_file;
    QFileInfo *file_info;
    QTextEdit *edit_text;
    QWebEngineView *view_text;

    QAction *search;
    QString root;
    QLineEdit *search_content;
    QLabel *root_line_tip;
    QListWidget *search_result;
    QDialog *search_dialog;

    vector<QString> files;
};
#endif // MAINWINDOW_H
