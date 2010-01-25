<?php
/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-22
 * Description : a php script to show GPS locator world map
 *               this script is used by GPSSync kipi-plugin.
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright 2008 by Gerhard Kulzer <gerhard at kulzer dot net>
 * Copyright (C) 2009,2010 by Michael G. Hansen <mike at mghansen dot de>
 *
 * Notes : This script can use Google Map API version 2 or 3:
 *         http://www.google.com/apis/maps/documentation
 *         This script must be copied to host kipi-plugins
 *         web project page.
 *         This script accept these values from url:
 *           - 'altitude'           : picture altitude.
 *           - 'longitude'          : picture longitude.
 *           - 'latitude'           : picture latitude.
 *           - 'width'              : width of map.
 *           - 'height'             : height of map.
 *           - 'zoom'               : map zoom level.
 *           - 'maptype'            : type of map (G_NORMAL_MAP, G_SATELLITE_MAP, G_HYBRID_MAP, G_TERRAIN_MAP)
 *           - 'filename'           : photo file name as string.
 *           - 'maplang'            : language of the map. See [1] for a list of supported values.
 *           - 'gmapversion'        : version of the Google Maps API to use: 2 or 3
 *           - 'pluginversion'      : version of the gpssync plugin
 *           - 'extraoptions'       : reserved for fixing things in later versions of the plugin
 *           - 'altitudeservice'    : which service to use for looking up the altitude of points: geonames, topocoding, none
 *
 * [1] http://spreadsheets.google.com/pub?key=p9pdwsai2hDMsLkXsoM05KQ&gid=1
 *
 * Notes on the service topocoding :
 *    here is how topoGetAltitude behaves:
 *    1. You call *topoGetAltitude*( lat, lon, action, context, timeout )
 *    2. As soon as the server sends back the altitude, the asynchronous call
 *       to action(altitude,context) is performed.
 *       Here you can operate with the altitude information. And the context
 *       variable contains any useful data that you also want to pass in,
 *       it can be for example the reference to an element where you want the altitude to be assigned.
 *       So for example you can pass the marker as a context.
 *       Note that you can ommit the context if you don't need it.
 *    3. If the server response does not arrive (timeout), the asynchronous
 *       call to action(null,context) is performed.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// extract the parameters from the URL:

// first, get the version of the plugin (not used yet):
$parPluginVersion = '1.0';
if (isset($_GET['pluginversion']))
{
    $parPluginVersion = stripslashes($_GET['pluginversion']));
}

// determine the extra options (not used yet):
$parExtraOptions = '';
if (isset($_GET['extraoptions']))
{
    $parExtraOptions = stripslashes($_GET['extraoptions']);
}

// determine the Google Maps API version to be used:
$parGoogleMapsAPIVersion = 2;
if (isset($_GET['gmapsversion']))
{
    $parGoogleMapsAPIVersion = intval($_GET['gmapsversion']);
}

// determine the language of the Google Maps API:
$parMapLang = 'en';
if (isset($_GET['maplang']))
{
    $parMapLang = $_GET['maplang'];
}

// determine the map type:
$parMapType = 'G_NORMAL_MAP';
if (isset($_GET['maptype']))
{
    $mapType = $_GET['maptype'];

    // make sure the parameter is valid:
    switch ($mapType)
    {
        case 'G_SATELLITE_MAP':
            $parMapType = 'G_SATELLITE_MAP';
            break;

        case 'G_HYBRID_MAP':
            $parMapType = 'G_HYBRID_MAP';
            break;

        case 'G_TERRAIN_MAP':
            if ($parGoogleMapsAPIVersion==3)
            {
                $parMapType = 'G_TERRAIN_MAP';
            }
            else
            {
                $parMapType = 'G_NORMAL_MAP';
            }
            break;

        case 'G_NORMAL_MAP':
        default:
            $parMapType = 'G_NORMAL_MAP';
    }
}

// get latitude, longitude and zoom:
$parCenterLat = '0.0';
$parCenterLon = '0.0';
$parMapZoom = 1;
if (isset($_GET['latitude'])&&isset($_GET['longitude']))
{
    $parCenterLat = $_GET['latitude'];
    $parCenterLon = $_GET['longitude'];
}
if (isset($_GET['zoom']))
{
    $parMapZoom = $_GET['zoom'];
}

// for translating the map type from v2 to v3:
$maptypetranslator = array(
    'G_NORMAL_MAP'=>'google.maps.MapTypeId.ROADMAP',
    'G_SATELLITE_MAP'=>'google.maps.MapTypeId.SATELLITE',
    'G_HYBRID_MAP'=>'google.maps.MapTypeId.HYBRID',
    'G_TERRAIN_MAP'=>'google.maps.MapTypeId.TERRAIN'
    );
if ($parGoogleMapsAPIVersion==3)
{
    $parMapType = $maptypetranslator[$parMapType];
}

// determine the API for looking up the altitude:
$parAltitudeService = 'geonames';
if (isset($_GET['altitudeservice']))
{
    $parAltitudeService = $_GET['altitudeservice'];
}

// determine the width and height of the map:
$parMapHeight = 480; $parMapWidth = 480;
if (isset($_GET['width'])&&isset($_GET['height']))
{
    $parMapHeight = intval($_GET['height']);
    $parMapWidth = intval($_GET['width']);
}

// the name of the file which is geo-coded:
$parFileName = '';
if (isset($_GET['filename']))
{
    $parFileName = stripslashes($_GET['filename']);
}

// start output of HTML:
?>
<!DOCTYPE html
     PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
     "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<title>GPSSync Kipi-plugin Geographical Location Editor</title><?

// now include the script for the appropriate Google Maps API:
if ($parGoogleMapsAPIVersion==3)
{
    ?><script src="http://maps.google.com/maps/api/js?sensor=false&amp;language=<?=$parMapLang ?>" type="text/javascript">
</script><?
}
else
{
    ?><script src="http://maps.google.com/maps?file=api&amp;v=2&amp;hl=<?=$parMapLang ?>&amp;key=ABQIAAAAy_Vv5rc03ctmYvwfsuTH6RSK29CRGKrdb78LNYpP1_riKtR3zRRxy4unyuWAi2vp7m1isLwuHObXDg"
type="text/javascript">
</script>
<script src="http://www.google.com/uds/api?file=uds.js&amp;v=1.0&amp;hl=<?=$parMapLang ?>" type="text/javascript"></script>
<script src="http://www.google.com/uds/solutions/localsearch/gmlocalsearch.js?hl=<?=$parMapLang ?>" type="text/javascript"></script>
<style type="text/css">
      @import url("http://www.google.com/uds/css/gsearch.css");
      @import url("http://www.google.com/uds/solutions/localsearch/gmlocalsearch.css");
</style>
<?
}

// include the scripts for the appropriate altitude service:
if ($parAltitudeService=='geonames')
{
?><script type="text/javascript">

//<![CDATA[

var jsonScriptCounter = 1;

function performJSONRequest(url)
{
    // create a new script object:
    var scriptObject = document.createElement("script");
    scriptObject.setAttribute("type", "text/javascript");
    scriptObject.setAttribute("charset", "utf-8");
    scriptObject.setAttribute("id", jsonScriptCounter++);

    // make sure no caching of the request can occur:
    var cacheBreak = (new Date()).getTime();
    scriptObject.setAttribute("src", url+'&cacheBreak='+cacheBreak);

    // add the script object to the header:
    var theHeader = document.getElementsByTagName("head").item(0);
    theHeader.appendChild(scriptObject);
}

function altitudeCallback(jData)
{
    if (jData == null) {
        return;
    }

    // jData should be: {"srtm3":206,"lng":10.2,"lat":50.01}
    window.status = "(lat:" + jData.lat + ", lon:" + jData.lng + ", alt:" + jData.srtm3  + ")";
}

function getAltitudeForPoint(point)
{
    var requestUrl = 'http://ws.geonames.org/srtm3JSON?lat='+point.lat()+'&lng='+point.lng()+'&callback=altitudeCallback';

    performJSONRequest(requestUrl);
}

//]]>

</script>
<?
}
else if ($parAltitudeService=='topocoding')
{
    ?><script type="text/javascript" src="http://topocoding.com/api/getapi_v1.php?key=ILOGFVOBCUOSRHC"></script>
<script type="text/javascript">

//<![CDATA[

function getAltitudeForPoint(point)
{
    topoGetAltitude( point.lat(), point.lng(),
        function( altitude ) {
            window.status = "(lat:" + point.lat() + ", lon:" + point.lng() + ", alt:" + altitude  + ")";
            }
        );
}

//]]>

</script><?
}
else
{
    // no service at all:
?><script type="text/javascript">

//<![CDATA[

function getAltitudeForPoint(point)
{
    window.status = "(lat:" + point.lat() + ", lon:" + point.lng() + ", alt:" + 0 + ")" ;
}

//]]>

</script>
<?
}

?><style type="text/css">
    /*<![CDATA[*/
    body {
        padding: 0px;
        margin: 0px;
    }
    /*]]>*/
</style>
<?

// now add the appropriate code for displaying the map:

if ($parGoogleMapsAPIVersion==3)
{
?><script type="text/javascript">

//<![CDATA[

function loadMap()
{
<?
    printf("var mapCenter = new google.maps.LatLng(%s,%s);\n", $parCenterLat, $parCenterLon);
    printf("var mapZoom = %s;\n", $parMapZoom);
    printf("var mapType = %s;\n", $parMapType);
    ?>
    var myOptions = {
        zoom: mapZoom,
        center: mapCenter,
        MapTypeId: mapType
    };
    var mapDiv = document.getElementById("map");
    var map = new google.maps.Map(mapDiv, myOptions);

    var myMarker = new google.maps.Marker({
        position: mapCenter,
        map: map,
        draggable: true
    });

    google.maps.event.addListener(map, "click",
        function(mouseEvent)
        {
            if (mouseEvent)
            {
                var latLng = mouseEvent.latLng;
                if (latLng)
                {
                    myMarker.setPosition(latLng);
                    getAltitudeForPoint(latLng);
                }
            }
        }
    );

    // do not update the coordinates while dragging,
    // to reduce the load on geonames.org
//     google.maps.event.addListener(myMarker, "drag",
//         function()
//         {
//             var latLng = myMarker.getPosition();
//             getAltitudeForPoint(latLng);
//         }
//     );

    google.maps.event.addListener(myMarker, "dragend",
        function()
        {
            var latLng = myMarker.getPosition();
            getAltitudeForPoint(latLng);
        }
    );

    google.maps.event.addListener(map, "zoom_changed",
        function()
        {
            msg = "newZoomLevel:" + map.getZoom();
            window.status=msg;
        }
    );

    google.maps.event.addListener(map, "maptypeid_changed",
        function()
        {
            var myMapType = map.getMapTypeId();
            if (myMapType == google.maps.MapTypeId.SATELLITE ) {msg = "newMapType:G_SATELLITE_MAP";}
            if (myMapType == google.maps.MapTypeId.ROADMAP   ) {msg = "newMapType:G_NORMAL_MAP";}
            if (myMapType == google.maps.MapTypeId.HYBRID    ) {msg = "newMapType:G_HYBRID_MAP";}
            if (myMapType == google.maps.MapTypeId.TERRAIN   ) {msg = "newMapType:G_TERRAIN_MAP";}
            window.status=msg;
        }
    );
}

//]]>

</script>
<?
}
else
{
?><script type="text/javascript">

//<![CDATA[

function loadMap()
{
    var map = new GMap2(document.getElementById("map"));

    var searchoptions = {
        suppressInitialResultSelection : true
    };

    var markeroptions = {
        autoPan : true,
        draggable : true,
        title: "<?=$parFileName ?>"
    };

    map.addControl(new GLargeMapControl());
    map.addControl(new GMapTypeControl());
    map.addControl(new GScaleControl());
    map.addControl(new google.maps.LocalSearch(searchoptions), new GControlPosition(G_ANCHOR_BOTTOM_RIGHT, new GSize(10,20)));
    map.enableScrollWheelZoom();

<?
    printf("var mapCenter = new GLatLng(%s,%s);\n", $parCenterLat, $parCenterLon);
    printf("var mapZoom = %s;\n", $parMapZoom);
    printf("var mapType = %s;\n", $parMapType);
?>
    map.setCenter(mapCenter, mapZoom);
    map.setMapType(mapType);

    var marker = new GMarker(mapCenter, markeroptions);
    map.addOverlay(marker);

    GEvent.addListener(map, "click",
        function(overlay, point)
        {
            if (point)
            {
                marker.setLatLng(point);
                getAltitudeForPoint(point);
            }
        }
    );

    // do not update the coordinates while dragging,
    // to reduce the load on geonames.org
//     GEvent.addListener(marker, "drag",
//         function()
//         {
//             var point = marker.getLatLng();
//             getAltitudeForPoint(point);
//         }
//     );

    GEvent.addListener(marker, "dragend",
        function()
        {
            var point = marker.getLatLng();
            getAltitudeForPoint(point);
        }
    );

    GEvent.addListener(map, "zoomend",
        function(oldLevel, newLevel)
        {
            msg = "newZoomLevel:" + newLevel;
            window.status=msg;
        }
    );

    GEvent.addListener(map, "maptypechanged",
        function()
        {
            var myMapType = map.getCurrentMapType();
            if (myMapType == G_SATELLITE_MAP) {msg = "newMapType:G_SATELLITE_MAP";}
            if (myMapType == G_NORMAL_MAP)    {msg = "newMapType:G_NORMAL_MAP";}
            if (myMapType == G_HYBRID_MAP)    {msg = "newMapType:G_HYBRID_MAP";}
            window.status=msg;
        }
    );
}

//]]>

</script>
<?
}

// final HTML code, independent of the versions:
?></head>

<body onload="loadMap()">
<?php
printf('<div id="map" style="width: %dpx; height: %dpx;">', $parMapWidth, $parMapHeight);
?>

</div>
</body>
</html>
