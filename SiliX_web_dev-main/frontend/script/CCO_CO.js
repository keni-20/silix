document.getElementById('uploadForm').addEventListener('submit', function(event) {
    event.preventDefault();

    let formData = new FormData();
    let preprocessOption = document.querySelector('input[name="preprocess"]:checked').value;
    let fileInput = document.getElementById('verilogFile');

    if (fileInput.files.length === 0) {
        alert('Please select a file.');
        return;
    }

    formData.append('preprocess', preprocessOption);
    formData.append('verilogFile', fileInput.files[0]);

    fetch('/process', {
        method: 'POST',
        body: formData
    })
    .then(response => {
        if (!response.ok) {
            throw new Error(`Server returned ${response.status} (${response.statusText})`);
        }
        return response.blob();
    })
    .then(blob => {
        const url = window.URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.style.display = 'none';
        a.href = url;
        a.download = 'output.txt';
        document.body.appendChild(a);
        a.click();
        window.URL.revokeObjectURL(url);
    })
    .catch(error => {
        console.error('Error:', error);
    });
});