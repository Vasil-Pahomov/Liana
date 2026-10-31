// Liana web UI. The effect and palette lists are built from GET /api/catalog,
// the live state comes from the WebSocket on port 81 (JSON messages, see src/websocket.cpp).

const ESP_WS = "ws://" + location.hostname + ":81/";
const LONG_PRESS_MS = 1000;   // long press on an effect/palette excludes it from random cycling
const POINT_INTERVAL_MS = 50; // throttle of orientation messages for the "magic" effect
const MAGIC_KIND = "magic";

let wsconn = null;
let catalog = null;
let bAng = 0, gAng = 0;       // device orientation mapped to 0..255 (position, color)
let preAcc = 0;
let lastPointMs = 0;
let presstimer = null;

function byId(id) { return document.getElementById(id); }

function send(msg) {
	if (wsconn && wsconn.readyState === WebSocket.OPEN) {
		wsconn.send(JSON.stringify(msg));
	}
}

function selectedValue(groupName) {
	const ctl = document.querySelector('input[name="' + groupName + '"]:checked');
	return ctl ? Number(ctl.value) : null;
}

// --- controls ---

// adds a radio button with a label to the container; returns the input
function addChoice(container, groupName, id, labelSpan, longPressKind) {
	const input = document.createElement('input');
	input.type = 'radio';
	input.name = groupName;
	input.value = id;
	input.id = groupName + id;
	input.addEventListener('click', onAPChange);

	const label = document.createElement('label');
	label.htmlFor = input.id;
	label.appendChild(labelSpan);
	if (longPressKind) {
		label.dataset.kind = longPressKind;
		label.dataset.index = id;
		label.addEventListener('mousedown', onStartLP);
		label.addEventListener('touchstart', onStartLP, {passive: true});
		label.addEventListener('mouseout', onStopLP);
		label.addEventListener('touchend', onStopLP);
		label.addEventListener('touchcancel', onStopLP);
	}

	container.appendChild(input);
	container.appendChild(label);
	return input;
}

function paletteGradient(colors) {
	// black at both ends, the palette colors evenly in between (as the gradient of a strip would look)
	const stops = ['#000 0%'];
	const n = colors.length;
	for (let i = 0; i < n; i++) {
		stops.push(colors[i] + ' ' + Math.round((i + 1) * 100 / (n + 1)) + '%');
	}
	stops.push('#000 100%');
	return 'linear-gradient(90deg, ' + stops.join(', ') + ')';
}

function renderCatalog(cat) {
	const anims = byId('anims');
	const pals = byId('pals');
	const magic = byId('magicc');
	anims.innerHTML = '';
	pals.innerHTML = '';
	magic.innerHTML = '';

	for (const e of cat.effects) {
		if (e.kind === 'regular') {
			const span = document.createElement('span');
			span.textContent = e.title;
			addChoice(anims, 'a', e.id, span, e.cyclable ? 'anim' : null);
		} else if (e.kind === MAGIC_KIND) {
			const span = document.createElement('span');
			span.textContent = e.title.toUpperCase();
			addChoice(magic, 'a', e.id, span, null);
		}
	}
	// the magic effect needs the device orientation sensors
	magic.style.display = (window.DeviceOrientationEvent && magic.children.length > 0) ? '' : 'none';

	for (const p of cat.palettes) {
		const span = document.createElement('span');
		span.style.background = paletteGradient(p.colors);
		span.title = p.title;
		addChoice(pals, 'p', p.id, span, 'pal');
	}
}

// --- state from the firmware ---

function applyState(s) {
	byId('onBox').checked = s.on;
	byId('mainControls').style.display = s.on ? '' : 'none';
	if (s.on) {
		const animctl = byId('a' + s.anim);
		if (animctl) animctl.checked = true;
	}
	const palctl = byId('p' + s.pal);
	if (palctl) palctl.checked = true;

	for (const e of catalog.effects) {
		const ctl = byId('a' + e.id);
		if (ctl && e.cyclable) ctl.disabled = ((s.disabledAnims >> e.id) & 1) === 1;
	}
	for (const p of catalog.palettes) {
		const ctl = byId('p' + p.id);
		if (ctl) ctl.disabled = ((s.disabledPals >> p.id) & 1) === 1;
	}
	updateSensors(s.on ? s.anim : null);
}

// --- user actions ---

function onOnBoxChange() {
	const pal = selectedValue('p');
	if (byId('onBox').checked) {
		send({cmd: 'set', anim: 0, pal: pal === null ? 0 : pal});
	} else {
		send({cmd: 'set', anim: 255, pal: pal === null ? 0 : pal});
	}
}

function onAPChange() {
	const anim = selectedValue('a');
	const pal = selectedValue('p');
	if (anim === null || pal === null) return;
	send({cmd: 'set', anim: anim, pal: pal});
	updateSensors(anim);
	onStopLP();
}

function onStartLP(e) {
	const label = e.currentTarget;
	const kind = label.dataset.kind;
	const index = Number(label.dataset.index);
	const input = byId((kind === 'anim' ? 'a' : 'p') + index);
	onStopLP();
	presstimer = setTimeout(function() {
		presstimer = null;
		send({cmd: 'enable', kind: kind, index: index, enabled: input.disabled});
	}, LONG_PRESS_MS);
}

function onStopLP() {
	if (presstimer !== null) {
		clearTimeout(presstimer);
		presstimer = null;
	}
}

// --- "magic" effect: device orientation ---

let magicId = null;

function updateSensors(animId) {
	if (catalog === null) return;
	if (magicId === null) {
		const m = catalog.effects.find(function(e) { return e.kind === MAGIC_KIND; });
		if (!m) return;
		magicId = m.id;
	}
	const wanted = (animId === magicId);
	if (wanted && !sensorsOn) {
		window.addEventListener("deviceorientation", handleOrientation, true);
		window.addEventListener("devicemotion", handleMotion, true);
		sensorsOn = true;
	} else if (!wanted && sensorsOn) {
		window.removeEventListener("deviceorientation", handleOrientation, true);
		window.removeEventListener("devicemotion", handleMotion, true);
		sensorsOn = false;
	}
}
let sensorsOn = false;

function handleOrientation(e) {
	if (e.beta > -45 && e.beta < 45) {
		bAng = Math.floor((Number(e.beta) + 45) * 255 / 90);
		gAng = Math.floor((Number(e.gamma) + 90) * 255 / 180);
		const now = Date.now();
		if (now - lastPointMs >= POINT_INTERVAL_MS) {
			lastPointMs = now;
			send({cmd: 'point', pos: bAng, color: gAng, boom: false});
		}
	}
}

function handleMotion(e) {
	if (preAcc > 10 && e.acceleration.y < 10) {
		send({cmd: 'point', pos: bAng, color: gAng, boom: true});
	}
	preAcc = e.acceleration.y;
}

// --- connection ---

function keepAlive() {
	send({cmd: 'info'});
	window.setTimeout(keepAlive, 5000);
}

function connectws() {
	wsconn = new WebSocket(ESP_WS, ['arduino']);
	byId('dscon').innerText = 'Соединение...';
	wsconn.onopen = function () {
		send({cmd: 'info'});
		byId('dscon').style.display = 'none';
		byId('allc').style.display = '';
	};
	wsconn.onerror = function (error) {
		console.log('WS error ' + error);
	};
	wsconn.onmessage = function (e) {
		let msg;
		try {
			msg = JSON.parse(e.data);
		} catch (err) {
			console.log('Bad message: ' + e.data);
			return;
		}
		if (msg.type === 'state') {
			applyState(msg);
		}
	};
	wsconn.onclose = function () {
		byId('dscon').style.display = '';
		byId('allc').style.display = 'none';
		byId('dscon').innerText = 'Нет соединения, повтор...';
		window.setTimeout(connectws, 1000);
	};
}

function loadCatalog() {
	fetch('/api/catalog', {cache: 'no-cache'})
		.then(function(r) { if (!r.ok) throw new Error(r.status); return r.json(); })
		.then(function(cat) {
			catalog = cat;
			renderCatalog(cat);
			connectws();
			keepAlive();
		})
		.catch(function(err) {
			byId('dscon').innerText = 'Нет связи с модулем, повтор...';
			console.log('Catalog error: ' + err);
			window.setTimeout(loadCatalog, 2000);
		});
}

function start() {
	byId('onBox').addEventListener('change', onOnBoxChange);
	loadCatalog();
}

document.addEventListener("DOMContentLoaded", start);
