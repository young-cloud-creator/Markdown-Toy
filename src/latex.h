#ifndef LATEX_H
#define LATEX_H
#include <QString>
#include <QTextStream>

class Latex
{
private:
    QString tex_content;
    QString html_content;

public:
    Latex(){}
    Latex(QString &input);
    inline QString get_html(){return html_content;}
};

#endif // LATEX_H
