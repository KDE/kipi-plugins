<?php
/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-02-07
 * Description : a php script to show world map track list
 *               this script is used by GPSSync kipi-plugin.
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * Notes : This script use Google Map API version 2:
 *         http://www.google.com/apis/maps/documentation
 *         This script must be copied to host kipi-plugins
 *         web project page.
 *         This script accept these values from url:
 *           - 'width'     : width of map.
 *           - 'height'    : height of map.
 *           - 'zoom'      : map zoom level.
 *           - 'maptype'   : type of map (G_NORMAL_MAP, G_SATELLITE_MAP, G_HYBRID_MAP)
 *           - 'items'     : number of track list items
 *           - 'lat#1'     : latitude for 1th item
 *           - 'lng#1'     : longitude for 1th item
 *           - 'title#1'   : title for 1th item
 *           - 'lat#2'     : latitude for 2nd item
 *           - 'lng#2'     : longitude for 2nd item
 *           - 'title#2'   : title for 2nd item
 *              ...
 *           - 'lat#n'     : latitude for item n
 *           - 'lng#n'     : longitude for item n
 *           - 'title#n'   : title for item n
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
?>
<!DOCTYPE html
     PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
     "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<title>GPSSync Kipi-plugin Geographical Track List Editor</title>
<script src="http://maps.google.com/maps?file=api&amp;v=2&amp;key=ABQIAAAAy_Vv5rc03ctmYvwfsuTH6RSK29CRGKrdb78LNYpP1_riKtR3zRRxy4unyuWAi2vp7m1isLwuHObXDg" 
type="text/javascript">
</script>
<script src="http://www.google.com/uds/api?file=uds.js&amp;v=1.0" type="text/javascript"></script>
<script src="http://www.google.com/uds/solutions/localsearch/gmlocalsearch.js" type="text/javascript"></script>
<script type="text/javascript" src="http://topocoding.com/api/getapi_v1.php?key=ILOGFVOBCUOSRHC"></script>
<style type="text/css">
      @import url("http://www.google.com/uds/css/gsearch.css");
      @import url("http://www.google.com/uds/solutions/localsearch/gmlocalsearch.css");

    /*<![CDATA[*/
    body {
        padding: 0px;
        margin: 0px;
    }
    /*]]>*/
</style>

<script type="text/javascript">

//<![CDATA[
function loadMap()
{
    var map = new GMap2(document.getElementById("map"));
    var searchoptions = {
      suppressInitialResultSelection : true
    };

    map.addControl(new GLargeMapControl());
    map.addControl(new GMapTypeControl());
    map.addControl(new GScaleControl());
    map.addControl(new google.maps.LocalSearch(searchoptions), new GControlPosition(G_ANCHOR_BOTTOM_RIGHT, new GSize(10,20)));

<?php
    $maptype = $_GET['maptype'];
    if ($maptype == "") $maptype = "G_NORMAL_MAP";

    echo "map.setCenter(new GLatLng(";
    echo $_GET['lat1'];
    echo ", ";
    echo $_GET['lng1'];
    echo "), ";
    echo $_GET['zoom'];
    echo ", ";
    echo $maptype;
    echo ");\n";

    $items = $_GET[items];

    for ($i=1; $i <= $items; $i++)
    {
        $lat   = sprintf("lat%d", $i);
        $lng   = sprintf("lng%d", $i);
        $alt   = sprintf("alt%d", $i);
        $title = sprintf("title%d", $i);

        echo "var markeroptions";
        echo $i;
        echo " = { ";
        echo "autoPan : true, ";
        echo "draggable : true, ";
        echo "title : \"";
        echo $_GET[$title];
        echo "\"}\n";

        echo "var marker";
        echo $i;
        echo " = new GMarker(new GLatLng(";
        echo $_GET[$lat];
        echo ", ";
        echo $_GET[$lng];
        echo "), markeroptions";
        echo $i;
        echo ");\n";
    
        echo "map.addOverlay(marker";
        echo $i;
        echo ")\n";

        // Post click over marker position event.
        echo "GEvent.addListener(marker";
        echo $i;
        echo ", \"click\", function(){";
        echo "var point = marker";
        echo $i;
        echo ".getPoint();";
        echo "topoGetAltitude( point.lat(), point.lng(), function( altitude ) { window.status = \"(mkr:\" + $i + \", lat:\" + point.lat() + \", lon:\" + point.lng() + \", alt:\" + altitude  + \")\"; });});\n";

        // Post drag-move marker position events.
        echo "GEvent.addListener(marker";
        echo $i;
        echo ", \"drag\", function(){";
        echo "var point = marker";
        echo $i;
        echo ".getPoint();";
        echo "topoGetAltitude( point.lat(), point.lng(), function( altitude ) { window.status = \"(mkr:\" + $i + \", lat:\" + point.lat() + \", lon:\" + point.lng() + \", alt:\" + altitude  + \")\"; });});\n";

        // Post drag-end marker position event.
        echo "GEvent.addListener(marker";
        echo $i;
        echo ", \"dragend\", function(){";
        echo "var point = marker";
        echo $i;
        echo ".getPoint();";
        echo "topoGetAltitude( point.lat(), point.lng(), function( altitude ) { window.status = \"(mkr:\" + $i + \", lat:\" + point.lat() + \", lon:\" + point.lng() + \", alt:\" + altitude  + \")\"; });});\n";
    }
?>

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
            if (myMapType == G_SATELLITE_TYPE) {msg = "newMapType:G_SATELLITE_TYPE";} 
            if (myMapType == G_MAP_TYPE)       {msg = "newMapType:G_MAP_TYPE";} 
            if (myMapType == G_HYBRID_TYPE)    {msg = "newMapType:G_HYBRID_TYPE";} 
            window.status=msg;
        }
    );
}
{
    window.addEventListener("load", 
        function() 
        {
            loadMap(); // Firefox and standard browsers
        }
    , false);
}
//]]>

</script>
</head>

<body onLoad="loadMap()">

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
