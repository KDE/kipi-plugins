//////////////////////////////////////////////////////////////////////////////
//
//    FINDDUPPLICATEIMAGES.CPP
//
//    Copyright (C) 2001 Richard Groult <rgroult at jalix.org> (from ShowImg project)
//    Copyright (C) 2004 Gilles Caulier <caulier dot gilles at gmail dot com>
//    Copyright (C) 2004 Richard Groult <rgroult at jalix.org>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

#include "fastcompare.h"
#include "actions.h"
#include <qstringlist.h>
#include <qapplication.h>
#include <kdebug.h>
#include <qfileinfo.h>

KIPIFindDupplicateImagesPlugin::FastCompare::FastCompare( QObject* parent )
    :m_parent( parent )
{
}


QDict < QPtrVector < QFile > > KIPIFindDupplicateImagesPlugin::FastCompare::compare( const QStringList& filesList )
{
    QDict < QPtrVector < QFile > > res;
    QDict < QPtrVector < QFile > >*dict = new QDict < QPtrVector < QFile > >;
    dict->setAutoDelete(true);
    QPtrVector < QFile > *list;

    QString size;
    QFile *file;
    int nbrF = 0;

    sendMessage( m_parent, KIPIFindDupplicateImagesPlugin::Progress, QString::null, filesList.count()*2, true, false );

    kdDebug( 51000 ) << filesList.count() << " images to parse with Fast method..." << endl;

    for ( QStringList::ConstIterator item = filesList.begin(); item != filesList.end(); ++item )
    {
        if ( m_stopRequested )
            return QDict < QPtrVector < QFile > >();

        QString itemName(*item);
        nbrF++;

        sendMessage( m_parent, KIPIFindDupplicateImagesPlugin::FastParsing, itemName, 0, true, false );

        // Create a file
        file = new QFile( itemName );

        // Read the file size
        size = QString::number(QFileInfo (*file).size ());

        // if not in the table, we do it
        if ( !dict->find (size) )
        {
            list = new QPtrVector < QFile >;
            list->setAutoDelete(true);
            dict->insert (size, list);
        }

        // Read the list
        list = (QPtrVector < QFile > *)dict->find (size);

        //Add the file
        list->resize (list->size () + 1);
        list->insert (list->size () - 1, file);

        //sendMessage( m_parent, KIPIFindDupplicateImagesPlugin::FastParsing, itemName, 0, false, true );
    }

    // For counting the files comparaison tasks.
    int count = 0;
    QDictIterator < QPtrVector < QFile > >itcount (*dict);        // Iterator for dict.

    while (itcount.current ())
    {
        list = (QPtrVector < QFile > *)itcount.current ();

        if (list->size () != 1)
            for (unsigned int i = 0; i < list->size (); i++)
                ++count;

        ++itcount;
    }

    // Files comparison
    QDictIterator < QPtrVector < QFile > >it (*dict);        // Iterator for dict.

    while (it.current ())
    {
        if ( m_stopRequested )
            return QDict < QPtrVector < QFile > >();
        QDict < QFile > *fait = new QDict < QFile >;
        list = (QPtrVector < QFile > *)it.current ();

        if (list->size () != 1)
        {
            for (unsigned int i = 0; i < list->size (); i++)
            {
                QFile *file1 = (QFile *) (list->at (i));

                sendMessage( m_parent, KIPIFindDupplicateImagesPlugin::Exact,file1->name(),filesList.count() + count, true, false );

                if (!fait->find (file1->name()))
                {
                    for (unsigned int j = i + 1; j < list->size (); j++)
                    {
                        QFile *file2 = (QFile *) (list->at (j));

                        // The files are equals ?

                        if (equals (file1, file2))
                        {
                            QPtrVector < QFile > *vect;

                            // Add the file.

                            if (!res.find (file1->name ()))
                            {
                                vect = new QPtrVector < QFile >;
                                vect->setAutoDelete(true);
                                res.insert (file1->name (), vect);
                            }
                            else
                                vect = (QPtrVector < QFile > *)res.find (file1->name ());

                            vect->resize (vect->size () + 1);
                            vect->insert (vect->size () - 1, file2);

                            fait->insert(file2->name(), file2);
                        }
                    }
                }

                // sendMessage( m_parent, KIPIFindDupplicateImagesPlugin::Exact, file1->name(), 0, false, true );
            }
        }

        delete(fait);
        ++it;
    }

    delete (it);

    return res;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

bool KIPIFindDupplicateImagesPlugin::FastCompare::equals(QFile * f1, QFile * f2)
{
    if ( QFileInfo (*f1).size () != QFileInfo (*f2).size () )
        return false;

    f1->open (IO_ReadOnly);
    f2->open (IO_ReadOnly);

    QDataStream s1 (f1);
    QDataStream s2 (f2);

    Q_INT8 b1, b2;
    bool eq = true;

    while ( !s1.atEnd () && eq )
    {
        s1 >> b1;
        s2 >> b2;
        eq = (b1 == b2);
    }

    f1->close ();
    f2->close ();
    return eq;
}
