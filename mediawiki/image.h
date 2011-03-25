#ifndef IMAGE_H
#define IMAGE_H
#include <QString>
#include <KUrl>
#include <QStringList>
#include <QSize>
class Image
{
    KUrl m_url;
    QString m_name;
    QString m_description;
    QStringList m_tags;
    QSize m_size;

public:
    Image();
    KUrl        url(){return m_url;}
    QString     name(){return m_name;}
    QString     description(){return m_description;}
    QStringList tag(){return m_tags;}
    QSize       size(){return m_size;}

    void    setUrl(KUrl url){m_url=url;}
    void    setName(QString name){m_name=name;}
    void    setDescription(QString description){m_description=description;}
    void    setTag(QStringList tags){m_tags=tags;}
    void    setSize(QSize size){m_size=size;}
};
bool operator==( Image & l, Image & r )
{
    return  l.url() == r.url() &&
            l.name() == r.name() &&
            l.description() == r.description() &&
            l.tag() == r.tag() &&
            l.size() == r.size();

}

#endif // IMAGE_H
