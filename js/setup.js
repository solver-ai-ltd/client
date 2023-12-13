const fs = require('fs');
const path = require('path');

const data_file_folder_path = path.resolve(__dirname, '..', 'example_files');
const setupFolder = path.resolve(__dirname, '..', 'setup');
const setupFilePath = path.join(setupFolder, 'setup.txt');

// read file synchronously
const data = fs.readFileSync(setupFilePath, 'utf8');

// parse `token`, `datamanagerUrl`, and `computerUrl` from `setup.txt`
const lines = data.split('\n');
const datamanagerUrl = lines.find(line => line.startsWith('datamanagerUrl')).split('=')[1];
const computerUrl = lines.find(line => line.startsWith('computerUrl')).split('=')[1];
const token = lines.find(line => line.startsWith('token')).split('=')[1];

module.exports = {
  token,
  data_file_folder_path,
  datamanagerUrl,
  computerUrl
};
