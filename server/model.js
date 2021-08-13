const RuleTextService = require("../build/Release/RuleTextService");
const http = require("https"); // or 'https' for https:// URLs
const fs = require("fs");
const { isUndefined } = require("lodash");
const util = require("util");
const ms = require("ms");

var cdata;

function allData() {
  return cdata;
}

function rule(r) {
  return isUndefined(cdata) ? undefined : RuleTextService.rule(r);
}

function chapter(c) {
  return isUndefined(cdata) ? undefined : RuleTextService.chapter(c);
}

function chapterFiltered(c, f) {
  return isUndefined(cdata)
    ? undefined
    : RuleTextService.chapter_rules_containing(c, f);
}

function dataFiltered(f) {
  return isUndefined(cdata) ? undefined : RuleTextService.rules_containing(f);
}

var download = function (url, dest, cb) {
  var file = fs.createWriteStream(dest);
  var request = http
    .get(url, function (response) {
      if (response.statusCode === 200) {
        response.pipe(file);
        file.on("finish", function () {
          file.close(cb); // close() is async, call cb after close completes.
        });
      }
      request.setTimeout(600, function () {
        // if after 600ms file not downlaoded, we abort a request
        request.destroy();
        if (cb) cb("Download took too long");
      });
    })
    .on("error", function (err) {
      // Handle errors
      fs.unlink(dest); // Delete the file async. (But we don't check the result)
      if (cb) cb(err.message);
    });
};

const downloadAsync = util.promisify(download);

async function readFile(url) {
  var downloaded = false;
  await downloadAsync(url, "text.txt")
    .then((err) => {
      if (err) console.log(err);
      else downloaded = true;
    })
    .catch((err) => console.log(err));
  try {
    if (downloaded) {
      cdata = RuleTextService.open("text.txt");
      return true;
    } else {
      cdata = undefined;
      return false;
    }
  } catch (error) {
    console.log(error);
    cdata = undefined;
    return false;
  }
}

module.exports = {
  allData,
  rule,
  chapter,
  chapterFiltered,
  dataFiltered,
  readFile,
};
