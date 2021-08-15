const RuleTextService = require("../build/Release/RuleTextService");
const http = require("https"); // or 'https' for https:// URLs
const fs = require("fs");
const { isUndefined } = require("lodash");
const util = require("util");
const ms = require("ms");

const Cache = require("./cache");

const ttl = 60 * 60 * 1;
const cache = new Cache(ttl);

var cdata;

function allData() {
  return cdata;
}

function rule(r) {
  return isUndefined(cdata) ? undefined : RuleTextService.rule(r);
}

function chapter(c) {
  return isUndefined(cdata)
    ? undefined
    : cache.get(c, () => {
        const res = RuleTextService.chapter(c);
        return res;
      });
}

function chapterFiltered(c, f) {
  console.log(f);
  console.log(RuleTextService.chapter_rules_containing(c, f));
  return isUndefined(cdata)
    ? undefined
    : RuleTextService.chapter_rules_containing(c, f);
}

function dataFiltered(f) {
  return isUndefined(cdata) ? undefined : RuleTextService.rules_containing(f);
}

var download = function (url, dest, cb) {
  fs.unlinkSync(dest);
  var file = fs.createWriteStream(dest);
  var request = http
    .get(url, function (response) {
      if (response.statusCode === 200) {
        response.pipe(file);
        file.on("finish", function () {
          file.close(cb);
        });
      }
      request.setTimeout(600, function () {
        request.destroy();
        if (cb) cb("Download took too long");
      });
    })
    .on("error", function (err) {
      fs.unlink(dest);
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
