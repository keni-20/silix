const express = require('express');
const multer = require('multer');
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');

const app = express();

// Serve static files from frontend folder
app.use(express.static('frontend'));

// Set up multer for file upload to 'uploaded' folder
const storage = multer.diskStorage({
    destination: function (req, file, cb) {
        cb(null, 'uploaded/'); // Specify destination folder for uploaded files
    },
    filename: function (req, file, cb) {
        cb(null, file.originalname); // Keep original filename
    }
});
const upload = multer({ storage: storage });

// Example route definition
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'frontend/index.html'));
});

// Serve the login.html page
app.get('/login.html', (req, res) => {
    res.sendFile(path.join(__dirname, 'frontend/login.html'));
});

// Serve other HTML files
app.get('/:page', (req, res) => {
    res.sendFile(path.join(__dirname, `frontend/${req.params.page}`));
});

// Function to process files
function processFile(preprocessOption, executablePath, inputFilePath, outputFilePath, res) {
    exec(`"${preprocessOption}" "${inputFilePath}" "${outputFilePath}"`, (error, stdout, stderr) => {
        if (error) {
            console.error('Preprocessing error:', error);
            return res.status(500).send('Error during preprocessing.');
        }

        exec(`"${executablePath}" "${outputFilePath}"`, (error, stdout, stderr) => {
            if (error) {
                console.error('Processing error:', error);
                return res.status(500).send('Error during processing.');
            }

            let resultFilePath = path.join(__dirname, 'uploaded', 'output', 'results.txt');
            fs.readFile(resultFilePath, 'utf8', (err, data) => {
                if (err) {
                    console.error('Error reading output file:', err);
                    return res.status(500).send('Error reading output file.');
                }

                res.setHeader('Content-disposition', 'attachment; filename=output.txt');
                res.setHeader('Content-type', 'text/plain');
                res.send(data);
            });
        });
    });
}

app.post('/process', upload.single('verilogFile'), (req, res) => {
    console.log('Received request for /process');
    console.log('File:', req.file);
    console.log('Preprocess option:', req.body.preprocess);

    if (!req.file) {
        console.error('No file uploaded.');
        return res.status(400).send('No file uploaded.');
    }

    let preprocessOption = path.join(__dirname, 'logic', 'preprocessing', req.body.preprocess);
    let inputFilePath = path.join(__dirname, req.file.path);
    let outputFilePath = path.join(__dirname, 'uploaded', 'output', 'output.v');

    // Ensure output directory exists
    if (!fs.existsSync(path.dirname(outputFilePath))) {
        fs.mkdirSync(path.dirname(outputFilePath), { recursive: true });
    }

    processFile(preprocessOption, path.join(__dirname, 'logic', 'CCO_CO'), inputFilePath, outputFilePath, res);
});

app.post('/process2', upload.single('verilogFile'), (req, res) => {
    console.log('Received request for /process2');
    console.log('File:', req.file);
    console.log('Preprocess option:', req.body.preprocess);

    if (!req.file) {
        console.error('No file uploaded.');
        return res.status(400).send('No file uploaded.');
    }

    let preprocessOption = path.join(__dirname, 'logic', 'preprocessing', req.body.preprocess);
    let inputFilePath = path.join(__dirname, req.file.path);
    let outputFilePath = path.join(__dirname, 'uploaded', 'output', 'output.v');

    // Ensure output directory exists
    if (!fs.existsSync(path.dirname(outputFilePath))) {
        fs.mkdirSync(path.dirname(outputFilePath), { recursive: true });
    }

    processFile(preprocessOption, path.join(__dirname, 'logic', 'FLOW_IPOP'), inputFilePath, outputFilePath, res);
});

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
    console.log(`Server is running on http://localhost:${PORT}`);
});
