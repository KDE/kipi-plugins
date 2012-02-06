document.onkeydown = function(e) {
	e = e || window.event;
	link = null;
	switch (e.keyCode) {
	        case 37:
	        	link = document.getElementById("prev"); 
			break;
		case 39:
	        	link = document.getElementById("next"); 
	                break;
		case 38:
	        	link = document.getElementById("up"); 
	                break;
	}
	if (link)
		location.href = link.href;
};
