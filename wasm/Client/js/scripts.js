var mapsPlaceholder = [];
async function init() {


	const rootDiv = document.querySelector('#qtrootDiv');

	rootDiv.innerHTML += '<figure  id="qtspinner"> <center > <img id="logo" crossorigin="anonymous" src="img/esterlogo.png" ></img> <div id="qtstatus"></div> </center> </figure> <div class="qtscreen" id="screen" ></div>';

	const spinner = rootDiv.querySelector('#qtspinner');
	const screen = rootDiv.querySelector('#screen');
	const status = rootDiv.querySelector('#qtstatus');


            const showUi = (ui) => {
                [spinner, screen].forEach(element => element.style.display = 'none');
                if (screen === ui)
                    screen.style.position = 'default';
                ui.style.display = 'block';
            }

	try {
                showUi(spinner);
                status.innerHTML = 'Loading...';

                qtModule = await qtLoad({
                    qt: {
                        onLoaded: () => 
			    {
				    showUi(screen);
			    },
                        onExit: exitData =>
                        {
                            status.innerHTML = 'Application exit';
                            status.innerHTML +=
                                exitData.code !== undefined ? ` with code ` : '';
                            status.innerHTML +=
                                exitData.text !== undefined ? ` ()` : '';
                            showUi(spinner);
                        },
                        entryFunction: window.createQtAppInstance,
                        containerElements: [screen],

                    }
                });
		resized = function() { 
			if(document.documentElement.clientWidth<600)
			{
				document.getElementById("map").style.zIndex = "1000";
				document.getElementById("qtrootDiv").style.zIndex = "1";
			}
			qtModule.BookClient.instance().resized(document.documentElement.clientWidth);
			var canvas = document.getElementById("screen");
			qtModule.qtResizeContainerElement(canvas);
		}
		window.addEventListener("resize", resized);
            } catch (e) {
                console.error(e);
                console.error(e.stack);
            }

L.Map.addInitHook(function () {
      mapsPlaceholder.push(this); 
    });
const map = L.map('map').setView([51.505, -0.09], 13);


	const mineMarker = L.icon({
    iconUrl: 'img/mineMarker.png',
    iconSize: [24, 24],
});

	L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
		maxZoom: 19,
		crossOrigin: "",
		attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
	},
	).addTo(map);
const options = {
  enableHighAccuracy: true,
  timeout: 5000,
  maximumAge: 0,
};

function success(pos) {
        const crd = pos.coords;
	map.setView([crd.latitude,crd.longitude],13);
	mine.setLatLng([crd.latitude,crd.longitude]);

    }
function initmapview(pos) {
        const crd = pos.coords;
	
	const mine = L.marker([crd.latitude,crd.longitude],{icon: mineMarker}).addTo(map);
	map.setView([crd.latitude,crd.longitude],13);
	const recButt = document.querySelector("#recenter");
	recButt.style.visibility="visible";
	recButt.onclick = () => {
	map.setView(mine.getLatLng(),13);
		
};


    }
	function error(err) {
  console.warn(`ERROR(${err.code}): ${err.message}`);
		
}

id = navigator.geolocation.watchPosition(success, error, options);
navigator.geolocation.getCurrentPosition(initmapview, error, options);

}

