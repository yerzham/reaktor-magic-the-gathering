const model = require("./model");

var readUrl =
  "https://media.wizards.com/2021/downloads/MagicCompRules%2020210419.txt";

var fileRead = model.readFile(readUrl);

fetchContents = async (request, reply) => {
  try {
    if (fileRead) {
      var allData = model.allData();
      allData.forEach((section) => {
        section.children.forEach((chapter) => {
          chapter.children = [];
        });
      });
      return allData;
    }
  } catch (error) {
    console.log(error);
    return [];
  }
};

fetchChapter = async (request, reply) => {
  try {
    if (fileRead) {
      var chapterData;
      if (request.query.filter != undefined) {
        chapterData = model.chapterFiltered(
          request.params.chapter,
          request.query.filter
        );
      } else {
        chapterData = model.chapter(request.params.chapter);
      }
      if (chapterData != undefined) return chapterData;
      else reply.code(404).type("text/html").send("Not Found");
    }
  } catch (error) {
    console.log(error);
    return [];
  }
};

fetchRule = async (request, reply) => {
  try {
    if (fileRead) {
      var ruleData;
      ruleData = model.rule(request.params.rule);
      if (ruleData != undefined) return ruleData;
      else reply.code(404).type("text/html").send("Not Found");
    }
  } catch (error) {
    console.log(error);
    return [];
  }
};

module.exports = { fetchContents, fetchChapter, fetchRule };
