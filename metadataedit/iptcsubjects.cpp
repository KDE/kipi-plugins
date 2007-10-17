/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-15
 * Description : IPTC subjects settings page.
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// QT includes.

#include <QMap>
#include <QLayout>
#include <QLabel>
#include <QValidator>
#include <QCheckBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QComboBox>

// KDE includes.

#include <khbox.h>
#include <klocale.h>
#include <kdialog.h>
#include <klistwidget.h>
#include <klineedit.h>
#include <kiconloader.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "iptcsubjects.h"
#include "iptcsubjects.moc"

namespace KIPIMetadataEditPlugin
{

class SubjectData
{
public:

    SubjectData(const QString& n, const QString& m, const QString& d)
    {
        name        = n;
        matter      = m;
        detail      = d;
    }

    QString name;         // English and Ascii Name of subject.
    QString matter;       // English and Ascii Matter Name of subject.
    QString detail;       // English and Ascii Detail Name of subject.
};

class IPTCSubjectsPriv
{
public:

    enum EditionMode 
    {
        STANDARD = 0,
        CUSTOM
    };

    IPTCSubjectsPriv()
    {
        addSubjectButton = 0;
        delSubjectButton = 0;
        repSubjectButton = 0;
        subjectsBox      = 0;
        subjectsCheck    = 0;
        iprEdit          = 0;
        refEdit          = 0;
        nameEdit         = 0;
        matterEdit       = 0;
        detailEdit       = 0;
        iprLabel         = 0;
        refLabel         = 0;
        nameLabel        = 0;
        matterLabel      = 0;
        detailLabel      = 0;
        btnGroup         = 0;
        stdBtn           = 0;
        customBtn        = 0;
        refCB            = 0;
        optionsBox       = 0;

        // Subject Codes map contents defined into IPTC/IIM spec.
        // NOTE: Strings are not internationalized following IPTC spec. page 29.
        // http://www.iptc.org/std/IIM/4.1/specification/IIMV4.1.pdf
        // The map is defined like this:
        // Reference Number, Subject Name, Subject Matter Name, and Subject Detail Name.

        subMap.insert("01000000", SubjectData("Arts, Culture & Entertainment", "", ""));
        subMap.insert("01001000", SubjectData("Arts, Culture & Entertainment", "Archaeology", ""));
        subMap.insert("01002000", SubjectData("Arts, Culture & Entertainment", "Architecture", ""));
        subMap.insert("01003000", SubjectData("Arts, Culture & Entertainment", "Bullfighting", ""));
        subMap.insert("01004000", SubjectData("Arts, Culture & Entertainment", "Carnival", ""));
        subMap.insert("01005000", SubjectData("Arts, Culture & Entertainment", "Cinema", ""));
        subMap.insert("01006000", SubjectData("Arts, Culture & Entertainment", "Dance", ""));
        subMap.insert("01007000", SubjectData("Arts, Culture & Entertainment", "Fashion", ""));
        subMap.insert("01008000", SubjectData("Arts, Culture & Entertainment", "Language", ""));
        subMap.insert("01009000", SubjectData("Arts, Culture & Entertainment", "Libraries & Museums", ""));
        subMap.insert("01010000", SubjectData("Arts, Culture & Entertainment", "Literature", ""));
        subMap.insert("01011000", SubjectData("Arts, Culture & Entertainment", "Music", ""));
        subMap.insert("01012000", SubjectData("Arts, Culture & Entertainment", "Painting", ""));
        subMap.insert("01013000", SubjectData("Arts, Culture & Entertainment", "Photography", ""));
        subMap.insert("01014000", SubjectData("Arts, Culture & Entertainment", "Radio", ""));
        subMap.insert("01015000", SubjectData("Arts, Culture & Entertainment", "Sculpture", ""));
        subMap.insert("01016000", SubjectData("Arts, Culture & Entertainment", "Television", ""));
        subMap.insert("01017000", SubjectData("Arts, Culture & Entertainment", "Theatre", ""));

        subMap.insert("02000000", SubjectData("Crime, Law & Justice", "", ""));
        subMap.insert("02001000", SubjectData("Crime, Law & Justice", "Crime", ""));
        subMap.insert("02002000", SubjectData("Crime, Law & Justice", "Judiciary", ""));
        subMap.insert("02003000", SubjectData("Crime, Law & Justice", "Police", ""));
        subMap.insert("02004000", SubjectData("Crime, Law & Justice", "Punishment", ""));
        subMap.insert("02005000", SubjectData("Crime, Law & Justice", "Prison", ""));

        subMap.insert("03000000", SubjectData("Disasters & Accidents", "", ""));
        subMap.insert("03001000", SubjectData("Disasters & Accidents", "Drought", ""));
        subMap.insert("03002000", SubjectData("Disasters & Accidents", "Earthquake", ""));
        subMap.insert("03003000", SubjectData("Disasters & Accidents", "Famine", ""));
        subMap.insert("03004000", SubjectData("Disasters & Accidents", "Fire", ""));
        subMap.insert("03005000", SubjectData("Disasters & Accidents", "Flood", ""));
        subMap.insert("03006000", SubjectData("Disasters & Accidents", "Industrial accident", ""));
        subMap.insert("03007000", SubjectData("Disasters & Accidents", "Meteorological disaster", ""));
        subMap.insert("03008000", SubjectData("Disasters & Accidents", "Nuclear accident", ""));
        subMap.insert("03009000", SubjectData("Disasters & Accidents", "Pollution", ""));
        subMap.insert("03010000", SubjectData("Disasters & Accidents", "Transport accident", ""));
        subMap.insert("03011000", SubjectData("Disasters & Accidents", "Volcanic eruption", ""));

        subMap.insert("04000000", SubjectData("Economy, Business & Finance", "", ""));
        subMap.insert("04001000", SubjectData("Economy, Business & Finance", "Agriculture", ""));
        subMap.insert("04001001", SubjectData("Economy, Business & Finance", "Agriculture", "Arable Farming"));
        subMap.insert("04001002", SubjectData("Economy, Business & Finance", "Agriculture", "Fishing Industry"));
        subMap.insert("04001003", SubjectData("Economy, Business & Finance", "Agriculture", "Forestry & Timber"));
        subMap.insert("04001004", SubjectData("Economy, Business & Finance", "Agriculture", "Livestock Farming"));
        subMap.insert("04002000", SubjectData("Economy, Business & Finance", "Chemicals", ""));
        subMap.insert("04002001", SubjectData("Economy, Business & Finance", "Chemicals", "Biotechnology"));
        subMap.insert("04002002", SubjectData("Economy, Business & Finance", "Chemicals", "Fertilisers"));
        subMap.insert("04002003", SubjectData("Economy, Business & Finance", "Chemicals", "Health & Beauty products"));
        subMap.insert("04002004", SubjectData("Economy, Business & Finance", "Chemicals", "Inorganic chemicals"));
        subMap.insert("04002005", SubjectData("Economy, Business & Finance", "Chemicals", "Organic chemicals"));
        subMap.insert("04002006", SubjectData("Economy, Business & Finance", "Chemicals", "Pharmaceuticals"));
        subMap.insert("04002007", SubjectData("Economy, Business & Finance", "Chemicals", "Synthetics & Plastics"));
        subMap.insert("04003000", SubjectData("Economy, Business & Finance", "Computing & Information Technology", ""));
        subMap.insert("04003001", SubjectData("Economy, Business & Finance", "Computing & Information Technology", "Hardware"));
        subMap.insert("04003002", SubjectData("Economy, Business & Finance", "Computing & Information Technology", "Networking"));
        subMap.insert("04003003", SubjectData("Economy, Business & Finance", "Computing & Information Technology", "Satellite technology"));
        subMap.insert("04003004", SubjectData("Economy, Business & Finance", "Computing & Information Technology", "Semiconductors & active components"));
        subMap.insert("04003005", SubjectData("Economy, Business & Finance", "Computing & Information Technology", "Software"));
        subMap.insert("04003006", SubjectData("Economy, Business & Finance", "Computing & Information Technology", "Telecommunications Equipment"));
        subMap.insert("04003007", SubjectData("Economy, Business & Finance", "Computing & Information Technology", "Telecommunications Services"));
        subMap.insert("04004000", SubjectData("Economy, Business & Finance", "Construction & Property", ""));
        subMap.insert("04004001", SubjectData("Economy, Business & Finance", "Construction & Property", "Heavy construction"));
        subMap.insert("04004002", SubjectData("Economy, Business & Finance", "Construction & Property", "House building"));
        subMap.insert("04004003", SubjectData("Economy, Business & Finance", "Construction & Property", "Real Estate"));
        subMap.insert("04005000", SubjectData("Economy, Business & Finance", "Energy & Resources", ""));
        subMap.insert("04005001", SubjectData("Economy, Business & Finance", "Energy & Resources", "Alternative energy"));
        subMap.insert("04005002", SubjectData("Economy, Business & Finance", "Energy & Resources", "Coal"));
        subMap.insert("04005003", SubjectData("Economy, Business & Finance", "Energy & Resources", "Oil & Gas - Downstream activities"));
        subMap.insert("04005004", SubjectData("Economy, Business & Finance", "Energy & Resources", "Oil & Gas - Upstream activities"));
        subMap.insert("04005005", SubjectData("Economy, Business & Finance", "Energy & Resources", "Nuclear power"));
        subMap.insert("04005006", SubjectData("Economy, Business & Finance", "Energy & Resources", "Electricity Production & Distribution"));
        subMap.insert("04005007", SubjectData("Economy, Business & Finance", "Energy & Resources", "Waste Management & Pollution Control"));
        subMap.insert("04005008", SubjectData("Economy, Business & Finance", "Energy & Resources", "Water Supply"));
        subMap.insert("04006000", SubjectData("Economy, Business & Finance", "Financial & Business Services", ""));
        subMap.insert("04006001", SubjectData("Economy, Business & Finance", "Financial & Business Services", "Accountancy & Auditing"));
        subMap.insert("04006002", SubjectData("Economy, Business & Finance", "Financial & Business Services", "Banking"));
        subMap.insert("04006003", SubjectData("Economy, Business & Finance", "Financial & Business Services", "Consultancy Services"));
        subMap.insert("04006004", SubjectData("Economy, Business & Finance", "Financial & Business Services", "Employment Agencies"));
        subMap.insert("04006005", SubjectData("Economy, Business & Finance", "Financial & Business Services", "Healthcare Providers"));
        subMap.insert("04006006", SubjectData("Economy, Business & Finance", "Financial & Business Services", "Insurance"));
        subMap.insert("04006007", SubjectData("Economy, Business & Finance", "Financial & Business Services", "Legal services"));
        subMap.insert("04006008", SubjectData("Economy, Business & Finance", "Financial & Business Services", "Market research"));
        subMap.insert("04006009", SubjectData("Economy, Business & Finance", "Financial & Business Services", "Stock broking"));
        subMap.insert("04007000", SubjectData("Economy, Business & Finance", "Goods Distribution", ""));
        subMap.insert("04007001", SubjectData("Economy, Business & Finance", "Goods Distribution", "Clothing"));
        subMap.insert("04007002", SubjectData("Economy, Business & Finance", "Goods Distribution", "Department stores"));
        subMap.insert("04007003", SubjectData("Economy, Business & Finance", "Goods Distribution", "Food"));
        subMap.insert("04007004", SubjectData("Economy, Business & Finance", "Goods Distribution", "Mail Order"));
        subMap.insert("04007005", SubjectData("Economy, Business & Finance", "Goods Distribution", "Retail"));
        subMap.insert("04007006", SubjectData("Economy, Business & Finance", "Goods Distribution", "Speciality stores"));
        subMap.insert("04007007", SubjectData("Economy, Business & Finance", "Goods Distribution", "Wholesale"));
        subMap.insert("04008000", SubjectData("Economy, Business & Finance", "Macro Economics", ""));
        subMap.insert("04008001", SubjectData("Economy, Business & Finance", "Macro Economics", "Central Banks"));
        subMap.insert("04008002", SubjectData("Economy, Business & Finance", "Macro Economics", "Consumer Issues"));
        subMap.insert("04008003", SubjectData("Economy, Business & Finance", "Macro Economics", "Debt Markets"));
        subMap.insert("04008004", SubjectData("Economy, Business & Finance", "Macro Economics", "Economic Indicators"));
        subMap.insert("04008005", SubjectData("Economy, Business & Finance", "Macro Economics", "Emerging Markets Debt"));
        subMap.insert("04008006", SubjectData("Economy, Business & Finance", "Macro Economics", "Foreign Exchange Markets"));
        subMap.insert("04008007", SubjectData("Economy, Business & Finance", "Macro Economics", "Government Aid"));
        subMap.insert("04008008", SubjectData("Economy, Business & Finance", "Macro Economics", "Government Debt"));
        subMap.insert("04008009", SubjectData("Economy, Business & Finance", "Macro Economics", "Interest Rates"));
        subMap.insert("04008010", SubjectData("Economy, Business & Finance", "Macro Economics", "International Economic Institutions"));
        subMap.insert("04008011", SubjectData("Economy, Business & Finance", "Macro Economics", "International Trade Issues"));
        subMap.insert("04008012", SubjectData("Economy, Business & Finance", "Macro Economics", "Loan Markets"));
        subMap.insert("04009000", SubjectData("Economy, Business & Finance", "Markets", ""));
        subMap.insert("04009001", SubjectData("Economy, Business & Finance", "Markets", "Energy"));
        subMap.insert("04009002", SubjectData("Economy, Business & Finance", "Markets", "Metals"));
        subMap.insert("04009003", SubjectData("Economy, Business & Finance", "Markets", "Securities"));
        subMap.insert("04009004", SubjectData("Economy, Business & Finance", "Markets", "Soft Commodities"));
        subMap.insert("04010000", SubjectData("Economy, Business & Finance", "Media", ""));
        subMap.insert("04010001", SubjectData("Economy, Business & Finance", "Media", "Advertising"));
        subMap.insert("04010002", SubjectData("Economy, Business & Finance", "Media", "Books"));
        subMap.insert("04010003", SubjectData("Economy, Business & Finance", "Media", "Cinema"));
        subMap.insert("04010004", SubjectData("Economy, Business & Finance", "Media", "News Agencies"));
        subMap.insert("04010005", SubjectData("Economy, Business & Finance", "Media", "Newspaper & Magazines"));
        subMap.insert("04010006", SubjectData("Economy, Business & Finance", "Media", "Online"));
        subMap.insert("04010007", SubjectData("Economy, Business & Finance", "Media", "Public Relations"));
        subMap.insert("04010008", SubjectData("Economy, Business & Finance", "Media", "Radio"));
        subMap.insert("04010009", SubjectData("Economy, Business & Finance", "Media", "Satellite & Cable Services"));
        subMap.insert("04010010", SubjectData("Economy, Business & Finance", "Media", "Television"));
        subMap.insert("04011000", SubjectData("Economy, Business & Finance", "Metal Goods & Engineering", ""));
        subMap.insert("04011001", SubjectData("Economy, Business & Finance", "Metal Goods & Engineering", "Aerospace"));
        subMap.insert("04011002", SubjectData("Economy, Business & Finance", "Metal Goods & Engineering", "Automotive Equipment"));
        subMap.insert("04011003", SubjectData("Economy, Business & Finance", "Metal Goods & Engineering", "Defence Equipment"));
        subMap.insert("04011004", SubjectData("Economy, Business & Finance", "Metal Goods & Engineering", "Electrical Appliances"));
        subMap.insert("04011005", SubjectData("Economy, Business & Finance", "Metal Goods & Engineering", "Heavy engineering"));
        subMap.insert("04011006", SubjectData("Economy, Business & Finance", "Metal Goods & Engineering", "Industrial components"));
        subMap.insert("04011007", SubjectData("Economy, Business & Finance", "Metal Goods & Engineering", "Instrument engineering"));
        subMap.insert("04011008", SubjectData("Economy, Business & Finance", "Metal Goods & Engineering", "Shipbuilding"));
        subMap.insert("04012000", SubjectData("Economy, Business & Finance", "Metals & Minerals", ""));
        subMap.insert("04012001", SubjectData("Economy, Business & Finance", "Metals & Minerals", "Building materials"));
        subMap.insert("04012002", SubjectData("Economy, Business & Finance", "Metals & Minerals", "Gold & Precious Materials"));
        subMap.insert("04012003", SubjectData("Economy, Business & Finance", "Metals & Minerals", "Iron & Steel"));
        subMap.insert("04012004", SubjectData("Economy, Business & Finance", "Metals & Minerals", "Non ferrous metals"));
        subMap.insert("04013000", SubjectData("Economy, Business & Finance", "Process Industries", ""));
        subMap.insert("04013001", SubjectData("Economy, Business & Finance", "Process Industries", "Alcoholic Drinks"));
        subMap.insert("04013002", SubjectData("Economy, Business & Finance", "Process Industries", "Food"));
        subMap.insert("04013003", SubjectData("Economy, Business & Finance", "Process Industries", "Furnishings & Furniture"));
        subMap.insert("04013004", SubjectData("Economy, Business & Finance", "Process Industries", "Paper & packaging products"));
        subMap.insert("04013005", SubjectData("Economy, Business & Finance", "Process Industries", "Rubber products"));
        subMap.insert("04013006", SubjectData("Economy, Business & Finance", "Process Industries", "Soft Drinks"));
        subMap.insert("04013007", SubjectData("Economy, Business & Finance", "Process Industries", "Textiles & Clothing"));
        subMap.insert("04013008", SubjectData("Economy, Business & Finance", "Process Industries", "Tobacco"));
        subMap.insert("04014000", SubjectData("Economy, Business & Finance", "Tourism & Leisure", ""));
        subMap.insert("04014001", SubjectData("Economy, Business & Finance", "Tourism & Leisure", "Casinos & Gambling"));
        subMap.insert("04014002", SubjectData("Economy, Business & Finance", "Tourism & Leisure", "Hotels & accommodation"));
        subMap.insert("04014003", SubjectData("Economy, Business & Finance", "Tourism & Leisure", "Recreational & Sports goods"));
        subMap.insert("04014004", SubjectData("Economy, Business & Finance", "Tourism & Leisure", "Restaurants & catering"));
        subMap.insert("04014005", SubjectData("Economy, Business & Finance", "Tourism & Leisure", "Tour operators"));
        subMap.insert("04015000", SubjectData("Economy, Business & Finance", "Transport", ""));
        subMap.insert("04015001", SubjectData("Economy, Business & Finance", "Transport", "Air Transport"));
        subMap.insert("04015002", SubjectData("Economy, Business & Finance", "Transport", "Railway"));
        subMap.insert("04015003", SubjectData("Economy, Business & Finance", "Transport", "Road Transport"));
        subMap.insert("04015004", SubjectData("Economy, Business & Finance", "Transport", "Waterway & Maritime Transport"));

        subMap.insert("05000000", SubjectData("Education", "", ""));
        subMap.insert("05001000", SubjectData("Education", "Adult Education", ""));
        subMap.insert("05002000", SubjectData("Education", "Further Education", ""));
        subMap.insert("05003000", SubjectData("Education", "Parent Organisations", ""));
        subMap.insert("05004000", SubjectData("Education", "Preschooling", ""));
        subMap.insert("05005000", SubjectData("Education", "Schools", ""));
        subMap.insert("05006000", SubjectData("Education", "Teachers Unions", ""));
        subMap.insert("05007000", SubjectData("Education", "University", ""));

        subMap.insert("06000000", SubjectData("Environmental Issues", "", ""));
        subMap.insert("06001000", SubjectData("Environmental Issues", "Alternative Energy", ""));
        subMap.insert("06002000", SubjectData("Environmental Issues", "Conservation", ""));
        subMap.insert("06003000", SubjectData("Environmental Issues", "Energy Savings", ""));
        subMap.insert("06004000", SubjectData("Environmental Issues", "Environmental Politics", ""));
        subMap.insert("06005000", SubjectData("Environmental Issues", "Environmental pollution", ""));
        subMap.insert("06006000", SubjectData("Environmental Issues", "Natural resources", ""));
        subMap.insert("06007000", SubjectData("Environmental Issues", "Nature", ""));
        subMap.insert("06008000", SubjectData("Environmental Issues", "Population", ""));
        subMap.insert("06009000", SubjectData("Environmental Issues", "Waste", ""));
        subMap.insert("06010000", SubjectData("Environmental Issues", "Water Supplies", ""));

        subMap.insert("07000000", SubjectData("Health", "", ""));
        subMap.insert("07001000", SubjectData("Health", "Diseases", ""));
        subMap.insert("07002000", SubjectData("Health", "Epidemic & Plague", ""));
        subMap.insert("07003000", SubjectData("Health", "Health treatment", ""));
        subMap.insert("07004000", SubjectData("Health", "Health organisations", ""));
        subMap.insert("07005000", SubjectData("Health", "Medical research", ""));
        subMap.insert("07006000", SubjectData("Health", "Medical staff", ""));
        subMap.insert("07007000", SubjectData("Health", "Medicines", ""));
        subMap.insert("07008000", SubjectData("Health", "Preventative medicine", ""));

        subMap.insert("08000000", SubjectData("Human Interest", "", ""));
        subMap.insert("08001000", SubjectData("Human Interest", "Animals", ""));
        subMap.insert("08002000", SubjectData("Human Interest", "Curiosities", ""));
        subMap.insert("08003000", SubjectData("Human Interest", "People", ""));

        subMap.insert("09000000", SubjectData("Labour", "", ""));
        subMap.insert("09001000", SubjectData("Labour", "Apprentices", ""));
        subMap.insert("09002000", SubjectData("Labour", "Collective contracts", ""));
        subMap.insert("09003000", SubjectData("Labour", "Employment", ""));
        subMap.insert("09004000", SubjectData("Labour", "Labour dispute", ""));
        subMap.insert("09005000", SubjectData("Labour", "Labour legislation", ""));
        subMap.insert("09006000", SubjectData("Labour", "Retirement", ""));
        subMap.insert("09007000", SubjectData("Labour", "Retraining", ""));
        subMap.insert("09008000", SubjectData("Labour", "Strike", ""));
        subMap.insert("09009000", SubjectData("Labour", "Unemployment", ""));
        subMap.insert("09010000", SubjectData("Labour", "Unions", ""));
        subMap.insert("09011000", SubjectData("Labour", "Wages & Pensions", ""));
        subMap.insert("09012000", SubjectData("Labour", "Work Relations", ""));

        subMap.insert("10000000", SubjectData("Lifestyle & Leisure", "", ""));
        subMap.insert("10001000", SubjectData("Lifestyle & Leisure", "Games", ""));
        subMap.insert("10002000", SubjectData("Lifestyle & Leisure", "Gaming & Lotteries", ""));
        subMap.insert("10003000", SubjectData("Lifestyle & Leisure", "Gastronomy", ""));
        subMap.insert("10004000", SubjectData("Lifestyle & Leisure", "Hobbies", ""));
        subMap.insert("10005000", SubjectData("Lifestyle & Leisure", "Holidays or vacations", ""));
        subMap.insert("10006000", SubjectData("Lifestyle & Leisure", "Tourism", ""));

        subMap.insert("11000000", SubjectData("Politics", "", ""));
        subMap.insert("11001000", SubjectData("Politics", "Defence", ""));
        subMap.insert("11002000", SubjectData("Politics", "Diplomacy", ""));
        subMap.insert("11003000", SubjectData("Politics", "Elections", ""));
        subMap.insert("11004000", SubjectData("Politics", "Espionage & Intelligence", ""));
        subMap.insert("11005000", SubjectData("Politics", "Foreign Aid", ""));
        subMap.insert("11006000", SubjectData("Politics", "Government", ""));
        subMap.insert("11007000", SubjectData("Politics", "Human Rights", ""));
        subMap.insert("11008000", SubjectData("Politics", "Local authorities", ""));
        subMap.insert("11009000", SubjectData("Politics", "Parliament", ""));
        subMap.insert("11010000", SubjectData("Politics", "Parties", ""));
        subMap.insert("11011000", SubjectData("Politics", "Refugees", ""));
        subMap.insert("11012000", SubjectData("Politics", "Regional authorities", ""));
        subMap.insert("11013000", SubjectData("Politics", "State Budget", ""));
        subMap.insert("11014000", SubjectData("Politics", "Treaties & Organisations", ""));

        subMap.insert("12000000", SubjectData("Religion & Belief", "", ""));
        subMap.insert("12001000", SubjectData("Religion & Belief", "Cults & sects", ""));
        subMap.insert("12002000", SubjectData("Religion & Belief", "Faith", ""));
        subMap.insert("12003000", SubjectData("Religion & Belief", "Free masonry", ""));
        subMap.insert("12004000", SubjectData("Religion & Belief", "Religious institutions", ""));

        subMap.insert("13000000", SubjectData("Science & Technology", "", ""));
        subMap.insert("13001000", SubjectData("Science & Technology", "Applied Sciences", ""));
        subMap.insert("13002000", SubjectData("Science & Technology", "Engineering", ""));
        subMap.insert("13003000", SubjectData("Science & Technology", "Human Sciences", ""));
        subMap.insert("13004000", SubjectData("Science & Technology", "Natural Sciences", ""));
        subMap.insert("13005000", SubjectData("Science & Technology", "Philosophical Sciences", ""));
        subMap.insert("13006000", SubjectData("Science & Technology", "Research", ""));
        subMap.insert("13007000", SubjectData("Science & Technology", "Scientific exploration", ""));
        subMap.insert("13008000", SubjectData("Science & Technology", "Space programmes", ""));

        subMap.insert("14000000", SubjectData("Social Issues", "", ""));
        subMap.insert("14001000", SubjectData("Social Issues", "Addiction", ""));
        subMap.insert("14002000", SubjectData("Social Issues", "Charity", ""));
        subMap.insert("14003000", SubjectData("Social Issues", "Demographics", ""));
        subMap.insert("14004000", SubjectData("Social Issues", "Disabled", ""));
        subMap.insert("14005000", SubjectData("Social Issues", "Euthanasia", ""));
        subMap.insert("14006000", SubjectData("Social Issues", "Family", ""));
        subMap.insert("14007000", SubjectData("Social Issues", "Family planning", ""));
        subMap.insert("14008000", SubjectData("Social Issues", "Health insurance", ""));
        subMap.insert("14009000", SubjectData("Social Issues", "Homelessness", ""));
        subMap.insert("14010000", SubjectData("Social Issues", "Minority groups", ""));
        subMap.insert("14011000", SubjectData("Social Issues", "Pornography", ""));
        subMap.insert("14012000", SubjectData("Social Issues", "Poverty", ""));
        subMap.insert("14013000", SubjectData("Social Issues", "Prostitution", ""));
        subMap.insert("14014000", SubjectData("Social Issues", "Racism", ""));
        subMap.insert("14015000", SubjectData("Social Issues", "Welfare", ""));

        subMap.insert("15000000", SubjectData("Sport", "", ""));
        subMap.insert("15001000", SubjectData("Sport", "Aero and Aviation Sports", ""));
        subMap.insert("15002000", SubjectData("Sport", "Alpine Skiing", ""));
        subMap.insert("15003000", SubjectData("Sport", "American Football", ""));
        subMap.insert("15004000", SubjectData("Sport", "Archery", ""));
        subMap.insert("15005000", SubjectData("Sport", "Athletics, Track & Field", ""));
        subMap.insert("15006000", SubjectData("Sport", "Badminton", ""));
        subMap.insert("15007000", SubjectData("Sport", "Baseball", ""));
        subMap.insert("15008000", SubjectData("Sport", "Basketball", ""));
        subMap.insert("15009000", SubjectData("Sport", "Biathlon", ""));
        subMap.insert("15010000", SubjectData("Sport", "Billiards, Snooker and Pool", ""));
        subMap.insert("15011000", SubjectData("Sport", "Bobsleigh", ""));
        subMap.insert("15012000", SubjectData("Sport", "Bowling", ""));
        subMap.insert("15013000", SubjectData("Sport", "Bowls & Petanque", ""));
        subMap.insert("15014000", SubjectData("Sport", "Boxing", ""));
        subMap.insert("15015000", SubjectData("Sport", "Canoeing & Kayaking", ""));
        subMap.insert("15016000", SubjectData("Sport", "Climbing", ""));
        subMap.insert("15017000", SubjectData("Sport", "Cricket", ""));
        subMap.insert("15018000", SubjectData("Sport", "Curling", ""));
        subMap.insert("15019000", SubjectData("Sport", "Cycling", ""));
        subMap.insert("15020000", SubjectData("Sport", "Dancing", ""));
        subMap.insert("15021000", SubjectData("Sport", "Diving", ""));
        subMap.insert("15022000", SubjectData("Sport", "Equestrian", ""));
        subMap.insert("15023000", SubjectData("Sport", "Fencing", ""));
        subMap.insert("15024000", SubjectData("Sport", "Field Hockey", ""));
        subMap.insert("15025000", SubjectData("Sport", "Figure Skating", ""));
        subMap.insert("15026000", SubjectData("Sport", "Freestyle Skiing", ""));
        subMap.insert("15027000", SubjectData("Sport", "Golf", ""));
        subMap.insert("15028000", SubjectData("Sport", "Gymnastics", ""));
        subMap.insert("15029000", SubjectData("Sport", "Handball (Team)", ""));
        subMap.insert("15030000", SubjectData("Sport", "Horse Racing, Harness Racing", ""));
        subMap.insert("15031000", SubjectData("Sport", "Ice Hockey", ""));
        subMap.insert("15032000", SubjectData("Sport", "Jai Alai (Pelota)", ""));
        subMap.insert("15033000", SubjectData("Sport", "Judo", ""));
        subMap.insert("15034000", SubjectData("Sport", "Karate", ""));
        subMap.insert("15035000", SubjectData("Sport", "Lacrosse", ""));
        subMap.insert("15036000", SubjectData("Sport", "Luge", ""));
        subMap.insert("15037000", SubjectData("Sport", "Marathon", ""));
        subMap.insert("15038000", SubjectData("Sport", "Modern Pentathlon", ""));
        subMap.insert("15039000", SubjectData("Sport", "Motor Racing", ""));
        subMap.insert("15040000", SubjectData("Sport", "Motor Rallying", ""));
        subMap.insert("15041000", SubjectData("Sport", "Motorcycling", ""));
        subMap.insert("15042000", SubjectData("Sport", "Netball", ""));
        subMap.insert("15043000", SubjectData("Sport", "Nordic Skiing", ""));
        subMap.insert("15044000", SubjectData("Sport", "Orienteering", ""));
        subMap.insert("15045000", SubjectData("Sport", "Polo", ""));
        subMap.insert("15046000", SubjectData("Sport", "Power Boating", ""));
        subMap.insert("15047000", SubjectData("Sport", "Rowing", ""));
        subMap.insert("15048000", SubjectData("Sport", "Rugby League", ""));
        subMap.insert("15049000", SubjectData("Sport", "Rugby Union", ""));
        subMap.insert("15050000", SubjectData("Sport", "Sailing", ""));
        subMap.insert("15051000", SubjectData("Sport", "Shooting", ""));
        subMap.insert("15052000", SubjectData("Sport", "Ski Jumping", ""));
        subMap.insert("15053000", SubjectData("Sport", "Snow Boarding", ""));
        subMap.insert("15054000", SubjectData("Sport", "Soccer", ""));
        subMap.insert("15055000", SubjectData("Sport", "Softball", ""));
        subMap.insert("15056000", SubjectData("Sport", "Speed Skating", ""));
        subMap.insert("15057000", SubjectData("Sport", "Speedway", ""));
        subMap.insert("15058000", SubjectData("Sport", "Sports Organisations", ""));
        subMap.insert("15059000", SubjectData("Sport", "Squash", ""));
        subMap.insert("15060000", SubjectData("Sport", "Sumo Wrestling", ""));
        subMap.insert("15061000", SubjectData("Sport", "Surfing", ""));
        subMap.insert("15062000", SubjectData("Sport", "Swimming", ""));
        subMap.insert("15063000", SubjectData("Sport", "Table Tennis", ""));
        subMap.insert("15064000", SubjectData("Sport", "Taekwon-Do", ""));
        subMap.insert("15065000", SubjectData("Sport", "Tennis", ""));
        subMap.insert("15066000", SubjectData("Sport", "Triathlon", ""));
        subMap.insert("15067000", SubjectData("Sport", "Volleyball", ""));
        subMap.insert("15068000", SubjectData("Sport", "Water Polo", ""));
        subMap.insert("15069000", SubjectData("Sport", "Water Skiing", ""));
        subMap.insert("15070000", SubjectData("Sport", "Weightlifting", ""));
        subMap.insert("15071000", SubjectData("Sport", "Windsurfing", ""));
        subMap.insert("15072000", SubjectData("Sport", "Wrestling", ""));

        subMap.insert("16000000", SubjectData("Unrest, Conflicts & War", "", ""));
        subMap.insert("16001000", SubjectData("Unrest, Conflicts & War", "Acts of terror", ""));
        subMap.insert("16002000", SubjectData("Unrest, Conflicts & War", "Armed conflict", ""));
        subMap.insert("16003000", SubjectData("Unrest, Conflicts & War", "Civil unrest", ""));
        subMap.insert("16004000", SubjectData("Unrest, Conflicts & War", "Coup d'Etat", ""));
        subMap.insert("16005000", SubjectData("Unrest, Conflicts & War", "Guerrilla activities", ""));
        subMap.insert("16006000", SubjectData("Unrest, Conflicts & War", "Massacre", ""));
        subMap.insert("16007000", SubjectData("Unrest, Conflicts & War", "Riots", ""));
        subMap.insert("16008000", SubjectData("Unrest, Conflicts & War", "Violent demonstrations", ""));
        subMap.insert("16009000", SubjectData("Unrest, Conflicts & War", "War", ""));

        subMap.insert("17000000", SubjectData("Weather", "", ""));
        subMap.insert("17001000", SubjectData("Weather", "Forecasts", ""));
        subMap.insert("17002000", SubjectData("Weather", "Global change", ""));
        subMap.insert("17003000", SubjectData("Weather", "Reports", ""));
        subMap.insert("17004000", SubjectData("Weather", "Statistics", ""));
        subMap.insert("17005000", SubjectData("Weather", "Warnings", ""));
    }

    typedef QMap<QString, SubjectData>  SubjectCodesMap; 

    SubjectCodesMap                     subMap;

    QStringList                         oldSubjects;

    QWidget                            *optionsBox;

    QPushButton                        *addSubjectButton;
    QPushButton                        *delSubjectButton;
    QPushButton                        *repSubjectButton;

    QCheckBox                          *subjectsCheck;

    QLabel                             *iprLabel;
    QLabel                             *refLabel;
    QLabel                             *nameLabel;
    QLabel                             *matterLabel;
    QLabel                             *detailLabel;

    QButtonGroup                       *btnGroup;

    QRadioButton                       *stdBtn;
    QRadioButton                       *customBtn;

    QComboBox                          *refCB;

    KLineEdit                          *iprEdit;
    KLineEdit                          *refEdit;
    KLineEdit                          *nameEdit;
    KLineEdit                          *matterEdit;
    KLineEdit                          *detailEdit;

    KListWidget                        *subjectsBox;
};

IPTCSubjects::IPTCSubjects(QWidget* parent)
            : QWidget(parent)
{
    d = new IPTCSubjectsPriv;
    QGridLayout *grid = new QGridLayout(this);

    // Subject string only accept printable Ascii char excepted these one:
    // - '*' (\x2A)
    // - ':' (\x3A)
    // - '?' (\x3F)
    QRegExp subjectAsciiRx("[\x20-\x29\x2B-\x39\x3B-\x3E\x40-\x7F]+$");
    QValidator *subjectAsciiValidator = new QRegExpValidator(subjectAsciiRx, this);

    // Subject Reference Number only accept digit.
    QRegExp refDigitRx("^[0-9]{8}$");
    QValidator *refValidator = new QRegExpValidator(refDigitRx, this);

    d->subjectsCheck = new QCheckBox(i18n("Use structured definition of the subject matter:"), this);    

    // --------------------------------------------------------

    d->optionsBox      = new QWidget(this);
    QGridLayout *grid2 = new QGridLayout(d->optionsBox);
    d->btnGroup        = new QButtonGroup(d->optionsBox);
    KHBox *hbox        = new KHBox(d->optionsBox);
    d->stdBtn          = new QRadioButton(i18n("Use standard"), hbox);
    QLabel *codeLink   = new QLabel("<b><a href='http://www.iptc.org/NewsCodes'>reference code</a></b>", hbox);
    d->refCB           = new QComboBox(d->optionsBox);
    d->customBtn       = new QRadioButton(i18n("Use custom definition"), d->optionsBox);
    codeLink->setOpenExternalLinks(true);
    codeLink->setWordWrap(true);
    hbox->setMargin(0);
    hbox->setSpacing(0);

    d->btnGroup->addButton(d->stdBtn,    IPTCSubjectsPriv::STANDARD);
    d->btnGroup->addButton(d->customBtn, IPTCSubjectsPriv::CUSTOM);
    d->btnGroup->setExclusive(true);
    d->stdBtn->setChecked(true);

    for (IPTCSubjectsPriv::SubjectCodesMap::Iterator it = d->subMap.begin();
         it != d->subMap.end(); ++it)
        d->refCB->addItem(it.key());

    // --------------------------------------------------------

    d->iprEdit = new KLineEdit(d->optionsBox);
    d->iprEdit->setClearButtonShown(true);
    d->iprEdit->setValidator(subjectAsciiValidator);
    d->iprEdit->setMaxLength(32);
    d->iprEdit->setWhatsThis(i18n("<p>Enter here the Informative Provider Reference. "
                                  "I.P.R is a name registered with the IPTC/NAA, identifying the "
                                  "provider that provides an indicator of the content. "
                                  "The default value for the I.P.R is \"IPTC\" if a standard Reference "
                                  "Code is used. This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->refEdit = new KLineEdit(d->optionsBox);
    d->refEdit->setClearButtonShown(true);
    d->refEdit->setValidator(refValidator);
    d->refEdit->setMaxLength(8);
    d->refEdit->setWhatsThis(i18n("<p>Enter here the Subject Reference Number. "
                                  "Provides a numeric code to indicate the Subject Name plus "
                                  "optional Subject Matter and Subject Detail Names in the "
                                  "language of the service. Subject Reference is a number "
                                  "from the range 01000000 to 17999999 and represent a "
                                  "language independent international reference to "
                                  "a Subject. A Subject is identified by its Reference Number "
                                  "and corresponding Names taken from a standard lists given "
                                  "by IPTC/NAA. If a standard reference code is used, these lists "
                                  "are the English language reference versions. "
                                  "This field is limited to 8 ASCII digit code."));

    // --------------------------------------------------------

    d->nameEdit = new KLineEdit(d->optionsBox);
    d->nameEdit->setClearButtonShown(true);
    d->nameEdit->setValidator(subjectAsciiValidator);
    d->nameEdit->setMaxLength(64);
    d->nameEdit->setWhatsThis(i18n("<p>Enter here the Subject Name. English language is used "
                                   "if you selected a standard IPTC/NAA reference code. "
                                   "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    d->matterEdit = new KLineEdit(d->optionsBox);
    d->matterEdit->setClearButtonShown(true);
    d->matterEdit->setValidator(subjectAsciiValidator);
    d->matterEdit->setMaxLength(64);
    d->matterEdit->setWhatsThis(i18n("<p>Enter here the Subject Matter Name. English language is used "
                                     "if you selected a standard IPTC/NAA reference code. "
                                     "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    d->detailEdit = new KLineEdit(d->optionsBox);
    d->detailEdit->setClearButtonShown(true);
    d->detailEdit->setValidator(subjectAsciiValidator);
    d->detailEdit->setMaxLength(64);
    d->detailEdit->setWhatsThis(i18n("<p>Enter here the Subject Detail Name. English language is used "
                                     "if you selected a standard IPTC/NAA reference code. "
                                     "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    d->iprLabel    = new QLabel(i18n("I.P.R:"), d->optionsBox);
    d->refLabel    = new QLabel(i18n("Reference:"), d->optionsBox);
    d->nameLabel   = new QLabel(i18n("Name:"), d->optionsBox);
    d->matterLabel = new QLabel(i18n("Matter:"), d->optionsBox);
    d->detailLabel = new QLabel(i18n("Detail:"), d->optionsBox);

    // --------------------------------------------------------

    grid2->addWidget(hbox, 0, 0, 1, 2);
    grid2->addWidget(d->refCB, 0, 2, 1, 1);
    grid2->addWidget(d->customBtn, 1, 0, 1, 4);
    grid2->addWidget(d->iprLabel, 2, 0, 1, 1);
    grid2->addWidget(d->iprEdit, 2, 1, 1, 4);
    grid2->addWidget(d->refLabel, 3, 0, 1, 1);
    grid2->addWidget(d->refEdit, 3, 1, 1, 1);
    grid2->addWidget(d->nameLabel,  4, 0, 1, 1);
    grid2->addWidget(d->nameEdit, 4, 1, 1, 4);
    grid2->addWidget(d->matterLabel, 5, 0, 1, 1);
    grid2->addWidget(d->matterEdit, 5, 1, 1, 4);
    grid2->addWidget(d->detailLabel, 6, 0, 1, 1);
    grid2->addWidget(d->detailEdit, 6, 1, 1, 4);
    grid2->setColumnStretch(4, 10);                     
    grid2->setMargin(0);
    grid2->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    d->subjectsBox = new KListWidget(this);
    d->subjectsBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    
    d->addSubjectButton = new QPushButton( i18n("&Add"), this);
    d->delSubjectButton = new QPushButton( i18n("&Delete"), this);
    d->repSubjectButton = new QPushButton( i18n("&Replace"), this);
    d->addSubjectButton->setIcon(SmallIcon("edit-add"));
    d->delSubjectButton->setIcon(SmallIcon("edit-delete"));
    d->repSubjectButton->setIcon(SmallIcon("view-refresh"));
    d->delSubjectButton->setEnabled(false);
    d->repSubjectButton->setEnabled(false);

    // --------------------------------------------------------

    QLabel *note = new QLabel(i18n("<b>Note: "
                 "<b><a href='http://en.wikipedia.org/wiki/IPTC'>IPTC</a></b> "
                 "text tags only support the printable "
                 "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b> "
                 "characters set and limit strings size. "
                 "Use contextual help for details.</b>"), this);
    note->setMaximumWidth(150);
    note->setOpenExternalLinks(true);
    note->setWordWrap(true);
    note->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    // --------------------------------------------------------

    grid->setAlignment( Qt::AlignTop );
    grid->addWidget(d->subjectsCheck, 0, 0, 1, 4);
    grid->addWidget(d->optionsBox, 1, 0, 1, 4);
    grid->addWidget(d->subjectsBox, 2, 0, 5, 3);
    grid->addWidget(d->addSubjectButton, 2, 3, 1, 1);
    grid->addWidget(d->delSubjectButton, 3, 3, 1, 1);
    grid->addWidget(d->repSubjectButton, 4, 3, 1, 1);
    grid->addWidget(note, 5, 3, 1, 1);
    grid->setRowStretch(6, 10);  
    grid->setColumnStretch(2, 1);                     
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());    
                                         
    // --------------------------------------------------------

    connect(d->subjectsBox, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotSubjectSelectionChanged()));
    
    connect(d->addSubjectButton, SIGNAL(clicked()),
            this, SLOT(slotAddSubject()));
    
    connect(d->delSubjectButton, SIGNAL(clicked()),
            this, SLOT(slotDelSubject()));

    connect(d->repSubjectButton, SIGNAL(clicked()),
            this, SLOT(slotRepSubject()));

    connect(d->btnGroup, SIGNAL(buttonReleased(int)),
            this, SLOT(slotEditOptionChanged(int)));

    connect(d->refCB, SIGNAL(activated(int)),
            this, SLOT(slotRefChanged()));

    // --------------------------------------------------------

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            this, SLOT(slotSubjectsToggled(bool)));

    // --------------------------------------------------------

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->addSubjectButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->delSubjectButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->repSubjectButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));
}

IPTCSubjects::~IPTCSubjects()
{
    delete d;
}

void IPTCSubjects::slotSubjectsToggled(bool b)
{
    d->optionsBox->setEnabled(b);
    d->subjectsBox->setEnabled(b);
    d->addSubjectButton->setEnabled(b);
    d->delSubjectButton->setEnabled(b);
    d->repSubjectButton->setEnabled(b);
    slotEditOptionChanged(d->btnGroup->id(d->btnGroup->checkedButton()));
}

void IPTCSubjects::slotEditOptionChanged(int b)
{
    if (b == IPTCSubjectsPriv::CUSTOM)
    {
        d->refCB->setEnabled(false);
        d->iprLabel->setEnabled(true);
        d->refLabel->setEnabled(true);
        d->nameLabel->setEnabled(true);
        d->matterLabel->setEnabled(true);
        d->detailLabel->setEnabled(true);
        d->iprEdit->setEnabled(true);
        d->refEdit->setEnabled(true);
        d->nameEdit->setEnabled(true);
        d->matterEdit->setEnabled(true);
        d->detailEdit->setEnabled(true);
    }
    else
    {
        d->refCB->setEnabled(true);
        d->iprLabel->setEnabled(false);
        d->refLabel->setEnabled(false);
        d->nameLabel->setEnabled(false);
        d->matterLabel->setEnabled(false);
        d->detailLabel->setEnabled(false);
        d->iprEdit->setEnabled(false);
        d->refEdit->setEnabled(false);
        d->nameEdit->setEnabled(false);
        d->matterEdit->setEnabled(false);
        d->detailEdit->setEnabled(false);
        slotRefChanged(); 
    }
}

void IPTCSubjects::slotRefChanged()
{
    QString key = d->refCB->currentText();
    QString name, matter, detail;

    for (IPTCSubjectsPriv::SubjectCodesMap::Iterator it = d->subMap.begin();
         it != d->subMap.end(); ++it)
    {
        if (key == it.key())
        {
            name   = it.value().name;
            matter = it.value().matter;
            detail = it.value().detail;
        }
    }

    d->iprEdit->setText("IPTC");
    d->refEdit->setText(key);
    d->nameEdit->setText(name);
    d->matterEdit->setText(matter);
    d->detailEdit->setText(detail);
}

QString IPTCSubjects::buildSubject() const
{
    QString subject = d->iprEdit->text();
    subject.append(":");
    subject.append(d->refEdit->text());
    subject.append(":");
    subject.append(d->nameEdit->text());
    subject.append(":");
    subject.append(d->matterEdit->text());
    subject.append(":");
    subject.append(d->detailEdit->text());
    return subject;
}

void IPTCSubjects::slotDelSubject()
{
    QListWidgetItem *item = d->subjectsBox->currentItem();
    if (!item) return;
    d->subjectsBox->takeItem(d->subjectsBox->row(item));
    delete item;
}

void IPTCSubjects::slotRepSubject()
{
    QString newSubject = buildSubject();
    if (newSubject.isEmpty()) return;

    if (!d->subjectsBox->selectedItems().isEmpty())
    {
        d->subjectsBox->selectedItems()[0]->setText(newSubject);
        d->iprEdit->clear();
        d->refEdit->clear();
        d->nameEdit->clear();
        d->matterEdit->clear();
        d->detailEdit->clear();
    }
}

void IPTCSubjects::slotSubjectSelectionChanged()
{
    if (!d->subjectsBox->selectedItems().isEmpty())
    {
        QString subject = d->subjectsBox->selectedItems()[0]->text();
        d->iprEdit->setText(subject.section(':', 0, 0));
        d->refEdit->setText(subject.section(':', 1, 1));
        d->nameEdit->setText(subject.section(':', 2, 2));
        d->matterEdit->setText(subject.section(':', 3, 3));
        d->detailEdit->setText(subject.section(':', 4, 4));
        d->delSubjectButton->setEnabled(true);
        d->repSubjectButton->setEnabled(true);
    }
    else
    {
        d->delSubjectButton->setEnabled(false);
        d->repSubjectButton->setEnabled(false);
    }
}

void IPTCSubjects::slotAddSubject()
{
    QString newSubject = buildSubject();
    if (newSubject.isEmpty()) return;

    bool found = false;
    for (int i = 0 ; i < d->subjectsBox->count(); i++)
    {
        QListWidgetItem *item = d->subjectsBox->item(i);
        if (newSubject == item->text()) 
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        d->subjectsBox->insertItem(d->subjectsBox->count(), newSubject);
        d->iprEdit->clear();
        d->refEdit->clear();
        d->nameEdit->clear();
        d->matterEdit->clear();
        d->detailEdit->clear();
    }
}

void IPTCSubjects::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    d->oldSubjects = exiv2Iface.getIptcSubjects();

    d->subjectsBox->clear();
    d->subjectsCheck->setChecked(false);
    if (!d->oldSubjects.isEmpty())
    {
        d->subjectsBox->insertItems(0, d->oldSubjects);
        d->subjectsCheck->setChecked(true);
    }
    blockSignals(false);
    slotSubjectsToggled(d->subjectsCheck->isChecked());
}

void IPTCSubjects::applyMetadata(QByteArray& iptcData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    QStringList newSubjects;    

    for (int i = 0 ; i < d->subjectsBox->count(); i++)
    {
        QListWidgetItem *item = d->subjectsBox->item(i);
        newSubjects.append(item->text());
    }

    if (d->subjectsCheck->isChecked())
        exiv2Iface.setIptcSubjects(d->oldSubjects, newSubjects);
    else
        exiv2Iface.setIptcSubjects(d->oldSubjects, QStringList());

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin
