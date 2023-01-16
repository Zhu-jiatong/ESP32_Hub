const module_html_url = new URL("/assets/modules/file-upload-form/file-upload-form.html", window.origin);
fetch(module_html_url).then(res => res.text()).then(data => {
    _("upload-files-form").innerHTML = data;
}).then(() => {
    _("fileForm").addEventListener("change", e => { files_queue = [...e.target.files]; uploadFormEvent(e); });
    _("upload-cover").addEventListener("click", e => {
        e.preventDefault();
        _("file-input").click();
    });
    _("fileForm").addEventListener('submit', e => {
        e.preventDefault();
        uploadFile();
    });
});

var files_queue = [];
var queue_indx = 0;

function uploadFormEvent(e) {
    let total_size = 0;
    const upFileInfo = document.createDocumentFragment();
    files_queue.forEach(file => {
        const table_file_row = upFileInfo.appendChild(document.createElement('tr'));
        table_file_row.addEventListener("click", () => {
            console.log(file);
            files_queue.splice(files_queue.indexOf(file), 1);
            uploadFormEvent(e);
        });
        const tableFileRowIcon = table_file_row.appendChild(document.createElement('td'));
        tableFileRowIcon.innerHTML = get_ico(file.name);
        tableFileRowIcon.classList.add('fluent-font');
        tableFileRowIcon.classList.add('fIcon');
        table_file_row.appendChild(document.createElement('td')).textContent = file.name;
        table_file_row.appendChild(document.createElement('td')).textContent = file.size;
        const progress_cell = table_file_row.appendChild(document.createElement("td"));
        progress_cell.classList.add("file-progress");
        progress_cell.textContent = "-";
        total_size += file.size;
    });
    upFileInfo.appendChild(document.createElement('td'));
    upFileInfo.appendChild(document.createElement('th')).textContent = files_queue.length;
    upFileInfo.appendChild(document.createElement('th')).textContent = total_size;
    _('uploadTabFiles').replaceChildren(upFileInfo);
}

function uploadFile() {
    queue_indx = 0;
    let i = 0;
    _("uploadTabFiles").querySelectorAll(".file-progress").forEach(e => {
        const ring = document.createElement("fluent-progress-ring");
        ring.classList.add(i);
        e.replaceChildren(ring);
        ++i;
    });
    upload_recursive();
}

function upload_recursive() {
    if (queue_indx >= files_queue.length)
        return;
    const elem = _("uploadTabFiles").getElementsByClassName(queue_indx)[0];
    const formDat = new FormData();
    formDat.set("file", files_queue[queue_indx]);
    var xhr = new XMLHttpRequest();
    xhr.upload.addEventListener("progress", e => progressHandler(e, elem), false);
    xhr.addEventListener("load", e => completeHandler(e, elem), false);
    xhr.addEventListener("error", () => errorHandler(elem), false);
    xhr.addEventListener("abort", () => abortHandler(elem), false);
    const url = new URL('/file', window.location.origin);
    url.searchParams.append("disk", current_disk);
    url.searchParams.append('path', current_dir);
    xhr.open("POST", url);
    xhr.send(formDat);
}

function progressHandler(e, elem) {
    elem.max = e.total;
    elem.value = e.loaded;
/*     _('loaded_n_total').textContent = 'Uploaded ' + adjust_size(e.loaded) + '/' + adjust_size(e.total);
    _('progressBar').hidden = false;
    _('progressBar').max = e.total;
    _('progressBar').value = e.loaded;
    _('status').textContent = Math.round((e.loaded / e.total) * 100) + '% uploaded... please wait';
    if (e.loaded >= e.total)
        _('status').textContent = 'Please wait, writing file to filesystem';
 */}
function completeHandler(e, elem) {
    const done_ico = document.createElement("span");
    done_ico.innerHTML = "&#xe930;";
    done_ico.classList.add("fluent-font");
    elem.replaceWith(done_ico);
    ++queue_indx;
    upload_recursive();
    dir_dom(current_dir);
}
function errorHandler(elem) {
    elem.classList.add("fluent-font");
    const err_ico = document.createElement("span");
    err_ico.innerHTML = "&#xea39;";
    err_ico.classList.add("fluent-font");
    elem.replaceWith(err_ico);
    ++queue_indx;
    upload_recursive();
}

function abortHandler(elem) {
    const err_ico = document.createElement("span");
    err_ico.innerHTML = "&#xea39;";
    err_ico.classList.add("fluent-font");
    elem.replaceWith(err_ico);
    ++queue_indx;
}