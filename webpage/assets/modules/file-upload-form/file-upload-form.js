const module_html_url = new URL("/assets/modules/file-upload-form/file-upload-form.html", window.origin);
fetch(module_html_url).then(res => res.text()).then(data => {
    _("upload-files-form").innerHTML = data;
}).then(() => {
    _("fileForm").addEventListener("change", e => uploadFormEvent(e));
    _("upload-cover").addEventListener("click", e => {
        e.preventDefault();
        _("file-input").click();
    });
});

function uploadFormEvent(e) {
    let total_size = 0;
    const upFileInfo = document.createDocumentFragment();
    const table_file_head = upFileInfo.appendChild(document.createElement('tr'));
    table_file_head.appendChild(document.createElement('th'));
    table_file_head.appendChild(document.createElement('th')).textContent = 'Name';
    table_file_head.appendChild(document.createElement('th')).textContent = 'Size (bytes)';
    let files_arr = Array.from(e.target.files);
    files_arr.forEach(file => {
        const table_file_row = upFileInfo.appendChild(document.createElement('tr'));
        table_file_row.addEventListener("click", () => {
            console.log(file);
            files_arr.splice(files_arr.indexOf(file), 1);
            uploadFormEvent(e);
        });
        const tableFileRowIcon = table_file_row.appendChild(document.createElement('td'));
        tableFileRowIcon.innerHTML = get_ico(file.name);
        tableFileRowIcon.classList.add('fluent-font');
        tableFileRowIcon.classList.add('fIcon');
        table_file_row.appendChild(document.createElement('td')).textContent = file.name;
        table_file_row.appendChild(document.createElement('td')).textContent = file.size;
        total_size += file.size;
    });
    upFileInfo.appendChild(document.createElement('td'));
    upFileInfo.appendChild(document.createElement('th')).textContent = e.target.files.length;
    upFileInfo.appendChild(document.createElement('th')).textContent = total_size;
    _('uploadTabFiles').replaceChildren(upFileInfo);
}

function uploadFile() {
    var xhr = new XMLHttpRequest();
    xhr.upload.addEventListener("progress", progressHandler, false);
    xhr.addEventListener("load", completeHandler, false);
    xhr.addEventListener("error", errorHandler, false);
    xhr.addEventListener("abort", abortHandler, false);
    const url = new URL('/file', window.location.origin);
    url.searchParams.append("disk", current_disk);
    url.searchParams.append('path', current_dir);
    xhr.open("POST", url);
    xhr.send(new FormData(_('fileForm')));
}

function progressHandler(e) {
    _('loaded_n_total').textContent = 'Uploaded ' + e.loaded + '/' + e.total + ' bytes';
    _('progressBar').hidden = false;
    _('progressBar').max = e.total;
    _('progressBar').value = e.loaded;
    _('status').textContent = Math.round((e.loaded / e.total) * 100) + '% uploaded... please wait';
    if (e.loaded >= e.total)
        _('status').textContent = 'Please wait, writing file to filesystem';
}
function completeHandler(event) {
    _("status").textContent = "Upload Complete";
    _("progressBar").value = 0;
    _("status").textContent = "File Uploaded";
    dir_dom(current_dir);
}
function errorHandler(event) {
    _("status").textContent = "Upload Failed";
}
function abortHandler(event) {
    _("status").textContent = "inUpload Aborted";
}