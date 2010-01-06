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
 *
 * Notes : This script use Google Map API version 2:
 *         http://www.google.com/apis/maps/documentation
 *         This script must be copied to host kipi-plugins
 *         web project page.
 *         This script accept these values from url:
 *           - 'altitude'  : picture altitude.
 *           - 'longitude' : picture longitude.
 *           - 'width'     : width of map.
 *           - 'height'    : height of map.
 *           - 'zoom'      : map zoom level.
 *           - 'maptype'   : type of map (G_NORMAL_MAP, G_SATELLITE_MAP, G_HYBRID_MAP)
 *           - 'filename'  : photo file name as string.
 *           - 'maplang'   : language of the map. See [1] for a list of supported values.
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

$maplang = $_GET['maplang'];
if ($maplang == "") $maplang = "en";

$maptype = $_GET['maptype'];
if ($maptype == "") $maptype = "G_NORMAL_MAP";
$maptypetranslator = array(
    'G_NORMAL_MAP'=>'google.maps.MapTypeId.ROADMAP',
    'G_SATELLITE_MAP'=>'google.maps.MapTypeId.SATELLITE',
    'G_HYBRID_MAP'=>'google.maps.MapTypeId.HYBRID',
    'G_TERRAIN_MAP'=>'google.maps.MapTypeId.TERRAIN'
    );
?>
<!DOCTYPE html
     PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
     "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<title>GPSSync Kipi-plugin Geographical Location Editor</title>
<script src="http://maps.google.com/maps/api/js?sensor=false&amp;language=<?=$maplang ?>" type="text/javascript">
</script>

<style type="text/css">

    /*<![CDATA[*/
    body {
        padding: 0px;
        margin: 0px;
    }
    /*]]>*/
</style>

<script type="text/javascript">

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
    window.status = "(lat:" + jData.lat + ", lon:" + jData.lng + ", alt:" + jData.srtm3  + ")" ;
}

function getAltitudeForPoint(point)
{
    var requestUrl = 'http://ws.geonames.org/srtm3JSON?lat='+point.lat()+'&lng='+point.lng()+'&callback=altitudeCallback';

    performJSONRequest(requestUrl);
}

function loadMap()
{
<?
    printf("var mapCenter = new google.maps.LatLng(%s,%s);\n", $_GET['latitude'], $_GET['longitude']);
    printf("var mapZoom = %s;\n", $_GET['zoom']);
    printf("var mapType = %s;\n", $maptypetranslator[$maptype]);
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
</head>

<body onload="loadMap()">
<div>
<?php
// print_r ( topoGetAltitudes( array( array( 'latitude', 'longitude' ) ) ) );
?>
</div>
<?php
    echo "<div id=\"map\" ";
    echo "style=\"width: ";
    echo $_GET['width'];
    echo "px; height: ";
    echo $_GET['height'];
    echo "px;\">";
?>

</div>
</body>
</html>
