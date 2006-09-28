<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>

<!--
/* ============================================================
 * Authors: Caulier Gilles <caulier dot gilles at kdemail dot net>
 * Date   : 2006-09-22
 * Description : a php script to show GPS locator world map
 *               this script is used by GPSSync kipi-plugin.
 * 
 * Copyright 2006 by Gilles Caulier
 * 
 * Note : this script use Google Map api:
 *        http://www.google.com/apis/maps/documentation
 *
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
-->

<head>
<script src="http://maps.google.com/maps?file=api&v=2&key=ABQIAAAAy_Vv5rc03ctmYvwfsuTH6RSK29CRGKrdb78LNYpP1_riKtR3zRRxy4unyuWAi2vp7m1isLwuHObXDg" 
type="text/javascript">
</script>

<script type="text/javascript">

//<![CDATA[
function loadMap()
{
    var map = new GMap2(document.getElementById("map"));
    map.addControl(new GLargeMapControl());
    map.addControl(new GMapTypeControl());

<?php
    echo "map.setCenter(new GLatLng(";
    echo $_GET['lat'];
    echo ", ";
    echo $_GET['lon'];
    echo "), 8);\n";
    
    echo "map.addOverlay(new GMarker(new GLatLng(";
    echo $_GET['lat'];
    echo ", ";
    echo $_GET['lon'];
    echo ")));\n";
?>

    GEvent.addListener(map, "click", 
        function(overlay, point)
        {
            map.clearOverlays();
            if (point) 
            {
                map.addOverlay(new GMarker(point));
                map.panTo(point);
                msg = "(lat:" + point.lat() + ", lon:" + point.lng() + ")";
                window.status=msg;
            }
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
<div id="map" style="width: 640px; height: 480px"></div>
</body>
</html>
