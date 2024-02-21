var mapsPlaceholder = [];
var serverMarkers = [];
async function init() {




	document.body.innerHTML += '<figure  id="qtspinner"> <center > <img id="logo" crossorigin="anonymous" src="img/esterlogo.png" ></img> <div id="qtstatus"></div> </center> </figure>';


	const spinner = document.body.querySelector('#qtspinner');
	const status = document.body.querySelector('#qtstatus');


	const rootDiv = document.querySelector('#qtrootDiv');
	rootDiv.innerHTML += '<div class="qtscreen" id="screen" ></div>';
	const screen = rootDiv.querySelector('#screen');

	const showUi = (ui) => {
		[spinner, screen].forEach(element => element.style.display = 'none');
		if (screen === ui)
			screen.style.position = 'default';
		ui.style.display = 'block';
	}

	try {
		showUi(spinner);
		status.innerHTML = 'Loading...';

		const	instance = await qtLoad({
			qt: {
				onLoaded: () =>
				{
					document.getElementById("map").style.visibility = "visible";
					L.Map.addInitHook(function () {
						mapsPlaceholder.push(this);
					});
					const map = L.map('map').setView([51.505, -0.09], 13);

					L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
						maxZoom: 19,
						crossOrigin: "",
						attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
					},
					).addTo(map);
					showUi(screen);
					resized = function() {
						if( document.activeElement.type != 'text') {
							if(document.documentElement.clientWidth<600)
							{
								document.getElementById("map").style.visibility = "visible";
								document.getElementById("qtrootDiv").style.visibility = "hidden";
								instance.BookClient.instance().setOneColumn(1);
							}
							else
							{
								document.getElementById("map").style.visibility = "visible";
								document.getElementById("qtrootDiv").style.visibility = "visible";
								instance.BookClient.instance().setOneColumn(0);
							}

						}
					}
					window.addEventListener("resize", resized);
					const options = {
						enableHighAccuracy: true,
						timeout: 5000,
						maximumAge: 0,
					};

					function success(pos) {
						const crd = pos.coords;

						if ( typeof success.mine == 'undefined' ) {
							const mineMarker = L.icon({
								iconUrl: 'img/mineMarker.png',
								iconSize: [24, 24],
							});
							success.mine = new L.marker([crd.latitude,crd.longitude],{icon: mineMarker});
							map.addLayer(success.mine);
							map.setView([crd.latitude,crd.longitude],13);
							const recButt = document.querySelector("#recenter");
							recButt.style.visibility="inherit";
							recButt.onclick = () => {
								map.setView(success.mine.getLatLng(),13);
							};
						}
						success.mine.setLatLng([crd.latitude,crd.longitude]);
					}
					function error(err) {
						console.warn(`ERROR(${err.code}): ${err.message}`);

					}

					id = navigator.geolocation.watchPosition(success, error, options);

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
	} catch (e) {
		console.error(e);
		console.error(e.stack);
	}

}

