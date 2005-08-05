/* ============================================================
 * File  : flickrtalker.cpp
 * Author: Vardhman Jain <vardhman @ gmail.com>
 * Date  : 2005-07-07
 * Description :
 *
 * Copyright 2005 by Vardhman Jain

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include <qcstring.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qimage.h>
#include <qstringlist.h>
#include <klocale.h>
#include <kio/job.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <kstandarddirs.h>
#include <kmdcodec.h>
#include <kapp.h>
#include <kmessagebox.h>
#include<qurl.h>
#include<qlineedit.h>
#include<qmessagebox.h>
#include <qdom.h>
#include <cstring>
#include <cstdio>

#include <string>
#include "flickritem.h"
#include "mpform.h"
#include "flickrtalker.h"
#include "flickrwindow.h"
namespace KIPIFlickrExportPlugin
{

FlickrTalker::FlickrTalker( QWidget* parent )
    : m_parent( parent ),  m_job( 0 )
{
	m_apikey="e2af1d606b93738b572f6ced255cb7ba";
	m_secret="08cbabc805de35a7";
	connect(this,SIGNAL(signalAuthenticate()),SLOT(slotAuthenticate()));
	//connect(this,SIGNAL(signalError(const QString &str)),SLOT(slotError(const QString &str)));
	authProgressDlg=new QProgressDialog();
}

FlickrTalker::~FlickrTalker()
{
    if (m_job)
        m_job->kill();
}


QString FlickrTalker::getApiSig(QString secret, QStringList headers)
{
	QStringList compressed ;//= new List<string>(headers.Length);

	for ( QStringList::Iterator it = headers.begin(); it != headers.end(); ++it ) {
		QStringList str=QStringList::split("=",(*it));
		compressed.append(str[0].stripWhiteSpace()+str[1].stripWhiteSpace());
	}
	
	compressed.sort();
	QString merged=compressed.join("");
	QString final = secret + merged;
	const char *test=final.ascii();
	KMD5 context (test);
	//kdDebug()<< "Test Hex Digest output: " << context.hexDigest().data() << endl;
	return context.hexDigest().data();
}

//get the Api sig and send it to the server server should return a frob.
void FlickrTalker::getFrob() {
	if (m_job){
		m_job->kill();
		m_job = 0;
	}
	QString url="http://www.flickr.com/services/rest/?";
        QStringList headers ;//
	headers.append("method=flickr.auth.getFrob");
	headers.append("api_key="+ m_apikey);
	QString md5=getApiSig(m_secret,headers);
	headers.append("api_sig="+ md5);
	QString queryStr=headers.join("&");
	QString final_url=url+queryStr;
	//kdDebug()<<"Updated the query as :#:"<<queryStr<<"###"<<endl;		
	QByteArray tmp;
	KIO::TransferJob* job = KIO::http_post(final_url,tmp,false);

	job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );	
	connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            SLOT(data(KIO::Job*, const QByteArray&)));
   	connect(job, SIGNAL(result(KIO::Job *)),
            SLOT(slotResult(KIO::Job *)));
	m_state = FE_GETFROB;
	authProgressDlg->setLabelText("Getting the Frob");
	authProgressDlg->setProgress(1,4);
	m_job   = job;
	m_buffer.resize(0);
    	emit signalBusy( true );
}

void FlickrTalker::checkToken(const QString& token) {
	if (m_job){
		m_job->kill();
		m_job = 0;
	}
	QString url="http://www.flickr.com/services/rest/?";
        QStringList headers ;//
	headers.append("method=flickr.auth.checkToken");
	headers.append("api_key="+ m_apikey);
	headers.append("auth_token="+ token);
	QString queryStr=headers.join("&");
	QString final_url=url+queryStr;
	QByteArray tmp;
	KIO::TransferJob* job = KIO::http_post(final_url,tmp,false);

	job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );	
	connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            SLOT(data(KIO::Job*, const QByteArray&)));
   	connect(job, SIGNAL(result(KIO::Job *)),
            SLOT(slotResult(KIO::Job *)));
	m_state = FE_CHECKTOKEN;
	authProgressDlg->setLabelText("Checking if previous token is still valid");
	authProgressDlg->setProgress(1,4);
	m_job   = job;
	m_buffer.resize(0);
    	emit signalBusy( true );
}

void FlickrTalker::slotAuthenticate(){
	//
	if (m_job){
		m_job->kill();
		m_job = 0;
	}
	QString url="http://www.flickr.com/services/auth/?";
        QStringList headers ;//
	headers.append("api_key="+ m_apikey);
	headers.append("frob="+ m_frob);
	headers.append("perms=write");
	QString md5=getApiSig(m_secret,headers);
	headers.append("api_sig="+ md5);
	const QString queryStr=url+headers.join("&");
 	KApplication::kApplication()->invokeBrowser(queryStr);
	int valueOk=KMessageBox::questionYesNo(0, "Please Follow through the instructions in the browser window and return back to press ok if you are authenticated or press No", "Flickr::Kipi Plugin:Web Authorization"); 
	
	if(valueOk==KMessageBox::Yes){
		getToken(); 
	authProgressDlg->setLabelText("Authenticating the User on web");
	authProgressDlg->setProgress(2,4);
    	emit signalBusy( false );
	}
	else //if(valueOk==KMessageBox::No)
	{
		kdDebug()<<"User didn't proceed with getToken Authorization, cannot proceed further, aborting"<<endl;
		cancel();	
	}
	
}


void FlickrTalker::getToken(){
	if (m_job){
		m_job->kill();
		m_job = 0;
	}
	QString url="http://www.flickr.com/services/rest/?";
        QStringList headers ;//
	headers.append("api_key="+ m_apikey);
	headers.append("method=flickr.auth.getToken");
	headers.append("frob="+ m_frob);
	QString md5=getApiSig(m_secret,headers);
	headers.append("api_sig="+ md5);
	QString queryStr=headers.join("&");
	QString completeUrl=url+queryStr;
	QByteArray tmp;	
	KIO::TransferJob* job = KIO::http_post(completeUrl,tmp,false);
	job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );	
	connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),SLOT(data(KIO::Job*, const QByteArray&)));
   	connect(job, SIGNAL(result(KIO::Job *)), SLOT(slotResult(KIO::Job *)));
	m_state = FE_GETTOKEN;
	m_job   = job;
	m_buffer.resize(0);
    	emit signalBusy( true );
	kdDebug()<<"Please paste the url on the browser:\n"<<queryStr<<endl;
	authProgressDlg->setLabelText("Getting the Token from the server");
	authProgressDlg->setProgress(3,4);
}
void FlickrTalker::listAlbums()
{
	//To be implemented.
}
void FlickrTalker::getPhotoProperty(const QString& photoId,const QString& method,const QString& argList){
	if (m_job){
		m_job->kill();
		m_job = 0;
	}
	QString url="http://www.flickr.com/services/rest/?";
        QStringList headers ;//
	headers.append("api_key="+ m_apikey);
	headers.append("method="+method);
	headers.append("frob="+ m_frob);
	headers.append(argList);
	QString md5=getApiSig(m_secret,headers);
	headers.append("api_sig="+ md5);
	QString queryStr=headers.join("&");
	QString completeUrl=url+queryStr;
	QByteArray tmp;	
	KIO::TransferJob* job = KIO::http_post(completeUrl,tmp,false);
	job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );	
	connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),SLOT(data(KIO::Job*, const QByteArray&)));
   	connect(job, SIGNAL(result(KIO::Job *)), SLOT(slotResult(KIO::Job *)));
	m_state = FE_GETPHOTOPROPERTY;
	m_job   = job;
	m_buffer.resize(0);
    	emit signalBusy( true );
	kdDebug()<<"Getting Photo Properties:\n"<<queryStr<<endl;
	//authProgressDlg->setLabelText("Getting the Token from the server");
	//authProgressDlg->setProgress(3,4);
}
void FlickrTalker::listPhotos( const QString& albumName )
{
	QString temp=albumName;//just for avoiding warning.
	//To be implemented
}

void FlickrTalker::createAlbum( const QString& parentAlbumName,
                                 const QString& albumName,
                                 const QString& albumTitle,
                                 const QString& albumCaption )
{
	QString temp=parentAlbumName+albumName+albumTitle+albumCaption;//for avoing warnings.
	//To be implemented. The eqivalent for Album is sets.
}

bool FlickrTalker::addPhoto(  const QString& photoPath,
                                FPhotoInfo& info,
				bool  rescale, int maxDim )
{
	if (m_job)
	{
		m_job->kill();
		m_job = 0;
	}
	QString url="http://www.flickr.com/services/upload/?";
	QString path = photoPath;
	QStringList headers; 
	MPForm form;
	form.addPair("auth_token", m_token);
	headers.append("auth_token="+ m_token);

	form.addPair("api_key", m_apikey);
	headers.append("api_key="+ m_apikey);
	
        QString ispublic=(info.is_public==1)?"1":"0";
	form.addPair("is_public",ispublic);
	headers.append("is_public="+ ispublic);
	
        QString isfamily=(info.is_family==1)?"1":"0";
	form.addPair("is_family",isfamily);
	headers.append("is_family="+ isfamily);
	
        QString isfriend=(info.is_friend==1)?"1":"0";
	form.addPair("is_friend",isfriend);
	headers.append("api_key="+ isfriend);

	QString tags=info.tags.join(" ");
	if(tags.length()>0){	
		form.addPair("tags",tags);
		headers.append("tags="+ tags);
	}

	if (!info.title.isEmpty()){
		form.addPair("title", info.title);
		headers.append("title="+ info.title);
	}

	if (!info.description.isEmpty()){
		form.addPair("description", info.description);
		headers.append("description="+ info.description);
	
	}

	QString md5=getApiSig(m_secret,headers);
	headers.append("api_sig="+ md5);
	QString queryStr=headers.join("&");
	QString completeUrl=url+queryStr;

	QImage image(photoPath);
	kdDebug() << "Add photo query"<<completeUrl<<endl;
	if (!image.isNull())
	{
		// image file - see if we need to rescale it
		if (rescale && (image.width() > maxDim || image.height() > maxDim))
		{
			image = image.smoothScale(maxDim, maxDim, QImage::ScaleMin);
			path = locateLocal("tmp", KURL(photoPath).filename());
			image.save(path, QImageIO::imageFormat(photoPath));
			kdDebug() << "Resizing and saving to temp file: "<< path << endl;
		}
	}

    if (!form.addFile("photo",path))
        return false;

    form.finish();

    KIO::TransferJob* job = KIO::http_post(completeUrl, form.formData(), false);
    job->addMetaData("content-type", form.contentType());
    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            SLOT(data(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(result(KIO::Job *)),
            SLOT(slotResult(KIO::Job *)));

    m_state = FE_ADDPHOTO;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy( true );
    return true;
}

void FlickrTalker::cancel()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    if (authProgressDlg && !authProgressDlg->isHidden()) 
	authProgressDlg->hide();
}	

void FlickrTalker::data(KIO::Job*, const QByteArray& data)
{
    if (data.isEmpty())
        return;

    int oldSize = m_buffer.size();
    m_buffer.resize(m_buffer.size() + data.size());
    memcpy(m_buffer.data()+oldSize, data.data(), data.size());
}

void FlickrTalker::slotError(const QString & error){
	 KMessageBox::warningContinueCancel( 0,
                                             i18n( "Error occured !")
                                             + error
                                             + i18n("\nDo you want to continue?" ) );
	kdDebug()<<"Not handling the error now will see it later"<<endl;
}

void FlickrTalker::slotResult(KIO::Job *job)
{
    m_job = 0;
    emit signalBusy( false );

    if ( job->error() )
    {
        if ( m_state == FE_ADDPHOTO )
            emit signalAddPhotoFailed( job->errorString() );
        else
            job->showErrorDialog( m_parent );
        return;
    }

    switch(m_state)
    {
    case(FE_LOGIN):
        //parseResponseLogin(m_buffer);
        break;
    case(FE_LISTALBUMS):
        parseResponseListAlbums(m_buffer);
        break;
    case(FE_GETFROB):
        parseResponseGetFrob(m_buffer);
        break;
    case(FE_GETTOKEN):
        parseResponseGetToken(m_buffer);
        break;
    case(FE_CHECKTOKEN):
        parseResponseCheckToken(m_buffer);
        break;
    case(FE_GETAUTHORIZED):
        //parseResponseGetToken(m_buffer);
        break;
    case(FE_LISTPHOTOS):
        parseResponseListPhotos(m_buffer);
        break;
    case(FE_GETPHOTOPROPERTY):
        parseResponsePhotoProperty(m_buffer);
        break;
    case(FE_ADDPHOTO):
        parseResponseAddPhoto(m_buffer);
        break;
    }
	/*
    if (m_state == FE_LOGIN && m_loggedIn)
    {
      //  listAlbums();
    }*/
}

void FlickrTalker::parseResponseGetFrob(const QByteArray &data)
{
    	bool success       = false;
	QString errorString;
	QDomDocument doc( "mydocument" );
	if ( !doc.setContent( data ) ) {
    	    return;
    	}
	QDomElement docElem = doc.documentElement();
    	QDomNode node = docElem.firstChild();
    	while( !node.isNull() ) {
	if ( node.isElement() && node.nodeName() == "frob" ) {
		QDomElement e = node.toElement(); // try to convert the node to an element.
       		kdDebug()<<"Frob is"<<e.text()<<endl; 
		m_frob=e.text();//this is what is obtained from data.
		success=true;
	}
	if ( node.isElement() && node.nodeName() == "error" ) {
		QDomElement e = node.toElement(); // try to convert the node to an element.
		errorString=(e.text());//this is what is obtained from data.
	}
        node = node.nextSibling();
    }
	kdDebug()<<"GetFrob finished"<<endl;
	authProgressDlg->setProgress(2,4);
	m_state = FE_GETAUTHORIZED;
	if(success)
	    	emit signalAuthenticate();
	else
		emit signalError(errorString);
}

void FlickrTalker::parseResponseCheckToken(const QByteArray &data)
{
	bool success=false;
	QString errString;
	QDomDocument doc( "checktoken" );
	if ( !doc.setContent( data ) ) {
    	    return;
    	}
	QDomElement docElem = doc.documentElement();
    	QDomNode node = docElem.firstChild();
	QDomElement e;
	while( !node.isNull() ) {
		if ( node.isElement() && node.nodeName() == "auth" ) {
			e = node.toElement(); // try to convert the node to an element.
			QDomNode details=e.firstChild();
			while(!details.isNull()){
				
				if(details.isElement()){
					e=details.toElement();
					if(details.nodeName()=="token"){
						kdDebug()<<"Token="<<e.text()<<endl; 
						m_token=e.text();//this is what is obtained from data.
					}
					if(details.nodeName()=="perms"){
						kdDebug()<<"Perms="<<e.text()<<endl; 
						QString	perms=e.text();//this is what is obtained from data.
						int valueOk=KMessageBox::questionYesNo(0, i18n("Your currently have \ 
									%1 permissions, \nWould you like to \
									proceed with current permissions ?\n[Upload requires Write permissions] ") 
								.arg( perms ));
						if(valueOk==KMessageBox::No){
							getFrob(); 
							return;
						}
						else{	
							authProgressDlg->hide();
							emit signalTokenObtained(m_token);
							
						}
					}
						if(details.nodeName()=="user"){
							kdDebug()<<"nsid="<<e.attribute("nsid")<<endl; 
							kdDebug()<<"username="<<e.attribute("username")<<endl; 
							kdDebug()<<"fullname="<<e.attribute("fullname")<<endl; 
						}
					}	
				details=details.nextSibling();
			}
				success=true;
		}
		if ( node.isElement() && node.nodeName() == "error" ) {
			errString =node.toElement().text();
		}
		node = node.nextSibling();
	}
	if(!success)
		emit signalError(errString);
	kdDebug()<<"CheckToken finished"<<endl;
}
void FlickrTalker::parseResponseGetToken(const QByteArray &data)
{
	bool success=false;
	QString errString;
	QDomDocument doc( "gettoken" );
	if ( !doc.setContent( data ) ) {
    	    return;
    	}
	QDomElement docElem = doc.documentElement();
    	QDomNode node = docElem.firstChild();
	QDomElement e;
	while( !node.isNull() ) {
		if ( node.isElement() && node.nodeName() == "auth" ) {
			e = node.toElement(); // try to convert the node to an element.
			QDomNode details=e.firstChild();
			while(!details.isNull()){
				
				if(details.isElement()){
					e=details.toElement();
					if(details.nodeName()=="token"){
						kdDebug()<<"Token="<<e.text()<<endl; 
						m_token=e.text();//this is what is obtained from data.
					}
					if(details.nodeName()=="perms"){
						kdDebug()<<"Perms="<<e.text()<<endl; 
					}	
					if(details.nodeName()=="user"){
						kdDebug()<<"nsid="<<e.attribute("nsid")<<endl; 
						kdDebug()<<"username="<<e.attribute("username")<<endl; 
						kdDebug()<<"fullname="<<e.attribute("fullname")<<endl; 
					}
				}
				details=details.nextSibling();
			}
			success=true;
		}
		if ( node.isElement() && node.nodeName() == "error" ) {
			errString=node.toElement().text();
		}
		node = node.nextSibling();
	}
	kdDebug()<<"GetToken finished"<<endl;
    	//emit signalBusy( false );
	authProgressDlg->hide();
	if(success)
	    	emit signalTokenObtained(m_token);
	else
		emit signalError(errString);
}
void FlickrTalker::parseResponseListAlbums(const QByteArray &data)
{
	QDomDocument doc( "getListAlbums" );
	if ( !doc.setContent( data ) ) {
    	    return;
    	}
	QDomElement docElem = doc.documentElement();
    	QDomNode node = docElem.firstChild();
	//QDomElement e;
	//To be implemented.
}

void FlickrTalker::parseResponseListPhotos(const QByteArray &data)
{
	QDomDocument doc( "getPhotosList" );
	if ( !doc.setContent( data ) ) {
    	    return;
    	}
	QDomElement docElem = doc.documentElement();
    	QDomNode node = docElem.firstChild();
	//QDomElement e;
	//To be implemented.
}

void FlickrTalker::parseResponseCreateAlbum(const QByteArray &data)
{
	QDomDocument doc( "getCreateAlbum" );
	if ( !doc.setContent( data ) ) {
    	    return;
    	}
	QDomElement docElem = doc.documentElement();
    	QDomNode node = docElem.firstChild();
	//QDomElement e;
	//To be implemented.
}

void FlickrTalker::parseResponseAddPhoto(const QByteArray &data)
{
    bool success=false;
    QString     line;
	QDomDocument doc( "AddPhoto Response" );
	if ( !doc.setContent( data ) ) {
    	    return;
    	}
	QDomElement docElem = doc.documentElement();
    	QDomNode node = docElem.firstChild();
	QDomElement e;
	while( !node.isNull() ) {
		if ( node.isElement() && node.nodeName() == "photoid" ) {
			e = node.toElement(); // try to convert the node to an element.
			QDomNode details=e.firstChild();
			kdDebug()<<"Photoid="<<e.text()<<endl; 
			success=true;
		}
		if ( node.isElement() && node.nodeName() == "error" ) {
			emit signalError(node.toElement().text());
		}
		node = node.nextSibling();
	}
	kdDebug()<<"GetToken finished"<<endl;
    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to upload photo"));
    }
    else
    {
        emit signalAddPhotoSucceeded();
    }

}
void FlickrTalker::parseResponsePhotoProperty(const QByteArray &data)
{
    bool success=false;
    QString     line;
	QDomDocument doc( "Photos Properties" );
	if ( !doc.setContent( data ) ) {
    	    return;
    	}
	QDomElement docElem = doc.documentElement();
    	QDomNode node = docElem.firstChild();
	QDomElement e;
	while( !node.isNull() ) {
		if ( node.isElement() && node.nodeName() == "photoid" ) {
			e = node.toElement(); // try to convert the node to an element.
			QDomNode details=e.firstChild();
			kdDebug()<<"Photoid="<<e.text()<<endl; 
			success=true;
		}
		if ( node.isElement() && node.nodeName() == "error" ) {
			emit signalError(node.toElement().text());
		}
		node = node.nextSibling();
	}
	kdDebug()<<"GetToken finished"<<endl;
    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to upload photo"));
    }
    else
    {
        emit signalAddPhotoSucceeded();
    }

}
}
#include "flickrtalker.moc"

