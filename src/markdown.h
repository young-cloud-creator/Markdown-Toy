#ifndef MARKDOWN_H
#define MARKDOWN_H
#include <QString>
#include <QTextStream>
#include <vector>
#include <QFile>

enum key_word {
    plainText = 0,
    para = 1,
    href = 2,
    ul = 3,
    ol = 4,
    li = 5,
    em = 6,
    b = 7,
    br = 8,
    img = 9,
    h1 = 10,
    h2 = 11,
    h3 = 12,
    h4 = 13,
    h5 = 14,
    h6 = 15,
    precode = 16,
    code = 17,
    quote = 18
};

const QString front_tag[]=
{
    "", "<p>", "", "<ul>", "<ol>", "<li>", "<em>", "<b>",
    "<br />", "", "<h1>", "<h2>", "<h3>", "<h4>", "<h5>",
    "<h6>", "<pre><code>", "<code>", "<blockquote>"
};

const QString back_tag[]=
{
  "", "</p>", "", "</ul>", "</ol>", "</li>", "</em>", "</b>",
  "", "", "</h1>", "</h2>", "</h3>", "</h4>", "</h5>", "</h6>",
  "</code></pre>", "</code>", "</blockquote>"
};

struct node
{
    int type;
    std::vector<node *> children;
    QString text[2]; //0-plainText 1-url,path,...

    node(int t):type(t){}
};

class Markdown
{
private:
    node *root;
    QString md_content;
    QString html_content;

    void clear(node* root);
    std::pair<bool, QString> preProcess(QString &s);
    std::pair<int, QString> processLine(QString str);
    void processInsert(node* root, QString str);
    void Transform2html(node* root);

public:
    Markdown(){}
    Markdown(QString &input);
    ~Markdown();
    inline QString get_html(){return "<link rel=\"stylesheet\" href=\"markdown.css\">"+html_content;}
};

#endif // MARKDOWN_H
