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
<title>GPSSync Kipi-plugin Geographical Location Editor</title>
<script src="http://maps.google.com/maps?file=api&amp;v=2&amp;key=ABQIAAAAy_Vv5rc03ctmYvwfsuTH6RSK29CRGKrdb78LNYpP1_riKtR3zRRxy4unyuWAi2vp7m1isLwuHObXDg" 
type="text/javascript">
</script>
<style type="text/css">
      @import url("http://www.google.com/uds/css/gsearch.css");
      @import url("http://www.google.com/uds/solutions/localsearch/gmlocalsearch.css");
      }
</style>
<script src="http://www.google.com/uds/api?file=uds.js&amp;v=1.0" type="text/javascript"></script>
<script src="http://www.google.com/uds/solutions/localsearch/gmlocalsearch.js" type="text/javascript"></script>

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
function loadMap()
{
    var map = new GMap2(document.getElementById("map"));
    var searchoptions = {
      suppressInitialResultSelection : true
    };

    var markeroptions = { 
      autoPan : true,
      draggable : true,
<?php
      $filename = $_GET['filename'];
      if ($filename != "") echo "title : \"$filename\""; 
?>
    };

    map.addControl(new GLargeMapControl());
    map.addControl(new GMapTypeControl());
    map.addControl(new GScaleControl());
    map.addControl(new google.maps.LocalSearch(searchoptions), new GControlPosition(G_ANCHOR_BOTTOM_RIGHT, new GSize(10,20)));

<?php
    $maptype = $_GET['maptype'];
    if ($maptype == "") $maptype = "G_NORMAL_MAP";

    echo "map.setCenter(new GLatLng(";
    echo $_GET['latitude'];
    echo ", ";
    echo $_GET['longitude'];
    echo "), ";
    echo $_GET['zoom'];
    echo ", ";
    echo $maptype;
    echo ");\n";

    echo "var marker = new GMarker(new GLatLng(";
    echo $_GET['latitude'];
    echo ", ";
    echo $_GET['longitude'];
    echo "), markeroptions";
    echo ");\n";

    echo "map.addOverlay(marker)";
?>

    GEvent.addListener(map, "click", 
        function(overlay, point)
        {
            if (point)
            {
                marker.setPoint(point); 
                msg = "(lat:" + point.lat() + ", lon:" + point.lng() + ")";
                window.status=msg;
            }
        }
    );
 
    GEvent.addListener(marker, "drag", 
        function()
        {
            var point = marker.getPoint(); 
            msg = "(lat:" + point.lat() + ", lon:" + point.lng() + ")";
            window.status=msg;
        }
    );

    GEvent.addListener(marker, "dragend", 
        function()
        {
            var point = marker.getPoint(); 
            msg = "(lat:" + point.lat() + ", lon:" + point.lng() + ")";
            window.status=msg;
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
