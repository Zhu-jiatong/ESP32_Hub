var current_dir = "/";
var current_disk = "/sdh";

function dir_dom(path) {
	current_dir = path;
	const temp_doc = new DocumentFragment();
	split_path(current_dir).forEach(substr => {
		const lvl = temp_doc.appendChild(document.createElement("fluent-breadcrumb-item"));
		lvl.textContent = substr;
		lvl.addEventListener("click", () => dir_dom(current_dir.split(substr)[0] + substr));
	});
	_("path-bar").replaceChildren(temp_doc);
	const temp = document.createElement('tbody');
	temp.id = _("files_table").id;
	dir_json(path).then(data => {
		data.forEach(item => {
			const row = temp.insertRow();
			const row_ico = row.insertCell(0);
			row_ico.classList.add("fluent-font");
			row_ico.classList.add("fIcon");
			row.insertCell(1).textContent = item["name"];
			const row_size = row.insertCell(2);
			row.insertCell(3).textContent = item["mod"];
			if (item["dir"]) {
				row_ico.innerHTML = get_ico(item["name"] + '.dir');
				row.addEventListener("dblclick", () => { dir_dom(item["path"]); });
			} else {
				row_ico.innerHTML = get_ico(item["name"]);
				row_size.textContent = adjust_size(item["size"]);
				row_size.classList.add("tbl_fsize");
				const file_url = new URL("/file", window.location.origin);
				file_url.searchParams.append("disk", item["disk"]);
				file_url.searchParams.append("path", item["path"]);
				file_url.searchParams.append("act", "view");
				row.addEventListener('dblclick', () => window.open(file_url));
			};
		});
		_("files_table").replaceWith(temp);
	});
}

dir_dom(current_dir);

function select_files_btn_toggle(ev) {
	ev.target.appearance = "accent";
	_('download-files-btn').hidden = _('delete-files-btn').hidden = false;
	_("upload-files-btn").hidden = true;
	_("files_header").insertCell(0).appendChild(document.createElement("fluent-checkbox"));
	_('files_table').querySelectorAll("tr").forEach(e => {
		const checkbox = document.createElement("fluent-checkbox");
		const new_cell = e.insertCell(0);
		new_cell.appendChild(checkbox);
	});
	ev.target.onclick = evt => {
		evt.target.appearance = "neutral";
		_('download-files-btn').hidden = _('delete-files-btn').hidden = true;
		_("upload-files-btn").hidden = false;
		_("files_header").children[0].remove();
		_('files_table').querySelectorAll("tr").forEach(e => {
			e.children[0].remove();
			evt.target.onclick = select_files_btn_toggle;
		});
	}
}

document.querySelectorAll("fluent-tab").forEach(e => {
	e.addEventListener("dblclick", ev => {
		document.querySelectorAll("#" + e.parentElement.id + " > fluent-tab > .tab-txt").forEach(e => {
			e.hidden = !e.hidden;
		});
	});
});

_('select-files-btn').onclick = select_files_btn_toggle;
_('upload-files-btn').onclick = e => { _('upload-files-pop').hidden = false };
[...__('close-btn')].forEach(e => {
	e.addEventListener("click", ev => { e.parentElement.parentElement.hidden = true; });
});
[...__("dashboard-item")].forEach(e => {
	e.addEventListener("click", ev => {
		_("dashboard-pop").hidden = false;
		_("dashboard-pop-title").textContent = e.title;
	});
});