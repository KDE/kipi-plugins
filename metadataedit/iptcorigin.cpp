/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-13
 * Description : IPTC origin settings page.
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "iptcorigin.h"
#include "iptcorigin.moc"

// Qt includes

#include <QCheckBox>
#include <QLabel>
#include <QMap>
#include <QPushButton>
#include <QTimeEdit>
#include <QValidator>
#include <QGridLayout>

// KDE includes

#include <kaboutdata.h>
#include <kcombobox.h>
#include <kcomponentdata.h>
#include <kdatewidget.h>
#include <kdialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kseparator.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes

#include <libkdcraw/squeezedcombobox.h>

// Local includes

#include "metadatacheckbox.h"
#include "multivaluesedit.h"

using namespace KDcrawIface;

namespace KIPIMetadataEditPlugin
{

class IPTCOriginPriv
{
public:

    IPTCOriginPriv()
    {
        cityEdit               = 0;
        sublocationEdit        = 0;
        provinceEdit           = 0;
        locationEdit           = 0;
        cityCheck              = 0;
        sublocationCheck       = 0;
        provinceCheck          = 0;
        countryCheck           = 0;
        dateCreatedSel         = 0;
        dateDigitalizedSel     = 0;
        timeCreatedSel         = 0;
        timeDigitalizedSel     = 0;
        dateCreatedCheck       = 0;
        dateDigitalizedCheck   = 0;
        timeCreatedCheck       = 0;
        timeDigitalizedCheck   = 0;
        syncHOSTDateCheck      = 0;
        syncEXIFDateCheck      = 0;
        setTodayCreatedBtn     = 0;
        setTodayDigitalizedBtn = 0;

        // We cannot use KLocale::allCountriesList() here because KDE only
        // support 2 characters country codes. IPTC require 3 characters country
        // following ISO 3166 (http://userpage.chemie.fu-berlin.de/diverse/doc/ISO_3166.html)

        // Standard ISO 3166 country codes.

        countryCodeMap.insert( "AFG", i18n("Afghanistan") );
        countryCodeMap.insert( "ALB", i18n("Albania") );
        countryCodeMap.insert( "DZA", i18n("Algeria") );
        countryCodeMap.insert( "ASM", i18n("American Samoa") );
        countryCodeMap.insert( "AND", i18n("Andorra") );
        countryCodeMap.insert( "AGO", i18n("Angola") );
        countryCodeMap.insert( "AIA", i18n("Anguilla") );
        countryCodeMap.insert( "AGO", i18n("Angola") );
        countryCodeMap.insert( "ATA", i18n("Antarctica") );
        countryCodeMap.insert( "ATG", i18n("Antigua and Barbuda") );
        countryCodeMap.insert( "ARG", i18n("Argentina") );
        countryCodeMap.insert( "ARM", i18n("Armenia") );
        countryCodeMap.insert( "ABW", i18n("Aruba") );
        countryCodeMap.insert( "AUS", i18n("Australia") );
        countryCodeMap.insert( "AUT", i18n("Austria") );
        countryCodeMap.insert( "AZE", i18n("Azerbaijan") );
        countryCodeMap.insert( "BHS", i18n("Bahamas") );
        countryCodeMap.insert( "BHR", i18n("Bahrain") );
        countryCodeMap.insert( "BGD", i18n("Bangladesh") );
        countryCodeMap.insert( "BRB", i18n("Barbados") );
        countryCodeMap.insert( "BLR", i18n("Belarus") );
        countryCodeMap.insert( "BEL", i18n("Belgium") );
        countryCodeMap.insert( "BLZ", i18n("Belize") );
        countryCodeMap.insert( "BEN", i18n("Benin") );
        countryCodeMap.insert( "BMU", i18n("Bermuda") );
        countryCodeMap.insert( "BTN", i18n("Bhutan") );
        countryCodeMap.insert( "BOL", i18n("Bolivia") );
        countryCodeMap.insert( "BIH", i18n("Bosnia and Herzegovina") );
        countryCodeMap.insert( "BWA", i18n("Botswana") );
        countryCodeMap.insert( "BVT", i18n("Bouvet Island") );
        countryCodeMap.insert( "BRA", i18n("Brazil") );
        countryCodeMap.insert( "IOT", i18n("British Indian Ocean Territory") );
        countryCodeMap.insert( "VGB", i18n("British Virgin Islands") );
        countryCodeMap.insert( "BRN", i18n("Brunei Darussalam") );
        countryCodeMap.insert( "BGR", i18n("Bulgaria") );
        countryCodeMap.insert( "BFA", i18n("Burkina Faso") );
        countryCodeMap.insert( "BDI", i18n("Burundi") );
        countryCodeMap.insert( "KHM", i18n("Cambodia") );
        countryCodeMap.insert( "CMR", i18n("Cameroon") );
        countryCodeMap.insert( "CAN", i18n("Canada") );
        countryCodeMap.insert( "CPV", i18n("Cape Verde") );
        countryCodeMap.insert( "CYM", i18n("Cayman Islands") );
        countryCodeMap.insert( "CAF", i18n("Central African Republic") );
        countryCodeMap.insert( "TCD", i18n("Chad") );
        countryCodeMap.insert( "CHL", i18n("Chile") );
        countryCodeMap.insert( "CHN", i18n("China") );
        countryCodeMap.insert( "CXR", i18n("Christmas Island ") );
        countryCodeMap.insert( "CCK", i18n("Cocos Islands") );
        countryCodeMap.insert( "COL", i18n("Colombia") );
        countryCodeMap.insert( "COM", i18n("Comoros") );
        countryCodeMap.insert( "COD", i18n("Zaire") );
        countryCodeMap.insert( "COG", i18n("Congo") );
        countryCodeMap.insert( "COK", i18n("Cook Islands") );
        countryCodeMap.insert( "CRI", i18n("Costa Rica") );
        countryCodeMap.insert( "CIV", i18n("Ivory Coast") );
        countryCodeMap.insert( "CUB", i18n("Cuba") );
        countryCodeMap.insert( "CYP", i18n("Cyprus") );
        countryCodeMap.insert( "CZE", i18n("Czech Republic") );
        countryCodeMap.insert( "DNK", i18n("Denmark") );
        countryCodeMap.insert( "DJI", i18n("Djibouti") );
        countryCodeMap.insert( "DMA", i18n("Dominica") );
        countryCodeMap.insert( "DOM", i18n("Dominican Republic") );
        countryCodeMap.insert( "ECU", i18n("Ecuador") );
        countryCodeMap.insert( "EGY", i18n("Egypt") );
        countryCodeMap.insert( "SLV", i18n("El Salvador") );
        countryCodeMap.insert( "GNQ", i18n("Equatorial Guinea") );
        countryCodeMap.insert( "ERI", i18n("Eritrea") );
        countryCodeMap.insert( "EST", i18n("Estonia") );
        countryCodeMap.insert( "ETH", i18n("Ethiopia") );
        countryCodeMap.insert( "FRO", i18n("Faeroe Islands") );
        countryCodeMap.insert( "FLK", i18n("Falkland Islands") );
        countryCodeMap.insert( "FJI", i18n("Fiji Islands") );
        countryCodeMap.insert( "FIN", i18n("Finland") );
        countryCodeMap.insert( "FRA", i18n("France") );
        countryCodeMap.insert( "GUF", i18n("French Guiana") );
        countryCodeMap.insert( "PYF", i18n("French Polynesia") );
        countryCodeMap.insert( "ATF", i18n("French Southern Territories") );
        countryCodeMap.insert( "GAB", i18n("Gabon") );
        countryCodeMap.insert( "GMB", i18n("Gambia") );
        countryCodeMap.insert( "GEO", i18n("Georgia") );
        countryCodeMap.insert( "DEU", i18n("Germany") );
        countryCodeMap.insert( "GHA", i18n("Ghana") );
        countryCodeMap.insert( "GIB", i18n("Gibraltar") );
        countryCodeMap.insert( "GRC", i18n("Greece") );
        countryCodeMap.insert( "GRL", i18n("Greenland") );
        countryCodeMap.insert( "GRD", i18n("Grenada") );
        countryCodeMap.insert( "GLP", i18n("Guadaloupe") );
        countryCodeMap.insert( "GUM", i18n("Guam") );
        countryCodeMap.insert( "GTM", i18n("Guatemala") );
        countryCodeMap.insert( "GIN", i18n("Guinea") );
        countryCodeMap.insert( "GNB", i18n("Guinea-Bissau") );
        countryCodeMap.insert( "GUY", i18n("Guyana") );
        countryCodeMap.insert( "HTI", i18n("Haiti") );
        countryCodeMap.insert( "HMD", i18n("Heard and McDonald Islands") );
        countryCodeMap.insert( "VAT", i18n("Vatican") );
        countryCodeMap.insert( "HND", i18n("Honduras") );
        countryCodeMap.insert( "HKG", i18n("Hong Kong") );
        countryCodeMap.insert( "HRV", i18n("Croatia") );
        countryCodeMap.insert( "HUN", i18n("Hungary") );
        countryCodeMap.insert( "ISL", i18n("Iceland") );
        countryCodeMap.insert( "IND", i18n("India") );
        countryCodeMap.insert( "IDN", i18n("Indonesia") );
        countryCodeMap.insert( "IRN", i18n("Iran") );
        countryCodeMap.insert( "IRQ", i18n("Iraq") );
        countryCodeMap.insert( "IRL", i18n("Ireland") );
        countryCodeMap.insert( "ISR", i18n("Israel") );
        countryCodeMap.insert( "ITA", i18n("Italy") );
        countryCodeMap.insert( "JAM", i18n("Jamaica") );
        countryCodeMap.insert( "JPN", i18n("Japan") );
        countryCodeMap.insert( "JOR", i18n("Jordan") );
        countryCodeMap.insert( "KAZ", i18n("Kazakhstan") );
        countryCodeMap.insert( "KEN", i18n("Kenya") );
        countryCodeMap.insert( "KIR", i18n("Kiribati") );
        countryCodeMap.insert( "PRK", i18n("Korea") );
        countryCodeMap.insert( "KOR", i18n("Korea") );
        countryCodeMap.insert( "KWT", i18n("Kuwait") );
        countryCodeMap.insert( "KGZ", i18n("Kyrgyz Republic") );
        countryCodeMap.insert( "LAO", i18n("Lao") );
        countryCodeMap.insert( "LVA", i18n("Latvia") );
        countryCodeMap.insert( "LBN", i18n("Lebanon") );
        countryCodeMap.insert( "LSO", i18n("Lesotho") );
        countryCodeMap.insert( "LBR", i18n("Liberia") );
        countryCodeMap.insert( "LBY", i18n("Libyan Arab Jamahiriya") );
        countryCodeMap.insert( "LIE", i18n("Liechtenstein") );
        countryCodeMap.insert( "LTU", i18n("Lithuania") );
        countryCodeMap.insert( "LUX", i18n("Luxembourg") );
        countryCodeMap.insert( "MAC", i18n("Macao") );
        countryCodeMap.insert( "MKD", i18n("Macedonia") );
        countryCodeMap.insert( "MDG", i18n("Madagascar") );
        countryCodeMap.insert( "MWI", i18n("Malawi") );
        countryCodeMap.insert( "MYS", i18n("Malaysia") );
        countryCodeMap.insert( "MDV", i18n("Maldives") );
        countryCodeMap.insert( "MLI", i18n("Mali") );
        countryCodeMap.insert( "MLT", i18n("Malta") );
        countryCodeMap.insert( "MHL", i18n("Marshall Islands") );
        countryCodeMap.insert( "MTQ", i18n("Martinique") );
        countryCodeMap.insert( "MRT", i18n("Mauritania") );
        countryCodeMap.insert( "MUS", i18n("Mauritius") );
        countryCodeMap.insert( "MYT", i18n("Mayotte") );
        countryCodeMap.insert( "MEX", i18n("Mexico") );
        countryCodeMap.insert( "FSM", i18n("Micronesia") );
        countryCodeMap.insert( "MDA", i18n("Moldova") );
        countryCodeMap.insert( "MCO", i18n("Monaco") );
        countryCodeMap.insert( "MNG", i18n("Mongolia") );
        countryCodeMap.insert( "MSR", i18n("Montserrat") );
        countryCodeMap.insert( "MAR", i18n("Morocco") );
        countryCodeMap.insert( "MOZ", i18n("Mozambique") );
        countryCodeMap.insert( "MMR", i18n("Myanmar") );
        countryCodeMap.insert( "NAM", i18n("Namibia") );
        countryCodeMap.insert( "NRU", i18n("Nauru") );
        countryCodeMap.insert( "NPL", i18n("Nepal") );
        countryCodeMap.insert( "ANT", i18n("Netherlands Antilles") );
        countryCodeMap.insert( "NLD", i18n("Netherlands") );
        countryCodeMap.insert( "NCL", i18n("New Caledonia") );
        countryCodeMap.insert( "NZL", i18n("New Zealand") );
        countryCodeMap.insert( "NIC", i18n("Nicaragua") );
        countryCodeMap.insert( "NER", i18n("Niger") );
        countryCodeMap.insert( "NGA", i18n("Nigeria") );
        countryCodeMap.insert( "NIU", i18n("Niue") );
        countryCodeMap.insert( "NFK", i18n("Norfolk Island") );
        countryCodeMap.insert( "MNP", i18n("Northern Mariana Islands") );
        countryCodeMap.insert( "NOR", i18n("Norway") );
        countryCodeMap.insert( "OMN", i18n("Oman") );
        countryCodeMap.insert( "PAK", i18n("Pakistan") );
        countryCodeMap.insert( "PLW", i18n("Palau") );
        countryCodeMap.insert( "PSE", i18n("Palestinian Territory") );
        countryCodeMap.insert( "PAN", i18n("Panama") );
        countryCodeMap.insert( "PNG", i18n("Papua New Guinea") );
        countryCodeMap.insert( "PRY", i18n("Paraguay") );
        countryCodeMap.insert( "PER", i18n("Peru") );
        countryCodeMap.insert( "PHL", i18n("Philippines") );
        countryCodeMap.insert( "PCN", i18n("Pitcairn Island") );
        countryCodeMap.insert( "POL", i18n("Poland") );
        countryCodeMap.insert( "PRT", i18n("Portugal") );
        countryCodeMap.insert( "PRI", i18n("Puerto Rico") );
        countryCodeMap.insert( "QAT", i18n("Qatar") );
        countryCodeMap.insert( "REU", i18n("Reunion") );
        countryCodeMap.insert( "ROU", i18n("Romania") );
        countryCodeMap.insert( "RUS", i18n("Russian Federation") );
        countryCodeMap.insert( "RWA", i18n("Rwanda") );
        countryCodeMap.insert( "SHN", i18n("St. Helena") );
        countryCodeMap.insert( "KNA", i18n("St. Kitts and Nevis") );
        countryCodeMap.insert( "LCA", i18n("St. Lucia") );
        countryCodeMap.insert( "SPM", i18n("St. Pierre and Miquelon") );
        countryCodeMap.insert( "VCT", i18n("St. Vincent and the Grenadines") );
        countryCodeMap.insert( "WSM", i18n("Samoa") );
        countryCodeMap.insert( "SMR", i18n("San Marino") );
        countryCodeMap.insert( "STP", i18n("Sao Tome and Principe") );
        countryCodeMap.insert( "SAU", i18n("Saudi Arabia") );
        countryCodeMap.insert( "SEN", i18n("Senegal") );
        countryCodeMap.insert( "SCG", i18n("Serbia and Montenegro") );
        countryCodeMap.insert( "SYC", i18n("Seychelles") );
        countryCodeMap.insert( "SLE", i18n("Sierra Leone") );
        countryCodeMap.insert( "SGP", i18n("Singapore") );
        countryCodeMap.insert( "SVK", i18n("Slovakia") );
        countryCodeMap.insert( "SVN", i18n("Slovenia") );
        countryCodeMap.insert( "SLB", i18n("Solomon Islands") );
        countryCodeMap.insert( "SOM", i18n("Somalia") );
        countryCodeMap.insert( "ZAF", i18n("South Africa") );
        countryCodeMap.insert( "SGS", i18n("South Georgia and the South Sandwich Islands") );
        countryCodeMap.insert( "ESP", i18n("Spain") );
        countryCodeMap.insert( "LKA", i18n("Sri Lanka") );
        countryCodeMap.insert( "SDN", i18n("Sudan") );
        countryCodeMap.insert( "SUR", i18n("Suriname") );
        countryCodeMap.insert( "SJM", i18n("Svalbard & Jan Mayen Islands") );
        countryCodeMap.insert( "SWZ", i18n("Swaziland") );
        countryCodeMap.insert( "SWE", i18n("Sweden") );
        countryCodeMap.insert( "CHE", i18n("Switzerland") );
        countryCodeMap.insert( "SYR", i18n("Syrian Arab Republic") );
        countryCodeMap.insert( "TWN", i18n("Taiwan") );
        countryCodeMap.insert( "TJK", i18n("Tajikistan") );
        countryCodeMap.insert( "TZA", i18n("Tanzania") );
        countryCodeMap.insert( "THA", i18n("Thailand") );
        countryCodeMap.insert( "TLS", i18n("Timor-Leste") );
        countryCodeMap.insert( "TGO", i18n("Togo") );
        countryCodeMap.insert( "TKL", i18n("Tokelau Islands") );
        countryCodeMap.insert( "TON", i18n("Tonga") );
        countryCodeMap.insert( "TTO", i18n("Trinidad and Tobago") );
        countryCodeMap.insert( "TUN", i18n("Tunisia") );
        countryCodeMap.insert( "TUR", i18n("Turkey") );
        countryCodeMap.insert( "TKM", i18n("Turkmenistan") );
        countryCodeMap.insert( "TCA", i18n("Turks and Caicos Islands") );
        countryCodeMap.insert( "TUV", i18n("Tuvalu") );
        countryCodeMap.insert( "VIR", i18n("US Virgin Islands") );
        countryCodeMap.insert( "UGA", i18n("Uganda") );
        countryCodeMap.insert( "UKR", i18n("Ukraine") );
        countryCodeMap.insert( "ARE", i18n("United Arab Emirates") );
        countryCodeMap.insert( "GBR", i18n("United Kingdom") );
        countryCodeMap.insert( "UMI", i18n("United States Minor Outlying Islands") );
        countryCodeMap.insert( "USA", i18n("United States of America") );
        countryCodeMap.insert( "URY", i18n("Uruguay, Eastern Republic of") );
        countryCodeMap.insert( "UZB", i18n("Uzbekistan") );
        countryCodeMap.insert( "VUT", i18n("Vanuatu") );
        countryCodeMap.insert( "VEN", i18n("Venezuela") );
        countryCodeMap.insert( "VNM", i18n("Viet Nam") );
        countryCodeMap.insert( "WLF", i18n("Wallis and Futuna Islands ") );
        countryCodeMap.insert( "ESH", i18n("Western Sahara") );
        countryCodeMap.insert( "YEM", i18n("Yemen") );
        countryCodeMap.insert( "ZMB", i18n("Zambia") );
        countryCodeMap.insert( "ZWE", i18n("Zimbabwe") );

        // Supplemental IPTC country codes.

        countryCodeMap.insert( "XUN", i18n("United Nations") );
        countryCodeMap.insert( "XEU", i18n("European Union") );
        countryCodeMap.insert( "XSP", i18n("Space") );
        countryCodeMap.insert( "XSE", i18n("At Sea") );
        countryCodeMap.insert( "XIF", i18n("In Flight") );
        countryCodeMap.insert( "XEN", i18n("England") );
        countryCodeMap.insert( "XSC", i18n("Scotland") );
        countryCodeMap.insert( "XNI", i18n("Northern Ireland") );
        countryCodeMap.insert( "XWA", i18n("Wales") );
        countryCodeMap.insert( "PSE", i18n("Palestine") );
        countryCodeMap.insert( "GZA", i18n("Gaza") );
        countryCodeMap.insert( "JRO", i18n("Jericho") );
    }

    typedef QMap<QString, QString> CountryCodeMap;

    CountryCodeMap                 countryCodeMap;

    QCheckBox                     *dateCreatedCheck;
    QCheckBox                     *dateDigitalizedCheck;
    QCheckBox                     *timeCreatedCheck;
    QCheckBox                     *timeDigitalizedCheck;
    QCheckBox                     *syncHOSTDateCheck;
    QCheckBox                     *syncEXIFDateCheck;
    QCheckBox                     *cityCheck;
    QCheckBox                     *sublocationCheck;
    QCheckBox                     *provinceCheck;

    QTimeEdit                     *timeCreatedSel;
    QTimeEdit                     *timeDigitalizedSel;

    QPushButton                   *setTodayCreatedBtn;
    QPushButton                   *setTodayDigitalizedBtn;

    KDateWidget                   *dateCreatedSel;
    KDateWidget                   *dateDigitalizedSel;

    KLineEdit                     *cityEdit;
    KLineEdit                     *sublocationEdit;
    KLineEdit                     *provinceEdit;

    MultiValuesEdit               *locationEdit;

    MetadataCheckBox              *countryCheck;

    SqueezedComboBox              *countryCB;
};

IPTCOrigin::IPTCOrigin(QWidget* parent)
          : QWidget(parent), d(new IPTCOriginPriv)
{
    QGridLayout* grid = new QGridLayout(this);

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->dateDigitalizedCheck   = new QCheckBox(i18n("Digitization date"), this);
    d->timeDigitalizedCheck   = new QCheckBox(i18n("Digitization time"), this);
    d->dateDigitalizedSel     = new KDateWidget(this);
    d->timeDigitalizedSel     = new QTimeEdit(this);

    d->setTodayDigitalizedBtn = new QPushButton();
    d->setTodayDigitalizedBtn->setIcon(SmallIcon("go-jump-today"));
    d->setTodayDigitalizedBtn->setWhatsThis(i18n("Set digitization date to today"));

    d->dateDigitalizedSel->setWhatsThis(i18n("Set here the creation date of "
                                             "digital representation."));
    d->timeDigitalizedSel->setWhatsThis(i18n("Set here the creation time of "
                                             "digital representation."));

    slotSetTodayDigitalized();

    // --------------------------------------------------------

    d->dateCreatedCheck   = new QCheckBox(i18n("Creation date"), this);
    d->timeCreatedCheck   = new QCheckBox(i18n("Creation time"), this);
    d->dateCreatedSel     = new KDateWidget(this);
    d->timeCreatedSel     = new QTimeEdit(this);
    d->syncHOSTDateCheck  = new QCheckBox(i18n("Sync creation date hosted by %1",
                                               KGlobal::mainComponent().aboutData()->programName()),
                                               this);
    d->syncEXIFDateCheck  = new QCheckBox(i18n("Sync EXIF creation date"), this);

    d->setTodayCreatedBtn = new QPushButton();
    d->setTodayCreatedBtn->setIcon(SmallIcon("go-jump-today"));
    d->setTodayCreatedBtn->setWhatsThis(i18n("Set creation date to today"));

    d->dateCreatedSel->setWhatsThis(i18n("Set here the creation date of "
                                         "intellectual content."));
    d->timeCreatedSel->setWhatsThis(i18n("Set here the creation time of "
                                         "intellectual content."));

    slotSetTodayCreated();

    // --------------------------------------------------------

    d->locationEdit = new MultiValuesEdit(this, i18n("Location:"),
                          i18n("Set here the full country name referenced by the content."));

    QStringList list;
    for (IPTCOriginPriv::CountryCodeMap::Iterator it = d->countryCodeMap.begin();
         it != d->countryCodeMap.end(); ++it)
        list.append(QString("%1 - %2").arg(it.key()).arg(it.value()));

    d->locationEdit->setData(list);

    // --------------------------------------------------------

    d->cityCheck = new QCheckBox(i18n("City:"), this);
    d->cityEdit  = new KLineEdit(this);
    d->cityEdit->setClearButtonShown(true);
    d->cityEdit->setValidator(asciiValidator);
    d->cityEdit->setMaxLength(32);
    d->cityEdit->setWhatsThis(i18n("Set here the city of content origin. "
                                   "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->sublocationCheck = new QCheckBox(i18n("Sublocation:"), this);
    d->sublocationEdit  = new KLineEdit(this);
    d->sublocationEdit->setClearButtonShown(true);
    d->sublocationEdit->setValidator(asciiValidator);
    d->sublocationEdit->setMaxLength(32);
    d->sublocationEdit->setWhatsThis(i18n("Set here the content location within city. "
                                          "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->provinceCheck = new QCheckBox(i18n("State/Province:"), this);
    d->provinceEdit  = new KLineEdit(this);
    d->provinceEdit->setClearButtonShown(true);
    d->provinceEdit->setValidator(asciiValidator);
    d->provinceEdit->setMaxLength(32);
    d->provinceEdit->setWhatsThis(i18n("Set here the Province or State of content origin. "
                                       "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->countryCheck = new MetadataCheckBox(i18n("Country:"), this);
    d->countryCB    = new SqueezedComboBox(this);

    for (IPTCOriginPriv::CountryCodeMap::Iterator it = d->countryCodeMap.begin();
         it != d->countryCodeMap.end(); ++it)
        d->countryCB->addSqueezedItem(QString("%1 - %2").arg(it.key()).arg(it.value()));

    d->countryCB->model()->sort(0);
    d->countryCB->setWhatsThis(i18n("Select here country name of content origin."));

    // --------------------------------------------------------

    QLabel *note = new QLabel(i18n("<b>Note: "
                 "<b><a href='http://en.wikipedia.org/wiki/IPTC'>IPTC</a></b> "
                 "text tags only support the printable "
                 "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b> "
                 "characters and limit string sizes. "
                 "Use contextual help for details.</b>"), this);
    note->setOpenExternalLinks(true);
    note->setWordWrap(true);
    note->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    // --------------------------------------------------------

    grid->addWidget(d->dateDigitalizedCheck,                0, 0, 1, 2);
    grid->addWidget(d->timeDigitalizedCheck,                0, 2, 1, 2);
    grid->addWidget(d->dateDigitalizedSel,                  1, 0, 1, 2);
    grid->addWidget(d->timeDigitalizedSel,                  1, 2, 1, 1);
    grid->addWidget(d->setTodayDigitalizedBtn,              1, 4, 1, 1);
    grid->addWidget(d->dateCreatedCheck,                    2, 0, 1, 2);
    grid->addWidget(d->timeCreatedCheck,                    2, 2, 1, 2);
    grid->addWidget(d->dateCreatedSel,                      3, 0, 1, 2);
    grid->addWidget(d->timeCreatedSel,                      3, 2, 1, 1);
    grid->addWidget(d->setTodayCreatedBtn,                  3, 4, 1, 1);
    grid->addWidget(d->syncHOSTDateCheck,                   4, 0, 1, 5);
    grid->addWidget(d->syncEXIFDateCheck,                   5, 0, 1, 5);
    grid->addWidget(d->locationEdit,                        6, 0, 1, 5);
    grid->addWidget(new KSeparator(Qt::Horizontal, this),   7, 0, 1, 5);
    grid->addWidget(d->cityCheck,                           8, 0, 1, 1);
    grid->addWidget(d->cityEdit,                            8, 1, 1, 4);
    grid->addWidget(d->sublocationCheck,                    9, 0, 1, 1);
    grid->addWidget(d->sublocationEdit,                     9, 1, 1, 4);
    grid->addWidget(d->provinceCheck,                      10, 0, 1, 1);
    grid->addWidget(d->provinceEdit,                       10, 1, 1, 4);
    grid->addWidget(d->countryCheck,                       11, 0, 1, 1);
    grid->addWidget(d->countryCB,                          11, 1, 1, 4);
    grid->addWidget(note,                                  12, 0, 1, 5);
    grid->setColumnStretch(3, 10);
    grid->setRowStretch(13, 10);
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            d->dateCreatedSel, SLOT(setEnabled(bool)));

    connect(d->dateDigitalizedCheck, SIGNAL(toggled(bool)),
            d->dateDigitalizedSel, SLOT(setEnabled(bool)));

    connect(d->timeCreatedCheck, SIGNAL(toggled(bool)),
            d->timeCreatedSel, SLOT(setEnabled(bool)));

    connect(d->timeDigitalizedCheck, SIGNAL(toggled(bool)),
            d->timeDigitalizedSel, SLOT(setEnabled(bool)));

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            d->syncHOSTDateCheck, SLOT(setEnabled(bool)));

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            d->syncEXIFDateCheck, SLOT(setEnabled(bool)));

    connect(d->cityCheck, SIGNAL(toggled(bool)),
            d->cityEdit, SLOT(setEnabled(bool)));

    connect(d->sublocationCheck, SIGNAL(toggled(bool)),
            d->sublocationEdit, SLOT(setEnabled(bool)));

    connect(d->provinceCheck, SIGNAL(toggled(bool)),
            d->provinceEdit, SLOT(setEnabled(bool)));

    connect(d->countryCheck, SIGNAL(toggled(bool)),
            d->countryCB, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateDigitalizedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->timeCreatedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->timeDigitalizedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->cityCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->sublocationCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->provinceCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->countryCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->locationEdit, SIGNAL(signalModified()),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->dateCreatedSel, SIGNAL(changed(const QDate&)),
            this, SIGNAL(signalModified()));

    connect(d->dateDigitalizedSel, SIGNAL(changed(const QDate&)),
            this, SIGNAL(signalModified()));

    connect(d->timeCreatedSel, SIGNAL(timeChanged(const QTime &)),
            this, SIGNAL(signalModified()));

    connect(d->timeDigitalizedSel, SIGNAL(timeChanged(const QTime &)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->setTodayCreatedBtn, SIGNAL(clicked()),
            this, SLOT(slotSetTodayCreated()));

    connect(d->setTodayDigitalizedBtn, SIGNAL(clicked()),
            this, SLOT(slotSetTodayDigitalized()));

    // --------------------------------------------------------

    connect(d->countryCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->cityEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->sublocationEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->provinceEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));
}

IPTCOrigin::~IPTCOrigin()
{
    delete d;
}

void IPTCOrigin::slotSetTodayCreated()
{
    d->dateCreatedSel->setDate(QDate::currentDate());
    d->timeCreatedSel->setTime(QTime::currentTime());
}

void IPTCOrigin::slotSetTodayDigitalized()
{
    d->dateDigitalizedSel->setDate(QDate::currentDate());
    d->timeDigitalizedSel->setTime(QTime::currentTime());
}

bool IPTCOrigin::syncHOSTDateIsChecked()
{
    return d->syncHOSTDateCheck->isChecked();
}

bool IPTCOrigin::syncEXIFDateIsChecked()
{
    return d->syncEXIFDateCheck->isChecked();
}

void IPTCOrigin::setCheckedSyncHOSTDate(bool c)
{
    d->syncHOSTDateCheck->setChecked(c);
}

void IPTCOrigin::setCheckedSyncEXIFDate(bool c)
{
    d->syncEXIFDateCheck->setChecked(c);
}

QDateTime IPTCOrigin::getIPTCCreationDate()
{
    return QDateTime(d->dateCreatedSel->date(), d->timeCreatedSel->time());
}

void IPTCOrigin::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    QString     data;
    QStringList code, list;
    QDate       date;
    QTime       time;
    QString     dateStr, timeStr;

    dateStr = exiv2Iface.getIptcTagString("Iptc.Application2.DateCreated", false);
    timeStr = exiv2Iface.getIptcTagString("Iptc.Application2.TimeCreated", false);

    d->dateCreatedSel->setDate(QDate::currentDate());
    d->dateCreatedCheck->setChecked(false);
    if (!dateStr.isEmpty())
    {
        date = QDate::fromString(dateStr, Qt::ISODate);
        if (date.isValid())
        {
            d->dateCreatedSel->setDate(date);
            d->dateCreatedCheck->setChecked(true);
        }
    }
    d->dateCreatedSel->setEnabled(d->dateCreatedCheck->isChecked());
    d->syncHOSTDateCheck->setEnabled(d->dateCreatedCheck->isChecked());
    d->syncEXIFDateCheck->setEnabled(d->dateCreatedCheck->isChecked());

    d->timeCreatedSel->setTime(QTime::currentTime());
    d->timeCreatedCheck->setChecked(false);
    if (!timeStr.isEmpty())
    {
        time = QTime::fromString(timeStr, Qt::ISODate);
        if (time.isValid())
        {
            d->timeCreatedSel->setTime(time);
            d->timeCreatedCheck->setChecked(true);
        }
    }
    d->timeCreatedSel->setEnabled(d->timeCreatedCheck->isChecked());

    dateStr = exiv2Iface.getIptcTagString("Iptc.Application2.DigitizationDate", false);
    timeStr = exiv2Iface.getIptcTagString("Iptc.Application2.DigitizationTime", false);

    d->dateDigitalizedSel->setDate(QDate::currentDate());
    d->dateDigitalizedCheck->setChecked(false);
    if (!dateStr.isEmpty())
    {
        date = QDate::fromString(dateStr, Qt::ISODate);
        if (date.isValid())
        {
            d->dateDigitalizedSel->setDate(date);
            d->dateDigitalizedCheck->setChecked(true);
        }
    }
    d->dateDigitalizedSel->setEnabled(d->dateDigitalizedCheck->isChecked());

    d->timeDigitalizedSel->setTime(QTime::currentTime());
    d->timeDigitalizedCheck->setChecked(false);
    if (!timeStr.isEmpty())
    {
        time = QTime::fromString(timeStr, Qt::ISODate);
        if (time.isValid())
        {
            d->timeDigitalizedSel->setTime(time);
            d->timeDigitalizedCheck->setChecked(true);
        }
    }
    d->timeDigitalizedSel->setEnabled(d->timeDigitalizedCheck->isChecked());


    code = exiv2Iface.getIptcTagsStringList("Iptc.Application2.LocationCode", false);
    for (QStringList::Iterator it = code.begin(); it != code.end(); ++it)
    {
        QStringList data = d->locationEdit->getData();
        QStringList::Iterator it2;
        for (it2 = data.begin(); it2 != data.end(); ++it2)
        {
            if ((*it2).left(3) == (*it))
            {
                list.append(*it2);
                break;
            }
        }
        if (it2 == data.end())
            d->locationEdit->setValid(false);
    }
    d->locationEdit->setValues(list);

    d->cityEdit->clear();
    d->cityCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.City", false);
    if (!data.isNull())
    {
        d->cityEdit->setText(data);
        d->cityCheck->setChecked(true);
    }
    d->cityEdit->setEnabled(d->cityCheck->isChecked());

    d->sublocationEdit->clear();
    d->sublocationCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.SubLocation", false);
    if (!data.isNull())
    {
        d->sublocationEdit->setText(data);
        d->sublocationCheck->setChecked(true);
    }
    d->sublocationEdit->setEnabled(d->sublocationCheck->isChecked());

    d->provinceEdit->clear();
    d->provinceCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.ProvinceState", false);
    if (!data.isNull())
    {
        d->provinceEdit->setText(data);
        d->provinceCheck->setChecked(true);
    }
    d->provinceEdit->setEnabled(d->provinceCheck->isChecked());

    d->countryCB->setCurrentIndex(0);
    d->countryCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.CountryCode", false);
    if (!data.isNull())
    {
        int item = -1;
        for (int i = 0 ; i < d->countryCB->count() ; i++)
            if (d->countryCB->item(i).left(3) == data)
                item = i;

        if (item != -1)
        {
            d->countryCB->setCurrentIndex(item);
            d->countryCheck->setChecked(true);
        }
        else
            d->countryCheck->setValid(false);
    }
    d->countryCB->setEnabled(d->countryCheck->isChecked());

    blockSignals(false);
}

void IPTCOrigin::applyMetadata(QByteArray& exifData, QByteArray& iptcData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setExif(exifData);
    exiv2Iface.setIptc(iptcData);

    if (d->dateCreatedCheck->isChecked())
    {
        exiv2Iface.setIptcTagString("Iptc.Application2.DateCreated",
                                    getIPTCCreationDate().toString(Qt::ISODate));
        if (syncEXIFDateIsChecked())
        {
            exiv2Iface.setExifTagString("Exif.Image.DateTime",
                    getIPTCCreationDate().toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
        }
    }
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.DateCreated");

    if (d->dateDigitalizedCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.DigitizationDate",
                                    d->dateDigitalizedSel->date().toString(Qt::ISODate));
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.DigitizationDate");

    if (d->timeCreatedCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.TimeCreated",
                                    d->timeCreatedSel->time().toString(Qt::ISODate));
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.TimeCreated");

    if (d->timeDigitalizedCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.DigitizationTime",
                                    d->timeDigitalizedSel->time().toString(Qt::ISODate));
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.DigitizationTime");

    QStringList oldList, newList;
    if (d->locationEdit->getValues(oldList, newList))
    {
        QStringList oldCode, newCode, oldName, newName;

        for (QStringList::Iterator it = oldList.begin(); it != oldList.end(); ++it)
        {
            oldCode.append((*it).left(3));
            oldName.append((*it).mid(6));
        }

        for (QStringList::Iterator it2 = newList.begin(); it2 != newList.end(); ++it2)
        {
            newCode.append((*it2).left(3));
            newName.append((*it2).mid(6));
        }

        exiv2Iface.setIptcTagsStringList("Iptc.Application2.LocationCode", 3, oldCode, newCode);
        exiv2Iface.setIptcTagsStringList("Iptc.Application2.LocationName", 64, oldName, newName);
    }
    else
    {
        exiv2Iface.removeIptcTag("Iptc.Application2.LocationCode");
        exiv2Iface.removeIptcTag("Iptc.Application2.LocationName");
    }

    if (d->cityCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.City", d->cityEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.City");

    if (d->sublocationCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.SubLocation", d->sublocationEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.SubLocation");

    if (d->provinceCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.ProvinceState", d->provinceEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.ProvinceState");

    if (d->countryCheck->isChecked())
    {
        QString countryName = d->countryCB->itemHighlighted().mid(6);
        QString countryCode = d->countryCB->itemHighlighted().left(3);
        exiv2Iface.setIptcTagString("Iptc.Application2.CountryCode", countryCode);
        exiv2Iface.setIptcTagString("Iptc.Application2.CountryName", countryName);
    }
    else if (d->countryCheck->isValid())
    {
        exiv2Iface.removeIptcTag("Iptc.Application2.CountryCode");
        exiv2Iface.removeIptcTag("Iptc.Application2.CountryName");
    }

    exifData = exiv2Iface.getExif();
    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin
