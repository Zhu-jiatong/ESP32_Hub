function _(id) {
	return document.getElementById(id);
}

function __(class_name) {
	return document.getElementsByClassName(class_name);
}

async function dir_json(path) {
	const url = new URL("/dir", window.location.origin);
	url.searchParams.append("disk", "/sdh"); // will be replaced by server side change
	url.searchParams.append("path", current_dir);
	let res = await fetch(url);
	if (!res.ok) throw new Error("unsuccessful request");
	return await res.json();
}

function adjust_size(size) {
	let intSize = Number(size);
	const sizeUnit = new Array(' B', ' KB', ' MB', ' GB');
	for (const unit of sizeUnit) {
		if (intSize >= 1024) {
			intSize /= 1024;
		}
		else
			return intSize.toFixed(1) + unit;
	}
}

function split_path(path) {
	return String(path).split("/");
}

function get_ico(fname) {
	switch (fname.slice((fname.lastIndexOf(".") - 1 >>> 0) + 2)) {
		case 'jpg': case 'jpeg': case 'png': case 'gif': case 'webp':
			return '&#xe8b9;';
		case 'ico':
			return '&#xec6c;';
		case 'mp4': case 'mov': case 'webm':
			return '&#xe714;';
		case 'mp3': return '&#xe8d6;'; case 'js': case 'css': case 'html': case 'bin':
			return '&#xe943;';
		case 'pdf':
			return '&#xea90;';
		case 'docx': case 'doc':
			return '&#xe8a5;';
		case 'ttf':
			return '&#xe8d2;';
		case 'json':
			return '&#xe713;';
		case 'zip': case 'rar': case '7z': case 'gz':
			return '&#xf012;'
		case "dir":
			return '&#xe8b7;';

		default:
			return '&#xed2b;';
	}
}