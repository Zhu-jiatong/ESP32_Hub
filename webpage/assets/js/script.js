function _(id) {
	return document.getElementById(id);
}

function __(class_name) {
	return document.getElementsByClassName(class_name);
}

_('rand').onload = function () {
	_('files-grid').rowsData = [
		{ Name: "file1wrtyuigfgugvgyfgyutyuytrrtyuioiuytrsrtyuioiuuuuuuuuuuuuuuuuuuuuuuuuuuuuuujkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk", Size: "10KB" },
		{ Name: "file2", Size: "123MB" },
		{ Name: '&#xwd;', Size: "43KB" },
	];
}

function select_files_btn_toggle(ev) {
	ev.target.appearance = "accent";
	_('download-files-btn').hidden = _('delete-files-btn').hidden = false;
	_("upload-files-btn").hidden = true;
	_('files-grid').querySelectorAll("fluent-data-grid-cell[grid-column='1']").forEach(e => {
		_('files-grid').rowsData.push({ Name: "catfile", Size: "catsize" })
		const checkbox = document.createElement("fluent-checkbox");
		checkbox.textContent = e.textContent;
		e.replaceChildren(checkbox);
	});
	ev.target.onclick = evt => {
		evt.target.appearance = "neutral";
		_('download-files-btn').hidden = _('delete-files-btn').hidden = true;
		_("upload-files-btn").hidden = false;
		_('files-grid').querySelectorAll("fluent-data-grid-cell[grid-column='1']").forEach(e => {
			e.replaceChildren(document.createTextNode(e.textContent));
			evt.target.onclick = select_files_btn_toggle;
		});
	};
}

document.querySelectorAll("fluent-tab").forEach(e => {
	console.log();
	e.addEventListener("dblclick", ev => {
		document.querySelectorAll("#" + e.parentElement.id + " > fluent-tab > .tab-txt").forEach(e => {
			e.hidden = !e.hidden;
		});
		console.log("db");
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